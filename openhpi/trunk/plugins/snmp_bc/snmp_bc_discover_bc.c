/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
 * file and program are licensed under a BSD style license. See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Peter Phan <pdphan@sourceforge.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <unistd.h>
#include <snmp_bc_plugin.h>

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);


struct SensorMibInfo snmp_bc_ipmi_sensors_temp[SNMP_BC_MAX_IPMI_TEMP_SENSORS] = {
	{ /* Generic IPMI Temp Sensor 1 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.12.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.22.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.23.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Temp Sensor 2 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.13.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.25.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.26.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Temp Sensor 3 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.14.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.28.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.29.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Temp Sensor 4 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.15.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.31.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.32.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Temp Sensor 5 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.16.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.34.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.35.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Temp Sensor 6 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.17.x",
		.threshold_oids = {
			.UpCritical = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.37.x",
			.UpMajor    = ".1.3.6.1.4.1.2.3.51.2.22.1.5.4.1.38.x",
		},
		.threshold_write_oids = {},
	},
};

struct SensorMibInfo snmp_bc_ipmi_sensors_voltage[SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS] = {
	{ /* Generic IPMI Voltage Sensor 1 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.15.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.23.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.24.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 2 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.16.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.25.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.26.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 3 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.17.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.27.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.28.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 4 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.18.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.29.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.30.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 5 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.19.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.31.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.32.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 6 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.20.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.33.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.34.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 7 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.21.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.35.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.36.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 8 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.22.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.37.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.38.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 9 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.23.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.39.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.40.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 10 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.24.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.41.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.42.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 11 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.25.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.43.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.44.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 12 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.26.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.45.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.46.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 13 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.27.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.47.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.48.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 14 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.28.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.49.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.50.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 15 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.29.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.51.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.52.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 16 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.30.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.53.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.54.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 17 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.31.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.55.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.56.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 18 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.32.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.57.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.58.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 19 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.33.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.59.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.60.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 20 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.34.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.61.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.62.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 21 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.35.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.63.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.64.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 22 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.36.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.65.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.66.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 23 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.37.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.67.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.68.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 24 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.38.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.69.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.70.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 25 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.39.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.71.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.72.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 26 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.40.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.73.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.74.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 27 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.41.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.75.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.76.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 28 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.42.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.77.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.78.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 29 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.43.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.79.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.80.x",
		},
		.threshold_write_oids = {},
	},
	{ /* Generic IPMI Voltage Sensor 30 */
		.not_avail_indicator_num = 0,
		.write_only = SAHPI_FALSE,
		.oid = ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.44.x",
		.threshold_oids = {
			.UpMajor  = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.81.x",
			.LowMajor = ".1.3.6.1.4.1.2.3.51.2.22.1.5.6.1.82.x",
		},
		.threshold_write_oids = {},
	},
};

static SaErrorT snmp_bc_discover_ipmi_sensors(struct oh_handler_state *handle,
					      struct snmp_bc_ipmi_sensor *sensor_array,
					      struct oh_event *res_oh_event);
/* Matching mmblade.mib definitions */		
/*	storageExpansion(1),        */
/* 	pciExpansion(2)             */
char *bladeexpansiondesc[] = {
		"Blade Expansion Module, BEM",
		"Blade Storage Expansion, BSE",
		"Blade PCI I/O Expansion, PEU"
};		

/**
 * snmp_bc_discover:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 *
 * Discovers IBM BladeCenter resources and RDRs.
 *
 * Return values:
 * SA_OK - normal case
 * SA_ERR_HPI_DUPLICATE - There is no changes to BladeCenter resource masks; normal case for re-discovery.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/

SaErrorT snmp_bc_discover(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root)
{

	SaErrorT err;
	struct snmp_value get_value_blade, get_value_blower,
			  get_value_power_module, get_value_switch,
			  get_value_media, get_value_mm;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !ep_root) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}


	/* If this is a rediscovery, then sync event log before all else.    */
	/* If event log synchronization process finds any hotswap event,     */
	/* eventlog synchronization process will kick off a targeted discovery*/
	/* for the hotswap resource. This is for both removal and installation*/
	if (custom_handle->isFirstDiscovery == SAHPI_FALSE) 
		err = snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);
	

	/**************************************************************
	 * Fetch various resource installation vectors from BladeCenter
	 **************************************************************/

	/* Fetch blade installed vector */
	get_installed_mask(SNMP_BC_PB_INSTALLED, get_value_blade);

	/* Fetch switch installed vector */
	get_installed_mask(SNMP_BC_SM_INSTALLED, get_value_switch);		 

	/* Fetch MM installed vector */
	get_installed_mask(SNMP_BC_MM_INSTALLED, get_value_mm);
		
	/* Fetch power module installed vector */
	get_installed_mask(SNMP_BC_PM_INSTALLED, get_value_power_module);

	/* Fetch media tray installed vector */
	get_integer_object(SNMP_BC_MT_INSTALLED, get_value_media); 
	
	/* Fetch blower installed vector  */
	get_installed_mask(SNMP_BC_BLOWER_INSTALLED, get_value_blower);
		
	if (  (g_ascii_strncasecmp(get_value_blade.string, custom_handle->installed_pb_mask, get_value_blade.str_len) == 0) &&
		(g_ascii_strncasecmp(get_value_blower.string, custom_handle->installed_blower_mask, get_value_blower.str_len) == 0) &&
		(g_ascii_strncasecmp(get_value_power_module.string, custom_handle->installed_pm_mask, get_value_power_module.str_len) == 0) &&
		(g_ascii_strncasecmp(get_value_switch.string, custom_handle->installed_sm_mask, get_value_switch.str_len) == 0) &&		
		(g_ascii_strncasecmp(get_value_mm.string, custom_handle->installed_mm_mask, get_value_mm.str_len) == 0) &&
		(get_value_media.integer == custom_handle->installed_mt_mask) ) {
		
		
		/**************************************************** 
		 * If **ALL** the resource masks are still the same, 
		 * do not rediscover, return with special return code 
		 ****************************************************/
		return(SA_ERR_HPI_DUPLICATE);
	} else {
		/*************************************************************
                 * Set saved masks to the newly read values
		 * Use strcpy() instead of strncpy(), counting on snmp_utils.c
		 * to NULL terminate strings read from snmp agent
		 *************************************************************/
		err = snmp_bc_update_chassis_topo(handle);
		if (err != SA_OK) return (err);
		
		strcpy(custom_handle->installed_pb_mask, get_value_blade.string);
		strcpy(custom_handle->installed_blower_mask, get_value_blower.string);
		strcpy(custom_handle->installed_pm_mask, get_value_power_module.string);
		strcpy(custom_handle->installed_sm_mask, get_value_switch.string);
		strcpy(custom_handle->installed_mm_mask, get_value_mm.string);
		custom_handle->installed_mt_mask = get_value_media.integer;
	}
		
	/******************************
	 * Discover BladeCenter Chassis
	 ******************************/
	err = snmp_bc_discover_chassis(handle, ep_root);
	if (err != SA_OK) return(err);
															
	/******************************
	 * Discover ALL BladeCenter Slots
	 * Warning: 
	 *   Discovery of Physical Slots must come **before** discovery of sloted resources.
	 *   Discovery of slots sets Slot State Sensor to empty.
	 *   Subsequent resource discovery changes state of Slot State Sensor accordingly.   
	 ******************************/
	err = snmp_bc_discover_all_slots(handle, ep_root);
	if (err != SA_OK) return(err);
				  
	/***************** 
	 * Discover Blades
	 *****************/
	err = snmp_bc_discover_blade(handle, ep_root,get_value_blade.string);
	if (err != SA_OK) return(err);
				  			
        /******************
	 * Discover Blowers
	 ******************/
	err = snmp_bc_discover_blowers(handle, ep_root, get_value_blower.string);
	if (err != SA_OK) return(err);
	
        /************************
	 * Discover Power Modules
	 ************************/
	err = snmp_bc_discover_power_module(handle, ep_root, get_value_power_module.string);
	if (err != SA_OK) return(err);
	
	/******************* 
	 * Discover Switches
	 *******************/
	err = snmp_bc_discover_switch(handle, ep_root, get_value_switch.string);
	if (err != SA_OK) return(err);
	
	/**********************
	 * Discover Media Trays
	 **********************/
	err = snmp_bc_discover_media_tray(handle, ep_root, get_value_media.integer);
	if (err != SA_OK) return(err);

	/*********************************** 
	 * Discover Management Modules (MMs)
	 ***********************************/
	err = snmp_bc_discover_mm(handle, ep_root, get_value_mm.string, SAHPI_TRUE);
	if (err != SA_OK) return(err);

	return(SA_OK);
}

/**
 * snmp_bc_discover_media_tray:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @media_tray_installed: Bitmap vector of installed media trays.
 *
 * Discovers media tray resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameters are NULL.
 **/
