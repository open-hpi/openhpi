/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * Copyright (c) 2003 by International Business Machines
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Sean Dague <sean@dague.net>
 *     Rusty Lynch
 */

#include <string.h>
#include <oh_config.h>
#include <openhpi.h>
#include <oh_plugin.h>
#include <SaHpi.h>

static enum {
	OH_STAT_UNINIT,
	OH_STAT_READY,
} oh_hpi_state = OH_STAT_UNINIT;
static const int entry_id_offset = 1000;
static struct oh_config config;

#define OH_STATE_READY_CHECK 					\
        do {							\
	            if (OH_STAT_READY!=oh_hpi_state) {		\
			dbg("Uninitialized HPI");		\
			return SA_ERR_HPI_UNINITIALIZED;	\
		}						\
	}while(0)						

#define OH_GET_RESOURCE						\
	do {							\
		struct oh_session *s;				\
								\
		OH_STATE_READY_CHECK;				\
								\
		s = session_get(SessionId);			\
		if (!s) {					\
			dbg("Invalid session");			\
			return SA_ERR_HPI_INVALID_SESSION;	\
		}						\
								\
		res = get_resource(ResourceId);			\
		if (!res) {					\
			dbg("Invalid resource");		\
			return SA_ERR_HPI_INVALID_RESOURCE;	\
		}						\
	}while(0)

static inline struct oh_rdr * get_rdr(
	struct oh_resource *res, 
	SaHpiRdrTypeT type, 
	SaHpiUint8T num)
{
	GSList *i;

	g_slist_for_each(i, res->rdr_list) {
		struct oh_rdr *rdr;
		rdr = i->data;
		
		if (rdr->rdr.RdrType != type)
			continue;
		
		switch (type) {
		case SAHPI_CTRL_RDR:
			if (rdr->rdr.RdrTypeUnion.CtrlRec.Num == num)
				return rdr;
			break;
		case SAHPI_SENSOR_RDR:
			if (rdr->rdr.RdrTypeUnion.SensorRec.Num == num)
				return rdr;
			break;
		case SAHPI_INVENTORY_RDR:
			if (rdr->rdr.RdrTypeUnion.InventoryRec.EirId == num)
				return rdr;
			break;
		case SAHPI_WATCHDOG_RDR:
			if (rdr->rdr.RdrTypeUnion.WatchdogRec.WatchdogNum == num)
				return rdr;
			break;
		case SAHPI_NO_RECORD:
		default:
			break;			
		}
	}

	return NULL;
}

SaErrorT SAHPI_API saHpiInitialize(SAHPI_OUT SaHpiVersionT *HpiImplVersion)
{
        if (OH_STAT_UNINIT != oh_hpi_state) {
                dbg("Cannot initialize twice");
                return SA_ERR_HPI_DUPLICATE;
        }
       	
	if (oh_load_config(&config)<0) {
		dbg("Can not load config");
                return SA_ERR_HPI_NOT_PRESENT;
	}
        
	oh_hpi_state = OH_STAT_READY;
        return SA_OK;
}

SaErrorT SAHPI_API saHpiFinalize(void)
{
        OH_STATE_READY_CHECK;	
        
	/*
          we should be doing handler shutdown here.
        */
        oh_hpi_state = OH_STAT_UNINIT;
	return SA_OK;
}

SaErrorT SAHPI_API saHpiSessionOpen(
		SAHPI_IN SaHpiDomainIdT DomainId,
		SAHPI_OUT SaHpiSessionIdT *SessionId,
		SAHPI_IN void *SecurityParams)
{
        struct oh_session *s;
        int rv;
        
        OH_STATE_READY_CHECK;
	
	if(!is_in_domain_list(DomainId)) {
		dbg("domain does not exist!");
		return SA_ERR_HPI_INVALID_DOMAIN;
	}
        
        rv = session_add(DomainId, &s);
        if(rv < 0) {
  		dbg("Out of space");
		return SA_ERR_HPI_OUT_OF_SPACE;
        }
        
        *SessionId = s->session_id;
	
	return SA_OK;
}


SaErrorT SAHPI_API saHpiSessionClose(SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
        
        OH_STATE_READY_CHECK;
        
        s = session_get(SessionId);
        if (!s) {
                dbg("Invalid session");
                return SA_ERR_HPI_INVALID_SESSION;
	}
        
        session_del(s);	
        return SA_OK;
}


