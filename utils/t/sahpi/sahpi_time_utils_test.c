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

#include <SaHpi.h>
#include <oh_utils.h>

int main(int argc, char **argv) 
{
	/* const char *expected_str; */
	SaErrorT   expected_err, err;
	SaHpiTextBufferT buffer;

	/************************** 
	 * oh_decode_time testcases
         **************************/
	{
		/* oh_decode_time: test initial time testcase */
		/* FIXME:: Can't look for a specific expected string, since it depends on locale 
		   Need to set and reset locale dynamically from this testcase */
		err = oh_decode_time(0, &buffer);

		if (err != SA_OK) {
                        printf("Error! oh_decode_time: test initial time testcase failed. Error=%d\n", err);
                        return -1;
                }
		/* printf("Time=%s\n", buffer.Data); */

		/* oh_decode_time: Null buffer testcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;
		
		err = oh_decode_time(0, 0);
		
                if (expected_err != err) {
                        printf("Error! oh_decode_time: Null buffer testcase failed. Error=%d\n", err);
                        return -1;
                }
	}

	{
		SaHpiTimeT time;
		
		/* oh_gettimeofday normal time textcase */
		err = oh_gettimeofday(&time);
		if (err != SA_OK) {
                        printf("Error! oh_gettimeofday: normal testcase failed. Error=%d\n", err);
                        return -1;
                }
		
		err = oh_decode_time(time, &buffer);
		if (err != SA_OK) {
                        printf("Error! oh_gettimeofday: normal testcase failed. oh_decode_time error=%d\n", err);
                        return -1;
                }
		/* printf("TOD Time=%s\n", buffer.Data); */

		/* oh_gettimeofday NULL time textcase */
		expected_err = SA_ERR_HPI_INVALID_PARAMS;

		err = oh_gettimeofday(0);
		if (err != expected_err) {
                        printf("Error! oh_gettimeofday: NULL time testcase failed\n");
			printf("Received Error=%d\n", err);
                        return -1;
                }
	}

	return(0);
}
