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

#include "simple_config.h"
#include <string.h>


typedef struct
{
  int m_bool;
  int m_int;
  char m_string[dConfigStringMaxLength];
} cTest;


int
main( int argc, char *argv[] )
{
  cConfigEntry table[] =
  {
    {
      .m_name = "bool",
      .m_type = eConfigTypeBool,
      .m_offset = dConfigOffset( cTest, m_bool ),
      .m_default.m_bool = 1
    },
    {
      .m_name = "int",
      .m_type = eConfigTypeInt,
      .m_offset = dConfigOffset( cTest, m_int ),
      .m_default.m_int = 13
    },
    {
      .m_name = "string",
      .m_type = eConfigTypeString,
      .m_offset = dConfigOffset( cTest, m_string ),
      .m_default.m_string = "default string"
    },
    {
      .m_type = eConfigTypeUnknown
    }
  };

  cTest t;
  
  int rv = ConfigRead( "config_000.conf", table, &t );

  if ( rv )
       return 1;

  if ( t.m_bool != 0 )
       return 1;

  if ( t.m_int != 14 )
       return 1;

  if ( strcmp( t.m_string, "test string" ) )
       return 1;

  return 0;
}
