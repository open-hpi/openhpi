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
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <math.h>
#include <snmp_bc_plugin.h>

/**
 * snmp_bc_discover_resources:
 * @hnd: Handler data pointer.
 *
 * Discover all the resources, sensors, controls, etc. for this instance 
 * of the plugin. Found entities are compared with what the HPI 
 * Infra-structure thinks is there and any new, deleted, or changed
 * entities are updated.
 *
 * Return values:
 * Builds/updates internal RPT cache - normal operation.
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
 **/
SaErrorT snmp_bc_discover_resources(void *hnd)
{
        char *root_tuple;
	SaErrorT err = SA_OK;
        SaHpiEntityPathT ep_root;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	/* Find root Entity Path */
	root_tuple = (char *)g_hash_table_lookup(handle->config, "entity_root");
        if (root_tuple == NULL) {
                dbg("Cannot find configuration parameter.");
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }
        err = string2entitypath(root_tuple, &ep_root);
        if (err) {
                dbg("Cannot convert entity path to string. Error=%s.", oh_lookup_error(err));
                return(err);
        }

	/* Allocate space for temporary RPT cache */
	custom_handle->tmpcache = (RPTable *)g_malloc0(sizeof(RPTable));
	if (custom_handle->tmpcache == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

	/* Initialize tmpqueue */
	custom_handle->tmpqueue = NULL;

	/* Individual platform discovery */
	if (custom_handle->platform == SNMP_BC_PLATFORM_RSA) {
		err = snmp_bc_discover_rsa(handle, &ep_root);
	}
	else {
		err = snmp_bc_discover(handle, &ep_root);
	}
 	if (err) {
		dbg("Discovery failed. Error=%s.", oh_lookup_error(err));
		goto CLEANUP;
	}

	/**********************************************************************
	 * Rediscovery:
	 * Get difference between current rptcache and custom_handle->tmpcache.
	 * Delete obsolete items from rptcache and add new items in.
	 **********************************************************************/
        GSList *res_new = NULL, *rdr_new = NULL, *res_gone = NULL, *rdr_gone = NULL;
        GSList *node = NULL;
        
       	rpt_diff(handle->rptcache, custom_handle->tmpcache, &res_new, &rdr_new, &res_gone, &rdr_gone);
	trace("%d resources have gone away.", g_slist_length(res_gone));
	trace("%d resources are new or have changed", g_slist_length(res_new));

        for (node = rdr_gone; node != NULL; node = node->next) {
                SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                SaHpiRptEntryT *res = oh_get_resource_by_ep(handle->rptcache, &(rdr->Entity));
                /* Create remove RDR event and add to event queue */
                struct oh_event *e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                if (e) {
			e->did = oh_get_default_domain_id();
                        e->type = OH_ET_RDR_DEL;
                        e->u.rdr_event.parent = res->ResourceId;			
			memcpy(&(e->u.rdr_event.rdr), rdr, sizeof(SaHpiRdrT));
                        handle->eventq = g_slist_append(handle->eventq, e);
                } 
		else { dbg("Out of memory."); }
                /* Remove RDR from plugin's RPT cache */
                if (rdr && res)
                        oh_remove_rdr(handle->rptcache, res->ResourceId, rdr->RecordId);
                else { dbg("No valid resource or rdr at hand. Could not remove rdr."); }
        }

        g_slist_free(rdr_gone);

        for (node = res_gone; node != NULL; node = node->next) {
                SaHpiRptEntryT *res = (SaHpiRptEntryT *)node->data;
		/* Create remove resource event and add to event queue */
		struct oh_event *e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
                if (e) {
			e->did = oh_get_default_domain_id();
                        e->type = OH_ET_RESOURCE_DEL;

                        e->u.res_event.entry.ResourceId = res->ResourceId;
                        handle->eventq = g_slist_append(handle->eventq, e);
                } else { dbg("Out of memory."); }
		/* Remove resource from plugin's RPT cache */
                if (res)
                        oh_remove_resource(handle->rptcache, res->ResourceId);
                else dbg("No valid resource at hand. Could not remove resource.");
        }

        g_slist_free(res_gone);

        for (node = res_new; node != NULL; node = node->next) {
                GSList *tmpnode = NULL;
                SaHpiRptEntryT *res = (SaHpiRptEntryT *)node->data;
                if (!res) {
                        dbg("No valid resource at hand. Could not process new resource.");
                        continue;
                }
                gpointer data = oh_get_resource_data(custom_handle->tmpcache, res->ResourceId);
                oh_add_resource(handle->rptcache, res, g_memdup(data, sizeof(struct snmp_rpt)),0);
                /* Add new/changed resources to the event queue */
                for (tmpnode = custom_handle->tmpqueue; tmpnode != NULL; tmpnode = tmpnode->next) {
                        struct oh_event *e = (struct oh_event *)tmpnode->data;
                        if (e->type == OH_ET_RESOURCE &&
                            e->u.res_event.entry.ResourceId == res->ResourceId) {
                                handle->eventq = g_slist_append(handle->eventq, e);
                                custom_handle->tmpqueue = g_slist_remove_link(custom_handle->tmpqueue, tmpnode);
				g_slist_free_1(tmpnode);
                                break;
                        }
                }
        }
        g_slist_free(res_new);
        
        for (node = rdr_new; node != NULL; node = node->next) {
                guint rdr_data_size = 0;
                GSList *tmpnode = NULL;
                SaHpiRdrT *rdr = (SaHpiRdrT *)node->data;
                SaHpiRptEntryT *res = oh_get_resource_by_ep(handle->rptcache, &(rdr->Entity));
                if (!res || !rdr) {
                        dbg("No valid resource or rdr at hand. Could not process new rdr.");
                        continue;
                }
                gpointer data = oh_get_rdr_data(custom_handle->tmpcache, res->ResourceId, rdr->RecordId);
                /* Need to figure out the size of the data associated with the rdr */
                if (rdr->RdrType == SAHPI_SENSOR_RDR) rdr_data_size = sizeof(struct SensorInfo);
                else if (rdr->RdrType == SAHPI_CTRL_RDR)
                        rdr_data_size = sizeof(struct ControlInfo);
                else if (rdr->RdrType == SAHPI_INVENTORY_RDR)
                        rdr_data_size = sizeof(struct InventoryInfo);
                oh_add_rdr(handle->rptcache, res->ResourceId, rdr, g_memdup(data, rdr_data_size),0);
                /* Add new/changed rdrs to the event queue */
                for (tmpnode = custom_handle->tmpqueue; tmpnode != NULL; tmpnode = tmpnode->next) {
                        struct oh_event *e = (struct oh_event *)tmpnode->data;
                        if (e->type == OH_ET_RDR &&
                            ep_cmp(&(e->u.rdr_event.rdr.Entity),&(rdr->Entity)) == 0 &&
                            e->u.rdr_event.rdr.RecordId == rdr->RecordId) {
                                handle->eventq = g_slist_append(handle->eventq, e);
                                custom_handle->tmpqueue = g_slist_remove_link(custom_handle->tmpqueue, tmpnode);
				g_slist_free_1(tmpnode);
                                break;
                        }
                }
        }        
        g_slist_free(rdr_new);

	/* FIXME:: RSA doesn't have??? */
	/* Build cache copy of SEL */
	snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);

 CLEANUP:        
        g_slist_free(custom_handle->tmpqueue);
        oh_flush_rpt(custom_handle->tmpcache);  
	g_free(custom_handle->tmpcache);

        return(err);
}

