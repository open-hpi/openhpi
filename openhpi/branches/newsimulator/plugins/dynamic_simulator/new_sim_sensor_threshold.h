/** 
 * @file    new_sim_sensor_threshold.h
 *
 * The file includes a class for threshold sensor handling:\n
 * NewSimulatorSensorThreshold
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.2
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
  
#ifndef __NEW_SIM_SENSOR_THRESHOLD_H__
#define __NEW_SIM_SENSOR_THRESHOLD_H__


#ifndef __NEW_SIM_SENSOR_H__
#include "new_sim_sensor.h"
#endif

/**
 * @class NewSimulatorSensorThreshold
 * 
 * Class for simulating threshold sensors
 * 
 **/
class NewSimulatorSensorThreshold : public NewSimulatorSensor {
private:
   SaHpiSensorThdMaskT   m_read_thold;
   SaHpiSensorThdMaskT   m_write_thold;
   SaHpiSensorThresholdsT  m_thres;

   SaErrorT checkThresholdValue( const SaHpiSensorReadingT &checkval,
                                 SaHpiSensorRangeFlagsT flag,
                                 SaHpiSensorReadingT &setval );
                               
   SaErrorT checkHysteresisValue( const SaHpiSensorReadingT &checkval,
                                  SaHpiSensorRangeFlagsT flag,
                                  SaHpiSensorReadingT &setval );
                                                              
   SaErrorT checkOrdering( const SaHpiSensorThresholdsT &thres );
   
   void setMask( SaHpiSensorThresholdsT &thres, const SaHpiSensorThdMaskT mask);


public:
  NewSimulatorSensorThreshold( NewSimulatorResource *res );
  NewSimulatorSensorThreshold( NewSimulatorResource *res,
                      SaHpiRdrT rdr, 
                      SaHpiSensorReadingT data, 
                      SaHpiEventStateT event_state, 
                      SaHpiEventStateT event_amask, 
                      SaHpiEventStateT event_dmask,
                      SaHpiSensorThresholdsT thresholds,
                      SaHpiBoolT enabled, 
                      SaHpiBoolT event_enabled);                     
  virtual ~NewSimulatorSensorThreshold();

  virtual void HandleNew( NewSimulatorDomain *domain );

//  virtual SaErrorT CreateEvent( NewSimulatorEvent *event, SaHpiEventT &h );
  // print some data
  virtual void Dump( NewSimulatorLog &dump ) const;
  // Compare to Sensors
  bool Cmp( const NewSimulatorSensor &s2 ) const;
  // create an RDR sensor record
  virtual bool CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr );

  // official hpi functions 

  // get sensor data
  SaErrorT GetSensorReading( SaHpiSensorReadingT &data, SaHpiEventStateT &state );
  // get thresholds data
  SaErrorT GetThresholds( SaHpiSensorThresholdsT &thres );
  // helper functions for SetThresholdsAndHysteresis
  SaErrorT SetThresholds( const SaHpiSensorThresholdsT &thres );

};


#endif
