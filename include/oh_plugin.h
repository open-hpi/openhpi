/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
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
 */

#ifndef __OH_PLUGIN_H
#define __OH_PLUGIN_H

#include <uuid/uuid.h>

#include <sys/time.h>
#include <SaHpi.h>

/* 
 * struct oh_resource_id is filled by plugin.
 * Open HPI uses it to identy different resource by the id.
 * Open HPI never assume any detail in 'ptr' field so plugin 
 * can store any data pointer in ptr member so that it can map 
 * id back to solid data
 */
struct oh_resource_id {
	void *ptr;
};

/*
 * struct oh_rdr_id is filled by plugin.
 * Open HPI use it to identy different rdr by the id.
 */
struct oh_rdr_id {
	void *ptr;
};
	
/*
 * The event is used for plugin to report its resources
 * (Domain, SEL and RDR etc.).
 */
struct oh_resource_event {
	struct oh_resource_id	id;
	
	/* XXX: upper layer will fill some fields which does not 
	 * owned by plugins (such as domain Id)
	 */
	SaHpiRptEntryT		entry;
};

/* 
 * The event is used for plugin to report its RDRs in resource.
 */
struct oh_rdr_event {
	/*This resource id which the rdr belongs to */
	struct oh_resource_id	parent;
	/*This is rdr id the RDR relate*/
	struct oh_rdr_id	id;
	
	SaHpiRdrT		rdr;
};

/*
 * The event is used for plugin to notify HPI events
 */
struct oh_hpi_event {
	/*This resource id which the rdr belongs to */
	struct oh_resource_id	parent;
	/*This is rdr id the RDR relate*/
	struct oh_rdr_id	id;
	
	/* XXX: upper layer will fill some fields which does not
	 * owned by plugins (ResourceId etc.). */
	SaHpiEventT		event;
};

/* 
 * This is the main event structure. It is used for plugin report
 * its discovery about new resource/rdr or what happend on resource
 */
struct oh_event {
	enum {
		OH_ET_RESOURCE,
		OH_ET_RDR,
		OH_ET_HPI
	}type;
	union {
		struct oh_resource_event res_event;
		struct oh_rdr_event	 rdr_event;
		struct oh_hpi_event	 hpi_event;
	} u;		    
};

/* UUID is ee778a5f-32cf-453b-a650-518814dc956c */
static const uuid_t UUID_OH_ABI_V1 = {
	0xee, 0x77, 0x8a, 0x5f, 0x32, 0xcf, 0x45, 0x3b,
	0xa6, 0x50, 0x51, 0x88, 0x14, 0xdc, 0x95, 0x6c
};

struct oh_abi_v1 {
	/**
	 * The function create an instance 
	 * @return the handler of the instance, this can be recognised 
	 * as a domain in upper layer
	 * @param name the mechanism's name. 
	 * for example, "snmp" for SNMP, "smi" for IPMI SMI
	 * @param addr the interface name.
	 * for example, "ipaddr:port" for SNMP, "if_num" for IPMI SMI
	 */
	void *(*open)(const char *name, const char *addr);
	
	void (*close)(void *hnd);
	/**
	 * The function wait for event. 
	 * 
	 *
	 * @remark at the start-up, plugins must send out res/rdr event for all
	 * resources and rdrs so as to OpenHPI can build up RPT/RDR.
	 * @return >0 if an event is returned; 0 if timeout; otherwise an error
	 * occur.
	 * @param event if existing, plugin store the event. 
	 * @param timeout is an upper bound on the amount of time elapsed
	 * before returns. It may be zero, causing select to return 
	 * immediately.
	 */
	int (*get_event)(void *hnd, struct oh_event *event, 
			 struct timeval *timeout);
	
	/**
	 * prompt plug-in to search for new resources
	 */
	int (*discover_resources)(void *hnd);

	/**
	 * get the id which the caller is running
	 */
	int (*get_sel_id)(void *hnd, struct oh_resource_id id);

	/**
	 * get info from system event log
	 */
	int (*get_sel_info)(void *hnd, struct oh_resource_id id, SaHpiSelInfoT *info);

	/**
	 * set time to system event log
	 */
	int (*set_sel_time)(void *hnd, struct oh_resource_id id, struct timeval *time);

