/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *    W. David Ashley <dashley@us.ibm.com>
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern "C"
{
#include <SaHpi.h>
}
#include "oSaHpiTypesEnums.hpp"
#include "oSaHpiWatchdog.hpp"


/**
 * Default constructor.
 */
oSaHpiWatchdog::oSaHpiWatchdog() {
    Log = false;
    Running = false;
    TimerUse = SAHPI_WTU_NONE;
    TimerAction = SAHPI_WA_NO_ACTION;
    PretimerInterrupt = SAHPI_WPI_NONE;
    PreTimeoutInterval = 0;
    TimerUseExpFlags = 0;
    InitialCount = 0;
    PresentCount = 0;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiWatchdog::oSaHpiWatchdog(const oSaHpiWatchdog& buf) {
    memcpy(this, &buf, sizeof(SaHpiWatchdogT));
}



/**
 * Assign a field in the SaHpiWatchdogT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdog::assignField(SaHpiWatchdogT *ptr,
                                 const char *field,
                                 const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "Log") == 0) {
        ptr->Log = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "Running") == 0) {
        ptr->Running = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "TimerUse") == 0) {
        ptr->TimerUse = oSaHpiTypesEnums::str2watchdogtimeruse(value);
        return false;
    }
    else if (strcmp(field, "TimerAction") == 0) {
        ptr->TimerAction = oSaHpiTypesEnums::str2watchdogaction(value);
        return false;
    }
    else if (strcmp(field, "PretimerInterrupt") == 0) {
        ptr->PretimerInterrupt = oSaHpiTypesEnums::str2watchdogpretimerinterrupt(value);
        return false;
    }
    else if (strcmp(field, "PreTimeoutInterval") == 0) {
        ptr->PreTimeoutInterval = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "TimerUseExpFlags") == 0) {
        ptr->TimerUseExpFlags |= oSaHpiTypesEnums::str2watchdogexpflags(value);
        return false;
    }
    else if (strcmp(field, "InitialCount") == 0) {
        ptr->InitialCount = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "PresentCount") == 0) {
        ptr->PresentCount = strtoul(value, NULL, 10);
        return false;
    }
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiWatchdogT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdog::fprint(FILE *stream,
                            const int indent,
                            const SaHpiWatchdogT *buffer) {
	int i, err;
    char buf[20];
    char indent_buf[indent + 1];

    if (stream == NULL || buffer == NULL) {
        return true;
    }
    for (i = 0; i < indent; i++) {
        indent_buf[i] = ' ';
    }
    indent_buf[indent] = '\0';

    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Log = %s\n", oSaHpiTypesEnums::torf2str(buffer->Log));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Running = %s\n", oSaHpiTypesEnums::torf2str(buffer->Running));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "TimerUse = %s\n", oSaHpiTypesEnums::watchdogtimeruse2str(buffer->TimerUse));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "TimerAction = %s\n", oSaHpiTypesEnums::watchdogaction2str(buffer->TimerAction));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "PreTimerInterrupt = %s\n", oSaHpiTypesEnums::watchdogpretimerinterrupt2str(buffer->PretimerInterrupt));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "PreTimeoutInterval = %u\n", buffer->PreTimeoutInterval);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "TimerUseExpFlags = %X\n", buffer->TimerUseExpFlags);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "InitialCount = %u\n", buffer->InitialCount);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "PresentCount = %u\n", buffer->PresentCount);
    if (err < 0) {
        return true;
    }

	return false;
}

