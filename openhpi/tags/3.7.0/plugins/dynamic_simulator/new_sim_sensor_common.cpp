/** 
 * @file    new_sim_sensor_common.cpp
 *
* The file includes a class for common sensor handling:\n
 * NewSimulatorSensorCommon
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.1
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 * 
 */
  
#include "new_sim_sensor_common.h"
#include "new_sim_domain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <math.h>


/**
 * Constructor
 **/
NewSimulatorSensorCommon::NewSimulatorSensorCommon( NewSimulatorResource *res )
  : NewSimulatorSensor( res )
{
}


/**
 * Fully qualified constructor to fill an object with the parsed data
 **/
NewSimulatorSensorCommon::NewSimulatorSensorCommon( NewSimulatorResource *res,
                      SaHpiRdrT rdr, 
                      SaHpiSensorReadingT data, 
                      SaHpiEventStateT event_state, 
                      SaHpiEventStateT event_amask, 
                      SaHpiEventStateT event_dmask,
                      SaHpiBoolT enabled, 
                      SaHpiBoolT event_enabled)
  : NewSimulatorSensor( res, rdr, data, event_state, event_amask, event_dmask, 
                        enabled, event_enabled ) {    
}


/**
 * Destructor
 **/
NewSimulatorSensorCommon::~NewSimulatorSensorCommon()
{
}


/**
 * A rdr structure is filled with the internally data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorSensorCommon::CreateRdr( SaHpiRptEntryT &resource,
                                     SaHpiRdrT &rdr ) {
  if ( NewSimulatorSensor::CreateRdr( resource, rdr ) == false )
       return false;

  return true;
}


/** 
 * HPI function saHpiSensorReadingGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param data sensor reading variable in which the data should be copied 
 * @param state sensor event state variable in which the states should be copied
 * 
 * @return error code 
 **/
SaErrorT NewSimulatorSensorCommon::GetSensorReading( SaHpiSensorReadingT &data,
                                                     SaHpiEventStateT &state ) {

  stdlog << "DBG: NewSimulatorSensorCommon::GetSensorReading is called\n";                                                 	
  if ( m_enabled == SAHPI_FALSE )
     return SA_ERR_HPI_INVALID_REQUEST;

  if ( &data != NULL ) {
     if (m_read_support) {
        memcpy( &data, &m_read_data, sizeof( SaHpiSensorReadingT ));
     } else {
        memset( &data, 0, sizeof( SaHpiSensorReadingT ) );
        data.IsSupported = SAHPI_FALSE;
     }
  }

  if ( &state != NULL ) {
     memcpy( &state, &m_event_data, sizeof( SaHpiEventStateT ) );
  }

  return SA_OK;
}

/**
SaErrorT
NewSimulatorSensorCommon::CreateEvent( NewSimulatorEvent *event, SaHpiEventT &h )
{

  SaErrorT rv = NewSimulatorSensor::CreateEvent( event, h );

  if ( rv != SA_OK )
       return rv;

  // sensor event
  SaHpiSensorEventT &se = h.EventDataUnion.SensorEvent;

  se.Assertion = (SaHpiBoolT)!(event->m_data[9] & 0x80);

  se.EventState = (1 << (event->m_data[10] & 0x0f));

  // default value
  h.Severity = SAHPI_INFORMATIONAL;

  SaHpiSensorOptionalDataT optional_data = 0;

  // byte 2
  tIpmiEventType type = (tIpmiEventType)(event->m_data[10] >> 6);

  if ( type == eIpmiEventData1 )
  {
      if ((event->m_data[11] & 0x0f) != 0x0f)
      {
          se.PreviousState = (1 << (event->m_data[11] & 0x0f));
          optional_data |= SAHPI_SOD_PREVIOUS_STATE;
      }
      if ((event->m_data[11] & 0xf0) != 0xf0)
      {
          SaHpiEventStateT evt_sec_state = (1 << ((event->m_data[11]>> 4) & 0x0f));
          switch (evt_sec_state)
          {
          case SAHPI_ES_OK:
              h.Severity = SAHPI_OK;
              break;
          case SAHPI_ES_MINOR_FROM_OK:
              h.Severity = SAHPI_MINOR;
              break;
          case SAHPI_ES_MAJOR_FROM_LESS:
              h.Severity = SAHPI_MAJOR;
              break;
          case SAHPI_ES_CRITICAL_FROM_LESS:
              h.Severity = SAHPI_CRITICAL;
              break;
          case SAHPI_ES_MINOR_FROM_MORE:
              h.Severity = SAHPI_MINOR;
              break;
          case SAHPI_ES_MAJOR_FROM_CRITICAL:
              h.Severity = SAHPI_MAJOR;
              break;
          case SAHPI_ES_CRITICAL:
              h.Severity = SAHPI_CRITICAL;
              break;
          case SAHPI_ES_MONITOR:
              h.Severity = SAHPI_INFORMATIONAL;
              break;
          case SAHPI_ES_INFORMATIONAL:
              h.Severity = SAHPI_INFORMATIONAL;
              break;
          }
      }
  }
  else if ( type == eIpmiEventData2 )
  {
       se.Oem = (SaHpiUint32T)event->m_data[11]; 
       optional_data |= SAHPI_SOD_OEM;
  }
  else if ( type == eIpmiEventData3 )
  {
       se.SensorSpecific = (SaHpiUint32T)event->m_data[11]; 
       optional_data |= SAHPI_SOD_SENSOR_SPECIFIC;
  }

  // byte 3
  type = (tIpmiEventType)((event->m_data[10] & 0x30) >> 4);

  if ( type == eIpmiEventData2 )
  {
       se.Oem |= (SaHpiUint32T)((event->m_data[12] << 8) & 0xff00);
       optional_data |= SAHPI_SOD_OEM;
  }
  else if ( type == eIpmiEventData3 )
  {
       se.SensorSpecific |= (SaHpiUint32T)((event->m_data[12] << 8) & 0xff00);
       optional_data |= SAHPI_SOD_SENSOR_SPECIFIC;
  }

  se.OptionalDataPresent = optional_data;


  return SA_OK;
}
**/


/** 
 * Dump the sensor information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorSensorCommon::Dump( NewSimulatorLog &dump ) const {
	
  NewSimulatorSensor::Dump( dump );
  dump << "Common Sensor is defined,\n";
}
