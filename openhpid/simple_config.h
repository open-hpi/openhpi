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

#ifndef dMarshal_Config_h
#define dMarshal_Config_h


#ifdef __cplusplus
extern "C" {
#endif

#define dConfigStringMaxLength 256


typedef enum
{
  eConfigTypeUnknown,
  eConfigTypeBool,
  eConfigTypeInt,
  eConfigTypeString,
} tConfigType;


#define dConfigOffset(type,field) (((char *)(&(((type *)0)->field)))-((char*)0))


typedef struct
{
  const char *m_name;
  tConfigType m_type;
  int         m_offset;

  union
  {
    int m_bool;
    int m_int;
    const char *m_string;
  } m_default;
} cConfigEntry;


int ConfigRead( const char *filename, cConfigEntry *entries, void *data );


#ifdef __cplusplus
}
#endif


#endif
