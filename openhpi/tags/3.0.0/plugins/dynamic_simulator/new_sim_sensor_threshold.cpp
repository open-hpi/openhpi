/** 
 * @file    new_sim_sensor_threshold.cpp
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
 
#include "new_sim_sensor_threshold.h"
#include "new_sim_log.h"
#include "new_sim_domain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>


/**
 * Constructor
 **/
NewSimulatorSensorThreshold::NewSimulatorSensorThreshold( NewSimulatorResource *res )
  : NewSimulatorSensor( res )
{
}


/**
 * Fully qualified constructor to fill an object with the parsed data
 **/
NewSimulatorSensorThreshold::NewSimulatorSensorThreshold( NewSimulatorResource *res,
                      SaHpiRdrT rdr, 
                      SaHpiSensorReadingT data, 
                      SaHpiEventStateT event_state, 
                      SaHpiEventStateT event_amask, 
                      SaHpiEventStateT event_dmask,
                      SaHpiSensorThresholdsT thresholds,
                      SaHpiBoolT enabled, 
                      SaHpiBoolT event_enabled)
  : NewSimulatorSensor( res, rdr, data, event_state, event_amask, event_dmask, 
                        enabled, event_enabled ) {

   memcpy(&m_thres, &thresholds, sizeof(SaHpiSensorThresholdsT));
   m_read_thold = rdr.RdrTypeUnion.SensorRec.ThresholdDefn.ReadThold;
   m_write_thold = rdr.RdrTypeUnion.SensorRec.ThresholdDefn.WriteThold;
}


/**
 * Destructor
 **/
NewSimulatorSensorThreshold::~NewSimulatorSensorThreshold()
{
}


/**
 * TBD: Check where and whether it is needed
 **/
void
NewSimulatorSensorThreshold::HandleNew( NewSimulatorDomain *domain )
{

  NewSimulatorSensor::HandleNew( domain );
}

/**
 * TBD: Check where and whether it is needed
 * TODO: Change it properly due to new data structure
 **/
bool
NewSimulatorSensorThreshold::Cmp( const NewSimulatorSensor &s2 ) const
{
  if ( NewSimulatorSensor::Cmp( s2 ) == false )
       return false;

  const NewSimulatorSensorThreshold *t = dynamic_cast<const NewSimulatorSensorThreshold *>( &s2 );

  if ( !t )
       return false;

/** TODO 
 * Should be changed due to other private variables 
 * 
  if ( m_sensor_init_thresholds  != t->m_sensor_init_thresholds )
       return false;

  if ( m_sensor_init_hysteresis  != t->m_sensor_init_hysteresis )
       return false;

  if ( m_hysteresis_support      != t->m_hysteresis_support )
       return false;

  if ( m_threshold_access        != t->m_threshold_access )
       return false;

  if ( m_assertion_event_mask    != t->m_assertion_event_mask )
       return false;
       
  if ( m_deassertion_event_mask  != t->m_deassertion_event_mask )
       return false;

  if ( m_reading_mask            != t->m_reading_mask )
       return false;

  if ( m_threshold_readable      != t->m_threshold_readable )
       return false;

  if ( m_threshold_settable      != t->m_threshold_settable )
       return false;

  if ( m_rate_unit          != t->m_rate_unit )
       return false;

  if ( m_modifier_unit_use  != t->m_modifier_unit_use )
       return false;

  if ( m_percentage         != t->m_percentage )
       return false;

  if ( m_base_unit          != t->m_base_unit )
       return false;

  if ( m_modifier_unit      != t->m_modifier_unit )
       return false;

  bool sf1 = m_sensor_factors ? true : false;
  bool sf2 = t->m_sensor_factors ? true : false;

  if ( sf1 != sf2 )
       return false;

  if ( m_sensor_factors )
       if ( m_sensor_factors->Cmp( *t->m_sensor_factors ) == false )
            return false;

  if ( m_normal_min_specified != t->m_normal_min_specified )
       return false;

  if ( m_normal_max_specified != t->m_normal_max_specified )
       return false;

  if ( m_nominal_reading_specified != t->m_nominal_reading_specified )
       return false;

  if ( m_nominal_reading != t->m_nominal_reading )
       return false;

  if ( m_normal_max != t->m_normal_max )
       return false;

  if ( m_normal_min != t->m_normal_min )
       return false;

  if ( m_sensor_max != t->m_sensor_max )
       return false;

  if ( m_sensor_min != t->m_sensor_min )
       return false;
  if (    m_upper_non_recoverable_threshold
       != t->m_upper_non_recoverable_threshold )
       return false;

  if ( m_upper_critical_threshold != t->m_upper_critical_threshold )
       return false;

  if (    m_upper_non_critical_threshold
       != t->m_upper_non_critical_threshold )
       return false;

  if (    m_lower_non_recoverable_threshold
       != t->m_lower_non_recoverable_threshold )
       return false;

  if ( m_lower_critical_threshold != t->m_lower_critical_threshold )
       return false;

  if ( m_lower_non_critical_threshold
      != t->m_lower_non_critical_threshold )
       return false;

  if (    m_positive_going_threshold_hysteresis
       != t->m_positive_going_threshold_hysteresis )
       return false;

  if (    m_negative_going_threshold_hysteresis
       != t->m_negative_going_threshold_hysteresis )
       return false;
**/

  return true;
}

