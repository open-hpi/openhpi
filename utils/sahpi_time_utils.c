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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SaHpi.h>
#include <oh_utils.h>

/**
 * oh_decode_time:
 * @time: SaHpiTimeT time to be converted.                    
 * @buffer: Location to store the converted string.
 *
 * Converts an SaHpiTimeT time value to the preferred date/time string 
 * representation defined for the current locale.
 * String is stored in an SaHpiTextBufferT data structure.
 * 
 * Returns:
 * SA_OK - normal operation. 
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is NULL.
 * SA_ERR_HPI_INTERNAL_ERROR - @buffer not big enough to accomodate
 *                             date/time representation string.
 **/
SaErrorT oh_decode_time(SaHpiTimeT time, SaHpiTextBufferT *buffer)
{
	int count;
	struct tm t;
	time_t tt;
	SaErrorT err;
	SaHpiTextBufferT working;

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* FIXME:: Is there a check for valid time?? */
	err = oh_init_textbuffer(&working);
	if (err != SA_OK) { return(err); }

	tt = time / 1000000000;
	localtime_r(&tt, &t);
	
	count = strftime(working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH, "%c", &t);
	if (count == 0) { return(SA_ERR_HPI_INTERNAL_ERROR); }
	
	err = oh_copy_textbuffer(buffer, &working);
	if (err != SA_OK) { return(err); }

	return(SA_OK);
}
