/*
 *
 * Copyright (c) 2003 by FORCE Computers
 *
 * Note that this file is based on parts of OpenIPMI
 * written by Corey Minyard <minyard@mvista.com>
 * of MontaVista Software. Corey's code was helpful
 * and many thanks go to him. He gave the permission
 * to use this code in OpenHPI under BSD license.
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

#ifndef dIpmiEntity_h
#define dIpmiEntity_h


#include <glib.h>
#include <string.h>


__BEGIN_DECLS
#include "SaHpi.h"
__END_DECLS


#ifndef dIpmiRdr_h
#include "ipmi_rdr.h"
#endif

#ifndef dIpmiEvent_h
#include "ipmi_event.h"
#endif

#ifndef dIpmiFru_h
#include "ipmi_fru.h"
#endif


class cIpmiSensor;
class cIpmiControl;
class cIpmiSel;


enum tIpmiEntityId
{
  eIpmiEntityInvalid                        =  0,
  eIpmiEntityIdOther                        =  1,
  eIpmiEntityIdUnkown                       =  2,
  eIpmiEntityIdProcessor                    =  3,
  eIpmiEntityIdDisk                         =  4,
  eIpmiEntityIdPeripheral                   =  5,
  eIpmiEntityIdSystemManagementModule       =  6,
  eIpmiEntityIdSystemBoard                  =  7,
  eIpmiEntityIdMemoryModule                 =  8,
  eIpmiEntityIdProcessorModule              =  9,
  eIpmiEntityIdPowerSupply                  = 10,
  eIpmiEntityIdAddInCard                    = 11,
  eIpmiEntityIdFrontPanelBoard              = 12,
  eIpmiEntityIdBackPanelBoard               = 13,
  eIpmiEntityIdPowerSystemBoard             = 14,
  eIpmiEntityIdDriveBackplane               = 15,
  eIpmiEntityIdSystemInternalExpansionBoard = 16,
  eIpmiEntityIdOtherSystemBoard             = 17,
  eIpmiEntityIdProcessorBoard               = 18,
  eIpmiEntityIdPowerUnit                    = 19,
  eIpmiEntityIdPowerModule                  = 20,
  eIpmiEntityIdPowerManagementBoard         = 21,
  eIpmiEntityIdChassisBackPanelBoard        = 22,
  eIpmiEntityIdSystemChassis                = 23,
  eIpmiEntityIdSubChassis                   = 24,
  eIpmiEntityIdOtherChassisBoard            = 25,
  eIpmiEntityIdDiskDriveBay                 = 26,
  eIpmiEntityIdPeripheralBay                = 27,
  eIpmiEntityIdDeviceBay                    = 28,
  eIpmiEntityIdFanCooling                   = 29,
  eIpmiEntityIdCoolingUnit                  = 30,
  eIpmiEntityIdCableInterconnect            = 31,
  eIpmiEntityIdMemoryDevice                 = 32,
  eIpmiEntityIdSystemManagementSoftware     = 33,
  eIpmiEntityIdBios                         = 34,
  eIpmiEntityIdOperatingSystem              = 35,
  eIpmiEntityIdSystemBus                    = 36,
  eIpmiEntityIdGroup                        = 37,
  eIpmiEntityIdRemoteMgmtCommDevice         = 38,
  eIpmiEntityIdExternalEnvironment          = 39,
  eIpmiEntityIdBattery                      = 40,
  eIpmiEntityIdProcessingBlade              = 41,
  eIpmiEntityIdConnectivitySwitch           = 42,
  eIpmiEntityIdProcessorMemoryModule        = 43,
  eIpmiEntityIdIoModule                     = 44,
  eIpmiEntityIdProcessorIoModule            = 45,
  eIpmiEntityIdMgmtControllerFirmware       = 46,

  // PIGMIG entity ids
  eIpmiEntityIdPigMgFrontBoard              = 0xa0,
  eIpmiEntityIdPigMgRearTransitionModule    = 0xc0,
  eIpmiEntityIdAtcaShelfManager             = 0xf0,
  eIpmiEntityIdAtcaFiltrationUnit           = 0xf1,
};

const char *IpmiEntityIdToString( tIpmiEntityId id );


// wrapper class for entity path
class cIpmiEntityPath
{
public:
  SaHpiEntityPathT m_entity_path;
  
  cIpmiEntityPath();
  cIpmiEntityPath( const SaHpiEntityPathT &entity_path );

  operator SaHpiEntityPathT()
  {
    return m_entity_path;
  }
};


cIpmiLog &operator<<( cIpmiLog &log, const cIpmiEntityPath &epath );


class cIpmiDomain;


struct tIpmiDeviceNum
{
  unsigned char channel;
  unsigned char address;
};


class cIpmiSensorHotswap;


class cIpmiEntity : public cIpmiRdrContainer
{
protected:
  cIpmiDomain *m_domain;

  // Key fields
  unsigned char m_access_address;
  unsigned char m_slave_address;
  unsigned char m_channel;
  unsigned char m_lun;
  unsigned char m_private_bus_id;

  // misc
  bool m_is_fru;
  bool m_is_mc;

  // If this entity is added by the user or comes from the main SDR
  // repository, we don't delete it automatically if it goes
  // away.
  bool m_came_from_sdr;

  // For FRU device only.
  bool m_is_logical_fru;
  unsigned char m_fru_id;

  /* For MC device only. */
  bool m_acpi_system_power_notify_required;
  bool m_acpi_device_power_notify_required;
  bool m_controller_logs_init_agent_errors;
  bool m_log_init_agent_errors_accessing;
  //  unsigned int global_init : 2;

  bool m_chassis_device;
  bool m_bridge;
  bool m_ipmb_event_generator;
  bool m_ipmb_event_receiver;
  bool m_fru_inventory_device;
  bool m_sel_device;
  bool m_sdr_repository_device;
  bool m_sensor_device;

  // For generic device only.
  unsigned char  m_address_span;

  tIpmiDeviceNum m_device_num;
  tIpmiEntityId  m_entity_id;
  unsigned int   m_entity_instance;

  unsigned char m_device_type;
  unsigned char m_device_modifier;
  unsigned int  m_oem;

  cIpmiTextBuffer m_id_string;

  bool          m_presence_sensor_always_there;
  int           m_presence_possibly_changed;

  cIpmiSensorHotswap *m_hotswap_sensor;

