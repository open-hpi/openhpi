/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
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
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <glib.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <snmp_util.h>
#include <oh_utils.h>
#include <bc_resources.h>
#include <snmp_bc.h>
#include <snmp_bc_event.h>
#include <snmp_bc_discover.h>

static inline struct oh_event *eventdup(const struct oh_event *event) 
{
	struct oh_event *e;
	e = g_malloc0(sizeof(struct oh_event));
	if (!e) {
		dbg("Out of memory!");
		return NULL;
	}
	memcpy(e, event, sizeof(struct oh_event));
	return e;
}

static inline int rdr_exists(struct snmp_session *ss, const char *oid, unsigned int na, 
			     int write_only)
{
        int err;
	struct snmp_value get_value;

	if (write_only) { return 1; }; /* Can't check it if its non-readable */

        err = snmp_get(ss, oid, &get_value);
        if (err || (get_value.type == ASN_INTEGER && na && na == get_value.integer) ||
                (get_value.type == ASN_OCTET_STR &&
                        (!strcmp(get_value.string,"Not available") ||
                         !strcmp(get_value.string,"Not installed") ||
                         !strcmp(get_value.string,"Not Readable!")))) {
                return 0;
        }

        return 1;
}

/**
 * Discovery of BladeCenter resources is very easy due to the existance
 * of bit vectors in the mibs.  All the discovery functions just look for their
 * enumeration in the bit vector, and figure out if it is true or not.
 *
 * Although this means that we could probably normalize further and reduce
 * the lines of code here, I don't want to do that too early.  It would be a big
 * over optimization.
 **/

struct oh_event * snmp_bc_discover_chassis(struct oh_handler_state *handle, char *blade_vector, SaHpiEntityPathT *ep) 
{
	int len;
        gchar  *instance_str;
	gchar  typename[32];
	struct oh_event working;
        struct oh_event *e = NULL;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

        memset(&working, 0, sizeof(struct oh_event));
         
        if (blade_vector != NULL) {
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		/* Find instance for entity root - defined in config */
		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", ep->Entry[0].EntityLocation);

		/* Find BladeCenter type */
		if (!strcmp(custom_handle->bc_type, SNMP_BC_PLATFORM_BC)) {
			strcpy(typename, " Integrated Chassis ");
		}
		else {
			if (!strcmp(custom_handle->bc_type, SNMP_BC_PLATFORM_BCT)) {
				strcpy(typename, " Telco Chassis ");
			}
			else {
				strcpy(typename, " Chassis ");	
			}
		}
		
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].comment) + 
			strlen(typename) + strlen(instance_str);
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, typename);
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);	
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].comment);
			return e;
		}
		g_free(instance_str);

                working.u.res_event.entry.ResourceId = oh_uid_from_entity_path(ep);
                working.u.res_event.entry.ResourceEntity = *ep;
                e = eventdup(&working);
		
		find_res_events(handle,ep, &snmp_rpt_array[BC_RPT_ENTRY_CHASSIS].bc_res_info);
        }
 
	return e;
}

struct oh_event * snmp_bc_discover_mgmnt(struct oh_handler_state *handle, char *mm_vector, SaHpiEntityPathT *ep, int mmnum)
{
	int len;
	gchar  *instance_str;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if (mm_vector != NULL) {
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
		
		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", mmnum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].comment);
			return e;
		}
		g_free(instance_str);

                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_SYS_MGMNT_MODULE, mmnum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                e = eventdup(&working);

		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_MGMNT_MODULE].bc_res_info);
        }
         
       return e;
}

struct oh_event * snmp_bc_discover_mediatray(struct oh_handler_state *handle, long exists, SaHpiEntityPathT *ep, int mtnum) 
{
	int len;
	gchar  *instance_str;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if (exists) {
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", mtnum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].comment);
			return e;
		}
		g_free(instance_str);

                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_PERIPHERAL_BAY, mtnum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                 e = eventdup(&working);

		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_MEDIA_TRAY].bc_res_info);
        } 
          
	return e;
}

struct oh_event * snmp_bc_discover_blade(struct oh_handler_state *handle, char *blade_vector, SaHpiEntityPathT *ep, int bladenum) 
{
	int len;
        gchar  *instance_str;
	struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if (blade_vector[bladenum] == '1') {
		
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_BLADE].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", bladenum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_BLADE].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_BLADE].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_BLADE].comment);
			return e;
		}
		g_free(instance_str);

                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_SBC_BLADE,bladenum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                e = eventdup(&working);

		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_BLADE].bc_res_info);
        }

	return e;
}

