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

#include <oHpi.h>
#include <oh_plugin.h>
#include <oh_error.h>
#include <oh_lock.h>

/* Plugin operations */

SaErrorT oHpiPluginLoad(char *name)
{
        if (!name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        if (oh_load_plugin(name))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

SaErrorT oHpiPluginUnload(char *name)
{
        if (!name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (oh_unload_plugin(name))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

SaErrorT oHpiPluginInfo(char *name, oHpiPluginInfoT *info)
{
        struct oh_plugin *p = NULL;

        if (!name || !info) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        data_access_lock();
        p = oh_lookup_plugin(name);
        if (!p) {
                dbg("Plugin %s not found.", name);
                return SA_ERR_HPI_NOT_PRESENT;
        	data_access_unlock();
        }

        info->refcount = p->refcount;
        data_access_unlock();

        return SA_OK;
}

SaErrorT oHpiPluginGetNext(char *name, char *next_name, int size)
{
        if (!name || !next_name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (oh_lookup_next_plugin(name, next_name, size))
                return SA_ERR_HPI_NOT_PRESENT;

        return SA_OK;
}


/* Handler operations */

SaErrorT oHpiHandlerCreate(GHashTable *config,
                           oHpiHandlerIdT *id)
{
        oHpiHandlerIdT hid = 0;

        if (!config || !id) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }                

        if (!(hid = oh_load_handler(config))) {
             *id = 0;
             return SA_ERR_HPI_ERROR;
        }

        *id = hid;

        return SA_OK;
}

SaErrorT oHpiHandlerDestroy(oHpiHandlerIdT id)
{
        if (!id)
                return SA_ERR_HPI_INVALID_PARAMS;

        if (oh_unload_handler(id))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

SaErrorT oHpiHandlerInfo(oHpiHandlerIdT id, oHpiHandlerInfoT *info)
{
        struct oh_handler *h = NULL;

        if (!id || !info)
               return SA_ERR_HPI_INVALID_PARAMS;

	data_access_lock();
        h = oh_lookup_handler(id);
	if (!h) {
		dbg("Handler not found.");
		data_access_unlock();
		return SA_ERR_HPI_NOT_PRESENT;
	}

	strncpy(info->plugin_name, h->plugin_name, MAX_PLUGIN_NAME_LENGTH);
	data_access_unlock();
	
	return SA_OK;
}

SaErrorT oHpiHandlerGetNext(oHpiHandlerIdT id, oHpiHandlerIdT *next_id)
{
        if (!id || !next_id) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (oh_lookup_next_handler(id, next_id))
                return SA_ERR_HPI_NOT_PRESENT;

        return SA_OK;
}

