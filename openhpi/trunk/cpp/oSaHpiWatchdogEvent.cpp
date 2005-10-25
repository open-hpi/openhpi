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
#include "oSaHpiWatchdogEvent.hpp"


/**
 * Default constructor.
 */
oSaHpiWatchdogEvent::oSaHpiWatchdogEvent() {
    WatchdogNum = 1;
    WatchdogAction = SAHPI_WAE_NO_ACTION;
    WatchdogPreTimerAction = SAHPI_WPI_NONE;
    WatchdogUse = SAHPI_WTU_NONE;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiWatchdogEvent::oSaHpiWatchdogEvent(const oSaHpiWatchdogEvent& range) {
    memcpy(this, &range, sizeof(SaHpiWatchdogEventT));
}


/**
 * Assign a field in the SaHpiWatchdogEventT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdogEvent::assignField(SaHpiWatchdogEventT *ptr,
                                      const char *field,
                                      const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "WatchdogNum") == 0) {
        ptr->WatchdogNum = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "WatchdogAction") == 0) {
        ptr->WatchdogAction = oSaHpiTypesEnums::str2watchdogactionevent(value);
        return false;
    }
    else if (strcmp(field, "WatchdogPreTimeAction") == 0) {
        ptr->WatchdogPreTimerAction = oSaHpiTypesEnums::str2watchdogpretimerinterrupt(value);
        return false;
    }
    else if (strcmp(field, "WatchdogUse") == 0) {
        ptr->WatchdogUse = oSaHpiTypesEnums::str2watchdogtimeruse(value);
        return false;
    }
    return true;
};


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiWatchdogEventT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdogEvent::fprint(FILE *stream,
                               const int indent,
                               const SaHpiWatchdogEventT *we) {
	int i, err = 0;
    char indent_buf[indent + 1];

    if (stream == NULL || we == NULL) {
        return true;
    }
    for (i = 0; i < indent; i++) {
        indent_buf[i] = ' ';
    }
    indent_buf[indent] = '\0';

    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WatchdogNum = %u\n", we->WatchdogNum);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WatchdogAction = %s\n", oSaHpiTypesEnums::watchdogactionevent2str(we->WatchdogAction));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WatchdogPreTimerAction = %s\n", oSaHpiTypesEnums::watchdogpretimerinterrupt2str(we->WatchdogPreTimerAction));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WatchdogUse = %s\n", oSaHpiTypesEnums::watchdogtimeruse2str(we->WatchdogUse));
    if (err < 0) {
        return true;
    }

	return false;
}

