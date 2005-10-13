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
 * Destructor.
 */
oSaHpiSensorRange::~oSaHpiSensorRange() {
}


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
    oSaHpiSensorReading *sr;

    if (stream == NULL || sr == NULL) {
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
    err = fprintf(stream, "Flags = %X\n", Flags);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Max\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorReading *)&rg->Max;
    err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Min\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorReading *)&rg->Min;
    err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Nominal\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorReading *)&rg->Nominal;
    err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "NormalMax\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorReading *)&rg->NormalMax;
    err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "NormalMin\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorReading *)&rg->NormalMin;
    err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }

	return false;
}

