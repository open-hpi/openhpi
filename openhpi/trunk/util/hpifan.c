/*      -*- linux-c -*-
 * hpifan.cpp
 *
 * Copyright (c) 2003,2004 by FORCE Computers
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "SaHpi.h"
#include <epath_utils.h>


static int fan_speed = -1;


#define NECODES  27
static struct 
{ 
        int code; char *str;
} ecodes[NECODES] =
{
        {  0,    "Success" },
        { -1001, "HPI unspecified error" },
        { -1002, "HPI unsupported function" },
        { -1003, "HPI busy" },
        { -1004, "HPI request invalid" },
        { -1005, "HPI command invalid" },
        { -1006, "HPI timeout" },
        { -1007, "HPI out of space" },
        { -1008, "HPI data truncated" },
        { -1009, "HPI data length invalid" },
        { -1010, "HPI data exceeds limits" },
        { -1011, "HPI invalid params" },
        { -1012, "HPI invalid data" },
        { -1013, "HPI not present" },
        { -1014, "HPI invalid data field" },
        { -1015, "HPI invalid sensor command" },
        { -1016, "HPI no response" },
        { -1017, "HPI duplicate request" },
        { -1018, "HPI updating" },
        { -1019, "HPI initializing" },
        { -1020, "HPI unknown error" },
        { -1021, "HPI invalid session" },
        { -1022, "HPI invalid domain" },
        { -1023, "HPI invalid resource id" },
        { -1024, "HPI invalid request" },
        { -1025, "HPI entity not present" },
        { -1026, "HPI uninitialized" }
};

static char def_estr[15] = "HPI error %d   ";


static char *
decode_error( SaErrorT code )
{
        int i;

        for( i = 0; i < NECODES; i++ )
                if ( code == ecodes[i].code )
                        return ecodes[i].str;

        sprintf( def_estr + 10, "%d", code );

        return def_estr;
}


static int
usage( void )
{
        fprintf( stderr, "usage: hpifan [-h] [-s fan_speed_level]\n" );
        fprintf( stderr, "\t\t -h        help\n" );
        fprintf( stderr, "\t\t -s speed  set fan speed for ALL fans\n" );

        return 1;
}


static void
display_textbuffer( SaHpiTextBufferT string )
{
        int i;
        switch( string.DataType ) {
        case SAHPI_TL_TYPE_BINARY:
                for( i = 0; i < string.DataLength; i++ )
                        printf( "%x", string.Data[i] );
                break;

        case SAHPI_TL_TYPE_BCDPLUS:
                for( i = 0; i < string.DataLength; i++ )
                        printf("%c", string.Data[i] );
                break;

        case SAHPI_TL_TYPE_ASCII6:
                for( i = 0; i < string.DataLength; i++ )
                        printf( "%c", string.Data[i] );
                break;

        case SAHPI_TL_TYPE_LANGUAGE:
                for( i = 0; i < string.DataLength; i++ )
                        printf( "%c", string.Data[i] );
                break;

        default:
                printf("Invalid string data type=%d", string.DataType );
        }
}


static SaErrorT
get_fan_speed( SaHpiSessionIdT session_id,
               SaHpiResourceIdT resource_id,
               SaHpiCtrlNumT ctrl_num,
               SaHpiCtrlStateAnalogT *speed )
{
        SaHpiCtrlStateT state;

        SaErrorT rv = saHpiControlStateGet( session_id, resource_id, ctrl_num, &state );

        if ( rv != SA_OK ) {
                fprintf( stderr, "cannot get fan state: %s!\n", decode_error( rv ) );
                return rv;
        }

        if ( state.Type != SAHPI_CTRL_TYPE_ANALOG ) {
                fprintf( stderr, "cannot handle non analog fan state !\n" );
                return SA_ERR_HPI_ERROR;
        }

        *speed = state.StateUnion.Analog;

        return SA_OK;
}


static SaErrorT
set_fan_speed( SaHpiSessionIdT session_id,
               SaHpiResourceIdT resource_id,
               SaHpiCtrlNumT ctrl_num,
               SaHpiCtrlStateAnalogT speed )
{
        SaErrorT rv;
        SaHpiCtrlStateT state;
        state.Type = SAHPI_CTRL_TYPE_ANALOG;
        state.StateUnion.Analog = speed;

        rv = saHpiControlStateSet( session_id, resource_id, ctrl_num, &state );

        if ( rv != SA_OK ) {
                fprintf( stderr, "cannot set fan state: %s!\n", decode_error( rv ) );
                return rv;
        }

        return SA_OK;
}


static int
do_fan( SaHpiSessionIdT session_id, SaHpiResourceIdT resource_id,
        SaHpiRdrT *rdr )
{
        SaErrorT rv;
        SaHpiCtrlRecT *ctrl_rec = &rdr->RdrTypeUnion.CtrlRec;

        printf( "\tfan: num %d, id ", ctrl_rec->Num );
        display_textbuffer( rdr->IdString );
        printf( "\n" );

        if ( ctrl_rec->Type != SAHPI_CTRL_TYPE_ANALOG ) {
                fprintf( stderr, "cannot handle non analog fan controls !\n" );
                return 0;
        }

        printf( "\t\tmin       %d\n", ctrl_rec->TypeUnion.Analog.Min );
        printf( "\t\tmax       %d\n", ctrl_rec->TypeUnion.Analog.Max );
        printf( "\t\tdefault   %d\n", ctrl_rec->TypeUnion.Analog.Default );

        SaHpiCtrlStateAnalogT speed;

        rv = get_fan_speed( session_id, resource_id, ctrl_rec->Num, &speed );

        if ( rv != SA_OK )
                return 0;

        printf( "\t\tcurrent   %d\n", speed );

        if ( fan_speed == -1 )
                return 0;
  
        if (    fan_speed < ctrl_rec->TypeUnion.Analog.Min 
                || fan_speed > ctrl_rec->TypeUnion.Analog.Max ) {
                fprintf( stderr, "fan speed %d out of range [%d,%d] !\n",
                         fan_speed, ctrl_rec->TypeUnion.Analog.Min,
                         ctrl_rec->TypeUnion.Analog.Max );
                return 0;
        }

        speed = fan_speed;
        rv = set_fan_speed( session_id, resource_id, ctrl_rec->Num, speed );

        if ( rv != SA_OK )
                return 0;
  
        rv = get_fan_speed( session_id, resource_id, ctrl_rec->Num, &speed );

        if ( rv != SA_OK )
                return 0;
  
        printf( "\t\tnew speed %d\n", speed );

        return 0;
}


static int
discover_domain( SaHpiSessionIdT session_id )
{
        /* walk the RPT list */
        SaErrorT rv;
        SaHpiEntryIdT next = SAHPI_FIRST_ENTRY;
        int found = 0;

        do {
                SaHpiRptEntryT entry;
                SaHpiEntryIdT  current = next;

                rv = saHpiRptEntryGet( session_id, current, &next, &entry );

                if ( rv != SA_OK ) {
                        printf( "saHpiRptEntryGet: %s\n", decode_error( rv ) );
                        return 1;
                }

                // check for control rdr
                if (    !(entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                        || !(entry.ResourceCapabilities & SAHPI_CAPABILITY_CONTROL) )
                        continue;

                /* walk the RDR list for this RPT entry */
                SaHpiEntryIdT next_rdr;
                SaHpiResourceIdT resource_id = entry.ResourceId;
                int epath_out = 1;

                do {
                        SaHpiEntryIdT current_rdr = next_rdr;
                        SaHpiRdrT     rdr;

                        rv = saHpiRdrGet( session_id, resource_id, current_rdr, 
                                          &next_rdr, &rdr );
           
                        if ( rv != SA_OK ) {
                                printf( "saHpiRdrGet: %s\n", decode_error( rv ) );
                                return 1;
                        }

                        // check for control
                        if ( rdr.RdrType != SAHPI_CTRL_RDR )
                                continue;

                        // check for fan
                        if ( rdr.RdrTypeUnion.CtrlRec.OutputType != SAHPI_CTRL_FAN_SPEED )
                                continue;

                        if ( epath_out ) {
                                char tmp_epath[128];
                                entitypath2string( &entry.ResourceEntity, tmp_epath,
                                                   sizeof( tmp_epath ) );

                                printf("resource: %s\n", tmp_epath );
                                epath_out = 0;
                        }

                        do_fan( session_id, resource_id, &rdr );
                        found++;
                } while( next_rdr != SAHPI_LAST_ENTRY );
        } while( next != SAHPI_LAST_ENTRY );

        if ( found == 0 )
                printf( "no fans found.\n" );

        return 0;
}


