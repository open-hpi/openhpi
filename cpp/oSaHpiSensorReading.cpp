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
#include "oSaHpiSensorReading.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorReading::oSaHpiSensorReading() {
    initSensorReading(this);
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorReading::oSaHpiSensorReading(const oSaHpiSensorReading& ent) {
    memcpy(this, &ent, sizeof(SaHpiSensorReadingT));
}


/**
 * Assign a field in the SaHpiSensorReadingT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorReading::assignField(SaHpiSensorReadingT *ptr,
                                      const char *field,
                                      const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "IsSupported") == 0) {
        ptr->IsSupported = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "Type") == 0) {
        ptr->Type = oSaHpiTypesEnums::str2sensorreadingtype(value);
        return false;
    }
    else if (strcmp(field, "SensorInt64") == 0) {
        ptr->Value.SensorInt64 = (SaHpiInt64T)strtoll(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "SensorUint64") == 0) {
        ptr->Value.SensorUint64 = (SaHpiUint64T)strtoull(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "SensorFloat64") == 0) {
        ptr->Value.SensorFloat64 = (SaHpiFloat64T)strtod(value, NULL);
        return false;
    }
    else if (strcmp(field, "SensorBuffer") == 0) {
        memset(ptr->Value.SensorBuffer, 0, SAHPI_SENSOR_BUFFER_LENGTH);
        memcpy(ptr->Value.SensorBuffer, value, strlen(value));
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
bool oSaHpiSensorReading::fprint(FILE *stream,
                                 const int indent,
                                 const SaHpiSensorReadingT *sr) {
	int i, err = 0;
    char indent_buf[indent + 1];

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
    err = fprintf(stream, "IsSupported = %s\n", oSaHpiTypesEnums::torf2str(IsSupported));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    switch (sr->Type) {
    case SAHPI_SENSOR_READING_TYPE_INT64:
        err = fprintf(stream, "Value.SensorInt64 = %d\n", sr->Value.SensorInt64);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_SENSOR_READING_TYPE_UINT64:
        err = fprintf(stream, "Value.SensorUint64 = %u\n", sr->Value.SensorUint64);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_SENSOR_READING_TYPE_FLOAT64:
        err = fprintf(stream, "Value.SensorFloat64 = %f\n", sr->Value.SensorFloat64);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_SENSOR_READING_TYPE_BUFFER:
        err = fprintf(stream, "Value.SensorBuffer = %s\n", sr->Value.SensorBuffer);
        if (err < 0) {
            return true;
        }
        break;
    default:
        err = fprintf(stream, "Value = Unknown\n");
        if (err < 0) {
            return true;
        }
        break;
    }

	return false;
}


void oSaHpiSensorReading::initSensorReading(SaHpiSensorReadingT *reading) {
    reading->IsSupported = ISSUPPORTED_DEFAULT;
    reading->Type = SAHPISENSORREADINGTYPET_DEFAULT;
    switch (reading->Type) {
    case SAHPI_SENSOR_READING_TYPE_INT64:
        reading->Value.SensorInt64 = 0;
        break;
    case SAHPI_SENSOR_READING_TYPE_UINT64:
        reading->Value.SensorUint64 = 0;
        break;
    case SAHPI_SENSOR_READING_TYPE_FLOAT64:
        reading->Value.SensorFloat64 = 0;
        break;
    case SAHPI_SENSOR_READING_TYPE_BUFFER:
    default:
        memset(&reading->Value.SensorBuffer, 0, sizeof(SAHPI_SENSOR_BUFFER_LENGTH));
        break;
    }
}

