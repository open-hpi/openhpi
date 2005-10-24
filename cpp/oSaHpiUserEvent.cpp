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
#include "oSaHpiTextBuffer.hpp"
#include "oSaHpiUserEvent.hpp"


/**
 * Default constructor.
 */
oSaHpiUserEvent::oSaHpiUserEvent() {
    UserEventData.DataType = SAHPI_TL_TYPE_TEXT;
    UserEventData.Language = SAHPI_LANG_ENGLISH;
    UserEventData.DataLength = 0;
    UserEventData.Data[0] = '\0';
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiUserEvent::oSaHpiUserEvent(const oSaHpiUserEvent& buf) {
    memcpy(this, &buf, sizeof(SaHpiUserEventT));
}



/**
 * Assign a field in the SaHpiUserEventT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiUserEvent::assignField(SaHpiUserEventT *ptr,
                                  const char *field,
                                  const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    // UserEventData
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiUserEventT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiUserEvent::fprint(FILE *stream,
                             const int indent,
                             const SaHpiUserEventT *buffer) {
	int i, err;
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
    err = fprintf(stream, "UserEventData\n");
    if (err < 0) {
        return true;
    }
    const SaHpiTextBufferT *tb = (const SaHpiTextBufferT *)&buffer->UserEventData;
    err = oSaHpiTextBuffer::fprint(stream, indent + 3, tb);
    if (err < 0) {
        return true;
    }

	return false;
}