SaErrorT SAHPI_API saHpiResourcesDiscover(SAHPI_IN SaHpiSessionIdT SessionId)
{
	struct oh_session *s;
	GSList *i;
	int rv =0;
	
	OH_STATE_READY_CHECK;

	s = session_get(SessionId);
	if (!s) {
		dbg("Invalid session");
		return SA_ERR_HPI_INVALID_SESSION;
	}
	
	g_slist_for_each(i, global_handler_list) {
		struct oh_handler *h = i->data;
		rv |= h->abi->discover_resources(h->hnd);
	}
	if (rv) {
		dbg("Error attempting to discover resource");	
		return SA_ERR_HPI_UNKNOWN;
	}
	
	rv = get_events();
	if (rv<0) {
		dbg("Error attempting to process resources");
		return SA_ERR_HPI_UNKNOWN;
	}

	return SA_OK;
}

SaErrorT SAHPI_API saHpiRptInfoGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiRptInfoT *RptInfo)
{
	OH_STATE_READY_CHECK;

	RptInfo->UpdateCount 	= global_rpt_counter;
	RptInfo->UpdateTimestamp= (SaHpiTimeT)global_rpt_timestamp.tv_sec*1000000000 + global_rpt_timestamp.tv_usec*1000;
	return SA_OK;
}

SaErrorT SAHPI_API saHpiRptEntryGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiEntryIdT EntryId,
		SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
	struct oh_session *s;
	
	int no;
	GSList *i;
	
	OH_STATE_READY_CHECK;

	s = session_get(SessionId);
	if (!s) {
		dbg("Invalid session");
		return SA_ERR_HPI_INVALID_SESSION;
	}
	
	switch (EntryId) {
	case SAHPI_FIRST_ENTRY:
		no = 0;
		break;
	default:
		no = EntryId - entry_id_offset;
		break;
	}
	
	if (no < 0 || no>=g_slist_length(global_rpt)) {
		dbg("Invalid EntryId");
		return SA_ERR_HPI_INVALID;
	}
	
	i = g_slist_nth(global_rpt, no);
	if (!resource_is_in_domain(i->data, s->domain_id)) {
		dbg("Invalid EntryId");
		return SA_ERR_HPI_INVALID;
	}
			
	memcpy(RptEntry, &((struct oh_resource *)(i->data))->entry, 
			sizeof(*RptEntry));
	
	i = g_slist_next(i);
	no++;
	g_slist_for_each(i, i) {
		if (resource_is_in_domain(i->data, s->domain_id)) 
			break;
		no++;
	}
	
	if (no<g_slist_length(global_rpt)) {
		*NextEntryId = no+entry_id_offset;
	} else {
		*NextEntryId = SAHPI_LAST_ENTRY;
	}
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
	struct oh_resource *res;

	OH_GET_RESOURCE;
	
	memcpy(RptEntry, &res->entry, sizeof(*RptEntry));

	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceSeveritySet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSeverityT Severity)
{
	struct oh_resource *res;

	OH_GET_RESOURCE;
	
	res->entry.ResourceSeverity = Severity;	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceTagSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
	struct oh_resource *res;
	
	OH_GET_RESOURCE;
	
	memcpy(&res->entry.ResourceTag, ResourceTag, sizeof(res->entry.ResourceTag));	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceIdGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
	return SA_ERR_HPI_UNKNOWN;
}

SaErrorT SAHPI_API saHpiEntitySchemaGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiUint32T *SchemaId)
{
	/*FIXME: how to determine schema is ongoing*/
	*SchemaId = ('C'<<24) | ('P'<<16) | ('C'<<8) | 'I';
	return SA_OK;
}


