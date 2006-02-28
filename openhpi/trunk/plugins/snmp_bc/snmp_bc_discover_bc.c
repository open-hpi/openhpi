/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Sean Dague <sdague@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 *      Chris Chia <cchia@users.sf.net>
 */

#include <glib.h>
#include <snmp_bc_plugin.h>

static void free_hash_data(gpointer key, gpointer value, gpointer user_data);

#define SNMP_BC_IPMI_STRING_DELIMITER "="

#define SNMP_BC_MAX_IPMI_TEMP_SENSORS 6
#define SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS 25

#define SNMP_BC_IPMI_TEMP_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.11.x"
#define SNMP_BC_IPMI_VOLTAGE_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.14.x"

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
};

static SaErrorT snmp_bc_discover_ipmi_sensors(struct oh_handler_state *handle,
					      struct snmp_bc_ipmi_sensor *sensor_array,
					      struct oh_event *res_oh_event);


SaErrorT snmp_bc_discover(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root)
{

	SaErrorT err;
	struct snmp_value get_value_blade, get_value_fan,
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

	/* --------------------------------------------------------- */
	/* Fetch various resource installation maps from BladeCenter */
	/* --------------------------------------------------------- */

	/* Fetch blade installed vector from BladeCenter */
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_BLADE_VECTOR, &get_value_blade, SAHPI_TRUE);
        if (err || get_value_blade.type != ASN_OCTET_STR) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_BLADE_VECTOR, get_value_blade.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	/* Fetch fan installed vector from BladeCenter */
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_FAN_VECTOR, &get_value_fan, SAHPI_TRUE);
        if (err || get_value_fan.type != ASN_OCTET_STR) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_FAN_VECTOR, get_value_fan.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	/* Fetch power module installed vector from BladeCenter */ 
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_POWER_VECTOR, &get_value_power_module, SAHPI_TRUE);
        if (err || get_value_power_module.type != ASN_OCTET_STR) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_POWER_VECTOR, get_value_power_module.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	/* Fetch switch installed vector from BladeCenter */
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_SWITCH_VECTOR, &get_value_switch, SAHPI_TRUE);
        if (err || get_value_switch.type != ASN_OCTET_STR) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_SWITCH_VECTOR, get_value_switch.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_MEDIATRAY_EXISTS, &get_value_media, SAHPI_TRUE);
        if (err || get_value_media.type != ASN_INTEGER) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_MEDIATRAY_EXISTS, get_value_media.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }

	/* Fetch MMs installed vector from BladeCenter */
	err = snmp_bc_snmp_get(custom_handle, SNMP_BC_MGMNT_VECTOR, &get_value_mm, SAHPI_TRUE);
        if (err || get_value_mm.type != ASN_OCTET_STR) {
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
		      SNMP_BC_MGMNT_VECTOR, get_value_mm.type, oh_lookup_error(err));
		if (err) { return(err); }
		else { return(SA_ERR_HPI_INTERNAL_ERROR); }
        }
															
	/****************** 
	 * Discover Chassis
	 ******************/
	err = snmp_bc_discover_chassis(handle, ep_root);
	if (err != SA_OK) return(err);
				  
	/***************** 
	 * Discover Blades
	 *****************/
	err = snmp_bc_discover_blade(handle, ep_root,get_value_blade.string);
	if (err != SA_OK) return(err);
				  			
        /*************** 
	 * Discover Fans
	 ***************/
	err = snmp_bc_discover_fans(handle, ep_root, get_value_fan.string);
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
	 * Discover Media Tray
	 *********************/
	err = snmp_bc_discover_media_tray(handle, ep_root, get_value_media.integer);
	if (err != SA_OK) return(err);

	/***************************** 
	 * Discover Management Modules (MMs)
	 *****************************/
	err = snmp_bc_discover_mm(handle, ep_root, get_value_mm.string);
	if (err != SA_OK) return(err);

	return(SA_OK);
}

