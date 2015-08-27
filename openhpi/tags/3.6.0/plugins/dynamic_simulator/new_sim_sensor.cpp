/** 
 * @file    new_sim_sensor.cpp
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
 * Thanks to \n
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>\n
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *      
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <glib.h>
#include <math.h>

#include <oh_error.h>

#include "new_sim_domain.h"
#include "new_sim_sensor.h"
#include "new_sim_entity.h"
#include "new_sim_utils.h"
#include "new_sim_text_buffer.h"



/**
 * Constructor
 **/
NewSimulatorSensor::NewSimulatorSensor( NewSimulatorResource *res )
  : NewSimulatorRdr( res, SAHPI_SENSOR_RDR ), 
    m_enabled( SAHPI_TRUE ),
    m_events_enabled( SAHPI_TRUE ),
    m_read_support( SAHPI_TRUE ),
    m_assert_mask( 0 ),
    m_deassert_mask( 0 ){
    	
   memset( &m_sensor_record, 0, sizeof( SaHpiSensorRecT ));
   memset( &m_read_data, 0, sizeof( SaHpiSensorReadingT ));
   memset( &m_event_data, 0, sizeof( SaHpiEventStateT ));
}


/**
 * Full qualified constructor to fill an object with the parsed data
 **/
NewSimulatorSensor::NewSimulatorSensor( NewSimulatorResource *res,
                                        SaHpiRdrT rdr, 
                                        SaHpiSensorReadingT data, 
                                        SaHpiEventStateT event_state, 
                                        SaHpiEventStateT event_amask, 
                                        SaHpiEventStateT event_dmask,
                                        SaHpiBoolT enabled, 
                                        SaHpiBoolT event_enabled)
  : NewSimulatorRdr( res, SAHPI_SENSOR_RDR, rdr.Entity, rdr.IsFru, rdr.IdString ),
    m_enabled( enabled ),
    m_events_enabled( event_enabled ),
    m_read_support( SAHPI_TRUE ),
    m_assert_mask( event_amask ),
    m_deassert_mask( event_dmask ),
    m_event_data( event_state ) {

   memcpy(&m_sensor_record, &rdr.RdrTypeUnion.SensorRec, sizeof( SaHpiSensorRecT ));
   memcpy(&m_read_data, &data, sizeof( SaHpiSensorReadingT ));
    
}
  
                      
/**
 * Destructor
 **/
NewSimulatorSensor::~NewSimulatorSensor()
{
}


/**
 * TBD: Check where and whether it is needed
 **/
void NewSimulatorSensor::HandleNew( NewSimulatorDomain *domain )
{
//  m_sensor_type_string        = IpmiSensorTypeToString( m_sensor_type );
//  m_event_reading_type_string = IpmiEventReadingTypeToString( m_event_reading_type );
}


/**
 * TBD: Check where and whether it is needed
 * TODO: Change it properly due to new data structure
 **/
bool NewSimulatorSensor::Cmp( const NewSimulatorSensor &s2 ) const {
/**	
  if ( m_entity_path != s2.m_entity_path )
       return false;

  if ( m_sensor_init_scanning    != s2.m_sensor_init_scanning )
       return false;

  if ( m_sensor_init_events      != s2.m_sensor_init_events )
       return false;

  if ( m_sensor_init_type        != s2.m_sensor_init_type )
       return false;

  if ( m_sensor_init_pu_events   != s2.m_sensor_init_pu_events )
       return false;

  if ( m_sensor_init_pu_scanning != s2.m_sensor_init_pu_scanning )
       return false;

  if ( m_ignore_if_no_entity     != s2.m_ignore_if_no_entity )
       return false;

  if ( m_supports_auto_rearm     != s2.m_supports_auto_rearm )
       return false;

  if ( m_event_support           != s2.m_event_support )
       return false;

  if ( m_sensor_type             != s2.m_sensor_type )
       return false;

  if ( m_event_reading_type      != s2.m_event_reading_type )
       return false;

  if ( m_oem != s2.m_oem )
       return false;

  if ( IdString() != s2.IdString() )
       return false;
**/
  return true;
}


