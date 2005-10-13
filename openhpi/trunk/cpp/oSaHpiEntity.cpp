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
#include "oSaHpiEntity.hpp"


/**
 * Default constructor.
 */
oSaHpiEntity::oSaHpiEntity() {
    EntityType = SAHPIENTITYTYPET_DEFAULT;
    EntityLocation = SAHPIENTITYLOCATIONT_DEFAULT;
};


/**
 * Constructor.
 *
 * @param type   The SaHpiEntityTypeT.
 * @param loc    The SaHpiEntityLocationT.
 */
oSaHpiEntity::oSaHpiEntity(const SaHpiEntityTypeT type,
                           const SaHpiEntityLocationT loc) {
    if (entitytype2str(type)) {
        EntityType = type;
    }
    else {
        EntityType = SAHPIENTITYTYPET_DEFAULT;
    }
    EntityLocation = loc;
};


/**
 * Constructor.
 *
 * @param buf    The reference to the class to be copied.
 */
oSaHpiEntity::oSaHpiEntity(const oSaHpiEntity& ent) {
    EntityType = ent.EntityType;
    EntityLocation = ent.EntityLocation;
}


/**
 * Assign a field in the SaHpiEntityT struct a value.
 *
 * @param field  The pointer to the struct (class).
 * @param field  The field name as a text string (case sensitive).
 * @param value  The character string value to be assigned to the field. This
 *               value will be converted as necessary.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiEntity::assignField(SaHpiEntityT *ptr,
                               const char *field,
                               const char *value) {
    if (ptr == NULL || field == NULL || value == NULL) {
        return true;
    }
    if (strcmp(field, "EntityType") == 0) {
        ptr->EntityType = str2entitytype(value);
        return false;
    }
    else if (strcmp(field, "EntityLocation") == 0) {
        ptr->EntityLocation = (SaHpiEntityLocationT)atoi(value);
        return false;
    }
    return true;
};


/**
 * Print the contents of the entity.
 *
 * @param stream Target stream.
 * @param buffer Address of the SaHpiEntityT struct.
 *
 * @return True if there was an error, otherwise false.
 */
bool oSaHpiEntity::fprint(FILE *stream,
                          const int indent,
                          const SaHpiEntityT *ent) {
	int i, err = 0;
    char buf[20];
    char indent_buf[indent + 1];

    if (stream == NULL || ent == NULL) {
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
    err = fprintf(stream, "EntityType = %s\n", entitytype2str(ent->EntityType));
    if (err < 0) {
        return true;
    }
    snprintf(buf, sizeof(buf), "%u", ent->EntityLocation, buf);
    err = fprintf(stream, indent_buf);
    if (err < 0) {
        return true;
    }
    err = fprintf(stream, "EntityLocation = %s\n", buf);
 	if (err < 0) {
   		return true;
   	}

	return false;
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
SaHpiEntityTypeT oSaHpiEntity::str2entitytype(const char *strtype) {
	int i, found = 0;

    if (strtype == NULL) {
        return SAHPIENTITYTYPET_DEFAULT;
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
	return SAHPIENTITYTYPET_DEFAULT;
}


/**
 * Translates an entity type to a string.
 *
 * @param value  The SaHpiEntityTypeT to be converted.
 *
 * @return The string value of the type.
 */
const char * oSaHpiEntity::entitytype2str(SaHpiEntityTypeT value) {
	int i;

	for (i = 0; entitytype_strings[i].str != NULL; i++) {
		if (value == entitytype_strings[i].type) {
			return entitytype_strings[i].str;
		}
	}
    return "Unknown";
}

