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
#include "oSaHpiTextBuffer.hpp"


/**
 * Default constructor.
 */
oSaHpiTextBuffer::oSaHpiTextBuffer() {
    DataType = SAHPITEXTTYPET_DEFAULT;
    Language = SAHPILANGUAGET_DEFAULT;
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
    if (texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPITEXTTYPET_DEFAULT;
    }
    if (language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPILANGUAGET_DEFAULT;
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
    if (texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPITEXTTYPET_DEFAULT;
    }
    if (language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPILANGUAGET_DEFAULT;
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
    if (texttype2str(type)) {
        DataType = type;
    }
    else {
        DataType = SAHPITEXTTYPET_DEFAULT;
    }
    if (language2str(lang)) {
        Language = lang;
    }
    else {
        Language = SAHPILANGUAGET_DEFAULT;
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
 * Destructor.
 */
oSaHpiTextBuffer::~oSaHpiTextBuffer() {
}


/**
 * Append a character string to the buffer.
 *
 * @param str    The zero-terminated character string to be appended to the
 *               buffer.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::append(const char *str) {
    return append(this, str);
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
 * @param str    The data to be appended.
 * @param len    The length of the data to be appended to the buffer.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::append(const void *str,
                              const SaHpiUint8T len) {
    return append(this, str, len);
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
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiTextBuffer::assignField(const char *field,
                                   const char *value) {
    return assignField(this, field, value);
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
        ptr->DataType = str2texttype(value);
        return false;
    }
    else if (strcmp(field, "Language") == 0) {
        ptr->Language = str2language(value);
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
    err = fprintf(stream, "DataType = %s\n", texttype2str(buffer->DataType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Language = %s\n", language2str(buffer->Language));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "DataLength = %d\n", (int)buffer->DataLength);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    if (buffer->DataType == SAHPI_TL_TYPE_TEXT) {
        err = fprintf(stream, "Data = %s\n", buffer->Data);
    }
	else {
        err = fprintf(stream, "(unknown value)");
	}
    if (err < 0) {
        return true;
    }

	return false;
}


static struct language_map {
    SaHpiLanguageT type;
    const char     *str;
} language_strings[] = {
   {SAHPI_LANG_UNDEF,          "SAHPI_LANG_UNDEF"},
   {SAHPI_LANG_AFAR,           "SAHPI_LANG_AFAR"},
   {SAHPI_LANG_ABKHAZIAN,      "SAHPI_LANG_ABKHAZIAN"},
   {SAHPI_LANG_AFRIKAANS,      "SAHPI_LANG_AFRIKAANS"},
   {SAHPI_LANG_AMHARIC,        "SAHPI_LANG_AMHARIC"},
   {SAHPI_LANG_ARABIC,         "SAHPI_LANG_ARABIC"},
   {SAHPI_LANG_ASSAMESE,       "SAHPI_LANG_ASSAMESE"},
   {SAHPI_LANG_AYMARA,         "SAHPI_LANG_AYMARA"},
   {SAHPI_LANG_AZERBAIJANI,    "SAHPI_LANG_AZERBAIJANI"},
   {SAHPI_LANG_BASHKIR,        "SAHPI_LANG_BASHKIR"},
   {SAHPI_LANG_BYELORUSSIAN,   "SAHPI_LANG_BYELORUSSIAN"},
   {SAHPI_LANG_BULGARIAN,      "SAHPI_LANG_BULGARIAN"},
   {SAHPI_LANG_BIHARI,         "SAHPI_LANG_BIHARI"},
   {SAHPI_LANG_BISLAMA,        "SAHPI_LANG_BISLAMA"},
   {SAHPI_LANG_BENGALI,        "SAHPI_LANG_BENGALI"},
   {SAHPI_LANG_TIBETAN,        "SAHPI_LANG_TIBETAN"},
   {SAHPI_LANG_BRETON,         "SAHPI_LANG_BRETON"},
   {SAHPI_LANG_CATALAN,        "SAHPI_LANG_CATALAN"},
   {SAHPI_LANG_CORSICAN,       "SAHPI_LANG_CORSICAN"},
   {SAHPI_LANG_CZECH,          "SAHPI_LANG_CZECH"},
   {SAHPI_LANG_WELSH,          "SAHPI_LANG_WELSH"},
   {SAHPI_LANG_DANISH,         "SAHPI_LANG_DANISH"},
   {SAHPI_LANG_GERMAN,         "SAHPI_LANG_GERMAN"},
   {SAHPI_LANG_BHUTANI,        "SAHPI_LANG_BHUTANI"},
   {SAHPI_LANG_GREEK,          "SAHPI_LANG_GREEK"},
   {SAHPI_LANG_ENGLISH,        "SAHPI_LANG_ENGLISH"},
   {SAHPI_LANG_ESPERANTO,      "SAHPI_LANG_ESPERANTO"},
   {SAHPI_LANG_SPANISH,        "SAHPI_LANG_SPANISH"},
   {SAHPI_LANG_ESTONIAN,       "SAHPI_LANG_ESTONIAN"},
   {SAHPI_LANG_BASQUE,         "SAHPI_LANG_BASQUE"},
   {SAHPI_LANG_PERSIAN,        "SAHPI_LANG_PERSIAN"},
   {SAHPI_LANG_FINNISH,        "SAHPI_LANG_FINNISH"},
   {SAHPI_LANG_FIJI,           "SAHPI_LANG_FIJI"},
   {SAHPI_LANG_FAEROESE,       "SAHPI_LANG_FAEROESE"},
   {SAHPI_LANG_FRENCH,         "SAHPI_LANG_FRENCH"},
   {SAHPI_LANG_FRISIAN,        "SAHPI_LANG_FRISIAN"},
   {SAHPI_LANG_IRISH,          "SAHPI_LANG_IRISH"},
   {SAHPI_LANG_SCOTSGAELIC,    "SAHPI_LANG_SCOTSGAELIC"},
   {SAHPI_LANG_GALICIAN,       "SAHPI_LANG_GALICIAN"},
   {SAHPI_LANG_GUARANI,        "SAHPI_LANG_GUARANI"},
   {SAHPI_LANG_GUJARATI,       "SAHPI_LANG_GUJARATI"},
   {SAHPI_LANG_HAUSA,          "SAHPI_LANG_HAUSA"},
   {SAHPI_LANG_HINDI,          "SAHPI_LANG_HINDI"},
   {SAHPI_LANG_CROATIAN,       "SAHPI_LANG_CROATIAN"},
   {SAHPI_LANG_HUNGARIAN,      "SAHPI_LANG_HUNGARIAN"},
   {SAHPI_LANG_ARMENIAN,       "SAHPI_LANG_ARMENIAN"},
   {SAHPI_LANG_INTERLINGUA,    "SAHPI_LANG_INTERLINGUA"},
   {SAHPI_LANG_INTERLINGUE,    "SAHPI_LANG_INTERLINGUE"},
   {SAHPI_LANG_INUPIAK,        "SAHPI_LANG_INUPIAK"},
   {SAHPI_LANG_INDONESIAN,     "SAHPI_LANG_INDONESIAN"},
   {SAHPI_LANG_ITALIAN,        "SAHPI_LANG_ITALIAN"},
   {SAHPI_LANG_HEBREW,         "SAHPI_LANG_HEBREW"},
   {SAHPI_LANG_JAPANESE,       "SAHPI_LANG_JAPANESE"},
   {SAHPI_LANG_YIDDISH,        "SAHPI_LANG_YIDDISH"},
   {SAHPI_LANG_JAVANESE,       "SAHPI_LANG_JAVANESE"},
   {SAHPI_LANG_GEORGIAN,       "SAHPI_LANG_GEORGIAN"},
   {SAHPI_LANG_KAZAKH,         "SAHPI_LANG_KAZAKH"},
   {SAHPI_LANG_GREENLANDIC,    "SAHPI_LANG_GREENLANDIC"},
   {SAHPI_LANG_CAMBODIAN,      "SAHPI_LANG_CAMBODIAN"},
   {SAHPI_LANG_KANNADA,        "SAHPI_LANG_KANNADA"},
   {SAHPI_LANG_KOREAN,         "SAHPI_LANG_KOREAN"},
   {SAHPI_LANG_KASHMIRI,       "SAHPI_LANG_KASHMIRI"},
   {SAHPI_LANG_KURDISH,        "SAHPI_LANG_KURDISH"},
   {SAHPI_LANG_KIRGHIZ,        "SAHPI_LANG_KIRGHIZ"},
   {SAHPI_LANG_LATIN,          "SAHPI_LANG_LATIN"},
   {SAHPI_LANG_LINGALA,        "SAHPI_LANG_LINGALA"},
   {SAHPI_LANG_LAOTHIAN,       "SAHPI_LANG_LAOTHIAN"},
   {SAHPI_LANG_LITHUANIAN,     "SAHPI_LANG_LITHUANIAN"},
   {SAHPI_LANG_LATVIANLETTISH, "SAHPI_LANG_LATVIANLETTISH"},
   {SAHPI_LANG_MALAGASY,       "SAHPI_LANG_MALAGASY"},
   {SAHPI_LANG_MAORI,          "SAHPI_LANG_MAORI"},
   {SAHPI_LANG_MACEDONIAN,     "SAHPI_LANG_MACEDONIAN"},
   {SAHPI_LANG_MALAYALAM,      "SAHPI_LANG_MALAYALAM"},
   {SAHPI_LANG_MONGOLIAN,      "SAHPI_LANG_MONGOLIAN"},
   {SAHPI_LANG_MOLDAVIAN,      "SAHPI_LANG_MOLDAVIAN"},
   {SAHPI_LANG_MARATHI,        "SAHPI_LANG_MARATHI"},
   {SAHPI_LANG_MALAY,          "SAHPI_LANG_MALAY"},
   {SAHPI_LANG_MALTESE,        "SAHPI_LANG_MALTESE"},
   {SAHPI_LANG_BURMESE,        "SAHPI_LANG_BURMESE"},
   {SAHPI_LANG_NAURU,          "SAHPI_LANG_NAURU"},
   {SAHPI_LANG_NEPALI,         "SAHPI_LANG_NEPALI"},
   {SAHPI_LANG_DUTCH,          "SAHPI_LANG_DUTCH"},
   {SAHPI_LANG_NORWEGIAN,      "SAHPI_LANG_NORWEGIAN"},
   {SAHPI_LANG_OCCITAN,        "SAHPI_LANG_OCCITAN"},
   {SAHPI_LANG_AFANOROMO,      "SAHPI_LANG_AFANOROMO"},
   {SAHPI_LANG_ORIYA,          "SAHPI_LANG_ORIYA"},
   {SAHPI_LANG_PUNJABI,        "SAHPI_LANG_PUNJABI"},
   {SAHPI_LANG_POLISH,         "SAHPI_LANG_POLISH"},
   {SAHPI_LANG_PASHTOPUSHTO,   "SAHPI_LANG_PASHTOPUSHTO"},
   {SAHPI_LANG_PORTUGUESE,     "SAHPI_LANG_PORTUGUESE"},
   {SAHPI_LANG_QUECHUA,        "SAHPI_LANG_QUECHUA"},
   {SAHPI_LANG_RHAETOROMANCE,  "SAHPI_LANG_RHAETOROMANCE"},
   {SAHPI_LANG_KIRUNDI,        "SAHPI_LANG_KIRUNDI"},
   {SAHPI_LANG_ROMANIAN,       "SAHPI_LANG_ROMANIAN"},
   {SAHPI_LANG_RUSSIAN,        "SAHPI_LANG_RUSSIAN"},
   {SAHPI_LANG_KINYARWANDA,    "SAHPI_LANG_KINYARWANDA"},
   {SAHPI_LANG_SANSKRIT,       "SAHPI_LANG_SANSKRIT"},
   {SAHPI_LANG_SINDHI,         "SAHPI_LANG_SINDHI"},
   {SAHPI_LANG_SANGRO,         "SAHPI_LANG_SANGRO"},
   {SAHPI_LANG_SERBOCROATIAN,  "SAHPI_LANG_SERBOCROATIAN"},
   {SAHPI_LANG_SINGHALESE,     "SAHPI_LANG_SINGHALESE"},
   {SAHPI_LANG_SLOVAK,         "SAHPI_LANG_SLOVAK"},
   {SAHPI_LANG_SLOVENIAN,      "SAHPI_LANG_SLOVENIAN"},
   {SAHPI_LANG_SAMOAN,         "SAHPI_LANG_SAMOAN"},
   {SAHPI_LANG_SHONA,          "SAHPI_LANG_SHONA"},
   {SAHPI_LANG_SOMALI,         "SAHPI_LANG_SOMALI"},
   {SAHPI_LANG_ALBANIAN,       "SAHPI_LANG_ALBANIAN"},
   {SAHPI_LANG_SERBIAN,        "SAHPI_LANG_SERBIAN"},
   {SAHPI_LANG_SISWATI,        "SAHPI_LANG_SISWATI"},
   {SAHPI_LANG_SESOTHO,        "SAHPI_LANG_SESOTHO"},
   {SAHPI_LANG_SUDANESE,       "SAHPI_LANG_SUDANESE"},
   {SAHPI_LANG_SWEDISH,        "SAHPI_LANG_SWEDISH"},
   {SAHPI_LANG_SWAHILI,        "SAHPI_LANG_SWAHILI"},
   {SAHPI_LANG_TAMIL,          "SAHPI_LANG_TAMIL"},
   {SAHPI_LANG_TELUGU,         "SAHPI_LANG_TELUGU"},
   {SAHPI_LANG_TAJIK,          "SAHPI_LANG_TAJIK"},
   {SAHPI_LANG_THAI,           "SAHPI_LANG_THAI"},
   {SAHPI_LANG_TIGRINYA,       "SAHPI_LANG_TIGRINYA"},
   {SAHPI_LANG_TURKMEN,        "SAHPI_LANG_TURKMEN"},
   {SAHPI_LANG_TAGALOG,        "SAHPI_LANG_TAGALOG"},
   {SAHPI_LANG_SETSWANA,       "SAHPI_LANG_SETSWANA"},
   {SAHPI_LANG_TONGA,          "SAHPI_LANG_TONGA"},
   {SAHPI_LANG_TURKISH,        "SAHPI_LANG_TURKISH"},
   {SAHPI_LANG_TSONGA,         "SAHPI_LANG_TSONGA"},
   {SAHPI_LANG_TATAR,          "SAHPI_LANG_TATAR"},
   {SAHPI_LANG_TWI,            "SAHPI_LANG_TWI"},
   {SAHPI_LANG_UKRAINIAN,      "SAHPI_LANG_UKRAINIAN"},
   {SAHPI_LANG_URDU,           "SAHPI_LANG_URDU"},
   {SAHPI_LANG_UZBEK,          "SAHPI_LANG_UZBEK"},
   {SAHPI_LANG_VIETNAMESE,     "SAHPI_LANG_VIETNAMESE"},
   {SAHPI_LANG_VOLAPUK,        "SAHPI_LANG_VOLAPUK"},
   {SAHPI_LANG_WOLOF,          "SAHPI_LANG_WOLOF"},
   {SAHPI_LANG_XHOSA,          "SAHPI_LANG_XHOSA"},
   {SAHPI_LANG_YORUBA,         "SAHPI_LANG_YORUBA"},
   {SAHPI_LANG_CHINESE,        "SAHPI_LANG_CHINESE"},
   {SAHPI_LANG_ZULU,           "SAHPI_LANG_ZULU"},
   {SAHPI_LANG_UNDEF,          NULL}
};


/**
 * Translates a string to a valid SaHpiLanguageT type.
 *
 * @param strtype The language type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiLanguageT oSaHpiTextBuffer::str2language(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPILANGUAGET_DEFAULT;
    }
	for (i = 0; language_strings[i].str != NULL; i++) {
		if (strcmp(strtype, language_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return language_strings[i].type;
	}
	return SAHPILANGUAGET_DEFAULT;
}


/**
 * Translates a language type to a string.
 *
 * @param value  The SaHpiLanguageT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTextBuffer::language2str(SaHpiLanguageT value) {
	int i;

	for (i = 0; language_strings[i].str != NULL; i++) {
		if (value == language_strings[i].type) {
			return language_strings[i].str;
		}
	}
    return language_strings[0].str;
}


static struct texttype_map {
    SaHpiTextTypeT type;
    const char     *typestr;
} texttype_strings[] = {
   {SAHPI_TL_TYPE_UNICODE, "SAHPI_TL_TYPE_UNICODE"},
   {SAHPI_TL_TYPE_BCDPLUS, "SAHPI_TL_TYPE_BCDPLUS"},
   {SAHPI_TL_TYPE_ASCII6,  "SAHPI_TL_TYPE_ASCII6"},
   {SAHPI_TL_TYPE_TEXT,    "SAHPI_TL_TYPE_TEXT"},
   {SAHPI_TL_TYPE_BINARY,  "SAHPI_TL_TYPE_BINARY"},
   {SAHPI_TL_TYPE_TEXT,    NULL}
};


/**
 * Translates a text type to a string.
 *
 * @param value  The SaHpiTextType to be converted.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
const char * oSaHpiTextBuffer::texttype2str(SaHpiTextTypeT value) {
	int i;

	for (i = 0; texttype_strings[i].typestr != NULL; i++) {
		if (value == texttype_strings[i].type) {
			return texttype_strings[i].typestr;
		}
	}
    return "Unknown";
}


/**
 * Translate a string into a valid SaHpiTextTypeT value.
 *
 * @param type   The string to be translated.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiTextTypeT oSaHpiTextBuffer::str2texttype(const char *type) {
	int i, found = 0;

    if (type == NULL) {
        return SAHPITEXTTYPET_DEFAULT;
    }
	for (i = 0; texttype_strings[i].typestr != NULL; i++) {
		if (strcmp(type, texttype_strings[i].typestr) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return texttype_strings[i].type;
	}
	return SAHPITEXTTYPET_DEFAULT;
}

