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
SaHpiLanguageT oSaHpiTypesEnums::str2language(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPI_LANG_ENGLISH;
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
	return SAHPI_LANG_ENGLISH;
}


/**
 * Translates a language type to a string.
 *
 * @param value  The SaHpiLanguageT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTypesEnums::language2str(SaHpiLanguageT value) {
	int i;

	for (i = 0; language_strings[i].str != NULL; i++) {
		if (value == language_strings[i].type) {
			return language_strings[i].str;
		}
	}
    return "Unknown";
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
   {SAHPI_TL_TYPE_BINARY,  NULL}
};


/**
 * Translate a string into a valid SaHpiTextTypeT value.
 *
 * @param type   The string to be translated.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiTextTypeT oSaHpiTypesEnums::str2texttype(const char *type) {
	int i, found = 0;

    if (type == NULL) {
        return SAHPI_TL_TYPE_TEXT;
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
	return SAHPI_TL_TYPE_TEXT;
}


/**
 * Translates a text type to a string.
 *
 * @param value  The SaHpiTextType to be converted.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
const char * oSaHpiTypesEnums::texttype2str(SaHpiTextTypeT value) {
	int i;

	for (i = 0; texttype_strings[i].typestr != NULL; i++) {
		if (value == texttype_strings[i].type) {
			return texttype_strings[i].typestr;
		}
	}
    return "Unknown";
}


static struct entitytype_map {
    SaHpiEntityTypeT type;
    const char       *str;
} entitytype_strings[] = {
   {SAHPI_ENT_UNSPECIFIED,              "SAHPI_ENT_UNSPECIFIED"},
   {SAHPI_ENT_OTHER,                    "SAHPI_ENT_OTHER"},
   {SAHPI_ENT_UNKNOWN,                  "SAHPI_ENT_UNKNOWN"},
   {SAHPI_ENT_PROCESSOR,                "SAHPI_ENT_PROCESSOR"},
   {SAHPI_ENT_DISK_BAY,                 "SAHPI_ENT_DISK_BAY"},
   {SAHPI_ENT_PERIPHERAL_BAY,           "SAHPI_ENT_PERIPHERAL_BAY"},
   {SAHPI_ENT_SYS_MGMNT_MODULE,         "SAHPI_ENT_SYS_MGMNT_MODULE"},
   {SAHPI_ENT_SYSTEM_BOARD,             "SAHPI_ENT_SYSTEM_BOARD"},
   {SAHPI_ENT_MEMORY_MODULE,            "SAHPI_ENT_MEMORY_MODULE"},
   {SAHPI_ENT_PROCESSOR_MODULE,         "SAHPI_ENT_PROCESSOR_MODULE"},
   {SAHPI_ENT_POWER_SUPPLY,             "SAHPI_ENT_POWER_SUPPLY"},
   {SAHPI_ENT_ADD_IN_CARD,              "SAHPI_ENT_ADD_IN_CARD"},
   {SAHPI_ENT_FRONT_PANEL_BOARD,        "SAHPI_ENT_FRONT_PANEL_BOARD"},
   {SAHPI_ENT_BACK_PANEL_BOARD,         "SAHPI_ENT_BACK_PANEL_BOARD"},
   {SAHPI_ENT_POWER_SYSTEM_BOARD,       "SAHPI_ENT_POWER_SYSTEM_BOARD"},
   {SAHPI_ENT_DRIVE_BACKPLANE,          "SAHPI_ENT_DRIVE_BACKPLANE"},
   {SAHPI_ENT_SYS_EXPANSION_BOARD,      "SAHPI_ENT_SYS_EXPANSION_BOARD"},
   {SAHPI_ENT_OTHER_SYSTEM_BOARD,       "SAHPI_ENT_OTHER_SYSTEM_BOARD"},
   {SAHPI_ENT_PROCESSOR_BOARD,          "SAHPI_ENT_PROCESSOR_BOARD"},
   {SAHPI_ENT_POWER_UNIT,               "SAHPI_ENT_POWER_UNIT"},
   {SAHPI_ENT_POWER_MODULE,             "SAHPI_ENT_POWER_MODULE"},
   {SAHPI_ENT_POWER_MGMNT,              "SAHPI_ENT_POWER_MGMNT"},
   {SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD, "SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD"},
   {SAHPI_ENT_SYSTEM_CHASSIS,           "SAHPI_ENT_SYSTEM_CHASSIS"},
   {SAHPI_ENT_SUB_CHASSIS,              "SAHPI_ENT_SUB_CHASSIS"},
   {SAHPI_ENT_OTHER_CHASSIS_BOARD,      "SAHPI_ENT_OTHER_CHASSIS_BOARD"},
   {SAHPI_ENT_DISK_DRIVE_BAY,           "SAHPI_ENT_DISK_DRIVE_BAY"},
   {SAHPI_ENT_PERIPHERAL_BAY_2,         "SAHPI_ENT_PERIPHERAL_BAY_2"},
   {SAHPI_ENT_DEVICE_BAY,               "SAHPI_ENT_DEVICE_BAY"},
   {SAHPI_ENT_COOLING_DEVICE,           "SAHPI_ENT_COOLING_DEVICE"},
   {SAHPI_ENT_COOLING_UNIT,             "SAHPI_ENT_COOLING_UNIT"},
   {SAHPI_ENT_INTERCONNECT,             "SAHPI_ENT_INTERCONNECT"},
   {SAHPI_ENT_MEMORY_DEVICE,            "SAHPI_ENT_MEMORY_DEVICE"},
   {SAHPI_ENT_SYS_MGMNT_SOFTWARE,       "SAHPI_ENT_SYS_MGMNT_SOFTWARE"},
   {SAHPI_ENT_BIOS,                     "SAHPI_ENT_BIOS"},
   {SAHPI_ENT_OPERATING_SYSTEM,         "SAHPI_ENT_OPERATING_SYSTEM"},
   {SAHPI_ENT_SYSTEM_BUS,               "SAHPI_ENT_SYSTEM_BUS"},
   {SAHPI_ENT_GROUP,                    "SAHPI_ENT_GROUP"},
   {SAHPI_ENT_REMOTE,                   "SAHPI_ENT_REMOTE"},
   {SAHPI_ENT_EXTERNAL_ENVIRONMENT,     "SAHPI_ENT_EXTERNAL_ENVIRONMENT"},
   {SAHPI_ENT_BATTERY,                  "SAHPI_ENT_BATTERY"},
   {SAHPI_ENT_CHASSIS_SPECIFIC,         "SAHPI_ENT_CHASSIS_SPECIFIC"},
   {SAHPI_ENT_BOARD_SET_SPECIFIC,       "SAHPI_ENT_BOARD_SET_SPECIFIC"},
   {SAHPI_ENT_OEM_SYSINT_SPECIFIC,      "SAHPI_ENT_OEM_SYSINT_SPECIFIC"},
   {SAHPI_ENT_ROOT,                     "SAHPI_ENT_ROOT"},
   {SAHPI_ENT_RACK,                     "SAHPI_ENT_RACK"},
   {SAHPI_ENT_SUBRACK,                  "SAHPI_ENT_SUBRACK"},
   {SAHPI_ENT_COMPACTPCI_CHASSIS,       "SAHPI_ENT_COMPACTPCI_CHASSIS"},
   {SAHPI_ENT_ADVANCEDTCA_CHASSIS,      "SAHPI_ENT_ADVANCEDTCA_CHASSIS"},
   {SAHPI_ENT_RACK_MOUNTED_SERVER,      "SAHPI_ENT_RACK_MOUNTED_SERVER"},
   {SAHPI_ENT_SYSTEM_BLADE,             "SAHPI_ENT_SYSTEM_BLADE"},
   {SAHPI_ENT_SWITCH,                   "SAHPI_ENT_SWITCH"},
   {SAHPI_ENT_SWITCH_BLADE,             "SAHPI_ENT_SWITCH_BLADE"},
   {SAHPI_ENT_SBC_BLADE,                "SAHPI_ENT_SBC_BLADE"},
   {SAHPI_ENT_IO_BLADE,                 "SAHPI_ENT_IO_BLADE"},
   {SAHPI_ENT_DISK_BLADE,               "SAHPI_ENT_DISK_BLADE"},
   {SAHPI_ENT_DISK_DRIVE,               "SAHPI_ENT_DISK_DRIVE"},
   {SAHPI_ENT_FAN,                      "SAHPI_ENT_FAN"},
   {SAHPI_ENT_POWER_DISTRIBUTION_UNIT,  "SAHPI_ENT_POWER_DISTRIBUTION_UNIT"},
   {SAHPI_ENT_SPEC_PROC_BLADE,          "SAHPI_ENT_SPEC_PROC_BLADE"},
   {SAHPI_ENT_IO_SUBBOARD,              "SAHPI_ENT_IO_SUBBOARD"},
   {SAHPI_ENT_SBC_SUBBOARD,             "SAHPI_ENT_SBC_SUBBOARD"},
   {SAHPI_ENT_ALARM_MANAGER,            "SAHPI_ENT_ALARM_MANAGER"},
   {SAHPI_ENT_SHELF_MANAGER,            "SAHPI_ENT_SHELF_MANAGER"},
   {SAHPI_ENT_DISPLAY_PANEL,            "SAHPI_ENT_DISPLAY_PANEL"},
   {SAHPI_ENT_SUBBOARD_CARRIER_BLADE,   "SAHPI_ENT_SUBBOARD_CARRIER_BLADE"},
   {SAHPI_ENT_PHYSICAL_SLOT,            "SAHPI_ENT_PHYSICAL_SLOT"},
   {SAHPI_ENT_ROOT,                     NULL}
};


/**
 * Translates a string to a valid SaHpiEntityTypeT type.
 *
 * @param strtype The entity type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiEntityTypeT oSaHpiTypesEnums::str2entitytype(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPI_ENT_ROOT;
    }
	for (i = 0; entitytype_strings[i].str != NULL; i++) {
		if (strcmp(strtype, entitytype_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return entitytype_strings[i].type;
	}
	return SAHPI_ENT_ROOT;
}


/**
 * Translates an entity type to a string.
 *
 * @param value  The SaHpiEntityTypeT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTypesEnums::entitytype2str(SaHpiEntityTypeT value) {
	int i;

	for (i = 0; entitytype_strings[i].str != NULL; i++) {
		if (value == entitytype_strings[i].type) {
			return entitytype_strings[i].str;
		}
	}
    return "Unknown";
}


static struct sensorreadingtype_map {
    SaHpiSensorReadingTypeT type;
    const char              *str;
} sensorreadingtype_strings[] = {
       {SAHPI_SENSOR_READING_TYPE_INT64,   "SAHPI_SENSOR_READING_TYPE_INT64"},
       {SAHPI_SENSOR_READING_TYPE_UINT64,  "SAHPI_SENSOR_READING_TYPE_UINT64"},
       {SAHPI_SENSOR_READING_TYPE_FLOAT64, "SAHPI_SENSOR_READING_TYPE_FLOAT64"},
       {SAHPI_SENSOR_READING_TYPE_BUFFER,  "SAHPI_SENSOR_READING_TYPE_BUFFER"},
       {SAHPI_SENSOR_READING_TYPE_BUFFER,  NULL},
};


/**
 * Translates a string to a valid SaHpiSensorReadingTypeT type.
 *
 * @param strtype The entity type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiSensorReadingTypeT oSaHpiTypesEnums::str2sensorreadingtype(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPI_SENSOR_READING_TYPE_INT64;
    }
	for (i = 0; sensorreadingtype_strings[i].str != NULL; i++) {
		if (strcmp(strtype, sensorreadingtype_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return sensorreadingtype_strings[i].type;
	}
	return SAHPI_SENSOR_READING_TYPE_INT64;
}


/**
 * Translates an sensor reading type to a string.
 *
 * @param value  The SaHpiSensorReadingTypeT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTypesEnums::sensorreadingtype2str(SaHpiSensorReadingTypeT value) {
	int i;

	for (i = 0; sensorreadingtype_strings[i].str != NULL; i++) {
		if (value == sensorreadingtype_strings[i].type) {
			return sensorreadingtype_strings[i].str;
		}
	}
    return "Unknown";
}


static struct sensorunits_map {
    SaHpiSensorUnitsT type;
    const char        *str;
} sensorunits_strings[] = {
   {SAHPI_SU_UNSPECIFIED,          "SAHPI_SU_UNSPECIFIED"},
   {SAHPI_SU_DEGREES_C,            "SAHPI_SU_DEGREES_C"},
   {SAHPI_SU_DEGREES_F,            "SAHPI_SU_DEGREES_F"},
   {SAHPI_SU_DEGREES_K,            "SAHPI_SU_DEGREES_K"},
   {SAHPI_SU_VOLTS,                "SAHPI_SU_VOLTS"},
   {SAHPI_SU_AMPS,                 "SAHPI_SU_AMPS"},
   {SAHPI_SU_WATTS,                "SAHPI_SU_WATTS"},
   {SAHPI_SU_JOULES,               "SAHPI_SU_JOULES"},
   {SAHPI_SU_COULOMBS,             "SAHPI_SU_COULOMBS"},
   {SAHPI_SU_VA,                   "SAHPI_SU_VA"},
   {SAHPI_SU_NITS,                 "SAHPI_SU_NITS"},
   {SAHPI_SU_LUMEN,                "SAHPI_SU_LUMEN"},
   {SAHPI_SU_LUX,                  "SAHPI_SU_LUX"},
   {SAHPI_SU_CANDELA,              "SAHPI_SU_CANDELA"},
   {SAHPI_SU_KPA,                  "SAHPI_SU_KPA"},
   {SAHPI_SU_PSI,                  "SAHPI_SU_PSI"},
   {SAHPI_SU_NEWTON,               "SAHPI_SU_NEWTON"},
   {SAHPI_SU_CFM,                  "SAHPI_SU_CFM"},
   {SAHPI_SU_RPM,                  "SAHPI_SU_RPM"},
   {SAHPI_SU_HZ,                   "SAHPI_SU_HZ"},
   {SAHPI_SU_MICROSECOND,          "SAHPI_SU_MICROSECOND"},
   {SAHPI_SU_MILLISECOND,          "SAHPI_SU_MILLISECOND"},
   {SAHPI_SU_SECOND,               "SAHPI_SU_SECOND"},
   {SAHPI_SU_MINUTE,               "SAHPI_SU_MINUTE"},
   {SAHPI_SU_HOUR,                 "SAHPI_SU_HOUR"},
   {SAHPI_SU_DAY,                  "SAHPI_SU_DAY"},
   {SAHPI_SU_WEEK,                 "SAHPI_SU_WEEK"},
   {SAHPI_SU_MIL,                  "SAHPI_SU_MIL"},
   {SAHPI_SU_INCHES,               "SAHPI_SU_INCHES"},
   {SAHPI_SU_FEET,                 "SAHPI_SU_FEET"},
   {SAHPI_SU_CU_IN,                "SAHPI_SU_CU_IN"},
   {SAHPI_SU_CU_FEET,              "SAHPI_SU_CU_FEET"},
   {SAHPI_SU_MM,                   "SAHPI_SU_MM"},
   {SAHPI_SU_CM,                   "SAHPI_SU_CM"},
   {SAHPI_SU_M,                    "SAHPI_SU_M"},
   {SAHPI_SU_CU_CM,                "SAHPI_SU_CU_CM"},
   {SAHPI_SU_CU_M,                 "SAHPI_SU_CU_M"},
   {SAHPI_SU_LITERS,               "SAHPI_SU_LITERS"},
   {SAHPI_SU_FLUID_OUNCE,          "SAHPI_SU_FLUID_OUNCE"},
   {SAHPI_SU_RADIANS,              "SAHPI_SU_RADIANS"},
   {SAHPI_SU_STERADIANS,           "SAHPI_SU_STERADIANS"},
   {SAHPI_SU_REVOLUTIONS,          "SAHPI_SU_REVOLUTIONS"},
   {SAHPI_SU_CYCLES,               "SAHPI_SU_CYCLES"},
   {SAHPI_SU_GRAVITIES,            "SAHPI_SU_GRAVITIES"},
   {SAHPI_SU_OUNCE,                "SAHPI_SU_OUNCE"},
   {SAHPI_SU_POUND,                "SAHPI_SU_POUND"},
   {SAHPI_SU_FT_LB,                "SAHPI_SU_FT_LB"},
   {SAHPI_SU_OZ_IN,                "SAHPI_SU_OZ_IN"},
   {SAHPI_SU_GAUSS,                "SAHPI_SU_GAUSS"},
   {SAHPI_SU_GILBERTS,             "SAHPI_SU_GILBERTS"},
   {SAHPI_SU_HENRY,                "SAHPI_SU_HENRY"},
   {SAHPI_SU_MILLIHENRY,           "SAHPI_SU_MILLIHENRY"},
   {SAHPI_SU_FARAD,                "SAHPI_SU_FARAD"},
   {SAHPI_SU_MICROFARAD,           "SAHPI_SU_MICROFARAD"},
   {SAHPI_SU_OHMS,                 "SAHPI_SU_OHMS"},
   {SAHPI_SU_SIEMENS,              "SAHPI_SU_SIEMENS"},
   {SAHPI_SU_MOLE,                 "SAHPI_SU_MOLE"},
   {SAHPI_SU_BECQUEREL,            "SAHPI_SU_BECQUEREL"},
   {SAHPI_SU_PPM,                  "SAHPI_SU_PPM"},
   {SAHPI_SU_RESERVED,             "SAHPI_SU_RESERVED"},
   {SAHPI_SU_DECIBELS,             "SAHPI_SU_DECIBELS"},
   {SAHPI_SU_DBA,                  "SAHPI_SU_DBA"},
   {SAHPI_SU_DBC,                  "SAHPI_SU_DBC"},
   {SAHPI_SU_GRAY,                 "SAHPI_SU_GRAY"},
   {SAHPI_SU_SIEVERT,              "SAHPI_SU_SIEVERT"},
   {SAHPI_SU_COLOR_TEMP_DEG_K,     "SAHPI_SU_COLOR_TEMP_DEG_K"},
   {SAHPI_SU_BIT,                  "SAHPI_SU_BIT"},
   {SAHPI_SU_KILOBIT,              "SAHPI_SU_KILOBIT"},
   {SAHPI_SU_MEGABIT,              "SAHPI_SU_MEGABIT"},
   {SAHPI_SU_GIGABIT,              "SAHPI_SU_GIGABIT"},
   {SAHPI_SU_BYTE,                 "SAHPI_SU_BYTE"},
   {SAHPI_SU_KILOBYTE,             "SAHPI_SU_KILOBYTE"},
   {SAHPI_SU_MEGABYTE,             "SAHPI_SU_MEGABYTE"},
   {SAHPI_SU_GIGABYTE,             "SAHPI_SU_GIGABYTE"},
   {SAHPI_SU_WORD,                 "SAHPI_SU_WORD"},
   {SAHPI_SU_DWORD,                "SAHPI_SU_DWORD"},
   {SAHPI_SU_QWORD,                "SAHPI_SU_QWORD"},
   {SAHPI_SU_LINE,                 "SAHPI_SU_LINE"},
   {SAHPI_SU_HIT,                  "SAHPI_SU_HIT"},
   {SAHPI_SU_MISS,                 "SAHPI_SU_MISS"},
   {SAHPI_SU_RETRY,                "SAHPI_SU_RETRY"},
   {SAHPI_SU_RESET,                "SAHPI_SU_RESET"},
   {SAHPI_SU_OVERRUN,              "SAHPI_SU_OVERRUN"},
   {SAHPI_SU_UNDERRUN,             "SAHPI_SU_UNDERRUN"},
   {SAHPI_SU_COLLISION,            "SAHPI_SU_COLLISION"},
   {SAHPI_SU_PACKETS,              "SAHPI_SU_PACKETS"},
   {SAHPI_SU_MESSAGES,             "SAHPI_SU_MESSAGES"},
   {SAHPI_SU_CHARACTERS,           "SAHPI_SU_CHARACTERS"},
   {SAHPI_SU_ERRORS,               "SAHPI_SU_ERRORS"},
   {SAHPI_SU_CORRECTABLE_ERRORS,   "SAHPI_SU_CORRECTABLE_ERRORS"},
   {SAHPI_SU_UNCORRECTABLE_ERRORS, "SAHPI_SU_UNCORRECTABLE_ERRORS"},
   {SAHPI_SU_UNSPECIFIED,          NULL}
};


/**
 * Translates a string to a valid SaHpiSensorUnitsT type.
 *
 * @param strtype The entity type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiSensorUnitsT oSaHpiTypesEnums::str2sensorunits(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPI_SU_UNSPECIFIED;
    }
	for (i = 0; sensorunits_strings[i].str != NULL; i++) {
		if (strcmp(strtype, sensorunits_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return sensorunits_strings[i].type;
	}
    return SAHPI_SU_UNSPECIFIED;
}


/**
 * Translates an sensor reading type to a string.
 *
 * @param value  The SaHpiSensorUnitsT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTypesEnums::sensorunits2str(SaHpiSensorUnitsT value) {
	int i;

	for (i = 0; sensorunits_strings[i].str != NULL; i++) {
		if (value == sensorunits_strings[i].type) {
			return sensorunits_strings[i].str;
		}
	}
    return "Unknown";
}


static struct sensormodunituse_map {
    SaHpiSensorModUnitUseT type;
    const char             *str;
} sensormodunituse_strings[] = {
   {SAHPI_SMUU_NONE,                 "SAHPI_SMUU_NONE"},
   {SAHPI_SMUU_BASIC_OVER_MODIFIER,  "SAHPI_SMUU_BASIC_OVER_MODIFIER"},
   {SAHPI_SMUU_BASIC_TIMES_MODIFIER, "SAHPI_SMUU_BASIC_TIMES_MODIFIER"},
   {SAHPI_SMUU_NONE,                 NULL}
};


/**
 * Translates a string to a valid SaHpiSensorModUnitsUseT type.
 *
 * @param strtype The entity type expressed as a string.
 *
 * @return SAHPI_OK on success, otherwise an HPI error code.
 */
