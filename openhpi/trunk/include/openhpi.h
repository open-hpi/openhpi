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

#ifndef __OPENHPI_H
#define __OPENHPI_H

#include <stdio.h>
#include <glib.h>

#include <oh_plugin.h>

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


/*
 *  Global listing of plugins (oh_plugin).  This list is populated
 *  by the configuration subsystem, and used by the plugin loader.
 */
extern GSList *global_plugin_list;

/*
 *  Global listing of handlers (oh_handler).  This list is populated
 *  by the first call the saHpiSessionOpen().
 */
extern GSList *global_handler_list;

/*
 *  Global listing of all active sessions (oh_session).  This list is 
 *  populated and depopulated by calls to saHpiSessionOpen() and
 *  saHpiSessionClose()
 */
extern GSList *global_session_list;

/*
 *  Global RPT Table (implemented as a linked list).
 *
 *  This list contains all resources (wrapped as oh_resource structures),
 *  regardless of whether an HPI caller can see the resources for the given
 *  permission level or domain view used.
 * 
 *  This list is populated by calls to saHpiDiscoverResources()
 */
extern GSList *global_rpt;
extern unsigned int global_rpt_counter; /*FIXME: I use the couter for two purposes. 
				   1) RptInfo counter 2) ResourceId allocation */
extern struct timeval global_rpt_timestamp;

/*
 * Representation of an HPI session
 */
struct oh_session {
        /*
          Session ID as returned by saHpiSessionOpen()
        */
        SaHpiSessionIdT	session_id;
        
        /*
	  A session is always associated with exactly one domain
	*/
        SaHpiDomainIdT domain_id;
       	 
	enum {
		OH_EVENT_UNSUBSCRIBE=0,
		OH_EVENT_SUBSCRIBE,
	} event_state;

        /*
          Even if multiple sessions are opened for the same domain,
          each session could receive different events depending on what
          events the caller signs up for.
          
          This is the session specific event queue
          (sld: changed to GSList, as GQueue isn't in glib-1.2)
        */
        GSList *eventq;
};

/*
 *  Representation of a plugin instance
 */
struct oh_handler {
        /* 
           pointer to associated plugin interface
        */
        struct oh_abi_v1 *abi;
        
        /*
          private pointer used by plugin implementations to distinguish
          between different instances
        */
        void *hnd;

	/*
	  This is the list of resources which the handler reports
	 */
	GSList *resource_list;
	
};

/*
 * Representation of an HPI resource
 */
struct oh_resource {
        /*
          Even though a handler initially instantiates this structure
          and hands it off to the infrastructure to manage via the global
          RPT table, the handler only needs a pointer to the oh_resource_id
          in order for the plugin implementation to know what resource
          the abi call is referring to.
        */
        struct oh_resource_id	oid;
        
        /*
          RPT entry visible by the HPI caller
        */
        SaHpiRptEntryT		entry;
     
	/*
	 * The two fields are valid when resource is CAPABILITY_HOTSWAP
	 */
	int controlled;
	SaHpiTimeoutT auto_extract_timeout;
	
	
	/*
	   The handler of the resource
	*/
	struct oh_handler *handler;
	
	/*
	  This is the list of domain ids which contain the handler
	 */
	GSList *domain_list;

        /*
          When the SAHPI_CAPABILITY_RDR flag is set in the
          ResourceCapabilities member of the RPT entry, this 
          contains the list of associated RDR entries
        */
        GSList *rdr_list;

	/*
	  When the SAHPI_CAPABILITY_DOMAIN flag is set in the
	  ResourceCapabilities member of the RPT entry, this
	  is domain id which is conatined by this
	 */
	SaHpiDomainIdT domain_id;
	
	/*
	  this is counter for rdr
	 */
	SaHpiSensorNumT	 sensor_counter; 
};

struct oh_rdr {
        /*
          Even though a handler initially instantiates this structure
          and hands it off to the infrastructure to manage via the global
          RPT table, the handler only needs a pointer to the oh_rdr_id
          in order for the plugin implementation to know what device
          the abi call is referring to.
        */	
        struct oh_rdr_id oid;
        
        /*
          RDR entry visible by the HPI caller
        */
        SaHpiRdrT	 rdr;
};

struct oh_session *session_get(SaHpiSessionIdT);
int session_add(SaHpiDomainIdT, struct oh_session**);
int session_del(struct oh_session*);
/* malloc/copy/add event into the tail of event_list */
int session_push_event(struct oh_session*, struct oh_event*);
/* del/copy/free event from the head of event_list */
int session_pop_event(struct oh_session*, struct oh_event*);
/*query if the session has events*/
int session_has_event(struct oh_session *s);

int is_in_domain_list(SaHpiDomainIdT domain_id);
/* this is used to pre-alllocated domainal id in config */
int add_domain(SaHpiDomainIdT domain_id);
/* this is used to allocate dynamical domain id for handler */
SaHpiDomainIdT new_domain(void);

struct oh_resource *get_res_by_oid(struct oh_resource_id oid);
struct oh_resource *get_resource(SaHpiResourceIdT rid);
struct oh_resource *insert_resource(struct oh_handler *h, struct oh_resource_id oid);
int resource_is_in_domain(struct oh_resource *res, SaHpiDomainIdT sid);

struct oh_rdr *insert_rdr(struct oh_resource *res, struct oh_rdr_id oid);
struct oh_rdr *get_rdr_by_oid(struct oh_resource *res, struct oh_rdr_id oid);

/* plugin load must be seperate from new_handler call*/
int init_plugin(void);
int uninit_plugin(void);
int load_plugin(const char *plugin_name);

/* here are the handler calls we need */
struct oh_handler *new_handler(const char *plugin_name, const char *name, const char *addr);
int free_handler(struct oh_handler*);

/* event handler*/
int get_events(void);

/* howswap */
void process_hotswap_policy(void);
int hotswap_push_event(struct oh_event *e);
int hotswap_pop_event(struct oh_event *e); 
int hotswap_has_event(void);
SaHpiTimeoutT get_hotswap_auto_insert_timeout(void);
void set_hotswap_auto_insert_timeout(SaHpiTimeoutT);


static inline void gettimeofday1(SaHpiTimeT *t)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	*t = (SaHpiTimeT) now.tv_sec * 1000000000 + now.tv_usec*1000;	
}

#define dbg(format, ...)                                      \
        do {							                      \
		fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);   \
                fprintf(stderr, format "\n", ## __VA_ARGS__); \
        } while(0)

#define g_slist_for_each(pos, head) \
	for (pos = head; pos != NULL; pos = g_slist_next(pos))
#endif/*__OPENHPI_H*/
