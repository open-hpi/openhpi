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
 *      W. David Ashley <dashley@us.ibm.com>
 */

#include <glib.h>
#include <SaHpi.h>

#include <snmp_util.h>
#include <uid_utils.h>
#include <epath_utils.h>

#include <openhpi.h>
#include <rsa_resources.h>
#include <snmp_rsa.h>
#include <snmp_rsa_utils.h>
#include <snmp_rsa_discover.h>
#include <string.h>

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


struct oh_event * snmp_rsa_discover_chassis(SaHpiEntityPathT *ep) 
{
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));
         
        working.type = OH_ET_RESOURCE;
        working.u.res_event.entry = snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].rpt;

	working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	len = strlen(snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
		strcpy(working.u.res_event.entry.ResourceTag.Data,
		       snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	} else {
		dbg("Comment string too long - %s\n",snmp_rpt_array[RSA_RPT_ENTRY_CHASSIS].comment);
	}

        working.u.res_event.entry.ResourceId = oh_uid_from_entity_path(ep);
        working.u.res_event.entry.ResourceEntity = *ep;

        e = eventdup(&working);
 
	return e;
}


struct oh_event * snmp_rsa_discover_cpu(SaHpiEntityPathT *ep, int num) 
{
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));
         
        working.type = OH_ET_RESOURCE;
        working.u.res_event.entry = snmp_rpt_array[RSA_RPT_ENTRY_CPU].rpt;

	working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	len = strlen(snmp_rpt_array[RSA_RPT_ENTRY_CPU].comment);
	if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
		strcpy(working.u.res_event.entry.ResourceTag.Data,
		       snmp_rpt_array[RSA_RPT_ENTRY_CPU].comment);
	} else {
		dbg("Comment string too long - %s\n",snmp_rpt_array[RSA_RPT_ENTRY_CPU].comment);
	}

        ep_concat(&working.u.res_event.entry.ResourceEntity, ep);

        set_epath_instance(&(working.u.res_event.entry.ResourceEntity),
                           SAHPI_ENT_PROCESSOR, num+RSA_HPI_INSTANCE_BASE);
        working.u.res_event.entry.ResourceId =
                oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));

        e = eventdup(&working);

//        find_res_events(&working.u.res_event.entry.ResourceEntity, 
//                        &snmp_rpt_array[RSA_RPT_ENTRY_CPU].rsa_res_info);
 
	return e;
}


struct oh_event * snmp_rsa_discover_dasd(SaHpiEntityPathT *ep, int num) 
{
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));
         
        working.type = OH_ET_RESOURCE;
        working.u.res_event.entry = snmp_rpt_array[RSA_RPT_ENTRY_DASD].rpt;

	working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	len = strlen(snmp_rpt_array[RSA_RPT_ENTRY_DASD].comment);
	if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
		strcpy(working.u.res_event.entry.ResourceTag.Data,
		       snmp_rpt_array[RSA_RPT_ENTRY_DASD].comment);
	} else {
		dbg("Comment string too long - %s\n",snmp_rpt_array[RSA_RPT_ENTRY_DASD].comment);
	}


        ep_concat(&working.u.res_event.entry.ResourceEntity, ep);

        set_epath_instance(&(working.u.res_event.entry.ResourceEntity),
                           SAHPI_ENT_DISK_BAY, num+RSA_HPI_INSTANCE_BASE);
        working.u.res_event.entry.ResourceId =
                oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));

        e = eventdup(&working);

//      find_res_events(&working.u.res_event.entry.ResourceEntity, 
//                      &snmp_rpt_array[RSA_RPT_ENTRY_DASD].rsa_res_info);
 
	return e;
}

/**
 * snmp_rsa_discover_sensors: Discover all available sensors for a resource
 * @ss: handle to snmp connection for this instance
 * @parent_ep: Entity path of RDR's parent resource
 * @parent_id: ID of RDR's parent resource
 * @control: Pointer to RDR's static control definition (SaHpiSensorRecT)
 * Return value: Pointer to Plugin Event, if success, NULL, if error or sensor does not exist
 **/
struct oh_event * snmp_rsa_discover_sensors(struct snmp_session *ss,
                                            SaHpiEntityPathT parent_ep,                                           
                                            const struct snmp_rsa_sensor *sensor)
{
	gchar *oid;
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));
	
	oid = snmp_derive_objid(parent_ep, sensor->rsa_sensor_info.mib.oid);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned\n");
		return e;
	}

	if (rdr_exists(ss, oid, sensor->rsa_sensor_info.mib.not_avail_indicator_num,sensor->rsa_sensor_info.mib.write_only)) {
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
		
//		find_sensor_events(&parent_ep, sensor->sensor.Num, sensor);
	}

	g_free(oid);
	return e;
}


struct oh_event * snmp_rsa_discover_fan(SaHpiEntityPathT *ep, int fannum)
{
	int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        working.type = OH_ET_RESOURCE;
        working.u.res_event.entry = snmp_rpt_array[RSA_RPT_ENTRY_FAN].rpt;

	working.u.res_event.entry.ResourceTag.DataType = SAHPI_TL_TYPE_LANGUAGE;
	working.u.res_event.entry.ResourceTag.Language = SAHPI_LANG_ENGLISH;
	len = strlen(snmp_rpt_array[RSA_RPT_ENTRY_FAN].comment);
	if (len <= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		working.u.res_event.entry.ResourceTag.DataLength = (SaHpiUint8T)len;
		strcpy(working.u.res_event.entry.ResourceTag.Data,
		       snmp_rpt_array[RSA_RPT_ENTRY_FAN].comment);
	} else {
		dbg("Comment string too long - %s\n",snmp_rpt_array[RSA_RPT_ENTRY_FAN].comment);
	}

        ep_concat(&working.u.res_event.entry.ResourceEntity, ep);

        set_epath_instance(&(working.u.res_event.entry.ResourceEntity),
                           SAHPI_ENT_FAN,fannum+RSA_HPI_INSTANCE_BASE);
        working.u.res_event.entry.ResourceId =
                oh_uid_from_entity_path(&(working.u.res_event.entry.ResourceEntity));
        e = eventdup(&working);

//	find_res_events(&working.u.res_event.entry.ResourceEntity, 
//			&snmp_rpt_array[RSA_RPT_ENTRY_FAN].rsa_res_info);

	return e;
}


/**
 * snmp_rsa_discover_inventories: Discover all available inventory records for a resource
 * @ss: handle to snmp connection for this instance
 * @parent_ep: Entity path of RDR's parent resource
 * @parent_id: ID of RDR's parent resource
 * @inventory: Pointer to RDR's static inventory definition 
 * Return value: Pointer to Plugin Event, if success, NULL, if error or control does not exist
 **/

struct oh_event * snmp_rsa_discover_inventories(struct snmp_session *ss,
                                            SaHpiEntityPathT parent_ep,
                                            const struct snmp_rsa_inventory *inventory)
{
        gchar *oid;
        int len;
        struct oh_event working;
        struct oh_event *e = NULL;

        memset(&working, 0, sizeof(struct oh_event));

        oid = snmp_derive_objid(parent_ep, inventory->rsa_inventory_info.mib.oid.OidManufacturer);
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
