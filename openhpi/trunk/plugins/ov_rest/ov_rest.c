/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 *
 * This file implements the entry point of the ov rest plug-in. This handles
 * custom handler for ov rest interface for handling the any request for rest
 * interface. This APIs uses rest_open and rest_close APIs from rest interface
 * for initiating and closing the communication with REST interface / ov.
 *
 *      build_ov_rest_custom_handler()  - Builds the OV REST custom handler
 *
 *      ov_rest_open()                  - Opens ov rest interface connection
 *                                        for initiating communication with OV.
 *
 *      ov_rest_close()                 - Closes the communication with OV.
 *
 *      ov_rest_set_resource_tag()      - Handles setting the tags to resources
 *
 *      ov_rest_set_resource_severity() - Sets the resource's severity.
 *
 */

#include "ov_rest.h"
#include "ov_rest_utils.h"
#include "sahpi_wrappers.h"

/**
 * build_ov_rest_custom_handler:
 *      @oh_handler: Pointer to OpenHPI handler.
 *
 * Purpose:
 *      Builds and initializes the OV REST custom handler.
 *
 * Detailed Description:
 *      - If the plugin initialization fails, then this method will be called
 *        more than once, until the plugin intialization succeeds.
 *      - In case of the data field in oh_handler is null, we are filling the
 *        ov_handler information
 *
 * Return values:
 *      SA_OK                     - on sucess.
 *      SA_ERR_HPI_INVALID_PARAMS - parameter(s) are NULL.
 *      SA_ERR_HPI_OUT_OF_MEMORY  - not enough memory to allocate.
 **/
