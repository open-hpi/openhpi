/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors: David Judkovics
 *     
 *     
 *     
 */

#include "openhpid.h"

void display_id_string(SaHpiTextBufferT string);
const char * rdrtype2str(SaHpiRdrTypeT type);
const char * type2string(SaHpiEntityTypeT type);
const char * get_error_string(SaErrorT error);
const char * severity2str(SaHpiSeverityT severity);




/********************************************************************/
/* init_openhpid                                                    */
/********************************************************************/
int init_openhpid(void) 
{
        SaErrorT err;

        dbg("\nsaHpiInitialize\n");
        err = saHpiInitialize(&version);
        if (SA_OK != err) {
                dbg("saHpiInitialize Failed: %d", err);
                exit(-1);
        }

        /* every domain requires a new session */
        dbg("\nsaHpiInitialize\n");
        err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
        if (SA_OK != err) {
                dbg("saHpiSessionOpen Failed: %d", err);
                return err;
        }

        return 0;
    
}

/********************************************************************/
/* close_openhpid                                                   */
/********************************************************************/
int close_openhpid(void) 
{
        SaErrorT err;

        dbg("\nsaHpiFinalize\n");
        err = saHpiFinalize();
        if (SA_OK != err) {
                dbg("saHpiFinalize Failed: %d", err);
                exit(-1);
        }
        return 0;
    
}

/********************************************************************/
/* open_msg                                                         */
/********************************************************************/
int open_msg(char * readbuffer, int sockfd) 
{

        SaErrorT rval;

        OPEN_MSG_STR *open_msg_ptr;

        OPEN_MSG_STR open_msg_response = {.header.msg_type = OPEN, 
                                          .header.msg_length = sizeof(OPEN_MSG_STR),
                                          .text_message = "New and Imporved OPEN response"};

       open_msg_ptr = (OPEN_MSG_STR *)readbuffer;

       dbg("OPEN text message: %s\n", open_msg_ptr->text_message);

       if ((rval = send_msg((char *)&open_msg_response, sizeof(OPEN_MSG_STR), sockfd)) < 0) 
               dbg("open_msg Failed: %d", rval);
    
       return(rval);
}


