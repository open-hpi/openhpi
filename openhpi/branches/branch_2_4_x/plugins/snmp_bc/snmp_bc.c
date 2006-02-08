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
 *      Renier Morales <renierm@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

/**
 * snmp_bc_get_event:
 * @hnd: Handler data pointer.
 * @event: Infra-structure event pointer. 
 *
 * Passes plugin events up to the infra-structure for processing.
 *
 * Return values:
 * 1 - events to be processed.
 * SA_OK - No events to be processed.
 * SA_ERR_HPI_INVALID_PARAMS - @event is NULL.
 **/
SaErrorT snmp_bc_get_event(void *hnd, struct oh_event *event)
{
        SaErrorT err;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        
        if (!event || !hnd) {
                dbg("Invalid parameter");
                return(SA_ERR_HPI_INVALID_PARAMS);
        }
	
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
     	snmp_bc_lock_handler(custom_handle);	
	err = snmp_bc_check_selcache(handle, 1, SAHPI_NEWEST_ENTRY);
	
	/* If err is encountered during el cache sync, */
	/* log error but take no corrected action.      */
	/* New entry will still be there next time      */
	if (err) {
		dbg("Event Log cache build/sync failed. Error=%s", oh_lookup_error(err));
		/* return(err); */
	}

        if (g_slist_length(handle->eventq) > 0) {
                memcpy(event, handle->eventq->data, sizeof(*event));
                free(handle->eventq->data);
                handle->eventq = g_slist_remove_link(handle->eventq, handle->eventq);
                snmp_bc_unlock_handler(custom_handle);
                return(1);
        } 

	/* No events for infra-structure to process */
        snmp_bc_unlock_handler(custom_handle);
	return(SA_OK);
}

/**
 * snmp_bc_set_resource_tag:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @tag: Pointer to SaHpiTextBufferT.
 *
 * Sets resource's tag.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @tag is NULL or invalid.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory to allocate event.
 **/
SaErrorT snmp_bc_set_resource_tag(void *hnd, SaHpiResourceIdT rid, SaHpiTextBufferT *tag)
{
	SaErrorT err;
        SaHpiRptEntryT *rpt;
        struct oh_event *e;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;

	if (!oh_valid_textbuffer(tag) || !hnd) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("No RID.");
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

	err = oh_copy_textbuffer(&(rpt->ResourceTag), tag);
	if (err) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("Cannot copy textbuffer");
		return(err);
	}

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}

        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *rpt;
        handle->eventq = g_slist_append(handle->eventq, e);
        snmp_bc_unlock_handler(custom_handle);
        return(SA_OK);
}

/**
 * snmp_bc_set_resource_severity:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @tag: Resource's severity.
 *
 * Sets severity of events when resource unexpectedly becomes unavailable.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @sev is invalid.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory to allocate event.
 **/
