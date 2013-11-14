/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
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
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *      Renier Morales <renier@openhpi.org>
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#include <stddef.h>
#include <string.h>

#include <glib.h>

#include <SaHpi.h>
#include <oHpi.h>
#include <config.h>
#include <marshal_hpi.h>
#include <oh_rpc_params.h>
#include <oh_utils.h>

#include "conf.h"
#include "init.h"
#include "session.h"


/*----------------------------------------------------------------------------*/
/* Utility functions                                                          */
/*----------------------------------------------------------------------------*/

static void __dehash_config(gpointer key, gpointer value, gpointer data)
{
    oHpiHandlerConfigT *hc = (oHpiHandlerConfigT *)data;

    strncpy((char *)hc->Params[hc->NumberOfParams].Name,
            (const char *)key,
            SAHPI_MAX_TEXT_BUFFER_LENGTH);
    strncpy((char *)hc->Params[hc->NumberOfParams].Value,
            (const char *)value,
            SAHPI_MAX_TEXT_BUFFER_LENGTH);

    ++hc->NumberOfParams;
}


/*----------------------------------------------------------------------------*/
/* oHpiVersionGet                                                             */
/*----------------------------------------------------------------------------*/

SaHpiUint64T SAHPI_API oHpiVersionGet(void)
{
    SaHpiUint64T v = 0;

    OHPI_VERSION_GET(v, VERSION);

    return v;
}

