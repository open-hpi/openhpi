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
#include "oSaHpiSensorDataFormat.hpp"
#include "oSaHpiSensorThdDefn.hpp"
#include "oSaHpiSensorRec.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorRec::oSaHpiSensorRec() {
    oSaHpiSensorReading *sr;

    Num = 0;
    Type = SAHPI_TEMPERATURE;
    Category = 0;
    EnableCtrl = false;
    EventCtrl = SAHPI_SEC_PER_EVENT;
    Events = 0;
    DataFormat.IsSupported = 0;
    DataFormat.ReadingType = SAHPI_SENSOR_READING_TYPE_INT64;
    DataFormat.BaseUnits = SAHPI_SU_UNSPECIFIED;
    DataFormat.ModifierUnits = SAHPI_SU_UNSPECIFIED;
    DataFormat.ModifierUse = SAHPI_SMUU_NONE;
    DataFormat.Percentage = false;
    DataFormat.Range.Flags = 0;
    sr = (oSaHpiSensorReading *)&DataFormat.Range.Max;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&DataFormat.Range.Min;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&DataFormat.Range.Nominal;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&DataFormat.Range.NormalMax;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&DataFormat.Range.NormalMin;
    sr->initSensorReading(sr);
    DataFormat.AccuracyFactor = 0;
    // thd defn
    ThresholdDefn.IsAccessible = false;
    ThresholdDefn.ReadThold = 0;
    ThresholdDefn.WriteThold = 0;
    ThresholdDefn.Nonlinear = false;
    Oem = 0;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorRec::oSaHpiSensorRec(const oSaHpiSensorRec& sr) {
    memcpy(this, &sr, sizeof(SaHpiSensorRecT));
}


/**
 * Assign a field in the SaHpiSensorRecT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorRec::assignField(SaHpiSensorRecT *ptr,
                                  const char *field,
                                  const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "Num") == 0) {
        ptr->Num = (SaHpiSensorNumT)atoi(value);
        return false;
    }
    else if (strcmp(field, "Type") == 0) {
        ptr->Type = oSaHpiTypesEnums::str2sensortype(value);
        return false;
    }
    else if (strcmp(field, "Category") == 0) {
        ptr->Category |= oSaHpiTypesEnums::str2eventcategory(value);
        return false;
    }
    else if (strcmp(field, "EnableCtrl") == 0) {
        ptr->EnableCtrl = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "EventCtrl") == 0) {
        ptr->EventCtrl = oSaHpiTypesEnums::str2sensoreventctrl(value);
        return false;
    }
    else if (strcmp(field, "Events") == 0) {
        ptr->Events |= oSaHpiTypesEnums::str2eventstate(value);
        return false;
    }
    // DataFormat
    // ThresholdDefn
    else if (strcmp(field, "Oem") == 0) {
        ptr->Oem = (SaHpiUint32T)atoi(value);
        return false;
    }
    return true;
};


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiSensorRecT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorRec::fprint(FILE *stream,
                                 const int indent,
                                 const SaHpiSensorRecT *sr) {
	int i, err = 0;
    char indent_buf[indent + 1];
    oSaHpiSensorDataFormat *df;
    oSaHpiSensorThdDefn *td;

    if (stream == NULL) {
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
    err = fprintf(stream, "Num = %d\n", sr->Num);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Type = %s\n", oSaHpiTypesEnums::sensortype2str(sr->Type));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Category = %X\n", sr->Category);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EnableCtrl = %s\n", oSaHpiTypesEnums::torf2str(sr->EnableCtrl));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventCtrl = %s\n", oSaHpiTypesEnums::sensoreventctrl2str(sr->EventCtrl));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Events = %X\n", sr->Events);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "DataFormat\n");
    if (err < 0) {
        return true;
    }
    df = (oSaHpiSensorDataFormat *)&(sr->DataFormat);
    err = df->oSaHpiSensorDataFormat::fprint(stream, indent + 3, df);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ThresholdDefn\n");
    if (err < 0) {
        return true;
    }
    td = (oSaHpiSensorThdDefn *)&(sr->ThresholdDefn);
    err = td->oSaHpiSensorThdDefn::fprint(stream, indent + 3, td);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Oem = %d\n", sr->Oem);
    if (err < 0) {
        return true;
    }

	return false;
}

