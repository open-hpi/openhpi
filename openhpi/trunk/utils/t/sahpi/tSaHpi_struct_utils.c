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
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <SaHpi_struct_utils.h>

#define UNDEFINED_MANUFACTURER  -1
#define BAD_CAT -1
#define BAD_EVENT 0xFFFF

int main(int argc, char **argv) 
{
	const char *expected_str;
	const char *str;
	char       buffer[512];
	int        buffer_size = 512;
	SaErrorT   expected_err, err;

	/************************** 
	 * SaHpiTimeT2str testcases
         **************************/
	{
		/* SaHpiTimeT2str: test initial time testcase */
		/* Can't look for a specific expected string, since it depends on locale */
		err = SaHpiTimeT2str(0, buffer, buffer_size);

		if (err != 0) {
                        printf("tSaHpi_struct_utils Error! SaHpiTimeT2str: test initial time testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiTimeT2str: Null buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		err = SaHpiTimeT2str(0, 0, buffer_size);
		
                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiTimeT2str: Null buffer testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiTimeT2str: buffer too small testcase */
		expected_err = SA_ERR_HPI_OUT_OF_SPACE;
		
		err = SaHpiTimeT2str(0, buffer, 0);
		
                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiTimeT2str: buffer too small testcase failed. Error=%d\n", err);
                        return -1;
                }
	}
	
	/************************************ 
	 * SaHpiManufacturerIdT2str testcases
         ************************************/
	{
		SaHpiManufacturerIdT mid;
		
        	/* SaHpiManufacturerIdT2str: SAHPI_MANUFACTURER_ID_UNSPECIFIED testcase */
		expected_str = "SAHPI_MANUFACTURER_ID_UNSPECIFIED";
		mid = SAHPI_MANUFACTURER_ID_UNSPECIFIED;

		str = SaHpiManufacturerIdT2str(mid); 
		
                if (strcmp(expected_str, str)) {
                        printf("tSaHpi_struct_utils Error! SaHpiManufacturerIdT2str: SAHPI_MANUFACTURER_ID_UNSPECIFIED testcase failed. %s= received string\n", str);
                        return -1;
                }

                /* SaHpiManufacturerIdT2str: IBM testcase */
		expected_str = "IBM";
		mid = 2;

		str = SaHpiManufacturerIdT2str(mid); 
		
                if (strcmp(expected_str, str)) {
                        printf("tSaHpi_struct_utils Error! SaHpiManufacturerIdT2str: IBM testcase failed. %s= received string\n", str);
                        return -1;
                }

                /* SaHpiManufacturerIdT2str: Undefined manufacturer testcase */
		expected_str = "Undefined Manufacturer";
		mid = UNDEFINED_MANUFACTURER;

		str = SaHpiManufacturerIdT2str(mid);
		
                if (strcmp(expected_str, str)) {
                        printf("tSaHpi_struct_utils Error! SaHpiManufacturerIdT2str: Undefined manufacturer testcase failed. %s= received string\n", str);
                        return -1;
                }
	}

	/*************************************************************** 
	 * SaHpiEventStateT2str and valid_SaHpiEventStateT4Cat testcases
         ***************************************************************/
	{
		SaHpiEventStateT  event_state;

		/* SaHpiEventStateT2str: Bad event category testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(SAHPI_ES_UNSPECIFIED, BAD_CAT, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event category testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_UNSPECIFIED testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_UNSPECIFIED, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_UNSPECIFIED testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_THRESHOLD testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_THRESHOLD testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_USAGE testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_USAGE, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_USAGE testcase failed. Error=%d\n", err);
                        return -1;
                }


		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_STATE testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_STATE, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_STATE testcase failed. Error=%d\n", err);
                        return -1;
                }


		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_PRED_FAIL testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_PRED_FAIL, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_PRED_FAIL testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_LIMIT testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_LIMIT, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_LIMIT testcase failed. Error=%d\n", err);
                        return -1;
                }


		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_PERFORMANCE testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_PERFORMANCE, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_PERFORMANCE testcase failed. Error=%d\n", err);
                        return -1;
                }


		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_SEVERITY testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_SEVERITY, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_SEVERITY testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_PRESENCE testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_PRESENCE, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_PRESENCE testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_ENABLE testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_ENABLE, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_ENABLE testcase failed. Error=%d\n", err);
                        return -1;
                }
		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_AVAILABILITY testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_AVAILABILITY, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_AVAILABILITY testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_REDUNDANCY testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_REDUNDANCY, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_REDUNDANCY testcase failed. Error=%d\n", err);
                        return -1;
                }

		/* SaHpiEventStateT2str: Bad event for SAHPI_EC_GENERIC testcase */
		/* Covers SAHPI_EC_SENSOR_SPECIFIC case as well */
		expected_err = SA_ERR_HPI_INVALID_DATA;
			
		err = SaHpiEventStateT2str(BAD_EVENT, SAHPI_EC_GENERIC, buffer, buffer_size);

                if (expected_err != err) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: Bad event for SAHPI_EC_GENERIC testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_ES_UNSPECIFIED testcase */
		expected_str = "SAHPI_ES_UNSPECIFIED ";
		memset(buffer, 0, buffer_size);

		err = SaHpiEventStateT2str(SAHPI_ES_UNSPECIFIED, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_ES_UNSPECIFIED testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_ES_UNSPECIFIED; SAHPI_EC_UNSPECIFIED testcase */
		expected_str = "SAHPI_ES_UNSPECIFIED ";
		memset(buffer, 0, buffer_size);

		err = SaHpiEventStateT2str(SAHPI_ES_UNSPECIFIED, SAHPI_EC_UNSPECIFIED, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_ES_UNSPECIFIED; SAHPI_EC_UNSPECIFIED testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Max events testcase */
		expected_str = "SAHPI_ES_LOWER_CRIT SAHPI_ES_LOWER_MAJOR SAHPI_ES_LOWER_MINOR SAHPI_ES_UPPER_CRIT SAHPI_ES_UPPER_MAJOR SAHPI_ES_UPPER_MINOR ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_LOWER_MINOR | 
			      SAHPI_ES_LOWER_MAJOR | 
			      SAHPI_ES_LOWER_CRIT |
                              SAHPI_ES_UPPER_MINOR | 
                              SAHPI_ES_UPPER_MAJOR |
                              SAHPI_ES_UPPER_CRIT; 
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (lower crit; no lower major) testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_LOWER_MINOR | 
			      SAHPI_ES_LOWER_CRIT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (lower crit no lower major) testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (lower major; no lower minor) testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_LOWER_MAJOR;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (lower major no lower minor) testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (upper crit; no upper major) testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_UPPER_MINOR | 
			      SAHPI_ES_UPPER_CRIT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (upper crit; no upper major) testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (upper major; no upper minor) testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_UPPER_MAJOR;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_THRESHOLD, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_THRESHOLD Completeness (upper major; no upper minor) testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_USAGE Max events testcase */
		expected_str = "SAHPI_ES_IDLE SAHPI_ES_ACTIVE SAHPI_ES_BUSY ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_IDLE | 
			      SAHPI_ES_ACTIVE | 
                              SAHPI_ES_BUSY;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_USAGE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_USAGE Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_STATE ok DEASSERTED testcase */
		expected_str = "SAHPI_ES_STATE_DEASSERTED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_STATE_DEASSERTED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_STATE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_STATE ok DEASSERTED testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_STATE ok ASSERTED testcase */
		expected_str = "SAHPI_ES_STATE_ASSERTED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_STATE_ASSERTED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_STATE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_STATE ok ASSERTED testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_STATE exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_STATE_DEASSERTED | SAHPI_ES_STATE_ASSERTED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_STATE, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_STATE exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL ok SAHPI_ES_PRED_FAILURE_DEASSERT testcase */
		expected_str = "SAHPI_ES_PRED_FAILURE_DEASSERT ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_PRED_FAILURE_DEASSERT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRED_FAIL, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL ok SAHPI_ES_PRED_FAILURE_DEASSERT failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL ok SAHPI_ES_PRED_FAILURE_ASSERT testcase */
		expected_str = "SAHPI_ES_PRED_FAILURE_ASSERT ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_PRED_FAILURE_ASSERT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRED_FAIL, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL ok SAHPI_ES_PRED_FAILURE_ASSERT failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_PRED_FAILURE_DEASSERT | SAHPI_ES_PRED_FAILURE_ASSERT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRED_FAIL, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRED_FAIL exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_LIMIT ok SAHPI_ES_LIMIT_NOT_EXCEEDED testcase */
		expected_str = "SAHPI_ES_LIMIT_NOT_EXCEEDED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_LIMIT_NOT_EXCEEDED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_LIMIT, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_LIMIT ok SAHPI_ES_LIMIT_NOT_EXCEEDED failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_LIMIT ok SAHPI_ES_LIMIT_EXCEEDED testcase */
		expected_str = "SAHPI_ES_LIMIT_EXCEEDED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_LIMIT_EXCEEDED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_LIMIT, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_LIMIT ok SAHPI_ES_LIMIT_EXCEEDED failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_LIMIT exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_LIMIT_NOT_EXCEEDED | SAHPI_ES_LIMIT_EXCEEDED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_LIMIT, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_LIMIT exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE ok SAHPI_ES_PERFORMANCE_MET testcase */
		expected_str = "SAHPI_ES_PERFORMANCE_MET ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_PERFORMANCE_MET;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PERFORMANCE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE ok SAHPI_ES_PERFORMANCE_MET failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE ok SAHPI_ES_PERFORMANCE_LAGS testcase */
		expected_str = "SAHPI_ES_PERFORMANCE_LAGS ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_PERFORMANCE_LAGS;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PERFORMANCE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE ok SAHPI_ES_PERFORMANCE_LAGS failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_PERFORMANCE_MET | SAHPI_ES_PERFORMANCE_LAGS;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PERFORMANCE, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PERFORMANCE exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRESENCE ok SAHPI_ES_ABSENT testcase */
		expected_str = "SAHPI_ES_ABSENT ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_ABSENT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRESENCE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRESENCE ok SAHPI_ES_ABSENT failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRESENCE ok SAHPI_ES_PRESENT testcase */
		expected_str = "SAHPI_ES_PRESENT ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_PRESENT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRESENCE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRESENCE ok SAHPI_ES_PRESENT failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_PRESENCE exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_ABSENT | SAHPI_ES_PRESENT;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRESENCE, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_PRESENCE exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_ENABLE ok SAHPI_ES_DISABLED testcase */
		expected_str = "SAHPI_ES_DISABLED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_DISABLED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_ENABLE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_ENABLE ok SAHPI_ES_DISABLED failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_ENABLE ok SAHPI_ES_ENABLED testcase */
		expected_str = "SAHPI_ES_ENABLED ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_ENABLED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_ENABLE, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_ENABLE ok SAHPI_ES_ENABLED failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_ENABLE exclusion testcase */
		expected_err = SA_ERR_HPI_INVALID_DATA;
		event_state = 0;
		event_state = SAHPI_ES_DISABLED | SAHPI_ES_ENABLED;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_ENABLE, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_ENABLE exclusion testcase failed. Error=%d\n", err);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_SEVERITY Max events testcase */
		expected_str = "SAHPI_ES_OK SAHPI_ES_MINOR_FROM_OK SAHPI_ES_MAJOR_FROM_LESS SAHPI_ES_CRITICAL_FROM_LESS SAHPI_ES_MINOR_FROM_MORE SAHPI_ES_MAJOR_FROM_CRITICAL SAHPI_ES_CRITICAL SAHPI_ES_MONITOR SAHPI_ES_INFORMATIONAL ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_OK |
			      SAHPI_ES_MINOR_FROM_OK |
                              SAHPI_ES_MAJOR_FROM_LESS |
                              SAHPI_ES_CRITICAL_FROM_LESS |
                              SAHPI_ES_MINOR_FROM_MORE |
                              SAHPI_ES_MAJOR_FROM_CRITICAL |
                              SAHPI_ES_CRITICAL |
                              SAHPI_ES_MONITOR |
                              SAHPI_ES_INFORMATIONAL;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_SEVERITY, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_SEVERITY Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_AVAILABILITY Max events testcase */
		expected_str = "SAHPI_ES_RUNNING SAHPI_ES_TEST SAHPI_ES_POWER_OFF SAHPI_ES_ON_LINE SAHPI_ES_OFF_LINE SAHPI_ES_OFF_DUTY SAHPI_ES_DEGRADED SAHPI_ES_POWER_SAVE SAHPI_ES_INSTALL_ERROR ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_RUNNING |
			      SAHPI_ES_TEST |
			      SAHPI_ES_POWER_OFF |
			      SAHPI_ES_ON_LINE |
			      SAHPI_ES_OFF_LINE |
			      SAHPI_ES_OFF_DUTY |
			      SAHPI_ES_DEGRADED |
			      SAHPI_ES_POWER_SAVE |
			      SAHPI_ES_INSTALL_ERROR;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_AVAILABILITY, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_AVAILABILITY Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SAHPI_EC_REDUNDANCY Max events testcase */
		expected_str = "SAHPI_ES_FULLY_REDUNDANT SAHPI_ES_REDUNDANCY_LOST SAHPI_ES_REDUNDANCY_DEGRADED SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_FULLY_REDUNDANT |
		              SAHPI_ES_REDUNDANCY_LOST |
			      SAHPI_ES_REDUNDANCY_DEGRADED |
		  	      SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES |
			      SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES |
			      SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES |
			      SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL |
			      SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_REDUNDANCY, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_REDUNDANCY Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

		/* Works for SAHPI_EC_SENSOR_SPECIFIC as well */
                /* SaHpiEventStateT2str: SAHPI_EC_GENERIC Max events testcase */
		expected_str = "SAHPI_ES_STATE_00 SAHPI_ES_STATE_01 SAHPI_ES_STATE_02 SAHPI_ES_STATE_03 SAHPI_ES_STATE_04 SAHPI_ES_STATE_05 SAHPI_ES_STATE_06 SAHPI_ES_STATE_07 SAHPI_ES_STATE_08 SAHPI_ES_STATE_09 SAHPI_ES_STATE_10 SAHPI_ES_STATE_11 SAHPI_ES_STATE_12 SAHPI_ES_STATE_13 SAHPI_ES_STATE_14 ";
		memset(buffer, 0, buffer_size);
		event_state = 0;
		event_state = SAHPI_ES_STATE_00 |
			      SAHPI_ES_STATE_01 |
			      SAHPI_ES_STATE_02 |
			      SAHPI_ES_STATE_03 |
			      SAHPI_ES_STATE_04 |
			      SAHPI_ES_STATE_05 |
			      SAHPI_ES_STATE_06 |
			      SAHPI_ES_STATE_07 |
			      SAHPI_ES_STATE_08 |
			      SAHPI_ES_STATE_09 |
			      SAHPI_ES_STATE_10 |
			      SAHPI_ES_STATE_11 |
			      SAHPI_ES_STATE_12 |
			      SAHPI_ES_STATE_13 |
			      SAHPI_ES_STATE_14;
		
		err = SaHpiEventStateT2str(event_state, SAHPI_EC_GENERIC, buffer, buffer_size);

                if (strcmp(expected_str, buffer)) {
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SAHPI_EC_GENERIC Max events testcase failed. %s= received string\n", buffer);
                        return -1;
                }

                /* SaHpiEventStateT2str: SaHpiEventStateT2str buffer too small testcase */
		expected_err = SA_ERR_HPI_OUT_OF_SPACE;
		event_state = 0;
		event_state = SAHPI_ES_PRESENT;
		memset(buffer, 0, 512);
		buffer_size = 0;

		err = SaHpiEventStateT2str(event_state, SAHPI_EC_PRESENCE, buffer, buffer_size);

                if (expected_err != err) { 
                        printf("tSaHpi_struct_utils Error! SaHpiEventStateT2str: SaHpiEventStateT2str buffer too small testcase failed. Error=%d\n", err);
                        return -1;
                }
	}

	return(0);

}
