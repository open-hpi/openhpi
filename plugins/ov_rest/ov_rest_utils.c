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
 * This file implements all the utility functions which will be useful of ov
 * REST functioning. Majority of the functions are helper functions for
 * different modules.
 *
 *      ov_rest_update_hs_event()       - Updates the Hotswap event structure
 *
 *      copy_ov_rest_event()            - Copies the event data from the event
 *                                        received from ov into the
 *                                        allocated event structure
 *
 *      push_event_to_queue()           - Pushes events into the infrastructure
 *                                        event queue
 *
 *      del_rdr_from_event()            - Delete RDRs from rdrlist of the
 *                                        event structure
 *
 *      lock_ov_rest_handler()          - Tries to lock the ov_hander mutex.
 *                                        If mutex is already locked earlier,
 *                                        returns error
 *
 *      ov_rest_check_config_parameters()-Checks whether all the parameters are
 *                                        present in the config file
 *
 *      ov_rest_delete_all_inv_info()    - Frees the memory allocated for
 *                                        inventory areas for all the resources
 *
 *      ov_rest_clean_rptable()        - Frees the memory allocated for
 *                                        plugin RPTable
 *
 *      release_ov_rest_resources()     - Frees the memory allocated for
 *                                        resources presence matrix and serial
 *                                        number array
 *
 *      ov_rest_lower_to_upper          - Converts the lower case to upper case
 *
 **/

#include "ov_rest_utils.h"
#include "sahpi_wrappers.h"


/**
 * ov_rest_update_hs_event
 *      @event:      Pointer to openhpi event structure
 *      @oh_handler: Pointer to the openhpi handler
 *
 * Purpose:
 *      this api updates the event structure with hotswap details
 *
 * Detailed Description: NA
 *
 * Return values:
 *      NONE
 **/
void ov_rest_update_hs_event(struct oh_handler_state *oh_handler,
                          struct oh_event *event)
{
        if (oh_handler == NULL || event == NULL) {
                err("Invalid parameters");
                return;
        }

        memset(event, 0, sizeof(struct oh_event));
        event->hid = oh_handler->hid;
        event->event.EventType = SAHPI_ET_HOTSWAP;
        /* FIXME: map the timestamp of the OV generated event */
        oh_gettimeofday(&(event->event.Timestamp));
        event->event.Severity = SAHPI_CRITICAL;
}

/**
 * copy_ov_rest_event
 *      @event: Pointer to the openhpi event structure
 *
 * Purpose:
 *      makes a copy of the received event
 *
 * Detailed Description:
 *      - Allocates the memory to new event structure.
 *      - Copies the hpi event to the newly created event structure
 *      - Returns the newly created event structure
 *
 * Return values:
 *      Pointer to copied oh_event structure - on success
 *      NULL                                 - if wrong parameters passed
 *                                           - if the memory allocation failed.
 **/
struct oh_event *copy_ov_rest_event(struct oh_event *event)
{
        struct oh_event *e = NULL;

        if (event == NULL) {
                err("Invalid parameter");
                return NULL;
        }

        e = (struct oh_event *)g_malloc0(sizeof(struct oh_event));
        if (e == NULL) {
                err("Out of memory!");
                return NULL;
        }
        memcpy(e, event, sizeof(struct oh_event));
        return e;
}

/**
 * lock_ov_rest_handler
 *      @ov_hander: Pointer to the ov_hander
 *
 * Purpose:
 *      Tries to lock the ov_hander mutex. If mutex is already locked earlier,
 *      returns error
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT lock_ov_rest_handler(struct ov_rest_handler *ov_handler)
{
        gboolean lock_state = TRUE;

        if (ov_handler == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Try to lock the ov_hander mutex */
        lock_state = wrap_g_mutex_trylock(ov_handler->ov_mutex);
        if (lock_state == FALSE) {
                err("OV REST Handler is locked.");
                err("No operation is allowed in this state");
                err("Please try after some time");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Unlock the ov_hander mutex */
        wrap_g_mutex_unlock(ov_handler->ov_mutex);
        return SA_OK;
}

/**
 * ov_rest_check_config_parameters
 *      @handler_config: Pointer to the config handler
 *
 * Purpose:
 *      Checks whether all the parameters are present in the config file
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on invalid parameters.
 *      SA_ERR_HPI_INTERNAL_ERROR - on failure.
 **/
