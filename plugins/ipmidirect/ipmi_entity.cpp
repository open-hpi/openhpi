/*
 * ipmi_entity.cpp
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

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <assert.h>

#include "ipmi_entity.h"
#include "ipmi_log.h"

extern "C" {
#include "epath_utils.h"
}


static const char *entity_id_types[] =
{
  "Unspecified",
  "Other",
  "Unkown",
  "Processor",
  "Disk",
  "Peripheral",
  "SystemManagementModule",
  "SystemBoard",
  "MemoryModule",
  "ProcessorModule",
  "PowerSupply",
  "AddInCard",
  "FrontPanelBoard",
  "BackPanelBoard",
  "PowerSystemBoard",
  "DriveBackplane",
  "SystemInternalExpansionBoard",
  "OtherSystemBoard",
  "ProcessorBoard",
  "PowerUnit",
  "PowerModule",
  "PowerManagementBoard",
  "ChassisBackPanelBoard",
  "SystemChassis",
  "SubChassis",
  "OtherChassisBoard",
  "DiskDriveBay",
  "PeripheralBay",
  "DeviceBay",
  "FanCooling",
  "CoolingUnit",
  "CableInterconnect",
  "MemoryDevice",
  "SystemManagementSoftware",
  "Bios",
  "OperatingSystem",
  "SystemBus",
  "Group",
  "RemoteMgmtCommDevice",
  "ExternalEnvironment",
  "Battery",
  "ProcessingBlade",
  "ConnectivitySwitch",
  "ProcessorMemoryModule",
  "IoModule",
  "ProcessorIoModule",
  "ManagementControllerFirmware",
};

#define dNumEntityIdTypes (sizeof(entity_id_types)/sizeof(char *))


const char *
IpmiEntityIdToString( tIpmiEntityId val )
{
  if ( (unsigned int)val < dNumEntityIdTypes )
       return entity_id_types[val];

  switch( val )
     {
       case eIpmiEntityIdPigMgFrontBoard:
            return "PigmigFrontBoard";

       case eIpmiEntityIdPigMgRearTransitionModule:
            return "PigmigRearTransitionModule";

       case eIpmiEntityIdAtcaShelfManager:
            return "AtcaShelfManager";

       case eIpmiEntityIdAtcaFiltrationUnit:
            return "AtcaFiltrationUnit";

       default:
            return "Invalid";
     }

  // not reached
  return "Invalid";
}


cIpmiEntityPath::cIpmiEntityPath()
{
  memset( &m_entity_path, 0, sizeof( SaHpiEntityPathT ) );
}


cIpmiEntityPath::cIpmiEntityPath( const SaHpiEntityPathT &entity_path )
{
  m_entity_path = entity_path;
}


void
cIpmiEntityPath::SetEntry( int idx, SaHpiEntityTypeT type,
                           SaHpiEntityInstanceT instance )
{
  assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

  m_entity_path.Entry[idx].EntityType     = type;
  m_entity_path.Entry[idx].EntityInstance = instance;
}


SaHpiEntityTypeT
cIpmiEntityPath::GetEntryType( int idx )
{
  assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

  return m_entity_path.Entry[idx].EntityType;
}


void
cIpmiEntityPath::SetEntryType( int idx, SaHpiEntityTypeT type )
{
  assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

  m_entity_path.Entry[idx].EntityType = type;
}


SaHpiEntityInstanceT 
cIpmiEntityPath::GetEntryInstance( int idx )
{
  assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

  return m_entity_path.Entry[idx].EntityInstance;
}


void
cIpmiEntityPath::SetEntryInstance( int idx,
                                   SaHpiEntityInstanceT instance )
{
  assert( idx >= 0 && idx < SAHPI_MAX_ENTITY_PATH );

  m_entity_path.Entry[idx].EntityInstance = instance;
}


cIpmiEntityPath &
cIpmiEntityPath::operator+=( const cIpmiEntityPath &epath )
{
  ep_concat( &m_entity_path, &epath.m_entity_path );

  return *this;
}


bool
cIpmiEntityPath::operator==( const cIpmiEntityPath &epath ) const
{
  return ep_cmp( &m_entity_path, &epath.m_entity_path ) == 0;
}


bool
cIpmiEntityPath::operator!=( const cIpmiEntityPath &epath ) const
{
  return ep_cmp( &m_entity_path, &epath.m_entity_path ) != 0;
}


bool
cIpmiEntityPath::AppendRoot()
{
  int rv = append_root( &m_entity_path );

  return rv ? false : true;
}


cIpmiLog &
operator<<( cIpmiLog &log, const cIpmiEntityPath &epath )
{
  char str[1024] = "{Invalid}";

  entitypath2string( &epath.m_entity_path, str, 1024 );

  log << str;

  return log;
}


bool
cIpmiEntityPath::FromString( const char *str )
{
  return string2entitypath( str, &m_entity_path ) ? false : true;
}
