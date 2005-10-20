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
#include "oSaHpiSensorEvent.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorEvent::oSaHpiSensorEvent() {
    oSaHpiSensorReading *sr;

    SensorNum = 1;
    SensorType = SAHPI_TEMPERATURE;
    EventCategory = SAHPI_EC_UNSPECIFIED;
    Assertion = false;
    EventState = SAHPI_ES_UNSPECIFIED;
    OptionalDataPresent = (SaHpiSensorOptionalDataT)0;
    sr = (oSaHpiSensorReading *)&TriggerReading;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&TriggerThreshold;
    sr->initSensorReading(sr);
    PreviousState = SAHPI_ES_UNSPECIFIED;
    CurrentState = SAHPI_ES_UNSPECIFIED;
    Oem = 0;
    SensorSpecific = 0;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorEvent::oSaHpiSensorEvent(const oSaHpiSensorEvent& range) {
    memcpy(this, &range, sizeof(SaHpiSensorEventT));
}


/**
 * Assign a field in the SaHpiSensorEventT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorEvent::assignField(SaHpiSensorEventT *ptr,
                                    const char *field,
                                    const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "SensorNum") == 0) {
        ptr->SensorNum = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "SensorType") == 0) {
        ptr->SensorType = oSaHpiTypesEnums::str2sensortype(value);
        return false;
    }
    else if (strcmp(field, "EventCategory") == 0) {
        ptr->EventCategory |= oSaHpiTypesEnums::str2eventcategory(value);
        return false;
    }
    else if (strcmp(field, "Assertion") == 0) {
        ptr->Assertion = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "EventState") == 0) {
        ptr->EventState |= oSaHpiTypesEnums::str2eventstate(value);
        return false;
    }
    else if (strcmp(field, "OptionalDataPresent") == 0) {
        ptr->OptionalDataPresent |= oSaHpiTypesEnums::str2sensoroptionaldata(value);
        return false;
    }
    // TriggerReading
    // TriggerThreshold
    else if (strcmp(field, "PreviousState") == 0) {
        ptr->PreviousState |= oSaHpiTypesEnums::str2eventstate(value);
        return false;
    }
    else if (strcmp(field, "CurrentState") == 0) {
        ptr->CurrentState |= oSaHpiTypesEnums::str2eventstate(value);
        return false;
    }
    else if (strcmp(field, "Oem") == 0) {
        ptr->Oem = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "SensorSpecific") == 0) {
        ptr->SensorSpecific = strtoul(value, NULL, 10);
        return false;
    }
    return true;
};


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiSensorEventT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorEvent::fprint(FILE *stream,
                               const int indent,
                               const SaHpiSensorEventT *se) {
	int i, err = 0;
    char indent_buf[indent + 1];
    oSaHpiSensorReading *sr;

    if (stream == NULL || se == NULL) {
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
    err = fprintf(stream, "SensorNum = %u\n", se->SensorNum);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "SensorType = %s\n", oSaHpiTypesEnums::sensortype2str(se->SensorType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventCategory = %X\n", se->EventCategory);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Assertion = %s\n", oSaHpiTypesEnums::torf2str(se->Assertion));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventState = %X\n", se->EventState);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "OptionalDataPresent = %X\n", se->OptionalDataPresent);
    if (err < 0) {
        return true;
    }
    if (OptionalDataPresent && SAHPI_SOD_TRIGGER_READING) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "TriggerReading\n");
        if (err < 0) {
            return true;
        }
        sr = (oSaHpiSensorReading *)&se->TriggerReading;
        err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
        if (err < 0) {
            return true;
        }
    }
    if (OptionalDataPresent && SAHPI_SOD_TRIGGER_THRESHOLD) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "TriggerThreshold\n");
        if (err < 0) {
            return true;
        }
        sr = (oSaHpiSensorReading *)&se->TriggerThreshold;
        err = sr->oSaHpiSensorReading::fprint(stream, indent + 3, sr);
        if (err < 0) {
            return true;
        }
    }
    if (OptionalDataPresent && SAHPI_SOD_PREVIOUS_STATE) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "PreviousState = %X\n", se->PreviousState);
        if (err < 0) {
            return true;
        }
    }
    if (OptionalDataPresent && SAHPI_SOD_CURRENT_STATE) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "CurrentState = %X\n", se->CurrentState);
        if (err < 0) {
            return true;
        }
    }
    if (OptionalDataPresent && SAHPI_SOD_OEM) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "Oem = %u\n", se->Oem);
        if (err < 0) {
            return true;
        }
    }
    if (OptionalDataPresent && SAHPI_SOD_SENSOR_SPECIFIC) {
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "SensorSpecific = %u\n", se->SensorSpecific);
        if (err < 0) {
            return true;
        }
    }

	return false;
}

