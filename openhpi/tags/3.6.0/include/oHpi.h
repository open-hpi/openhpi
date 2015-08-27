/*      -*- linux-c -*-
 *
 * (C) Copright IBM Corp 2004-2006
 * (C) Copyright Pigeon Point Systems. 2010-2011
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
 *     Michael Thompson <michael.thompson@pentair.com>
 */

#ifndef __OHPI_H
#define __OHPI_H

#include <stdlib.h>

#include <SaHpi.h>

#include <glib.h>

#define OH_DEFAULT_DOMAIN_ID 0

#define OPENHPI_DEFAULT_DAEMON_PORT 4743
#define MAX_PLUGIN_NAME_LENGTH 32

#define OH_SAHPI_INTERFACE_VERSION_MIN_SUPPORTED (SaHpiVersionT)0x020101 /* B.01.01 */
#define OH_SAHPI_INTERFACE_VERSION_MAX_SUPPORTED SAHPI_INTERFACE_VERSION

#define OH_PATH_PARAM_MAX_LENGTH 2048

#ifdef __cplusplus
extern "C" {
#endif


typedef SaHpiUint32T oHpiHandlerIdT;


typedef struct {
    oHpiHandlerIdT id;
    SaHpiUint8T plugin_name[MAX_PLUGIN_NAME_LENGTH];
    SaHpiEntityPathT entity_root;
    SaHpiInt32T load_failed;
} oHpiHandlerInfoT;


typedef struct {
    SaHpiDomainIdT   id;
    SaHpiTextBufferT host;
    SaHpiUint16T     port;
    SaHpiEntityPathT entity_root;
} oHpiDomainEntryT;


typedef enum {
    OHPI_ON_EP = 1, // Not used now
    OHPI_LOG_ON_SEV,
    OHPI_EVT_QUEUE_LIMIT,
    OHPI_DEL_SIZE_LIMIT,
    OHPI_DEL_SAVE,
    OHPI_DAT_SIZE_LIMIT,
    OHPI_DAT_USER_LIMIT,
    OHPI_DAT_SAVE,
    //OHPI_DEBUG,
    //OHPI_DEBUG_TRACE,
    //OHPI_DEBUG_LOCK,
    OHPI_PATH,
    OHPI_VARPATH,
    OHPI_CONF
} oHpiGlobalParamTypeT;


typedef union {
    SaHpiEntityPathT OnEP; /* Not used now */
    /* Log events of severity equal to ore more critical than this */
    SaHpiSeverityT LogOnSev;
    SaHpiUint32T EvtQueueLimit; /* Max events # allowed in session queue */
    SaHpiUint32T DelSizeLimit; /* Maximum size of domain event log */
    SaHpiBoolT DelSave; /* True if domain event log is to be saved to disk */
    SaHpiUint32T DatSizeLimit; /* Max alarms allowed in alarm table */
    SaHpiUint32T DatUserLimit; /* Max number of user alarms allowed */
    SaHpiBoolT DatSave; /* True if domain alarm table is to be saved to disk */
    //unsigned char Debug; /* 1 = YES, 0 = NO */
    //unsigned char DebugTrace; /* !0 = YES, 0 = NO */
    //unsigned char DebugLock; /* !0 = YES, 0 = NO */
    SaHpiUint8T Path[OH_PATH_PARAM_MAX_LENGTH]; /* Dir path to openhpi plugins */
    SaHpiUint8T VarPath[OH_PATH_PARAM_MAX_LENGTH]; /* Dir path for openhpi data */
    SaHpiUint8T Conf[OH_PATH_PARAM_MAX_LENGTH]; /* File path of openhpi configuration */
} oHpiGlobalParamUnionT;


typedef struct {
    oHpiGlobalParamTypeT Type;
    oHpiGlobalParamUnionT u;
} oHpiGlobalParamT;


/***************************************************************************
**
** Name: oHpiVersionGet()
**
** Description:
**   This function returns the version of the Base Library.
**
** Remarks:
**   This is a Base Library level function.
**   The SaHpiUint64T version consists of 3 16-bit blocks: MAJOR, MINOR, and PATCH.
**   MAJOR occupies bytes[7:6].
**   MINOR occupies bytes[5:4].
**   PATCH occupies bytes[3:2].
**   Bytes[1:0] are zeroed.
**   For example version 2.17.0 is returned as 0x0002001100000000.
**
***************************************************************************/
SaHpiUint64T SAHPI_API oHpiVersionGet(void);

/***************************************************************************
**
** Name: oHpiHandlerCreate()
**
** Description:
**   This function creates a new handler (instance of a plugin) on the
**   OpenHPI daemon side.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   config - [in] Hash table. Holds configuration information used by the handler.
**      The table key is a configuration parameter name (as a string).
**      The table item is the configuration parameter value (as a string).
**   id - [out] Unique (for the targeted OpenHPI daemon) id associated
**      with the created handler.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned when the:
**   * config is is passed in as NULL.
**   * id is is passed in as NULL.
**   SA_ERR_HPI_INTERNAL_ERROR is returned if a handler is created,
**     but failed to open.
**   SA_ERR_HPI_ERROR is returned:
**   * when config doesn't have "plugin" parameter
**   * in other error cases.
**
** Remarks:
**   This is a Daemon level function.
**   The config table must have an entry for "plugin" in order to know for which
**   plugin the handler is being created.
**   If the handler failed to open the oHpiHandlerRetry can be used to retry
**   opening the handler.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerCreate (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    GHashTable *config,
     SAHPI_OUT   oHpiHandlerIdT *id );

/***************************************************************************
**
** Name: oHpiHandlerDestroy()
**
** Description:
**   This function destroys the specified handler on the OpenHPI daemon side.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   id - [in] Unique (for the targeted OpenHPI daemon) id associated
**      with the handler. Reserved id values:
**   * SAHPI_FIRST_ENTRY     Get first handler.
**   * SAHPI_LAST_ENTRY      Reserved as delimiter for end of list. Not a valid
**      handler identifier.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_ERROR is returned:
**   * when the daemon failed to destroy the handler.
**   * in other error cases.
**
** Remarks:
**   This is Daemon level function.
**   TODO: Improve the current set of error codes for the function.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerDestroy (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id );

/***************************************************************************
**
** Name: oHpiHandlerInfo()
**
** Description:
**   This function is used for requesting information about specified handler.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   id - [in] Unique (for the targeted OpenHPI daemon) id associated
**      with the handler. Reserved id values:
**   * SAHPI_FIRST_ENTRY     Get first handler.
**   * SAHPI_LAST_ENTRY      Reserved as delimiter for end of list. Not a valid
**      handler identifier.
**   info - [out] Pointer to struct to hold the handler information.
**   conf_params - [out] Pointer to to pre-allocated hash-table to hold
**      the handler's configuration parameters.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned when the:
**   * info is passed in as NULL
**   * conf_params is passed in as NULL
**   * id is passed in as SAHPI_FIRST_ENTRY
**   SA_ERR_HPI_NOT_PRESENT is returned when the specified the targeted
**     OpenHPI daemon has no handler with the specified id.
**
** Remarks:
**   This is Daemon level function.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerInfo (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_OUT   oHpiHandlerInfoT *info,
     SAHPI_INOUT GHashTable *conf_params );

/***************************************************************************
**
** Name: oHpiHandlerGetNext()
**
** Description:
**   This function is used for iterating through all loaded handlers on the
**   targeted OpenHPI daemon.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   id - [in] Unique (for the targeted OpenHPI daemon) id associated
**      with the handler. Reserved id values:
**   * SAHPI_FIRST_ENTRY     Get first handler.
**   * SAHPI_LAST_ENTRY      Reserved as delimiter for end of list. Not a valid
**      handler identifier.
**   next_id - [out] Pointer to the location to store the id of the next handler.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_NOT_PRESENT is returned when the:
**   * handler identified by id is not present.
*    * there is no next handler after the specified one.
**   * id is SAHPI_FIRST_ENTRY and there are no loaded handlers in the targeted
**     OpenHPI daemon.
**   SA_ERR_HPI_INVALID_PARAMS is returned if the:
**   * next_id pointer is passed in as NULL.
**   * id is an invalid reserved value such as SAHPI_LAST_ENTRY.
**
** Remarks:
**   This is a Daemon level function.
**   If the id parameter is set to SAHPI_FIRST_ENTRY, the function sets
**   the next_id parameter to the id of the first handler for the targeted
**   OpenHPI daemon. To retrieve an entire list of ids for the targeted
**   OpenHPI daemon call this function first with an id of SAHPI_FIRST_ENTRY,
**   and then use the returned next_id the next call.
**   Proceed until the next_id returned is SAHPI_LAST_ENTRY or
**   the call returned SA_ERR_HPI_NOT_PRESENT.
**   TODO: Implement a check that the id is and invalid reserved value.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerGetNext (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_OUT   oHpiHandlerIdT *next_id );

/***************************************************************************
**
** Name: oHpiHandlerFind()
**
** Description:
**   This function is used for requesting a handler id that manages the specified
**   resource.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   rid - [in] Resource id.
**   id - [out] Pointer to the location to store the handler id.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_RESOURCE is returned if resource id does not match
**      any resource in the targeted OpenHPI daemon.  
**
** Remarks:
**   This is Daemon level function.
**   Every resource that the OpenHPI daemon exposes is created and managed by
**   some handler. This function allows to know the id of the handler
**   responsible for the specified resource.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerFind (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    SaHpiResourceIdT rid,
     SAHPI_OUT   oHpiHandlerIdT *id );

/***************************************************************************
**
** Name: oHpiHandlerRetry()
**
** Description:
**   This function is used for retrying initialization of the failed handler.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   id - [in] Unique (for the targeted OpenHPI daemon) id associated
**      with the handler. Reserved id values:
**   * SAHPI_FIRST_ENTRY     Get first handler.
**   * SAHPI_LAST_ENTRY      Reserved as delimiter for end of list. Not a valid
**      handler identifier.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_OK is returned if the handler was already successfully initialized.
**   SA_ERR_HPI_NOT_PRESENT is returned when the targeted OpenHPI daemon
**      has no handler with the specified id.
**
** Remarks:
**   This is Daemon level function.
**   It is possible that the handler has failed to initialize.
**   For example because of a connection failure to the managed entity.
**   This function allows an HPI User to retry the handler initialization process.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiHandlerRetry (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id );

/***************************************************************************
**
** Name: oHpiGlobalParamGet()
**
** Description:
**   This function is used for requesting global configuration parameters
**   from the targeted OpenHPI daemon.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   param - [in/out] Pointer to a Parameter data structure into which the
**      requested parameter is placed. The requested parameter type is
**      passed in via param->Type.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned if param is passed in as NULL.
**   SA_ERR_HPI_UNKNOWN is returned if param->Type is unknown to the
**      targeted OpenHPI daemon.
**
** Remarks:
**   This is Daemon level function.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiGlobalParamGet (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_INOUT oHpiGlobalParamT *param );

/***************************************************************************
**
** Name: oHpiGlobalParamSet()
**
** Description:
**   This function is used for setting the value of the specified global
**   configuration parameters for the targeted OpenHPI daemon.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   param - [in] Pointer to a Parameter data structure holding the
**      data to be set. The requested parameter type is passed in via
**      param->Type.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned if param is passed in as NULL.
**   SA_ERR_HPI_ERROR is returned:
**   * when the param->Type is unknown to the targeted OpenHPI daemon.
**   * in other error cases
**
** Remarks:
**   This is Daemon level function.
**   TODO: Improve the current set of error codes for the function.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiGlobalParamSet (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiGlobalParamT *param );

/***************************************************************************
**
** Name: oHpiInjectEvent()
**
** Description:
**   This function is used to inject an HPI event into the targeted handler as if
**   it originated from the handler.
**   This call will set the event.Source, rpte.ResourceId, rpte.ResourceEntity
**   so that the caller knows what the final assigned values were.
**   For rpte.ResourceEntity, the entity_root configuration parameter for the plugin
**   is used to complete it. In addition, for each rdr in rdr, a Num, RecordId,
**   and Entity will be assigned. This will also be reflected in the passed rdr
**   list so that the caller can know what the assigned values were.
**
** Parameters:
**   sid - [in] Identifier for a session context previously obtained using
**      saHpiSessionOpen().
**   id - [in] Unique (for the targeted OpenHPI daemon) id associated
**      with the handler for which the event will be injected.
**   event - [in] pointer to the event to be injected.
**   rpte - [in] pointer to the resource to be injected.
**   rdr - [in] pointer to the list of RDRs to be injected along with rpte
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_INVALID_SESSION is returned if sid is null.
**   SA_ERR_HPI_INVALID_PARAMS is returned if
**      id is null.
**      event, rpte, or rdr pointer are passed in as NULL
**
** Remarks:
**   This is Daemon level function.
**   Currently only the simulator plug-in supports this function.
**   id and event are required parameters. rpte is only required if the event
**   is of RESOURCE type or HOTSWAP type. rdr is an optional argument in all
**   cases and can be NULL. If @rdrs is passed, it will be copied. It is the
**   responsibility of the caller to clean up the RDRs list once it is used here.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiInjectEvent (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_IN    SaHpiEventT    *event,
     SAHPI_IN    SaHpiRptEntryT *rpte,
     SAHPI_IN    SaHpiRdrT *rdr);

/***************************************************************************
**
** Name: oHpiDomainAdd()
**
** Description:
**   This function informs the Base Library about the existence of an HPI domain
**   and returns a Unique Domain Id that Base Library associates with the domain.
**   In other words the function adds a new entry in Base Library known domain
**   table.
**
** Parameters:
**   host - [in] The hostname of the domain host.
**   port - [in] The port number on the domain host.
**   entity_root - [in] This entity_root will be added to all entity paths
**     exposed in the domain.
**   domain_id - [out] Unique Domain Id associated with the new domain
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**     returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned if the host, port, or entity_root
**     is passed in as NULL.
**   SA_ERR_HPI_INVALID_DATA is returned if the host is not
**     SAHPI_TL_TYPE_BCDPLUS, SAHPI_TL_TYPE_ASCII6, or SAHPI_TL_TYPE_TEXT.
**   SA_ERR_HPI_OUT_OF_SPACE is returned if the domain cannot be added.
**
** Remarks:
**   This is Base Library level function.
**   It only informs Base Library about existence of HPI service with the
**   specified connectivity parameters.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiDomainAdd (
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port,
     SAHPI_IN    const SaHpiEntityPathT *entity_root,
     SAHPI_OUT   SaHpiDomainIdT *domain_id );

/***************************************************************************
**
** Name: oHpiDomainAddById()
**
** Description:
**   This function informs the Base Library about the existence of an HPI domain.
**   The function allows an HPI User to specify a Unique Domain Id that Base Library
**     associates with the domain.
**   In other words the function adds new entry in the Base Library known domain
**   table.
**
** Parameters:
**   host - [in] The hostname of the domain host.
**   port - [in] The port number on the domain host.
**   entity_root - [in] This entity_root will be added to all entity paths
**     exposed in the domain.
**   domain_id - [in] Unique Domain Id that User wants to be associated with
**     the new domain.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**     returned.
**   SA_ERR_HPI_INVALID_PARAMS is returned if the host, port, or entity_root
**     is passed in as NULL.
**   SA_ERR_HPI_INVALID_DATA is returned if the host is not
**     SAHPI_TL_TYPE_BCDPLUS, SAHPI_TL_TYPE_ASCII6, or SAHPI_TL_TYPE_TEXT.
**   SA_ERR_HPI_DUPLICATE is returned if the specified domain_id is already
**     in use.
**   SA_ERR_HPI_OUT_OF_SPACE is returned if the domain cannot be added.
**
** Remarks:
**   This is Base Library level function.
**   It only informs Base Library about existence of HPI service with the
**   specified connectivity parameters.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiDomainAddById (
     SAHPI_IN    SaHpiDomainIdT domain_id,
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port,
     SAHPI_IN    const SaHpiEntityPathT *entity_root );

/***************************************************************************
**
** Name: oHpiDomainEntryGet()
**
** Description:
**   This function retrieves resource information for the specified entry of the
**   Base Library known domain table. This function allows an HPI User to read
**   the table entry-by-entry.
**
** Parameters:
**   EntryId - [in] Identifier of the domain entry to retrieve. Reserved EntryId
**      values:
**   * SAHPI_FIRST_ENTRY     Get first entry.
**   * SAHPI_LAST_ENTRY      Reserved as delimiter for end of list. Not a valid
**      entry identifier.
**   NextEntryId - [out] Pointer to the location to store the EntryId of next
**      domain entry.
**   DomainEntry - [out] Pointer to the structure to hold the returned domain
**      entry.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_NOT_PRESENT is returned when the:
**   * Entry identified by EntryId is not present.
**   * EntryId is SAHPI_FIRST_ENTRY and the Base Library domain table is empty.
**   SA_ERR_HPI_INVALID_PARAMS is returned if the:
**   * DomainEntry pointer is passed in as NULL.
**   * NextEntryId pointer is passed in as NULL.
**   * EntryId is an invalid reserved value such as SAHPI_LAST_ENTRY.
**
** Remarks:
**   This is Base Library level function.
**   If the EntryId parameter is set to SAHPI_FIRST_ENTRY, the first entry in
**   the Base Library domain table is returned. When an entry is successfully
**   retrieved, NextEntryId  is set to the identifier of the next valid entry;
**   however, when the last entry has been retrieved, NextEntryId is set to
**   SAHPI_LAST_ENTRY. To retrieve an entire list of entries, call this function
**   first with an EntryId of SAHPI_FIRST_ENTRY, and then use the returned
**   NextEntryId in the next call. Proceed until the NextEntryId returned is
**   SAHPI_LAST_ENTRY.
***************************************************************************/
SaErrorT SAHPI_API oHpiDomainEntryGet (
     SAHPI_IN    SaHpiEntryIdT    EntryId,
     SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
     SAHPI_OUT   oHpiDomainEntryT *DomainEntry );

/*******************************************************************************
**
** Name: oHpiDomainEntryGetByDomainId()
**
** Description:
**   This function retrieves resource information from the Base Library known
**   domain table for the specified domain using its DomainId.
**
** Parameters:
**   DomainId - [in] Domain identified for this operation.
**   DomainEntry - [out] Pointer to the structure to hold the returned domain
**      entry.
**
** Return Value:
**   SA_OK is returned on successful completion; otherwise, an error code is
**      returned.
**   SA_ERR_HPI_NOT_PRESENT is returned when the specified DomainId is unknown
**      to the Base Library.
**   SA_ERR_HPI_INVALID_PARAMS is returned if the DomainEntry pointer is passed in
**      as NULL.
**
** Remarks:
**   This is Base Library level function.
**
***************************************************************************/
SaErrorT SAHPI_API oHpiDomainEntryGetByDomainId (
     SAHPI_IN    SaHpiDomainIdT    DomainId,
     SAHPI_OUT   oHpiDomainEntryT *DomainEntry );


#define OHPI_VERSION_GET(v, VER) \
{ \
    char version[] = VER; \
    char *start = version; \
    char *end = version; \
    v += (strtoull(start, &end, 10) << 48); \
    end++; \
    start = end; \
    v += (strtoull(start, &end, 10) << 32); \
    end++; \
    start = end; \
    v += (strtoull(start, &end, 10) << 16); \
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__OHPI_H*/