/**
 * snmp_bc_discover_sensors: 
 * @handler: Pointer to handler's data.
 * @sensor_array: Pointer to resource's static sensor data array.
 * @parent_res_event: Pointer to resource's event structure.
 *
 * Discovers resource's available sensors and their events.
 *
 * Return values:
 * Adds sensor RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
  **/
SaErrorT snmp_bc_discover_sensors(struct oh_handler_state *handle,
				  struct snmp_bc_sensor *sensor_array,
				  struct oh_event *res_oh_event)
{
	int i;
	SaErrorT err;
	SaHpiBoolT valid_sensor;
	struct oh_event *e;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;
	struct SensorInfo *sensor_info_ptr;
	
	for (i=0; sensor_array[i].sensor.Num != 0; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		valid_sensor = SAHPI_FALSE;
		/* Check for event-only sensor */
		if (sensor_array[i].sensor.DataFormat.IsSupported == SAHPI_FALSE) {
			valid_sensor = SAHPI_TRUE;
		}
		else {
			if (sensor_array[i].sensor_info.mib.oid != NULL) {
				gchar *oid;

				oid = snmp_derive_objid(&(res_oh_event->u.res_event.entry.ResourceEntity),
							sensor_array[i].sensor_info.mib.oid);
				if (oid == NULL) {
					dbg("Cannot derive %s.", sensor_array[i].sensor_info.mib.oid);
					g_free(e);
					return(SA_ERR_HPI_INTERNAL_ERROR);
				}
				valid_sensor = rdr_exists(ss, oid, 
							  sensor_array[i].sensor_info.mib.not_avail_indicator_num,
							  sensor_array[i].sensor_info.mib.write_only);
				g_free(oid);
			}
			else {
				dbg("Sensor %s cannot be read.", sensor_array[i].comment);
				g_free(e);
				return(SA_ERR_HPI_INTERNAL_ERROR);
			}
		}

		/* Add sensor RDR, if sensor is event-only or can be read */
		if (valid_sensor) {
			e->type = OH_ET_RDR;
			e->did = oh_get_default_domain_id();
			e->u.rdr_event.parent = res_oh_event->u.res_event.entry.ResourceId;
			e->u.rdr_event.rdr.RdrType = SAHPI_SENSOR_RDR;
			e->u.rdr_event.rdr.Entity = res_oh_event->u.res_event.entry.ResourceEntity;
			e->u.rdr_event.rdr.RdrTypeUnion.SensorRec = sensor_array[i].sensor;

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), sensor_array[i].comment);
#if 0
			trace("Discovered sensor: %s.", e->u.rdr_event.rdr.IdString.Data);
#endif
			sensor_info_ptr = g_memdup(&(sensor_array[i].sensor_info), sizeof(struct SensorInfo));