SaErrorT SAHPI_API saHpiEventLogInfoGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiSelInfoT *Info)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId,
		SAHPI_OUT SaHpiSelEntryIdT *PrevEntryId,
		SAHPI_OUT SaHpiSelEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiSelEntryT *EventLogEntry,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryT *EvtEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryDelete (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogClear (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogTimeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeT *Time)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogTimeSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeT Time)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiBoolT *Enable)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiBoolT Enable)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiBoolT ProvideActiveAlarms)
{
        struct oh_session *s;
        
        OH_STATE_READY_CHECK;
        
        s = session_get(SessionId);
        if (!s) {
                dbg("Invalid session");
                return SA_ERR_HPI_INVALID_SESSION;
	}
	
	if (s->event_state == OH_EVENT_SUBSCRIBE) {
		dbg("Duplicate subscribe");
		return SA_ERR_HPI_DUPLICATE;
	}
	
	if (ProvideActiveAlarms) {
		/*FIXME: nothing to do here?! */
	}

	s->event_state = OH_EVENT_SUBSCRIBE;
	return SA_OK;
}


SaErrorT SAHPI_API saHpiUnsubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId)
{
        struct oh_session *s;
        
        OH_STATE_READY_CHECK;
        
        s = session_get(SessionId);
        if (!s) {
                dbg("Invalid session");
                return SA_ERR_HPI_INVALID_SESSION;
	}

	if (s->event_state == OH_EVENT_UNSUBSCRIBE) {
		dbg("Duplicate subscribe");
		return SA_ERR_HPI_INVALID_REQUEST;
	}
	
	s->event_state = OH_EVENT_UNSUBSCRIBE;
	return SA_OK;
}

SaErrorT SAHPI_API saHpiEventGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout,
		SAHPI_OUT SaHpiEventT *Event,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
        struct oh_session *s;
	SaHpiTimeT now, end;
        OH_STATE_READY_CHECK;
       	int value;

        s = session_get(SessionId);
        if (!s) {
                dbg("Invalid session");
                return SA_ERR_HPI_INVALID_SESSION;
	}
	
	gettimeofday1(&now);
	if (Timeout== SAHPI_TIMEOUT_BLOCK) {
		end = now + (SaHpiTimeT)10000*1000000000; /*set a long time*/
	} else {
		end = now + Timeout;
	}
	
	while (1) {
		int rv;
		
		rv = get_events();
		if (rv<0) {
			value = SA_ERR_HPI_UNKNOWN;
			break;
		}
		
		if (session_has_event(s)) {
			value = SA_OK;
			break;
		}
		
		gettimeofday1(&now);	
		if (now>=end) {
			value = SA_ERR_HPI_TIMEOUT;
			break;
		}
	}
	
	//dbg("now=%lld, end=%lld", now, end);
	
	if (value==SA_OK) {
		struct oh_event e;
		struct oh_resource *res;
		struct oh_rdr *rdr;
		
		if (session_pop_event(s, &e)<0) {
			dbg("Empty event queue?!");
			return SA_ERR_HPI_UNKNOWN;
		}
		if (e.type != OH_ET_HPI) {
			dbg("Non-HPI event?! type=%d", e.type);
			return SA_ERR_HPI_UNKNOWN;
		}

		memcpy(Event, &e.u.hpi_event.event, sizeof(*Event));

		res = get_res_by_oid(e.u.hpi_event.parent);
		if (res) {
			memcpy(RptEntry, &res->entry, sizeof(*RptEntry));
			
			rdr = get_rdr_by_oid(res, e.u.hpi_event.id);
			if (rdr) {
				memcpy(Rdr, &rdr->rdr, sizeof(*Rdr));
			} else {
				dbg("Event without resource");
			}
		}else {
			dbg("Event without resource");
		}

	}
	
	return value;

}

SaErrorT SAHPI_API saHpiRdrGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEntryIdT EntryId,
		SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiRdrT *Rdr)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;	
	int no;
	
	OH_GET_RESOURCE;

	switch (EntryId) {
	case SAHPI_FIRST_ENTRY:
		no = 0;
		break;
	default:
		no = EntryId - entry_id_offset;
		break;
	}
	
	if (no>=g_slist_length(res->rdr_list)) {
		dbg("Invalid EntryId");
		return SA_ERR_HPI_INVALID;
	}

	rdr = g_slist_nth_data(res->rdr_list, no);
	memcpy(Rdr, &rdr->rdr, sizeof(*Rdr));
	
	no++;
		
	if (no < g_slist_length(res->rdr_list)) {
		*NextEntryId = no+entry_id_offset;
	} else {
		*NextEntryId = SAHPI_LAST_ENTRY;
	}
	return SA_OK;
}

/* Sensor data functions */

