/*
 * configuration file handling code
 *
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

#include "simple_config.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


static char *
Whitespace( char *p )
{
  while( *p == ' ' || *p == '\t' )
       p++;

  return p;
}


static cConfigEntry *
FindEntry( cConfigEntry *e, const char *name )
{
  for( ; e->m_type != eConfigTypeUnknown; e++ )
       if ( !strcmp( e->m_name, name ) )
	    return e;

  return 0;
}


int
ConfigRead( const char *filename, cConfigEntry *entries, void *data )
{
  unsigned char *dp = (unsigned char *)data;

  // initialize data
  cConfigEntry *e;

  for( e = entries; e->m_type != eConfigTypeUnknown; e++ )
       switch( e->m_type )
	  {
	    case eConfigTypeBool:
		 *(int *)(void *)(dp + e->m_offset) = e->m_default.m_bool;
		 break;
 
	    case eConfigTypeInt:
		 *(int *)(void *)(dp + e->m_offset) = e->m_default.m_int;
		 break;

	    case eConfigTypeString:
		 if ( e->m_default.m_string )
		      strcpy( (char *)dp + e->m_offset,
			      e->m_default.m_string );
		 else
		      *(char *)(dp + e->m_offset) = 0;

		 break;

	    default:
		 assert( 0 );
		 break;
     }

  FILE *fd = fopen( filename, "r" );

  if ( fd == 0 )
       return 1;

  int line = 0;
  int rv = 0;

  while( 1 )
     {
       line++;

       char buf[dConfigStringMaxLength];

       if ( !fgets( buf, dConfigStringMaxLength, fd ) )
	    break;

       int l = strlen( buf );
       
       if ( l && buf[l-1] == '\n' )
	    buf[l-1] = 0;

       char *p = Whitespace( buf );

       // comment or empty line
       if ( *p == 0 || *p == '#' )
	    continue;

       char name[dConfigStringMaxLength];
       char *n = name;

       while( *p && *p != ' ' && *p != '\t' && *p != '=' )
	    *n++ = *p++;

       *n = 0;

       if ( name[0] == 0 )
	  {
	    fprintf( stderr, "%s.%d: missing entriy !\n",
		     filename, line );

	    rv = 1;

	    continue;
	  }

       // find entry
       cConfigEntry *e = FindEntry( entries, name );

       if ( !e )
	  {
	    fprintf( stderr, "%s.%d: unknown entry '%s' !\n",
		     filename, line, name );

	    rv = 1;
	    
	    continue;
	  }

       p = Whitespace( p );

       if ( *p != '=' )
	  {
	    fprintf( stderr, "%s.%d: missing '=' !\n",
		     filename, line );

	    rv = 1;

	    continue;
	  }

       p++;
       p = Whitespace( p );

       if ( *p )
	  {
	    // remove whitespace at the end of the line
	    char *q = p + strlen( p );
	    q--;

	    while( q > p && (*q == ' ' || *q == '\t' ) )
		 q--;

	    q++;
	    *q = 0;
	  }

       switch( e->m_type )
	  {
	    case eConfigTypeBool:
		 if ( !strcasecmp( p, "false" ) )
		      *(int *)(void *)(dp + e->m_offset) = 0;
		 else if ( !strcasecmp( p, "true" ) )
		      *(int *)(void *)(dp + e->m_offset) = 1;
		 else
		    {
		      fprintf( stderr, "%s.%d: expecting 'true' or 'false' !\n",
			       filename, line );

		      rv = 1;
		    }
		 
		 break;
 
	    case eConfigTypeInt:
		 {
		   char *q;
		   *(int *)(void *)(dp + e->m_offset) = strtol( p, &q, 0 );

		   q = Whitespace( q );

		   if ( *q )
		      {
			fprintf( stderr, "%s.%d: expecting integer value !\n",
			       filename, line );

			rv = 1;
		      }
		 }

		 break;

	    case eConfigTypeString:
		 
		 strcpy( (char *)dp + e->m_offset, p );
		 break;

	    default:
		 assert( 0 );
		 break;
	  }
     }

  fclose( fd );

  return rv;
}
