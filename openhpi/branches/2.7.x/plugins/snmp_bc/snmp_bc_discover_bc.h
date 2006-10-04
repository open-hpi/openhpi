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
 */

#ifndef __SNMP_BC_DISCOVER_BC_H
#define __SNMP_BC_DISCOVER_BC_H

#define SNMP_BC_IPMI_STRING_DELIMITER "="

#define SNMP_BC_MAX_IPMI_TEMP_SENSORS 6
#define SNMP_BC_MAX_IPMI_VOLTAGE_SENSORS 30

#define SNMP_BC_IPMI_TEMP_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.3.1.11.x"
#define SNMP_BC_IPMI_VOLTAGE_BLADE_OID ".1.3.6.1.4.1.2.3.51.2.22.1.5.5.1.14.x"

#define SNMP_BC_RESOURCE_INSTALLED 1
#define SNMP_BC_RESOURCE_REMOVED   2

#define get_installed_mask(maskOID, getvalue) \
do { \
	err = snmp_bc_snmp_get(custom_handle, maskOID, &getvalue, SAHPI_TRUE); \
        if (err || getvalue.type != ASN_OCTET_STR) { \
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.", \
		      		maskOID, getvalue.type, oh_lookup_error(err)); \
		if (err) { return(err); } \
		else { return(SA_ERR_HPI_INTERNAL_ERROR); } \
        } \
} while(0)

#define  get_string_object(maskOID, getvalue)  get_installed_mask(maskOID, getvalue)

#define get_integer_object(maskOID, getintvalue) \
do { \
	err = snmp_bc_snmp_get(custom_handle, maskOID, &getintvalue, SAHPI_TRUE); \
        if (err || getintvalue.type != ASN_INTEGER) { \
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.", \
		      		maskOID, getintvalue.type, oh_lookup_error(err)); \
		if (err) { return(err); } \
		else { return(SA_ERR_HPI_INTERNAL_ERROR); } \
        } \
} while(0)

#define pdp_debug(string) \
do { \
	err = snmp_bc_snmp_get(custom_handle, maskOID, &getintvalue, SAHPI_TRUE); \
        if (err || getintvalue.type != ASN_INTEGER) { \
		dbg("Cannot get OID=%s; Received Type=%d; Error=%s.", \
		      		maskOID, getintvalue.type, oh_lookup_error(err)); \
		if (err) { return(err); } \
		else { return(SA_ERR_HPI_INTERNAL_ERROR); } \
        } \
} while(0)

guint snmp_bc_isrediscover(SaHpiEventT *working_event);

SaErrorT snmp_bc_construct_blade_rpt(struct oh_event *e, 
				     struct ResourceInfo **res_info_ptr,
				     SaHpiEntityPathT *ep_root, 
				     guint blade_index);
				     
SaErrorT snmp_bc_construct_blower_rpt(struct oh_event* e, 
				      struct ResourceInfo **res_info_ptr,
				      SaHpiEntityPathT *ep_root, 
				      guint blower_index);
				      
SaErrorT snmp_bc_construct_pm_rpt(struct oh_event *e, 
				  struct ResourceInfo **res_info_ptr, 
				  SaHpiEntityPathT *ep_root,
				  guint pm_index);
				  
SaErrorT snmp_bc_construct_sm_rpt(struct oh_event *e, 
				  struct ResourceInfo **res_info_ptr,
				  SaHpiEntityPathT *ep_root, 
				  guint sm_index);
				  
SaErrorT snmp_bc_construct_mm_rpt(struct oh_event *e, 
				  struct ResourceInfo **res_info_ptr,
				  SaHpiEntityPathT *ep_root, 
				  guint mm_index);
				  
SaErrorT snmp_bc_add_blade_rptcache(struct oh_handler_state *handle, 
				  struct oh_event *e, 
				  struct ResourceInfo *res_info_ptr,
				  guint blade_index);
				  
SaErrorT snmp_bc_add_blower_rptcache(struct oh_handler_state *handle, 
				  struct oh_event *e, 
				  struct ResourceInfo *res_info_ptr,
				  guint blower_index);
				  
SaErrorT snmp_bc_add_pm_rptcache(struct oh_handler_state *handle, 
				  struct oh_event *e, 
				  struct ResourceInfo *res_info_ptr,
				  guint pm_index);
				  
SaErrorT snmp_bc_add_switch_rptcache(struct oh_handler_state *handle, 
				  struct oh_event *e, 
				  struct ResourceInfo *res_info_ptr,
				  guint switch_index);
				  
SaErrorT snmp_bc_add_mm_rptcache(struct oh_handler_state *handle, 
				  struct oh_event *e, 
				  struct ResourceInfo *res_info_ptr,
				  guint mm_index);
				  
SaErrorT snmp_bc_discover_blade_i(struct oh_handler_state *handle,
			  	  SaHpiEntityPathT *ep_root, 
				  guint blade_index);
				  
SaErrorT snmp_bc_discover_blower_i(struct oh_handler_state *handle,
			  	   SaHpiEntityPathT *ep_root, 
				   guint blower_index);
				   
SaErrorT snmp_bc_discover_pm_i(struct oh_handler_state *handle,
			       SaHpiEntityPathT *ep_root, 
			       guint pm_index);
			       
SaErrorT snmp_bc_discover_switch_i(struct oh_handler_state *handle,
			  	   SaHpiEntityPathT *ep_root, 
				   guint sm_index);
				   
SaErrorT snmp_bc_discover_mm_i(struct oh_handler_state *handle,
			  	SaHpiEntityPathT *ep_root, 
				guint mm_index);

#endif