/**
 * snmp_bc_discover_media_tray:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf
 * @media_tray_installed: Indicator whether medis tray is installed in system
 *
 * Discovers media tray resources.
 *
 * Return values:
 * Adds chassis RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_media_tray(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, int  media_tray_installed)
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

	if (media_tray_installed) {
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

		res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                /* Get UUID and convert to GUID */
                err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);
		
		/* Add resource to temporary event cache/queue */
		err = oh_add_resource(custom_handle->tmpcache, 
				      &(e->u.res_event.entry),
				      res_info_ptr, 0);
		if (err) {
			dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
			g_free(e);
			return(err);
		}
		custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
		
		/* Find resource's events, sensors, controls, etc. */
		snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
		snmp_bc_discover_sensors(handle, snmp_bc_mediatray_sensors, e);
		snmp_bc_discover_controls(handle, snmp_bc_mediatray_controls, e);
		snmp_bc_discover_inventories(handle, snmp_bc_mediatray_inventories, e);
	}
	
	return(SA_OK);
}
/**
 * snmp_bc_discover_chassis:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf
 *
 * Discovers chassis resources.
 *
 * Return values:
 * Adds chassis RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
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
	}
	else {
		e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_CHASSIS].rpt;	
	}

	e->u.res_event.entry.ResourceEntity = *ep_root;
	e->u.res_event.entry.ResourceId = 
		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
	{ /* Generate Chassis Resource Tag */
		SaHpiTextBufferT build_name;

		oh_init_textbuffer(&build_name);
		oh_append_textbuffer(&build_name, snmp_bc_rpt_array[BC_RPT_ENTRY_CHASSIS].comment);
		if (custom_handle->platform == SNMP_BC_PLATFORM_BC) {
			oh_append_textbuffer(&build_name, " Integrated Chassis");
		}
		else {
			if (custom_handle->platform == SNMP_BC_PLATFORM_BCT) {
				oh_append_textbuffer(&build_name, " Telco Chassis");
			}
			else {
				oh_append_textbuffer(&build_name, " Chassis");
			}
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
	err = oh_add_resource(custom_handle->tmpcache, 
			      &(e->u.res_event.entry), 
			      res_info_ptr, 0);
	if (err) {
		dbg("Cannot add resource. Error=%s.", oh_lookup_error(err));
		g_free(e);
		return(err);
	}
	custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);

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
 * snmp_bc_discover_blades:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf
 * @blade_vector: String of installed blade bitmap
 *
 * Discovers blade resources.
 *
 * Return values:
 * Adds blade RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_blade(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, char *blade_vector)
{

	int i;
	SaErrorT err;
        struct oh_event *e;
	struct snmp_value get_value, get_blade_resourcetag;
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
	
	for (i=0; i < strlen(blade_vector); i++) {
		if (blade_vector[i] == '1') {

			e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
			if (e == NULL) {
				dbg("Out of memory.");
				return(SA_ERR_HPI_OUT_OF_SPACE);
			}

			e->type = OH_ET_RESOURCE;
			e->did = oh_get_default_domain_id();
			e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].rpt;
			oh_concat_ep(&(e->u.res_event.entry.ResourceEntity), ep_root);
			oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
					   SAHPI_ENT_PHYSICAL_SLOT, i + SNMP_BC_HPI_LOCATION_BASE);
			oh_set_ep_location(&(e->u.res_event.entry.ResourceEntity),
					   SAHPI_ENT_SBC_BLADE, i + SNMP_BC_HPI_LOCATION_BASE);
			e->u.res_event.entry.ResourceId = 
				oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
				
			
		  	err = snmp_bc_oid_snmp_get(custom_handle,
					   &(e->u.res_event.entry.ResourceEntity),
					   snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].OidResourceTag,
					   &get_blade_resourcetag, SAHPI_TRUE);
					   
			if (!err && (get_blade_resourcetag.type == ASN_OCTET_STR)) {
				snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
							   get_blade_resourcetag.string,
							   SNMP_BC_NOT_VALID);
			
			} else {			
				snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
							   snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].comment,
							   i + SNMP_BC_HPI_LOCATION_BASE);
			}
			


			trace("Discovered resource=%s; ID=%d", 
			      e->u.res_event.entry.ResourceTag.Data,
			      e->u.res_event.entry.ResourceId);

			/* Create platform-specific info space to add to infra-structure */
			res_info_ptr = g_memdup(&(snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE].res_info),
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
			err = oh_add_resource(custom_handle->tmpcache, 
					      &(e->u.res_event.entry),
					      res_info_ptr, 0);
			if (err) {
				dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
				g_free(e);
				return(err);
			}
			custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			
			/* Find resource's events, sensors, controls, etc. */
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			snmp_bc_discover_sensors(handle, snmp_bc_blade_sensors, e);
			snmp_bc_discover_ipmi_sensors(handle, snmp_bc_blade_ipmi_sensors, e);
			snmp_bc_discover_controls(handle, snmp_bc_blade_controls, e);
			snmp_bc_discover_inventories(handle, snmp_bc_blade_inventories, e);
			
			/******************************** 
			 * Discover Blade Expansion Cards
			 ********************************/
			{
				SaHpiEntityPathT ep;

				ep = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].rpt.ResourceEntity;
				oh_concat_ep(&ep, ep_root);
				oh_set_ep_location(&ep, SAHPI_ENT_SYS_EXPANSION_BOARD, i + SNMP_BC_HPI_LOCATION_BASE);
				oh_set_ep_location(&ep, SAHPI_ENT_PHYSICAL_SLOT, i + SNMP_BC_HPI_LOCATION_BASE);
				oh_set_ep_location(&ep, SAHPI_ENT_SBC_BLADE, i + SNMP_BC_HPI_LOCATION_BASE);


				err = snmp_bc_oid_snmp_get(custom_handle, &ep, SNMP_BC_BLADE_EXPANSION_VECTOR, &get_value, SAHPI_TRUE);

				if (!err && get_value.integer != 0) {

					/* Found an expansion card */
					e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
					if (e == NULL) {
						dbg("Out of memory.");
						return(SA_ERR_HPI_OUT_OF_SPACE);
					}
	
					e->type = OH_ET_RESOURCE;
					e->did = oh_get_default_domain_id();
					e->u.res_event.entry = snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].rpt;
					e->u.res_event.entry.ResourceEntity = ep;
					e->u.res_event.entry.ResourceId = oh_uid_from_entity_path(&ep);
					snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
								   snmp_bc_rpt_array[BC_RPT_ENTRY_BLADE_EXPANSION_CARD].comment,
								   i + SNMP_BC_HPI_LOCATION_BASE);

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
					err = oh_add_resource(custom_handle->tmpcache, 
							      &(e->u.res_event.entry),
							      res_info_ptr, 0);
					if (err) {
						dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
						g_free(e);
						return(err);
					}
					custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			
					/* Find resource's events, sensors, controls, etc. */
					snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
					snmp_bc_discover_sensors(handle, snmp_bc_blade_expansion_sensors, e);
					snmp_bc_discover_controls(handle, snmp_bc_blade_expansion_controls, e);
					snmp_bc_discover_inventories(handle, snmp_bc_blade_expansion_inventories, e);
				}
			}
		}
	}

	return(SA_OK);
}