/**
SaErrorT
NewSimulatorSensorThreshold::CreateEvent( NewSimulatorEvent *event, SaHpiEventT &h )
{
  SaErrorT rv = NewSimulatorSensor::CreateEvent( event, h );

  if ( rv != SA_OK )
       return rv;

  // sensor event
  SaHpiSensorEventT &se = h.EventDataUnion.SensorEvent;

  se.Assertion = (SaHpiBoolT)!(event->m_data[9] & 0x80);

  tIpmiThresh threshold = (tIpmiThresh)((event->m_data[10] >> 1) & 0x07);

  switch( threshold )
     {
       case eIpmiLowerNonCritical:
            se.EventState = SAHPI_ES_LOWER_MINOR;
            h.Severity    = SAHPI_MINOR;
            break;

       case eIpmiLowerCritical:
            se.EventState = SAHPI_ES_LOWER_MAJOR;
            h.Severity    = SAHPI_MAJOR;
            break;

       case eIpmiLowerNonRecoverable:
            se.EventState = SAHPI_ES_LOWER_CRIT;
            h.Severity    = SAHPI_CRITICAL;
            break;

       case eIpmiUpperNonCritical:
            se.EventState = SAHPI_ES_UPPER_MINOR;
            h.Severity    = SAHPI_MINOR;
            break;

       case eIpmiUpperCritical:
            se.EventState = SAHPI_ES_UPPER_MAJOR;
            h.Severity    = SAHPI_MAJOR;
            break;

       case eIpmiUpperNonRecoverable:
            se.EventState = SAHPI_ES_UPPER_CRIT;
            h.Severity = SAHPI_CRITICAL;
            break;

       default:
            stdlog << "Invalid threshold giving !\n";
            se.EventState = SAHPI_ES_UNSPECIFIED;
     }

  SaHpiSensorOptionalDataT optional_data = 0;

  // byte 2
  tIpmiEventType type = (tIpmiEventType)(event->m_data[10] >> 6);

  if ( type == eIpmiEventData1 )
  {
       ConvertToInterpreted( event->m_data[11],  se.TriggerReading );
       optional_data |= SAHPI_SOD_TRIGGER_READING;
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

  if ( type == eIpmiEventData1 )
  {
       ConvertToInterpreted( event->m_data[12], se.TriggerThreshold );
       optional_data |= SAHPI_SOD_TRIGGER_THRESHOLD;
  }
  else if ( type == eIpmiEventData2 )
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
void NewSimulatorSensorThreshold::Dump( NewSimulatorLog &dump ) const {
  NewSimulatorSensor::Dump( dump );
  dump << "Reading Threshold definition: " << m_read_thold << "\n";
  dump << "Writing Threshold definition: " << m_write_thold << "\n";
  dump << "Threshold values should come here - to be done\n"; 
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
bool NewSimulatorSensorThreshold::CreateRdr( SaHpiRptEntryT &resource,
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
SaErrorT NewSimulatorSensorThreshold::GetSensorReading( SaHpiSensorReadingT &data,
                                                        SaHpiEventStateT &state ) {

   stdlog << "DBG: NewSimulatorSensorThreshold::GetSensorReading is called\n";
   if ( m_enabled == SAHPI_FALSE )
      return SA_ERR_HPI_INVALID_REQUEST;

   if ( &data != NULL )
     memcpy( &data, &m_read_data, sizeof( SaHpiSensorReadingT ));

   if ( &state != NULL )
     memcpy( &state, &m_event_data, sizeof( SaHpiEventStateT ));

   return SA_OK;
}

/** 
 * HPI function saHpiSensorThresholdsGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copy of the internal threshold values (if a read is allowed).
 * 
 * @param thres Threshold structure in which the internal data should be copied 
 * 
 * @return error code 
 **/
SaErrorT NewSimulatorSensorThreshold::GetThresholds( SaHpiSensorThresholdsT &thres ) {

   stdlog << "DBG: read thresholds for sensor " << EntityPath() << " num " 
          << m_sensor_record.Num << " " << IdString() << ".\n";

   if ((ThresholdDefn().IsAccessible == SAHPI_FALSE ) ||
        ( m_read_thold == 0 ))
      return SA_ERR_HPI_INVALID_CMD;

   memcpy(&thres, &m_thres, sizeof(SaHpiSensorThresholdsT));
   setMask(thres, m_read_thold);
  
   return SA_OK;
}