			err = oh_add_rdr(custom_handle->tmpcache,
					 res_oh_event->u.res_event.entry.ResourceId,
					 &(e->u.rdr_event.rdr),
					 sensor_info_ptr, 0);
			if (err) {
				dbg("Cannot add RDR. Error=%s.", oh_lookup_error(err));
				g_free(e);
			}
			else {
				custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
				snmp_bc_discover_sensor_events(handle,
							       &(res_oh_event->u.res_event.entry.ResourceEntity),
							       sensor_array[i].sensor.Num,
							       &(sensor_array[i]));
			}
		}
		else {
			g_free(e);
		}
	}
	
	return(SA_OK);
}

/**
 * snmp_bc_discover_controls: 
 * @handler: Pointer to handler's data.
 * @control_array: Pointer to resource's static control data array.
 * @res_oh_event: Pointer to resource's event structure.
 *
 * Discovers resource's available controls.
 *
 * Return values:
 * Adds control RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
  **/
SaErrorT snmp_bc_discover_controls(struct oh_handler_state *handle,
				   struct snmp_bc_control *control_array,
				   struct oh_event *res_oh_event)
{
	int i;
	gchar *oid;
	SaErrorT err;
	SaHpiBoolT valid_control;
	struct oh_event *e;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;
	struct ControlInfo *control_info_ptr;
	
	for (i=0; control_array[i].control.Num != 0; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		oid = snmp_derive_objid(&(res_oh_event->u.res_event.entry.ResourceEntity),
					control_array[i].control_info.mib.oid);
		if (oid == NULL) {
			dbg("Cannot derive %s.", control_array[i].control_info.mib.oid);
			g_free(e);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

		valid_control = rdr_exists(ss, oid, 
					   control_array[i].control_info.mib.not_avail_indicator_num,
					   control_array[i].control_info.mib.write_only);
		g_free(oid);

		/* Add control RDR, if control can be read */
		if (valid_control) {
			e->type = OH_ET_RDR;
			e->did = oh_get_default_domain_id();
			e->u.rdr_event.rdr.RdrType = SAHPI_CTRL_RDR;
			e->u.rdr_event.rdr.Entity = res_oh_event->u.res_event.entry.ResourceEntity;
			e->u.rdr_event.rdr.RdrTypeUnion.CtrlRec = control_array[i].control;

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), control_array[i].comment);
#if 0
			trace("Discovered control: %s.", e->u.rdr_event.rdr.IdString.Data);
#endif
			control_info_ptr = g_memdup(&(control_array[i].control_info), sizeof(struct ControlInfo));
			err = oh_add_rdr(custom_handle->tmpcache,
					 res_oh_event->u.res_event.entry.ResourceId,
					 &(e->u.rdr_event.rdr),
					 control_info_ptr, 0);
			if (err) {
				dbg("Cannot add RDR. Error=%s.", oh_lookup_error(err));
				g_free(e);
			}
			else {
				custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			}
		}
		else {
			g_free(e);
		}
	}
	
	return(SA_OK);
}

