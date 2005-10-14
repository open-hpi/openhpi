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
#include "oSaHpiSensorThdDefn.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorThdDefn::oSaHpiSensorThdDefn() {
    IsAccessible = false;
    ReadThold = 0;
    WriteThold = 0;
    Nonlinear = false;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorThdDefn::oSaHpiSensorThdDefn(const oSaHpiSensorThdDefn& df) {
    memcpy(this, &df, sizeof(SaHpiSensorThdDefnT));
}


/**
 * Assign a field in the SaHpiSensorThdDefnT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorThdDefn::assignField(SaHpiSensorThdDefnT *ptr,
                                      const char *field,
                                      const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "IsAccessible") == 0) {
        ptr->IsAccessible = (SaHpiBoolT)atoi(value);
        return false;
    }
    else if (strcmp(field, "ReadThold") == 0) {
        ptr->ReadThold = str2sensorthdmask(value);
        return false;
    }
    else if (strcmp(field, "WriteThold") == 0) {
        ptr->WriteThold = str2sensorthdmask(value);
        return false;
    }
    else if (strcmp(field, "Nonlinear") == 0) {
        ptr->Nonlinear = (SaHpiBoolT)atoi(value);
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
bool oSaHpiSensorThdDefn::fprint(FILE *stream,
                                 const int indent,
                                 const SaHpiSensorThdDefnT *df) {
	int i, err = 0;
    char indent_buf[indent + 1];

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
    err = fprintf(stream, "IsAccessible = %d\n", df->IsAccessible);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ReadThold = %s\n", sensorthdmask2str(df->ReadThold));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WriteThold = %s\n", sensorthdmask2str(df->WriteThold));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Nonlinear = %d\n", df->Nonlinear);
    if (err < 0) {
        return true;
    }

	return false;
}


static struct sensorthdmasktype_map {
    SaHpiSensorThdMaskT type;
    const char          *str;
} sensorthdmask_strings[] = {
       {SAHPI_STM_LOW_MINOR,       "SAHPI_STM_LOW_MINOR"},
       {SAHPI_STM_LOW_MAJOR,       "SAHPI_STM_LOW_MAJOR"},
       {SAHPI_STM_LOW_CRIT,        "SAHPI_STM_LOW_CRIT"},
       {SAHPI_STM_UP_MAJOR,        "SAHPI_STM_UP_MAJOR"},
       {SAHPI_STM_UP_MINOR,        "SAHPI_STM_UP_MINOR"},
       {SAHPI_STM_UP_CRIT,         "SAHPI_STM_UP_CRIT"},
       {SAHPI_STM_UP_HYSTERESIS,   "SAHPI_STM_UP_HYSTERESIS"},
       {SAHPI_STM_LOW_HYSTERESIS,  "SAHPI_STM_LOW_HYSTERESIS"},
       {0,                         NULL},
};


/**
 * Translates a string to a valid SaHpiSensorThdMaskT type.
 *
 * @param strtype The entity type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiSensorThdMaskT oSaHpiSensorThdDefn::str2sensorthdmask(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return 0;
    }
	for (i = 0; sensorthdmask_strings[i].str != NULL; i++) {
		if (strcmp(strtype, sensorthdmask_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return sensorthdmask_strings[i].type;
	}
	return 0;
}


/**
 * Translates an sensor reading type to a string.
 *
 * @param value  The SaHpiSensorThdMaskT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiSensorThdDefn::sensorthdmask2str(SaHpiSensorThdMaskT value) {
	int i;

	for (i = 0; sensorthdmask_strings[i].str != NULL; i++) {
		if (value == sensorthdmask_strings[i].type) {
			return sensorthdmask_strings[i].str;
		}
	}
    return "Unknown";
}

