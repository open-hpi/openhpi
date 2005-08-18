/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      W. David Ashley <dashley@us.ibm.com>
 *
 */


#include <sim_init.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <sim_injector_ext.h>


static char *find_value(char *name, char *buf);
static void process_sensor_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_sensor_enable_change_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_hot_swap_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_watchdog_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_sw_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_oem_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_user_event_msg(SIM_MSG_QUEUE_BUF *buf);
static void process_resource_event_msg(SIM_MSG_QUEUE_BUF *buf);


static struct oh_event *eventdup(const struct oh_event *event)
{
        struct oh_event *e;
        e = g_malloc0(sizeof(*e));
        if (!e) {
                dbg("Out of memory!");
                return NULL;
        }
        memcpy(e, event, sizeof(*e));
        return e;
}



static SaErrorT sim_create_resourcetag(SaHpiTextBufferT *buffer, const char *str, SaHpiEntityLocationT loc)
{
	char *locstr;
	SaErrorT err = SA_OK;
	SaHpiTextBufferT working;

	if (!buffer || loc < SIM_HPI_LOCATION_BASE ||
	    loc > (pow(10, OH_MAX_LOCATION_DIGITS) - 1)) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_textbuffer(&working);
	if (err) { return(err); }

	locstr = (gchar *)g_malloc0(OH_MAX_LOCATION_DIGITS + 1);
	if (locstr == NULL) {
		dbg("Out of memory.");
		return(SA_ERR_HPI_OUT_OF_SPACE);
	}
	snprintf(locstr, OH_MAX_LOCATION_DIGITS + 1, " %d", loc);

	if (str) { oh_append_textbuffer(&working, str); }
	err = oh_append_textbuffer(&working, locstr);
	if (!err) {
		err = oh_copy_textbuffer(buffer, &working);
	}
	g_free(locstr);
	return(err);
}


/* return a handler state pointer by looking for its handler_name */
struct oh_handler_state *sim_get_handler_by_name(char *name)
{
        struct oh_handler_state *state = NULL;
        int i = 0;
        char *handler_name;
        state = (struct oh_handler_state *)g_slist_nth_data(sim_handler_states, i);
        while (state != NULL) {
                handler_name = (char *)g_hash_table_lookup(state->config,
                                                           "handler_name");
                if (strcmp(handler_name, name) == 0) {
                        return state;
                }
                i++;
                state = (struct oh_handler_state *)g_slist_nth_data(sim_handler_states, i);
        }

        return NULL;
}







