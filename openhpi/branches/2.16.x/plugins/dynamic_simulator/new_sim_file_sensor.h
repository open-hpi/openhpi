/** 
 * @file    new_sim_file_sensor.h
 *
 * The file includes helper classes for parsing sensor data from the simulation file:\n
 * NewSimulatorFileSensor
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
 
#ifndef __NEW_SIM_FILE_SENSOR_H__
#define __NEW_SIM_FILE_SENSOR_H__

#include <glib.h>

extern "C" {
#include "SaHpi.h"
}

#ifndef __NEW_SIM_FILE_RDR_H__
#include "new_sim_file_rdr.h"
#endif

#ifndef __NEW_SIM_RDR_H__
#include "new_sim_rdr.h"
#endif

#ifndef __NEW_SIM_RESOURCE_H__
#include "new_sim_resource.h"
#endif

#ifndef __NEW_SIM_SENSOR_H__
#include "new_sim_sensor.h"
#endif

/**
 * @class NewSimulatorFileSensor
 * 
 * Provides some functions for parsing the sensor section of the simulation file.
 **/
class NewSimulatorFileSensor : public NewSimulatorFileRdr {
   private:
   SaHpiSensorRecT        *m_sensor_rec;
   SaHpiSensorReadingT    m_sensor_data;
   SaHpiEventStateT       m_sensor_event_state;
   SaHpiEventStateT       m_sensor_event_amask;
   SaHpiEventStateT       m_sensor_event_dmask;
   SaHpiSensorThresholdsT m_sensor_thresholds;
   SaHpiBoolT             m_sensor_enabled;
   SaHpiBoolT             m_sensor_event_enabled;

   bool process_dataformat        ( SaHpiSensorDataFormatT *dataformat);
   bool process_dataformat_range  ( SaHpiSensorRangeT      *datarange );
   bool process_thresholddef      ( SaHpiSensorThdDefnT    *thresdef );
   bool process_sensor_data_token ( void );
   bool process_sensor_thresholds ( SaHpiSensorThresholdsT *thres );
   bool process_sensor_reading    ( SaHpiSensorReadingT    *sensorreading );
   

   public:
   NewSimulatorFileSensor(GScanner *scanner);
   virtual ~NewSimulatorFileSensor();
   virtual NewSimulatorRdr * process_token(NewSimulatorResource *res);

};

#endif /*__NEW_SIM_FILE_SENSOR_H_*/
