/*      -*- c++ -*-
 *
 * Copyright (c) 2010 by Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <oh_error.h>

#include "hpi.h"
#include "hpi_xml_writer.h"
#include "schema.h"


/***************************************************
 * Main
 ***************************************************/
int main( int argc, char * argv[] )
{
    int  indent_step = 0;
    bool use_names = false;

    SaHpiDomainIdT did = SAHPI_UNSPECIFIED_DOMAIN_ID;

    int c;
    bool print_usage = false;
    bool print_xsd = false;
    while ( ( c = getopt( argc, argv,"D:its") ) != EOF ) {
        switch( c ) {
            case 'D':
                if ( optarg ) {
                    did = atoi( optarg );
                } else {
                    print_usage = true;
                }
                break;
            case 'i':
                indent_step = 1;
                break;
            case 't':
                use_names = true;
                break;
            case 's':
                print_xsd = true;
                break;
            default:
                print_usage = true;
                break;
        }
    }
    if ( optind < argc ) {
        print_usage = true;
    }
    if ( print_usage ) {
        printf( "Usage: %s [-option]\n\n", argv[0] );
        printf( "    (No Option) Work with default domain\n" );
        printf( "    -D nn  Select domain id nn\n" );
        printf( "    -i     Use indentation\n" );
        printf( "    -t     Use enum and flag text names instead of raw values\n" );
        printf( "    -s     Show XML schema\n" );
        printf( "\n" );
        return 1;
    }
    if ( print_xsd ) {
        for ( char * p = &schema_begin; p < &schema_end; ++p ) {
            fwrite( p, 1, 1, stdout );
        }
        return 0;
    }

    bool rc;

    cHpi hpi( did );
    rc = hpi.Open();
    if ( !rc ) {
        return 1;
    }
    cHpiXmlWriter writer( indent_step, use_names );
    rc = hpi.Dump( writer );
    if ( !rc ) {
        CRIT( "Failed to produce XML" );
        return 1;
    }

    return 0;
}