/********************************************************************/
/* discover_resources_msg                                                         */
/********************************************************************/
int discover_resources_msg(char * readbuffer, int sockfd) 
{
	GSList *event_list_ptr = NULL;
	GSList *event_list_member_ptr = NULL;
	guint len;

	int i;

	struct oh_event *oh_event_ptr;

        DISCOVER_RESOURCES_MSG_STR *discover_resources_msg_ptr = NULL;

        err = saHpiResourcesDiscover(session_id);
        if (SA_OK != err) {
                dbg("saHpiResourcesDiscover Failed:%d", err);
		return(-1);
        }
        else {
		/* this call adds not only resource events but also events for the rdrs */
		event_list_ptr = resource_list_add(event_list_ptr);
		if (event_list_ptr == (GSList *)-1) {
			dbg("BAD things have happened building event list");
			return(-1);
		}

/* we need to do much more than this here,  it needs to realloc enough memroy for the entire messsage,
build the message from the event slist,  free the slist memory, send message and free the message memory 
*/
		len = g_slist_length(event_list_ptr);
dbg("discover_resources_msg: event list current length %d", len);
		i = sizeof(DISCOVER_RESOURCES_MSG_STR) + (len * sizeof(struct oh_event));
dbg("discover_resources_msg: message current length %d", i);
		discover_resources_msg_ptr = (DISCOVER_RESOURCES_MSG_STR *)malloc(i);

		if (!discover_resources_msg_ptr) {
			dbg("couldn't malloc mem for response message");
			return(-1);
		}

		discover_resources_msg_ptr->header.msg_type = DISCOVER_RESOURCES;
		discover_resources_msg_ptr->header.msg_length = i;
		discover_resources_msg_ptr->num_resource_events = len;

		oh_event_ptr = &discover_resources_msg_ptr->event;

		event_list_member_ptr = g_slist_next(event_list_ptr);

		while(event_list_member_ptr != NULL) {
			memcpy(oh_event_ptr, event_list_member_ptr->data, sizeof(struct oh_event));
			event_list_member_ptr = g_slist_next(event_list_member_ptr);
			oh_event_ptr++;
		}
		discover_resources_msg_ptr->error = SA_OK;
        }

        send_msg((char *)discover_resources_msg_ptr, discover_resources_msg_ptr->header.msg_length, sockfd);

        return(SA_OK);
}
GSList *resource_list_add(GSList *event_list_ptr) 
{
	struct oh_event *oh_event_ptr = NULL;

        SaHpiEntryIdT   current;
        SaHpiEntryIdT   next = SAHPI_FIRST_ENTRY;
        SaHpiRptEntryT  entry;

        SaHpiRptInfoT   rpt_info;

guint len;
    
        /* from here the number of RPT entries are found and it is not necessary to have a variable spin loop */
        err = saHpiRptInfoGet(session_id, &rpt_info);
        if (SA_OK != err) {
                dbg("saHpiRptInfoGet Failed: %d", err);
                return( (GSList *)-1 );
        }
        do {
                current = next;
                err = saHpiRptEntryGet(session_id, current, &next, &entry);
                if (SA_OK != err) {
                        if (current != SAHPI_FIRST_ENTRY) {
                                dbg("saHpiRptEntryGet Failed: %d", err);     
                                return( (GSList *)-1 );
                        } else {
                                dbg("Empty RPT\n");
                                return(event_list_ptr);
                        }
                }

		/* add resource event to event list */
		oh_event_ptr = NULL;
		oh_event_ptr = (struct oh_event *)malloc(sizeof(struct oh_event));
		if (!oh_event_ptr) 
			return( (GSList *)-1 );
		oh_event_ptr->type = OH_ET_RESOURCE;
		memcpy(&oh_event_ptr->u.res_event.entry, &entry, sizeof(SaHpiRptEntryT));
		event_list_ptr = g_slist_append(event_list_ptr, oh_event_ptr);

len = g_slist_length(event_list_ptr);
dbg("resource_list_add: event list current length %d", len);


		/* generate list of rdr's if any for this resouce */
		if (entry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
			event_list_ptr = rdr_list_add(entry.ResourceId, event_list_ptr);  

        } while ( next != SAHPI_LAST_ENTRY );

len = g_slist_length(event_list_ptr);
dbg("resource_list_add: right before return, event list current length %d", len);

        return(event_list_ptr);
}


GSList *rdr_list_add(SaHpiResourceIdT resource_id, GSList *event_list_ptr)
{
        SaErrorT        err;
	SaHpiEntryIdT	current_rdr;
	SaHpiEntryIdT	next_rdr;
	SaHpiRdrT	rdr;

	printf("RDR Info:\n");
	next_rdr = SAHPI_FIRST_ENTRY;
	do {
		int i;
		current_rdr = next_rdr;
		err = saHpiRdrGet(session_id, resource_id, current_rdr, 
				&next_rdr, &rdr);
		if (SA_OK != err) {
			if (current_rdr == SAHPI_FIRST_ENTRY) {
				printf("************************ BAD Empty RDR table\n");
				return(event_list_ptr);
			}
			else {
				printf("************************ BAD saHpiRdrGet");
				return(event_list_ptr);
			}
		}
		
		printf("\tRecordId: %x\n", rdr.RecordId);
		printf("\tRdrType: %s\n", rdrtype2str(rdr.RdrType));
		printf("\tEntity: \n");
		for ( i=0; i<SAHPI_MAX_ENTITY_PATH; i++)
		{
			SaHpiEntityT tmp = rdr.Entity.Entry[i];
			if (tmp.EntityType <= SAHPI_ENT_UNSPECIFIED)
				break;
				printf("\t\t{%s, %i}\n", 
				type2string(tmp.EntityType),
				tmp.EntityInstance);
		}
		printf("\tIdString: ");
	       	display_id_string(rdr.IdString);
	}while(next_rdr != SAHPI_LAST_ENTRY);

	return(event_list_ptr);
}

const char * severity2str(SaHpiSeverityT severity)
{
	switch (severity) {
	case SAHPI_CRITICAL:
		return "CRITICAL";
	case SAHPI_MAJOR:
		return "MAJOR";
	case SAHPI_MINOR:
		return "MINOR";
	case SAHPI_INFORMATIONAL:
		return "INFORMATIONAL";	       
	case SAHPI_OK:
		return "OK";
	case SAHPI_DEBUG:
		return "DEBUG";
	default:
		return "UNKNOWN SEVERITY";
	}
}

const char * get_error_string(SaErrorT error)
{
	switch(error) {
		case SA_ERR_HPI_ERROR:
			return "SA_ERR_HPI_ERROR";
		case SA_ERR_HPI_UNSUPPORTED_API:
			return "SA_ERR_UNSUPPORTED_API";
		case SA_ERR_HPI_BUSY:
			return "SA_ERR_HPI_BUSY";
		case SA_ERR_HPI_INVALID:
			return "SA_ERR_HPI_INVALID";
		case SA_ERR_HPI_INVALID_CMD:
			return "SA_ERR_HPI_INVALID_CMD";
		case SA_ERR_HPI_TIMEOUT:
			return "SA_ERR_HPI_TIMEOUT";
		case SA_ERR_HPI_OUT_OF_SPACE:
			return "SA_ERR_HPI_OUT_OF_SPACE";
		case SA_ERR_HPI_DATA_TRUNCATED:
			return "SA_ERR_HPI_DATA_TRUNCATED";
		case SA_ERR_HPI_DATA_LEN_INVALID:
			return "SA_ERR_HPI_DATA_LEN_INVALID";
		case SA_ERR_HPI_DATA_EX_LIMITS:
			return "SA_ERR_HPI_DATA_EX_LIMITS";
		case SA_ERR_HPI_INVALID_PARAMS:
			return "SA_ERR_HPI_INVALID_PARAMS";
		case SA_ERR_HPI_INVALID_DATA:
			return "SA_ERR_HPI_INVALID_DATA";
		case SA_ERR_HPI_NOT_PRESENT:
			return "SA_ERR_HPI_NOT_PRESENT";
		case SA_ERR_HPI_INVALID_DATA_FIELD:
			return "SA_ERR_HPI_INVALID_DATA_FIELD";
		case SA_ERR_HPI_INVALID_SENSOR_CMD:
			return "SA_ERR_HPI_INVALID_SENSOR_CMD";
		case SA_ERR_HPI_NO_RESPONSE:
			return "SA_ERR_HPI_NO_RESPONSE";
		case SA_ERR_HPI_DUPLICATE:
			return "SA_ERR_HPI_DUPLICATE";
		case SA_ERR_HPI_INITIALIZING:
			return "SA_ERR_HPI_INITIALIZING";
		case SA_ERR_HPI_UNKNOWN:
			return "SA_ERR_HPI_UNKNOWN";
		case SA_ERR_HPI_INVALID_SESSION:
			return "SA_ERR_HPI_INVALID_SESSION";
		case SA_ERR_HPI_INVALID_RESOURCE:
			return "SA_ERR_HPI_INVALID_RESOURCE";
		case SA_ERR_HPI_INVALID_REQUEST:
			return "SA_ERR_HPI_INVALID_REQUEST";
		case SA_ERR_HPI_ENTITY_NOT_PRESENT:
			return "SA_ERR_HPI_ENTITY_NOT_PRESENT";
		case SA_ERR_HPI_UNINITIALIZED:
			return "SA_ERR_HPI_UNINITIALIZED";
		default:
			return "(invalid error code)";
	}
}

const char * type2string(SaHpiEntityTypeT type)
{
	switch(type) {
	case SAHPI_ENT_UNSPECIFIED:
		return "SAHPI_ENT_UNSPECIFIED";
	case SAHPI_ENT_OTHER:
		return "SAHPI_ENT_OTHER";
	case SAHPI_ENT_UNKNOWN:
		return "SAHPI_ENT_UNKNOWN";
	case SAHPI_ENT_PROCESSOR:
		return "SAHPI_ENT_PROCESSOR";
	case SAHPI_ENT_DISK_BAY:
		return "SAHPI_ENT_DISK_BAY";
	case SAHPI_ENT_PERIPHERAL_BAY:
		return "SAHPI_ENT_PERIPHERAL_BAY";
	case SAHPI_ENT_SYS_MGMNT_MODULE:
		return "SAHPI_ENT_SYS_MGMNT_MODULE";
	case SAHPI_ENT_SYSTEM_BOARD:
		return "SAHPI_ENT_SYSTEM_BOARD";
	case SAHPI_ENT_MEMORY_MODULE:
		return "SAHPI_ENT_MEMORY_MODULE";
	case SAHPI_ENT_PROCESSOR_MODULE:
		return "SAHPI_ENT_PROCESSOR_MODULE";
	case SAHPI_ENT_POWER_SUPPLY:
		return "SAHPI_ENT_POWER_SUPPLY";
	case SAHPI_ENT_ADD_IN_CARD:
		return "SAHPI_ENT_ADD_IN_CARD";
	case SAHPI_ENT_FRONT_PANEL_BOARD:
		return "SAHPI_ENT_FRONT_PANEL_BOARD";
	case SAHPI_ENT_BACK_PANEL_BOARD:
		return "SAHPI_ENT_BACK_PANEL_BOARD";
	case SAHPI_ENT_POWER_SYSTEM_BOARD:
		return "SAHPI_ENT_POWER_SYSTEM_BOARD";
	case SAHPI_ENT_DRIVE_BACKPLANE:
		return "SAHPI_ENT_DRIVE_BACKPLANE";
	case SAHPI_ENT_SYS_EXPANSION_BOARD:
		return "SAHPI_ENT_SYS_EXPANSION_BOARD";
	case SAHPI_ENT_OTHER_SYSTEM_BOARD:
		return "SAHPI_ENT_OTHER_SYSTEM_BOARD";
	case SAHPI_ENT_PROCESSOR_BOARD:
		return "SAHPI_ENT_PROCESSOR_BOARD";
	case SAHPI_ENT_POWER_UNIT:
		return "SAHPI_ENT_POWER_UNIT";
	case SAHPI_ENT_POWER_MODULE:
		return "SAHPI_ENT_POWER_MODULE";
	case SAHPI_ENT_POWER_MGMNT:
		return "SAHPI_ENT_POWER_MGMNT";
	case SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD:
		return "SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD";
	case SAHPI_ENT_SYSTEM_CHASSIS:
		return "SAHPI_ENT_SYSTEM_CHASSIS";
	case SAHPI_ENT_SUB_CHASSIS:
		return "SAHPI_ENT_SUB_CHASSIS";
	case SAHPI_ENT_OTHER_CHASSIS_BOARD:
		return "SAHPI_ENT_OTHER_CHASSIS_BOARD";
	case SAHPI_ENT_DISK_DRIVE_BAY:
		return "SAHPI_ENT_DISK_DRIVE_BAY";
	case SAHPI_ENT_PERIPHERAL_BAY_2:
		return "SAHPI_ENT_PERIPHERAL_BAY_2";
	case SAHPI_ENT_DEVICE_BAY:
		return "SAHPI_ENT_DEVICE_BAY";
	case SAHPI_ENT_COOLING_DEVICE:
		return "SAHPI_ENT_COOLING_DEVICE";
	case SAHPI_ENT_COOLING_UNIT:
		return "SAHPI_ENT_COOLING_UNIT";
	case SAHPI_ENT_INTERCONNECT:
		return "SAHPI_ENT_INTERCONNECT";
	case SAHPI_ENT_MEMORY_DEVICE:
		return "SAHPI_ENT_MEMORY_DEVICE";
	case SAHPI_ENT_SYS_MGMNT_SOFTWARE:
		return "SAHPI_ENT_SYS_MGMNT_SOFTWARE";
	case SAHPI_ENT_BIOS:
		return "SAHPI_ENT_BIOS";
	case SAHPI_ENT_OPERATING_SYSTEM:
		return "SAHPI_ENT_OPERATING_SYSTEM";
	case SAHPI_ENT_SYSTEM_BUS:
		return "SAHPI_ENT_SYSTEM_BUS";
	case SAHPI_ENT_GROUP:
		return "SAHPI_ENT_GROUP";
	case SAHPI_ENT_REMOTE:
		return "SAHPI_ENT_REMOTE";
	case SAHPI_ENT_EXTERNAL_ENVIRONMENT:
		return "SAHPI_ENT_EXTERNAL_ENVIRONMENT";
	case SAHPI_ENT_BATTERY:
		return "SAHPI_ENT_BATTERY";
	case SAHPI_ENT_CHASSIS_SPECIFIC:
		return "SAHPI_ENT_CHASSIS_SPECIFIC";
	case SAHPI_ENT_BOARD_SET_SPECIFIC:
		return "SAHPI_ENT_BOARD_SET_SPECIFIC";
	case SAHPI_ENT_OEM_SYSINT_SPECIFIC:
		return "SAHPI_ENT_OEM_SYSINT_SPECIFIC";
	case SAHPI_ENT_ROOT:
		return "SAHPI_ENT_ROOT";
	case SAHPI_ENT_RACK:
		return "SAHPI_ENT_RACK";
	case SAHPI_ENT_SUBRACK:
		return "SAHPI_ENT_SUBRACK";
	case SAHPI_ENT_COMPACTPCI_CHASSIS:
		return "SAHPI_ENT_COMPACTPCI_CHASSIS";
	case SAHPI_ENT_ADVANCEDTCA_CHASSIS:
		return "SAHPI_ENT_ADVANCEDTCA_CHASSIS";
	case SAHPI_ENT_SYSTEM_SLOT:
		return "SAHPI_ENT_SYSTEM_SLOT";
	case SAHPI_ENT_SBC_BLADE:
		return "SAHPI_ENT_SBC_BLADE";
	case SAHPI_ENT_IO_BLADE:
		return "SAHPI_ENT_IO_BLADE";
	case SAHPI_ENT_DISK_BLADE:
		return "SAHPI_ENT_DISK_BLADE";
	case SAHPI_ENT_DISK_DRIVE:
		return "SAHPI_ENT_DISK_DRIVE";
	case SAHPI_ENT_FAN:
		return "SAHPI_ENT_FAN";
	case SAHPI_ENT_POWER_DISTRIBUTION_UNIT:
		return "SAHPI_ENT_POWER_DISTRIBUTION_UNIT";
	case SAHPI_ENT_SPEC_PROC_BLADE:
		return "SAHPI_ENT_SPEC_PROC_BLADE";
	case SAHPI_ENT_IO_SUBBOARD:
		return "SAHPI_ENT_IO_SUBBOARD";
	case SAHPI_ENT_SBC_SUBBOARD:
		return "SAHPI_ENT_SBC_SUBBOARD";
	case SAHPI_ENT_ALARM_MANAGER:
		return "SAHPI_ENT_ALARM_MANAGER";
	case SAHPI_ENT_ALARM_MANAGER_BLADE:
		return "SAHPI_ENT_ALARM_MANAGER_BLADE";
	case SAHPI_ENT_SUBBOARD_CARRIER_BLADE:
		return "SAHPI_ENT_SUBBOARD_CARRIER_BLADE";
	default:
		return "(invalid entity type)";
	}
	return "\0";
}

const char * rdrtype2str(SaHpiRdrTypeT type)
{
	switch (type) {
	case SAHPI_NO_RECORD:
		return "SAHPI_NO_RECORD";
	case SAHPI_CTRL_RDR:
		return "SAHPI_CTRL_RDR";
	case SAHPI_SENSOR_RDR:
		return "SAHPI_SENSOR_RDR";
	case SAHPI_INVENTORY_RDR:
		return "SAHPI_INVENTORY_RDR";
	case SAHPI_WATCHDOG_RDR:
		return "SAHPI_WATCHDOG_RDR";
	default:
		return "(invalid rdr type)";
	}
	return "\0";
}

void display_id_string(SaHpiTextBufferT string)
{
	int i;
	switch(string.DataType) {
	case SAHPI_TL_TYPE_ASCII6:
		for (i = 0; i < string.DataLength; i++)
			printf("%c", string.Data[i]);
		break;
	default:
		printf("Unsupported string type");
	}
	printf("\n");
}

