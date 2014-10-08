/** 
 * @file    new_sim_file_sensor.cpp
 *
 * The file includes the class for parsing the sensor data:\n
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
 
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <SaHpi.h>

#include "new_sim_file_rdr.h"
#include "new_sim_file_sensor.h"
#include "new_sim_file_util.h"
#include "new_sim_resource.h"
#include "new_sim_rdr.h"
#include "new_sim_sensor.h"
#include "new_sim_sensor_common.h"
#include "new_sim_sensor_threshold.h"

#include <oh_error.h>

/**
 * Constructor
 **/
NewSimulatorFileSensor::NewSimulatorFileSensor( GScanner *scanner ) 
                       : NewSimulatorFileRdr ( scanner ),
                       m_sensor_event_state ( 0 ),
                       m_sensor_event_amask ( 0 ), 
                       m_sensor_event_dmask ( 0 ), 
                       m_sensor_enabled ( SAHPI_TRUE ), 
                       m_sensor_event_enabled( SAHPI_TRUE ) { 
   
   m_sensor_rec = &m_rdr.RdrTypeUnion.SensorRec;
   memset ( &m_sensor_data, 0, sizeof( SaHpiSensorReadingT ));
   memset ( &m_sensor_thresholds, 0, sizeof( SaHpiSensorThresholdsT ));
}


/**
 * Destructor
 **/
NewSimulatorFileSensor::~NewSimulatorFileSensor() {
}


/**
 * Parse inside the \c SENSOR_TOKEN_HANDLER the \c RDR_DETAIL_TOKEN_HANDLER
 *
 * Depend on which sensor type is parsed several help methods are called. Startpoint is the
 * \c RDR_DETAIL_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY of the \c 
 * SENSOR_TOKEN_HANDLER.
 *  
 * @param res Pointer on the resource which includes the sensor
 *
 * @return Pointer on a new Rdr entry
 **/
NewSimulatorRdr * NewSimulatorFileSensor::process_token( NewSimulatorResource *res) {
   bool success = true;
   char *sensorfield;
   
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token.");
      return NULL;
   }
   m_depth++;
   
   while ( (m_depth > 0) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {

         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
       	    sensorfield = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(sensorfield, "Num")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_sensor_rec->Num = m_scanner->value.v_int;
         	   
            } else if (!strcmp(sensorfield, "Type")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_sensor_rec->Type = (SaHpiSensorTypeT) m_scanner->value.v_int;
         	   
         	} else if (!strcmp(sensorfield, "Category")) {
         	   if (cur_token == G_TOKEN_INT)
            	      m_sensor_rec->Category = m_scanner->value.v_int;

         	} else if (!strcmp(sensorfield, "EnableCtrl")) {
            	   if (cur_token == G_TOKEN_INT) 
         	      m_sensor_rec->EnableCtrl = m_scanner->value.v_int;

         	} else if (!strcmp(sensorfield, "EventCtrl")) {
            	   if (cur_token == G_TOKEN_INT) 
         	      m_sensor_rec->EventCtrl = ( SaHpiSensorEventCtrlT ) m_scanner->value.v_int;
         	
         	} else if (!strcmp(sensorfield, "Events")) {
            	   if (cur_token == G_TOKEN_INT) 
         	      m_sensor_rec->Events = m_scanner->value.v_int;
         	
         	} else if (!strcmp(sensorfield, "DataFormat")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse sensor rdr entry - Missing left curly at DataFormat");
         	      success = false;
         	   } else {
         	      success = process_dataformat( &m_sensor_rec->DataFormat );
         	   }

            } else if (!strcmp(sensorfield, "ThresholdDefn")) {
         	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing parse sensor rdr entry - Missing left curly at ThresholdDefn");
         	      success = false;
         	   } else {
         	      success = process_thresholddef( &m_sensor_rec->ThresholdDefn );
         	   }
         	   
         	} else if (!strcmp(sensorfield, "Oem")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_sensor_rec->Oem = m_scanner->value.v_int;
         	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", sensorfield);
               success = false;	
            }
            
            break;
            
         case SENSOR_DATA_TOKEN_HANDLER:
            success = process_sensor_data_token();
         
            break;   
         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   if ( success ) { 
      if ( m_sensor_rec->ThresholdDefn.IsAccessible ) {
         return ( new NewSimulatorSensorThreshold(res, m_rdr, m_sensor_data, m_sensor_event_state, 
                                            m_sensor_event_amask, m_sensor_event_dmask,
                                            m_sensor_thresholds, m_sensor_enabled, 
                                            m_sensor_event_enabled) );
      
      } else {
         return ( new NewSimulatorSensorCommon(res, m_rdr, m_sensor_data, m_sensor_event_state, 
                                            m_sensor_event_amask, m_sensor_event_dmask,
                                            m_sensor_enabled, m_sensor_event_enabled) );
      }
   }

   return NULL;

}


