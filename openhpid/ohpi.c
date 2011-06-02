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
 *
 * Returns the version of the library as a SaHpiUint64T type.
 * The version consists of 3 16-bit integers: MAJOR, MINOR, and PATCH.
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
 * @sid:    IN.     a valid session id
 * @config: IN.     Hash table. Holds configuration information used by handler.
 * @id:     IN/OUT. The id of the newly created handler is returned here.
 *
 * Creates a new handler (instance of a plugin). Plugin handlers are what
 * respond to most API calls.
 * @config needs to have an entry for "plugin" in order to know for which
 * plugin the handler is being created.
 *
 * Returns: SA_OK on success. SA_ERR_HPI_INTERNAL_ERROR if a handler is
 * created, but failed to open. oHpiHandlerRetry can be used to retry
 * opening the handler.
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
 * @sid:    IN.     a valid session id
 * @id:     IN.     The id of the handler to destroy
 *
 * Destroys a handler. Calls the plugin's abi close function.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
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
 * @sid:    IN.     a valid session id
 * @id:     IN.     The id of the handler to query
 * @info:   IN/OUT. Pointer to struct for holding handler information
 * @conf_params: IN/OUT Pointer to pre-allocated hash-table, for holding the
 *               handler's configuration parameters
 *
 * Queries a handler for the information associated with it.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
// function to copy hash table
static void copy_hashed_config_info (gpointer key, gpointer value, gpointer newhash)
{
   g_hash_table_insert ( newhash, g_strdup(key), g_strdup(value) );
}  
SaErrorT SAHPI_API oHpiHandlerInfo (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_OUT   oHpiHandlerInfoT *info,
     SAHPI_INOUT GHashTable *conf_params )
{
        SaHpiDomainIdT did;
        struct oh_domain *d = NULL;
        struct oh_handler *h = NULL;

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

        h = oh_get_handler(id);
        if (!h) {
                oh_release_domain(d); /* Unlock domain */
                return SA_ERR_HPI_NOT_PRESENT;
        }

	info->id = id;
        strncpy(info->plugin_name, h->plugin_name, MAX_PLUGIN_NAME_LENGTH);
	oh_encode_entitypath((const char *)g_hash_table_lookup(h->config, "entity_root"),
			     &info->entity_root);
	
	if (!h->hnd) info->load_failed = 1;
	else info->load_failed = 0;

        // copy h->config to the output hash table
        g_hash_table_foreach(h->config,copy_hashed_config_info,conf_params);

        //Don't transmit passwords in case the handler has a password in its config
        if (g_hash_table_lookup(conf_params,"password"))
           g_hash_table_replace(conf_params,
                                g_strdup("password"), g_strdup("********"));

        oh_release_handler(h);
        oh_release_domain(d); /* Unlock domain */

        return SA_OK;
}

/**
 * oHpiHandlerGetNext
 * @sid:     IN.     a valid session id
 * @id:      IN.     Id of handler to search for.
 * @next_id: OUT.    The id of the handler next to the handler being searched for
 *                   will be returned here.
 *
 * Used for iterating through all loaded handlers. If you pass
 * 0 (SAHPI_FIRST_ENTRY), you will get the id of the first handler returned
 * in next_id.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
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
 * @sid:    IN.     a valid session id
 * @rid:    IN.     resource id
 * @id:     OUT     pointer where handler id found will be placed.
 *
 * Inputs are the @sid and @rid. @rid corresponds to some resource available
 * in that session. The function then will return the handler that served such
 * resource.
 *
 * Returns: SA_OK if handler was found.
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
 * @sid:    IN.     a valid session id
 * @id:     IN      handler id
 *
 * Returns: SA_OK if handler opens successfully.
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
 * @sid:    IN.     a valid session id
 * @param:  IN/OUT  param->type needs to be set to know what parameter to fetch.
 *
 * Gets the value of the specified global parameter.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
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

        p.type = param->Type;

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
 * @sid:    IN.     a valid session id
 * @param:  IN.     param->type needs to be set to know what parameter to set.
 *                  Also, the appropiate value in param->u needs to be filled in.
 *
 * Sets a global parameter.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
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

        p.type = param->Type;
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
 * @sid:    IN.     a valid session id
 * @id:     IN.     id of handler into which the event will be injected.
 * @event:  IN.     pointer to the event to be injected.
 * @rpte:   IN.     pointer to the resource to be injected.
 * @rdrs:   IN.     pointer to the list of RDRs to be injected along with @resoruce
 *
 * @id and @event are required parameters. @rpte is only required if the event
 * is of RESOURCE type or HOTSWAP type. @rdrs is an optional argument in all
 * cases and can be NULL. If @rdrs is passed, it will be copied. It is the
 * responsibility of the caller to clean up the RDRs list once it is used here.
 *
 * Returns: SA_OK on success. This call will set the event.Source, rpte.ResourceId,
 * rpte.ResourceEntity so that the caller knows what the final assigned values were.
 * For rpte.ResourceEntity, the entity_root configuration parameter for the plugin
 * is used to complete it. In addition, for each rdr in @rdrs, a Num, RecordId,
 * and Entity will be assigned. This will also be reflected in the passed @rdrs
 * list so that the caller can know what the assigned values were.
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
 * @host: host name of domain config item
 * @port: port for domain config item
 * @entity_root: entity path to be added to all resources/rdrs
 * @domain_id: domain_id for newly created domain config item
 *
 * Returns: SA_OK on success and fills domain_id.
 *
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
 * @host: host name of domain config item
 * @port: port for domain config item
 * @entity_root: entity path to be added to all resources/rdrs
 * @domain_id: domain_id for newly created domain config item
 *
 * Returns: SA_OK on success and fills domain_id.
 *
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
 * @EntryId:     id of an entry of the library's domain table
 *               or SAHPI_FIRST_ENTRY
 * @NextEntryId: id of the next entry of the library's domain table
 *               or SAHPI_LAST_ENTRY
 * @DomainEntry: configuration info of the domain listed in
 *               the entry identified by NextEntryId
 *
 * Returns: SA_OK on success and fills DomainEntry
 *
 * Currently only available in client library, but not in daemon
 **/
SaErrorT SAHPI_API oHpiDomainEntryGet (
     SAHPI_IN    SaHpiEntryIdT    EntryId,
     SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
     SAHPI_OUT   oHpiDomainEntryT *DomainEntry )
{
    return SA_ERR_HPI_UNSUPPORTED_API;
}


