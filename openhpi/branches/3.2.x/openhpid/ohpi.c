/*      -*- linux-c -*-
 *
 * (C) Copright IBM Corp 2004,2006
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renier@openhpi.org>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 */

#include <string.h>

#include <oHpi.h>

#include <config.h>

#include <oh_domain.h>
#include <oh_error.h>
#include <oh_plugin.h>
#include <oh_session.h>
#include <oh_utils.h>
#include <sahpimacros.h>

#include "conf.h"
#include "event.h"
#include "init.h"
#include "lock.h"


/**
 * oHpiVersionGet
 */
SaHpiUint64T oHpiVersionGet()
{
        SaHpiUint64T v = 0;

	OHPI_VERSION_GET(v, VERSION);

        return v;
}

/* Handler operations */

/**
 * oHpiHandlerCreate
 **/
SaErrorT SAHPI_API oHpiHandlerCreate (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    GHashTable *config,
     SAHPI_OUT   oHpiHandlerIdT *id )
{
	SaErrorT error = SA_OK;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
 
        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
        if (!config || !id) 
                return SA_ERR_HPI_INVALID_PARAMS;
        if (g_hash_table_size(config)==0)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */        
        
        if (oh_init()) error = SA_ERR_HPI_INTERNAL_ERROR;

 	if (error == SA_OK)
            error = oh_create_handler(config, id);
 
        oh_release_domain(d); /* Unlock domain */

	return error;
}

/**
 * oHpiHandlerDestroy
 **/
SaErrorT SAHPI_API oHpiHandlerDestroy (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
	SaErrorT error = SA_OK;

        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
        if (id == 0)
                return SA_ERR_HPI_INVALID_PARAMS;
                
        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
                
        if (oh_init()) error = SA_ERR_HPI_INTERNAL_ERROR;

        if (error == SA_OK)
           if (oh_destroy_handler(id)) error = SA_ERR_HPI_ERROR;

        if (error == SA_OK) {
            // Remove all handler remaing resources from the Domain RPT
            SaHpiRptEntryT *rpte;
            SaHpiResourceIdT rid;
            GSList *events = 0;

            rid = SAHPI_FIRST_ENTRY;
            while ((rpte = oh_get_resource_next(&(d->rpt), rid)) != 0) {
                const void * data;
                data = oh_get_resource_data(&(d->rpt), rpte->ResourceId);
                if (data) {
                    const unsigned int hid = *(const unsigned int*)(data);
                    if (hid == id) {
                        struct oh_event * e = g_new0(struct oh_event, 1);
                        e->hid = id;
                        e->resource = *rpte;
                        e->rdrs = 0;
                        e->rdrs_to_remove = 0;
                        e->event.Source = rpte->ResourceId;
                        e->event.EventType = SAHPI_ET_RESOURCE;
                        oh_gettimeofday(&e->event.Timestamp);
                        e->event.Severity = SAHPI_MAJOR;
                        e->event.EventDataUnion.ResourceEvent.ResourceEventType
                            = SAHPI_RESE_RESOURCE_REMOVED;
                        events = g_slist_prepend(events, e);
                    }
                }
                rid = rpte->ResourceId;
            }

            GSList *iter = events;
            while (iter) {
                oh_evt_queue_push(oh_process_q, iter->data);
                iter = g_slist_next(iter);
            }
            g_slist_free(events);
        }
   
        oh_release_domain(d); /* Unlock domain */
        return error;
}

/**
 * oHpiHandlerInfo
 **/
SaErrorT SAHPI_API oHpiHandlerInfo (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_OUT   oHpiHandlerInfoT *info,
     SAHPI_INOUT GHashTable *conf_params )
{
        SaErrorT error = SA_OK;
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;

        if (sid == 0)
               return SA_ERR_HPI_INVALID_SESSION;
        if (id == 0 || !info || !conf_params)
               return SA_ERR_HPI_INVALID_PARAMS;
        if (g_hash_table_size(conf_params)!=0)
               return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        error = oh_get_handler_info(id, info, conf_params);

        oh_release_domain(d); /* Unlock domain */

        return error;
}

/**
 * oHpiHandlerGetNext
 **/
SaErrorT SAHPI_API oHpiHandlerGetNext (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_OUT   oHpiHandlerIdT *next_id )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        SaErrorT error = SA_OK;

        if (sid == 0)
                return SA_ERR_HPI_INVALID_SESSION;
        if (!next_id) 
                return SA_ERR_HPI_INVALID_PARAMS;
        
        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */
 
        if (oh_init()) error = SA_ERR_HPI_INTERNAL_ERROR;

        if (error == SA_OK)
           if (oh_getnext_handler_id(id, next_id)) 
               error = SA_ERR_HPI_NOT_PRESENT;

        oh_release_domain(d); /* Unlock domain */
        return error;
}

/**
 * oHpiHandlerFind
 **/
