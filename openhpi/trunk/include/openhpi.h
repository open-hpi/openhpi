/* BSD License
 * Copyright (C) by Intel Crop.
 * Author: Louis Zhuang <louis.zhuang@linux.intel.com>
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
	struct list_head node;
	SaHpiResourceIdT rid;
	struct oh_id	 oid;
	union {
		/* valid when OH_DOMAIN */
		struct oh_domain *domain;
		/* valid when OH_SEL */
		struct oh_sel *sel;
		/* valid when OH_ENTITY */
		struct list_head rdr_list;
	} u;
	SaHpiRptEntryT rpt_entry;
	int present:1;
};

struct oh_rdr {
	struct list_head node;
	struct oh_id	 oid;
	union {
		/* valid when OH_SENSOR */
		struct oh_sensor	*sensor;
		/* valid when OH_CONTROL */
		struct oh_control	*control;
		/* valid when OH_INVENTORY */
		struct oh_inventory	*inventory;
		/* valid when OH_WATCHDOG */
		struct oh_watchdog	*watchdog;
	} u;
};

struct oh_sensor {
};


int init_session(void);	
int uninit_session(void);
struct oh_session *session_get(SaHpiSessionIdT sid);
int session_add(struct oh_domain *d, struct oh_session **s);
int session_del(struct oh_session *s);
int session_get_events(struct oh_session *s);
/* malloc/copy/add event into the tail of event_list */
int session_push_event(struct oh_session *s, struct oh_event *e);
/* del/copy/free event from the head of event_list */
int session_pop_event(struct oh_session *s, struct oh_event *e);


int init_domain(void);
int uninit_domain(void);
struct oh_domain *domain_get(SaHpiDomainIdT did);
int domain_add(struct oh_domain **d);
int domain_del(struct oh_domain *d);
int domain_process_event(struct oh_domain *d, struct oh_event *e);


struct oh_resource *get_res(struct oh_domain *d, SaHpiResourceIdT rid);
int absent_entity(struct oh_domain *d, struct oh_id *oid);
int present_entity(struct oh_domain *d, struct oh_id *oid);


int init_plugin(void);
int uninit_plugin(void);

#define dbg(format, ...)                                     \
        do {							\
		fprintf(stderr, "%s:%d: " format "\n",		\
			__FUNCTION__, __LINE__, ## __VA_ARGS__);	\
        } while(0)

#endif/*__OPENHPI_H*/