SaErrorT snmp_bc_update_chassis_topo(struct oh_handler_state *handle)
{
	SaErrorT err;
	struct snmp_value get_value;
	struct snmp_bc_hnd *custom_handle;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	if (custom_handle->isFirstDiscovery == SAHPI_TRUE) {
	
		get_integer_object(SNMP_BC_NOS_PB_SUPPORTED, get_value);
		custom_handle->max_pb_supported = get_value.integer;		/* pb - processor blade */
		
		get_integer_object(SNMP_BC_NOS_SM_SUPPORTED, get_value);		
		custom_handle->max_sm_supported = get_value.integer;		/* sm - switch module   */
		
		get_integer_object(SNMP_BC_NOS_MM_SUPPORTED, get_value);		
		custom_handle->max_mm_supported = get_value.integer;		/* mm - management module */

		get_integer_object(SNMP_BC_NOS_PM_SUPPORTED, get_value);
		custom_handle->max_pm_supported = get_value.integer;		/* pm - power module    */
		
		get_integer_object(SNMP_BC_NOS_MT_SUPPORTED, get_value);		
		custom_handle->max_mt_supported = get_value.integer;		/* mt - media tray        */

		get_integer_object(SNMP_BC_NOS_BLOWER_SUPPORTED, get_value);		
		custom_handle->max_blower_supported = get_value.integer;	/* blower - blower  */
	 }
	
	err = SA_OK;
	return(err);
}

/**
 * snmp_bc_discover_media_tray:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @media_tray_installed: Bitmap vector of installed media trays.
 *
 * Discovers media tray resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameters are NULL.
 **/
SaErrorT snmp_bc_discover_media_tray(struct oh_handler_state *handle,
			  	     SaHpiEntityPathT *ep_root, 
				     int  media_tray_installed)
{

	SaErrorT err;
	guint mt_width;
        struct oh_event *e;
	struct snmp_value get_value;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !ep_root) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].rpt;
	oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   BLADECENTER_PERIPHERAL_BAY_SLOT, SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   SAHPI_ENT_PERIPHERAL_BAY, SNMP_BC_HPI_LOCATION_BASE);
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
				   snmp_bc_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].comment,
				   SNMP_BC_HPI_LOCATION_BASE);

	trace("Discovered resource=%s; ID=%d",
	       	e->u.res_event.entry.ResourceTag.Data,
		      e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].res_info),
					sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}


	if (!media_tray_installed) {
		res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		g_free(e);
		g_free(res_info_ptr);

	} else if (media_tray_installed) {
		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                /* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);
		
		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(handle->rptcache, 
				      &(e->u.res_event.entry),
				      res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		handle->eventq = g_slist_append(handle->eventq, e);
		
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_mediatray_sensors, e);
		snmp_bc_discover_controls(handle, snmp_bc_mediatray_controls, e);
		snmp_bc_discover_inventories(handle, snmp_bc_mediatray_inventories, e);
		
		mt_width = 1;    /* Default to 1-wide */
		if (res_info_ptr->mib.OidResourceWidth != NULL) {
			err = snmp_bc_oid_snmp_get(custom_handle,  &(e->u.res_event.entry.ResourceEntity), 0,
						   res_info_ptr->mib.OidResourceWidth, &get_value, SAHPI_TRUE);
			if (!err && (get_value.type == ASN_INTEGER)) {
					mt_width = get_value.integer;
			}
		}			

		err = snmp_bc_set_resource_slot_state_sensor(handle, e, mt_width);
		
	}
	
	return(SA_OK);
}

/**
 * snmp_bc_discover_chassis:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 *
 * Discovers the BladeCenter chassis resource and its RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_chassis(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root)
{

	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !ep_root) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/****************** 
	 * Discover Chassis
	 ******************/
	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();

	if (custom_handle->platform == SNMP_BC_PLATFORM_BCT) {
		e->u.res_event.entry = snmp_bc_rpt_array_bct[BCT_RPT_ENTRY_CHASSIS].rpt;
	} else {
		e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_CHASSIS].rpt;	
	}

	e->u.res_event.entry.ResourceEntity = *ep_root;
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	{ /* Generate Chassis Resource Tag */
		SaHpiTextBufferT build_name;

		oh_init_textbuffer(&build_name);

		switch (custom_handle->platform) {
		case SNMP_BC_PLATFORM_BC:
			oh_append_textbuffer(&build_name, "BladeCenter Chassis");
			break;
		case SNMP_BC_PLATFORM_BCH:
			oh_append_textbuffer(&build_name, "BladeCenter H Chassis");
			break;
		case SNMP_BC_PLATFORM_BCT:
			oh_append_textbuffer(&build_name, "BladeCenter T Chassis");
			break;
		default:	
			oh_append_textbuffer(&build_name, "BladeCenter Chassis");
		}

		snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
					   (char *)build_name.Data,
					   ep_root->Entry[0].EntityLocation);
	}

	trace("Discovered resource=%s; ID=%d",
	      e->u.res_event.entry.ResourceTag.Data,
	      e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_CHASSIS].res_info),
				sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(handle->rptcache, 
			      &(e->u.res_event.entry), 
			      res_info_ptr, 0);
	if (err) {
		dbg("Cannot add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	handle->eventq = g_slist_append(handle->eventq, e);

	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_chassis_sensors, e);
	if (custom_handle->platform == SNMP_BC_PLATFORM_BCT) {
		snmp_bc_discover_sensors(handle, snmp_bc_chassis_sensors_bct, e);
		snmp_bc_discover_controls(handle, snmp_bc_chassis_controls_bct, e);
	}
	else {
		snmp_bc_discover_controls(handle, snmp_bc_chassis_controls_bc, e);
	}

	snmp_bc_discover_inventories(handle, snmp_bc_chassis_inventories, e);
	return(SA_OK);

}

/**
 * snmp_bc_discover_blade:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @blade_vector: Bitmap vector of installed blades.
 *
 * Discovers blade resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_blade(struct oh_handler_state *handle,
			  	SaHpiEntityPathT *ep_root, 
				char *blade_vector)
{

	gint i;
	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !blade_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	e = NULL;
	res_info_ptr = NULL;	
	for (i=0; i < strlen(blade_vector); i++) {
	
		if ((blade_vector[i] == '1') || (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			err = snmp_bc_construct_blade_res(&e, &res_info_ptr, ep_root, i);
			if (err) return(err);
		}
			
		if ((blade_vector[i] == '0') && 
		    (custom_handle->isFirstDiscovery == SAHPI_TRUE)) 
		{
			/* Make sure that we have static infomation 
			 * for this **empty** blade slot in hash during HPI initialization
			 */ 
			res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			g_free(e);
			g_free(res_info_ptr);
			
		} else if (blade_vector[i] == '1') {
		
			err = snmp_bc_discover_blade_i(handle,
			  	  			res_info_ptr, 
				  			e,
							ep_root, 
				  			i);
			
		
		}
	}

	return(SA_OK);
}

/**
 * snmp_bc_construct_blade_res:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @blade_index: Index of the main blade 
 *
 * Construct oh_event structure for a blade resource.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_construct_blade_res(struct oh_event **e,
			  	     struct ResourceInfo **res_info_ptr,
				     SaHpiEntityPathT *ep_root,
				     guint blade_index) 
{	
	struct oh_event *le = NULL;
	struct ResourceInfo *lres_info_ptr = NULL;
	
	le = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (le == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	le->type = OH_ET_RESOURCE;
	le->did = oh_get_default_domain_id();
	le->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].rpt;
	oh_concat_ep(&(le->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(le->u.res_event.entry.ResourceEntity),
		  SAHPI_ENT_PHYSICAL_SLOT, blade_index + SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&(le->u.res_event.entry.ResourceEntity),
		   SAHPI_ENT_SBC_BLADE, blade_index + SNMP_BC_HPI_LOCATION_BASE);
	le->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(le->u.res_event.entry.ResourceEntity));
	lres_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].res_info),
							sizeof(struct ResourceInfo));
	if (!lres_info_ptr) {
		dbg("Out of memory.");
		g_free(le);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	*e = le;
	*res_info_ptr = lres_info_ptr;
	return(SA_OK);
}
/**
 * snmp_bc_discover_blade_i:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @blade_index: Index of the main blade 
 *
 * Discovers blade expansion resources and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_blade_i(struct oh_handler_state *handle,
			  	  struct ResourceInfo *res_info_ptr, 
				  struct oh_event *e,
				  SaHpiEntityPathT *ep_root,
				  guint blade_index)
{

	SaErrorT err;
	guint blade_width;
	struct snmp_value get_value, get_blade_resourcetag;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !e) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	/* In case the MM is still discovering newly installed blade */
	/* when this condition is detected, we wait for 3 seconds ...*/
	/* then try again.                                           */
	while(1) {
		err = snmp_bc_oid_snmp_get(custom_handle,
			 &(e->u.res_event.entry.ResourceEntity), 0,
			 snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].OidResourceTag,
					&get_blade_resourcetag, SAHPI_TRUE);
				
		if ( (get_blade_resourcetag.type == ASN_OCTET_STR) && 
		   ( g_ascii_strncasecmp(get_blade_resourcetag.string, LOG_DISCOVERING, 
		     sizeof(LOG_DISCOVERING)) == 0 ) )
		{
			sleep(3);
		} else break;
	}
			
	/* Create resource tag */		
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
				     snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].comment,
						   blade_index + SNMP_BC_HPI_LOCATION_BASE);
			
	/* Tack on MM's defined blade name */
	if (!err && (get_blade_resourcetag.type == ASN_OCTET_STR)) {
		oh_append_textbuffer(&(e->u.res_event.entry.ResourceTag), " - ");
		oh_append_textbuffer(&(e->u.res_event.entry.ResourceTag), get_blade_resourcetag.string);
	}

	trace("Discovered resource=%s; ID=%d", 
			e->u.res_event.entry.ResourceTag.Data,
			      e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */			
	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;  /* Default to ACTIVE */
	if (res_info_ptr->mib.OidPowerState != NULL) {
		/* Read power state of resource */
		err = snmp_bc_oid_snmp_get(custom_handle, &(e->u.res_event.entry.ResourceEntity), 0,
				   		res_info_ptr->mib.OidPowerState, &get_value, SAHPI_TRUE);
		if (!err && (get_value.type == ASN_INTEGER)) {
			if (get_value.integer == 0)   /*  state = SAHPI_POWER_OFF */
					res_info_ptr->cur_state = SAHPI_HS_STATE_INACTIVE;
		}
	}

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(handle->rptcache, 
			      &(e->u.res_event.entry),
					      res_info_ptr, 0);
	if (err) {
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	handle->eventq = g_slist_append(handle->eventq, e);
			
	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_blade_sensors, e);
	snmp_bc_discover_ipmi_sensors(handle, snmp_bc_blade_ipmi_sensors, e);
	snmp_bc_discover_controls(handle, snmp_bc_blade_controls, e);
	snmp_bc_discover_inventories(handle, snmp_bc_blade_inventories, e);
			
	blade_width = 1;    /* Default to 1-wide blade */
	if (res_info_ptr->mib.OidResourceWidth != NULL) {
	err = snmp_bc_oid_snmp_get(custom_handle,  &(e->u.res_event.entry.ResourceEntity), 0,
		       		  res_info_ptr->mib.OidResourceWidth, &get_value, SAHPI_TRUE);
		if (!err && (get_value.type == ASN_INTEGER)) {
			blade_width = get_value.integer;
		}
	}			

	err = snmp_bc_set_resource_slot_state_sensor(handle, e, blade_width);

	/********************************** 
	 * Discover Blade Expansion Modules
	 **********************************/
	{
			
		err = snmp_bc_discover_blade_expansion(handle, ep_root, blade_index);

	}

	return(err);
}