public:
  // return hotswap sensor if there is one
  cIpmiSensorHotswap *GetHotswapSensor() { return m_hotswap_sensor; }
 
  // add cIpmiRdr and create an Hpi Rdr.
  virtual bool Add( cIpmiRdr *rdr );

  // remove cIpmiRdr
  virtual bool Rem( cIpmiRdr *rdr );

protected:
  unsigned int m_current_control_id;

public:
  // get a unique control num for this entity
  unsigned int GetControlNum()
  {
    return ++m_current_control_id;
  }

protected:
  // SEL
  cIpmiSel *m_sel;

public:
  cIpmiSel *&Sel() { return m_sel; }

public:
  cIpmiEntity( cIpmiDomain *domain, tIpmiDeviceNum device_num,
               tIpmiEntityId entity_id, unsigned int entity_instance,
               bool came_from_sdr );
  ~cIpmiEntity();

  unsigned char &AccessAddress() { return m_access_address; }
  unsigned char SlaveAddress()   { return m_slave_address; }
  unsigned char &Channel()       { return m_channel; }
  unsigned char &Lun()           { return m_lun; }

  bool &CameFromSdr() { return m_came_from_sdr; }

  unsigned char FruId() { return m_fru_id; }

  tIpmiDeviceNum DeviceNum() { return m_device_num; }
  cIpmiDomain *Domain() { return m_domain; }

  tIpmiEntityId EntityId() const { return m_entity_id; }
  unsigned char EntityInstance() const { return m_entity_instance; }

  cIpmiTextBuffer &IdString()
  {
    return m_id_string;
  }

  unsigned int &Oem() { return m_oem; }

  bool Destroy();

  // HPI resource id
  SaHpiResourceIdT m_resource_id;

  virtual bool CreateResource( SaHpiRptEntryT &entry );
};


#endif
