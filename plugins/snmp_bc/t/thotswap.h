/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef THOTSWAP_H
#define THOTSWAP_H

gboolean  ifobj_force_error = 0;
gboolean  ifobj_data_force_error = 0;
gboolean  snmp_force_error = 0;

gboolean  snmp_get_string_type  = 0; /* 1 implies ASN_OCTET_STR; 0 implies ASN_INTEGER */
gchar     *snmp_value_string =  "";
long      snmp_value_integer = 255;

struct snmp_rpt test_rpt = {
	.rpt = {
		.ResourceInfo = {
			.ManufacturerId = 2,
		},
		.ResourceEntity = {
			.Entry[0] = 
			{
				.EntityType = SAHPI_ENT_SBC_BLADE,
				.EntityInstance = 1,
			},
			{
				.EntityType = SAHPI_ENT_SUB_CHASSIS,
				.EntityInstance = 1,
			},
			{
				.EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
				.EntityInstance = 1,
			},			
		},
		.ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SEL |
		                        SAHPI_CAPABILITY_SENSOR,
		.ResourceSeverity = SAHPI_CRITICAL,
	},
	.bc_res_info = {
		.mib = {
			.OidHealth = ".1.3.6.1.4.1.2.3.51.2.2.7.1.0",
			.HealthyValue = 255,
			.OidReset = ".1.3.6.1.4.1.2.3.51.2.22.3.1.1.1.8.x",
			.OidPowerState = ".1.3.6.1.4.1.2.3.51.2.22.3.1.1.1.7.x",
			.OidPowerOnOff = ".1.3.6.1.4.1.2.3.51.2.22.3.1.1.1.7.x",
		},
	},
	.comment = "Top-level blade chassis"
};

#endif
