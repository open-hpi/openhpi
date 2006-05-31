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
#include "oSaHpiTextBuffer.hpp"


/**
 * Default constructor.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer() {
    DataType = SAHPI_TL_TYPE_TEXT;
    Language = SAHPI_LANG_ENGLISH;
    DataLength = 0;
    Data[0] = '\0';
};


/**
 * Constructor.
 *
 * @param type   The SaHpiTextType for the buffer.
 * @param lang   The SaHpiLanguage for the buffer.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer(const SaHpiTextTypeT type,
                                   const SaHpiLanguageT lang) {
    if (oSaHpiTypesEnums::texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPI_TL_TYPE_TEXT;
    }
    if (oSaHpiTypesEnums::language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPI_LANG_ENGLISH;
    }
    Data[0] = '\0';
};


/**
 * Constructor.
 *
 * @param type   The SaHpiTextType for the buffer.
 * @param lang   The SaHpiLanguage for the buffer.
 * @param str    The zero-terminated character string to be assigned to the
 *               buffer.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer(const SaHpiTextTypeT type,
                                   const SaHpiLanguageT lang,
                                   const char *str) {
    if (oSaHpiTypesEnums::texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPI_TL_TYPE_TEXT;
    }
    if (oSaHpiTypesEnums::language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPI_LANG_ENGLISH;
    }
    if (str == NULL) {
        Data[0] = '\0';
        DataLength = 0;
    }
    else {
        int str_len = strlen(str);
        if (str_len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
            strcpy((char *)Data, str);
            DataLength = (SaHpiUint8T)str_len;
        }
        else {
            memcpy(Data, str, SAHPI_MAX_TEXT_BUFFER_LENGTH);
            DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
        }
    }
};


/**
 * Constructor.
 *
 * @param type   The SaHpiTextType for the buffer.
 * @param lang   The SaHpiLanguage for the buffer.
 * @param str    The data array to be assigned to the buffer.
 * @param len    The length of the data to be assigned to the buffer.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer(const SaHpiTextTypeT type,
                                   const SaHpiLanguageT lang,
                                   const void *str,
                                   const SaHpiUint8T len) {
    if (oSaHpiTypesEnums::texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPI_TL_TYPE_TEXT;
    }
    if (oSaHpiTypesEnums::language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPI_LANG_ENGLISH;
    }
    DataLength = len;
    memcpy(Data, str, len);
    if (DataType == SAHPI_TL_TYPE_TEXT &&
     DataLength < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
        Data[len] = '\0';
    }
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer(const oSaHpiTextBuffer& buf) {
    DataType = buf.DataType;
    Language = buf.Language;
    DataLength = buf.DataLength;
    memcpy(Data, &buf.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
}


/**
 * Append a character string to the buffer.
 *
 * @param ptr    Pointer to the struct (class).
 * @param str    The zero-terminated character string to be appended to the
 *               buffer.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::append(SaHpiTextBufferT *ptr,
                              const char *str) {
    if (ptr == NULL) {
        return true;
    }
    if (str == NULL) {
        return false;
    }
    if (strlen(str) + ptr->DataLength > SAHPI_MAX_TEXT_BUFFER_LENGTH - 1) {
        return true;
    }
    strcat((char *)&ptr->Data[DataLength], str);
    ptr->DataLength = strlen((char *)ptr->Data);
    return false;
};


/**
 * Append an array of data to the buffer.
 *
 * @param ptr    Pointer to the struct (class).
 * @param str    The data to be appended.
 * @param len    The length of the data to be appended to the buffer.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::append(SaHpiTextBufferT *ptr,
                              const void *str,
                              const SaHpiUint8T len) {
    if (ptr == NULL) {
        return true;
    }
    if (str == NULL) {
        return false;
    }
    if (ptr->DataLength + len > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
        return true;
    }
    memcpy(&ptr->Data[DataLength], str, (size_t)len);
    ptr->DataLength += len;
    if (ptr->DataType == SAHPI_TL_TYPE_TEXT &&
     ptr->DataLength < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
        ptr->Data[len] = '\0';
    }
    return false;
};



/**
 * Assign a field in the SaHpiTextBufferT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::assignField(SaHpiTextBufferT *ptr,
                                   const char *field,
                                   const char *value) {
    // note that DataLength cannot be assigned a value using this method
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "DataType") == 0) {
        ptr->DataType = oSaHpiTypesEnums::str2texttype(value);
        return false;
    }
    else if (strcmp(field, "Language") == 0) {
        ptr->Language = oSaHpiTypesEnums::str2language(value);
        return false;
    }
    else if (strcmp(field, "Data") == 0) {
        int str_len = strlen(value);
        if (str_len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
            strcpy((char *)ptr->Data, value);
            ptr->DataLength = (SaHpiUint8T)str_len;
        }
        else {
            memcpy(ptr->Data, value, SAHPI_MAX_TEXT_BUFFER_LENGTH);
            ptr->DataLength = SAHPI_MAX_TEXT_BUFFER_LENGTH;
        }
        return false;
    }
    return true;
};


/**
 * Print the contents of the buffer.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiTextBuferT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::fprint(FILE *stream,
                              const int indent,
                              const SaHpiTextBufferT *buffer) {
	int i, err;
    char indent_buf[indent + 1];

    if (stream == NULL || buffer == NULL) {
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
    err = fprintf(stream, "DataType = %s\n", oSaHpiTypesEnums::texttype2str(buffer->DataType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Language = %s\n", oSaHpiTypesEnums::language2str(buffer->Language));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "DataLength = %d\n", (int)buffer->DataLength);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "%s", indent_buf);
    if (err < 0) {
        return true;
    }
    if (buffer->DataType == SAHPI_TL_TYPE_TEXT) {
        err = fprintf(stream, "Data = %s\n", buffer->Data);
    }
	else {
        err = fprintf(stream, "Data = ");
        if (err < 0) {
            return true;
        }
        for (i = 0; i < buffer->DataLength; i++) {
            err = fprintf(stream, "%c", buffer->Data[i]);
            if (err < 0) {
                return true;
            }
        }
        err = fprintf(stream, "\n");
        if (err < 0) {
            return true;
        }
	}
    if (err < 0) {
        return true;
    }

	return false;
}