struct oh_event * snmp_bc_discover_blade_addin(struct oh_handler_state *handle,
						char *blade_vector,
                                               	SaHpiEntityPathT *ep, int bladenum)
{
	gchar bladenum_str[2]; /* Implies max number of blades is 99 */
	gchar  *instance_str;
	gchar *oid = NULL; 
	gchar *oidtmp = NULL;
	gchar **oidparts = NULL;
	int len, err;
        struct oh_event working;
        struct oh_event *e = NULL;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;
	struct snmp_value get_value;

        memset(&working, 0, sizeof(struct oh_event));

        if (blade_vector[bladenum] == '1') {
		/* Assumming one 'x' at end of SNMP_BC_BLADE_ADDIN_VECTOR */
		oidtmp = g_strdup(SNMP_BC_BLADE_ADDIN_VECTOR);
                if (!oidtmp) {
			dbg("Out of memory");
			goto CLEANUP;
		}
		oidparts = g_strsplit(oidtmp, OID_BLANK_STR, -1);
                if (!oidparts) {
			dbg("Could not split blade add-in oid");
			goto CLEANUP;
		}
                if (oidparts[1] != NULL && oidparts[1] != '\0') {
			dbg("More than one expansion character in OID string\n");
			goto CLEANUP;
		}
		sprintf(bladenum_str, "%d", bladenum+BC_HPI_INSTANCE_BASE);
		oid = g_strconcat(oidparts[0], bladenum_str, NULL);
                if (!oid) {
			dbg("Could not concatenate blade add-in oid"); 
			goto CLEANUP;
		}
		
		err = snmp_get(ss, oid, &get_value);
		if (!(err || get_value.integer == 0)) {
 			
			working.type = OH_ET_RESOURCE;
			working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].rpt;

			working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
			working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

			instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
			if (instance_str == NULL) {
				dbg("Cannot allocate memory for instance string");
				return e;
			}
			snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", bladenum+BC_HPI_INSTANCE_BASE);
			
			/* Add one for the added blank */
			len = strlen(snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].comment) + strlen(instance_str) + 1;		
			if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
				working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
				strcpy(working.u.res_event.entry.ResourceTag.Data,
				       snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].comment);
				strcat(working.u.res_event.entry.ResourceTag.Data, " ");
				strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
			} else {
				g_free(instance_str);
				dbg("Comment string too long - %s\n",
				    snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].comment);
				return e;
			}
			g_free(instance_str);

                        ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
			set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                           SAHPI_ENT_SBC_BLADE,bladenum+BC_HPI_INSTANCE_BASE);
                        /* Add-in cards have same number as parent blade
			 * This will be a problem if one can have multiple addins/blade.
                         * If this happens, need better SNMP support, since the current
			 * blade based DASD1 temp vector will not normalize properly
                         * if addin card numbers don't agree with parent blade number.
			 */
                        set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                           SAHPI_ENT_ADD_IN_CARD,bladenum+BC_HPI_INSTANCE_BASE);
                        working.u.res_event.entry.ResourceId =
                                oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
			e = eventdup(&working);

			find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
					&snmp_rpt_array[BC_RPT_ENTRY_BLADE_ADDIN_CARD].bc_res_info);
		}		
        }

CLEANUP:
        g_free(oid);
	g_free(oidtmp);
	g_strfreev(oidparts);
        return e;
}

struct oh_event * snmp_bc_discover_fan(struct oh_handler_state *handle,char *fan_vector, SaHpiEntityPathT *ep, int fannum)
{
	int len;
        gchar  *instance_str;
	struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if (fan_vector[fannum] == '1') { 
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", fannum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].comment);
			return e;
		}
		g_free(instance_str);
		
                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_FAN,fannum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                e = eventdup(&working);

		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_BLOWER_MODULE].bc_res_info);
        }

	return e;
}

