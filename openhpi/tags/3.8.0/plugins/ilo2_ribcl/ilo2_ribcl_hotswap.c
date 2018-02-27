/*
 * Copyright (C) 2007-2015, Hewlett-Packard Development Company, LLP
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
 * Neither the name of the Hewlett-Packard Corporation, nor the names
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
 *      Praveen Kumar praveen.kumar15@hp.com
 *      Mohan Devarajulu mohan@fc.hp.com
 *
 * This file handles all the hotswap related event handling
 *
 *      ilo2_ribcl_get_hotswap_state()             - gets the hotswap state of the
 *                                                resource
 *
 *      ilo2_ribcl_set_hotswap_state()             - sets the hotswap state of the
 *                                                resource
 *
 *      ilo2_ribcl_get_indicator_state()           - gets the hotswap LED indicator
 *                                                state of the resource
 *
 *      ilo2_ribcl_set_indicator_state()           - sets the hotswap LED indicator
 *                                                state of the resource
 *
 *      ilo2_ribcl_request_hotswap_action()        - requests the hotswap action
 *
 *      ilo2_ribcl_hotswap_policy_cancel()         - requests hotswap policy cancel
 *
 *      ilo2_ribcl_get_autoinsert_timeout()        - gets the auto insert event
 *                                                Timeout period
 *
 *      ilo2_ribcl_set_autoinsert_timeout()        - sets the auto insert event
 *                                                Timeout period
 *
 *      ilo2_ribcl_get_autoextract_timeout()       - gets the auto extract event
 *                                                Timeout period
 *
 *      ilo2_ribcl_set_autoextract_timeout()       - sets the auto extract event
 *                                                Timeout period
 **/

#include "ilo2_ribcl_hotswap.h"

/**
 * ilo2_ribcl_get_hotswap_state
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @state:       Hotswap state
 *
 * Purpose:
 *      Gets the hotswap state of the resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                       - on success.
 *      SA_ERR_HPI_INVALID_PARAMS   - on wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource id
 *      SA_ERR_HPI_CAPABILITY       - on not having hotswap capability
 *      SA_ERR_HPI_INTERNAL_ERROR   - on failure.
 **/
SaErrorT ilo2_ribcl_get_hotswap_state(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiHsStateT *state)
{
        struct ilo2_ribcl_hotswap_state *hotswap_state = NULL;
        struct oh_handler_state *handler = NULL;
        SaHpiRptEntryT *rpt = NULL;

        if (oh_handler == NULL || state == NULL) {
                err("Invalid parameters");
                return(SA_ERR_HPI_INVALID_PARAMS);
        }

        handler = (struct oh_handler_state *) oh_handler;
        rpt = oh_get_resource_by_id(handler->rptcache, resource_id);
        if (rpt == NULL) {
                err("failed to get rpt entry");
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_FRU)) {
                err("Resource does not have FRU capability");
                return SA_ERR_HPI_CAPABILITY;
        }

        /* For FAN, PS etc give the state based on ResouceFailed
           The standards changed to include all FRUs */
        if (! (rpt->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)) {
                if(!(rpt->ResourceFailed)) {
                          *state = SAHPI_HS_STATE_ACTIVE;
                } else {
                          *state = SAHPI_HS_STATE_INACTIVE;
                }
         } else {

                /* Get the hotswap structure of MANAGED_HOTSWAP */
                hotswap_state = (struct ilo2_ribcl_hotswap_state *)
                         oh_get_resource_data(handler->rptcache, resource_id);
                if (hotswap_state == NULL) {
                         err("Unable to get the resource private data");
                         return SA_ERR_HPI_INVALID_RESOURCE;
                }

                *state  = hotswap_state->currentHsState;
        }
        if ( *state == SAHPI_HS_STATE_NOT_PRESENT) {
                /* We can never have any resouce information in RPT
                    with * NOT_PRESENT hotswap state Ideally, this
                    code should never gets executed */
                 return  SA_ERR_HPI_INVALID_RESOURCE;
        }

        return SA_OK;
}