SaErrorT SAHPI_API saHpiSensorReadingGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorReadingT *Reading)
{
	dbg("saHpiSensorReadingGet() called");

	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*get_func) (void *, struct oh_rdr_id, SaHpiSensorReadingT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);

	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	get_func = res->handler->abi->get_sensor_data;

	if (!get_func)
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_func(res->handler->hnd, rdr->oid, Reading))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorReadingConvert (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorReadingT *ReadingInput,
		SAHPI_OUT SaHpiSensorReadingT *ConvertedReading)
{
	if (!SensorNum) {
		switch (ReadingInput->ValuesPresent) {

			case SAHPI_SRF_INTERPRETED:
				ConvertedReading->ValuesPresent = SAHPI_SRF_RAW;
				break;
			case SAHPI_SRF_RAW:
				ConvertedReading->ValuesPresent = SAHPI_SRF_INTERPRETED;
				break;
			default:
				return SA_ERR_HPI_INVALID_PARAMS;
		}
	} else 
		return SA_ERR_HPI_NOT_PRESENT;
	

	return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*set_func) (void *, struct oh_rdr_id,const SaHpiSensorThresholdsT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);

	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;
	
	set_func = res->handler->abi->set_sensor_thresholds;

	if (!set_func)
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_func(res->handler->hnd, rdr->oid, SensorThresholds))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*get_func) (void *, struct oh_rdr_id,SaHpiSensorThresholdsT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);

	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;
	
	get_func = res->handler->abi->get_sensor_thresholds;

	if (!get_func)
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_func(res->handler->hnd, rdr->oid, SensorThresholds))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

	/* Function: SaHpiSensorTypeGet */
	/* Core SAF_HPI function */
	/* Not mapped to plugin */
	/* Data in RDR */
SaErrorT SAHPI_API saHpiSensorTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorTypeT *Type,
		SAHPI_OUT SaHpiEventCategoryT *Category)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);
	
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	if (!memcpy(Type, &rdr->rdr.RdrTypeUnion.SensorRec.Type,
			sizeof(SaHpiSensorTypeT)))
		return SA_ERR_HPI_ERROR;

	if (!memcpy(Category, &rdr->rdr.RdrTypeUnion.SensorRec.Category,
			sizeof(SaHpiEventCategoryT)))
		return SA_ERR_HPI_ERROR;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorEventEnablesGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorEvtEnablesT *Enables)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;
	int (*get_sensor_event_enables)(void *hnd, struct oh_rdr_id id,
					SaHpiSensorEvtEnablesT *enables);
	
	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);
	
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	get_sensor_event_enables = res->handler->abi->get_sensor_event_enables;
	
	if (!get_sensor_event_enables)
		return SA_ERR_HPI_UNSUPPORTED_API;
	if (get_sensor_event_enables(res->handler->hnd, rdr->oid, Enables))
		return SA_ERR_HPI_UNKNOWN;
	return SA_OK;
}

SaErrorT SAHPI_API saHpiSensorEventEnablesSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorEvtEnablesT *Enables)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;
	int (*set_sensor_event_enables)(void *hnd, struct oh_rdr_id id,
					SaHpiSensorEvtEnablesT *enables);
	
	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_SENSOR_RDR, SensorNum);
	
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	set_sensor_event_enables = res->handler->abi->get_sensor_event_enables;
	
	if (!set_sensor_event_enables)
		return SA_ERR_HPI_UNSUPPORTED_API;
	if (set_sensor_event_enables(res->handler->hnd, rdr->oid, Enables))
		return SA_ERR_HPI_UNKNOWN;
	return SA_OK;
}

/* End Sensor functions */

/* Control data functions */

