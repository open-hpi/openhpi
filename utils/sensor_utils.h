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
 *
 */
#include <SaHpi.h>
 
 /*
 * Structure used by build_state_string routine.
 */
#ifndef STATE_CATEGORY_STRING_HAVE
#define STATE_CATEGORY_STRING_HAVE
typedef struct state_category_string_ 
{
    SaHpiEventCategoryT category;
    SaHpiEventStateT state;
    unsigned char *str;
} state_category_string;
#endif	

#define SENSOR_READING_UNSIGNED_INT "%u"
#define SENSOR_READING_SIGNED_INT "%d"
#define SENSOR_READING_UNSIGNED_INT_LEN 2
#define SENSOR_READING_SIGNED_INT_LEN 2


#define SENSOR_READING_FLOAT "%f"
#define SENSOR_READING_FLOAT_LEN 2

#define SENSOR_READING_MAX_LEN 256


/*
 * Construct a string value from a category and state 
 * parameter.
 *
 * @param category [IN] The category in which this state has to be converted by.
 * @param state [IN] The state of the event.
 * @param str [OUT] The new value in string format.
 * @param len [OUT] length of the new string.
 * @param max_len [IN] max length that can put in data.
 *
 * @return AGENT_ERR_NOERROR - operation went ok
 * @return AGENT_ERR_MEMORY_FAULT - Not enough memory to complete the operation.
 */
int build_state_string (SaHpiEventCategoryT category,
			SaHpiEventStateT state,
			unsigned char *str,
			size_t *len,
			size_t max_len);
			

/*
 * Construct from the string value the correct state value.
 *
 * @param str [IN] The string value containing the state values.
 * @param len [IN] Length of the string value.
 * @param state [OUT] the state value
 *
 * @return AGENT_ERR_NOERROR - operation went ok
 */
int build_state_value (unsigned char *str,
		       size_t len,
		       SaHpiEventStateT *state);

/*
 * Build four different values:
 * - ValuesPresent
 * - Raw reading (if applicable)
 * - Interpreted reading (if applicable)
 * - Event Status (if applicable)
 *
 * These four values are built from the SaHpiSensorReadingT structure 
 *
 * @param reading [IN] SaHpiSensorReadingT with sensor readings.
 * @param values_present [OUT] Values Present value with +1 added.
 * @param raw_reading [OUT] raw reading (if applicable)
 * @param interpreted_reading [OUT] the interpreted reading stored in
 *                                   char array.
 * @param interpreted_reading_len [OUT] the length of the array that has been
 *  filled.
 * @param interpreted_reading_max [IN] Maximum length allowed in interpreted
 * char array.
 * @param sense_status [OUT] The status of the sensor  +1
 * @param event_status [OUT] the status of the event of the sensor in a
 * char array.
 * @param event_status_len [OUT] the length of the event char array.
 * @param event_status_max [IN] the maximum length of the event char array.
 *
 */
int build_reading_strings (SaHpiSensorReadingT *reading,
			   SaHpiEventCategoryT sensor_category,
			   long *values_present,
			   long *raw_reading,
			   unsigned char *interpreted_reading,
			   size_t *interpreted_reading_len,
			   size_t  interpreted_reading_max,
			   long *sensor_status,
			   unsigned char *event_status,
			   size_t *event_status_len,
			   size_t event_status_max);
 

