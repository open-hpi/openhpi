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
#include "oSaHpiResourceEvent.hpp"
#include "oSaHpiDomainEvent.hpp"
#include "oSaHpiSensorEvent.hpp"
#include "oSaHpiSensorEnableChangeEvent.hpp"
#include "oSaHpiHotSwapEvent.hpp"
#include "oSaHpiWatchdogEvent.hpp"
#include "oSaHpiHpiSwEvent.hpp"
#include "oSaHpiOemEvent.hpp"
#include "oSaHpiUserEvent.hpp"
#include "oSaHpiEvent.hpp"


/**
 * Default constructor.
 */
oSaHpiEvent::oSaHpiEvent() {
    Source = 1;
    EventType = SAHPI_ET_RESOURCE;
    Timestamp = 0;
    Severity = SAHPI_OK;
    EventDataUnion.ResourceEvent.ResourceEventType = SAHPI_RESE_RESOURCE_ADDED;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiEvent::oSaHpiEvent(const oSaHpiEvent& buf) {
    memcpy(this, &buf, sizeof(SaHpiEventT));
}



/**
 * Assign a field in the SaHpiEventT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiEvent::assignField(SaHpiEventT *ptr,
                              const char *field,
                              const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "Source") == 0) {
        ptr->Source = strtoul(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "EventType") == 0) {
        ptr->EventType = oSaHpiTypesEnums::str2eventtype(value);
        return false;
    }
    else if (strcmp(field, "Timestamp") == 0) {
        ptr->Timestamp = strtoull(value, NULL, 10);
        return false;
    }
    else if (strcmp(field, "Severity") == 0) {
        ptr->Severity = oSaHpiTypesEnums::str2severity(value);
        return false;
    }
    // Event
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiEventT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiEvent::fprint(FILE *stream,
                         const int indent,
                         const SaHpiEventT *buffer) {
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
    err = fprintf(stream, "Source = %u\n", buffer->Source);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventType = %s\n", oSaHpiTypesEnums::eventtype2str(buffer->EventType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Timestamp = %ld\n", buffer->Timestamp);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Severity = %s\n", oSaHpiTypesEnums::severity2str(buffer->Severity));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventDataUnion\n");
    if (err < 0) {
        return true;
    }
    switch (buffer->EventType) {
    case SAHPI_ET_RESOURCE:
        oSaHpiResourceEvent * re = (oSaHpiResourceEvent *)&buffer->EventDataUnion.ResourceEvent;
        err = re->oSaHpiResourceEvent::fprint(stream, indent + 3, (SaHpiResourceEventT *)re);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_DOMAIN:
        oSaHpiDomainEvent * de = (oSaHpiDomainEvent *)&buffer->EventDataUnion.DomainEvent;
        err = de->oSaHpiDomainEvent::fprint(stream, indent + 3, (SaHpiDomainEventT *)de);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_SENSOR:
        oSaHpiSensorEvent * se = (oSaHpiSensorEvent *)&buffer->EventDataUnion.SensorEvent;
        err = se->oSaHpiSensorEvent::fprint(stream, indent + 3, (SaHpiSensorEventT *)se);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_SENSOR_ENABLE_CHANGE:
        oSaHpiSensorEnableChangeEvent * sec = (oSaHpiSensorEnableChangeEvent *)&buffer->EventDataUnion.SensorEnableChangeEvent;
        err = sec->oSaHpiSensorEnableChangeEvent::fprint(stream, indent + 3, (SaHpiSensorEnableChangeEventT *)sec);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_HOTSWAP:
        oSaHpiHotSwapEvent * hs = (oSaHpiHotSwapEvent *)&buffer->EventDataUnion.HotSwapEvent;
        err = hs->oSaHpiHotSwapEvent::fprint(stream, indent + 3, (SaHpiHotSwapEventT *)hs);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_WATCHDOG:
        oSaHpiWatchdogEvent * we = (oSaHpiWatchdogEvent *)&buffer->EventDataUnion.WatchdogEvent;
        err = we->oSaHpiWatchdogEvent::fprint(stream, indent + 3, (SaHpiWatchdogEventT *)we);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_HPI_SW:
        oSaHpiHpiSwEvent * hpise = (oSaHpiHpiSwEvent *)&buffer->EventDataUnion.HpiSwEvent;
        err = hpise->oSaHpiHpiSwEvent::fprint(stream, indent + 3, (SaHpiHpiSwEventT *)hpise);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_OEM:
        oSaHpiOemEvent * oe = (oSaHpiOemEvent *)&buffer->EventDataUnion.OemEvent;
        err = oe->oSaHpiOemEvent::fprint(stream, indent + 3, (SaHpiOemEventT *)oe);
        if (err < 0) {
            return true;
        }
        break;
    case SAHPI_ET_USER:
        oSaHpiUserEvent * ue = (oSaHpiUserEvent *)&buffer->EventDataUnion.UserEvent;
        err = ue->oSaHpiUserEvent::fprint(stream, indent + 3, (SaHpiUserEventT *)ue);
        if (err < 0) {
            return true;
        }
        break;
    default:
        err = fprintf(stream, indent_buf);
        if (err < 0) {
            return true;
        }
        err = fprintf(stream, "   Unknown\n");
        if (err < 0) {
            return true;
        }
    }



	return false;
}