/**
 * Parse the DataFormat structure of sensor record
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @param dataformat Pointer on SaHpiSensorDataFormatT to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileSensor::process_dataformat( SaHpiSensorDataFormatT *dataformat ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
         
            if (!strcmp(field, "IsSupported")) {
               if (cur_token == G_TOKEN_INT)
                  dataformat->IsSupported = m_scanner->value.v_int;

         	} else if (!strcmp(field, "ReadingType")) {
               if (cur_token == G_TOKEN_INT)
                  dataformat->ReadingType = ( SaHpiSensorReadingTypeT ) m_scanner->value.v_int;
                  	
            } else if (!strcmp(field, "BaseUnits")) {
         	   if (cur_token == G_TOKEN_INT)
                  dataformat->BaseUnits = ( SaHpiSensorUnitsT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ModifierUnits")) {
         	   if (cur_token == G_TOKEN_INT)
                  dataformat->ModifierUnits = ( SaHpiSensorUnitsT ) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ModifierUse")) {
         	   if (cur_token == G_TOKEN_INT)
                  dataformat->ModifierUse = ( SaHpiSensorModUnitUseT ) m_scanner->value.v_int;
                  	
            } else if (!strcmp(field, "Percentage")) {
         	   if (cur_token == G_TOKEN_INT)
                  dataformat->Percentage = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Range")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor dataformat - Missing left curly at DataFormat.Range");
         	      success = false;
         	   } else {
         	      success = process_dataformat_range( &dataformat->Range );
         	   }
         	   
            } else if (!strcmp(field, "AccuracyFactor")) {
         	   if (cur_token == G_TOKEN_FLOAT)
                  dataformat->AccuracyFactor = m_scanner->value.v_float;
                  
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown DataFormat.Range field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break;
      }   
   }
   
   return success;	
}


/**
 * Parse the DataFormat.Range structure of sensor record
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY 
 * of DataFormat.Range.
 *  
 * @param dataformat Pointer on SaHpiSensorRangeT to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileSensor::process_dataformat_range( SaHpiSensorRangeT *datarange ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "Flags")) {
               if (cur_token == G_TOKEN_INT)
                  datarange->Flags = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Max")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at DataFormat.Range.Max");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &datarange->Max );
         	   }
            
            } else if (!strcmp(field, "Min")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at DataFormat.Range.Min");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &datarange->Min );
         	   }
            
            } else if (!strcmp(field, "Nominal")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at DataFormat.Range.Nominal");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &datarange->Nominal );
         	   }
         	
         	} else if (!strcmp(field, "NormalMax")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at DataFormat.Range.NormalMax");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &datarange->NormalMax );
         	   }
         	   
         	} else if (!strcmp(field, "NormalMin")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at DataFormat.Range.NormalMin");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &datarange->NormalMin );
         	   }
         	   
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break; 
      }  
   }
   
   return success;	
}


/**
 * Parse the ThresholdDefn structure of sensor record
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY 
 * of DataFormat.Range.
 *  
 * @param thresdef Pointer on SaHpiSensorThdDefnT to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileSensor::process_thresholddef( SaHpiSensorThdDefnT *thresdef ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "IsAccessible")) {
               if (cur_token == G_TOKEN_INT)
                  thresdef->IsAccessible = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "ReadThold")) {
               if (cur_token == G_TOKEN_INT)
                  thresdef->ReadThold = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "WriteThold")) {
               if (cur_token == G_TOKEN_INT)
                  thresdef->WriteThold = m_scanner->value.v_int;
                  
            } else if (!strcmp(field, "Nonlinear")) {
               if (cur_token == G_TOKEN_INT)
                  thresdef->Nonlinear = m_scanner->value.v_int;
                        
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break; 
      }  
   }
   
   return success;	
}


/**
 * Parse inside the \c RDR_DETAIL_TOKEN_HANDLER the \c SENSOR_DATA_TOKEN_HANDLER
 *
 * Depend on which sensor type is parsed several help methods are called. Startpoint is the
 * \c SENSOR_DATA_TOKEN_HANDLER. Endpoint is the last \c G_TOKEN_RIGHT_CURLY.
 *  
 * @return success
 **/
