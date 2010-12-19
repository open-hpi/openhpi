/*      -*- linux-c -*-
 *
 * (C) Copright IBM Corp 2004-2006
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

#ifndef __OHPI_H
#define __OHPI_H

#include <stdlib.h>

#include <SaHpi.h>
#include <glib.h>
#include <oh_utils.h>

#define OH_DEFAULT_DOMAIN_ID 0

#define OPENHPI_DEFAULT_DAEMON_PORT 4743
#define MAX_PLUGIN_NAME_LENGTH 32

#define OH_SAHPI_INTERFACE_VERSION_MIN_SUPPORTED (SaHpiVersionT)0x020101 /* B.01.01 */
#define OH_SAHPI_INTERFACE_VERSION_MAX_SUPPORTED SAHPI_INTERFACE_VERSION

#ifdef __cplusplus
extern "C" {
#endif

typedef SaHpiUint32T oHpiHandlerIdT;

typedef struct {
        oHpiHandlerIdT id;
        char plugin_name[MAX_PLUGIN_NAME_LENGTH];
        SaHpiEntityPathT entity_root;
        int load_failed;
} oHpiHandlerInfoT;

typedef struct {
        SaHpiDomainIdT   id;
        SaHpiTextBufferT daemonhost;
        SaHpiUint16T     port;
} oHpiDomainEntryT;



typedef enum {
        OHPI_ON_EP = 1,
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
        SaHpiEntityPathT OnEP; /* Entity path where this openhpi instance runs */
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
        char Path[OH_MAX_TEXT_BUFFER_LENGTH]; /* Dir path to openhpi plugins */
        char VarPath[OH_MAX_TEXT_BUFFER_LENGTH]; /* Dir path for openhpi data */
        char Conf[OH_MAX_TEXT_BUFFER_LENGTH]; /* File path of openhpi configuration */
} oHpiGlobalParamUnionT;

typedef struct {
        oHpiGlobalParamTypeT Type;
        oHpiGlobalParamUnionT u;
} oHpiGlobalParamT;

/* Version function */
SaHpiUint64T SAHPI_API oHpiVersionGet(void);

/* Exported OpenHPI handler (plugin instance) calls */
SaErrorT SAHPI_API oHpiHandlerCreate ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    GHashTable *config,
     SAHPI_OUT   oHpiHandlerIdT *id );
SaErrorT SAHPI_API oHpiHandlerDestroy (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id );
SaErrorT SAHPI_API oHpiHandlerInfo ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id, 
     SAHPI_OUT   oHpiHandlerInfoT *info,
     SAHPI_INOUT GHashTable *conf_params );
SaErrorT SAHPI_API oHpiHandlerGetNext (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id, 
     SAHPI_OUT   oHpiHandlerIdT *next_id );
SaErrorT SAHPI_API oHpiHandlerFind ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    SaHpiResourceIdT rid,
     SAHPI_OUT   oHpiHandlerIdT *id );
SaErrorT SAHPI_API oHpiHandlerRetry (
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id );

/* Global parameters */
SaErrorT SAHPI_API oHpiGlobalParamGet ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_OUT   oHpiGlobalParamT *param );
SaErrorT SAHPI_API oHpiGlobalParamSet ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiGlobalParamT *param );

/* Injector */
SaErrorT SAHPI_API oHpiInjectEvent ( 
     SAHPI_IN    SaHpiSessionIdT sid,
     SAHPI_IN    oHpiHandlerIdT id,
     SAHPI_IN    SaHpiEventT    *event,
     SAHPI_IN    SaHpiRptEntryT *rpte,
     SAHPI_IN    SaHpiRdrT *rdr);

/* Domain configuration */
SaErrorT SAHPI_API oHpiDomainAdd ( 
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port,
     SAHPI_OUT   SaHpiDomainIdT *domain_id );
SaErrorT SAHPI_API oHpiDomainAddById ( 
     SAHPI_IN    SaHpiDomainIdT domain_id,
     SAHPI_IN    const SaHpiTextBufferT *host,
     SAHPI_IN    SaHpiUint16T port );
SaErrorT SAHPI_API oHpiDomainEntryGet (
     SAHPI_IN    SaHpiEntryIdT    EntryId,
     SAHPI_OUT   SaHpiEntryIdT    *NextEntryId,
     SAHPI_OUT   oHpiDomainEntryT *DomainEntry );
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