SaErrorT SAHPI_API oHpiHandlerFind (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    SaHpiResourceIdT rid,
     SAHPI_OUT   oHpiHandlerIdT *id )
{
	SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        unsigned int *hid = NULL;

        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
        if (!id || !rid)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        hid = (unsigned int *)oh_get_resource_data(&d->rpt, rid);

        if (hid == NULL) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        *id = *hid;
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

/**
 * oHpiHandlerRetry
 **/
SaErrorT SAHPI_API oHpiHandlerRetry (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
	struct oh_handler *h = NULL;
	SaErrorT error = SA_OK;

        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
	if (id == 0) return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	h = oh_get_handler(id);
        if (!h) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

	if (h->hnd != NULL) {
                // handler already running
 		oh_release_handler(h);
                oh_release_domain(d); /* Unlock domain */
		return SA_OK;
	}

	h->hnd = h->abi->open(h->config, h->id, oh_process_q);
	if (h->hnd == NULL) error = SA_ERR_HPI_INTERNAL_ERROR;
	else error = SA_OK;

	oh_release_handler(h);
        oh_release_domain(d); /* Unlock domain */

	return error;
}

/* Global parameters */

/**
 * oHpiGlobalParamGet
 **/
SaErrorT SAHPI_API oHpiGlobalParamGet (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_INOUT oHpiGlobalParamT *param )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_global_param p;

        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
        if (!param || !param->Type) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        // TODO fix this ugly cast
        p.type = (oh_global_param_type)param->Type;

        if (oh_get_global_param(&p)) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_UNKNOWN;
        }

        memcpy(&param->u, &p.u, sizeof(oh_global_param_union));

        oh_release_domain(d); /* Unlock domain */
        return SA_OK;
}

/**
 * oHpiGlobalParamSet
 **/
SaErrorT SAHPI_API oHpiGlobalParamSet (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiGlobalParamT *param )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_global_param p;

        if (sid == 0)
		return SA_ERR_HPI_INVALID_SESSION;
        if (!param || !param->Type) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

        if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        // TODO fix this ugly cast
        p.type = (oh_global_param_type)param->Type;
        memcpy(&p.u, &param->u, sizeof(oh_global_param_union));

        if (oh_set_global_param(&p)){
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_ERROR;
        }
 
        oh_release_domain(d); /* Unlock domain */
        return SA_OK;
}

/**
 * oHpiInjectEvent
 **/
SaErrorT SAHPI_API oHpiInjectEvent (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_IN    SaHpiEventT    *event,
     SAHPI_IN    SaHpiRptEntryT *rpte,
     SAHPI_IN    SaHpiRdrT *rdr)
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
	SaErrorT (*inject_event)(void *hnd,
                            	 SaHpiEventT *evt,
                            	 SaHpiRptEntryT *rpte,
                                 SaHpiRdrT *rdr); /* TODO: Allow for an array/list of RDRs */

	struct oh_handler *h = NULL;
	SaErrorT error = SA_OK;

        if (sid == 0){
		return SA_ERR_HPI_INVALID_SESSION;
        }
	if (id == 0) {
		return SA_ERR_HPI_INVALID_PARAMS;
	} 
        if (!event) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}
        if (!rpte) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}
        if (!rdr) {
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        OH_CHECK_INIT_STATE(sid);
        OH_GET_DID(sid, did);
        OH_GET_DOMAIN(did, d); /* Lock domain */

	if (oh_init()) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	h = oh_get_handler(id);
	inject_event = h ? h->abi->inject_event : NULL;
        if (!inject_event) {
                oh_release_handler(h);
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_INVALID_CMD;
        }

	error = inject_event(h->hnd, event, rpte, rdr);

        oh_release_handler(h);
        oh_release_domain(d); /* Unlock domain */
        return error;
}

/**
 * oHpiDomainAdd
 * Currently only available in client library, but not in daemon
 **/
SaErrorT SAHPI_API oHpiDomainAdd (
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port,
     SAHPI_IN    const SaHpiEntityPathT *entity_root,
     SAHPI_OUT   SaHpiDomainIdT *domain_id )
{
    return SA_ERR_HPI_UNSUPPORTED_API;
}


/**
 * oHpiDomainAddById
 * Currently only available in client library, but not in daemon
 **/
SaErrorT SAHPI_API oHpiDomainAddById (
     SAHPI_IN    SaHpiDomainIdT domain_id,
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port,
     SAHPI_IN    const SaHpiEntityPathT *entity_root )
{
    return SA_ERR_HPI_UNSUPPORTED_API;
}


/**
 * oHpiDomainEntryGet
 * Currently only available in client library, but not in daemon
 **/
SaErrorT SAHPI_API oHpiDomainEntryGet (
     SAHPI_IN    SaHpiEntryIdT    EntryId,
     SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
     SAHPI_OUT   oHpiDomainEntryT *DomainEntry )
{
    return SA_ERR_HPI_UNSUPPORTED_API;
}