bool NewSimulatorFileSensor::process_sensor_data_token( ) {
   bool success = true;
   int start = m_depth;
   char *field;
   
   guint cur_token = g_scanner_get_next_token(m_scanner);
   
   if (cur_token != G_TOKEN_LEFT_CURLY) {
   	  err("Processing parse configuration: Expected left curly token after SENSOR_DATA_TOKEN_HANDLER.");
      return false;
   }
   m_depth++;
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {

         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
       	    field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse rdr entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "SensorEnable")) {
            	   if (cur_token == G_TOKEN_INT)
         	      m_sensor_enabled = ( SaHpiBoolT ) m_scanner->value.v_int;
         	   
            } else if (!strcmp(field, "SensorEventEnable")) {
            	   if (cur_token == G_TOKEN_INT)
         	       m_sensor_event_enabled = ( SaHpiBoolT ) m_scanner->value.v_int;
         	   
         	} else if (!strcmp(field, "EventState")) {
         	   if (cur_token == G_TOKEN_INT)
            	      m_sensor_event_state = ( SaHpiEventStateT ) m_scanner->value.v_int;

         	} else if (!strcmp(field, "SensorReading")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at SensorReading");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &m_sensor_data );
         	   }
            
            } else if (!strcmp(field, "SensorThresholds")) {     
               if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at SensorThresholds");
         	      success = false;
         	   } else {
         	      success = process_sensor_thresholds( &m_sensor_thresholds );
         	   }
         	
         	} else if (!strcmp(field, "AssertEventMask")) {
         	   if (cur_token == G_TOKEN_INT)
            	      m_sensor_event_amask = ( SaHpiEventStateT ) m_scanner->value.v_int;
            	
            } else if (!strcmp(field, "DeassertEventMask")) {
         	   if (cur_token == G_TOKEN_INT)
            	      m_sensor_event_dmask = ( SaHpiEventStateT ) m_scanner->value.v_int;
            	      	
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing parse rdr entry: Unknown token");
            success = false;
            break;   
      }
   }
   
   return success;
}

/**
 * Parse the ThresholdsT structure 
 * 
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY 
 * of the ThresholdsT structure.
 *  
 * @param thres Pointer on SaHpiSensorThresholdsT to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileSensor::process_sensor_thresholds( SaHpiSensorThresholdsT *thres ) {
   bool success = true;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing parse thresholds entry: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (!strcmp(field, "LowCritical")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold LowCritical");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->LowCritical );
         	   }
            
            } else if (!strcmp(field, "LowMajor")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold LowMajor");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->LowMajor );
         	   }
                  
            } else if (!strcmp(field, "LowMinor")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold LowMinor");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->LowMinor );
         	   }
         	
         	} else if (!strcmp(field, "UpCritical")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold UpCritical");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->UpCritical );
         	   }
         	
         	} else if (!strcmp(field, "UpMajor")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold UpMajor");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->UpMajor );
         	   }

            } else if (!strcmp(field, "UpMinor")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold UpMinor");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->UpMinor );
         	   }
            
            } else if (!strcmp(field, "PosThdHysteresis")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold PosThdHysteresis");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->PosThdHysteresis );
         	   }
            
            } else if (!strcmp(field, "NegThdHysteresis")) {
            	   if (cur_token != G_TOKEN_LEFT_CURLY) {
         	      err("Processing sensor - Missing left curly at Threshold NegThdHysteresis");
         	      success = false;
         	   } else {
         	      success = process_sensor_reading( &thres->NegThdHysteresis );
         	   }
         	   
            } else {
               // Unknown Token 
               err("Processing parse rdr entry: Unknown Rdr field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break; 
      }  
   }
   
   return success;	
}


/**
 * Parse the SensorReading structure
 *
 * Startpoint is the \c G_TOKEN_LEFT_CURLY. Endpoint is the last \c G_TOKEN_RIGHT_CURLY 
 * of SensorReading
 *  
 * @param sensorreading Pointer on SaHpiSensorReadingT to be filled
 * @return bool value success
 * 
 **/