/**
 * snmp_bc_discover_fans:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf 
 * @fan_vector: String of installed fan bitmap
 *
 * Discovers fan resources.
 *
 * Return values:
 * Adds fan RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_fans(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, char *fan_vector)
{

	int i;
	SaErrorT err;
        struct oh_event *e;
	struct ResourceInfo *res_info_ptr;
	struct snmp_bc_hnd *custom_handle;


	if (!handle || !fan_vector) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	for (i=0; i < strlen(fan_vector); i++) {
		if (fan_vector[i] == '1') {
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
					   SAHPI_ENT_FAN, i + SNMP_BC_HPI_LOCATION_BASE);
			e->u.res_event.entry.ResourceId = 
				oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
			snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
						   snmp_bc_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].comment,
						   i + SNMP_BC_HPI_LOCATION_BASE);

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

			res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                        /* Get UUID and convert to GUID */
                        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

			/* Add resource to temporary event cache/queue */
			err = oh_add_resource(custom_handle->tmpcache, 
					      &(e->u.res_event.entry),
					      res_info_ptr, 0);
			if (err) {
				dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
				g_free(e);
				return(err);
			}
			custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			
			/* Find resource's events, sensors, controls, etc. */
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			snmp_bc_discover_sensors(handle, snmp_bc_fan_sensors, e);
			snmp_bc_discover_controls(handle, snmp_bc_fan_controls, e);
			snmp_bc_discover_inventories(handle, snmp_bc_fan_inventories, e);
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_discover_power_module:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf 
 * @power_module_vector: String of installed power module bitmap
 *
 * Discovers power module resources.
 *
 * Return values:
 * Adds power module RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_power_module(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, char *power_module_vector)
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


	for (i=0; i < strlen(power_module_vector); i++) {
		if (power_module_vector[i] == '1') {
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
					   SAHPI_ENT_POWER_SUPPLY, i + SNMP_BC_HPI_LOCATION_BASE);
			e->u.res_event.entry.ResourceId = 
				oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
			snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
						   snmp_bc_rpt_array[BC_RPT_ENTRY_POWER_MODULE].comment,
						   i + SNMP_BC_HPI_LOCATION_BASE);

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

			res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                        /* Get UUID and convert to GUID */
                        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

			/* Add resource to temporary event cache/queue */
			err = oh_add_resource(custom_handle->tmpcache, 
					      &(e->u.res_event.entry),
					      res_info_ptr, 0);
			if (err) {
				dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
				g_free(e);
				return(err);
			}
			custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			
			/* Find resource's events, sensors, controls, etc. */
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			snmp_bc_discover_sensors(handle, snmp_bc_power_sensors, e);
			snmp_bc_discover_controls(handle, snmp_bc_power_controls, e);
			snmp_bc_discover_inventories(handle, snmp_bc_power_inventories, e);
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_discover_switch:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf 
 * @switch_vector: String of installed switch bitmap 
 *
 * Discovers switch resources.
 *
 * Return values:
 * Adds switch RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_switch(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, char *switch_vector)
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

	for (i=0; i < strlen(switch_vector); i++) {
		if (switch_vector[i] == '1') {
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
					   SAHPI_ENT_INTERCONNECT, i + SNMP_BC_HPI_LOCATION_BASE);
			e->u.res_event.entry.ResourceId = 
				oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
			snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
						   snmp_bc_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].comment,
						   i + SNMP_BC_HPI_LOCATION_BASE);

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

			res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                        /* Get UUID and convert to GUID */
                        err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

			/* Add resource to temporary event cache/queue */
			err = oh_add_resource(custom_handle->tmpcache, 
					      &(e->u.res_event.entry),
					      res_info_ptr, 0);
			if (err) {
				dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
				g_free(e);
				return(err);
			}
			custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			
			/* Find resource's events, sensors, controls, etc. */
			snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
			snmp_bc_discover_sensors(handle, snmp_bc_switch_sensors, e);
			snmp_bc_discover_controls(handle, snmp_bc_switch_controls, e);
			snmp_bc_discover_inventories(handle, snmp_bc_switch_inventories, e);
		}
	}
	return(SA_OK);
}