/**
 * snmp_bc_discover_blade_expansion:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @blade_index: Index of the main blade 
 *
 * Discovers blade expansion resources and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_blade_expansion(struct oh_handler_state *handle,
			  		  SaHpiEntityPathT *ep_root, 
					  guint blade_index)
{

	SaErrorT err;
	gint i, j;
	SaHpiEntityPathT ep;
	struct snmp_value get_value;
	BCExpansionTypeT expansionType;
	struct snmp_bc_hnd *custom_handle;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	ep = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].rpt.ResourceEntity;
	oh_concat_ep(&ep, ep_root);
	oh_set_ep_location(&ep, SAHPI_ENT_PHYSICAL_SLOT, blade_index + SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&ep, SAHPI_ENT_SBC_BLADE, blade_index + SNMP_BC_HPI_LOCATION_BASE);

	/* Determine which scheme to detect blade expansion */
	
	/* Set entity_path index to the first entry (SNMP_BC_HPI_LOCATION_BASE) in table */
	oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, SNMP_BC_HPI_LOCATION_BASE);
	/* Go get value at (SNMP_BC_HPI_LOCATION_BASE + offset 0) */	
	err = snmp_bc_oid_snmp_get(custom_handle, &ep, 0,
				   SNMP_BC_BLADE_EXP_BLADE_BAY, &get_value, SAHPI_TRUE);

	j = 0;
	if (err ==  SA_ERR_HPI_NOT_PRESENT) {
		
		/* No object exists with SNMP_BC_BLADE_EXP_BLADE_BAY oid */
		/* Ether the target is running with older MM mib version,*/
		/* or there is no expansion board at all in system.      */
		/* Use old scheme to discover BladeExpandion resource.   */
		
		/* Set entity_path index to the desired entry  */
		/* (blade_index + SNMP_BC_HPI_LOCATION_BASE) in table */	   	
		oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, 
							blade_index + SNMP_BC_HPI_LOCATION_BASE);
		/* Go get value at (SNMP_BC_HPI_LOCATION_BASE + offset 0) */								
		err = snmp_bc_oid_snmp_get(custom_handle, &ep, 0,
					SNMP_BC_BLADE_EXPANSION_VECTOR, &get_value, SAHPI_TRUE);

		/* With the old scheme, we can only detect one of the blade expansion board */ 
                /* For example, if a blade has BSE and PEU, we see only one with this scheme*/
		oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, j + SNMP_BC_HPI_LOCATION_BASE);

		if (!err && get_value.integer != 0) {
			err = snmp_bc_add_blade_expansion_resource(handle, &ep, blade_index,
				 DEFAULT_BLADE_EXPANSION_CARD_TYPE, j);
		}
	} else if(err ==  SA_OK) {
	
		/* New scheme; i == index for Processor Blade, j == index for Blade Expansion for each Processor Blade  */
		for (i=0; i < (custom_handle->max_pb_supported ); i++) {

			/* Set entity_path index to the first entry (SNMP_BC_HPI_LOCATION_BASE) in table */
			oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, 
							    SNMP_BC_HPI_LOCATION_BASE);

			/* Go get value at (SNMP_BC_HPI_LOCATION_BASE + offset i) */
			err = snmp_bc_oid_snmp_get(custom_handle, &ep, i,
				   SNMP_BC_BLADE_EXP_BLADE_BAY, &get_value, SAHPI_TRUE);
				   
			if (err == SA_OK) {
				if (get_value.type != ASN_OCTET_STR) continue;
				

				if ( atoi(get_value.string) == (blade_index + SNMP_BC_HPI_LOCATION_BASE)) {
				
					/* Go get value at (SNMP_BC_HPI_LOCATION_BASE + offset i) */
					err = snmp_bc_oid_snmp_get(custom_handle, &ep, i,
				   			SNMP_BC_BLADE_EXP_TYPE, &get_value, SAHPI_TRUE);
					
					if ((err == SA_OK) && (get_value.type == ASN_INTEGER)) {
						/*		
						storageExpansion(1),
                    				pciExpansion(2)
						*/	
						expansionType = get_value.integer;
					} else 	{
						dbg(" Error reading Expansion Board Type\n");
						expansionType = DEFAULT_BLADE_EXPANSION_CARD_TYPE;
					}
					
					
					oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, 
										j + SNMP_BC_HPI_LOCATION_BASE);
																
				 	err = snmp_bc_add_blade_expansion_resource(handle, &ep, blade_index, expansionType, j);
					j++;
			
				}
			}
		
		} /* end for custom_handle->max_pb_supported */
	} 

	return(SA_OK);
}