	/**
	 * set state to system event log
	 */
	int (*set_sel_state)(void *hnd, struct oh_resource_id id, int enabled);
	
	/**
	 * get sensor data
	 */
	int (*get_sensor_data)(void *hnd, struct oh_rdr_id id, 
			       SaHpiSensorReadingT *data);

	/**
	 * get sensor thresholds
	 */
	int (*get_sensor_thresholds)(void *hnd, struct oh_rdr_id id,
				     SaHpiSensorThresholdsT *thres);
	
	/**
	 * set sensor thresholds
	 */
	int (*set_sensor_thresholds)(void *hnd, struct oh_rdr_id id,
				     const SaHpiSensorThresholdsT *thres);

	/**
	 * get sensor event enables
	 */
	int (*get_sensor_event_enables)(void *hnd, struct oh_rdr_id id,
					SaHpiSensorEvtEnablesT *enables);

	/**
	 * set sensor event enables
	 */
	int (*set_sensor_event_enables)(void *hnd, struct oh_rdr_id id,
					const SaHpiSensorEvtEnablesT *enables);
	/**
	 * get control info
	 */
	int (*get_control_info)(void *hnd, struct oh_rdr_id id,
				SaHpiCtrlTypeT *type);

	/**
	 * get control state
	 */
	int (*get_control_state)(void *hnd, struct oh_rdr_id id,
				 SaHpiCtrlStateT *state);
	
	/**
	 * set control state
	 */
	int (*set_control_state)(void *hnd, struct oh_rdr_id id,
				 SaHpiCtrlStateT *state);
	
	/**
	 * set inventory state
	 */
	int (*get_inventory_size)(void *hnd, struct oh_rdr_id id,
				  size_t *size);

	/**
	 * get inventory state
	 */
	int (*get_inventory_info)(void *hnd, struct oh_rdr_id id,
				  SaHpiInventoryDataT *data);

	/**
	 * set inventory state
	 */
	int (*set_inventory_info)(void *hnd, struct oh_rdr_id id,
				  const SaHpiInventoryDataT *data);

	/**
	 * get watchdog timer info
	 */
	int (*get_watchdog_info)(void *hnd, struct oh_rdr_id id,
				 SaHpiWatchdogT *wdt);

	/** 
	 * set watchdog timer info
	 */
	int (*set_watchdog_info)(void *hnd, struct oh_rdr_id id,
				 SaHpiWatchdogT *wdt);

	/**
	 * reset watchdog timer info
	 */
	int (*reset_watchdog)(void *hnd, struct oh_rdr_id id);

	/**
	 * get hotswap state
	 */
	int (*get_hotswap_state)(void *hnd, struct oh_resource_id id, 
				 SaHpiHsStateT *state);

	/**
	 * set hotswap state
	 */
	int (*set_hotswap_state)(void *hnd, struct oh_resource_id id, 
				 SaHpiHsStateT state);

	/**
	 * request hotswap state
	 */
	int (*request_hotswap_action)(void *hnd, struct oh_resource_id id, 
				      SaHpiHsActionT act);

	/**
	 * get power state
	 */
	int (*get_power_state)(void *hnd, struct oh_resource_id id, 
			       SaHpiHsPowerStateT *state);

	/**
	 * set power state
	 */
	int (*set_power_state)(void *hnd, struct oh_resource_id id, 
			       SaHpiHsPowerStateT state);
	
	/**
	 * get indicator state
	 */
	int (*get_indicator_state)(void *hnd, struct oh_resource_id id, 
				   SaHpiHsIndicatorStateT *state);

	/**
	 * set indicator state
	 */
	int (*set_indicator_state)(void *hnd, struct oh_resource_id id, 
				   SaHpiHsIndicatorStateT state);

	/**
	 * control parameter
	 */
	int (*control_parm)(void *hnd, struct oh_resource_id id, SaHpiParmActionT act);

	/**
	 * get reset state
	 */
	int (*get_reset_state)(void *hnd, struct oh_resource_id id, 
			       SaHpiResetActionT *act);

	/**
	 * set_reset state
	 */
	int (*set_reset_state)(void *hnd, struct oh_resource_id id, 
			       SaHpiResetActionT act);

};

/*The function is used for plugin loader to get interface*/
int get_interface(void **pp, uuid_t uuid);

#endif/*__OH_PLUGIN_H*/
