/*
 * Copyright (c) 2004 by FORCE Computers.
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

#include "connection.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>


int
main( int argc, char *argv[] )
{
  cConnectionSeq cs;
  ConnectionSeqInit( &cs );

  cConnectionSeq cs1;
  ConnectionSeqInit( &cs1 );

  int i;

  for( i = 0; i < 1000; i++ )
     {
       unsigned char seq = ConnectionSeqGet( &cs1 );

       if ( ConnectionSeqCheck( &cs, seq ) )
            return 1;

       unsigned char seq1 = ConnectionSeqGet( &cs );

       if ( seq != seq1 )
            return 1;

       if ( ConnectionSeqCheck( &cs, seq ) == 0 )
            return 1;
     }

  ConnectionSeqInit( &cs );

  for( i = 0; i < 8; i++ )
       if ( ConnectionSeqCheck( &cs, 16 - i - 1) )
            return 1;

  for( i = 0; i < 8; i++ )
       if ( ConnectionSeqCheck( &cs, 16 + i ) )
            return 1;

  for( i = 0; i < 16; i++ )
       if ( ConnectionSeqCheck( &cs, i+8 ) == 0 )
            return 1;

  return 0;
}