/**
 * snmp_bc_add_blade_expansion_resource:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @blade_index: Index of the main blade 
 *
 * Discovers blade expansion resources and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_add_blade_expansion_resource(struct oh_handler_state *handle,
			  				SaHpiEntityPathT *ep, 
							guint blade_index,
							BCExpansionTypeT expansionType,
							guint expansionindex)
{

	SaErrorT err;
	struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	{
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}
	
		e->type = OH_ET_RESOURCE;
		e->did = oh_get_default_domain_id();
		e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].rpt;
		e->u.res_event.entry.ResourceEntity = *ep;
		e->u.res_event.entry.ResourceId = oh_uid_from_entity_path(ep);
			
		{
		SaHpiTextBufferT  working, working2;
		snmp_bc_create_resourcetag(&working, "Blade", blade_index + SNMP_BC_HPI_LOCATION_BASE);
		snmp_bc_create_resourcetag(&working2,
				 	bladeexpansiondesc[expansionType],
				 	SNMP_BC_HPI_LOCATION_BASE + expansionindex);
		oh_init_textbuffer(&(e->u.res_event.entry.ResourceTag));
		oh_append_textbuffer(&(e->u.res_event.entry.ResourceTag), (char *)working.Data);
		oh_append_textbuffer(&(e->u.res_event.entry.ResourceTag), " ");
		oh_append_textbuffer(&(e->u.res_event.entry.ResourceTag), (char *)working2.Data);
		}

		trace("Discovered resource=%s; ID=%d",
			e->u.res_event.entry.ResourceTag.Data,
			e->u.res_event.entry.ResourceId);

		/* Create platform-specific info space to add to infra-structure */
		res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].res_info),
						sizeof(struct ResourceInfo));
		if (!res_info_ptr) {
			dbg("Out of memory.");
			g_free(e);
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                /* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(handle->rptcache, 
					&(e->u.res_event.entry),
					res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		handle->eventq = g_slist_append(handle->eventq, e);
			
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_bem_sensors, e);
		snmp_bc_discover_ipmi_sensors(handle, snmp_bc_bem_ipmi_sensors, e);
		snmp_bc_discover_controls(handle, snmp_bc_bem_controls, e);
		snmp_bc_discover_inventories(handle, snmp_bc_bem_inventories, e);
	}
			
	return(SA_OK);

}						
/**
 * snmp_bc_discover_blowers:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @blower_vector: Bitmap vector of installed blowers.
 *
 * Discovers blower resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer paramter(s) NULL.
 **/
SaErrorT snmp_bc_discover_blowers(struct oh_handler_state *handle,
			          SaHpiEntityPathT *ep_root, 
				  char *blower_vector)
{

	guint i;
	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !blower_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	e= NULL;
	res_info_ptr = NULL;
	
	for (i=0; i < strlen(blower_vector); i++) {
	
		if ((blower_vector[i] == '1') || (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			err = snmp_bc_construct_blower_res(&e, &res_info_ptr, ep_root, i);
			if (err) return(err);
		}
		
		if ((blower_vector[i] == '0') && (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			g_free(e);
			g_free(res_info_ptr);
			
		} else if (blower_vector[i] == '1') {
			err = snmp_bc_discover_blower_i(handle,
			  	  			res_info_ptr, 
				  			e, 
				  			i);			
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_construct_blower_res:
 * @e: Pointer to resource oh_event.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @blower_index: Index of the blower 
 *
 * Construct oh_event structure for a blower resource.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_construct_blower_res(struct oh_event **re,
			  	     struct ResourceInfo **rres_info_ptr,
				     SaHpiEntityPathT *ep_root, 
				     guint blower_index) 
{
	struct oh_event *e = NULL;
	struct ResourceInfo *res_info_ptr = NULL;

	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].rpt;
	oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   BLADECENTER_BLOWER_SLOT, blower_index + SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
				   SAHPI_ENT_FAN, blower_index + SNMP_BC_HPI_LOCATION_BASE);
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
			   snmp_bc_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].comment,
					   blower_index + SNMP_BC_HPI_LOCATION_BASE);

	trace("Discovered resource=%s; ID=%d",
      		e->u.res_event.entry.ResourceTag.Data,
	      		e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].res_info),
						sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	*re = e;
	*rres_info_ptr = res_info_ptr;
	return(SA_OK);
}
/**
 * snmp_bc_discover_blower_i:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @blade_index: Index of the blower 
 *
 * Discovers **a** blower resource and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_blower_i(struct oh_handler_state *handle,
			  	  struct ResourceInfo *res_info_ptr, 
				  struct oh_event *e, 
				  guint blower_index)
{
	guint blower_width;
	SaErrorT err;
	struct snmp_value get_value;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !res_info_ptr || !e) return(SA_ERR_HPI_INVALID_PARAMS);
	
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = SA_OK;
	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(handle->rptcache, 
			      &(e->u.res_event.entry),
			      res_info_ptr, 0);
	if (err) {
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	handle->eventq = g_slist_append(handle->eventq, e);
			
	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_blower_sensors, e);
	if (custom_handle->platform == SNMP_BC_PLATFORM_BCH) {
		snmp_bc_discover_sensors(handle, snmp_bc_blower_sensors_bch, e);	
	}
	snmp_bc_discover_controls(handle, snmp_bc_blower_controls, e);
	snmp_bc_discover_inventories(handle, snmp_bc_blower_inventories, e);
			
	blower_width = 1;    /* Default to 1-wide blade */
	if (res_info_ptr->mib.OidResourceWidth != NULL) {
		err = snmp_bc_oid_snmp_get(custom_handle,  
					   &(e->u.res_event.entry.ResourceEntity), 0,
				   	   res_info_ptr->mib.OidResourceWidth, 
					   &get_value, 
					   SAHPI_TRUE);
		if (!err && (get_value.type == ASN_INTEGER)) {
			blower_width = get_value.integer;
		}
	}			

	err = snmp_bc_set_resource_slot_state_sensor(handle, e, blower_width);
	
	return(err);

}
/**
 * snmp_bc_discover_power_module:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @power_module_vector: Bitmap vector of installed power modules.
 *
 * Discovers power module resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_power_module(struct oh_handler_state *handle,
			  		SaHpiEntityPathT *ep_root, 
					char *power_module_vector)
{

	int i;
	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !power_module_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	e = NULL;
	res_info_ptr = NULL;
	
	for (i=0; i < strlen(power_module_vector); i++) {

		if ((power_module_vector[i] == '1') || (custom_handle->isFirstDiscovery == SAHPI_TRUE)) 
		{
			err = snmp_bc_construct_pm_res(&e, &res_info_ptr, ep_root, i);
			if (err) return(err);
			
		}
	
		if ((power_module_vector[i] == '0') && (custom_handle->isFirstDiscovery == SAHPI_TRUE)) 
		{
			res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			g_free(e);
			g_free(res_info_ptr);
			
		} else if (power_module_vector[i] == '1') {
			err = snmp_bc_discover_pm_i(handle,
			  	  		    res_info_ptr, 
				  		    e, 
				  		    i);					
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_construct_pm_res:
 * @e: Pointer to resource oh_event.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @pm_index: Index of the power module (pm) 
 *
 * Construct oh_event structure for a power module (pm) resource.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_construct_pm_res(struct oh_event **re,
			  	     struct ResourceInfo **rres_info_ptr,
				     SaHpiEntityPathT *ep_root, 
				     guint pm_index) 
{
	struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	
	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_POWER_MODULE].rpt;
	oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   BLADECENTER_POWER_SUPPLY_SLOT, pm_index + SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   SAHPI_ENT_POWER_SUPPLY, pm_index + SNMP_BC_HPI_LOCATION_BASE);
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
	  			   snmp_bc_rpt_array[BC_RPT_ENTRY_POWER_MODULE].comment,
				   pm_index + SNMP_BC_HPI_LOCATION_BASE);

	trace("Discovered resource=%s; ID=%d",
		   e->u.res_event.entry.ResourceTag.Data,
			      	e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_POWER_MODULE].res_info),
						sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	*re = e;
	*rres_info_ptr = res_info_ptr;
	return(SA_OK);
}
/**
 * snmp_bc_discover_pm_i:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @pm_index: Index of the power module 
 *
 * Discovers **a** power module (pm) resource and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_pm_i(struct oh_handler_state *handle,
			  	  struct ResourceInfo *res_info_ptr, 
				  struct oh_event *e, 
				  guint pm_index)
{

	guint pm_width;
	SaErrorT err;
	struct snmp_value get_value;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !res_info_ptr || !e) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	trace("Discovering resources for installed power module %d.\n",pm_index);
	
	err = SA_OK;
	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(handle->rptcache, 
				&(e->u.res_event.entry),
					 res_info_ptr, 0);
	if (err) {
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	handle->eventq = g_slist_append(handle->eventq, e);
			
	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_power_sensors, e);

	if (custom_handle->platform == SNMP_BC_PLATFORM_BCH) {
		snmp_bc_discover_sensors(handle, snmp_bc_power_sensors_bch, e);	
	}

	snmp_bc_discover_controls(handle, snmp_bc_power_controls, e);
	snmp_bc_discover_inventories(handle, snmp_bc_power_inventories, e);
			
	pm_width = 1;    /* Default to 1-wide */
	if (res_info_ptr->mib.OidResourceWidth != NULL) {
		err = snmp_bc_oid_snmp_get(custom_handle,  
					   &(e->u.res_event.entry.ResourceEntity), 
					   0,
				   	   res_info_ptr->mib.OidResourceWidth, 
					   &get_value, 
					   SAHPI_TRUE);
					   
		if (!err && (get_value.type == ASN_INTEGER)) {
			pm_width = get_value.integer;
		}
	}			

	err = snmp_bc_set_resource_slot_state_sensor(handle, e, pm_width);
	return(err);	

}
/**
 * snmp_bc_discover_switch:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @switch_vector: Bitmap vector of installed I/O modules.
 *
 * Discovers I/O module resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_switch(struct oh_handler_state *handle,
			  	 SaHpiEntityPathT *ep_root, 
				 char *switch_vector)
{
	int i;
	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !switch_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	e = NULL;
	res_info_ptr = NULL;
	
	for (i=0; i < strlen(switch_vector); i++) {
	
		if ((switch_vector[i] == '1') || (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			err = snmp_bc_construct_sm_res(&e, &res_info_ptr, ep_root, i);
			if (err) return(err);
		}
		
		if ((switch_vector[i] == '0') && (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			g_free(e);
			g_free(res_info_ptr);
			
		} else if (switch_vector[i] == '1') {
			err = snmp_bc_discover_switch_i(handle,
			  	  			res_info_ptr, 
				  			e, 
				  			i);
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_construct_sm_res:
 * @e: Pointer to resource oh_event.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @sm_index: Index of the switch module (sm) 
 *
 * Construct oh_event structure for a switch module (sm) resource.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_construct_sm_res(struct oh_event **re,
			  	  struct ResourceInfo **rres_info_ptr,
				  SaHpiEntityPathT *ep_root, 
				  guint sm_index) 
{
	struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	
	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].rpt;
	oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   BLADECENTER_INTERCONNECT_SLOT, sm_index + SNMP_BC_HPI_LOCATION_BASE);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			   SAHPI_ENT_INTERCONNECT, sm_index + SNMP_BC_HPI_LOCATION_BASE);
	e->u.res_event.entry.ResourceId = 
			oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
			   snmp_bc_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].comment,
						   sm_index+ SNMP_BC_HPI_LOCATION_BASE);

	trace("Discovered resource=%s; ID=%d",
		    e->u.res_event.entry.ResourceTag.Data,
		      		e->u.res_event.entry.ResourceId);

	/* Create platform-specific info space to add to infra-structure */
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].res_info),
						sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	*re = e;
	*rres_info_ptr = res_info_ptr;
	return(SA_OK);

}

