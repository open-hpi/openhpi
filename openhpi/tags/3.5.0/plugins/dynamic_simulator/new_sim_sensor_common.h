/** 
 * @file    new_sim_sensor_common.h
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


#ifndef __NEW_SIM_SENSOR_COMMON_H__
#define __NEW_SIM_SENSOR_COMMON_H__


#ifndef __NEW_SIM_SENSOR_H__
#include "new_sim_sensor.h"
#endif

/**
 * @class NewSimulatorSensorCommon
 * 
 * Class for simulating common sensors
 * 
 **/
class NewSimulatorSensorCommon : public NewSimulatorSensor
{
public:
  NewSimulatorSensorCommon( NewSimulatorResource *res );
  NewSimulatorSensorCommon( NewSimulatorResource *res,
                      SaHpiRdrT rdr, 
                      SaHpiSensorReadingT data, 
                      SaHpiEventStateT event_state, 
                      SaHpiEventStateT event_amask, 
                      SaHpiEventStateT event_dmask,
                      SaHpiBoolT enabled, 
                      SaHpiBoolT event_enabled);
                      
  virtual ~NewSimulatorSensorCommon();

  // create an hpi event from ipmi event
  //  virtual SaErrorT CreateEvent( NewSimulatorEvent *event, SaHpiEventT &h );

  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // get sensor data
  virtual SaErrorT GetSensorReading( SaHpiSensorReadingT &data, SaHpiEventStateT &state );
  // print the data
  virtual void Dump( NewSimulatorLog &dump ) const;

};


#endif
