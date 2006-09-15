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
#include "oSaHpiSensorReading.hpp"
#include "oSaHpiSensorRange.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorRange::oSaHpiSensorRange() {
    oSaHpiSensorReading *sr;

    Flags = 0;
    sr = (oSaHpiSensorReading *)&Max;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Min;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Nominal;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&NormalMax;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&NormalMin;
    sr->initSensorReading(sr);
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorRange::oSaHpiSensorRange(const oSaHpiSensorRange& range) {
    memcpy(this, &range, sizeof(SaHpiSensorRangeT));
}


/**
 * Assign a field in the SaHpiSensorRangeT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorRange::assignField(SaHpiSensorRangeT *ptr,
                                    const char *field,
                                    const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "Flags") == 0) {
        ptr->Flags |= (SaHpiSensorRangeFlagsT)atoi(value);
        return false;
    }
    return true;
};


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiSensorReadingT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorRange::fprint(FILE *stream,
                               const int indent,
                               const SaHpiSensorRangeT *rg) {
	int i, err = 0;
    char indent_buf[indent + 1];
    const SaHpiSensorReadingT *sr;

    if (stream == NULL || rg == NULL) {
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
    err = fprintf(stream, "Flags = %X\n", rg->Flags);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Max\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&rg->Max;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Min\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&rg->Min;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Nominal\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&rg->Nominal;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "NormalMax\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&rg->NormalMax;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "NormalMin\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&rg->NormalMin;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }

	return false;
}