SaErrorT SAHPI_API saHpiControlTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_OUT SaHpiCtrlTypeT *Type)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_CTRL_RDR, CtrlNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	if (!memcpy(Type, &rdr->rdr.RdrTypeUnion.CtrlRec.Type, 
		    sizeof(SaHpiCtrlTypeT)))
	    return SA_ERR_HPI_ERROR;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiControlStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_INOUT SaHpiCtrlStateT *CtrlState)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*get_func)(void *, struct oh_rdr_id,SaHpiCtrlStateT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_CTRL_RDR, CtrlNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	get_func = res->handler->abi->get_control_state;
	if (!get_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_func(res->handler->hnd, rdr->oid, CtrlState))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiControlStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_IN SaHpiCtrlStateT *CtrlState)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*set_func)(void *, struct oh_rdr_id,SaHpiCtrlStateT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_CTRL_RDR, CtrlNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	set_func = res->handler->abi->set_control_state;
	if (!set_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_func(res->handler->hnd, rdr->oid, CtrlState))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

/* current sahpi.h missed SA_ERR_INVENT_DATA_TRUNCATED */
#ifndef SA_ERR_INVENT_DATA_TRUNCATED
//#warning "No 'SA_ERR_INVENT_DATA_TRUNCATED 'definition in sahpi.h!"
#define SA_ERR_INVENT_DATA_TRUNCATED	(SaErrorT)(SA_HPI_ERR_BASE - 1000)
#endif

SaErrorT SAHPI_API saHpiEntityInventoryDataRead (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiUint32T BufferSize,
		SAHPI_OUT SaHpiInventoryDataT *InventData,
		SAHPI_OUT SaHpiUint32T *ActualSize)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*get_size)(void *, struct oh_rdr_id, size_t *);
	int (*get_func)(void *, struct oh_rdr_id, SaHpiInventoryDataT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_INVENTORY_RDR, EirId);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	get_size = res->handler->abi->get_inventory_size;
	get_func = res->handler->abi->get_inventory_info;
	if (!get_func || !get_size)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_size(res->handler->hnd, rdr->oid, ActualSize))
		return SA_ERR_HPI_UNKNOWN;
	
	if (*ActualSize>BufferSize)
		return SA_ERR_INVENT_DATA_TRUNCATED;
	
	if (get_func(res->handler->hnd, rdr->oid, InventData))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiEntityInventoryDataWrite (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiInventoryDataT *InventData)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*set_func)(void *, struct oh_rdr_id, const SaHpiInventoryDataT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_INVENTORY_RDR, EirId);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	set_func = res->handler->abi->set_inventory_info;
	if (!set_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_func(res->handler->hnd, rdr->oid, InventData))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_OUT SaHpiWatchdogT *Watchdog)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*get_func)(void *, struct oh_rdr_id,SaHpiWatchdogT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_WATCHDOG_RDR, WatchdogNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	get_func = res->handler->abi->get_watchdog_info;
	if (!get_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_func(res->handler->hnd, rdr->oid, Watchdog))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_IN SaHpiWatchdogT *Watchdog)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*set_func)(void *, struct oh_rdr_id,SaHpiWatchdogT *);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_WATCHDOG_RDR, WatchdogNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	set_func = res->handler->abi->set_watchdog_info;
	if (!set_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_func(res->handler->hnd, rdr->oid, Watchdog))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
	struct oh_resource *res;
	struct oh_rdr *rdr;

	int (*reset_func)(void *, struct oh_rdr_id);

	OH_GET_RESOURCE;

	rdr = get_rdr(res, SAHPI_WATCHDOG_RDR, WatchdogNum);
	if (!rdr)
		return SA_ERR_HPI_INVALID_PARAMS;

	reset_func = res->handler->abi->reset_watchdog;
	if (!reset_func)		
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (reset_func(res->handler->hnd, rdr->oid))
		return SA_ERR_HPI_UNKNOWN;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapControlRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	struct oh_resource *res;
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	res->controlled = 1;
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceActiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	struct oh_resource *res;
	int (*set_hotswap_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsStateT state);
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	if (!res->controlled)
		return SA_ERR_HPI_INVALID_CMD;
	
	set_hotswap_state = res->handler->abi->set_hotswap_state;
	if (!set_hotswap_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	res->controlled = 0;
	if (set_hotswap_state(res->handler->hnd, res->oid, SAHPI_HS_STATE_ACTIVE_HEALTHY)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceInactiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	struct oh_resource *res;
	int (*set_hotswap_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsStateT state);
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	if (!res->controlled)
		return SA_ERR_HPI_INVALID_CMD;
	
	set_hotswap_state = res->handler->abi->set_hotswap_state;
	if (!set_hotswap_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	res->controlled = 0;
	if (set_hotswap_state(res->handler->hnd, res->oid, SAHPI_HS_STATE_INACTIVE)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiTimeoutT *Timeout)
{
	struct oh_session *s;
	
	OH_STATE_READY_CHECK;

	s = session_get(SessionId);
	if (!s) {
		dbg("Invalid session");
		return SA_ERR_HPI_INVALID_SESSION;
	}
	
	*Timeout = get_hotswap_auto_insert_timeout();
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout)
{
	struct oh_session *s;
	
	OH_STATE_READY_CHECK;

	s = session_get(SessionId);
	if (!s) {
		dbg("Invalid session");
		return SA_ERR_HPI_INVALID_SESSION;
	}
	
	set_hotswap_auto_insert_timeout(Timeout);
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeoutT *Timeout)
{
	struct oh_resource *res;
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	
	*Timeout = res->auto_extract_timeout;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeoutT Timeout)
{
	struct oh_resource *res;
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	
	res->auto_extract_timeout = Timeout;

	return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsStateT *State)
{
	struct oh_resource *res;
	int (*get_hotswap_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsStateT *state);
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	
	get_hotswap_state = res->handler->abi->get_hotswap_state;
	if (!get_hotswap_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_hotswap_state(res->handler->hnd, res->oid, State)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsActionT Action)
{
	struct oh_resource *res;
	int (*request_hotswap_action)(void *hnd, struct oh_resource_id id,
			SaHpiHsActionT act);
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP))
		return SA_ERR_HPI_INVALID;
	
	request_hotswap_action = res->handler->abi->request_hotswap_action;
	if (!request_hotswap_action) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (request_hotswap_action(res->handler->hnd, res->oid, Action)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	get_events();
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsPowerStateT *State)
{
	struct oh_resource *res;
	int (*get_power_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsPowerStateT *state);
	
	OH_GET_RESOURCE;
	
	get_power_state = res->handler->abi->get_power_state;
	if (!get_power_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_power_state(res->handler->hnd, res->oid, State)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsPowerStateT State)
{
	struct oh_resource *res;
	int (*set_power_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsPowerStateT state);
	
	OH_GET_RESOURCE;
	
	set_power_state = res->handler->abi->set_power_state;
	if (!set_power_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_power_state(res->handler->hnd, res->oid, State)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
	struct oh_resource *res;
	int (*get_indicator_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsIndicatorStateT *state);
	
	OH_GET_RESOURCE;
	
	get_indicator_state = res->handler->abi->get_indicator_state;
	if (!get_indicator_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_indicator_state(res->handler->hnd, res->oid, State)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsIndicatorStateT State)
{
	struct oh_resource *res;
	int (*set_indicator_state)(void *hnd, struct oh_resource_id id,
			SaHpiHsIndicatorStateT state);
	
	OH_GET_RESOURCE;
	
	set_indicator_state = res->handler->abi->set_indicator_state;
	if (!set_indicator_state) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_indicator_state(res->handler->hnd, res->oid, State)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiParmControl (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiParmActionT Action)
{
	struct oh_resource *res;
	int (*control_parm)(void *, struct oh_resource_id, SaHpiParmActionT);
	
	OH_GET_RESOURCE;
	
	if (!(res->entry.ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION))
		return SA_ERR_HPI_INVALID;
	
	control_parm = res->handler->abi->control_parm;
	if (!control_parm) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (control_parm(res->handler->hnd, res->oid, Action)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceResetStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiResetActionT *ResetAction)
{
	struct oh_resource *res;
	int (*get_func)(void *, struct oh_resource_id, SaHpiResetActionT *);
	
	OH_GET_RESOURCE;
	
	get_func = res->handler->abi->get_reset_state;
	if (!get_func) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (get_func(res->handler->hnd, res->oid, ResetAction)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}

SaErrorT SAHPI_API saHpiResourceResetStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiResetActionT ResetAction)
{
	struct oh_resource *res;
	int (*set_func)(void *, struct oh_resource_id, SaHpiResetActionT);
	
	OH_GET_RESOURCE;
	
	set_func = res->handler->abi->set_reset_state;
	if (!set_func) 
		return SA_ERR_HPI_UNSUPPORTED_API;

	if (set_func(res->handler->hnd, res->oid, ResetAction)<0) 
		return SA_ERR_HPI_UNKNOWN;
	
	return SA_OK;
}