SaErrorT snmp_bc_set_resource_severity(void *hnd, SaHpiResourceIdT rid, SaHpiSeverityT sev)
{
        SaHpiRptEntryT *rpt;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct oh_event *e;

	if (oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;
	
	snmp_bc_lock_handler(custom_handle);
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		snmp_bc_unlock_handler(custom_handle);
                dbg("No RID.");
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        rpt->ResourceSeverity = sev;

        /* Add changed resource to event queue */
        e = g_malloc0(sizeof(struct oh_event));
	if (e == NULL) {
		snmp_bc_unlock_handler(custom_handle);
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	
        e->type = OH_ET_RESOURCE;
        e->u.res_event.entry = *rpt;
        handle->eventq = g_slist_append(handle->eventq, e);
	snmp_bc_unlock_handler(custom_handle);

        return(SA_OK);
}

/**
 * snmp_bc_control_parm:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @act: Configuration action.
 *
 * Save and restore saved configuration parameters.
 *
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_INVALID_PARAMS - @act is invalid.
 **/
SaErrorT snmp_bc_control_parm(void *hnd, SaHpiResourceIdT rid, SaHpiParmActionT act)
{

	if (!hnd) return(SA_ERR_HPI_INVALID_PARAMS);	
	if (oh_lookup_parmaction(act) == NULL) {
		dbg("Invalid parameter");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	SaHpiRptEntryT *rpt;
	struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
	struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	snmp_bc_lock_handler(custom_handle);
	rpt = oh_get_resource_by_id(handle->rptcache, rid);
	if (!rpt) {
                dbg("No RID.");
		snmp_bc_unlock_handler(custom_handle);
                return(SA_ERR_HPI_INVALID_RESOURCE);
	}

	if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
		dbg("Resource configuration saving not supported.");
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	else {
		snmp_bc_unlock_handler(custom_handle);
		return(SA_ERR_HPI_CAPABILITY);
	}
}


#define snmp_bc_internal_retry()                   \
	if (l_retry >= 2) {                        \
        	custom_handle->handler_retries++;  \
		err = SA_ERR_HPI_BUSY;             \
		break;                             \
	} else {                                   \
		l_retry++;                         \
		trace("Retrying OID=%s.", objid);  \
		continue;                          \
	}


/**
 * snmp_bc_snmp_get:
 * @custom_handle:  Plugin's data pointer.
 * @objid: SNMP OID.
 * @value: Location to store returned SNMP value.
 * @retry: retry is requested on snmp timeout
 *
 * Plugin wrapper for SNMP get call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_snmp_get(struct snmp_bc_hnd *custom_handle,
                          const char *objid,
			  struct snmp_value *value,
			  SaHpiBoolT retry)
{
        SaErrorT err;
        /* struct snmp_session *ss = custom_handle->ss; */
	int l_retry;
	
	if (retry) l_retry = 0;
	else l_retry = 2;
	
	do {	
        	err = snmp_get(custom_handle->sessp, objid, value);
        	if (err == SA_ERR_HPI_TIMEOUT) {
                	if (custom_handle->handler_retries == SNMP_BC_MAX_SNMP_RETRY_ATTEMPTED) {
                        	custom_handle->handler_retries = 0;
                        	err = SA_ERR_HPI_NO_RESPONSE;
				break;
                	} else {
				trace("HPI_TIMEOUT %s", objid);
				snmp_bc_internal_retry();			
			}
        	} else {
                	custom_handle->handler_retries = 0;
			if ((err == SA_OK) && (value->type == ASN_OCTET_STR)) {
				if ( (strncmp(value->string,"(No temperature)", sizeof("(No temperature)")) == 0) ||
			     		(strncmp(value->string,"(No voltage)", sizeof("(No voltage)")) == 0) )
				{
					snmp_bc_internal_retry();
				} else if (strncmp(value->string,"Not Readable!", sizeof("Not Readable!")) == 0) {
                        		custom_handle->handler_retries = 0;
					trace("Not Readable! reading from OID=%s.", objid);
                        		err = SA_ERR_HPI_NO_RESPONSE;
					break;
				} else {
					break;
				} 
			} else break;
	        }               

	} while(l_retry < 3);
	
        return(err);
}


/**
 * snmp_bc_oid_snmp_get:
 * @custom_handle:  Plugin's data pointer.
 * @ep: Entity path of the resource
 * @oidstr: raw SNMP OID.
 * @value: Location to store returned SNMP value.
 * @retry: Retry requested on snmp timeout
 *
 * Plugin wrapper for SNMP get call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_oid_snmp_get(struct snmp_bc_hnd *custom_handle,
				SaHpiEntityPathT *ep, const gchar *oidstr,
				struct snmp_value *value, SaHpiBoolT retry)
{

	SaErrorT rv = SA_OK;
	gchar *oid;
	
	oid = oh_derive_string(ep, oidstr);
	if (oid == NULL) {
		dbg("Cannot derive %s.", oidstr); 
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	
	rv = snmp_bc_snmp_get(custom_handle, oid, value, retry);
	g_free(oid);
	
	return(rv);
}


/**
 * snmp_bc_snmp_set:
 * @custom_handle:  Plugin's data pointer.
 * @objid: SNMP OID.
 * @value: SNMP value to set.
 *
 * Plugin wrapper for SNMP set call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_snmp_set(struct snmp_bc_hnd *custom_handle,
                          char *objid,
			  struct snmp_value value)
{
        SaErrorT err;
	/* struct snmp_session *ss = custom_handle->ss; */

        err = snmp_set(custom_handle->sessp, objid, value);
        if (err == SA_ERR_HPI_TIMEOUT) {
                if (custom_handle->handler_retries == SNMP_BC_MAX_SNMP_RETRY_ATTEMPTED) {
                        custom_handle->handler_retries = 0;
                        err = SA_ERR_HPI_NO_RESPONSE;
                } 
		else {
                        custom_handle->handler_retries++;
                        err = SA_ERR_HPI_BUSY;
                }
        } 
	else {
                custom_handle->handler_retries = 0;
        }

        return(err);
}


/**
 * snmp_bc_oid_snmp_set:
 * @custom_handle:  Plugin's data pointer.
 * @ep: Entity path of the resource
 * @oidstr: raw SNMP OID.
 * @value: SNMP value to set.
 *
 * Plugin wrapper for SNMP set call. If SNMP command times out,
 * this function returns an SA_ERR_HPI_BUSY until a max number
 * of retries occurs - then it returns SA_ERR_HPI_NO_RESPONSE.
 * BladeCenter hardware often takes several SNMP attempts before
 * it responses. User applications should continue to retry on
 * BUSY and only fail on NO_RESPONSE.
 *
 * Return values:
 * SA_OK - Normal case.
 **/
SaErrorT snmp_bc_oid_snmp_set(struct snmp_bc_hnd *custom_handle,
				SaHpiEntityPathT *ep, const gchar *oidstr,
			  	struct snmp_value value)
{
	SaErrorT rv = SA_OK;
	gchar *oid;

	oid = oh_derive_string(ep , oidstr);
	if (oid == NULL) {
		dbg("NULL SNMP OID returned for %s.", oidstr);
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}
	rv = snmp_bc_snmp_set(custom_handle, oid, value);
	g_free(oid);
	return(rv);
}


void * oh_get_event (void *, struct oh_event *) 
                __attribute__ ((weak, alias("snmp_bc_get_event")));
						
void * oh_set_resource_tag (void *, SaHpiResourceIdT, SaHpiTextBufferT *) 
                __attribute__ ((weak, alias("snmp_bc_set_resource_tag")));
		
void * oh_set_resource_severity (void *, SaHpiResourceIdT, SaHpiSeverityT) 
                __attribute__ ((weak, alias("snmp_bc_set_resource_severity")));
	       
void * oh_control_parm (void *, SaHpiResourceIdT, SaHpiParmActionT)
                __attribute__ ((weak, alias("snmp_bc_control_parm")));