SaErrorT build_ov_rest_custom_handler(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        struct ov_rest_handler *ov_handler = NULL;

        if (oh_handler == NULL) {
                err("Invalid parmaters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        ov_handler = (struct ov_rest_handler *) oh_handler->data;


        /* Check whether ov_rest_handler is initialized or not.
         * If the plugin initialization fails, then this method will be
         * called more than once.
         *
         * The below if statement is to avoid multiple initialization
         * of the ov_rest_handler.
         */
        if (oh_handler->data == NULL) {
                /* Initialize the ov_rest_handler */
                ov_handler = (struct ov_rest_handler *)
                        g_malloc0(sizeof(struct ov_rest_handler));
                if (ov_handler == NULL) {
                        err("The ov_handler is out of memory in __func__");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
		ov_handler->mutex = wrap_g_mutex_new_init();
                ov_handler->ov_mutex = wrap_g_mutex_new_init();  
                ov_handler->status = PRE_DISCOVERY;
                ov_handler->connection = (REST_CON *)
                        g_malloc0(sizeof(REST_CON));
                ov_handler->thread_handler = NULL;
                oh_handler->data = ov_handler;
        }

        rv = ov_rest_connection_init(oh_handler);
        if(rv !=SA_OK){
                ov_handler->status = PLUGIN_NOT_INITIALIZED;
                err("OV_handler connection init failed");
                return rv;
        }

        ov_handler->uri_rid = g_hash_table_new(g_str_hash, g_str_equal);

        return rv;
}

/**
 * ov_rest_open:
 *      @handler_config: Handler data pointer.
 *      @handler_id:     Id for the handler
 *      @eventq:         Pointer to the infrstructure event queue.
 *
 * Purpose:
 *      This function opens OV REST plugin handler instance.
 *
 * Detailed Description:
 *      - This function will be the entry point for the ov rest plug-in.
 *      - We will check all configuration parameters. We won't validate the
 *        content of the variables. We will check whether the parameters are
 *        assigned or not. User can assign even the empty string into the
 *        configuration file.
 *      - Will assign handler_config, handler id and allocate memory for the
 *        RPT table and cache.
 *      - "build_ov_rest_custom_handler" will be called for initializing the
 *        customer handler address into framework.
 *
 * Return values:
 *      Plugin handle - on success.
 *      NULL          - on error.
 **/
void *ov_rest_open(GHashTable *handler_config,
                   unsigned int handler_id,
                   oh_evt_queue *eventq)
{
	int rv = SA_OK;
	struct oh_handler_state *handler = NULL;

	if (handler_config == NULL || handler_id == 0 || eventq == NULL) {
		err("Invalid parameters");
		return NULL;
	}

	/* Check whether all the parameters are provided or not */
	rv = ov_rest_check_config_parameters(handler_config);
	if (rv != SA_OK) {
		err("Config file has some missing parameters");
		return NULL;
	}

	/* Initialize the handler */
	handler = (struct oh_handler_state *)
		g_malloc0(sizeof(struct oh_handler_state));
	if (handler == NULL) {
		err("Handler is out of memory in __func__");
		return NULL;
	}

	handler->config = handler_config;
	handler->hid = handler_id;
	handler->eventq = eventq;
	handler->rptcache = (RPTable *) g_malloc0(sizeof(RPTable));
	if (handler->rptcache == NULL) {
		wrap_g_free(handler);
		err("Handler rptcache is out of memory in __func__");
		return NULL;
	}

	rv = oh_init_rpt(handler->rptcache);
	if (rv != SA_OK) {
		err("Initializing rptcache failed");
		wrap_g_free(handler->rptcache);
		wrap_g_free(handler);
		return NULL;
	}
	handler->data = NULL;

	/* Build the custom handler for OV REST plugin */
	rv = build_ov_rest_custom_handler(handler);
	if (rv != SA_OK) {
		err("Build OV_REST custom handler failed");
		/* If the failure due to out of memory, return NULL
		 * Else, try to build the ov_rest_handler during discovery call
		 *
		 */
		if (rv == SA_ERR_HPI_OUT_OF_MEMORY) {
			wrap_g_free(handler->rptcache);
			wrap_g_free(handler);
			return NULL;
		}
	}
	return ((void *)handler);
}

/**
 * ov_rest_close:
 *      @oh_handler: Handler data pointer.
 *
 * Purpose:
 *      This function closes OV REST plugin handler instance.
 *
 * Detailed Description:
 *      - Releases all the memory allocated by OV REST plugin handler
 *
 * Return values:
 *      NONE
 **/
void ov_rest_close(void *oh_handler)
{
        struct oh_handler_state *handler = NULL;
        struct ov_rest_handler *ov_handler = NULL;
        dbg("Shutting down the ov_rest plugin");
        handler = (struct oh_handler_state *) oh_handler;
	ov_handler = (struct ov_rest_handler *) handler->data;
        /* Check whether ov_handler is initialized or not */
        if (ov_handler == NULL){
                return;
	}
        /* Set the event thread shutdown status to TRUE */
        ov_handler->shutdown_event_thread = SAHPI_TRUE;

	if(ov_handler->thread_handler != NULL){
		g_thread_join(ov_handler->thread_handler);
	}
	ov_rest_clean_rptable(handler);
	wrap_g_free(handler->rptcache);
}

/**
 * ov_rest_set_resource_tag:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @tag:         Pointer to new tag.
 *
 * Purpose:
 *      Sets resource's tag.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - parameter(s) are NULL @tag is invalid.
 *      SA_ERR_HPI_NOT_PRESENT    - resource does not exist.
 **/
SaErrorT ov_rest_set_resource_tag(void *oh_handler,
                                  SaHpiResourceIdT resource_id,
                                  SaHpiTextBufferT *tag)
{
        SaErrorT rv = SA_OK;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;
        SaHpiBoolT valid_tag = SAHPI_TRUE;

        if (tag == NULL || oh_handler == NULL) {
                err("Invalid parameters tag/oh_handler");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Validate the tag */
        valid_tag = oh_valid_textbuffer(tag);
        if (valid_tag == SAHPI_FALSE) {
                err("The tag = '%s' is not in correct format for resource "
					"id %d", (char*)tag, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler =(struct oh_handler_state *) oh_handler;
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("There is no resource for resource id %d", resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* Copy the tag to the resource tag */
        rv = oh_copy_textbuffer(&(rpt->ResourceTag), tag);
        if (rv != SA_OK) {
                err("Copying textbuffer failed for resource id %d",
							resource_id);
                return rv;
        }

        return SA_OK;
}

/**
 * ov_rest_set_resource_severity:
 *      @oh_handler: Handler data pointer.
 *      @id:         Resource Id.
 *      @severity:   Resource severity.
 *
 * Purpose:
 *      Sets severity of the resource.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *      SA_ERR_HPI_NOT_PRESENT    - resource does not exist.
 **/
SaErrorT ov_rest_set_resource_severity(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiSeverityT severity)
{
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter oh_handler");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Validate the severity */
        if (oh_lookup_severity(severity) == NULL) {
                err("Invalid severity %d for the resource id %d",
			severity, resource_id);
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        handler = (struct oh_handler_state *) oh_handler;
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("There is no resource for resource id %d",resource_id);
                return SA_ERR_HPI_NOT_PRESENT;
        }

        rpt->ResourceSeverity = severity;
        return SA_OK;
}

/**
 * ov_rest_control_parm:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @action:      Configuration action.
 *
 * Purpose:
 *      Save and restore saved configuration parameters.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current ov_rest implementation does not
 *                                   support this API.
 **/
SaErrorT ov_rest_control_parm(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiParmActionT action)
{
        err("The ov_rest control parm is not supported");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

void * oh_open (void *)
                __attribute__ ((weak, alias("ov_rest_open")));
void * oh_close (void *)
                __attribute__ ((weak, alias("ov_rest_close")));

void * oh_set_resource_tag (void *,
                            SaHpiResourceIdT,
                            SaHpiTextBufferT *)
                __attribute__ ((weak, alias("ov_rest_set_resource_tag")));

void * oh_set_resource_severity (void *,
                                 SaHpiResourceIdT,
                                 SaHpiSeverityT)
                __attribute__ ((weak, alias("ov_rest_set_resource_severity")));

void * oh_control_parm (void *,
                        SaHpiResourceIdT,
                        SaHpiParmActionT)
                __attribute__ ((weak, alias("ov_rest_control_parm")));

