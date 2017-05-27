/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 * (C) Copyright Pigeon Point Systems. 2010
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
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <glib.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>


/**
 * oh_localtime:
 * @time: SaHpiTimeT time to be converted.                    
 * @tm: Location to store the converted broken-down time.
 *
 * Converts an SaHpiTimeT time value to broken-down
 * time representation.
 * 
 * Returns:
 * SA_OK - normal operation. 
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is NULL.
 * SA_ERR_HPI_INTERNAL_ERROR - conversion failed
 **/
SaErrorT oh_localtime(const SaHpiTimeT time, struct tm *tm)
{
        time_t t;
        struct tm *tm2;

        if (!tm) {
                return(SA_ERR_HPI_INVALID_PARAMS);
        }

        t = (time_t)(time / 1000000000);

#ifndef _WIN32
        tm2 = localtime_r(&t, tm);
#else /* _WIN32 */
        // Windows version of localtime is thread-safe
        tm2 = localtime(&t);
        if (tm2) {
                *tm = *tm2;
        }
#endif /* _WIN32 */

        if (!tm2) {
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return(SA_OK);
}

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
 *                             date/time representation string or
 *                             conversion failed.
 **/
SaErrorT oh_decode_time(SaHpiTimeT time, SaHpiTextBufferT *buffer)
{
	int count;
	SaErrorT err;
	SaHpiTextBufferT working;

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_textbuffer(&working);
	if (err != SA_OK) { return(err); }

        if (time == SAHPI_TIME_UNSPECIFIED) {
                strcpy((char *)working.Data,"SAHPI_TIME_UNSPECIFIED     ");
		count = sizeof("SAHPI_TIME_UNSPECIFIED     ");
        } else if (time > SAHPI_TIME_MAX_RELATIVE) { /*absolute time*/
                struct tm tm;
                err = oh_localtime(time, &tm);
                if (err != SA_OK) {
                        return(err);
                }
                count = strftime((char *)working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH, "%Y-%m-%d %H:%M:%S", &tm);
        } else {   /*relative time*/
                long secs  = (long)( time / 1000000000L );
                long mins  = ( secs / 60L ) % 60L;
                long hours = ( secs / 3600L ) % 24L;
                long days  = secs / 86400L;
                secs = secs % 60L;
                count = snprintf( (char *)working.Data,
                                  SAHPI_MAX_TEXT_BUFFER_LENGTH,
                                  "RELATIVE: %ldd %02ld:%02ld:%02ld",
                                  days, hours, mins, secs );
        }

        if (count == 0) { return(SA_ERR_HPI_INTERNAL_ERROR); }
	else working.DataLength = count;
	
	err = oh_copy_textbuffer(buffer, &working);
	if (err != SA_OK) { return(err); }

	return(SA_OK);
}

/**
 * oh_gettimeofday:
 * @time: Location to store Time of Day value
 *
 * Find the time of day and converts it into an HPI time.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @time is NULL.
 **/
SaErrorT oh_gettimeofday(SaHpiTimeT *time)
{
        GTimeVal now;

	if (!time) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        g_get_current_time(&now);

        *time = (SaHpiTimeT)now.tv_sec * 1000000000 + now.tv_usec * 1000;

	return(SA_OK);
}

