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
#include "oSaHpiSensorReading.hpp"
#include "oSaHpiSensorRange.hpp"
#include "oSaHpiSensorDataFormat.hpp"


/**
 * Default constructor.
 */
oSaHpiSensorDataFormat::oSaHpiSensorDataFormat() {
    oSaHpiSensorReading *sr;

    IsSupported = 0;
    ReadingType = SAHPISENSORREADINGTYPET_DEFAULT;
    BaseUnits = SAHPI_SU_UNSPECIFIED;
    ModifierUnits = SAHPI_SU_UNSPECIFIED;
    ModifierUse = SAHPI_SMUU_NONE;
    Percentage = false;
    Range.Flags = 0;
    sr = (oSaHpiSensorReading *)&Range.Max;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Range.Min;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Range.Nominal;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Range.NormalMax;
    sr->initSensorReading(sr);
    sr = (oSaHpiSensorReading *)&Range.NormalMin;
    sr->initSensorReading(sr);
    AccuracyFactor = 0;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiSensorDataFormat::oSaHpiSensorDataFormat(const oSaHpiSensorDataFormat& df) {
    memcpy(this, &df, sizeof(SaHpiSensorDataFormatT));
}


/**
 * Assign a field in the SaHpiSensorDataFormatT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiSensorDataFormat::assignField(SaHpiSensorDataFormatT *ptr,
                                         const char *field,
                                         const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "IsSupported") == 0) {
        ptr->IsSupported = (SaHpiBoolT)atoi(value);
        return false;
    }
    else if (strcmp(field, "ReadingType") == 0) {
        ptr->ReadingType = str2sensorreadingtype(value);
        return false;
    }
    else if (strcmp(field, "BaseUnits") == 0) {
        ptr->BaseUnits = str2sensorunits(value);
        return false;
    }
    else if (strcmp(field, "ModifierUnits") == 0) {
        ptr->ModifierUnits = str2sensorunits(value);
        return false;
    }
    else if (strcmp(field, "ModifierUse") == 0) {
        ptr->ModifierUse = str2sensoruse(value);
        return false;
    }
    else if (strcmp(field, "Percentage") == 0) {
        ptr->Percentage = (SaHpiBoolT)atoi(value);
        return false;
    }
    else if (strcmp(field, "AccuracyFactor") == 0) {
        ptr->AccuracyFactor = (SaHpiFloat64T)atof(value);
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
bool oSaHpiSensorDataFormat::fprint(FILE *stream,
                                    const int indent,
                                    const SaHpiSensorDataFormatT *df) {
	int i, err = 0;
    char indent_buf[indent + 1];
    oSaHpiSensorRange *sr;

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
    err = fprintf(stream, "IsSupported = %d\n", df->IsSupported);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ReadingType = %s\n", sensorreadingtype2str(df->ReadingType));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "BaseUnits = %s\n", sensorunits2str(df->BaseUnits));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ModifierUnits = %s\n", sensorunits2str(df->ModifierUnits));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "ModifierUse = %s\n", sensoruse2str(df->ModifierUse));
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Percentage = %d\n", df->Percentage);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "Range\n");
    if (err < 0) {
        return true;
    }
    sr = (oSaHpiSensorRange *)&df->Range;
    err = sr->oSaHpiSensorRange::fprint(stream, indent + 3, sr);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "AccuracyFactor = %f\n", df->AccuracyFactor);
    if (err < 0) {
        return true;
    }

	return false;
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
SaHpiSensorReadingTypeT oSaHpiSensorDataFormat::str2sensorreadingtype(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPISENSORREADINGTYPET_DEFAULT;
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
	return SAHPISENSORREADINGTYPET_DEFAULT;
}


/**
 * Translates an sensor reading type to a string.
 *
 * @param value  The SaHpiSensorReadingTypeT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiSensorDataFormat::sensorreadingtype2str(SaHpiSensorReadingTypeT value) {
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
SaHpiSensorUnitsT oSaHpiSensorDataFormat::str2sensorunits(const char *strtype) {
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
const char * oSaHpiSensorDataFormat::sensorunits2str(SaHpiSensorUnitsT value) {
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
SaHpiSensorModUnitUseT oSaHpiSensorDataFormat::str2sensoruse(const char *strtype) {
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
const char * oSaHpiSensorDataFormat::sensoruse2str(SaHpiSensorModUnitUseT value) {
	int i;

	for (i = 0; sensormodunituse_strings[i].str != NULL; i++) {
		if (value == sensormodunituse_strings[i].type) {
			return sensormodunituse_strings[i].str;
		}
	}
    return "Unknown";
}

