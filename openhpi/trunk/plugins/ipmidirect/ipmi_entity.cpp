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

__BEGIN_DECLS
#include "epath_utils.h"
__END_DECLS


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


cIpmiEntityPath &
cIpmiEntityPath::operator+=( const cIpmiEntityPath &epath )
{
  ep_concat( &m_entity_path, &epath.m_entity_path );

  return *this;
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