/**
 * snmp_bc_discover_switch_i:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @sm_index: Index of the switch module 
 *
 * Discovers **a** switch module (sm) resource and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_switch_i(struct oh_handler_state *handle,
			  	  struct ResourceInfo *res_info_ptr, 
				  struct oh_event *e, 
				  guint sm_index)
{
	guint sw_width;
	SaErrorT err;
	struct snmp_bc_hnd *custom_handle;
	struct snmp_value get_value;


	if (!handle || !res_info_ptr || !e) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	trace("Discovering resources for installed switch module %d.\n",sm_index);
	err = SA_OK;
	
	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;  /* Default to ACTIVE */
	if (res_info_ptr->mib.OidPowerState != NULL) {
		/* Read power state of resource */
		err = snmp_bc_oid_snmp_get(custom_handle,  
					   &(e->u.res_event.entry.ResourceEntity), 
					   0,
				   	   res_info_ptr->mib.OidPowerState, 
					   &get_value, 
					   SAHPI_TRUE);
		if (!err && (get_value.type == ASN_INTEGER)) {
			if (get_value.integer == 0)   /*  state = SAHPI_POWER_OFF */
				res_info_ptr->cur_state = SAHPI_HS_STATE_INACTIVE;
		}			
	}

        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

	/* Add resource to temporary event cache/queue */
	err = oh_add_resource(handle->rptcache, 
				&(e->u.res_event.entry),
				res_info_ptr, 0);
	if (err) {
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	handle->eventq = g_slist_append(handle->eventq, e);
			
	/* Find resource's events, sensors, controls, etc. */
	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
	snmp_bc_discover_sensors(handle, snmp_bc_switch_sensors, e);
	snmp_bc_discover_controls(handle, snmp_bc_switch_controls, e);
	snmp_bc_discover_inventories(handle, snmp_bc_switch_inventories, e);

	sw_width = 1;    /* Default to 1-wide */
	if (res_info_ptr->mib.OidResourceWidth != NULL) {
		err = snmp_bc_oid_snmp_get(custom_handle,  
					   &(e->u.res_event.entry.ResourceEntity), 
					   0,
				   	   res_info_ptr->mib.OidResourceWidth, 
					   &get_value, 
					   SAHPI_TRUE);
		if (!err && (get_value.type == ASN_INTEGER)) {
			sw_width = get_value.integer;
		}
	}			

	err = snmp_bc_set_resource_slot_state_sensor(handle, e, sw_width);

	return(err);
}
/**
 * snmp_bc_discover_mm:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @mm_vector: Bitmap vector of installed MMs.
 * @global_discovery: Also include Virtual MM in the discovery
 *
 * Discovers management module (MM) resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_mm(struct oh_handler_state *handle,
			  	SaHpiEntityPathT *ep_root, 
				char *mm_vector, 
				SaHpiBoolT global_discovery)
{

	int i;
	SaErrorT err;
        struct oh_event *e;
	struct snmp_value get_value;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !mm_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	e = NULL;
	res_info_ptr = NULL;
	
	/* Discover Virtual MM */
	if (global_discovery == SAHPI_TRUE)
	{			
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                if (e == NULL) {
                       	dbg("Out of memory.");
                        return(SA_ERR_HPI_OUT_OF_SPACE);
                }

                e->type = OH_ET_RESOURCE;
                e->did = oh_get_default_domain_id();
                e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_VIRTUAL_MGMNT_MODULE].rpt;
                oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
                oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
                                       SAHPI_ENT_SYS_MGMNT_MODULE, 0);
                e->u.res_event.entry.ResourceId =
                       	oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
                snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
                                             snmp_bc_rpt_array[BC_RPT_ENTRY_VIRTUAL_MGMNT_MODULE].comment,
                                             0);

                trace("Discovered resource=%s; ID=%d",
                        e->u.res_event.entry.ResourceTag.Data,
                        e->u.res_event.entry.ResourceId);

                /* Create platform-specific info space to add to infra-structure */
                res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_VIRTUAL_MGMNT_MODULE].res_info),
                        	                sizeof(struct ResourceInfo));
                if (!res_info_ptr) {
                        dbg("Out of memory.");
                        g_free(e);
                        return(SA_ERR_HPI_OUT_OF_SPACE);
                }

                /* Add resource to temporary event cache/queue */
                err = oh_add_resource(handle->rptcache,
                        	        &(e->u.res_event.entry),
                                	        res_info_ptr, 0);
                if (err) {
                        dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
                        g_free(e);
                        return(err);
                }
                handle->eventq = g_slist_append(handle->eventq, e);
				
		/* Find resource's events, sensors, controls, etc. */
                snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
                snmp_bc_discover_sensors(handle, snmp_bc_virtual_mgmnt_sensors, e);
                snmp_bc_discover_controls(handle, snmp_bc_virtual_mgmnt_controls, e);
                snmp_bc_discover_inventories(handle, snmp_bc_virtual_mgmnt_inventories, e);		
	}		
			
	/* Discover Physical MM */                				
	for (i=0; i < strlen(mm_vector); i++) {
		trace("Management Module installed bit map %s", get_value.string);
		if ((mm_vector[i] == '1') || (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
			err = snmp_bc_construct_mm_res(&e, &res_info_ptr, ep_root, i);
			if(err) return(err);
		}

		if ((mm_vector[i] == '0') && (custom_handle->isFirstDiscovery == SAHPI_TRUE))
		{
		        res_info_ptr->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			g_free(e);
			g_free(res_info_ptr);
			
		} else if (mm_vector[i] == '1'){
			err = snmp_bc_discover_mm_i(handle,
			  	  		    res_info_ptr, 
				  	            e, 
				  		    i);
		}
	}
	return(SA_OK);
}
/**
 * snmp_bc_construct_mm_res:
 * @e: Pointer to resource oh_event.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @mm_index: Index of the management module (mm) 
 *
 * Construct oh_event structure for a management module (mm) resource.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_construct_mm_res(struct oh_event **re,
			  	  struct ResourceInfo **rres_info_ptr,
				  SaHpiEntityPathT *ep_root, 
				  guint mm_index) 
{
	struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	
	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
        if (e == NULL) {
        	dbg("Out of memory.");
                return(SA_ERR_HPI_OUT_OF_SPACE);
        }
        e->type = OH_ET_RESOURCE;
        e->did = oh_get_default_domain_id();
        e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].rpt;
        oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
        oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
                        BLADECENTER_SYS_MGMNT_MODULE_SLOT, mm_index + SNMP_BC_HPI_LOCATION_BASE);
        oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
                        SAHPI_ENT_SYS_MGMNT_MODULE, mm_index+ SNMP_BC_HPI_LOCATION_BASE);
        e->u.res_event.entry.ResourceId =
                      oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
        snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
                      snmp_bc_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].comment,
                                               mm_index + SNMP_BC_HPI_LOCATION_BASE);

        trace("Discovered resource=%s; ID=%d",
                    e->u.res_event.entry.ResourceTag.Data,
                        e->u.res_event.entry.ResourceId);

        /* Create platform-specific info space to add to infra-structure */
        res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].res_info),
                        	                sizeof(struct ResourceInfo));
	if (!res_info_ptr) {
       		dbg("Out of memory.");
                g_free(e);
                return(SA_ERR_HPI_OUT_OF_SPACE);
        }
	
	*re = e;
	*rres_info_ptr = res_info_ptr;
	return(SA_OK);
}
/**
 * snmp_bc_discover_mm_i:
 * @handler: Pointer to handler's data.
 * @res_info_ptr: Pointer to resource info fetched from snmp_bc_resource.c.
 * @mm_index: Index of the management module (mm) 
 *
 * Discovers **a** management module (mm) resource and their RDRs, if any.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
 **/