SaHpiSensorModUnitUseT oSaHpiTypesEnums::str2sensoruse(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPI_SMUU_NONE;
    }
	for (i = 0; sensormodunituse_strings[i].str != NULL; i++) {
		if (strcmp(strtype, sensormodunituse_strings[i].str) == 0) {
			found++;
			break;
		}
	}

	if (found) {
		return sensormodunituse_strings[i].type;
	}
    return SAHPI_SMUU_NONE;
}


/**
 * Translates an sensor reading type to a string.
 *
 * @param value  The SaHpiSensorModUnitUseT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiTypesEnums::sensoruse2str(SaHpiSensorModUnitUseT value) {
	int i;

	for (i = 0; sensormodunituse_strings[i].str != NULL; i++) {
		if (value == sensormodunituse_strings[i].type) {
			return sensormodunituse_strings[i].str;
		}
	}
    return "Unknown";
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
SaHpiSensorThdMaskT oSaHpiTypesEnums::str2sensorthdmask(const char *strtype) {
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
const char * oSaHpiTypesEnums::sensorthdmask2str(SaHpiSensorThdMaskT value) {
	int i;

	for (i = 0; sensorthdmask_strings[i].str != NULL; i++) {
		if (value == sensorthdmask_strings[i].type) {
			return sensorthdmask_strings[i].str;
		}
	}
    return "Unknown";
}


