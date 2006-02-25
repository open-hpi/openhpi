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
	SaErrorT err, err1;
        SaHpiEntityPathT ep_root;
	SaHpiEntityPathT valEntity;	 
        GSList *res_new,
		 *rdr_new,
		 *res_gone,
		 *rdr_gone;
        GSList *node;
        guint rdr_data_size;
        GSList *tmpnode;
        SaHpiRdrT *rdr;
        SaHpiRptEntryT *res;
        struct oh_event *e;
        gpointer data;

        struct oh_handler_state *handle;		
        struct snmp_bc_hnd *custom_handle;
	
	if (!hnd) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}		
			
	err = SA_OK;
	err1 = SA_OK;
	handle = (struct oh_handler_state *)hnd;		
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
        if (!custom_handle) {
                dbg("Invalid parameter.");
                return(SA_ERR_HPI_INVALID_PARAMS);
        }

        snmp_bc_lock_handler(custom_handle);

	/* Find root Entity Path */
	root_tuple = (char *)g_hash_table_lookup(handle->config, "entity_root");
        if (root_tuple == NULL) {
                dbg("Cannot find configuration parameter.");
                snmp_bc_unlock_handler(custom_handle);
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }
        err = oh_encode_entitypath(root_tuple, &ep_root);
        if (err) {
                dbg("Cannot convert entity path to string. Error=%s.", oh_lookup_error(err));
                snmp_bc_unlock_handler(custom_handle);
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }

	/* Allocate space for temporary RPT cache */
        custom_handle->tmpcache = (RPTable *)g_malloc0(sizeof(RPTable));
        if (custom_handle->tmpcache == NULL) {
                dbg("Out of memory.");
                snmp_bc_unlock_handler(custom_handle);
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
        res_new = NULL;
	rdr_new = NULL;
	res_gone = NULL;
	rdr_gone = NULL;
        node = NULL;
        
       	rpt_diff(handle->rptcache, custom_handle->tmpcache, &res_new, &rdr_new, &res_gone, &rdr_gone);
	trace("%d resources have gone away.", g_slist_length(res_gone));
	trace("%d resources are new or have changed", g_slist_length(res_new));

        for (node = rdr_gone; node != NULL; node = node->next) {
                rdr = (SaHpiRdrT *)node->data;
		snmp_bc_validate_ep(&(rdr->Entity), &valEntity);
                res = oh_get_resource_by_ep(handle->rptcache, &(valEntity));
		
                /* Create remove RDR event and add to event queue */
                e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
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
                res = (SaHpiRptEntryT *)node->data;
		/* Create remove resource event and add to event queue */
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
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
                tmpnode = NULL;
                res = (SaHpiRptEntryT *)node->data;
                if (!res) {
                        dbg("No valid resource at hand. Could not process new resource.");
                        continue;
                }
                data = oh_get_resource_data(custom_handle->tmpcache, res->ResourceId);
		if (data) {
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

                } else {
		       dbg(" NULL data pointer for ResourceID %d \n", res->ResourceId);
                }
        }
        g_slist_free(res_new);
        
        for (node = rdr_new; node != NULL; node = node->next) {
                rdr_data_size = 0;
                tmpnode = NULL;
                rdr = (SaHpiRdrT *)node->data;
		snmp_bc_validate_ep(&(rdr->Entity), &valEntity);
                res = oh_get_resource_by_ep(handle->rptcache, &(valEntity));
                if (!res || !rdr) {
                        dbg("No valid resource or rdr at hand. Could not process new rdr.");
                        continue;
                }
                data = oh_get_rdr_data(custom_handle->tmpcache, res->ResourceId, rdr->RecordId);
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
                            oh_cmp_ep(&(e->u.rdr_event.rdr.Entity),&(rdr->Entity)) &&
                            e->u.rdr_event.rdr.RecordId == rdr->RecordId) {
                                handle->eventq = g_slist_append(handle->eventq, e);
                                custom_handle->tmpqueue = g_slist_remove_link(custom_handle->tmpqueue, tmpnode);
				g_slist_free_1(tmpnode);
                                break;
                        }
                }
        }        
        g_slist_free(rdr_new);


	/* Build cache copy of SEL. RID == 1 (2nd parm) is a dummy id */
	/*
	   This design depends on the BladeCenter management of the Event Log.
	   That is, 
	   	(a) The BC Event Log will always have at least one entry. It *never* has zero entry.
	   	(b) If a Clear Event Log command is received, the BC clears the log, then creates 
		    "Event Log has just been cleared by xxx" entry
	   So, if the cache copy of the Event Log is empty, this is the first invocation of OpenHPI/snmp_bc.
	   Otherwise, only processes newer entries for (re) discovery.
	*/
	if (g_list_length(handle->elcache->elentries) == 0) err1 = snmp_bc_build_selcache(handle, 1);
	else err1 = snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);
	if (err1) {
		/* --------------------------------------------------------------- */
		/* If an error is encounterred during building of snmp_bc elcache, */
		/* only log the error.  Do not do any recovery because log entries */
		/* are still kept in bc mm.  We'll pick them up during synch.      */
		/* --------------------------------------------------------------- */
		trace("snmp_bc_discover, Error %s when building elcache.\n", oh_lookup_error(err1));
	}
	
	if (custom_handle->first_discovery == SAHPI_FALSE)
	                                custom_handle->first_discovery = SAHPI_TRUE;

 CLEANUP:        
        g_slist_free(custom_handle->tmpqueue);
        oh_flush_rpt(custom_handle->tmpcache);  
        g_free(custom_handle->tmpcache);
        snmp_bc_unlock_handler(custom_handle);

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
	struct snmp_bc_hnd *custom_handle;
	struct SensorInfo *sensor_info_ptr;
	
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	for (i=0; sensor_array[i].index != 0; i++) {
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
				valid_sensor = rdr_exists(custom_handle,
							  &(res_oh_event->u.res_event.entry.ResourceEntity),
							  sensor_array[i].sensor_info.mib.oid, 
							  sensor_array[i].sensor_info.mib.not_avail_indicator_num,
							  sensor_array[i].sensor_info.mib.write_only);
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
			err = snmp_bc_mod_sensor_ep(e, sensor_array, i);
			e->u.rdr_event.rdr.RdrTypeUnion.SensorRec = sensor_array[i].sensor;

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), sensor_array[i].comment);

			trace("Discovered sensor: %s.", e->u.rdr_event.rdr.IdString.Data);

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
	SaErrorT err;
	SaHpiBoolT valid_control;
	struct oh_event *e;
	struct snmp_bc_hnd *custom_handle;
	struct ControlInfo *control_info_ptr;
	
	custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	for (i=0; control_array[i].control.Num != 0; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}

		valid_control = rdr_exists(custom_handle,
					   &(res_oh_event->u.res_event.entry.ResourceEntity),
					   control_array[i].control_info.mib.oid,
					   control_array[i].control_info.mib.not_avail_indicator_num,
					   control_array[i].control_info.mib.write_only);

		/* Add control RDR, if control can be read */
		if (valid_control) {
			e->type = OH_ET_RDR;
			e->did = oh_get_default_domain_id();
			e->u.rdr_event.parent = res_oh_event->u.res_event.entry.ResourceId;
			e->u.rdr_event.rdr.RdrType = SAHPI_CTRL_RDR;
			e->u.rdr_event.rdr.Entity = res_oh_event->u.res_event.entry.ResourceEntity;
			e->u.rdr_event.rdr.RdrTypeUnion.CtrlRec = control_array[i].control;

			oh_init_textbuffer(&(e->u.rdr_event.rdr.IdString));
			oh_append_textbuffer(&(e->u.rdr_event.rdr.IdString), control_array[i].comment);

			trace("Discovered control: %s.", e->u.rdr_event.rdr.IdString.Data);

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
	SaHpiBoolT valid_idr;
	SaErrorT err;
	struct oh_event *e;
	struct snmp_bc_hnd *custom_handle;
	struct InventoryInfo *inventory_info_ptr;

	custom_handle = (struct snmp_bc_hnd *)handle->data;

	/* Assumming OidManufacturer is defined and determines readable of other VPD */
	for (i=0; inventory_array[i].inventory_info.mib.oid.OidManufacturer != NULL; i++) {
		e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
		if (e == NULL) {
			dbg("Out of memory.");
			return(SA_ERR_HPI_OUT_OF_SPACE);
		}
		
		valid_idr = rdr_exists(custom_handle,
					 &(res_oh_event->u.res_event.entry.ResourceEntity),
					 inventory_array[i].inventory_info.mib.oid.OidManufacturer,
					 0, 0);

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

			trace("Discovered inventory: %s.", e->u.rdr_event.rdr.IdString.Data);

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
	if (loc != SNMP_BC_NOT_VALID) 
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
SaHpiBoolT rdr_exists(struct snmp_bc_hnd *custom_handle,
			SaHpiEntityPathT *ep,
			const gchar *oidstr,
			unsigned int na,
			SaHpiBoolT write_only)
{
        SaErrorT err;
	struct snmp_value get_value;

	if (write_only == SAHPI_TRUE) { return(SAHPI_FALSE); }; /* Can't check it if its non-readable */

        err = snmp_bc_oid_snmp_get(custom_handle, ep, oidstr, &get_value, SAHPI_TRUE);
        if (err || (get_value.type == ASN_INTEGER && na && na == get_value.integer)) {
		return(SAHPI_FALSE);
        }

        return(SAHPI_TRUE);
}


/**
 * snmp_bc_validate_ep:
 * @org_ep: Pointer to entity path contained within SaHpiRdrT structure.
 * @val_ep: Pointer to returned entity path that has been validated.
 *
 * Remove entity path entries that is not a snmp resource  in entity path structures (SaHpiEntityPathT).
 * The validated entity path is returned in val_ep structure.  
 * This is used for CPU sensors. We wish to have CPU in entity path. But snmp_bc does not have CPU as a 
 * resource.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @org_ep or @val_ep is NULL.
 **/
SaErrorT snmp_bc_validate_ep(SaHpiEntityPathT *org_ep, SaHpiEntityPathT *val_ep)
{
        int i, j;

        if (!org_ep || !val_ep) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	i = 0;
        for (j=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
		if (org_ep->Entry[i].EntityType != SAHPI_ENT_PROCESSOR) {
                	val_ep->Entry[j].EntityLocation = org_ep->Entry[i].EntityLocation;
                	val_ep->Entry[j].EntityType = org_ep->Entry[i].EntityType;
			j++;
		}
                if (org_ep->Entry[i].EntityType == SAHPI_ENT_ROOT) break;
        }
        return(SA_OK);
}


/**
 * snmp_bc_mod_sensor_ep:
 * @e: Pointer to event structure for this sensor.
 * @sensor_array: Pointer to resource's static sensor data array.
 * @index: index in the static sensor data array for the current sensor. 
 *
 * If the sensor being discoverred is belong to blade CPU, then add CPU tuple 
 * to sensor entity path. Else, do nothing.
 * 
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @e or @sensor is NULL.
 **/
SaErrorT snmp_bc_mod_sensor_ep(struct oh_event *e,
				 void *sensor_array_in, 
				 int index)
{

	int j;
	gchar *pch;
	struct snmp_bc_sensor *sensor_array;
	struct snmp_bc_ipmi_sensor *sensor_array_ipmi;
	SaHpiEntityPathT ep_add;
	
	sensor_array = (struct snmp_bc_sensor *)sensor_array_in;
	sensor_array_ipmi = (struct snmp_bc_ipmi_sensor *)sensor_array_in;

        if (!e || !sensor_array) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	ep_add.Entry[0].EntityType = SAHPI_ENT_PROCESSOR;
	ep_add.Entry[0].EntityLocation = 0;
	
	if (((struct snmp_bc_sensor *)sensor_array_in == (struct snmp_bc_sensor *)snmp_bc_blade_sensors)) {
		 
		for (j=0; j < 3; j++) {
			if (sensor_array[index].sensor_info.mib.oid != NULL) {
				if ((strncmp (sensor_array[index].sensor_info.mib.oid, 
					snmp_bc_blade_sensors[j].sensor_info.mib.oid, 34) == 0)) {
					
					ep_add.Entry[0].EntityLocation = j + 1;
					snmp_bc_add_ep(e, &ep_add);
					break;
				} 
			} 
		}
	} else if ((struct snmp_bc_ipmi_sensor *)sensor_array_in == (struct snmp_bc_ipmi_sensor *)snmp_bc_blade_ipmi_sensors) {
 
		if ( (pch = strstr(sensor_array_ipmi[index].ipmi_tag, "CPU")) != NULL) { 
			ep_add.Entry[0].EntityLocation = atoi(&pch[3]);
			snmp_bc_add_ep(e,&ep_add);
		}
	
	} else {
		trace("This not one of the Blade sensor.\n");
	}
	
	return(SA_OK);	  

}


/**
 * snmp_bc_add_ep:
 * @e: Pointer to event structure for this sensor.
 * @ep_add: Pointer to entity path tuple to be prepended.
 *
 * Prepend an entity path tuple to the existing (parent)  
 * entity path in oh_event struct.
 * 
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @e or @ep_add is NULL.
 **/
SaErrorT snmp_bc_add_ep(struct oh_event *e, SaHpiEntityPathT *ep_add)
{

	int i;
	
	if ( !e || !ep_add) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);	
	}
	
	for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
        	ep_add->Entry[i+1].EntityLocation = 
				e->u.rdr_event.rdr.Entity.Entry[i].EntityLocation;
                ep_add->Entry[i+1].EntityType = 
				e->u.rdr_event.rdr.Entity.Entry[i].EntityType;
                if (e->u.rdr_event.rdr.Entity.Entry[i].EntityType == SAHPI_ENT_ROOT) break;
       	}

        for (i=0; i<SAHPI_MAX_ENTITY_PATH; i++) {
                e->u.rdr_event.rdr.Entity.Entry[i].EntityLocation = ep_add->Entry[i].EntityLocation;
                e->u.rdr_event.rdr.Entity.Entry[i].EntityType = ep_add->Entry[i].EntityType;
                if (ep_add->Entry[i].EntityType == SAHPI_ENT_ROOT) break;
       	 }

	return(SA_OK);

}

/**
 * Aliasing  
 **/

void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("snmp_bc_discover_resources")));