SaErrorT snmp_bc_discover_mm_i(struct oh_handler_state *handle,
			  	  struct ResourceInfo *res_info_ptr, 
				  struct oh_event *e, 
				  guint mm_index)
{
	guint mm_width;
	SaErrorT err;
	struct snmp_value get_value, get_active;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !res_info_ptr || !e) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	trace("Discovering resources for installed management module %d.\n",mm_index);
	err = SA_OK;
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_MGMNT_ACTIVE, &get_active, SAHPI_TRUE);
	if (err || get_active.type != ASN_INTEGER) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
			     	SNMP_BC_MGMNT_ACTIVE, get_active.type, oh_lookup_error(err));
		if (err) { return(err); }
			else { return(SA_ERR_HPI_INTERNAL_ERROR); }
	}
			
        /* Set active MM location in handler's custom data  */
	/* - used to override duplicate MM events in snmp_bc_event.c */
        custom_handle->active_mm = get_active.integer;
	if (custom_handle->active_mm == mm_index+1) 
                res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;
	else 
		res_info_ptr->cur_state = SAHPI_HS_STATE_INACTIVE;
				
        /* Get UUID and convert to GUID */
        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

        /* Add resource to temporary event cache/queue */
        err = oh_add_resource(handle->rptcache,
                              &(e->u.res_event.entry),
                              res_info_ptr, 
			      0);
        if (err) {
        	dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
                g_free(e);
                return(err);
        }
        handle->eventq = g_slist_append(handle->eventq, e);
                
        /* Find resource's events, sensors, controls, etc. */
        snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			
	/* See if mmHeathState OID is supported; If it is a readable Operational State sensor
           is supported; else the Operational State sensor is event only */
	{
		struct snmp_value value;

		err = snmp_bc_snmp_get(custom_handle, SNMP_BC_MM_HEALTH_OID, &value, SAHPI_TRUE);
		if (err) {  
			snmp_bc_discover_sensors(handle, snmp_bc_mgmnt_sensors, e);
		}
		else { 
			snmp_bc_discover_sensors(handle, snmp_bc_mgmnt_health_sensors, e);
		}
	}

        snmp_bc_discover_controls(handle, snmp_bc_mgmnt_controls, e);
        snmp_bc_discover_inventories(handle, snmp_bc_mgmnt_inventories, e);

	mm_width = 1;    /* Default to 1-wide */
	if (res_info_ptr->mib.OidResourceWidth != NULL) {
		err = snmp_bc_oid_snmp_get(custom_handle,  
					   &(e->u.res_event.entry.ResourceEntity), 
					   0,
					   res_info_ptr->mib.OidResourceWidth, 
					   &get_value, 
					   SAHPI_TRUE);
					   
		if (!err && (get_value.type == ASN_INTEGER)) {
			mm_width = get_value.integer;
		}
	}			
	err = snmp_bc_set_resource_slot_state_sensor(handle, e, mm_width);
	return(err);
}
/**
 * snmp_bc_discover_ipmi_sensors:
 * @handler: Pointer to handler's data.
 * @sensor_array: Pointer to resource's static sensor data array.
 * @parent_res_event: Pointer to resource's event structure.
 *
 * Discovers resource's available IPMI sensors and their events.
 *
 * Return values:
 * Adds sensor RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 **/
static SaErrorT snmp_bc_discover_ipmi_sensors(struct oh_handler_state *handle,
					      struct snmp_bc_ipmi_sensor *sensor_array,
					      struct oh_event *res_oh_event)
{
	int i;
	GHashTable *ipmi_sensor_hash;
	SaErrorT err, rtn_code = SA_OK;
	struct SensorMibInfo *mib_info;
	struct snmp_bc_hnd *custom_handle;
	struct snmp_value get_value;
	
	custom_handle = (struct snmp_bc_hnd *)handle->data;

	/* Check if this is an IPMI blade */
	err = snmp_bc_oid_snmp_get(custom_handle,
				   &(res_oh_event->u.res_event.entry.ResourceEntity), 0,
				   SNMP_BC_IPMI_TEMP_BLADE_OID, &get_value, SAHPI_FALSE);
				  
        if (err || get_value.type != ASN_INTEGER) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		   SNMP_BC_IPMI_TEMP_BLADE_OID, get_value.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }
	if (get_value.integer == 0) return(SA_OK); /* Not an IPMI Blade */

	/* Create an temporary hash table and populate with all of 
           the blade's active IPMI sensors */
	ipmi_sensor_hash = g_hash_table_new(g_str_hash, g_str_equal);
	
	if (ipmi_sensor_hash == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	/***************************************** 
	 * Search for all the defined IPMI sensors
         *****************************************/

	/* Find blade's defined temperature IPMI sensors */
	for (i=0; i<SNMP_BC_MAX_IPMI_TEMP_SENSORS; i++) {
		err = snmp_bc_oid_snmp_get(custom_handle,
					   &(res_oh_event->u.res_event.entry.ResourceEntity), 0,
					   snmp_bc_ipmi_sensors_temp[i].oid, &get_value, SAHPI_FALSE);
		if (!err) {
			char *hash_existing_key, *hash_value;
			gchar  **strparts = NULL;
			gchar  *s, *ipmi_tag;
			
			/* Find IPMI tag in returned value */
			strparts = g_strsplit(get_value.string, SNMP_BC_IPMI_STRING_DELIMITER, -1);
			if (strparts == NULL || strparts[0] == '\0') {
				dbg("Cannot split IPMI temp returned value=%s.", get_value.string);
				g_strfreev(strparts);
				continue;
			}
			ipmi_tag = g_strstrip(g_strdup(strparts[0]));
			g_strfreev(strparts);
			if (ipmi_tag == NULL || ipmi_tag[0] == '\0') {
				dbg("Stripped IPMI tag is NULL"); 
				g_free(ipmi_tag);
				continue;
			}
			
			/* Change IPMI Tag to upper case */
			for (s=ipmi_tag; *s; s++) { *s = g_ascii_toupper(*s); }
			
			trace("Found OID IPMI sensor=%s", ipmi_tag);

			/* Insert tag and OID info in temporary hash */
			if (!g_hash_table_lookup_extended(ipmi_sensor_hash,
							  ipmi_tag,
							  (gpointer)&hash_existing_key,
							  (gpointer)&hash_value)) {

				mib_info = g_memdup(&(snmp_bc_ipmi_sensors_temp[i]),
						    sizeof(struct SensorMibInfo));
				if (!mib_info) {
					dbg("Out of memory.");
					g_free(ipmi_tag);
					rtn_code = SA_ERR_HPI_OUT_OF_SPACE;
					goto CLEANUP;
				}
				g_hash_table_insert(ipmi_sensor_hash, ipmi_tag, mib_info);
			}
			else { /* Already exists */
				dbg("Duplicate IPMI OID=%s.", snmp_bc_ipmi_sensors_temp[i].oid);
				g_free(ipmi_tag);
			}
		}
	}

	/* Find blade's voltage IPMI sensors */
	for (i=0; i<SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS; i++) {
		err = snmp_bc_oid_snmp_get(custom_handle,
					   &(res_oh_event->u.res_event.entry.ResourceEntity), 0,
					   snmp_bc_ipmi_sensors_voltage[i].oid, &get_value, SAHPI_FALSE);
		if (!err) {
			char *hash_existing_key, *hash_value;
			gchar  **strparts = NULL;
			gchar  *s, *ipmi_tag;
			
			/* Find IPMI tag in returned value */
			strparts = g_strsplit(get_value.string, SNMP_BC_IPMI_STRING_DELIMITER, -1);
			if (strparts == NULL || strparts[0] == '\0') {
				dbg("Cannot split IPMI voltage returned value=%s.", get_value.string);
				g_strfreev(strparts);
				continue;
			}
			ipmi_tag = g_strstrip(g_strdup(strparts[0]));
			g_strfreev(strparts);
			if (ipmi_tag == NULL || ipmi_tag[0] == '\0') {
				dbg("Stripped IPMI tag is NULL"); 
				g_free(ipmi_tag);
				continue;
			}

			/* Change IPMI Tag to upper case */
			for (s=ipmi_tag; *s; s++) { *s = g_ascii_toupper(*s); }
			
			trace("Found OID IPMI sensor=%s", ipmi_tag);

			/* Insert tag and OID info in temporary hash */
			if (!g_hash_table_lookup_extended(ipmi_sensor_hash,
							  ipmi_tag,
							  (gpointer)&hash_existing_key,
							  (gpointer)&hash_value)) {

				mib_info = g_memdup(&(snmp_bc_ipmi_sensors_voltage[i]), sizeof(struct SensorMibInfo));
				if (!mib_info) {
					dbg("Out of memory.");
					g_free(ipmi_tag);
					rtn_code = SA_ERR_HPI_OUT_OF_SPACE;
					goto CLEANUP;
				}
				g_hash_table_insert(ipmi_sensor_hash, ipmi_tag, mib_info);
			}
			else { /* Already exists */
				dbg("Duplicate IPMI OID=%s.", snmp_bc_ipmi_sensors_voltage[i].oid);
				g_free(ipmi_tag);
			}
		}
	}

	/* Iterate thru all the possible IPMI sensors, if it's defined for this blade,
	   push up its RDR info to Infra-structure */
	for (i=0; sensor_array[i].ipmi.index != 0; i++) {
		mib_info = (struct SensorMibInfo *)g_hash_table_lookup(ipmi_sensor_hash, sensor_array[i].ipmi_tag);
		
		/* See if the tag has an alias */
		if (!mib_info && (sensor_array[i].ipmi_tag_alias1 && sensor_array[i].ipmi_tag_alias1[0] != '\0')) {
			mib_info = (struct SensorMibInfo *)g_hash_table_lookup(ipmi_sensor_hash, sensor_array[i].ipmi_tag_alias1);	
		}

		if (mib_info) {
			struct oh_event *e;
			struct SensorInfo *sinfo;

			e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
			if (e == NULL) {
				dbg("Out of memory.");
				rtn_code = SA_ERR_HPI_OUT_OF_SPACE;
				goto CLEANUP;
			}

			e->type = OH_ET_RDR;
			e->did = oh_get_default_domain_id();
			e->u.rdr_event.parent = res_oh_event->u.res_event.entry.ResourceId;
			e->u.rdr_event.rdr.RdrType = SAHPI_SENSOR_RDR;
			e->u.rdr_event.rdr.Entity = res_oh_event->u.res_event.entry.ResourceEntity;
			err = snmp_bc_mod_sensor_ep(e, sensor_array, i);
			e->u.rdr_event.rdr.RdrTypeUnion.SensorRec = sensor_array[i].ipmi.sensor;

			trace("Blade Found IPMI Sensor=%s", sensor_array[i].ipmi.comment);

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), sensor_array[i].ipmi.comment);
			
			sinfo = g_memdup(&(sensor_array[i].ipmi.sensor_info), sizeof(struct SensorInfo));
			if (!sinfo) {
				dbg("Out of memory.");
				rtn_code = SA_ERR_HPI_OUT_OF_SPACE;
				g_free(e);
				goto CLEANUP;
			}

			sinfo->mib = *mib_info;
			
			err = oh_add_rdr(handle->rptcache,
					 res_oh_event->u.res_event.entry.ResourceId,
					 &(e->u.rdr_event.rdr),
					 sinfo, 0);
			if (err) {
				dbg("Cannot add RDR. Error=%s.", oh_lookup_error(err));
				g_free(e);
			}
			else {
				handle->eventq = g_slist_append(handle->eventq, e);
				snmp_bc_discover_sensor_events(handle,
							       &(res_oh_event->u.res_event.entry.ResourceEntity),
							       sensor_array[i].ipmi.sensor.Num,
							       &(sensor_array[i].ipmi));
			}
		}
	}

 CLEANUP:
        /* Destroy temporary hash table */
	g_hash_table_foreach(ipmi_sensor_hash, free_hash_data, NULL);
	g_hash_table_destroy(ipmi_sensor_hash);

	return(rtn_code);
}

