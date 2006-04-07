/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004, 2006
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 */

#ifndef __SNMP_BC_DISCOVER_BC_H
#define __SNMP_BC_DISCOVER_BC_H

#define SNMP_BC_IPMI_STRING_DELIMITER "="

#define SNMP_BC_MAX_IPMI_TEMP_SENSORS 6
#define SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS 25

#define SNMP_BC_IPMI_TEMP_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.11.x"
#define SNMP_BC_IPMI_VOLTAGE_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.14.x"

#define get_install_mask(maskOID, getvalue) \
do { \
	err = snmp_bc_snmp_get(custom_handle, maskOID, &getvalue, SAHPI_TRUE); \
        if (err || getvalue.type != ASN_OCTET_STR) { \
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.", \
		      		maskOID, getvalue.type, oh_lookup_error(err)); \
		if (err) { return(err); } \
		else { return(SA_ERR_HPI_INTERNAL_ERROR); } \
        } \
} while(0)




#endif