/** 
 * Check if val1 > val2
 * 
 * @param val1 SensorReading
 * @param val2 SensorReading 
 * 
 * @return false also in error case
 **/
bool NewSimulatorSensor::gt(const SaHpiSensorReadingT &val1, 
                             const SaHpiSensorReadingT &val2) {

   if (val1.Type != val2.Type) {
   	  err("Different sensor reading types in comparision.");
      return false;
   }
   
   switch(val1.Type) {

   case SAHPI_SENSOR_READING_TYPE_INT64:
      return (val1.Value.SensorInt64 > val2.Value.SensorInt64) ? true : false;
      break;

   case SAHPI_SENSOR_READING_TYPE_UINT64:
      return (val1.Value.SensorUint64 > val2.Value.SensorUint64) ? true : false;
      break;

   case SAHPI_SENSOR_READING_TYPE_FLOAT64:
      return (val1.Value.SensorFloat64 > val2.Value.SensorFloat64) ? true : false;
      break;
   
   case SAHPI_SENSOR_READING_TYPE_BUFFER:
      return (memcmp(&val1.Value.SensorBuffer, 
                      &val2.Value.SensorBuffer,
                      SAHPI_SENSOR_BUFFER_LENGTH) > 0) ? true : false;

   default:
      err("Invalid sensor reading type.");
      return false;
   }
}


/** 
 * Check if val1 == val2
 * 
 * @param val1 SensorReading
 * @param val2 SensorReading 
 * 
 * @return false also in error case
 **/
bool NewSimulatorSensor::eq(const SaHpiSensorReadingT &val1, 
                             const SaHpiSensorReadingT &val2) {

   if (val1.Type != val2.Type) {
   	  err("Different sensor reading types in comparision.");
      return false;
   }
   
   switch(val1.Type) {

   case SAHPI_SENSOR_READING_TYPE_INT64:
      return (val1.Value.SensorInt64 == val2.Value.SensorInt64) ? true : false;
      break;

   case SAHPI_SENSOR_READING_TYPE_UINT64:
      return (val1.Value.SensorUint64 == val2.Value.SensorUint64) ? true : false;
      break;

   case SAHPI_SENSOR_READING_TYPE_FLOAT64:
      return (val1.Value.SensorFloat64 == val2.Value.SensorFloat64) ? true : false;
      break;
   
   case SAHPI_SENSOR_READING_TYPE_BUFFER:
      return (memcmp(&val1.Value.SensorBuffer, 
                      &val2.Value.SensorBuffer,
                      SAHPI_SENSOR_BUFFER_LENGTH) == 0) ? true : false;

   default:
      err("Invalid sensor reading type.");
      return false;
   }
}


/** 
 * Check if val1 >= val2
 * 
 * @param val1 SensorReading
 * @param val2 SensorReading 
 * 
 * @return false also in error case
 **/
bool NewSimulatorSensor::ge(const SaHpiSensorReadingT &val1, 
                             const SaHpiSensorReadingT &val2) {

   if (val1.Type != val2.Type) {
   	  err("Different sensor reading types in comparision.");
      return false;
   }
   
   return (  gt( val1, val2 ) 
           || eq( val1, val2 ) );
}


/** 
 * Check if val1 < val2
 * 
 * @param val1 SensorReading
 * @param val2 SensorReading 
 * 
 * @return false also in error case
 **/
bool NewSimulatorSensor::lt(const SaHpiSensorReadingT &val1, 
                             const SaHpiSensorReadingT &val2) {

   if (val1.Type != val2.Type) {
   	  err("Different sensor reading types in comparision.");
      return false;
   }
   
   return ( ! ge( val1, val2 ) ) ;
}


/** 
 * Check if val1 <= val2
 * 
 * @param val1 SensorReading
 * @param val2 SensorReading 
 * 
 * @return false also in error case
 **/
bool NewSimulatorSensor::le(const SaHpiSensorReadingT &val1, 
                             const SaHpiSensorReadingT &val2) {

   if (val1.Type != val2.Type) {
   	  err("Different sensor reading types in comparision.");
      return false;
   }
   
   return ( ! gt( val1, val2 ) );
}