bool NewSimulatorFileSensor::process_sensor_reading( SaHpiSensorReadingT *sensorreading ) {
   bool success = true;
   bool negative = false;
   int start = m_depth;
   char *field;
   guint cur_token;
   
   m_depth++; // Left Curly -- our startpoint   
   
   while ( (m_depth > start) && success ) {
      cur_token = g_scanner_get_next_token(m_scanner);
      
      switch (cur_token) {
         case G_TOKEN_EOF:  
            err("Processing parse rpt entry: File ends too early");
       	    success = false;
       	    break;
       	    
       	 case G_TOKEN_RIGHT_CURLY:
            m_depth--;
            break;
          
         case G_TOKEN_LEFT_CURLY:
            m_depth++;
            break;

         case G_TOKEN_STRING:
            negative = false;
            field = g_strdup(m_scanner->value.v_string);
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if (cur_token != G_TOKEN_EQUAL_SIGN) {
               err("Processing sensorreading: Missing equal sign");
               success = false;
            }
            cur_token = g_scanner_get_next_token(m_scanner);
            
            if ( cur_token == '-' ) {
               negative = !negative;
               cur_token = g_scanner_get_next_token(m_scanner);	
            }
            
            if (!strcmp(field, "IsSupported")) {
               if (cur_token == G_TOKEN_INT)
                  sensorreading->IsSupported = m_scanner->value.v_int;
            
            } else if (!strcmp(field, "Type")) {
            	   if (cur_token == G_TOKEN_INT)
                  sensorreading->Type = (SaHpiSensorReadingTypeT) m_scanner->value.v_int;
            
            } else if (!strcmp(field, "value.SensorInt64")) {
            	   if (cur_token == G_TOKEN_INT) {
                  sensorreading->Value.SensorInt64 = m_scanner->value.v_int;
                  if (negative)
                     sensorreading->Value.SensorInt64 = -sensorreading->Value.SensorInt64;
            	   }
                  
            } else if (!strcmp(field, "value.SensorUint64")) {
               if (cur_token == G_TOKEN_INT)
                  sensorreading->Value.SensorUint64 = m_scanner->value.v_int;
            		
            } else if (!strcmp(field, "value.SensorFloat64")) {
               if (cur_token == G_TOKEN_FLOAT) {
                  sensorreading->Value.SensorFloat64 = m_scanner->value.v_float;
                  if (negative)
                     sensorreading->Value.SensorFloat64 = -sensorreading->Value.SensorFloat64;
               }
               	
            } else if (!strcmp(field, "value.SensorBuffer")) {
            	   if (cur_token == G_TOKEN_STRING)
            	      success = process_hexstring( SAHPI_SENSOR_BUFFER_LENGTH,
            	                                   g_strdup(m_scanner->value.v_string),
            	                                   &(sensorreading->Value.SensorBuffer[0]) );
            	   /**
            	   char *val_str = NULL;
            	   guint val_uint = 0;
            	   val_str = g_strdup(m_scanner->value.v_string);               
               int len = strlen(val_str);
               int i;
               
               if (val_str == NULL) { 
                  err("Processing parse sensorreading: Wrong SensorBuffer\n");
               } else if (len % 2) {
         	      err("Processing parse sensorreading: Wrong SensorBuffer length\n");
               } else {
         	      stdlog << "DBG: SensorReading: Parsing SensorBuffer ";
         	      for (i = 0; (i < SAHPI_SENSOR_BUFFER_LENGTH) || (i*2<len); i++) {
         	         sscanf(val_str, "%02X", &val_uint);
         	         sensorreading->Value.SensorBuffer[i] = static_cast<SaHpiUint8T>(val_uint);
         	         
         	         val_str++;
         	         val_str++;
         	         stdlog << sensorreading->Value.SensorBuffer[i] << " ";
         	      }
         	      stdlog << "\n";
               }
               **/	
            } else {
               // Unknown Token 
               err("Processing sensorreading entry: Unknown field %s", field);
               success = false;	
            }
            
            break;
            
         default: 
            err("Processing data format: Unknown token");
            success = false;
            break; 
      }  
   }
   
   return success;	
}

