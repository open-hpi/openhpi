/*
 * event_get
 *
 * Copyright (c) 2004 by FORCE Computers
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
#include <sys/time.h>
#include <time.h>
#include <getopt.h>
#include <stdarg.h>
#include "SaHpi.h"
#include <ecode_utils.h>


static int
usage( void )
{
  fprintf( stderr, "usage: event_get [-h] [-t timeout in ms] [-n num_events]\n" );
  fprintf( stderr, "\t\t -h         help\n" );
  fprintf( stderr, "\t\t -t timeout timeout for saHpiEventGet in ms (-1 block)\n" );
  fprintf( stderr, "\t\t -n events  number of events to read (-1 infinite)\n" );

  return 1;
}


static void 
Log( const char *fmt, ... )
{  
  struct timeval tv;
  gettimeofday( &tv, 0 );

  char b[1024];

  struct tm tmt;
  time_t dummy = tv.tv_sec;

  localtime_r( &dummy, &tmt );

  // 11:11:11
  strftime( b, 10, "%H:%M:%S", &tmt );

  sprintf( b + 9 - 1, ".%03ld ", tv.tv_usec / 1000 );

  va_list ap;
  va_start( ap, fmt );
  vsnprintf( b+13, 1024, fmt, ap );  

  va_end( ap );

  printf( "%s\n", b );
}


int
main( int argc, char *argv[] )
{
  int c;
  int help = 0;
  SaErrorT rv;
  int to = -1;
  int num = -1;

  while( (c = getopt( argc, argv,"ht:n:") ) != -1 )
       switch( c )
	  {
	    case 'h': 
		 help = 1;
		 break;

	    case 't':
		 to = atoi( optarg );
		 break;

	    case 'n':
		 num = atoi( optarg );
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

  if ( rv != SA_OK )
     {
       fprintf( stderr, "saHpiInitialize: %s\n",decode_error( rv ) );
       return 1;
     }

  SaHpiSessionIdT sessionid;
  rv = saHpiSessionOpen( SAHPI_DEFAULT_DOMAIN_ID, &sessionid, 0 );

  if ( rv != SA_OK )
     {
       printf( "saHpiSessionOpen: %s\n", decode_error( rv ) );
       return 1;
     }

  Log( "saHpiSessionOpen: %d", sessionid );

  rv = saHpiResourcesDiscover( sessionid );

  if ( rv != SA_OK )
     {
       printf( "saHpiResourcesDiscover: %s\n", decode_error( rv ) );
       return 1;
     }

  rv = saHpiSubscribe( sessionid, SAHPI_TRUE );

  if ( rv != SA_OK )
     {
       printf( "saHpiSubscribe: %s\n", decode_error( rv ) );
       return 1;
     }

  if ( num < 0 )
       num = 1000000;

  if ( to < 0 )
       to = 1000000;

  SaHpiTimeT t = to;
  t *= 1000000;

  int i;

  for( i = 0; i < num; i++ )
     {
       SaHpiEventT    event;
       SaHpiRdrT      rdr;
       SaHpiRptEntryT rpt_entry;

       Log( "saHpiEventGet start" );

       rv = saHpiEventGet( sessionid, t, &event, &rdr, &rpt_entry );

       if ( rv == SA_ERR_HPI_TIMEOUT )
	    Log( "saHpiEventGet: timeout" );
       else if ( rv != SA_OK )
	    Log( "saHpiEventGet: %s", decode_error( rv ) );
       else
	    Log( "saHpiEventGet: read event" );
     }

  rv = saHpiUnsubscribe( sessionid );
       printf( "saHpiUnsubscribe: %s\n", decode_error( rv ) );

  rv = saHpiSessionClose( sessionid );

  if ( rv != SA_OK )
       printf( "saHpiSessionClose: %s\n", decode_error( rv ) );

  rv = saHpiFinalize();

  if ( rv != SA_OK )
       printf( "saHpiFinalize: %s\n", decode_error( rv ) );

  return 0;
}