/** 
 * Check if val1 < 0
 * 
 * @param val1 SensorReading
 * 
 * @return return false also in error case
 **/
bool NewSimulatorSensor::ltZero(const SaHpiSensorReadingT &val1) {
   
   
   switch(val1.Type) {

   case SAHPI_SENSOR_READING_TYPE_INT64:
      return (val1.Value.SensorInt64 < 0) ? true : false;
      break;

   case SAHPI_SENSOR_READING_TYPE_UINT64:
      return false;
      break;

   case SAHPI_SENSOR_READING_TYPE_FLOAT64:
      return (val1.Value.SensorFloat64 < 0) ? true : false;
      break;
      
   case SAHPI_SENSOR_READING_TYPE_BUFFER:
      SaHpiUint8T zeroBuffer[SAHPI_SENSOR_BUFFER_LENGTH];
      memset(&zeroBuffer, 0, (sizeof(SaHpiUint8T)*SAHPI_SENSOR_BUFFER_LENGTH));
      return (memcmp(&val1.Value.SensorBuffer, 
                      &zeroBuffer,
                      SAHPI_SENSOR_BUFFER_LENGTH) < 0) ? true : false;
      break;
      
   default:
      err("Invalid sensor reading type.");
      return false;
   }
}


/** 
 * Dump the sensor information
 * 
 * @param dump Address of the log
 * 
 **/
void NewSimulatorSensor::Dump( NewSimulatorLog &dump ) const {
  char str[256];
  IdString().GetAscii( str, 256 );

  dump << "Sensor: " << m_sensor_record.Num << " " << str << "\n";
}


/**
 * A rdr structure is filled with the data
 * 
 * This method is called by method NewSimulatorRdr::Populate().
 * 
 * @param resource Address of resource structure
 * @param rdr Address of rdr structure
 * 
 * @return true
 **/
bool NewSimulatorSensor::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr ) {
  
  if ( NewSimulatorRdr::CreateRdr( resource, rdr ) == false )
       return false;

  // sensor record
  memcpy(&rdr.RdrTypeUnion.SensorRec, &m_sensor_record, sizeof(SaHpiSensorRecT));

  return true;
}


/**
 * HPI function saHpiSensorEnableGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param enable address of enable return value
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::GetEnable( SaHpiBoolT &enable ) {
    enable = m_enabled;

    return SA_OK;
}


/**
 * HPI function  saHpiSensorEnableSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param enable address of enable value to be set internally
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::SetEnable( const SaHpiBoolT &enable ) {
    if (m_enabled == enable)
        return SA_OK;

    m_enabled = enable;

    CreateEnableChangeEvent();

    return SA_OK;
}

/**
 * HPI function saHpiSensorEventEnableGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param enables address of event enable return value 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::GetEventEnables( SaHpiBoolT &enables ) {

    enables = m_events_enabled;

    return SA_OK;
}


/**
 * HPI function saHpiSensorEventEnableSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 *
 * @param enables address of event enable value to be set internally 
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::SetEventEnables( const SaHpiBoolT &enables ) {

    if ( EventCtrl() == SAHPI_SEC_READ_ONLY )
        return SA_ERR_HPI_READ_ONLY;

    if ( m_events_enabled == enables )
        return SA_OK;

    m_events_enabled = enables;

    CreateEnableChangeEvent();

    return SA_OK;
}


/**
 * HPI function saHpiSensorEventMasksGet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param AssertEventMask address of mask to be filled with assertion mask 
 * @param  DeassertEventMask address of mask to be filled with deassertion mask
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::GetEventMasks( SaHpiEventStateT &AssertEventMask,
                            SaHpiEventStateT &DeassertEventMask) {

    if (&AssertEventMask) AssertEventMask = m_assert_mask;
    if (&DeassertEventMask) DeassertEventMask = m_deassert_mask;

    return SA_OK;
}

/**
 * HPI function saHpiSensorEventMasksSet()
 * 
 * See also the description of the function inside the specification or header file.
 * Copying the internal reading values (if a read is allowed).
 * 
 * @param act address of variable which includes the action to be done
 * @param AssertEventMask address of variable which include new assertion mask data
 * @param DeassertEventMask address of variable which include new deassertion mask data
 * 
 * @return HPI return code
 **/
