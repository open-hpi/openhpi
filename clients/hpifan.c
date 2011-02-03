/*      -*- linux-c -*-
 * hpifan.cpp
 *
 * Copyright (c) 2003,2004 by FORCE Computers
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
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
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *     10/13/2004  kouzmich   porting to HPI B
 *     09/06/2010  ulikleber  New option -D to select domain
 *     01/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

static SaHpiBoolT set_new = SAHPI_FALSE;
static SaHpiCtrlModeT new_mode = SAHPI_CTRL_MODE_MANUAL;
static int new_speed = -1;
static gchar *f_speed = NULL;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "speed", 's', 0, G_OPTION_ARG_STRING, &f_speed, "Set fan speed for ALL fans in domain\n"
    "                      speed is a number or \"auto\" for setting fan in auto mode", "auto|nn" },
  { NULL }
};

static SaErrorT
get_fan_speed( SaHpiSessionIdT session_id,
               SaHpiResourceIdT resource_id,
               SaHpiCtrlNumT ctrl_num,
               SaHpiCtrlStateAnalogT *speed,
	       SaHpiCtrlModeT *mode )
{
        SaHpiCtrlStateT state;

	if (copt.debug) printf("get fan speed for resource %u, control %u\n", resource_id, ctrl_num);

        SaErrorT rv = saHpiControlGet( session_id, resource_id, ctrl_num, mode, &state );

        if ( rv != SA_OK ) {
                fprintf( stderr, "cannot get fan state: %s!\n", oh_lookup_error( rv ) );
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
               SaHpiCtrlStateAnalogT speed,
	       SaHpiCtrlModeT mode )
{
        SaErrorT rv;
        SaHpiCtrlStateT state;
        state.Type = SAHPI_CTRL_TYPE_ANALOG;
        state.StateUnion.Analog = speed;

	if (copt.debug) printf("set fan speed for resource %u, control %u\n", resource_id, ctrl_num);
        rv = saHpiControlSet( session_id, resource_id, ctrl_num, mode, &state );

        if ( rv != SA_OK ) {
                fprintf( stderr, "cannot set fan state: %s!\n", oh_lookup_error( rv ) );
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
	SaHpiCtrlModeT ctrl_mode;

        printf( "\tfan: num %u, id ", ctrl_rec->Num );
        oh_print_text ( &rdr->IdString );
        printf( "\n" );

        if ( ctrl_rec->Type != SAHPI_CTRL_TYPE_ANALOG ) {
                fprintf( stderr, "cannot handle non analog fan controls !\n" );
                return 0;
        }

        printf( "\t\tmin       %d\n", ctrl_rec->TypeUnion.Analog.Min );
        printf( "\t\tmax       %d\n", ctrl_rec->TypeUnion.Analog.Max );
        printf( "\t\tdefault   %d\n", ctrl_rec->TypeUnion.Analog.Default );

        SaHpiCtrlStateAnalogT speed;

        rv = get_fan_speed( session_id, resource_id, ctrl_rec->Num, &speed, &ctrl_mode);

        if ( rv != SA_OK )
                return 0;

        printf( "\t\tmode      %s\n", ctrl_mode == SAHPI_CTRL_MODE_AUTO ? "auto" : "manual" );
        printf( "\t\tcurrent   %d\n", speed );

        if ( set_new == SAHPI_FALSE ) {
                return 0;
        }
        if ( new_mode == SAHPI_CTRL_MODE_AUTO ) {
                new_speed = speed;
        }
  
        if (   new_speed < ctrl_rec->TypeUnion.Analog.Min 
                || new_speed > ctrl_rec->TypeUnion.Analog.Max ) {
                fprintf( stderr, "fan speed %d out of range [%d,%d] !\n",
                        new_speed, ctrl_rec->TypeUnion.Analog.Min,
                         ctrl_rec->TypeUnion.Analog.Max );
                return 0;
        }

        rv = set_fan_speed( session_id, resource_id, ctrl_rec->Num, new_speed, new_mode);

        if ( rv != SA_OK )
                return 0;
  
        rv = get_fan_speed( session_id, resource_id, ctrl_rec->Num, &speed, &ctrl_mode);

        if ( rv != SA_OK )
                return 0;
  
        printf( "\t\tnew mode  %s\n", ctrl_mode == SAHPI_CTRL_MODE_AUTO ? "auto" : "manual" );
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
                        printf( "saHpiRptEntryGet: %s\n", oh_lookup_error( rv ) );
                        return 1;
                }

                // check for control rdr
                if (    !(entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                        || !(entry.ResourceCapabilities & SAHPI_CAPABILITY_CONTROL) )
                        continue;

                /* walk the RDR list for this RPT entry */
                SaHpiEntryIdT next_rdr = SAHPI_FIRST_ENTRY;
                SaHpiResourceIdT resource_id = entry.ResourceId;
                int epath_out = 1;

                do {
                        SaHpiEntryIdT current_rdr = next_rdr;
                        SaHpiRdrT     rdr;

                        rv = saHpiRdrGet( session_id, resource_id, current_rdr, 
                                          &next_rdr, &rdr );
           
                        if ( rv != SA_OK ) {
                                printf( "saHpiRdrGet: %s\n", oh_lookup_error( rv ) );
                                return 1;
                        }

                        // check for control
                        if ( rdr.RdrType != SAHPI_CTRL_RDR )
                                continue;

                        // check for fan
                        if ( rdr.RdrTypeUnion.CtrlRec.OutputType != SAHPI_CTRL_FAN_SPEED )
                                continue;

                        if ( epath_out ) {
				oh_print_ep(&entry.ResourceEntity, 0);
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
        SaErrorT rv;
        SaHpiSessionIdT sessionid;
        GError *error = NULL;
        GOptionContext *context;

        oh_prog_version(argv[0], OH_SVN_REV);

        context = g_option_context_new ("- Show \"Fan Control\" management instruments");
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127
                    - OHC_VERBOSE_OPTION,    // no verbose mode implemented
                error)) { 
                g_print ("option parsing failed: %s\n", error->message);
                g_option_context_free (context);
		exit(1);
	}
        g_option_context_free (context);
 
        if (f_speed) {
           set_new = SAHPI_TRUE;
           if ( strcmp( f_speed, "auto" ) == 0 ) {
              new_mode = SAHPI_CTRL_MODE_AUTO;
           } else if ( strcmp( f_speed, "0" ) == 0 ) {
              new_speed = 0;
           } else {
              new_speed = atoi( f_speed );
              if (new_speed == 0) {
                 g_print ("please enter a valid speed: \"auto\" or a number.\n");
                 g_free (f_speed);
                 exit(1);
              }
           }
           g_free(f_speed);
        }

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) exit(-1);

	/*
	 * Resource discovery
	 */
	if (copt.debug) printf("saHpiDiscover\n");
	rv = saHpiDiscover(sessionid);
        if ( rv != SA_OK ) {
                printf( "saHpiDiscover: %s\n", oh_lookup_error( rv ) );
                return 1;
        }
	if (copt.debug) printf("Discovery done\n");

        int rc = discover_domain( sessionid );

        rv = saHpiSessionClose( sessionid );
        if ( rv != SA_OK )
                printf( "saHpiSessionClose: %s\n", oh_lookup_error( rv ) );

        return  rc;
}
