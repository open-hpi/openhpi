/*      -*- linux-c -*-
 *
 * (C) Copright IBM Corp 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renierm@users.sourceforge.net>
 */

#ifndef __OHPI_H
#define __OHPI_H

#include <SaHpi.h>
#include <glib.h>

typedef SaHpiUint32T oHpiHandlerIdT;
#define MAX_PLUGIN_NAME_LENGTH 32
#define OH_GLOBAL_STR_MAX_LENGTH SAHPI_MAX_TEXT_BUFFER_LENGTH*2

typedef struct {
        int refcount; /* refcount - 1 = # of handlers using it. */
        /* More to come later... */
} oHpiPluginInfoT;

typedef struct {
        char plugin_name[MAX_PLUGIN_NAME_LENGTH];
        /* More to come later... */
} oHpiHandlerInfoT;

typedef enum {
        OHPI_ON_EP = 1,
        OHPI_LOG_ON_SEV,
        //OHPI_DEBUG,
        //OHPI_DEBUG_TRACE,
        //OHPI_DEBUG_LOCK,
        OHPI_THREADED,
        OHPI_PATH,
        OHPI_CONF
} oHpiGlobalParamTypeT;

typedef union {
        SaHpiEntityPathT OnEP;
        SaHpiSeverityT LogOnSev;
        //unsigned char Debug; /* 1 = YES, 0 = NO */
        //unsigned char DebugTrace; /* !0 = YES, 0 = NO */
        //unsigned char DebugLock; /* !0 = YES, 0 = NO */
        unsigned char Threaded; /* !0 = YES, 0 = NO */
        char Path[OH_GLOBAL_STR_MAX_LENGTH];
        char Conf[SAHPI_MAX_TEXT_BUFFER_LENGTH];
} oHpiGlobalParamUnionT;

typedef struct {
        oHpiGlobalParamTypeT Type;
        oHpiGlobalParamUnionT u;
} oHpiGlobalParamT;

/* Exported OpenHPI plugin prototypes */
SaErrorT oHpiPluginLoad(char *name);
SaErrorT oHpiPluginUnload(char *name);
SaErrorT oHpiPluginInfo(char *name, oHpiPluginInfoT *info);
SaErrorT oHpiPluginGetNext(char *name, char *next_name, int size);

/* Exported OpenHPI handler (plugin instance) prototypes */
SaErrorT oHpiHandlerCreate(GHashTable *config,
                           oHpiHandlerIdT *id);
SaErrorT oHpiHandlerDestroy(oHpiHandlerIdT id);
SaErrorT oHpiHandlerInfo(oHpiHandlerIdT id, oHpiHandlerInfoT *info);
SaErrorT oHpiHandlerGetNext(oHpiHandlerIdT id, oHpiHandlerIdT *next_id);

/* Global parameters */
SaErrorT oHpiGlobalParamGet(oHpiGlobalParamT *param);
SaErrorT oHpiGlobalParamSet(oHpiGlobalParamT *param);

#endif /*__OHPI_H*/