SaErrorT ov_rest_check_config_parameters(GHashTable *handler_config)
{
        char *temp = NULL;

        if (handler_config == NULL) {
                err("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* Check for entity_root entry */
        temp = (char *)g_hash_table_lookup(handler_config, "entity_root");
        if (temp == NULL) {
                err("entity_root is missing in the config file.");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for OV user name entry */
        temp = (char *) g_hash_table_lookup(handler_config, "OV_User_Name");
        if (temp == NULL) {
                err("OV_User_Name is missing in the config file.");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for OV_Password entry */
        temp = (char *) g_hash_table_lookup(handler_config, "OV_Password");
        if (temp == NULL) {
                err("OV_Password is missing in the config file.");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        /* Check for Active OV hostname/IP address entry
         */
        temp = (char *) g_hash_table_lookup(handler_config, "ACTIVE_OV");
        if (temp == NULL) {
                err("ACTIVE_OV is missing in the config file.");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        return SA_OK;
}

/**
 * ov_rest_delete_all_inv_info
 *      @oh_handler: Pointer to the plugin handler
 *
 * Purpose:
 *      Traverses through all resources and extracts the inventory RDR
 *      Frees up the memory allocated for inventory information
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_OK                     - on success.
 *      SA_ERR_HPI_INVALID_PARAMS - on wrong parameters
 **/
SaErrorT ov_rest_delete_all_inv_info(struct oh_handler_state *oh_handler)
{
        SaErrorT rv = SA_OK;
        SaHpiRptEntryT  *rpt = NULL;

        if (oh_handler == NULL) {
                err("Invalid parameter");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        rpt = oh_get_resource_next(oh_handler->rptcache, SAHPI_FIRST_ENTRY);
        while (rpt) {
                if (rpt->ResourceCapabilities 
                    & SAHPI_CAPABILITY_INVENTORY_DATA) {
                        /* Free the inventory info from inventory RDR */
                        rv = ov_rest_free_inventory_info(oh_handler, 
					rpt->ResourceId);
                        if (rv != SA_OK) 
                                err("Inventory cleanup failed for resource %d",
                                    rpt->ResourceId);
                }
                /* Get the next resource */
                rpt = oh_get_resource_next(oh_handler->rptcache,
                                           rpt->ResourceId);
        }

        return SA_OK;
}

/**
 * ov_rest_clean_rptable
 *      @oh_handler: Pointer to the plugin handler
 *
 * Purpose:
 *      Frees up the memory allocated for RPT and RDR entries
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      NONE
 **/
void ov_rest_clean_rptable(struct oh_handler_state *oh_handler)
{
	SaErrorT rv = SA_OK;
	struct ov_rest_handler *ov_handler = NULL;
	struct enclosureStatus *enclosure = NULL, *temp = NULL;
	REST_CON *con = NULL;

	if (oh_handler == NULL) {
		err("Invalid parameter");
		return;
	}

	ov_handler = (struct ov_rest_handler *) oh_handler->data;
	if(ov_handler == NULL) {
		err("Plugin handler not present");
		return;
	}
	con = (REST_CON *) ov_handler->connection;
	if(con != NULL){
		if(con->url){
			wrap_free(con->url);
		}
	}

	rv = ov_rest_delete_all_inv_info(oh_handler);
	if (rv != SA_OK) {
		err("Deleting all inventory information failed");
	}

	enclosure = ov_handler->ov_rest_resources.enclosure;
	while(enclosure != NULL){
		temp = enclosure->next;
		release_ov_rest_resources(enclosure);
		enclosure = temp;
	}

	ov_handler->ov_rest_resources.enclosure = NULL;
	rv = oh_flush_rpt(oh_handler->rptcache);
	if (rv != SA_OK) {
		err("Plugin RPTable flush failed");
	}

	remove(ov_handler->cert_t.fSslKey);
	remove(ov_handler->cert_t.fSslCert);
	remove(ov_handler->cert_t.fCaRoot);

	return;
}

/*
 * release_ov_rest_resources
 *      @enclosure: Pointer to ov rest handler
 *
 * Purpose:
 *      To free the memory allocated for resource presence and serial
 *      number
 *      for OV, interconnect, server, fan and power supply
 *
 * Detailed Description: NA
 *
 * Return values:
 *      None
 **/
void release_ov_rest_resources(struct enclosureStatus *enclosure)
{
	SaHpiInt32T i = 0;;

	/* Release memory of blade presence, resource id and blade
	 * serial number arrays
	 */
	if(enclosure->serialNumber != NULL){
		wrap_g_free(enclosure->serialNumber);
	}
	if(enclosure->composer.presence != NULL){
		wrap_g_free(enclosure->composer.presence);
	}
	if(enclosure->composer.type){
		wrap_g_free(enclosure->composer.type);
	}
	if(enclosure->composer.resource_id != NULL){
		wrap_g_free(enclosure->composer.resource_id);
	}
	if(enclosure->composer.serialNumber != NULL){
		for(i = 0; i < enclosure->composer.max_bays; i++){
			if(enclosure->composer.serialNumber[i] !=
					NULL){
				wrap_g_free(enclosure->composer.
						serialNumber[i]);
			}
		}
		wrap_g_free(enclosure->composer.serialNumber);
	}
	if (enclosure->server.type != NULL) {
		wrap_g_free(enclosure->server.type);
	}
	if (enclosure->server.resource_id != NULL) {
		wrap_g_free(enclosure->server.resource_id);
	}
	if(enclosure->server.serialNumber != NULL) {
		for (i = 0; i < enclosure->server.max_bays; i++)
		{
			if (enclosure->server.serialNumber[i] !=
					NULL) {
				wrap_g_free(enclosure->server.
						serialNumber[i]);
			}
		}
		wrap_g_free(enclosure->server.serialNumber);
	}

	/* Release memory of interconnect presence and serial number array */
	if (enclosure->interconnect.presence != NULL) {
		wrap_g_free(enclosure->interconnect.presence);
	}
	if (enclosure->interconnect.type != NULL) {
		wrap_g_free(enclosure->interconnect.type);
	}
	if (enclosure->interconnect.resource_id != NULL) {
		wrap_g_free(enclosure->interconnect.resource_id);
	}
	if(enclosure->interconnect.serialNumber != NULL) {
		for (i = 0; i < enclosure->interconnect.max_bays;
				i++) {
			if (enclosure->interconnect.
					serialNumber[i] != NULL) {
				wrap_g_free(enclosure->interconnect.
						serialNumber[i]);
			}
		}
		wrap_g_free(enclosure->interconnect.serialNumber);
	}

	/* Release memory of fan presence and serial number array */
	if (enclosure->fan.presence != NULL) {
		wrap_g_free(enclosure->fan.presence);
	}
	if (enclosure->fan.type != NULL) {
		wrap_g_free(enclosure->fan.type);
	}
	if (enclosure->fan.resource_id != NULL) {
		wrap_g_free(enclosure->fan.resource_id);
	}
	if(enclosure->fan.serialNumber != NULL){
		for(i =0 ; i< enclosure->fan.max_bays; i++){
			if (enclosure->fan.serialNumber != NULL) {
				wrap_g_free(enclosure->fan.serialNumber[i]);
			}
		}
		wrap_g_free(enclosure->fan.serialNumber);
	}


	/* Release memory of power supply presence and serial number array */
	if (enclosure->ps_unit.presence !=NULL) {
		wrap_g_free(enclosure->ps_unit.presence);
	}
	if (enclosure->ps_unit.type !=NULL) {
		wrap_g_free(enclosure->ps_unit.type);
	}
	if (enclosure->ps_unit.resource_id !=NULL) {
		wrap_g_free(enclosure->ps_unit.resource_id);
	}
	if(enclosure->ps_unit.serialNumber != NULL) {
		for (i = 0; i < enclosure->ps_unit.max_bays; i++)
		{
			if (enclosure->ps_unit.serialNumber[i]
					!= NULL) {
				wrap_g_free(enclosure->
						ps_unit.serialNumber[i]);
			}
		}
		wrap_g_free(enclosure->ps_unit.serialNumber);
	}
	wrap_g_free(enclosure);
}

/* FIXME: Do we want to move this function to general utils? */
/**
 * ov_rest_lower_to_upper
 *      @src:      Pointer to the source string handler
 *      @src_len:  String length of the source string
 *      @dest:     Pointer to destination string
 *      @dest_len: Length of the destination string
 *
 * Purpose:
 *      Converts the lower case characters to upper case
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_HPI_ERR_INAVLID_PARAMS - on invalid parametersfailure
 *      SA_HPI_ERR_INTERNAL_ERROR - on failure
 *      SA_OK                     - on success
 **/
SaErrorT ov_rest_lower_to_upper(char *src,
                                SaHpiInt32T src_len,
                                char *dest,
                                SaHpiInt32T dest_len)
{
        SaHpiInt32T i = 0;

        if (src == NULL || dest == NULL) {
                dbg("Invalid parameters");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (dest_len < src_len) {
                err("Source string is longer than destination string");
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

        memset(dest, 0, dest_len);
        for (i = 0; i < src_len; i++)
                dest[i] = toupper(src[i]);
	dest[i] = '\0';

        return SA_OK;
}

/**
 * ov_rest_update_resource_status()
 *      @res_status     pointer to resource_info_t
 *      @index          index into the resource info fields in res_status
 *      @serialNumber  serialNumber string to be copied into res_status
 *      @resource_id    resource id to be updated to res_status
 *      @presence       presence status
 *
 *      Description:
 *      This routine updates the resource status entry with passed in
 *      serialNumber, resource_id, and presence.  This routine should be
 *      called to set and reset the resource status fields that change
 *      when a a resource gets added and removed.
 *
 *      Return value: none
**/
void ov_rest_update_resource_status(resource_info_t *res_status,
                                    SaHpiInt32T index,
                                    char *serialNumber,
                                    SaHpiResourceIdT resource_id,
                                    resource_presence_t presence,
                                    resourceCategory_t type)
{
        if (index <= 0) {
                err("Invalid index value %d - returning without update\n",
                    index);
                return;
        }
        if ((serialNumber != NULL) && (serialNumber[0] != '\0')) {
                size_t len;
                len = strlen(serialNumber);
                strncpy(res_status->serialNumber[index-1], serialNumber,len);
                res_status->serialNumber[index-1][len] = '\0';
        }
        res_status->resource_id[index-1] = resource_id;
        res_status->presence[index-1] = presence;
	res_status->type[index-1] = type;

        return;
}

/**
 * ov_rest_trim_whitespace:
 *      @s: Pointer to source and destination string.
 *
 * Purpose:
 *      Trims the given string.
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      Character pointer.
 **/
char * ov_rest_trim_whitespace(char *s)
{
  int i, len = strlen(s);

  for (i=(len-1); i>=0; i--) {
    if ((s[i] == ' ') || (s[i] == '\t')) {
      s[i] = 0;
    }
    else
      break;
  }
  return(s);
}

/**
 * itostr:
 *      @x: Input variable of integer type.
 *      @s: Pointer to destination string.
 *
 * Purpose:
 *      Converts integer to string.
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      None.
 **/
void itostr(int x,char **s)
{
	char c=0x0;
	char buf[33];
	int sign=0;
	int i, end, top;
	end = top = 32;
	if(x<0){
		x*=-1;
		sign=1;
	}
	for(i = x;i!=0;i/=10 ){
		c |=0x03;
		c <<= 4;
		c |= i%10;
		buf[top--]=c;
		c=0x0;
	}
	if(sign)
		buf[0]='-';
	for(i=sign; top < end;i++)
		buf[i]= buf[++top];
	buf[i]='\0';
	*s =(char *)malloc((i+1)*sizeof(char));
	strcpy(*s,buf);
	return;
}

/**
 * get_url_from_idr:
 *      @handler:     Pointer to openhpi handler.
 *      @resource_id: Resource id.
 *      @url:         Pointer to resorce url.
 *
 * Purpose:
 *      Gets the url from idr.
 *
 * Detailed Descrption: NA
 *
 * Return values:
 *      SA_OK                     - On Success.
 *      SA_HPI_ERR_INTERNAL_ERROR - On Failure.
 **/
SaErrorT get_url_from_idr(struct oh_handler_state *handler,
			SaHpiResourceIdT resource_id, char ** url)
{
	struct ov_rest_inventory *idr = NULL;
	struct ovRestField **f = NULL;

	idr = (struct ov_rest_inventory*) oh_get_rdr_data(handler->rptcache,
							resource_id, 0);
	if (idr == NULL) {
		err("No idr data for resource = %d\n", resource_id);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	if (idr) {
		if (idr->info.area_list){
			f = &idr->info.area_list[0].field_list;

			while (*f && (*f)->next_field)
				f = &(*f)->next_field;
			
			WRAP_ASPRINTF(url,"%s",(char*)&(*f)->field.Field.Data[6]);
		}
	}

	return SA_OK;
}