struct oh_event * snmp_bc_discover_power(struct oh_handler_state *handle,char *power_vector, SaHpiEntityPathT *ep, int powernum)
{
	int len;
        gchar  *instance_str;
	struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if(power_vector[powernum] == '1') { 
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", powernum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str);
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].comment);
			return e;
		}
		g_free(instance_str);

                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_POWER_SUPPLY,powernum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                e = eventdup(&working);
                
		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_POWER_MODULE].bc_res_info);
        }

        return e;
}

struct oh_event * snmp_bc_discover_switch(struct oh_handler_state *handle,char *switch_vector, SaHpiEntityPathT *ep, int switchnum)
{
	int len;
        gchar  *instance_str;
	struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        if(switch_vector[switchnum] == '1') { 
                working.type = OH_ET_RESOURCE;
                working.u.res_event.entry = snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].rpt;

		working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;

		instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
		if (instance_str == NULL) {
			dbg("Cannot allocate memory for instance string");
			return e;
		}
                snprintf(instance_str, MAX_INSTANCE_DIGITS + 1, "%d", switchnum+BC_HPI_INSTANCE_BASE);

		/* Add one for the added blank */
		len = strlen(snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].comment) + strlen(instance_str) + 1;		
		if (len < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.res_event.entry.ResourceTag.Data,
			       snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].comment);
			strcat(working.u.res_event.entry.ResourceTag.Data, " ");
			strcat(working.u.res_event.entry.ResourceTag.Data, instance_str);
		} else {
			g_free(instance_str); 
			dbg("Comment string too long - %s\n",snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].comment);
			return e;
		}
		g_free(instance_str);

                ep_concat(&working.u.res_event.entry.ResourceEntity, ep);
                set_ep_instance(&(working.u.res_event.entry.ResourceEntity),
                                   SAHPI_ENT_INTERCONNECT,switchnum+BC_HPI_INSTANCE_BASE);
                working.u.res_event.entry.ResourceId =
                        oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
                e = eventdup(&working);

 		find_res_events(handle, &working.u.res_event.entry.ResourceEntity, 
				&snmp_rpt_array[BC_RPT_ENTRY_SWITCH_MODULE].bc_res_info);
       }

        return e;        
}

/*******************************
 * Discovery of BladeCenter RDRs
 *******************************/

/**
 * snmp_bc_discover_controls: Discover all available controls for a resource
 * @ss: handle to snmp connection for this instance
 * @parent_ep: Entity path of RDR's parent resource
 * @parent_id: ID of RDR's parent resource
 * @control: Pointer to RDR's static control definition (SaHpiCtrlRecT)
 * Return value: Pointer to Plugin Event, if success, NULL, if error or control does not exist
 **/

struct oh_event * snmp_bc_discover_controls(struct oh_handler_state *handle,
					    SaHpiEntityPathT parent_ep,
					    const struct snmp_bc_control *control)
{
	gchar *oid;
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;

        memset(&working, 0, sizeof(struct oh_event));

	oid = snmp_derive_objid(parent_ep, control->bc_control_info.mib.oid);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned\n");
		return e;
	}

	if (rdr_exists(ss, oid, control->bc_control_info.mib.not_avail_indicator_num,control->bc_control_info.mib.write_only)) {
		working.type = OH_ET_RDR;
		working.u.rdr_event.rdr.RdrType = SAHPI_CTRL_RDR;
		working.u.rdr_event.rdr.Entity = parent_ep;
		working.u.rdr_event.rdr.RdrTypeUnion.CtrlRec = control->control;

		working.u.rdr_event.rdr.IdString.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.rdr_event.rdr.IdString.Language = SAHPI_LANG_ENGLISH;
		len = strlen(control->comment);
		if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.rdr_event.rdr.IdString.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.rdr_event.rdr.IdString.Data,control->comment);
		} else {
			dbg("Comment string too long - %s\n",control->comment);
		}

		e = eventdup(&working);
	}

	g_free(oid);
	return e;
}

/**
 * snmp_bc_discover_sensors: Discover all available sensors for a resource
 * @ss: handle to snmp connection for this instance
 * @parent_ep: Entity path of RDR's parent resource
 * @parent_id: ID of RDR's parent resource
 * @control: Pointer to RDR's static control definition (SaHpiSensorRecT)
 * Return value: Pointer to Plugin Event, if success, NULL, if error or sensor does not exist
 **/
