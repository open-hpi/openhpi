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

#ifndef __OPENHPI_H
#define __OPENHPI_H

#include <stdio.h>

#include <list.h>
#include <hash.h>

#include <oh_plugin.h>

struct oh_session {
	struct list_head node;	/* link-node in session table */
	struct list_head dnode;	/* link-node in the same domain */
	SaHpiSessionIdT	sid;
	struct oh_domain *domain;

	struct list_head event_list; /* event list in the session */
};

struct oh_domain {
	struct list_head node;	/* link-node in domain table */
	SaHpiDomainIdT did;


	SaHpiUint32T update_counter;
	struct timeval update_time;

	SaHpiResourceIdT res_counter;
	struct list_head res_list;	/*res list in the domain */
	
	struct oh_abi_v1 *abi;
	void *hnd;

	struct list_head session_list;	/* session list in the domain*/
};

struct oh_resource {
	struct list_head	node;

	struct oh_resource_id	oid;
	SaHpiRptEntryT		entry;
	
	/* 
	 * when entry.ResourceCapabilities | SAHPI_CAPABILITY_DOMAIN 
	 * The entry.DomainId identies the domain which the resource
	 * contains
	 */
	
	/* valid when entry.ResourceCapabilities | SAHPI_CAPABILITY_SEL */
	struct oh_sel		*sel;
	/* valid when entry.ResourceCapabilities | SAHPI_CAPABILITY_RDR */
	struct list_head	rdr_list;
	
	/* These counters are used to assign corresponding Num to rdr */
	SaHpiSensorNumT		sensor_counter;
	/* following counter will be added when such RDR type is supported
	SaHpiCtrlNumT	 ctrl_counter;
	SaHpiWatchdogNumT wd_counter;
	SaHpiEirIdT	inv_counter;
	*/
};

struct oh_rdr {
	struct list_head node;
	
	struct oh_rdr_id oid;
	SaHpiRdrT	 rdr;
};


int init_session(void);	
int uninit_session(void);
struct oh_session *session_get(SaHpiSessionIdT sid);
int session_add(struct oh_domain *d, struct oh_session **s);
int session_del(struct oh_session *s);
int session_get_events(struct oh_session *s);
int session_discover_resources(struct oh_session *s);
/* malloc/copy/add event into the tail of event_list */
int session_push_event(struct oh_session *s, struct oh_event *e);
/* del/copy/free event from the head of event_list */
int session_pop_event(struct oh_session *s, struct oh_event *e);


int init_domain(void);
int uninit_domain(void);
struct oh_domain *domain_get(SaHpiDomainIdT did);
struct oh_domain *domain_add(void);
int domain_process_event(struct oh_domain *d, struct oh_event *e);

struct oh_resource *get_res_by_oid(struct oh_domain *d, struct oh_resource_id oid);
struct oh_resource *get_resource(struct oh_domain *d, SaHpiResourceIdT rid);
struct oh_resource *insert_resource(struct oh_domain *d, struct oh_resource_id oid);

struct oh_rdr *insert_rdr(struct oh_resource *res, struct oh_rdr_id oid);

int init_plugin(void);
int uninit_plugin(void);
int load_plugin(const char*, const char*, const char*);

#define dbg(format, ...)                                      \
        do {							                      \
		fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);   \
                fprintf(stderr, format "\n", ## __VA_ARGS__); \
        } while(0)

#endif/*__OPENHPI_H*/