/** 
 * HPI function saHpiSensorThresholdsSet()
 * 
 * See also the description of the function inside the specification or header file.
 * The checks are done in the methods \n
 * NewSimulatorSensorThreshold::checkThresholdValue()\n
 * NewSimulatorSensorThreshold::checkHysteresisValue()\n
 * NewSimulatorSensorThreshold::checkOrdering().
 * 
 * @param thres Threshold structure with data which should be set internally 
 * 
 * @return error code 
 **/
SaErrorT NewSimulatorSensorThreshold::SetThresholds( const SaHpiSensorThresholdsT &thres ) {
   SaErrorT rv;
   SaHpiSensorThresholdsT tmp;

   stdlog << "DBG: write thresholds for sensor " << EntityPath() << " num " 
         << m_sensor_record.Num << " " << IdString() << ".\n";

   
   if (  ( EventCategory() != SAHPI_EC_THRESHOLD )
       || ( ThresholdDefn().IsAccessible == SAHPI_FALSE )) {
      stdlog << "DBG: return INVALID_CMD since ";
      if ( EventCategory() != SAHPI_EC_THRESHOLD )
         stdlog << " - the event category isn't EC_THRESHOLD ";
      if ( ThresholdDefn().IsAccessible == SAHPI_FALSE )
         stdlog << " - the thresholdsdefinition isn't accessible";
      stdlog << "\n";
      return SA_ERR_HPI_INVALID_CMD;
   }

   memcpy(&tmp, &m_thres, sizeof(SaHpiSensorThresholdsT));
   
   // Check the threshold values
   if (thres.LowCritical.IsSupported) {
      rv = checkThresholdValue( thres.LowCritical, SAHPI_STM_LOW_CRIT, tmp.LowCritical );   
      if (rv != SA_OK) 
         return rv;
   } 
      
   if (thres.LowMajor.IsSupported) {
      rv = checkThresholdValue( thres.LowMajor, SAHPI_STM_LOW_MAJOR, tmp.LowMajor );   
       if (rv != SA_OK) 
          return rv;
   }
   
   if (thres.LowMinor.IsSupported) {
      rv = checkThresholdValue( thres.LowMinor, SAHPI_STM_LOW_MINOR, tmp.LowMinor );   
       if (rv != SA_OK) 
          return rv;
   }
   
   if (thres.UpCritical.IsSupported) {
      rv = checkThresholdValue( thres.UpCritical, SAHPI_STM_UP_CRIT, tmp.UpCritical );   
       if (rv != SA_OK) 
          return rv;
   }
      
   if (thres.UpMajor.IsSupported) {
      rv = checkThresholdValue( thres.UpMajor, SAHPI_STM_UP_MAJOR, tmp.UpMajor );   
       if (rv != SA_OK) 
          return rv;
   }
   
   if (thres.UpMinor.IsSupported) {
      rv = checkThresholdValue( thres.UpMinor, SAHPI_STM_UP_MINOR, tmp.UpMinor );   
       if (rv != SA_OK) 
          return rv;
   }
   
   
   // Check the hysteresis values 
   if (thres.PosThdHysteresis.IsSupported) {
      rv = checkHysteresisValue( thres.PosThdHysteresis, SAHPI_STM_UP_HYSTERESIS, tmp.PosThdHysteresis );   
       if (rv != SA_OK) 
          return rv;
   }
   
   if (thres.NegThdHysteresis.IsSupported) {
      rv = checkHysteresisValue( thres.NegThdHysteresis, SAHPI_STM_LOW_HYSTERESIS, tmp.NegThdHysteresis );   
       if (rv != SA_OK) 
          return rv;
   }


   // Check the ordering
   rv = checkOrdering( tmp );
   if ( rv != SA_OK )
      return rv; 
   
   // Ok, it seems everything is fine - take the new values    
   memcpy( &m_thres, &tmp, sizeof(SaHpiSensorThresholdsT));

   return SA_OK;
}


/** 
 * Check whether the setting of one threshold value is allowed
 * 
 * It is checked if the WriteFlag is set, the value has the correct type and is in the Range
 * MIN <= value <= MAX. If the checks are successful, the value is copied.
 * 
 * @param checkval Value which should be checked
 * @param flag     Check whether a Write operation is allowed
 * @param setval   Address where to copy the value
 * 
 * @return error code or SA_OK in success case
 **/
