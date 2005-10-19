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
#include "oSaHpiEntityPath.hpp"
#include "oSaHpiTextBuffer.hpp"
#include "oSaHpiCondition.hpp"


/**
 * Default constructor.
 */
oSaHpiCondition::oSaHpiCondition() {
    Type = SAHPI_STATUS_COND_TYPE_SENSOR;
    Entity.Entry[0].EntityType = SAHPI_ENT_ROOT;
    Entity.Entry[0].EntityLocation = 0;
    DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
    ResourceId = 1;
    SensorNum = 1;
    EventState = SAHPI_ES_UNSPECIFIED;
    Name.Length = 0;
    Name.Value[0] = '\0';
    Mid = 0;
    Data.DataType = SAHPI_TL_TYPE_TEXT;
    Data.Language = SAHPI_LANG_ENGLISH;
    Data.DataLength = 0;
    Data.Data[0] = '\0';
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiCondition::oSaHpiCondition(const oSaHpiCondition& buf) {
    memcpy(this, &buf, sizeof(SaHpiConditionT));
}



/**
 * Assign a field in the SaHpiConditionT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiCondition::assignField(SaHpiConditionT *ptr,
                                  const char *field,
                                  const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "Type") == 0) {
        ptr->Type = oSaHpiTypesEnums::str2statuscondtype(value);
        return false;
    }
    // Entity
    else if (strcmp(field, "DomainId") == 0) {
        if (strcmp(value, "SAHPI_UNSPECIFIED_DOMAIN_ID") == 0) {
            ptr->DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID;
        }
        else {
            ptr->DomainId = atoi(value);
        }
        return false;
    }
    else if (strcmp(field, "ResourceId") == 0) {
        ptr->ResourceId = atoi(value);
        return false;
    }
    else if (strcmp(field, "SensorNum") == 0) {
        ptr->SensorNum = atoi(value);
        return false;
    }
    else if (strcmp(field, "EventState") == 0) {
        ptr->EventState = oSaHpiTypesEnums::str2eventstate(value);
        return false;
    }
    else if (strcmp(field, "Name") == 0) {
        if (strlen(value) < SA_HPI_MAX_NAME_LENGTH) {
            Name.Length = strlen(value);
            strcpy((char *)Name.Value, value);
        }
        else {
            Name.Length = SA_HPI_MAX_NAME_LENGTH;
            memcpy(Name.Value, value, SA_HPI_MAX_NAME_LENGTH);
        }
        return false;
    }
    else if (strcmp(field, "Mid") == 0) {
        ptr->Mid = atoi(value);
        return false;
    }
    else if (strcmp(field, "Data") == 0) {
        Data.DataType = SAHPI_TL_TYPE_TEXT;
        Data.Language = SAHPI_LANG_ENGLISH;
        if (strlen(value) < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
            Data.DataLength = strlen(value);
            strcpy((char *)Data.Data, value);
        }
        else {
            Name.Length = SAHPI_MAX_TEXT_BUFFER_LENGTH;
            memcpy(Data.Data, value, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        }
        return false;
    }
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiConditionT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiCondition::fprint(FILE *stream,
                             const int indent,
                             const SaHpiConditionT *buffer) {
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
    err = fprintf(stream, "Type = %s\n", oSaHpiTypesEnums::statuscondtype2str(buffer->Type));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Entity\n");
    oSaHpiEntityPath * ep = (oSaHpiEntityPath *)&buffer->Entity;
    err = ep->oSaHpiEntityPath::fprint(stream, indent + 3, (SaHpiEntityPathT *)ep);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ResourceId = %d\n", buffer->ResourceId);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "SensorNum = %d\n", buffer->SensorNum);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EventState = %s\n", oSaHpiTypesEnums::eventstate2str(buffer->EventState));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Name = ");
    for (i = 0; i < buffer->Name.Length; i++) {
        err = fprintf(stream, "%c\n", buffer->Name.Value[i]);
        if (err < 0) {
            return true;
        }
    }
    err = fprintf(stream, "\n");
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Mid = %d\n", buffer->Mid);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Data\n");
    oSaHpiTextBuffer * tb = (oSaHpiTextBuffer *)&buffer->Data;
    err = tb->oSaHpiTextBuffer::fprint(stream, indent + 3, (SaHpiTextBufferT *)tb);
    if (err < 0) {
        return true;
    }

	return false;
}

