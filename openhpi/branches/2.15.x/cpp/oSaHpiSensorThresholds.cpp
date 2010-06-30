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
#include "oSaHpiSensorThresholds.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorThresholds::oSaHpiSensorThresholds() {
    oSaHpiSensorReading *sr;

    sr = (oSaHpiSensorReading *)&LowCritical;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&LowMajor;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&LowMinor;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&UpCritical;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&UpMajor;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&UpMinor;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&PosThdHysteresis;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&NegThdHysteresis;
    sr->initSensorReading(sr);
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorThresholds::oSaHpiSensorThresholds(const oSaHpiSensorThresholds& th) {
    memcpy(this, &th, sizeof(SaHpiSensorThresholdsT));
}


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiSensorReadingT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorThresholds::fprint(FILE *stream,
                                 const int indent,
                                 const SaHpiSensorThresholdsT *st) {
	int i, err = 0;
    char indent_buf[indent + 1];
    const SaHpiSensorReadingT *sr;

    if (stream == NULL || st == NULL) {
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
    err = fprintf(stream, "LowCritical\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->LowCritical;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "LowMajor\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->LowMajor;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "LowMinor\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->LowMinor;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "UpCritical\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->UpCritical;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "UpMajor\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->UpMajor;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "UpMinor\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->UpMinor;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "PosThdHysteresis\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->PosThdHysteresis;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "NegThdHysteresis\n");
    if (err < 0) {
        return true;
    }
    sr = (const SaHpiSensorReadingT *)&st->NegThdHysteresis;
    err = oSaHpiSensorReading::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }

	return false;
}