/*----------------------------------------------------------------------------*/
/* oHpiHandlerCreate                                                          */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiHandlerCreate (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    GHashTable *config,
    SAHPI_OUT   oHpiHandlerIdT *id)
{
    SaErrorT rv;
    oHpiHandlerConfigT handler_config;

    if (!config || !id) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (g_hash_table_size(config) == 0) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    handler_config.Params = g_new0(oHpiHandlerConfigParamT, g_hash_table_size(config));
    handler_config.NumberOfParams = 0;
    // add each hash table entry to the marshable handler_config
    g_hash_table_foreach(config, __dehash_config, &handler_config);

    // now create the handler
    ClientRpcParams iparams(&handler_config);
    ClientRpcParams oparams(id);
    rv = ohc_sess_rpc(eFoHpiHandlerCreate, sid, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerDestroy                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiHandlerDestroy (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiHandlerIdT id)
{
    SaErrorT rv;

    if (id == 0) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&id);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFoHpiHandlerDestroy, sid, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerInfo                                                            */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiHandlerInfo (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiHandlerIdT id,
    SAHPI_OUT   oHpiHandlerInfoT *info,
    SAHPI_INOUT GHashTable *conf_params)
{
    SaErrorT rv;
    oHpiHandlerConfigT config;

    if (id == 0 || !info || !conf_params) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (g_hash_table_size(conf_params) != 0) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&id);
    ClientRpcParams oparams(info, &config);
    rv = ohc_sess_rpc(eFoHpiHandlerInfo, sid, iparams, oparams);

    for (unsigned int n = 0; n < config.NumberOfParams; n++) {
        g_hash_table_insert(conf_params,
                            g_strdup((const gchar *)config.Params[n].Name),
                            g_strdup((const gchar *)config.Params[n].Value));
    }
    g_free(config.Params);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiHandlerGetNext                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiHandlerGetNext (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiHandlerIdT id,
    SAHPI_OUT   oHpiHandlerIdT *next_id)
{
    SaErrorT rv;

    if (!next_id) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&id);
    ClientRpcParams oparams(next_id);
    rv = ohc_sess_rpc(eFoHpiHandlerGetNext, sid, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* oHpiHandlerFind                                                            */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiHandlerFind (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    SaHpiResourceIdT rid,
    SAHPI_OUT   oHpiHandlerIdT *id)
{
    SaErrorT rv;

    if (!id || !rid) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    *id = 0; //Initialize output var

    ClientRpcParams iparams(&sid, &rid);
    ClientRpcParams oparams(id);
    rv = ohc_sess_rpc(eFoHpiHandlerFind, sid, iparams, oparams);

    return rv;
}

/*----------------------------------------------------------------------------*/
/* oHpiHandlerRetry                                                           */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiHandlerRetry (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiHandlerIdT id)
{
    SaErrorT rv;

    if (id == 0) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&id);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFoHpiHandlerRetry, sid, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiGlobalParamGet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiGlobalParamGet (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_INOUT oHpiGlobalParamT *param)
{
    SaErrorT rv;

    if (!param) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(param);
    ClientRpcParams oparams(param);
    rv = ohc_sess_rpc(eFoHpiGlobalParamGet, sid, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiGlobalParamSet                                                         */
/*----------------------------------------------------------------------------*/

SaErrorT SAHPI_API oHpiGlobalParamSet (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiGlobalParamT *param)
{
    SaErrorT rv;

    if (!param) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(param);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFoHpiGlobalParamSet, sid, iparams, oparams);

    return rv;
}


/*----------------------------------------------------------------------------*/
/* oHpiInjectEvent                                                            */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiInjectEvent (
    SAHPI_IN    SaHpiSessionIdT sid,
    SAHPI_IN    oHpiHandlerIdT id,
    SAHPI_IN    SaHpiEventT    *event,
    SAHPI_IN    SaHpiRptEntryT *rpte,
    SAHPI_IN    SaHpiRdrT *rdr)
{
    SaErrorT rv;

    if (id == 0 || !event || !rpte || !rdr) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    ClientRpcParams iparams(&id, event, rpte, rdr);
    ClientRpcParams oparams;
    rv = ohc_sess_rpc(eFoHpiInjectEvent, sid, iparams, oparams);

    return rv;
}



/*----------------------------------------------------------------------------*/
/* oHpiDomainAdd                                                              */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiDomainAdd (
    SAHPI_IN    const SaHpiTextBufferT *host,
    SAHPI_IN    SaHpiUint16T port,
    SAHPI_IN    const SaHpiEntityPathT *entity_root,
    SAHPI_OUT   SaHpiDomainIdT *domain_id)
{
    if (!host) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!domain_id) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!entity_root) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if ((host->DataType != SAHPI_TL_TYPE_BCDPLUS) &&
         (host->DataType != SAHPI_TL_TYPE_ASCII6) &&
         (host->DataType != SAHPI_TL_TYPE_TEXT))
    {
        return SA_ERR_HPI_INVALID_DATA;
    }

    // Function may be called before first session was opened,
    // so we may need to initialize
    ohc_init();

    char buf[SAHPI_MAX_TEXT_BUFFER_LENGTH+1];
    memcpy(&buf[0], &host->Data[0], host->DataLength);
    buf[host->DataLength] = '\0';

    return ohc_add_domain_conf(buf, port, entity_root, domain_id);
}



/*----------------------------------------------------------------------------*/
/* oHpiDomainAddById                                                          */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiDomainAddById (
    SAHPI_IN    SaHpiDomainIdT domain_id,
    SAHPI_IN    const SaHpiTextBufferT *host,
    SAHPI_IN    SaHpiUint16T port,
    SAHPI_IN    const SaHpiEntityPathT *entity_root)
{
    if (!host) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (!entity_root) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if ((host->DataType != SAHPI_TL_TYPE_BCDPLUS) &&
         (host->DataType != SAHPI_TL_TYPE_ASCII6) &&
         (host->DataType != SAHPI_TL_TYPE_TEXT))
    {
        return SA_ERR_HPI_INVALID_DATA;
    }

    // Function may be called before first session was opened,
    // so we may need to initialize
    ohc_init();

    char buf[SAHPI_MAX_TEXT_BUFFER_LENGTH+1];
    memcpy(&buf[0], &host->Data[0], host->DataLength);
    buf[host->DataLength] = '\0';

    return ohc_add_domain_conf_by_id(domain_id, buf, port, entity_root);
}

/*----------------------------------------------------------------------------*/
/* oHpiDomainEntryGet                                                         */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiDomainEntryGet (
    SAHPI_IN    SaHpiEntryIdT    EntryId,
    SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
    SAHPI_OUT   oHpiDomainEntryT *DomainEntry)

{
    if (!NextEntryId || !DomainEntry) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    // Function may be called before first session was opened,
    // so we may need to initialize
    ohc_init();

    const ohc_domain_conf *dc = ohc_get_next_domain_conf(EntryId, NextEntryId);
    if (dc == 0) { // no config for did found
        return SA_ERR_HPI_NOT_PRESENT;
    }

    DomainEntry->id = (SaHpiDomainIdT) EntryId;
    if (oh_init_textbuffer(&DomainEntry->host) != SA_OK) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (oh_append_textbuffer(&DomainEntry->host, dc->host)!= SA_OK) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    DomainEntry->port = dc->port;
    memcpy(&DomainEntry->entity_root, &dc->entity_root, sizeof(SaHpiEntityPathT));

    return SA_OK;
}


/*----------------------------------------------------------------------------*/
/* oHpiDomainEntryGetByDomainId                                               */
/*----------------------------------------------------------------------------*/
SaErrorT SAHPI_API oHpiDomainEntryGetByDomainId (
    SAHPI_IN    SaHpiDomainIdT    DomainId,
    SAHPI_OUT   oHpiDomainEntryT *DomainEntry)

{
    if (!DomainEntry) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    // Function may be called before first session was opened,
    // so we may need to initialize
    ohc_init();

    const ohc_domain_conf *entry = ohc_get_domain_conf(DomainId);
    if (entry == 0) { // no config for did found
        return SA_ERR_HPI_NOT_PRESENT;
    }

    DomainEntry->id = DomainId;
    if (oh_init_textbuffer(&DomainEntry->host) != SA_OK) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    if (oh_append_textbuffer(&DomainEntry->host, entry->host)!= SA_OK) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }
    DomainEntry->port = entry->port;
    memcpy(&DomainEntry->entity_root, &entry->entity_root, sizeof(SaHpiEntityPathT));    

    return SA_OK;
}