SaErrorT NewSimulatorSensor::SetEventMasks( const SaHpiSensorEventMaskActionT &act,
                                            SaHpiEventStateT                   &AssertEventMask,
                                            SaHpiEventStateT                   &DeassertEventMask ) {
	
   if ( EventCtrl() != SAHPI_SEC_PER_EVENT )
      return SA_ERR_HPI_READ_ONLY;

   if ( AssertEventMask == SAHPI_ALL_EVENT_STATES )
      AssertEventMask = EventStates();

   if ( DeassertEventMask == SAHPI_ALL_EVENT_STATES )
      DeassertEventMask = EventStates();

   if ( act == SAHPI_SENS_ADD_EVENTS_TO_MASKS ) {
      if ((( AssertEventMask & ~EventStates() ) != 0 )
       || (( DeassertEventMask & ~EventStates() ) != 0 ))
         return SA_ERR_HPI_INVALID_DATA;
   }

   SaHpiEventStateT save_assert_mask = m_assert_mask;
   SaHpiEventStateT save_deassert_mask = m_deassert_mask;
   
   if ( act == SAHPI_SENS_ADD_EVENTS_TO_MASKS ) {
      m_assert_mask   |= AssertEventMask;
      m_deassert_mask |= DeassertEventMask;
    
   } else if ( act == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS ) {
      m_assert_mask   &= (AssertEventMask ^ SAHPI_ALL_EVENT_STATES);
      m_deassert_mask &= (DeassertEventMask ^ SAHPI_ALL_EVENT_STATES);
    
   } else 
      return SA_ERR_HPI_INVALID_PARAMS;
    
   stdlog << "SetEventMasks sensor " << m_sensor_record.Num << " assert " << m_assert_mask << " deassert " << m_deassert_mask << "\n";

   if (( save_assert_mask == m_assert_mask )
    && ( save_deassert_mask == m_deassert_mask ))
      return SA_OK;

   CreateEnableChangeEvent();

   return SA_OK;
}


/**
 * Generate an event if the enabling / mask of the sensor was changed 
 **/
void NewSimulatorSensor::CreateEnableChangeEvent()
{
  NewSimulatorResource *res = Resource();
  if( !res )
     {
       stdlog << "CreateEnableChangeEvent: No resource !\n";
       return;
     }

  oh_event *e = (oh_event *)g_malloc0( sizeof( struct oh_event ) );
  
  e->event.EventType = SAHPI_ET_SENSOR_ENABLE_CHANGE;

  SaHpiRptEntryT *rptentry = oh_get_resource_by_id( res->Domain()->GetHandler()->rptcache, res->ResourceId() );
  SaHpiRdrT *rdrentry = oh_get_rdr_by_id( res->Domain()->GetHandler()->rptcache, res->ResourceId(), m_record_id );

  if ( rptentry )
      e->resource = *rptentry;
  else
      e->resource.ResourceCapabilities = 0;

  if ( rdrentry )
      e->rdrs = g_slist_append(e->rdrs, g_memdup(rdrentry, sizeof(SaHpiRdrT)));
  else
      e->rdrs = NULL;

  // hpi events
  e->event.Source    = res->ResourceId();
  e->event.EventType = SAHPI_ET_SENSOR_ENABLE_CHANGE;
  e->event.Severity  = SAHPI_INFORMATIONAL;
  
  oh_gettimeofday(&e->event.Timestamp);
  
  // sensor enable event
  SaHpiSensorEnableChangeEventT *se = &e->event.EventDataUnion.SensorEnableChangeEvent;
  se->SensorNum     = m_sensor_record.Num;
  se->SensorType    = Type();
  se->EventCategory = EventCategory();
  se->SensorEnable  = m_enabled;
  se->SensorEventEnable = m_events_enabled;
  se->AssertEventMask   = m_assert_mask;
  se->DeassertEventMask = m_deassert_mask;
  
  stdlog << "NewSimulatorSensor::CreateEnableChangeEvent OH_ET_HPI Event enable change resource " << res-> ResourceId() << "\n";
  res->Domain()->AddHpiEvent( e );

  return;
}

