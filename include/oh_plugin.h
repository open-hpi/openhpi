/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copright IBM Corp 2003
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
 *     Sean Dague <http://dague.net/sean>
 */

#ifndef __OH_PLUGIN_H
#define __OH_PLUGIN_H

#include <uuid/uuid.h>
#include <glib.h>

#include <sys/time.h>
#include <SaHpi.h>
#include <oh_utils.h>

#ifdef __cplusplus
extern "C" {
#endif

void oh_cond_signal(void); 

/*
 * Common OpenHPI implementation specific definitions
 * --------------------------------------------------
 *
 * plugin - software component contained in a shared library that exports
 *          a function named 'get_interface'.  Loading a plugin entails
 *          performing a dlopen on the library, finding 'get_interface' with
 *          dl_sym, and calling 'get_interface' to get an interface pointer
 *          (referred to as the 'abi'.)
 *
 * abi - pointer to a structure of type oh_abi_XX where XX represents a
 *       version of the structure.  This structure is a bundle of function
 *       pointers that represents the interface between a given plug-in
 *       instance (known as a handler), and the OpenHPI infrastructure.
 *
 * handler - an instance of a plugin represented by a structure of type
 *           oh_handler which contains an abi and a pointer to an instance
 *           specific data structure that is private to the plug-in.
 */

/*
 * How plugins are instantiated
 * ----------------------------
 *
 * When an HPI application initializes OpenHPI by calling saHpiInitialize(),
 * the OpenHPI infrastructure will seek out all configured plug-ins
 * (see oh_config.h for details on how a plug-in is configured), and:
 * 1. load the plug-in into memory
 * 2. extract an abi from the plug-in
 * 3. create a new oh_plugin containing the name of the plugin and
 *    the abi extracted from the plugin
 * 4. add the oh_plugin to the global list of plugins
 *
 * The first time the HPI application creates a new session by calling
 * saHpiSessionOpen(), the OpenHPI infrastructure will once again examine
 * the implementation configuration and create new plug-in instances
 * (i.e. a handler) as the configuration dictates.
 *
 * Each handler configuration item will specify:
 * 1. name of plugin in question
 * 2. additional arguments understood by the plug-in
 *
 * Each new handler is created by:
 * 1. finding the oh_plugin containing the same plugin name as the
 *    configuration item
 * 2. using the abi found in the oh_plugin to call abi->open(), passing
 *    the additional plug-in specific arguments to the open function.
 *    The open call will return a void* pointer (known as hnd) that is
 *    required to be passed back to the plug-in for all further abi
 *    function calls.
 * 3. creating a new oh_handler that contains a pointer to the associated
 *    abi, and the hnd returned by the open function.
 */

/*
 * How plugins can have multiple instances open at the same time
 * -------------------------------------------------------------
 *
 * The key to a plugin being able to support multiple instances
 * is in the 'void *hnd' passed to all abi functions (except open().)
 * The intent is that hnd is used as a private pointer to an instance specific
 * data structure.
 *
 * For example, if a plug-in were created to allow an HPI implementation
 * running a remote server to inter-operate with the local OpenHPI
 * implementation, then the plug-in could be written such that:
 * 1. the plugin defines a new structure containing an event queue and tcp
 *    socket to the remote machine
 * 2. the plugin requires that handler configuration entries for this
 *    plugin to contain the IP address of the remote machine to connect
 * 3. when open() is called, the plugin
 *    - opens a socket to the new machine
 *    - allocates a new event queue
 *    - allocates a new instance structure
 *    - stores the event queue and socket in the instance structure
 *    - returns a pointer to the structure as 'hnd'.
 * 4. as other abi functions are called, the 'hnd' passed in with those
 *    functions is cast back to a pointer to the instance data, and then
 *    communicates over the socket in that structure to service the given
 *    request.
 *
 */

struct oh_handler_state {
        RPTable 	*rptcache;
        oh_el  		*elcache;
        GSList 		*eventq;
        GAsyncQueue 	*eventq_async;
        GStaticRecMutex	*handler_lock;
	GThread 	*thread_handle;
        GHashTable 	*config;
        void 		*data;
};

/* Current abi is version 2. Version 1 is out-of-date and nobody
 * should use it
 */

#if 0
/* UUID_OH_ABI_V1 is out-of-date, keep here just for reference
 * ee778a5f-32cf-453b-a650-518814dc956c
 */
static const uuid_t UUID_OH_ABI_V1 = {
        0xee, 0x77, 0x8a, 0x5f, 0x32, 0xcf, 0x45, 0x3b,
        0xa6, 0x50, 0x51, 0x88, 0x14, 0xdc, 0x95, 0x6c
};
#endif

/* 13adfcc7-d788-4aaf-b966-5cd30bdcd808 */
static const uuid_t UUID_OH_ABI_V2 = {
        0x13, 0xad, 0xfc, 0xc7, 0xd7, 0x88, 0x4a, 0xaf,
        0xb9, 0x66, 0x5c, 0xd3, 0x0b, 0xdc, 0xd8, 0x08
};

struct oh_abi_v2 {
        /**
         * The function create an instance
         * @return the handler of the instance, this can be recognised
         * as a domain in upper layer
         * @param name the mechanism's name.
         * for example, "snmp" for SNMP, "smi" for IPMI SMI
         * @param addr the interface name.
         * for example, "ipaddr:port" for SNMP, "if_num" for IPMI SMI
         */
        void *(*open)(GHashTable *handler_config);

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
         */
        SaErrorT (*get_event)(void *hnd, struct oh_event *event);

        /**
         * prompt plug-in to search for new resources
         */
        SaErrorT (*discover_resources)(void *hnd);

        /**
         * set resource tag, this is passed down so the device has
         * a chance to set it in nv storage if it likes
         */
        SaErrorT (*set_resource_tag)(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag);

        /**
         * set resource severity is pushed down so the device has
         * a chance to set it in nv storage
         */
        SaErrorT (*set_resource_severity)(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev);

        /******************************************************
         *
         *  Event Log functions
         *
         *****************************************************/

        /**
         * get info from EL
         */
        SaErrorT (*get_el_info)(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info);

        /**
         * set time to EL
         */
        SaErrorT (*set_el_time)(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time);

        /**
         * add entry to EL
         */
        SaErrorT (*add_el_entry)(void *hnd, SaHpiResourceIdT id, const SaHpiEventT *Event);

        /**
         * get entry in EL
         *
         * although it looks like we need Resource and RDR passed back up, we don't
         * because EventLogEntryT has that info stored in it.  We'll just unwind
         * that in infrastructure.
         */
        SaErrorT (*get_el_entry)(void *hnd, SaHpiResourceIdT id, SaHpiEventLogEntryIdT current,
                             SaHpiEventLogEntryIdT *prev, SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT *entry);

        /**
         * clear EL
         */
        SaErrorT (*clear_el)(void *hnd, SaHpiResourceIdT id);

        /* end of EL functions */
        /**
         * get sensor data
         */
        SaErrorT (*get_sensor_data)(void *hnd, SaHpiResourceIdT id,
                               SaHpiSensorNumT num,
                               SaHpiSensorReadingT *data);

        /**
         * get sensor thresholds
         */
        SaErrorT (*get_sensor_thresholds)(void *hnd, SaHpiResourceIdT id,
                                     SaHpiSensorNumT num,
                                     SaHpiSensorThresholdsT *thres);

        /**
         * set sensor thresholds
         */
        SaErrorT (*set_sensor_thresholds)(void *hnd, SaHpiResourceIdT id,
                                     SaHpiSensorNumT num,
                                     const SaHpiSensorThresholdsT *thres);

        /**
         * get sensor event enables
         */
        SaErrorT (*get_sensor_event_enables)(void *hnd, SaHpiResourceIdT id,
                                        SaHpiSensorNumT num,
                                        SaHpiBoolT *enables);

        /**
         * set sensor event enables
         */
        SaErrorT (*set_sensor_event_enables)(void *hnd, SaHpiResourceIdT id,
                                    SaHpiSensorNumT num,
                                    const SaHpiBoolT enables);

	/**
	 * get sensor event masks
	 */

	SaErrorT (*get_sensor_event_masks)(void *hnd, SaHpiResourceIdT id,
				    SaHpiSensorNumT   num,
				    SaHpiEventStateT    *AssertEventMask,
				    SaHpiEventStateT    *DeassertEventMask);
	/**
	 * set sensor event masks
	 */

	SaErrorT (*set_sensor_event_masks)(void *hnd, SaHpiResourceIdT id,
				    SaHpiSensorNumT   num,
				    SaHpiEventStateT    *AssertEventMask,
				    SaHpiEventStateT    *DeassertEventMask);

        /* SLD - 6/8/2004 we might want to change the names of these calls
           as control calls also get mode now */

        /**
         * get control state
         */
        SaErrorT (*get_control_state)(void *hnd, SaHpiResourceIdT id,
                                  SaHpiCtrlNumT num,
                                  SaHpiCtrlModeT *mode,
                                  SaHpiCtrlStateT *state);

        /**
         * set control state
         */
        SaErrorT (*set_control_state)(void *hnd, SaHpiResourceIdT id,
                                  SaHpiCtrlNumT num,
                                  SaHpiCtrlModeT mode,
                                  SaHpiCtrlStateT *state);
				  
        /**
         * get Inventory Data Record
         */
        SaErrorT (*get_idr_info)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrInfoT *idrinfo);

        /**
         * get Inventory Data Area Header
         */
        SaErrorT (*get_idr_area_header)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrAreaTypeT areatype,
			      SaHpiEntryIdT areaid,  SaHpiEntryIdT *nextareaid, SaHpiIdrAreaHeaderT *header);
			      
        /**
         * add Inventory Data Area
         */
        SaErrorT (*add_idr_area)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrAreaTypeT areatype, SaHpiEntryIdT *areaid);
			      
        /**
         * delete Inventory Data Area
         */
        SaErrorT (*del_idr_area)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiEntryIdT areaid);

        /**
         * get Inventory Data Field
         */
        SaErrorT (*get_idr_field)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
	                     SaHpiEntryIdT areaid, SaHpiIdrFieldTypeT fieldtype, SaHpiEntryIdT fieldid,
			     SaHpiEntryIdT *nextfieldid, SaHpiIdrFieldT *field);
			     
        /**
         * add Inventory Data Field
         */
        SaErrorT (*add_idr_field)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrFieldT *field );
	
        /**
         * set Inventory Data Field
         */
        SaErrorT (*set_idr_field)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrFieldT *field );
	
        /**
         * delete Inventory Data Field
         */
        SaErrorT (*del_idr_field)(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiEntryIdT areaid, SaHpiEntryIdT fieldid);

        /**
         * get watchdog timer info
         */
        SaErrorT (*get_watchdog_info)(void *hnd, SaHpiResourceIdT id,
                                 SaHpiWatchdogNumT num,
                                 SaHpiWatchdogT *wdt);

        /**
         * set watchdog timer info
         */
        SaErrorT (*set_watchdog_info)(void *hnd, SaHpiResourceIdT id,
                                 SaHpiWatchdogNumT num,
                                 SaHpiWatchdogT *wdt);

        /**
         * reset watchdog timer info
         */
        SaErrorT (*reset_watchdog)(void *hnd, SaHpiResourceIdT id,
                              SaHpiWatchdogNumT num);

        /**
         * get hotswap state
         */
        SaErrorT (*get_hotswap_state)(void *hnd, SaHpiResourceIdT id,
                                 SaHpiHsStateT *state);

        /**
         * set hotswap state
         */
        SaErrorT (*set_hotswap_state)(void *hnd, SaHpiResourceIdT id,
                                 SaHpiHsStateT state);

        /**
         * request hotswap state
         */
        SaErrorT (*request_hotswap_action)(void *hnd, SaHpiResourceIdT id,
                                      SaHpiHsActionT act);

        /**
         * get power state
         */
        SaErrorT (*get_power_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiPowerStateT *state);

        /**
         * set power state
         */
        SaErrorT (*set_power_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiPowerStateT state);

        /**
         * get indicator state
         */
        SaErrorT (*get_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                   SaHpiHsIndicatorStateT *state);

        /**
         * set indicator state
         */
        SaErrorT (*set_indicator_state)(void *hnd, SaHpiResourceIdT id,
                                   SaHpiHsIndicatorStateT state);

        /**
         * control parameter
         */
        SaErrorT (*control_parm)(void *hnd, SaHpiResourceIdT id, SaHpiParmActionT act);

        /**
         * get reset state
         */
        SaErrorT (*get_reset_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiResetActionT *act);

        /**
         * set_reset state
         */
        SaErrorT (*set_reset_state)(void *hnd, SaHpiResourceIdT id,
                               SaHpiResetActionT act);



};


/*The function is used for plugin loader to get interface*/
int get_interface(void **pp, const uuid_t uuid) __attribute__ ((weak));

/* Structure for static plugins */
typedef int (*get_interface_t)( void **pp, const uuid_t uuid );

struct oh_static_plugin
{
        char           *name;
        get_interface_t get_interface;
};

#ifdef __cplusplus
}
#endif

#endif/*__OH_PLUGIN_H*/
