/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <http://dague.net/sean>
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 *	Christina Hernandez <hernanc@us.ibm.com>
 */

#include <sim_resources.h>
#include <sim_init.h>
#include <sim_sensors.h>
#include <math.h>

//	/* Find resource's events, sensors, controls, etc. */
//	dummy_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
//	dummy_discover_sensors(handle, dummy_chassis_sensors, e);
//	if (custom_handle->platform == DUMMY_PLATFORM_BCT) {
//		dummy_discover_sensors(handle, dummy_chassis_sensors_bct, e);
//		dummy_discover_controls(handle, dummy_chassis_controls_bct, e);
//	}
//	else {
//		dummy_discover_controls(handle, dummy_chassis_controls_bc, e);
//	}
//
//	dummy_discover_inventories(handle, dummy_chassis_inventories, e);

/**
 * dummy_create_resourcetag:
 * @buffer: Location of Resource Tag buffer.
 * @str: Resource name.
 * @location: Resource location.
 *
 * Creates a user friendly Resource Tag. Takes the comment found in the 
 * Resource's static definition, appends a trailing string (can be NULL) 
 * plus the resource's location.
 *
 * Return values:
 * SaHpiTextBufferT - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is NULL; or @loc not valid
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 **/
SaErrorT dummy_create_resourcetag(SaHpiTextBufferT *buffer, const char *str, SaHpiEntityLocationT loc)
{
	char *locstr;
	SaErrorT err = SA_OK;
	SaHpiTextBufferT working;

	if (!buffer || loc < DUMMY_HPI_LOCATION_BASE ||
	    loc > (pow(10, OH_MAX_LOCATION_DIGITS) - 1)) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_textbuffer(&working);
	if (err) { return(err); }

	locstr = (gchar *)g_malloc0(OH_MAX_LOCATION_DIGITS + 1);
	if (locstr == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	snprintf(locstr, OH_MAX_LOCATION_DIGITS + 1, " %d", loc);

	if (str) { oh_append_textbuffer(&working, str); }
	err = oh_append_textbuffer(&working, locstr);
	if (!err) {
		err = oh_copy_textbuffer(buffer, &working);
	}
	g_free(locstr);
	return(err);
}


/**************************************************************************
 *                        Resource Definitions
 **************************************************************************/

struct dummy_rpt dummy_rpt_array[] = {
        /* Chassis */
	{ 
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
                                                SAHPI_CAPABILITY_EVT_DEASSERTS |
                                                SAHPI_CAPABILITY_EVENT_LOG |
                                                SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_CRITICAL,
			.ResourceFailed = SAHPI_FALSE,
                
        	},
                .comment = "Sim Blade Center"
	},	
        /* Management module */
	{ 
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_SYS_MGMNT_MODULE,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
                                                SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESET |
                                                SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Sim Management Module"
	},
        /* Switch module */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_INTERCONNECT,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_INVENTORY_DATA |
			                        SAHPI_CAPABILITY_POWER |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESET |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Sim Network Switch Module"
	},
        /* Blade */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_SBC_BLADE,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
                                                SAHPI_CAPABILITY_EVT_DEASSERTS |
                                                SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_INVENTORY_DATA |
			                        SAHPI_CAPABILITY_POWER |
                                                SAHPI_CAPABILITY_RDR |
			                        SAHPI_CAPABILITY_RESET |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Sim Blade"
	},
        /* Blade expansion (add-in) card */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_ADD_IN_CARD,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_SBC_BLADE,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
                                                SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_RESOURCE |
			                        SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Sim Blade Expansion Card"
	},
        /* Media Tray */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_PERIPHERAL_BAY,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                },
                .comment = "Sim Control Panel/Media Tray"

	},
        /* Blower module */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_FAN,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_EVT_DEASSERTS |
                                                SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                 },
                .comment = "Sim Blower Module"

	},
        /* Power module */
	{
                .rpt = {
                        .ResourceInfo = {
                                .ManufacturerId = IBM_MANUFACTURING_ID,
                        },
                        .ResourceEntity = {
                                .Entry[0] =
                                {
                                        .EntityType = SAHPI_ENT_POWER_SUPPLY,
                                        .EntityLocation = DUMMY_HPI_LOCATION_BASE,
                                },
                                {
                                        .EntityType = SAHPI_ENT_ROOT,
                                        .EntityLocation = 0,
                                }
                        },
                        .ResourceCapabilities = SAHPI_CAPABILITY_FRU |
                                                SAHPI_CAPABILITY_INVENTORY_DATA |
                                                SAHPI_CAPABILITY_RESOURCE |
                                                SAHPI_CAPABILITY_RDR |
                                                SAHPI_CAPABILITY_SENSOR,
                        .ResourceSeverity = SAHPI_MAJOR,
			.ResourceFailed = SAHPI_FALSE,
                 },
                .comment = "Sim Power Module"
        
	},
};

