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
#include "oSaHpiWatchdogRec.hpp"


/**
 * Default constructor.
 */
oSaHpiWatchdogRec::oSaHpiWatchdogRec() {
    WatchdogNum = 1;
    Oem = 0;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiWatchdogRec::oSaHpiWatchdogRec(const oSaHpiWatchdogRec& buf) {
    memcpy(this, &buf, sizeof(SaHpiWatchdogRecT));
}



/**
 * Assign a field in the SaHpiWatchdogRecT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdogRec::assignField(SaHpiWatchdogRecT *ptr,
                                    const char *field,
                                    const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "WatchdogNum") == 0) {
        ptr->WatchdogNum = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "Oem") == 0) {
        ptr->Oem = strtoul(value, NULL, 10);
        return false;
    }
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiWatchdogRecT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiWatchdogRec::fprint(FILE *stream,
                               const int indent,
                               const SaHpiWatchdogRecT *buffer) {
	int i, err;
    char indent_buf[indent + 1];

    if (stream == NULL || buffer == NULL) {
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
    err = fprintf(stream, "WatchdogNum = %u\n", buffer->WatchdogNum);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Oem = %u\n", buffer->Oem);
    if (err < 0) {
        return true;
    }

	return false;
}

