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
#include <oh_init.h>
#include <oh_plugin.h>
#include <oh_error.h>
#include <oh_lock.h>

#define OH_CHECK_STATE \
        do { \
                if (oh_initialized() != SA_OK) { \
                        dbg("OpenHPI is not initialized"); \
                        return SA_ERR_HPI_INVALID_SESSION; \
                } \
        } while (0)        

/* Plugin operations */

/**
 * oHpiPluginLoad
 * @name: IN. String. name of plugin to load (e.g. "libdummy")
 *
 * Loads plugin into library creating a plugin object.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiPluginLoad(char *name)
{
        if (!name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;
        
        if (oh_load_plugin(name))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

/**
 * oHpiPluginUnload
 * @name: IN. String. name of plugin to unload (e.g. "libdummy")
 *
 * Unload plugin from library, destroying the plugin object.
 * This will return an error if there are any handlers referencing
 * the plugin (e.g. refcount > 1).
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiPluginUnload(char *name)
{
        if (!name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;

        if (oh_unload_plugin(name))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

/**
 * oHpiPluginInfo
 * @name: IN. String. name of plugin to query (e.g. "libdummy")
 * @info: IN/OUT. Reference to information structure on the plugin.
 *
 * Unload plugin from library, destroying the plugin object.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiPluginInfo(char *name, oHpiPluginInfoT *info)
{
        struct oh_plugin *p = NULL;

        if (!name || !info) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;

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

/**
 * oHpiPluginGetNext
 * @name: IN. String. name of plugin to search for (e.g. "libdummy")
 * @next_name: IN/OUT. Next plugin after @name will be placed here.
 * @size: IN. Size in bytes of the @next_name buffer.
 *
 * Searches for the specified plugin and returns the next plugin name
 * after that one in the list. If you pass NULL in @name, you will get
 * the name of the first plugin in @next_name. Used to iterate through
 * all loaded plugins.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiPluginGetNext(char *name, char *next_name, int size)
{
        if (!next_name) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;

        if (oh_lookup_next_plugin(name, next_name, size))
                return SA_ERR_HPI_NOT_PRESENT;

        return SA_OK;
}


/* Handler operations */

/**
 * oHpiHandlerCreate
 * @config: IN. Hash table. Holds configuration information used by handler.
 * @id: IN/OUT. The id of the newly created handler is returned here.
 *
 * Creates a new handler (instance of a plugin). Plugin handlers are what
 * respond to most API calls.
 * @config needs to have an entry for "plugin" in order to know for which
 * plugin the handler is being created.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiHandlerCreate(GHashTable *config,
                           oHpiHandlerIdT *id)
{
        oHpiHandlerIdT hid = 0;

        if (!config || !id) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;

        if (!(hid = oh_load_handler(config))) {
             *id = 0;
             return SA_ERR_HPI_ERROR;
        }

        *id = hid;

        return SA_OK;
}

/**
 * oHpiHandlerDestroy
 * @id: IN. The id of the handler to destroy
 *
 * Destroys a handler. Calls the plugin's abi close function.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiHandlerDestroy(oHpiHandlerIdT id)
{
        if (!id)
                return SA_ERR_HPI_INVALID_PARAMS;

        OH_CHECK_STATE;
        
        if (oh_unload_handler(id))
                return SA_ERR_HPI_ERROR;

        return SA_OK;
}

/**
 * oHpiHandlerInfo
 * @id: IN. The id of the handler to query
 * @info: IN/OUT. Pointer to struct for holding handler information
 *
 * Queries a handler for the information associated with it.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiHandlerInfo(oHpiHandlerIdT id, oHpiHandlerInfoT *info)
{
        struct oh_handler *h = NULL;

        if (!id || !info)
               return SA_ERR_HPI_INVALID_PARAMS;
        
        OH_CHECK_STATE;

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

/**
 * oHpiHandlerGetNext
 * @id: IN. Id of handler to search for.
 * @next_id: IN/OUT. The id of the handler next to the handler being searched for
 * will be returned here.
 *
 * Used for iterating through all loaded handlers. If you pass
 * 0 (SAHPI_FIRST_ENTRY), you will get the id of the first handler returned
 * in next_id.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiHandlerGetNext(oHpiHandlerIdT id, oHpiHandlerIdT *next_id)
{
        if (!next_id) {
                dbg("Invalid parameters.");
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        OH_CHECK_STATE;

        if (oh_lookup_next_handler(id, next_id))
                return SA_ERR_HPI_NOT_PRESENT;

        return SA_OK;
}

/* Global parameters */

/**
 * oHpiGlobalParamGet
 * @name: IN. Name of global parameter to get.
 * @value: IN/OUT. Place to put the value for parameter @name.
 * @size: IN. Size of @value buffer.
 *
 * Gets the value of the specified global parameter.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiGlobalParamGet(char *name, char *value, int size)
{
        return -1;
}

/**
 * oHpiGlobalParamSet
 * @name: IN. Name of global parameter to set.
 * @value: IN. Value to set.
 *
 * Sets a global parameter. This will return an error if
 * an attempt to set is made after any plugins have been loaded.
 *
 * Returns: SA_OK on success. Minus SA_OK on error.
 **/
SaErrorT oHpiGlobalParamSet(const char *name, char *value)
{
        return -1;
}
