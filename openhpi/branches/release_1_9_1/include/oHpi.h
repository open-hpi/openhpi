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

typedef struct {
        int refcount; /* refcount - 1 = # of handlers using it. */
        /* More to come later... */
} oHpiPluginInfoT;

typedef struct {
        char plugin_name[MAX_PLUGIN_NAME_LENGTH];
        /* More to come later... */
} oHpiHandlerInfoT;

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
SaErrorT oHpiGlobalParamGet(char *name, char *value, int size);
SaErrorT oHpiGlobalParamSet(const char *name, char *value);

#endif /*__OHPI_H*/