int
main( int argc, char *argv[] )
{
        int c;
        int help = 0;
        SaErrorT rv;

        while( (c = getopt( argc, argv,"hs:") ) != -1 )
                switch( c ) {
                case 'h': 
                        help = 1;
                        break;

                case 's':
                        fan_speed = atoi( optarg );
                        break;
                 
                default:
                        fprintf( stderr, "unknown option %s !\n",
                                 argv[optind] );
                        help = 1;
                }
  
        if ( help )
                return usage();

        SaHpiVersionT hpiVer;
        rv = saHpiInitialize( &hpiVer );

        if ( rv != SA_OK ) {
                fprintf( stderr, "saHpiInitialize: %s\n",decode_error( rv ) );
                return 1;
        }

        SaHpiSessionIdT sessionid;
        rv = saHpiSessionOpen( SAHPI_DEFAULT_DOMAIN_ID, &sessionid, 0 );

        if ( rv != SA_OK ) {
                printf( "saHpiSessionOpen: %s\n", decode_error( rv ) );
                return 1;
        }

        rv = saHpiResourcesDiscover( sessionid );

        if ( rv != SA_OK ) {
                printf( "saHpiResourcesDiscover: %s\n", decode_error( rv ) );
                return 1;
        }

        int rc = discover_domain( sessionid );

        rv = saHpiSessionClose( sessionid );

        if ( rv != SA_OK )
                printf( "saHpiSessionClose: %s\n", decode_error( rv ) );

        rv = saHpiFinalize();

        if ( rv != SA_OK )
                printf( "saHpiFinalize: %s\n", decode_error( rv ) );

        return  rc;
}