/**
 * ilo2_ribcl_set_hotswap_state
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @state:       Hotswap state
 *
 * Purpose:
 *      Sets the hotswap state of the resource
 *
 * Detailed Description:
 *      Currently, the OA plug-in does not stay in the InsertionPending or
 *      ExtractionPending states.  Because of this, the ActiveSet() and
 *      InactiveSet() will always be an invalid request, as per the HPI
 *      specification.
 *
 *      As it turns out, the current infrastructure code does not even call
 *      this plug-in routine.  However, if it's ever called, we need to be
 *      returning the correct values.
 *
 * Return value:
 *      SA_ERR_HPI_INVALID_REQUEST  - We're not in one of the pending states
 **/
SaErrorT ilo2_ribcl_set_hotswap_state(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiHsStateT state)
{
        return SA_ERR_HPI_INVALID_REQUEST;
}


/**
 * ilo2_ribcl_get_indicator_state
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @state:       Hotswap state
 *
 * Purpose:
 *      Gets the hotswap LED indicator state of the resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                       - on success.
 *      SA_ERR_HPI_INVALID_PARAMS   - on wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource id
 *      SA_ERR_HPI_CAPABILITY       - on not having hotswap capability
 *      SA_ERR_HPI_INTERNAL_ERROR   - on failure.
 **/