static void free_hash_data(gpointer key, gpointer value, gpointer user_data)
{
        g_free(key);
        g_free(value);
}

/**
 * snmp_bc_rediscover: 
 * @handler: Pointer to handler's data.
 * @event:   Pointer to event being processed.
 *
 * Check install masks and target discovery 
 *   -- If resource is removed, then remove rpt and associated rdr's
 *   -- In resource is newly installed, then rediscover ...
 *
 * Return values:
 *
 **/
SaErrorT snmp_bc_rediscover(struct oh_handler_state *handle,
			  		SaHpiEventT *event, 
					LogSource2ResourceT *logsrc2res)
{
	SaErrorT err;
	gint i;
	gint rediscovertype;
        SaHpiRptEntryT *res;	
	SaHpiBoolT foundit;
	struct oh_event *e;
	struct snmp_value get_value;
	struct snmp_bc_hnd *custom_handle;
	char *root_tuple;
	struct ResourceInfo *resinfo;
	char resource_mask[SNMP_BC_MAX_RESOURCES_MASK];
	SaHpiEntityPathT     ep_root;
	SaHpiEntityTypeT     hotswap_entitytype;
    	SaHpiEntityLocationT hotswap_entitylocation;


	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	memset(&resource_mask, 0, SNMP_BC_MAX_RESOURCES_MASK);
			
	rediscovertype = 0; /* Default - do nothing */
	if (event->EventType == SAHPI_ET_HOTSWAP) {
		if (event->EventDataUnion.HotSwapEvent.PreviousHotSwapState == SAHPI_HS_STATE_NOT_PRESENT)
		{
			if (event->EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_NOT_PRESENT)
				dbg("Sanity check FAILED! PreviousHotSwapState = HotSwapState == SAHPI_HS_STATE_NOT_PRESENT\n");
			rediscovertype = 1;  /* New resource is installed  */			
		}
		else if (event->EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_NOT_PRESENT)
		{ 
			if (event->EventDataUnion.HotSwapEvent.PreviousHotSwapState == SAHPI_HS_STATE_NOT_PRESENT)
				dbg("Sanity check FAILED! PreviousHotSwapState = HotSwapState == SAHPI_HS_STATE_NOT_PRESENT\n");
			rediscovertype = 2;  /* resource is removed  */					
		} 
	 }
	 
	/* ------------------------------------------------------------------ */
	/* Parse EntityPath to find out the type of resource being hotswapped */
	/* ------------------------------------------------------------------ */
	memset(resource_mask, '\0', SNMP_BC_MAX_RESOURCES_MASK);
	foundit = SAHPI_FALSE;
	hotswap_entitytype = SAHPI_ENT_UNKNOWN;
    	hotswap_entitylocation = SNMP_BC_NOT_VALID;   /* Invalid location                   */
						 /* Do not use 0 for invalid location  */
						 /* because virtual resource has loc 0 */
	for (i=0; logsrc2res->ep.Entry[i].EntityType != SAHPI_ENT_SYSTEM_CHASSIS; i++) {
		switch (logsrc2res->ep.Entry[i].EntityType) {
			case SAHPI_ENT_SBC_BLADE:
			case SAHPI_ENT_FAN: 
			case SAHPI_ENT_POWER_SUPPLY: 
			case SAHPI_ENT_INTERCONNECT: 
			case SAHPI_ENT_SYS_MGMNT_MODULE:
			case SAHPI_ENT_PERIPHERAL_BAY:
				foundit = SAHPI_TRUE;					
				hotswap_entitytype = logsrc2res->ep.Entry[i].EntityType;
    				hotswap_entitylocation = logsrc2res->ep.Entry[i].EntityLocation;
				for (i=0; i < SNMP_BC_MAX_RESOURCES_MASK; i++) {
					if (  i != (hotswap_entitylocation - 1) ) 
								resource_mask[i] = '0';
					else resource_mask[i] = '1';
				}
					
				break;
			default: 
				break;
		}
		
		if (foundit) break;
	}
	
	if ( (!foundit)  || ( hotswap_entitylocation == 0xFF) ) {
		dbg("Hotswap event for non hotswap-able resource\n");
		return(SA_OK);
	}	 
	/* ------------------------------------------------------------------ */
	/* Hotswap: removal ...                                               */ 
	/* remove rpt and associated rdrs of the removed resource             */ 
	/* ------------------------------------------------------------------ */
	{
	
	if (rediscovertype == 2) {
		res = oh_get_resource_by_id(handle->rptcache, event->Source);
		resinfo = (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, event->Source);
					
        	if (res)  {
			/* Create remove resource event and add to event queue */
			e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                	if (e) {
				e->did = oh_get_default_domain_id();
                        	e->type = OH_ET_RESOURCE_DEL;
                        	e->u.res_event.entry.ResourceId = res->ResourceId;
                        	handle->eventq = g_slist_append(handle->eventq, e);
                	} else { dbg("Out of memory."); }

			if (resinfo) {
				resinfo->cur_state = SAHPI_HS_STATE_NOT_PRESENT;
			}

			err = snmp_bc_reset_resource_slot_state_sensor(handle, res);
                	oh_remove_resource(handle->rptcache, res->ResourceId);
			
			switch (hotswap_entitytype) {
				case SAHPI_ENT_SBC_BLADE:
				/* Fetch blade installed vector */
					get_installed_mask(SNMP_BC_PB_INSTALLED, get_value);
					strcpy(custom_handle->installed_pb_mask, get_value.string);
					break;
				case SAHPI_ENT_FAN:
				/* Fetch blower installed vector */
					get_installed_mask(SNMP_BC_BLOWER_INSTALLED, get_value);
					strcpy(custom_handle->installed_blower_mask, get_value.string);
					break;
				case SAHPI_ENT_POWER_SUPPLY:
				/* Fetch power module installed vector */
					get_installed_mask(SNMP_BC_PM_INSTALLED, get_value);
					strcpy(custom_handle->installed_pm_mask, get_value.string);
					break;
				case SAHPI_ENT_INTERCONNECT:
					/* Fetch switch installed vector */
					get_installed_mask(SNMP_BC_SM_INSTALLED, get_value);
					strcpy(custom_handle->installed_sm_mask, get_value.string);
					break;
				case SAHPI_ENT_SYS_MGMNT_MODULE:
					/* Fetch MMs installed vector */
					get_installed_mask(SNMP_BC_MM_INSTALLED, get_value);
					strcpy(custom_handle->installed_mm_mask, get_value.string);
					break;
				case SAHPI_ENT_PERIPHERAL_BAY:
					get_integer_object(SNMP_BC_MT_INSTALLED, get_value);
					custom_handle->installed_mt_mask = get_value.integer;
					break;
				default: 
					break;
			}			
                
        	} else dbg("No valid resource at hand. Could not remove resource.");
		
		return(SA_OK);
	}
	
	}		
	/* ------------------------------------------------------------------ */
	/* Hotswap: insertion ...                                             */ 
	/* remove rpt and associated rdrs of the removed resource             */ 
	/* ------------------------------------------------------------------ */
	if ( rediscovertype == 1)
	{
		/* e and resinfo got malloc in construct_res functions below */
		e = NULL;
		resinfo = NULL;

		oh_init_ep(&ep_root);
		root_tuple = (gchar *)g_hash_table_lookup(handle->config, "entity_root");
        	oh_encode_entitypath(root_tuple, &ep_root);

		/* --------------------------------------------------------- */
		/* Fetch various resource installation maps from BladeCenter */
		/* --------------------------------------------------------- */	

		switch (hotswap_entitytype) {
			case SAHPI_ENT_SBC_BLADE:
				/* Fetch blade installed vector */
				get_installed_mask(SNMP_BC_PB_INSTALLED, get_value);
				for (i=0; i < strlen(get_value.string); i++) {
					if ( custom_handle->installed_pb_mask[i] != 
								get_value.string[i] ) {
						err = snmp_bc_construct_blade_res(&e, &resinfo, &ep_root, i);
						if (!err) 
						err = snmp_bc_discover_blade_i(handle, resinfo, e, &ep_root, i);								
					}		
				}
				strcpy(custom_handle->installed_pb_mask, get_value.string);
				break;
			case SAHPI_ENT_FAN:
				/* Fetch blower installed vector */
				get_installed_mask(SNMP_BC_BLOWER_INSTALLED, get_value);
				for (i=0; i < strlen(get_value.string); i++) {
					if ( custom_handle->installed_blower_mask[i] != 
								get_value.string[i] ) {
						err = snmp_bc_construct_blower_res(&e, &resinfo, &ep_root, i);
						if (!err) 
						err = snmp_bc_discover_blower_i(handle, resinfo, e,i);								
					}		
				}				
				
				strcpy(custom_handle->installed_blower_mask, get_value.string);
				break;
			case SAHPI_ENT_POWER_SUPPLY:
				/* Fetch power module installed vector */
				get_installed_mask(SNMP_BC_PM_INSTALLED, get_value);
				for (i=0; i < strlen(get_value.string); i++) {
					if ( custom_handle->installed_pm_mask[i] != 
									get_value.string[i] ) {
						err = snmp_bc_construct_pm_res(&e, &resinfo, &ep_root, i);
						if(!err)
						err = snmp_bc_discover_pm_i(handle, resinfo, e,i);								
					}		
				}								
				
				strcpy(custom_handle->installed_pm_mask, get_value.string);
				break;
			case SAHPI_ENT_INTERCONNECT:
				/* Fetch switch installed vector */
				get_installed_mask(SNMP_BC_SM_INSTALLED, get_value);
				for (i=0; i < strlen(get_value.string); i++) {
					if ( custom_handle->installed_sm_mask[i] != 
									get_value.string[i] ) {
						err = snmp_bc_construct_sm_res(&e, &resinfo, &ep_root, i);
						if(!err) 
						err = snmp_bc_discover_switch_i(handle, resinfo, e,i);								
					}		
				}											
				strcpy(custom_handle->installed_sm_mask, get_value.string);
				break;
			case SAHPI_ENT_SYS_MGMNT_MODULE:
				/* Fetch MMs installed vector */
				get_installed_mask(SNMP_BC_MM_INSTALLED, get_value);
				for (i=0; i < strlen(get_value.string); i++) {
					if ( custom_handle->installed_mm_mask[i] != 
								get_value.string[i] ) {
						err = snmp_bc_construct_mm_res(&e, &resinfo, &ep_root, i);
						if (!err) 
						err = snmp_bc_discover_mm_i(handle, resinfo, e,i);								
					}		
				}			
				strcpy(custom_handle->installed_mm_mask, get_value.string);
				break;
			case SAHPI_ENT_PERIPHERAL_BAY:
				// get_integer_object(custom_handle,SNMP_BC_MEDIATRAY_EXISTS, get_value);
				get_integer_object(SNMP_BC_MT_INSTALLED, get_value);
				custom_handle->installed_mt_mask = get_value.integer;
				err = snmp_bc_discover_media_tray(handle, &ep_root, hotswap_entitylocation);
				break;
			default: 
				break;
		}

		/* ------------------------------------------------------------- */	
		/* ------------------------------------------------------------- */
		/* ------------------------------------------------------------- */
		/* ------------------------------------------------------------- */		
		
		/** 
		 **  Before returning, see if we need to readjust current Hotswap state.
		 **  (1) Previously, snmp_bc_log2event()/snmp_bc_set_cur_prev_event_states() set 
		 **      HotSwapState =  SAHPI_HS_STATE_INACTIVE by default if there **was** no rpt,
		 **      no resinfo.   
		 **  (2) Now that rediscovery is complete, check handle->rptcache for this resource
		 **      CAPABILITY.  If it is Managed Hotswap, then INACTIVE HotSwapState is OK.
		 **      If it is Simple Hotswap, then HotSwapState needs to be set to ACTIVE in both event
		 **      and resinfo. 
		 **/
		 res = oh_get_resource_by_ep(handle->rptcache, &logsrc2res->ep);
		 if (res) {
		 	if ( (event->EventType == SAHPI_ET_HOTSWAP) 
					&& (event->EventDataUnion.HotSwapEvent.HotSwapState == SAHPI_HS_STATE_INACTIVE) ) 
			{
				if (!(res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
				{
					resinfo = (struct ResourceInfo *)oh_get_resource_data(handle->rptcache, event->Source);
			
					resinfo->cur_state =
						event->EventDataUnion.HotSwapEvent.HotSwapState = 
												SAHPI_HS_STATE_ACTIVE;
				}
			}
		}
	}	
	return(SA_OK);
}

/**
 * snmp_bc_discover_all_slots
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 *
 * Discovers all BladeCenter physical slots.                 
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
  **/
SaErrorT snmp_bc_discover_all_slots(struct oh_handler_state *handle,
					SaHpiEntityPathT *ep_root)
{

	guint i;
	SaErrorT err;
	struct snmp_bc_hnd *custom_handle;

	if (!handle || !ep_root) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	for (i = 0; i < custom_handle->max_pb_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, SAHPI_ENT_PHYSICAL_SLOT,i); 
	}							

	for (i = 0; i < custom_handle->max_blower_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, BLADECENTER_BLOWER_SLOT,i);
	}							
	for (i = 0; i < custom_handle->max_pm_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, BLADECENTER_POWER_SUPPLY_SLOT,i);
	}
								
	for (i = 0; i < custom_handle->max_sm_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, BLADECENTER_INTERCONNECT_SLOT,i);
	}
								
	for (i = 0; i < custom_handle->max_mm_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, BLADECENTER_SYS_MGMNT_MODULE_SLOT,i);
	}
								
	for (i = 0; i < custom_handle->max_mt_supported; i++) {
		err = snmp_bc_discover_slot(handle, ep_root, BLADECENTER_PERIPHERAL_BAY_SLOT,i);
	}
	
	return(SA_OK);							
}