/**
 * snmp_bc_discover_mm:
 * @handler: Pointer to handler's data.
 * @ep_root: Pointer to chassis Root Entity Path which comes from openhpi.conf 
 * @mm_vector: String of installed MM bitmap 
 *
 * Discovers management module (mm) resources.
 *
 * Return values:
 * Adds switch RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 * SA_ERR_HPI_INVALID_PARAMS - Invalid pointers passed in
  **/
SaErrorT snmp_bc_discover_mm(struct oh_handler_state *handle,
			  SaHpiEntityPathT *ep_root, char *mm_vector)
{

	int i;
	SaErrorT err;
        struct oh_event *e;
	struct snmp_value get_value, get_active;
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

	for (i=0; i < strlen(mm_vector); i++) {
		trace("Management Module installed bit map %s\n", get_value.string);
		if (mm_vector[i] == '1'){
			err = snmp_bc_snmp_get(custom_handle, SNMP_BC_MGMNT_ACTIVE, &get_active, SAHPI_TRUE);
			if (err || get_active.type != ASN_INTEGER) {
				dbg("Cannot get OID=%s; Received Type=%d; Error=%s.",
			      		SNMP_BC_MGMNT_ACTIVE, get_active.type, oh_lookup_error(err));
				if (err) { return(err); }
				else { return(SA_ERR_HPI_INTERNAL_ERROR); }
			}
                        trace("Active Management Module Id %ld\n", get_active.integer);
			
                	/* Set active MM location in handler's custom data  */
			/* - used to override duplicate MM events in snmp_bc_event.c */
                	custom_handle->active_mm = get_active.integer;

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
                                        SAHPI_ENT_SYS_MGMNT_MODULE, i + SNMP_BC_HPI_LOCATION_BASE);
                	e->u.res_event.entry.ResourceId =
                        		oh_uid_from_entity_path(&(e->u.res_event.entry.ResourceEntity));
                	snmp_bc_create_resourcetag(&(e->u.res_event.entry.ResourceTag),
                                                snmp_bc_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].comment,
                                                i + SNMP_BC_HPI_LOCATION_BASE);

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

                	res_info_ptr->cur_state = SAHPI_HS_STATE_ACTIVE;

                	/* Get UUID and convert to GUID */
                	err = snmp_bc_get_guid(custom_handle, e, res_info_ptr);

                	/* Add resource to temporary event cache/queue */
                	err = oh_add_resource(custom_handle->tmpcache,
                        	                &(e->u.res_event.entry),
                                	        res_info_ptr, 0);
                	if (err) {
                        	dbg("Failed to add resource. Error=%s.", oh_lookup_error(err));
                        	g_free(e);
                        	return(err);
                	}
                	custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
                
                	/* Find resource's events, sensors, controls, etc. */
                	snmp_bc_discover_res_events(handle, &(e->u.res_event.entry.ResourceEntity), res_info_ptr);
                	snmp_bc_discover_sensors(handle, snmp_bc_mgmnt_sensors, e);
                	snmp_bc_discover_controls(handle, snmp_bc_mgmnt_controls, e);
                	snmp_bc_discover_inventories(handle, snmp_bc_mgmnt_inventories, e);
		}
	}
	return(SA_OK);

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
				   &(res_oh_event->u.res_event.entry.ResourceEntity),
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

	/* Search for all the defined IPMI sensors.
         * Generic IPMI SNMP OID values are populated sequentially. There will not be any valid
         * sensors after the first "Not Readable!", "(No temperature)", or "(No voltage)" reading.
	 */

	/* Find blade's defined temperature IPMI sensors */
	for (i=0; i<SNMP_BC_MAX_IPMI_TEMP_SENSORS; i++) {
		err = snmp_bc_oid_snmp_get(custom_handle,
					   &(res_oh_event->u.res_event.entry.ResourceEntity),
					   snmp_bc_ipmi_sensors_temp[i].oid, &get_value, SAHPI_FALSE);
		if (err ||
		    (strncmp(get_value.string, "(No temperature)", sizeof("(No temperature)")) == 0) ||
		    (strncmp(get_value.string, "Not Readable!", sizeof("Not Readable!")) == 0)) {
			break;
		}
		else {
			char *hash_existing_key, *hash_value;
			gchar  **strparts = NULL;
			gchar  *ipmi_tag;
			
			/* Find IPMI tag in returned value */
			strparts = g_strsplit(get_value.string, SNMP_BC_IPMI_STRING_DELIMITER, -1);
			if (strparts == NULL || strparts[0] == '\0') {
				dbg("Cannot split IPMI returned value=%s.", get_value.string);
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

			trace("Found OID IPMI Sensor=%s", ipmi_tag);

			/* Insert tag and OID info in temporary hash */
			if (!g_hash_table_lookup_extended(ipmi_sensor_hash,
							  ipmi_tag,
							  (gpointer)&hash_existing_key,
							  (gpointer)&hash_value)) {

				mib_info = g_memdup(&(snmp_bc_ipmi_sensors_temp[i]), sizeof(struct SensorMibInfo));
				if (!mib_info) {
					dbg("Out of memory.");
					g_free(ipmi_tag);
					rtn_code = SA_ERR_HPI_OUT_OF_SPACE;
					goto CLEANUP;
				}
				g_hash_table_insert(ipmi_sensor_hash, ipmi_tag, mib_info);
			}
			else { /* Already exists */
				dbg("IPMI OID ERROR=%s.", snmp_bc_ipmi_sensors_temp[i].oid);
				g_free(ipmi_tag);
			}
		}
	}

	/* FIXME:: Make voltage and temperature search into a single routine */
	/* Find blade's voltage IPMI sensors */
	for (i=0; i<SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS; i++) {
		err = snmp_bc_oid_snmp_get(custom_handle,
					   &(res_oh_event->u.res_event.entry.ResourceEntity),
					   snmp_bc_ipmi_sensors_voltage[i].oid, &get_value, SAHPI_FALSE);
		if (err ||
		    (strncmp(get_value.string, "(No voltage)", sizeof("(No voltage)")) == 0) ||
		    (strncmp(get_value.string, "Not Readable!", sizeof("Not Readable!")) == 0)) {
			break;
		}
		else {
			char *hash_existing_key, *hash_value;
			gchar  **strparts = NULL;
			gchar  *ipmi_tag;
			
			/* Find IPMI tag in returned value */
			strparts = g_strsplit(get_value.string, SNMP_BC_IPMI_STRING_DELIMITER, -1);
			if (strparts == NULL || strparts[0] == '\0') {
				dbg("Cannot split IPMI returned value=%s.", get_value.string);
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

			trace("Found OID IPMI Sensor=%s", ipmi_tag);

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
				dbg("IPMI OID ERROR=%s.", snmp_bc_ipmi_sensors_voltage[i].oid);
				g_free(ipmi_tag);
			}
		}
	}

	/* Iterate thru all the possible IPMI sensors, if it's defined for this blade,
	   push up its RDR info to Infra-structure */
	for (i=0; sensor_array[i].ipmi.index != 0; i++) {
		mib_info = (struct SensorMibInfo *)g_hash_table_lookup(ipmi_sensor_hash, sensor_array[i].ipmi_tag);
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
			
			err = oh_add_rdr(custom_handle->tmpcache,
					 res_oh_event->u.res_event.entry.ResourceId,
					 &(e->u.rdr_event.rdr),
					 sinfo, 0);
			if (err) {
				dbg("Cannot add RDR. Error=%s.", oh_lookup_error(err));
				g_free(e);
			}
			else {
				custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
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
