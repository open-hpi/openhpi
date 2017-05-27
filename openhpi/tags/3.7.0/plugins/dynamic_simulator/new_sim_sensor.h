/** 
 * @file    new_sim_sensor.h
 *
 * The file includes an abstract class for sensor handling:\n
 * NewSimulatorSensor
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.3
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *    
 */

#ifndef __NEW_SIM_SENSOR_H__
#define __NEW_SIM_SENSOR_H__


extern "C" {
#include "SaHpi.h"
}


/**
#ifndef __NEW_SIM_EVENT_H__
#include "new_sim_event.h"
#endif
**/

#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif


class  NewSimulatorDomain;

/**
 * @class NewSimulatorSensor
 * 
 * Abstract class for simulating sensors
 * 
 **/
class NewSimulatorSensor : public NewSimulatorRdr {

protected:
  /// Record with the sensor information
  SaHpiSensorRecT        m_sensor_record;
  
  /// Is this sensor enabled
  SaHpiBoolT             m_enabled;
  /// Are events from this sensor enabled
  SaHpiBoolT             m_events_enabled;
  /// Is the reading supported
  SaHpiBoolT             m_read_support;
  /// Events assertion mask
  SaHpiEventStateT       m_assert_mask;
  /// Events deassertion mask
  SaHpiEventStateT       m_deassert_mask;
  
  /// SensorReading values
  SaHpiSensorReadingT    m_read_data;
  /// EventState
  SaHpiEventStateT       m_event_data;
  
  virtual bool gt(const SaHpiSensorReadingT &val1, const SaHpiSensorReadingT &val2);
  virtual bool ge(const SaHpiSensorReadingT &val1, const SaHpiSensorReadingT &val2);
  virtual bool lt(const SaHpiSensorReadingT &val1, const SaHpiSensorReadingT &val2);
  virtual bool le(const SaHpiSensorReadingT &val1, const SaHpiSensorReadingT &val2);
  virtual bool eq(const SaHpiSensorReadingT &val1, const SaHpiSensorReadingT &val2);
  virtual bool ltZero(const SaHpiSensorReadingT &val1);
  
public:
  NewSimulatorSensor( NewSimulatorResource *res );
  NewSimulatorSensor( NewSimulatorResource *res,
                      SaHpiRdrT rdr, 
                      SaHpiSensorReadingT data, 
                      SaHpiEventStateT event_state, 
                      SaHpiEventStateT event_amask, 
                      SaHpiEventStateT event_dmask,
                      SaHpiBoolT enabled, 
                      SaHpiBoolT event_enabled);
                      
  virtual ~NewSimulatorSensor();

  /// Return sensor Number
  virtual unsigned int Num() const { return m_sensor_record.Num; }

  /// Return Sensor Type
  SaHpiSensorTypeT       Type() const { return m_sensor_record.Type; }
  /// Return Sensor Event Category
  SaHpiEventCategoryT    EventCategory() const { return m_sensor_record.Category; }
  /// Return if Sensor is enabbled or not 
  SaHpiBoolT             EnableCtrl() const { return m_sensor_record.EnableCtrl; }
  /// Return the event change capability of the sensor
  SaHpiSensorEventCtrlT  EventCtrl() const { return m_sensor_record.EventCtrl; }
  /// Return Sensor Event States
  SaHpiEventStateT       EventStates() const { return m_sensor_record.Events; }
  /// Return Sensor Data Format
  SaHpiSensorDataFormatT DataFormat() const { return m_sensor_record.DataFormat; }
  /// Return Sensor Threshold definitions
  SaHpiSensorThdDefnT    ThresholdDefn() const { return m_sensor_record.ThresholdDefn; }
  /// Return Sensor Oem information
  SaHpiUint32T           Oem() const { return m_sensor_record.Oem; }

  /// Not clear if this function is really needed
  virtual void HandleNew( NewSimulatorDomain *domain );
  /// Not implemented - to be verified if needed
  virtual bool Cmp( const NewSimulatorSensor &s2 ) const;
  
  // create an HPI event from ipmi event
//  virtual SaErrorT CreateEvent( NewSimulatorEvent *event, SaHpiEventT &h );

  /// create and send HPI sensor enable change event
  void CreateEnableChangeEvent();

  // handle all incoming sensor events
//  virtual void HandleEvent( NewSimulatorEvent *event );
  virtual void Dump( NewSimulatorLog &dump ) const;

  /// create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // Official HPI functions
  /// abstract method for the GetSensorReading command
  virtual SaErrorT GetSensorReading( SaHpiSensorReadingT &data, SaHpiEventStateT &state ) = 0;
  SaErrorT GetEnable( SaHpiBoolT &enable );
  SaErrorT GetEventEnables( SaHpiBoolT &enables );
  SaErrorT GetEventMasks( SaHpiEventStateT &AssertEventMask,
                          SaHpiEventStateT &DeassertEventMask);
  SaErrorT SetEnable( const SaHpiBoolT &enable );
  SaErrorT SetEventEnables( const SaHpiBoolT &enables );
  SaErrorT SetEventMasks( const SaHpiSensorEventMaskActionT &act,
                          SaHpiEventStateT &AssertEventMask,
                          SaHpiEventStateT &DeassertEventMask );

};


#endif