SaErrorT ilo2_ribcl_get_indicator_state(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiHsIndicatorStateT *state)
{
        err("ilo2_ribcl_get_indicator_state not supported");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ilo2_ribcl_set_indicator_state
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @state:       Hotswap state
 *
 * Purpose:
 *      Sets the hotswap LED indicator state of the resource
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                       - on success.
 *      SA_ERR_HPI_INVALID_PARAMS   - on wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource id
 *      SA_ERR_HPI_CAPABILITY       - on not having hotswap capability
 *      SA_ERR_HPI_INTERNAL_ERROR   - on failure.
 **/
SaErrorT ilo2_ribcl_set_indicator_state(void *oh_handler,
                                     SaHpiResourceIdT resource_id,
                                     SaHpiHsIndicatorStateT state)
{
        err("ilo2_ribcl_set_indicator_state not supported");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ilo2_ribcl_request_hotswap_action
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @action:      Hotswap action
 *
 * Purpose:
 *      Requests the hotswap action
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                       - on success.
 *      SA_ERR_HPI_INVALID_PARAMS   - on wrong parameters
 *      SA_ERR_HPI_INVALID_RESOURCE - on invalid resource id
 *      SA_ERR_HPI_CAPABILITY       - on not having hotswap capability
 *      SA_ERR_HPI_INTERNAL_ERROR   - on failure.
 **/
SaErrorT ilo2_ribcl_request_hotswap_action(void *oh_handler,
                                        SaHpiResourceIdT resource_id,
                                        SaHpiHsActionT action)
{
        return SA_ERR_HPI_INVALID_REQUEST;
}

/**
 * ilo2_ribcl_hotswap_policy_cancel
 *      @oh_handler:  Pointer to openhpi handler structure
 *      @resource_id: Resource id
 *      @tm:          Timeout value
 *
 * Purpose:
 *      Requests hotswap policy cancel
 *
 * Detailed Description:
 *      Currently, the OA plug-in does not stay in the InsertionPending or
 *      ExtractionPending states.  Because of this, the policy_cancel request
 *      will always be an invalid request, as per the HPI specification.
 *
 *      As it turns out, the current infrastructure code does not even call
 *      this plug-in routine.  However, if it's ever called, we need to be
 *      returning the correct values.
 *
 * Return value:
 *      SA_ERR_HPI_INVALID_REQUEST  - We're not in one of the pending states
 **/
SaErrorT ilo2_ribcl_hotswap_policy_cancel(void *oh_handler,
                                       SaHpiResourceIdT resource_id,
                                       SaHpiTimeoutT timeout)
{
        return SA_ERR_HPI_INVALID_REQUEST;
}

/**
 * ilo2_ribcl_get_autoinsert_timeout:
 *      @oh_handler:  Handler data pointer.
 *      @timeout:     Timeout to set.
 *
 * Purpose:
 *      Get hotswap autoinsert timeout.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT ilo2_ribcl_get_autoinsert_timeout(void *oh_handler,
                                        SaHpiTimeoutT *timeout)
{
        dbg("ilo2_ribcl_get_autoinsert_timeout is not supported");
        dbg("Default auto insert timeout is IMMEDIATE and read only");
        *timeout=SAHPI_TIMEOUT_IMMEDIATE;
        return SA_OK;
}

/**
 * ilo2_ribcl_set_autoinsert_timeout:
 *      @oh_handler: Handler data pointer.
 *      @timeout:    Timeout to set.
 *
 * Purpose:
 *      Set hotswap autoinsert timeout.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_READ_ONLY      - auto-insert timeout is fixed
 *      SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT ilo2_ribcl_set_autoinsert_timeout(void *oh_handler,
                                        SaHpiTimeoutT timeout)
{
        err("ilo2_ribcl_set_autoinsert_timeout setting timeout not supported");
        err("Default auto insert timeout is IMMEDIATE and read only");
        return SA_ERR_HPI_READ_ONLY;
}

/**
 * ilo2_ribcl_get_autoextract_timeout:
 *      @oh_handler:  Handler data pointer.
 *      @resource_id: Resource ID.
 *      @timeout:     Timeout value.
 *
 * Purpose:
 *      Get a resource's hotswap autoextract timeout.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - Normal case.
 *      SAHPI_TIMEOUT_IMMEDIATE   - autonomous handling is immediate
 *      SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT ilo2_ribcl_get_autoextract_timeout(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiTimeoutT *timeout)
{
        dbg("ilo2_ribcl_get_autoextract_timeout is not supported");
        dbg("Default auto extract timeout is IMMEDIATE and read only");
        *timeout=SAHPI_TIMEOUT_IMMEDIATE;
        return SA_OK;
}

/**
 * ilo2_ribcl_set_autoextract_timeout:
 *      @oh_handler:  Handler data pointer.
 *      @resource_id: Resource ID.
 *      @timeout:     Timeout to set.
 *
 * Purpose:
 *      Set a resource hotswap autoextract timeout.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success
 *      SA_ERR_HPI_READ_ONLY      - auto-insert timeout is fixed
 *      SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT ilo2_ribcl_set_autoextract_timeout(void *oh_handler,
                                         SaHpiResourceIdT resource_id,
                                         SaHpiTimeoutT timeout)
{
        err("ilo2_ribcl_set_autoextract_timeout is not supported");
        err("Default auto extract timeout is IMMEDIATE and read only");
        return SA_ERR_HPI_READ_ONLY;
}

void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("ilo2_ribcl_get_hotswap_state")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("ilo2_ribcl_set_hotswap_state")));

void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("ilo2_ribcl_request_hotswap_action")));

void * oh_hotswap_policy_cancel (void *, SaHpiResourceIdT, SaHpiTimeoutT)
                __attribute__ ((weak, alias("ilo2_ribcl_hotswap_policy_cancel")));

void * oh_get_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT *)
                __attribute__ ((weak, alias("ilo2_ribcl_get_indicator_state")));

void * oh_set_indicator_state (void *, SaHpiResourceIdT, SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("ilo2_ribcl_set_indicator_state")));

void * oh_get_autoinsert_timeout (void *, SaHpiResourceIdT, SaHpiTimeoutT)
                __attribute__ ((weak, alias("ilo2_ribcl_get_autoinsert_timeout")));

void * oh_set_autoinsert_timeout (void *, SaHpiTimeoutT)
                __attribute__ ((weak, alias("ilo2_ribcl_set_autoinsert_timeout")));

void * oh_get_autoextract_timeout (void *, SaHpiResourceIdT, SaHpiTimeoutT *)
                __attribute__ ((weak,
                               alias("ilo2_ribcl_get_autoextract_timeout")));

void * oh_set_autoextract_timeout (void *, SaHpiResourceIdT, SaHpiTimeoutT)
                __attribute__ ((weak,
                               alias("ilo2_ribcl_set_autoextract_timeout")));