SaErrorT NewSimulatorSensorThreshold::checkThresholdValue( const SaHpiSensorReadingT &checkval,
                                                           SaHpiSensorRangeFlagsT flag,
                                                           SaHpiSensorReadingT &setval ) {
   if (!(m_write_thold & flag))
      return SA_ERR_HPI_INVALID_CMD;
                                                     	
   if (checkval.Type != DataFormat().ReadingType)
      return SA_ERR_HPI_INVALID_DATA;
          
   if (DataFormat().Range.Flags & SAHPI_SRF_MIN) {
      if (lt(checkval, DataFormat().Range.Min))
         return SA_ERR_HPI_INVALID_CMD;
   }
   
   if (DataFormat().Range.Flags & SAHPI_SRF_MAX) {
      if (gt(checkval, DataFormat().Range.Max))
         return SA_ERR_HPI_INVALID_CMD;
   }
   
   memcpy(&setval, &checkval, sizeof(SaHpiSensorReadingT));
   
   return SA_OK;
}


/** 
 * Check whether the setting of one Hysteresis value is allowed
 * 
 * It is checked if the WriteFlag is set, the value has the correct type and is > 0. 
 * If the checks are successful, the value is copied.
 * 
 * @param checkval Value which should be checked
 * @param flag     Check whether a Write operation is allowed
 * @param setval   Address where to copy the value
 * 
 * @return error code or SA_OK in success case
 **/
SaErrorT NewSimulatorSensorThreshold::checkHysteresisValue( const SaHpiSensorReadingT &checkval,
                                                            SaHpiSensorRangeFlagsT flag,
                                                            SaHpiSensorReadingT &setval ) {
 
   if (!(m_write_thold & flag))
      return SA_ERR_HPI_INVALID_CMD;
                                                     	
   if (checkval.Type != DataFormat().ReadingType)
      return SA_ERR_HPI_INVALID_DATA;
          
   if (ltZero(checkval))
      return SA_ERR_HPI_INVALID_DATA;
       
   memcpy(&setval, &checkval, sizeof(SaHpiSensorReadingT));
   
   return SA_OK;
}


/** 
 * Check if the ordering is correct
 * 
 * @param thres   Threshold values to be checked 
 * 
 * @return SA_ERR_HPI_INVALID_DATA if thres is out-of-order
 **/
SaErrorT NewSimulatorSensorThreshold::checkOrdering( const SaHpiSensorThresholdsT &thres ) {
   int i = 0, j;
   SaHpiSensorReadingT val[6];

   if (m_write_thold & SAHPI_STM_UP_CRIT) {
      memcpy(&val[i], &thres.UpCritical, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   if (m_write_thold & SAHPI_STM_UP_MAJOR) {
      memcpy(&val[i], &thres.UpMajor, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   if (m_write_thold & SAHPI_STM_UP_MINOR) {
      memcpy(&val[i], &thres.UpMinor, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   if (m_write_thold & SAHPI_STM_LOW_MINOR) {
      memcpy(&val[i], &thres.LowMinor, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   if (m_write_thold & SAHPI_STM_LOW_MAJOR) {
      memcpy(&val[i], &thres.LowMajor, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   if (m_write_thold & SAHPI_STM_LOW_CRIT) {
      memcpy(&val[i], &thres.LowCritical, sizeof( SaHpiSensorReadingT ));
      i++;
   }
   
   // No ordering must be checked - only one value could be set
   if (i < 1) 
      return SA_OK;
      
   for (j = 1; j < i; j++) 
      if ( lt(val[j-1], val[j]) )
         return SA_ERR_HPI_INVALID_DATA;
         
   return SA_OK;
}


/** 
 * Set the threshold support flags how it is allowed by a mask
 * 
 * @param thres    address of threshold to be set
 * @param mask     threshold mask to set in structure
 **/
void NewSimulatorSensorThreshold::setMask( SaHpiSensorThresholdsT &thres, 
                                            const SaHpiSensorThdMaskT mask) {

   if (mask & SAHPI_STM_UP_CRIT) {
      thres.UpCritical.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.UpCritical.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_UP_MAJOR) {
      thres.UpMajor.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.UpMajor.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_UP_MINOR) {
   	  thres.UpMinor.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.UpMinor.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_LOW_MINOR) {
   	  thres.LowMinor.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.LowMinor.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_LOW_MAJOR) {
   	  thres.LowMajor.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.LowMajor.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_LOW_CRIT) {
   	  thres.LowCritical.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.LowCritical.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_UP_HYSTERESIS) {
   	  thres.PosThdHysteresis.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.PosThdHysteresis.IsSupported = SAHPI_FALSE;
   }
   if (mask & SAHPI_STM_LOW_HYSTERESIS) {
   	  thres.NegThdHysteresis.IsSupported = SAHPI_TRUE;
   } else {
   	  thres.NegThdHysteresis.IsSupported = SAHPI_FALSE;
   }
                                           	
}
