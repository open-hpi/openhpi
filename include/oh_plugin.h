#ifndef __OH_PLUGIN_H
#define __OH_PLUGIN_H

#include <uuid/uuid.h>

#include <sys/time.h>
#include <SaHpi.h>

enum oh_id_type {
	OH_ID_EVENT_LOG,
	OH_ID_WATCHDOG,
	OH_ID_INVENTORY,
	OH_ID_ENTITY,
	OH_ID_SENSOR,
	OH_ID_CONTROL,
	OH_ID_SEL,
	OH_ID_DOMAIN
};

/* 
 * struct oh_id is filled by plugin and used by Open HPI.
 * Open HPI never assume any detail in struct oh_id.
 * Plugin can store any data pointer in ptr member so that
 * it can map id back to solid data
 */
struct oh_id {
	enum oh_id_type type;
	void *ptr;
};
	

struct oh_entity_event {
	SaHpiEventCategoryT	category;
	SaHpiEventStateT	state;
};

struct oh_sel_event {
	SaHpiEventCategoryT	category;
	SaHpiEventStateT	state;
};

struct oh_event {
	struct oh_id			oid;
	struct timeval			timestamp;
	SaHpiSeverityT			severity;
	union {
		SaHpiSensorEventT   sensor;
		SaHpiHotSwapEventT  hotswap;
		SaHpiWatchdogEventT watchdog;
		SaHpiOemEventT      oem;
		SaHpiUserEventT     user;
		struct oh_entity_event entity;
		struct oh_sel_event sel;
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
	 * @return the handler of the instance
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
	 * @remark at the start-up, plugins must send out ADD event for all
	 * resources and items so as to OpenHPI can build up RPT/RDR.
	 * @return >0 if an event is returned; 0 if timeout; otherwise an error
	 * occur.
	 * @param event if existing, plugin store the event. 
	 * @param timeout is an upper bound on the amount of time elapsed
	 * before returns. It may be zero, causing select to return 
	 * immediately.
	 */
	int (*get_event)(void *hnd, struct oh_event *event, struct timeval *timeout);

	/**
	 * get the id which the caller is running
	 */
	int (*get_sel_id)(void *hnd, struct oh_id *id);

	/**
	 * get info from system event log
	 */
	int (*get_sel_info)(void *hnd, struct oh_id *id, SaHpiSelInfoT *info);

	/**
	 * set time to system event log
	 */
	int (*set_sel_time)(void *hnd, struct oh_id *id, struct timeval *time);

	/**
	 * set state to system event log
	 */
	int (*set_sel_state)(void *hnd, struct oh_id *id, int enabled);
	
	/**
	 * get sensor info
	 */
	int (*get_sensor_info)(void *hnd, struct oh_id *id, SaHpiSensorDataFormatT *format);

	/**
	 * get sensor data
	 */
	int (*get_sensor_data)(void *hnd, struct oh_id *id, SaHpiSensorReadingT *data);

	/**
	 * get sensor thresholds
	 */
	int (*get_sensor_thresholds)(void *hnd, struct oh_id *id, SaHpiSensorThresholdsT *thres);
	
	/**
	 * set sensor thresholds
	 */
	int (*set_sensor_thresholds)(void *hnd, struct oh_id *id, const SaHpiSensorThresholdsT *thres);
	
	/**
	 * get control info
	 */
	int (*get_control_info)(void *hnd, struct oh_id *id, SaHpiCtrlTypeT *type);

	/**
	 * get control state
	 */
	int (*get_control_state)(void *hnd, struct oh_id *id, SaHpiCtrlStateT *state);
	
	/**
	 * set control state
	 */
	int (*set_control_state)(void *hnd, struct oh_id *id, SaHpiCtrlStateT *state);
	
	/**
	 * set inventory state
	 */
	int (*get_inventory_size)(void *hnd, struct oh_id *id, size_t *size);

	/**
	 * get inventory state
	 */
	int (*get_inventory_info)(void *hnd, struct oh_id *id, SaHpiInventoryDataT *data);

	/**
	 * set inventory state
	 */
	int (*set_inventory_info)(void *hnd, struct oh_id *id, SaHpiInventoryDataT *data);

	/**
	 * get watchdog timer info
	 */
	int (*get_watchdog_info)(void *hnd, struct oh_id *id, SaHpiWatchdogT *wdt);

	/** 
	 * set watchdog timer info
	 */
	int (*set_watchdog_info)(void *hnd, struct oh_id *id, SaHpiWatchdogT *wdt);

	/**
	 * reset watchdog timer info
	 */
	int (*reset_watchdog)(void *hnd, struct oh_id *id);

	/**
	 * get hotswap state
	 */
	int (*get_hotswap_state)(void *hnd, struct oh_id *id, SaHpiHsStateT *state);

	/**
	 * set hotswap state
	 */
	int (*set_hotswap_state)(void *hnd, struct oh_id *id, SaHpiHsStateT state);

	/**
	 * request hotswap state
	 */
	int (*request_hotswap_action)(void *hnd, struct oh_id *id, SaHpiHsActionT act);

	/**
	 * get power state
	 */
	int (*get_power_state)(void *hnd, struct oh_id *id, SaHpiHsPowerStateT *state);

	/**
	 * set power state
	 */
	int (*set_power_state)(void *hnd, struct oh_id *id, SaHpiHsPowerStateT state);
	
	/**
	 * get indicator state
	 */
	int (*get_indicator_state)(void *hnd, struct oh_id *id, SaHpiHsIndicatorStateT *state);

	/**
	 * set indicator state
	 */
	int (*set_indicator_state)(void *hnd, struct oh_id *id, SaHpiHsIndicatorStateT state);

	/**
	 * control parameter
	 */
	int (*control_parm)(void *hnd, struct oh_id *id, SaHpiParmActionT act);

	/**
	 * get reset state
	 */
	int (*get_reset_state)(void *hnd, struct oh_id *id, SaHpiResetActionT *act);

	/**
	 * set_reset state
	 */
	int (*set_reset_state)(void *hnd, struct oh_id *id, SaHpiResetActionT act);

};


#endif/*__OH_PLUGIN_H*/
