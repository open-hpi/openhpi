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
 *      Steve Sherman <stevees.ibm.com> 
 */

#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

#define BAD_CAT -1

int main(int argc, char **argv) 
{
        const char *expected_str;
        SaErrorT err, expected_err;
        SaHpiEventStateT event_state, expected_state;
        SaHpiEventCategoryT event_cat, expected_cat;
        SaHpiTextBufferT buffer;

	/******************************** 
	 * oh_decode_eventstate testcases
         ********************************/

	/* oh_decode_eventstate: Bad event category testcase */
	{
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
	
		err = oh_decode_eventstate(SAHPI_ES_UNSPECIFIED, BAD_CAT, &buffer);
		
		if (expected_err != err) {
			printf("Error! oh_decode_eventstate: Bad event category testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

	/* oh_decode_eventstate: NULL buffer testcase */
	{
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		err = oh_decode_eventstate(SAHPI_ES_UNSPECIFIED, SAHPI_EC_UNSPECIFIED, 0);
		
		if (expected_err != err) {
			printf("Error! oh_decode_eventstate: NULL buffer testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

	/* oh_decode_eventstate: print UNSPECIFIED testcase */
	{
		event_state = SAHPI_ES_UNSPECIFIED;
		event_cat = SAHPI_EC_GENERIC;
		expected_str = "UNSPECIFIED";
		
		err = oh_decode_eventstate(event_state, event_cat, &buffer);
		
		if (err != SA_OK) {
			printf("Error! oh_decode_eventstate: print UNSPECIFIED testcase failed. Received error=%d\n", err);
			return -1;
		}

                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! oh_decode_eventstate: Testcase print UNSPECIFIED failed\n");
                        printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }
	}

	/* oh_decode_eventstate: strip extra UNSPECIFIED testcase */
	{
		event_state = SAHPI_ES_STATE_01 | SAHPI_ES_STATE_03 | SAHPI_ES_UNSPECIFIED;
		expected_str = "STATE_01 | STATE_03";

		err = oh_decode_eventstate(event_state, event_cat, &buffer);
		
		if (err != SA_OK) {
			printf("Error! oh_decode_eventstate: strip extra UNSPECIFIED testcase failed. Received error=%d\n", err);
			return -1;
		}

                if (strcmp(expected_str, buffer.Data)) {
                        printf("Error! oh_decode_eventstate: Testcase strip extra UNSPECIFIED failed\n");
                        printf("Received string=%s\n", buffer.Data);
                        return -1;             
                }
	}

	/******************************** 
	 * oh_encode_eventstate testcases
         ********************************/

        /* oh_encode_eventstate testcases - NULL parameters testcase */
        {
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
  
                err = oh_encode_eventstate(0, 0, 0);

		if (expected_err != err) {
			printf("Error! oh_encode_eventstate: NULL parameters testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

        /* oh_encode_eventstate testcases - No Data testcase */
        {
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		buffer.Data[0] = 0x00;

                err = oh_encode_eventstate(&buffer, &event_state, &event_cat);

		if (expected_err != err) {
			printf("Error! oh_encode_eventstate: No Data testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

        /* oh_encode_eventstate testcases - handle blanks testcase */
        {
		strcpy(buffer.Data, "  LOWER_MINOR  |  LOWER_MAJOR|LOWER_CRIT ");
                expected_cat = SAHPI_EC_THRESHOLD;
                expected_state = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_MAJOR | SAHPI_ES_LOWER_CRIT;
  
                err = oh_encode_eventstate(&buffer, &event_state, &event_cat);
                if (err != SA_OK) {
                        printf("Error! oh_encode_eventstate: handle blanks testcase failed. Received error=%d\n", err);
                        return -1; 
                }

                if ((expected_state != event_state) || (expected_cat != event_cat)) {
                        printf("Error! oh_encode_eventstate: Testcase handle blanks failed\n");
                        printf("Receive state:d state=%x; Received cat=%x\n", event_state, event_cat);
                        return -1;
                }
	}

	/* oh_encode_eventstate testcases - valid states but different categories testcase */
        {
		strcpy(buffer.Data, "LOWER_MINOR | STATE_13 | IDLE");
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
  
                err = oh_encode_eventstate(&buffer, &event_state, &event_cat);

		if (expected_err != err) {
			printf("Error! oh_encode_eventstate: valid states but different categories testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

	/* oh_encode_eventstate testcases - garbage state testcase */	
        {
		strcpy(buffer.Data, "GARBAGE_STATE");
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
  
                err = oh_encode_eventstate(&buffer, &event_state, &event_cat);

		if (expected_err != err) {
			printf("Error! oh_encode_eventstate: garbage state testcase failed. Received error=%d\n", err);
			return -1;
		}
	}

	/******************************* 
	 * oh_valid_eventstate testcases
         *******************************/

	/* oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (lower crit; no lower major) testcase */
	{
		event_state = SAHPI_ES_LOWER_MINOR | SAHPI_ES_LOWER_CRIT;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_THRESHOLD)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (lower crit no lower major) testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (lower major; no lower minor) testcase */
	{
		event_state = SAHPI_ES_LOWER_MAJOR;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_THRESHOLD)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (lower major no lower minor) testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (upper crit; no upper major) testcase */
	{
		event_state = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_CRIT;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_THRESHOLD)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (upper crit; no upper major) testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (upper major; no upper minor) testcase */
	{
		event_state = SAHPI_ES_UPPER_MAJOR;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_THRESHOLD)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_THRESHOLD Completeness (upper major; no upper minor) testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}
	
	/* oh_valid_eventstate: SAHPI_EC_STATE exclusion testcase */
	{
		event_state = SAHPI_ES_STATE_DEASSERTED | SAHPI_ES_STATE_ASSERTED;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_STATE)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_STATE exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_PRED_FAIL exclusion testcase */
	{
		event_state = SAHPI_ES_PRED_FAILURE_DEASSERT | SAHPI_ES_PRED_FAILURE_ASSERT;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_PRED_FAIL)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_PRED_FAIL exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}
	
	/* oh_valid_eventstate: SAHPI_EC_LIMIT exclusion testcase */
	{
		event_state = SAHPI_ES_LIMIT_NOT_EXCEEDED | SAHPI_ES_LIMIT_EXCEEDED;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_LIMIT)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_LIMIT exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}
	
	/* oh_valid_eventstate: SAHPI_EC_PERFORMANCE exclusion testcase */
	{
		event_state = SAHPI_ES_PERFORMANCE_MET | SAHPI_ES_PERFORMANCE_LAGS;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_PERFORMANCE)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_PERFORMANCE exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }

	}

	/* oh_valid_eventstate: SAHPI_EC_PRESENCE exclusion testcase */
	{
		event_state = SAHPI_ES_ABSENT | SAHPI_ES_PRESENT;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_PRESENCE)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_PRESENCE exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_ENABLE exclusion testcase */
	{
		event_state = SAHPI_ES_DISABLED | SAHPI_ES_ENABLED;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_ENABLE)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_ENABLE exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_FULLY_REDUNDANT exclusion testcase */
	{
		event_state = SAHPI_ES_FULLY_REDUNDANT | SAHPI_ES_REDUNDANCY_LOST;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_REDUNDANCY)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_FULLY_REDUNDANT exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_DEGRADED exclusion testcase */
	{
		event_state =  SAHPI_ES_REDUNDANCY_DEGRADED | SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_REDUNDANCY)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_DEGRADED exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_LOST exclusion testcase */
	{
		event_state = SAHPI_ES_REDUNDANCY_LOST | SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_REDUNDANCY)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_DEGRADED exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL exclusion testcase */
	{
		event_state = SAHPI_ES_REDUNDANCY_DEGRADED | 
			      SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL |
			      SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_REDUNDANCY)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

	/* oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES exclusion testcase */
	{
		event_state = SAHPI_ES_REDUNDANCY_LOST | 
			      SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES |
 			      SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES;
		
		if (oh_valid_eventstate(event_state, SAHPI_EC_REDUNDANCY)) {
                        printf("Error! oh_valid_eventstate: SAHPI_EC_REDUNDANCY - SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES exclusion testcase failed. Received error=%d\n", err);
                        return -1;
                }
	}

        return 0;
}

