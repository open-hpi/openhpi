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
        ptr->IsAccessible = oSaHpiTypesEnums::str2torf(value);
        return false;
    }
    else if (strcmp(field, "ReadThold") == 0) {
        ptr->ReadThold |= oSaHpiTypesEnums::str2sensorthdmask(value);
        return false;
    }
    else if (strcmp(field, "WriteThold") == 0) {
        ptr->WriteThold |= oSaHpiTypesEnums::str2sensorthdmask(value);
        return false;
    }
    else if (strcmp(field, "Nonlinear") == 0) {
        ptr->Nonlinear = oSaHpiTypesEnums::str2torf(value);
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
    err = fprintf(stream, "IsAccessible = %s\n", oSaHpiTypesEnums::torf2str(df->IsAccessible));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ReadThold = %X\n", df->ReadThold);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "WriteThold = %X\n", df->WriteThold);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Nonlinear = %s\n", oSaHpiTypesEnums::torf2str(df->Nonlinear));
    if (err < 0) {
        return true;
    }

	return false;
}