struct oh_event * snmp_bc_discover_sensors(struct oh_handler_state *handle,
                                           SaHpiEntityPathT parent_ep,                                           
                                           const struct snmp_bc_sensor *sensor)
{
	gchar *oid=NULL;
	int len, sensor_event_only=0;
        struct oh_event working;
        struct oh_event *e = NULL;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;

        memset(&working, 0, sizeof(struct oh_event));
	
	/* Check for Event-only sensor - no OID */
	if ((sensor->bc_sensor_info.mib.oid == NULL) &&
	    (sensor->sensor.DataFormat.ReadingFormats & SAHPI_SRF_EVENT_STATE)) {
		sensor_event_only = 1;
	}

	if ((sensor->bc_sensor_info.mib.oid == NULL) &&
	    !(sensor->sensor.DataFormat.ReadingFormats & SAHPI_SRF_EVENT_STATE)) {
	 	dbg("Sensor %s cannot be read and is NOT event-only", sensor->comment);
		return e;
	}

	if (!sensor_event_only) { 
		oid = snmp_derive_objid(parent_ep, sensor->bc_sensor_info.mib.oid);
		if (oid == NULL) {
			dbg("NULL SNMP OID returned\n");
			return e;
		}
	}

	if (sensor_event_only ||
	    rdr_exists(ss, oid, sensor->bc_sensor_info.mib.not_avail_indicator_num,sensor->bc_sensor_info.mib.write_only)) {
		working.type = OH_ET_RDR;
		working.u.rdr_event.rdr.RdrType = SAHPI_SENSOR_RDR;
		working.u.rdr_event.rdr.Entity = parent_ep;
		working.u.rdr_event.rdr.RdrTypeUnion.SensorRec = sensor->sensor;

		working.u.rdr_event.rdr.IdString.DataType = SAHPI_TL_TYPE_LANGUAGE;
		working.u.rdr_event.rdr.IdString.Language = SAHPI_LANG_ENGLISH;
		len = strlen(sensor->comment);
		if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
			working.u.rdr_event.rdr.IdString.DataLength = (SaHpiUint8T)len;
			strcpy(working.u.rdr_event.rdr.IdString.Data,sensor->comment);
		} else {
			dbg("Comment string too long - %s\n",sensor->comment);
		}
		e = eventdup(&working);

		find_sensor_events(handle, &parent_ep, sensor->sensor.Num, sensor);
	}

	g_free(oid);
	return e;
}

/**
 * snmp_bc_discover_inventories: Discover all available inventory records for a resource
 * @ss: handle to snmp connection for this instance
 * @parent_ep: Entity path of RDR's parent resource
 * @parent_id: ID of RDR's parent resource
 * @inventory: Pointer to RDR's static inventory definition 
 * Return value: Pointer to Plugin Event, if success, NULL, if error or control does not exist
 **/

struct oh_event * snmp_bc_discover_inventories(struct oh_handler_state *handle,
                                            SaHpiEntityPathT parent_ep,
                                            const struct snmp_bc_inventory *inventory)
{
        gchar *oid;
        int len;
        struct oh_event working;
        struct oh_event *e = NULL;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;

        memset(&working, 0, sizeof(struct oh_event));

        oid = snmp_derive_objid(parent_ep, inventory->bc_inventory_info.mib.oid.OidManufacturer);
        if (oid == NULL) {
                dbg("NULL SNMP OID returned.\n");
                return e;
        }


        if (rdr_exists(ss, oid, 0, 0)) {
                working.type = OH_ET_RDR;
                working.u.rdr_event.rdr.RdrType = SAHPI_INVENTORY_RDR;
                working.u.rdr_event.rdr.Entity = parent_ep;
                working.u.rdr_event.rdr.RdrTypeUnion.InventoryRec = inventory->inventory;

                working.u.rdr_event.rdr.IdString.DataType = SAHPI_TL_TYPE_LANGUAGE;
                working.u.rdr_event.rdr.IdString.Language = SAHPI_LANG_ENGLISH;
                len = strlen(inventory->comment);
                if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                        working.u.rdr_event.rdr.IdString.DataLength = (SaHpiUint8T)len;
                        strcpy(working.u.rdr_event.rdr.IdString.Data,inventory->comment);
                } else {
                        dbg("Comment string too long - %s\n",inventory->comment);
                }

                e = eventdup(&working);
        }

        g_free(oid);
        return e;
}