/**
 * snmp_bc_discovery_slot:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf.
 * @entitytype: Resource type of the slot.
 * @entitylocation: Slot location of the resource.
 *
 * Discovers slot resources and their RDRs.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) NULL.
  **/
SaErrorT snmp_bc_discover_slot( struct oh_handler_state *handle,
			         SaHpiEntityPathT *ep_root,
				 SaHpiEntityTypeT entitytype,
				 guint		  entitylocation) 
{

	SaErrorT err;
	char *comment;
        struct oh_event *e;
	struct snmp_bc_hnd *custom_handle;
	struct ResourceInfo *res_info_ptr;

	if (!handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	e = NULL;
	res_info_ptr = NULL;
	
	e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	e->type = OH_ET_RESOURCE;
	e->did = oh_get_default_domain_id();
	e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_PHYSICAL_SLOT].rpt;
	oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
	oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
			SAHPI_ENT_CHASSIS_SPECIFIC, entitylocation + SNMP_BC_HPI_LOCATION_BASE);
			
			
	switch (entitytype) {
		case SAHPI_ENT_PHYSICAL_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = SAHPI_ENT_PHYSICAL_SLOT;
			comment = SNMP_BC_PHYSICAL_SLOT;
			break;
			
		case BLADECENTER_INTERCONNECT_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = BLADECENTER_INTERCONNECT_SLOT;
			comment = SNMP_BC_INTERCONNECT_SLOT;
			break;
			
		case BLADECENTER_POWER_SUPPLY_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = BLADECENTER_POWER_SUPPLY_SLOT;
			comment = SNMP_BC_POWER_SUPPLY_SLOT;
			break;
			
		case BLADECENTER_PERIPHERAL_BAY_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = BLADECENTER_PERIPHERAL_BAY_SLOT;
			comment = SNMP_BC_PERIPHERAL_BAY_SLOT;
			break;
			
		case BLADECENTER_SYS_MGMNT_MODULE_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = BLADECENTER_SYS_MGMNT_MODULE_SLOT;
			comment = SNMP_BC_SYS_MGMNT_MODULE_SLOT;
			break;
			
		case BLADECENTER_BLOWER_SLOT:
			e->u.res_event.entry.ResourceEntity.Entry[0].EntityType = BLADECENTER_BLOWER_SLOT;
			comment = SNMP_BC_BLOWER_SLOT;
			break;
			
		default:
			dbg("Invalid slot resource type\n");
			return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	
	e->u.res_event.entry.ResourceId =
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
		
	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
				     comment,
				     entitylocation + SNMP_BC_HPI_LOCATION_BASE);
				     
	res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_PHYSICAL_SLOT].res_info),
						sizeof(struct ResourceInfo));		
	if (!res_info_ptr) {
		dbg("Out of memory.");
		g_free(e);
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
	err = oh_add_resource(handle->rptcache,
				&(e->u.res_event.entry),
				res_info_ptr, 0);
	if (err) { 
		dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	
	handle->eventq = g_slist_append(handle->eventq, e);
        snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
        snmp_bc_discover_sensors(handle, snmp_bc_slot_sensors, e);
        snmp_bc_discover_controls(handle, snmp_bc_slot_controls, e);
        snmp_bc_discover_inventories(handle, snmp_bc_slot_inventories, e);
			
	return(SA_OK);
}
