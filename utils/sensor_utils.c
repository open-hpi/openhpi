/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *	David Judkovics <djudkovi@us.ibm.com>
 *	Renier Morales <renierm@us.ibm.com>
 *
 */
 
#include <string.h>
#include <stdio.h>
#include <glib.h>

#include <oh_error.h>
#include <sensor_utils.h>
 
static state_category_string state_string[] = {
  {SAHPI_EC_UNSPECIFIED, SAHPI_ES_UNSPECIFIED, "UNSPECIFIED"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MINOR, "LOWER_MINOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MAJOR, "LOWER_MAJOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_CRIT, "LOWER_CRIT"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MINOR, "UPPER_MINOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MAJOR, "UPPER_MAJOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_CRIT, "UPPER_CRIT"},
  {SAHPI_EC_USAGE, SAHPI_ES_IDLE, "IDLE"},
  {SAHPI_EC_USAGE, SAHPI_ES_ACTIVE, "ACTIVE"},
  {SAHPI_EC_USAGE, SAHPI_ES_BUSY, "BUSY"},
  {SAHPI_EC_STATE, SAHPI_ES_STATE_DEASSERTED, "STATE_DEASSERTED"},
  {SAHPI_EC_STATE, SAHPI_ES_STATE_ASSERTED, "STATE_ASSERTED"},
  {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_DEASSERT,
   "PRED_FAILURE_DEASSERT"},
  {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_ASSERT, "PRED_FAILURE_ASSERT"},
  {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_NOT_EXCEEDED, "LIMIT_NOT_EXCEEDED"},
  {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_EXCEEDED, "LIMIT_EXCEEDED"},
  {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_MET, "PERFORMANCE_MET"},
  {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_LAGS, "PERFORMANCE_LAGS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_OK, "OK"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_OK, "MINOR_FROM_OK"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_LESS, "MAJOR_FROM_LESS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL_FROM_LESS, "CRITICAL_FROM_LESS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_MORE, "MINOR_FROM_MORE"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_CRITICAL, "MAJOR_FROM_CRITICAL"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL, "CRITICAL"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MONITOR, "MONITOR"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_INFORMATIONAL, "INFORMATIONAL"},
  {SAHPI_EC_PRESENCE, SAHPI_ES_ABSENT, "ABSENT"},
  {SAHPI_EC_PRESENCE, SAHPI_ES_PRESENT, "PRESENT"},
  {SAHPI_EC_ENABLE, SAHPI_ES_DISABLED, "DISABLED"},
  {SAHPI_EC_ENABLE, SAHPI_ES_ENABLED, "ENABLED"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_RUNNING, "RUNNING"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_TEST, "TEST"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_OFF, "POWER_OFF"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_ON_LINE, "ON_LINE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_LINE, "OFF_LINE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_DUTY, "OFF_DUTY"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_DEGRADED, "DEGRADED"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_SAVE, "POWER_SAVE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_INSTALL_ERROR, "INSTALL_ERROR"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_FULLY_REDUNDANT, "FULLY_REDUNDANT"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST, "REDUNDANCY_LOST"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED, "REDUNDANCY_DEGRADED"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES,
   "REDUNDANCY_LOST_SUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES,
   "NON_REDUNDANT_SUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES,
   "NON_REDUNDANT_INSUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL,
   "REDUNDANCY_DEGRADED_FROM_FULL"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON,
   "REDUNDANCY_DEGRADED_FROM_NON"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_00, "STATE_00"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_01, "STATE_01"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_02, "STATE_02"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_03, "STATE_03"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_04, "STATE_04"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_05, "STATE_05"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_06, "STATE_06"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_07, "STATE_07"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_08, "STATE_08"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_09, "STATE_09"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_10, "STATE_10"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_11, "STATE_11"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_12, "STATE_12"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_13, "STATE_13"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_14, "STATE_14"}
};

#define STATESTRING_VALUE_DELIMITER ", "
#define STATESTRING_VALUE_DELIMITER_LENGTH 2
#define STATESTRING_MAX_LENGTH 1000
#define STATESTRING_MAX_ENTRIES 63

char *format[] = {
	"%d",
	"%u",
	"%f"
};

int
build_state_string (SaHpiEventCategoryT category,
		    SaHpiEventStateT state,
		    unsigned char *str, 
		    size_t * len, 
		    size_t max_len)
{

  char *temp;
  size_t idx = 0;
  size_t temp_len;
  int i;

  int rc = SA_OK;

  if (!str || !len) return SA_ERR_HPI_ERROR;
  
  *len = 0;
  idx = 0;
  temp = (char *) g_malloc0 (STATESTRING_MAX_LENGTH);
  if (temp == NULL) {
	  dbg("Memory Fault: temp");
	  return SA_ERR_HPI_ERROR;
  }

  for (i = 0; i < STATESTRING_MAX_ENTRIES; i++)
    {
      /* snmp_log (LOG_INFO, "How about this one? : %d %d\n", state_string[i].category,
         category); */
      if (state_string[i].category == category)
	{
	  /*
	     snmp_log (LOG_INFO, "Matched : %d %d\n", state_string[i].category,
	     category);
	   */
	  /* Found category, time to match states. */
	  /* Match the right states */
	  if ((state_string[i].state & state) == state_string[i].state)
	    {
	      /* Found it 
	         snmp_log (LOG_INFO, "Matched state: %d %d\n", state_string[i].state,state);
	       */
	      temp_len = strlen (state_string[i].str);
	      if (idx + temp_len + STATESTRING_VALUE_DELIMITER_LENGTH >
		  max_len) {
		  dbg("rc = ERR_MEMORY_FAULT");
		  rc = SA_ERR_HPI_ERROR;
		  break;
	      }
	      memcpy (temp + idx, state_string[i].str, temp_len);
	      idx = idx + temp_len;
	      memcpy (temp + idx, STATESTRING_VALUE_DELIMITER,
		      STATESTRING_VALUE_DELIMITER_LENGTH);
	      idx = idx + STATESTRING_VALUE_DELIMITER_LENGTH;
	    }
	}
    }

  if (idx > 0)
    idx = idx - STATESTRING_VALUE_DELIMITER_LENGTH;

  if (idx < max_len)
    temp[idx] = 0x00;
  else
    temp[max_len] = 0x00;


  memcpy (str, temp, idx);
  *len = idx;

  g_free (temp);
  temp = NULL;
  return rc;
}

int
build_state_value (unsigned char *str, size_t len, SaHpiEventStateT * state)
{

  int rc = SA_OK;
  char *s = NULL;
  char *delim = NULL;
  char *tok = NULL;
  int i = 0;

  if (!str || !state) return SA_ERR_HPI_ERROR;
  
  if (len == 0) {
	  dbg("Nothing to do, len 0, returning");
	  return(SA_OK);
  }

  s = (char *) g_malloc0 (len);
  if (s == NULL){
    dbg("build_state_value: Memory Fault, s");
    return SA_ERR_HPI_ERROR;
  }

  delim = (char *) g_malloc0 (STATESTRING_VALUE_DELIMITER_LENGTH);
  if (delim == NULL) {
      g_free (s);
      dbg("build_state_value: Memory Fault, delim");
      return SA_ERR_HPI_ERROR;
  }

  memcpy (s, str, len);
  s[len] = 0x00;
  memcpy (delim, STATESTRING_VALUE_DELIMITER,
	  STATESTRING_VALUE_DELIMITER_LENGTH);
  delim[STATESTRING_VALUE_DELIMITER_LENGTH] = 0x00; 
  tok = strtok (s, delim);
  while (tok != NULL)
    {

      /*  snmp_log(LOG_INFO,"Tok: [%s]\n", tok); */
      for (i = 0; i < STATESTRING_MAX_ENTRIES; i++)
	{
	  if (strncasecmp
	      (tok, state_string[i].str, strlen (state_string[i].str)) == 0)
	    {
	      /*
	         snmp_log(LOG_INFO,"Matched: %X [%s] = [%s]\n", 
	         state_string[i].state,
	         state_string[i].str,
	         tok);
	       */
	      *state = *state + state_string[i].state;
	    }
	}
      tok = strtok ((char *)NULL, delim);
    }

  g_free (s);
  g_free (delim);
  return rc;
}


#if 0

int
build_reading_strings (SaHpiSensorReadingT * reading,
		       SaHpiEventCategoryT sensor_category,
		       long *values_present,
		       long *raw_reading,
		       unsigned char *interpreted_reading,
		       size_t * interpreted_reading_len,
		       size_t interpreted_reading_max,
		       long *sensor_status,
		       unsigned char *event_status,
		       size_t * event_status_len, size_t event_status_max)
{

//  char format[SENSOR_READING_MAX_LEN];
  size_t len;

  if (!reading || !values_present || !raw_reading || !interpreted_reading ||
      !interpreted_reading_len || !sensor_status || !event_status ||
      !event_status_len)
	  return SA_ERR_HPI_ERROR;
  
  if (interpreted_reading_len) 
          *interpreted_reading_len = 0;
  
  if (event_status_len)
          *event_status_len = 0;
  if (event_status)
          *event_status = 0;
  

  /* 
   *       SaHpiSensorInterpretedT     Interpreted;
   * is always converted to a string buffer.
   */

  if (reading->Interpreted.Type == SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER)
  {
          if (interpreted_reading) {
                  memcpy (interpreted_reading,
                          &reading->Interpreted.Value.SensorBuffer,
                          SAHPI_SENSOR_BUFFER_LENGTH);
	    /*
	     * Old code: 
         *interpreted_reading_len = SAHPI_SENSOR_BUFFER_LENGTH;
	     * Check to see how much of the string is actually 0x00 and 
	     * do not count those
	     */
                  len = strlen(reading->Interpreted.Value.SensorBuffer);
                  *interpreted_reading_len = ( len < SAHPI_SENSOR_BUFFER_LENGTH ) ? len : SAHPI_SENSOR_BUFFER_LENGTH;
          }
  }
  
  else
  {
//	  memset (&format, 0x00, SENSOR_READING_MAX_LEN);
          /* Setting up the format  - %d or %u or %f .. etc */
/*	  switch (reading->Interpreted.Type)
	    {
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
	      strncpy (format,
		       SENSOR_READING_SIGNED_INT,
		       SENSOR_READING_SIGNED_INT_LEN);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
	      strncpy (format,
		       SENSOR_READING_UNSIGNED_INT,
		       SENSOR_READING_UNSIGNED_INT_LEN);

	      break;

	    case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
	      strncpy (format,
		       SENSOR_READING_FLOAT, 
		       SENSOR_READING_FLOAT_LEN);
	      break;

	    default:
	      break;
	    }
*/	  /* Done with setting up the format. Parsing the value.
	   * This could be done using a void pointer, but where 
	   * would I put the type information? Cast it back to its type?
	   *
	   * Any ideas?
	   */
          if (interpreted_reading) {
	    switch (reading->Interpreted.Type)
	      {
	      case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
		*interpreted_reading_len =
		  snprintf (interpreted_reading,
			    interpreted_reading_max,
			    "%d",/*format[0],*/ 
			    reading->Interpreted.Value.SensorInt8);
		break;
	      case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
		*interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%d", /*format[0], */
			  reading->Interpreted.Value.SensorInt16);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%d", /*format[0], */
			  reading->Interpreted.Value.SensorInt32);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%u", /*format[1],*/ 
			  reading->Interpreted.Value.SensorUint8);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%u", /*format[1],*/ 
			  reading->Interpreted.Value.SensorUint16);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%u", /*format[1],*/ 
			  reading->Interpreted.Value.SensorUint32);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  "%f", /*format[2], */
			  reading->Interpreted.Value.SensorFloat32);
	      break;

	    default:
	      break;
	    }

	  *interpreted_reading_len =
	    (*interpreted_reading_len >
	     interpreted_reading_max) ? interpreted_reading_max :
	    *interpreted_reading_len;

	  }
	}
    }
  if (reading->ValuesPresent & SAHPI_SRF_EVENT_STATE)
    {
      if (sensor_status)
	*sensor_status = reading->EventStatus.SensorStatus + 1;
      if (event_status || event_status_len) 
	build_state_string (sensor_category,
			    reading->EventStatus.EventStatus,
			    event_status, event_status_len, event_status_max);
    }
  return SA_OK;
} 
#endif