/**
 * snmp_bc_discover_inventory: 
 * @handler: Pointer to handler's data.
 * @inventory_array: Pointer to resource's static inventory data array.
 * @res_oh_event: Pointer to resource's event structure.
 *
 * Discovers resource's available inventory data records.
 *
 * Return values:
 * Adds inventory RDRs to internal Infra-structure queues - normal case
 * SA_ERR_HPI_OUT_OF_SPACE - Cannot allocate space for internal memory
  **/
SaErrorT snmp_bc_discover_inventories(struct oh_handler_state *handle,
				      struct snmp_bc_inventory *inventory_array,
				      struct oh_event *res_oh_event)
{
	int i;
	gchar *oid;
	SaHpiBoolT valid_idr;
	SaErrorT err;
	struct oh_event *e;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	struct snmp_session *ss = custom_handle->ss;
	struct InventoryInfo *inventory_info_ptr;

	/* Assumming OidManufacturer is defined and determines readable of other VPD */
	for (i=0; inventory_array[i].inventory_info.mib.oid.OidManufacturer != NULL; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		oid = snmp_derive_objid(&(res_oh_event->u.res_event.entry.ResourceEntity),
					inventory_array[i].inventory_info.mib.oid.OidManufacturer);
		if (oid == NULL) {
			dbg("Cannot derive %s.", 
			      inventory_array[i].inventory_info.mib.oid.OidManufacturer);
			g_free(e);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}
		
		valid_idr = rdr_exists(ss, oid, 0, 0);
		g_free(oid);

		/* Add inventory RDR, if inventory can be read */
		if (valid_idr) {
			e->type = OH_ET_RDR;
			e->did = oh_get_default_domain_id();
			e->u.rdr_event.parent = res_oh_event->u.res_event.entry.ResourceId;
			e->u.rdr_event.rdr.RdrType = SAHPI_INVENTORY_RDR;
			e->u.rdr_event.rdr.Entity = res_oh_event->u.res_event.entry.ResourceEntity;
			e->u.rdr_event.rdr.RdrTypeUnion.InventoryRec = inventory_array[i].inventory;

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), inventory_array[i].comment);
#if 0
			trace("Discovered inventory: %s.", e->u.rdr_event.rdr.IdString.Data);
#endif
			inventory_info_ptr = g_memdup(&(inventory_array[i].inventory_info), sizeof(struct InventoryInfo));
			err = oh_add_rdr(custom_handle->tmpcache,
					 res_oh_event->u.res_event.entry.ResourceId,
					 &(e->u.rdr_event.rdr),
					 inventory_info_ptr, 0);
			if (err) {
				dbg("Cannot add RDR. Error=%s.", oh_lookup_error(err));
				g_free(e);
			}
			else {
				custom_handle->tmpqueue = g_slist_append(custom_handle->tmpqueue, e);
			}
		}
		else {
			g_free(e);
		}
	}
	
	return(SA_OK);
}

/**
 * snmp_bc_create_resourcetag:
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
SaErrorT snmp_bc_create_resourcetag(SaHpiTextBufferT *buffer, const char *str, SaHpiEntityLocationT loc)
{
	char *locstr;
	SaErrorT err = SA_OK;
	SaHpiTextBufferT working;

	if (!buffer || loc < SNMP_BC_HPI_LOCATION_BASE ||
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

/**
 * rdr_exists:
 * @ss: Pointer to SNMP session.
 * @oid: SNMP OID string
 * @na: Not available integer, if applicable
 * @write-only: SNMP OID write-only indicator
 *
 * Determines if an SNMP OID is available to be read. OID may not exist, or if
 * it does exist may be write-only or indicate that it's value is non-existant.
 * 
 * Return values: 
 * SAHPI_TRUE - if OID is valid and readable.
 * SAHPI_FALSE - if OID's value cannot be read.
 **/
SaHpiBoolT rdr_exists(struct snmp_session *ss,
		      const char *oid,
		      unsigned int na,
		      SaHpiBoolT write_only)
{
        SaErrorT err;
	struct snmp_value get_value;

	if (write_only == SAHPI_TRUE) { return(SAHPI_FALSE); }; /* Can't check it if its non-readable */

        err = snmp_get(ss, oid, &get_value);
        if (err || (get_value.type == ASN_INTEGER && na && na == get_value.integer) ||
                (get_value.type == ASN_OCTET_STR &&
                        (!strcmp(get_value.string,"Not available") ||
                         !strcmp(get_value.string,"Not installed") ||
                         !strcmp(get_value.string,"Not Readable!")))) {
                return(SAHPI_FALSE);
        }

        return(SAHPI_TRUE);
}