/* inject a resource */
// assumptions about the input SaHpiRptEntryT *data entry
// - all fields are assumed to have valid values except
//    o EntryId (filled in by oh_add_resource function)
//    o ResourceId
//    o ResourceEntity (assumed to have only partial data)
SaErrorT sim_inject_resource(struct oh_handler_state *state,
                             SaHpiRptEntryT *data, void *privdata,
                             const char * comment) {
	SaHpiEntityPathT root_ep;
	SaHpiRptEntryT *res;
	char *entity_root;
	struct oh_event event;
        struct simResourceInfo *privinfo;
        SaErrorT rc;

        /* check arguments */
        if (state == NULL || data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* get the entity root */
	entity_root = (char *)g_hash_table_lookup(state->config,"entity_root");
	oh_encode_entitypath (entity_root, &root_ep);

        /* set up the rpt entry */
        res = g_malloc(sizeof(SaHpiRptEntryT));
        if (res == NULL) {
                dbg("Out of memory in build_rptcache\n");
                return SA_ERR_HPI_OUT_OF_MEMORY;
        }
        memcpy(res, data, sizeof(SaHpiRptEntryT));
        oh_concat_ep(&res->ResourceEntity, &root_ep);
        res->ResourceId = oh_uid_from_entity_path(&res->ResourceEntity);
        sim_create_resourcetag(&res->ResourceTag, comment,
                               root_ep.Entry[0].EntityLocation);

        /* set up our private data store for resource state info */
        if (!privdata) {
                privinfo = (struct simResourceInfo *)g_malloc0(sizeof(struct simResourceInfo));
                if (privinfo == NULL) {
                        dbg("Out of memory in build_rptcache\n");
                        return SA_ERR_HPI_OUT_OF_MEMORY;
                }
                if (res->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
                        privinfo->cur_hsstate = SAHPI_HS_STATE_ACTIVE;
                }
                privdata = (void *)privinfo;
        }

        /* perform the injection */
        dbg("Injecting ResourceId %d", res->ResourceId);
        rc = oh_add_resource(state->rptcache, res, privdata, FREE_RPT_DATA);
        if (rc) {
                dbg("Error %d injecting ResourceId %d", rc, res->ResourceId);
                return rc;
        }

        /* make sure the caller knows what resiurce ID was actually assigned */
        data->ResourceId = res->ResourceId;

        /* now add an event for the resource add */
        memset(&event, 0, sizeof(event));
        event.type = OH_ET_RESOURCE;
        event.did = oh_get_default_domain_id();
        memcpy(&event.u.res_event.entry, res, sizeof(SaHpiRptEntryT));
        sim_inject_event(state, eventdup(&event));

        return SA_OK;
}


/* inject an rdr */
// assumptions about the input SaHpiRdrT *data entry
// - all fields are assumed to have valid values
// - no checking of the data is performed
// assuptions about the input *privdata entry
// - no checking of the data is performed
SaErrorT sim_inject_rdr(struct oh_handler_state *state, SaHpiResourceIdT resid,
                        SaHpiRdrT *rdr, void * privinfo) {
	struct oh_event event;
        SaErrorT rc;

        /* check arguments */
        if (state == NULL || resid == 0 || rdr == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        dbg("Injecting rdr for ResourceId %d", resid);
	rc = oh_add_rdr(state->rptcache, resid, rdr, privinfo, 0);
        if (rc) {
                dbg("Error %d injecting rdr for ResourceId %d", rc, resid);
                return rc;
        }

        /* now inject an event for the rdr */
        memset(&event, 0, sizeof(event));
        event.type = OH_ET_RDR;
        event.u.rdr_event.parent = resid;
        memcpy(&event.u.rdr_event.rdr, rdr, sizeof(SaHpiRdrT));
        sim_inject_event(state, eventdup(&event));

        return SA_OK;
}


/* inject an event */
// assumptions about the input oh_event *data entry
// - all fields are assumed to have valid values
// - no checking of the data is performed
SaErrorT sim_inject_event(struct oh_handler_state *state, struct oh_event *data) {

        /* check arguments */
        if (state== NULL || data == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* perform the injection */
        dbg("Injecting event");
	g_async_queue_push(state->eventq_async, data);
        return SA_OK;
}


/*--------------------------------------------------------------------*/
/* Function: service_thread                                           */
/*--------------------------------------------------------------------*/

static gpointer injector_service_thread(gpointer data) {
        key_t ipckey;
        int msgqueid = -1;
        SIM_MSG_QUEUE_BUF buf;
        int n;

        /* construct the queue */
        ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
        msgqueid = msgget(ipckey, IPC_CREAT | 0660);
        if (msgqueid == -1) {
                return NULL;
        }

        /* get message loop */
        while (TRUE) {
                n = msgrcv(msgqueid, &buf, SIM_MSG_QUEUE_BUFSIZE, 0, 0);
                if (n == -1) {
                    dbg("error during msgrcv");
                    dbg("errorno = %d", errno);
                    // invalid message, just ignore it
                    continue;
                }
                switch (buf.mtype) {
                case SIM_MSG_SENSOR_EVENT:
                    dbg("processing sensor event");
                    process_sensor_event_msg(&buf);
                    break;
                case SIM_MSG_SENSOR_ENABLE_CHANGE_EVENT:
                    dbg("processing sensor enable change event");
                    process_sensor_enable_change_event_msg(&buf);
                    break;
                case SIM_MSG_HOT_SWAP_EVENT:
                    dbg("processing hot swap event");
                    process_hot_swap_event_msg(&buf);
                    break;
                case SIM_MSG_WATCHDOG_EVENT:
                    dbg("processing watchdog event");
                    process_watchdog_event_msg(&buf);
                    break;
                case SIM_MSG_SW_EVENT:
                    dbg("processing sw event");
                    process_sw_event_msg(&buf);
                    break;
                case SIM_MSG_OEM_EVENT:
                    dbg("processing oem event");
                    process_oem_event_msg(&buf);
                    break;
                case SIM_MSG_USER_EVENT:
                    dbg("processing user event");
                    process_user_event_msg(&buf);
                    break;
                case SIM_MSG_RESOURCE_EVENT:
                    dbg("processing resource event");
                    process_resource_event_msg(&buf);
                    break;
                default:
                    dbg("invalid msg recieved");
                    break;
                }
        }
        g_thread_exit(NULL);
}


static SaHpiBoolT thrd_running = FALSE;


/*--------------------------------------------------------------------*/
/* Function: start_injector_service_thread                            */
/*--------------------------------------------------------------------*/

GThread *start_injector_service_thread(gpointer data) {
        GThread *service_thrd;

        /* if thread already running then quit */
        if (thrd_running) {
                return NULL;
        }

        /* start the thread */
        service_thrd = g_thread_create(injector_service_thread, NULL,
                                       FALSE, NULL);
        if (service_thrd != NULL) {
                thrd_running = TRUE;
        }
        return service_thrd;
}


/*--------------------------------------------------------------------*/
/* Function: find_value                                               */
/*--------------------------------------------------------------------*/

static char *find_value(char *name, char *buf) {
        char *value = NULL;
        int len;

        while (*buf != '\0') {
                len = strlen(name);
                if (strncmp(buf, name, len) == 0 && *(buf + len) == '=') {
                        value = buf + len + 1;
                        return value;
                }
                buf += strlen(buf) + 1;
        }
        return value;
}


/*--------------------------------------------------------------------*/
/* Function: process_sensor_event_msg                                 */
/*--------------------------------------------------------------------*/

static void process_sensor_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_SENSOR;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiSensorEventT part of the structure */
        /* get the sensor number */
        value = find_value(SIM_MSG_SENSOR_NUM, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_NUM");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.SensorNum =
         (SaHpiSensorNumT)atoi(value);

        /* get the sensor type */
        value = find_value(SIM_MSG_SENSOR_TYPE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_TYPE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.SensorType =
         (SaHpiSensorTypeT)atoi(value);

        /* get the sensor event category */
        value = find_value(SIM_MSG_SENSOR_EVENT_CATEGORY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_EVENT_CATEGORY");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.EventCategory =
         (SaHpiEventCategoryT)atoi(value);

        /* get the sensor assertion */
        value = find_value(SIM_MSG_SENSOR_ASSERTION, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_ASSERTION");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.Assertion =
         (SaHpiBoolT)atoi(value);

        /* get the sensor event state */
        value = find_value(SIM_MSG_SENSOR_EVENT_STATE, buf->mtext);
        if (value == NULL) {
            dbg("invalid SIM_MSG_SENSOR_EVENT_STATE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.EventState =
         (SaHpiEventStateT)atoi(value);

        /* get the sensor event optional data */
        value = find_value(SIM_MSG_SENSOR_OPTIONAL_DATA, buf->mtext);
        if (value != NULL) {
            ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent =
             (SaHpiSensorOptionalDataT)atoi(value);

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_TRIGGER_READING) {
                value = find_value(SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.IsSupported =
                 (SaHpiBoolT)atoi(value);
                value = find_value(SIM_MSG_SENSOR_TRIGGER_READING_TYPE, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_READING_TYPE");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Type =
                 (SaHpiSensorReadingTypeT)atoi(value);
                value = find_value(SIM_MSG_SENSOR_TRIGGER_READING, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_READING");
                        return;
                }
                switch (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Type) {
                case SAHPI_SENSOR_READING_TYPE_INT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Value.SensorInt64 =
                     strtoll(value, NULL, 10);
                    break;
                case SAHPI_SENSOR_READING_TYPE_UINT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Value.SensorInt64 =
                     strtoull(value, NULL, 10);
                    break;
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Value.SensorFloat64 =
                     strtod(value, NULL);
                    break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                    strncpy(ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerReading.Value.SensorBuffer,
                     value, strlen(value));
                    break;
                default:
                    return;
                }
            }

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_TRIGGER_THRESHOLD) {
                value = find_value(SIM_MSG_SENSOR_TRIGGER_THRESHOLD_SUPPORTED, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_THRESHOLD_SUPPORTED");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.IsSupported =
                 (SaHpiBoolT)atoi(value);
                value = find_value(SIM_MSG_SENSOR_TRIGGER_THRESHOLD_TYPE, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_THRESHOLD_TYPE");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Type =
                 (SaHpiSensorReadingTypeT)atoi(value);
                value = find_value(SIM_MSG_SENSOR_TRIGGER_THRESHOLD, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_TRIGGER_THRESHOLD");
                        return;
                }
                switch (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Type) {
                case SAHPI_SENSOR_READING_TYPE_INT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Value.SensorInt64 =
                     strtoll(value, NULL, 10);
                    break;
                case SAHPI_SENSOR_READING_TYPE_UINT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Value.SensorInt64 =
                     strtoull(value, NULL, 10);
                    break;
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                    ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Value.SensorFloat64 =
                     strtod(value, NULL);
                    break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                    strncpy(ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.TriggerThreshold.Value.SensorBuffer,
                     value, strlen(value));
                    break;
                default:
                    return;
                }
            }

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_PREVIOUS_STATE) {
                value = find_value(SIM_MSG_SENSOR_PREVIOUS_STATE, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_PREVIOUS_STATE");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.PreviousState =
                 (SaHpiEventStateT)atoi(value);
            }

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_CURRENT_STATE) {
                value = find_value(SIM_MSG_SENSOR_CURRENT_STATE, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_CURRENT_STATE");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.CurrentState =
                 (SaHpiEventStateT)atoi(value);
            }

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_OEM) {
                value = find_value(SIM_MSG_SENSOR_OEM, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_OEM");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.Oem =
                 (SaHpiEventStateT)atoi(value);
            }

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.OptionalDataPresent
             & SAHPI_SOD_SENSOR_SPECIFIC) {
                value = find_value(SIM_MSG_SENSOR_SPECIFIC, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_SPECIFIC");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.SensorSpecific =
                 (SaHpiEventStateT)atoi(value);
            }
        }

        /* now fill out the RPT entry and the RDR */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        if (rc) {
                saHpiSessionClose(sid);
                return;
        }
        rc = saHpiRdrGetByInstrumentId(sid, ohevent.u.hpi_event.event.Source,
                                       SAHPI_SENSOR_RDR,
                                       ohevent.u.hpi_event.event.EventDataUnion.SensorEvent.SensorNum,
                                       &ohevent.u.hpi_event.rdr);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_sensor_enable_change_event_msg                   */
/*--------------------------------------------------------------------*/

static void process_sensor_enable_change_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_SENSOR_ENABLE_CHANGE;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiSensorEnableChangeEventT part of the structure */
        /* get the sensor number */
        value = find_value(SIM_MSG_SENSOR_NUM, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_NUM");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.SensorNum =
         (SaHpiSensorNumT)atoi(value);

        /* get the sensor type */
        value = find_value(SIM_MSG_SENSOR_TYPE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_TYPE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.SensorType =
         (SaHpiSensorTypeT)atoi(value);

        /* get the sensor event category */
        value = find_value(SIM_MSG_SENSOR_EVENT_CATEGORY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_EVENT_CATEGORY");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.EventCategory =
         (SaHpiEventCategoryT)atoi(value);

        /* get the sensor enable */
        value = find_value(SIM_MSG_SENSOR_ENABLE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SENSOR_ENABLE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.SensorEnable =
         (SaHpiBoolT)atoi(value);

        /* get the sensor event enable */
        value = find_value(SIM_MSG_SENSOR_EVENT_ENABLE, buf->mtext);
        if (value == NULL) {
            dbg("invalid SIM_MSG_SENSOR_EVENT_ENABLE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.SensorEventEnable =
         (SaHpiBoolT)atoi(value);

        /* get the sensor assert event mask */
        value = find_value(SIM_MSG_SENSOR_ASSERT_EVENT_MASK, buf->mtext);
        if (value == NULL) {
            dbg("invalid SIM_MSG_SENSOR_ASSERT_EVENT_MASK");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.AssertEventMask =
         (SaHpiEventStateT)atoi(value);

        /* get the sensor deassert event mask */
        value = find_value(SIM_MSG_SENSOR_DEASSERT_EVENT_MASK, buf->mtext);
        if (value == NULL) {
            dbg("invalid SIM_MSG_SENSOR_DEASSERT_EVENT_MASK");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.DeassertEventMask =
         (SaHpiEventStateT)atoi(value);

        /* get the sensor event optional data */
        value = find_value(SIM_MSG_SENSOR_OPTIONAL_DATA, buf->mtext);
        if (value != NULL) {
            ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.OptionalDataPresent =
             (SaHpiSensorOptionalDataT)atoi(value);

            if (ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.OptionalDataPresent
             & SAHPI_SOD_CURRENT_STATE) {
                value = find_value(SIM_MSG_SENSOR_CURRENT_STATE, buf->mtext);
                if (value == NULL) {
                    dbg("invalid SIM_MSG_SENSOR_CURRENT_STATE");
                        return;
                }
                ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.CurrentState =
                 (SaHpiEventStateT)atoi(value);
            }
        }

        /* now fill out the RPT entry and the RDR */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        if (rc) {
                saHpiSessionClose(sid);
                return;
        }
        rc = saHpiRdrGetByInstrumentId(sid, ohevent.u.hpi_event.event.Source,
                                       SAHPI_SENSOR_RDR,
                                       ohevent.u.hpi_event.event.EventDataUnion.SensorEnableChangeEvent.SensorNum,
                                       &ohevent.u.hpi_event.rdr);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_hot_swap_event_msg                               */
/*--------------------------------------------------------------------*/

static void process_hot_swap_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_HOTSWAP;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiHotSwapEventT part of the structure */
        /* get the hot swap state */
        value = find_value(SIM_MSG_HS_STATE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HS_STATE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.HotSwapEvent.HotSwapState =
         (SaHpiHsStateT)atoi(value);

        /* get the hot swap state */
        value = find_value(SIM_MSG_HS_PREVIOUS_STATE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HS_PREVIOUS_STATE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.HotSwapEvent.PreviousHotSwapState =
         (SaHpiHsStateT)atoi(value);

        /* now fill out the RPT entry  */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                dbg("cannot open session");
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        if (rc) {
                saHpiSessionClose(sid);
                dbg("cannot get RPT entry");
                return;
        }
        saHpiSessionClose(sid);

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_watchdog_event_msg                               */
/*--------------------------------------------------------------------*/

static void process_watchdog_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_WATCHDOG;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiWatchdogEventT part of the structure */
        /* get the watchdog number */
        value = find_value(SIM_MSG_WATCHDOG_NUM, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_WATCHDOG_NUM");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.WatchdogEvent.WatchdogNum =
         (SaHpiWatchdogNumT)atoi(value);

        /* get the watchdog action */
        value = find_value(SIM_MSG_WATCHDOG_ACTION, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_WATCHDOG_ACTION");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.WatchdogEvent.WatchdogAction =
         (SaHpiWatchdogActionEventT)atoi(value);

        /* get the watchdog pretimer action */
        value = find_value(SIM_MSG_WATCHDOG_PRETIMER_ACTION, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_WATCHDOG_PRETIMER_ACTION");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.WatchdogEvent.WatchdogPreTimerAction =
         (SaHpiWatchdogPretimerInterruptT)atoi(value);

        /* get the watchdog timer use */
        value = find_value(SIM_MSG_WATCHDOG_TIMER_USE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_WATCHDOG_TIMER_USE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.WatchdogEvent.WatchdogUse =
         (SaHpiWatchdogTimerUseT)atoi(value);

        /* now fill out the RPT entry and the RDR */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        if (rc) {
                saHpiSessionClose(sid);
                return;
        }
        rc = saHpiRdrGetByInstrumentId(sid, ohevent.u.hpi_event.event.Source,
                                       SAHPI_WATCHDOG_RDR,
                                       ohevent.u.hpi_event.event.EventDataUnion.WatchdogEvent.WatchdogNum,
                                       &ohevent.u.hpi_event.rdr);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_sw_event_msg                                     */
/*--------------------------------------------------------------------*/

static void process_sw_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_HPI_SW;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiSwEventT part of the structure */
        /* get the sw mid */
        value = find_value(SIM_MSG_SW_MID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SW_MID");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.MId =
         (SaHpiManufacturerIdT)atoi(value);

        /* get the sw event tyep */
        value = find_value(SIM_MSG_SW_EVENT_TYPE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SW_EVENT_TYPE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.Type =
         (SaHpiSwEventTypeT)atoi(value);

        /* get the sw event data */
        value = find_value(SIM_MSG_SW_EVENT_DATA, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_SW_EVENT_DATA");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.DataType =
         SAHPI_TL_TYPE_TEXT;
        ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Language =
         SAHPI_LANG_ENGLISH;
        ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.DataLength =
         strlen(value);
        strncpy(ohevent.u.hpi_event.event.EventDataUnion.HpiSwEvent.EventData.Data,
                value, strlen(value));

        /* now fill out the RPT entry */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_oem_event_msg                                    */
/*--------------------------------------------------------------------*/

static void process_oem_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_OEM;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiOemEventT part of the structure */
        /* get the oem mid */
        value = find_value(SIM_MSG_OEM_MID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_OEM_MID");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.OemEvent.MId =
         (SaHpiManufacturerIdT)atoi(value);

        /* get the oem event data */
        value = find_value(SIM_MSG_OEM_EVENT_DATA, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_OEM_EVENT_DATA");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.OemEvent.OemEventData.DataType =
         SAHPI_TL_TYPE_TEXT;
        ohevent.u.hpi_event.event.EventDataUnion.OemEvent.OemEventData.Language =
         SAHPI_LANG_ENGLISH;
        ohevent.u.hpi_event.event.EventDataUnion.OemEvent.OemEventData.DataLength =
         strlen(value);
        strncpy(ohevent.u.hpi_event.event.EventDataUnion.OemEvent.OemEventData.Data,
                value, strlen(value));

        /* now fill out the RPT entry */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_user_event_msg                                   */
/*--------------------------------------------------------------------*/

static void process_user_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_USER;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiUserEventT part of the structure */
        /* get the user event data */
        value = find_value(SIM_MSG_USER_EVENT_DATA, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_USER_EVENT_DATA");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.UserEvent.UserEventData.DataType =
         SAHPI_TL_TYPE_TEXT;
        ohevent.u.hpi_event.event.EventDataUnion.UserEvent.UserEventData.Language =
         SAHPI_LANG_ENGLISH;
        ohevent.u.hpi_event.event.EventDataUnion.UserEvent.UserEventData.DataLength =
         strlen(value);
        strncpy(ohevent.u.hpi_event.event.EventDataUnion.UserEvent.UserEventData.Data,
                value, strlen(value));

        /* now fill out the RPT entry */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}


/*--------------------------------------------------------------------*/
/* Function: process_resource_event_msg                               */
/*--------------------------------------------------------------------*/

static void process_resource_event_msg(SIM_MSG_QUEUE_BUF *buf) {
        struct oh_handler_state *state;
        struct oh_event ohevent;
        char *value;

        memset(&ohevent, sizeof(struct oh_event), 0);
        ohevent.did = oh_get_default_domain_id();
        ohevent.type = OH_ET_HPI;

        /* get the handler state */
        value = find_value(SIM_MSG_HANDLER_NAME, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }
        state = sim_get_handler_by_name(value);
        if (state == NULL) {
                dbg("invalid SIM_MSG_HANDLER_NAME");
                return;
        }

        /* get the resource id */
        value = find_value(SIM_MSG_RESOURCE_ID, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_ID");
                return;
        }
        ohevent.u.hpi_event.event.Source = (SaHpiResourceIdT)atoi(value);

        /* set the event type */
        ohevent.u.hpi_event.event.EventType = SAHPI_ET_RESOURCE;

        /* get the event timestamp */
        oh_gettimeofday(&ohevent.u.hpi_event.event.Timestamp);

        /* get the severity */
        value = find_value(SIM_MSG_EVENT_SEVERITY, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_EVENT_SEVERITY");
                return;
        }
        ohevent.u.hpi_event.event.Severity = (SaHpiSeverityT)atoi(value);

        /* fill out the SaHpiResourceEventT part of the structure */
        /* get the resource event type */
        value = find_value(SIM_MSG_RESOURCE_EVENT_TYPE, buf->mtext);
        if (value == NULL) {
                dbg("invalid SIM_MSG_RESOURCE_EVENT_TYPE");
                return;
        }
        ohevent.u.hpi_event.event.EventDataUnion.ResourceEvent.ResourceEventType =
         atoi(value);

        /* now fill out the RPT entry */
        SaHpiSessionIdT sid;
        SaErrorT rc = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sid, NULL);
        if (rc) {
                return;
        }
        rc = saHpiRptEntryGetByResourceId(sid, ohevent.u.hpi_event.event.Source,
                                          &ohevent.u.hpi_event.res);
        saHpiSessionClose(sid);
        if (rc) {
                return;
        }

        /* now inject the event */
        sim_inject_event(state, &ohevent);

        return;
}





