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
 *     Kevin Gao <kevin.gao@linux.intel.com>
 *     Rusty Lynch <rusty.lynch@linux.intel.com>
 *     Tariq Shureih <tariq.shureih@intel.com>
 *     Racing Guo  <racing.guo@intel.com>
 *     Andy Cress  <andrew.r.cress@intel.com> (watchdog)
 */

#include "ipmi.h"
#include <oh_domain.h>
#include <netdb.h>

/* Watchdog definitions */
#define WATCHDOG_RESET   0x22
#define WATCHDOG_SET     0x24
#define WATCHDOG_GET     0x25
#define NETFN_APP        0x06
#define IPMI_WD_RESOLUTION           100
#define IPMI_WD_PRETIMER_RESOLUTION 1000
#define uchar  unsigned char
extern int ipmi_close_mv(void);
extern int ipmicmd_mv(uchar cmd, uchar netfn, uchar lun, uchar *pdata,
               uchar sdata, uchar *presp, int sresp, int *rlen);

/**
 * This is data structure reference by rsel_id.ptr
 */
struct ohoi_sel_entry {
        ipmi_mc_t       *mc;
        unsigned int    recid;
};

/* global reference count of instances */
static int ipmi_refcount = 0;

/* ABI Interface functions */


/**
 * *ipmi_open: open (initiate) instance of the ipmi plug-in
 * @handler_config: pointer to openhpi config file
 *
 * This function initiates an instance of the ipmi plug-in
 * and opens a new connection to OpenIPMI.
 * Depending on what the config file defines, the connection
 * could be SMI (local) or LAN.
 *
 **/
static void *ipmi_open(GHashTable *handler_config)
{
	struct oh_handler_state *handler;
	struct ohoi_handler *ipmi_handler;

	const char *name;
	const char *addr;
	int rv = 0;

	dbg("ipmi_open");
	if (!handler_config) {
		dbg("No config file provided.....ooops!");
		return(NULL);
	}

	name = g_hash_table_lookup(handler_config, "name");
	addr = g_hash_table_lookup(handler_config, "addr");

	handler = (struct oh_handler_state *)g_malloc0(sizeof(struct oh_handler_state));
	ipmi_handler = (struct ohoi_handler *)g_malloc0(sizeof(struct ohoi_handler));
	if (!handler || !ipmi_handler) {
		dbg("Cannot allocate handler or private ipmi");
		return NULL;
	}	
	handler->data = ipmi_handler;

	handler->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));

	handler->config = handler_config;

	/* Discovery routine depends on these flags */
	ipmi_handler->SDRs_read_done = 0;
	/* Domain (main) SDR flag, 1 when done */
	ipmi_handler->SELs_read_done = 0;
	/* SEL flag, 1 when done */
	ipmi_handler->mc_count = 0;
	/* MC level SDRs, 0 when done */

	//ipmi_handler->FRU_done = 0;
	/* MC level SDRs, 0 when done */

	ipmi_handler->entity_root = g_hash_table_lookup(handler_config, "entity_root");
	
	if ( !name || !addr || !ipmi_handler->entity_root) {
		dbg("Problem getting correct required parameters! \
				check config file");
		return NULL;
	} else
		dbg("name: %s, addr: %s, entity_root: %s", name, addr, ipmi_handler->entity_root);

	/* OS handler allocated first. */
	ipmi_handler->os_hnd = ipmi_posix_get_os_handler();
	sel_alloc_selector(ipmi_handler->os_hnd, &ipmi_handler->ohoi_sel);
	ipmi_posix_os_handler_set_sel(ipmi_handler->os_hnd, ipmi_handler->ohoi_sel);
	ipmi_init(ipmi_handler->os_hnd);
	
	if (strcmp(name, "smi") == 0) {
		int tmp = strtol(addr, (char **)NULL, 10);

		rv = ipmi_smi_setup_con(tmp,ipmi_handler->os_hnd,
					ipmi_handler->ohoi_sel,&ipmi_handler->con);
		if (rv) {
			dbg("Cannot setup connection");
			return NULL;
		}
		ipmi_handler->islan = 0;
	} else if (strcmp(name, "lan") == 0) {
		static struct in_addr lan_addr;
		static int lan_port;
		static int auth;
		static int priv;
			
		char *tok;
		char user[32], passwd[32];

		/* Address */
		tok = g_hash_table_lookup(handler_config, "addr");
		dbg("IPMI LAN Address: %s", tok);
		struct hostent *ent = gethostbyname(tok);
		if (!ent) {
			dbg("Unable to resolve IPMI LAN address");
			return NULL;
		}
			
		memcpy(&lan_addr, ent->h_addr_list[0], ent->h_length);
			
		/* Port */
		tok = g_hash_table_lookup(handler_config, "port");
		lan_port = atoi(tok);
		dbg("IPMI LAN Port: %i", lan_port);

		/* Authentication type */
		tok = g_hash_table_lookup(handler_config, "auth_type");
		if (strcmp(tok, "none") == 0) {
			auth = IPMI_AUTHTYPE_NONE;
		} else if (strcmp(tok, "straight") == 0) {
			auth = IPMI_AUTHTYPE_STRAIGHT;
		} else if (strcmp(tok, "md2") == 0) {
			auth = IPMI_AUTHTYPE_MD2;
		} else if (strcmp(tok, "md5") == 0) {
			auth = IPMI_AUTHTYPE_MD5;
		} else {
			dbg("Invalid IPMI LAN authenication method: %s", tok);
			return NULL;
		}

		dbg("IPMI LAN Authority: %s(%i)", tok, auth);

		/* Priviledge */
		tok = g_hash_table_lookup(handler_config, "auth_level");
		if (strcmp(tok, "callback") == 0) {
			priv = IPMI_PRIVILEGE_CALLBACK;
		} else if (strcmp(tok, "user") == 0) {
			priv = IPMI_PRIVILEGE_USER;
		} else if (strcmp(tok, "operator") == 0) {
			priv = IPMI_PRIVILEGE_OPERATOR;
		} else if (strcmp(tok, "admin") == 0) {
			priv = IPMI_PRIVILEGE_ADMIN;
		} else if (strcmp(tok, "oem") == 0) {
			priv = IPMI_PRIVILEGE_OEM;
		} else {
			dbg("Invalid IPMI LAN authenication method: %s", tok);
			return NULL;
		}
			
		dbg("IPMI LAN Priviledge: %s(%i)", tok, priv);

		/* User Name */
		tok = g_hash_table_lookup(handler_config, "username"); 
		strncpy(user, tok, 32);
		dbg("IPMI LAN User: %s", user);

		/* Password */
		tok = g_hash_table_lookup(handler_config, "password");  
		strncpy(passwd, tok, 32);
		dbg("IPMI LAN Password: %s", passwd);

		free(tok);

		rv = ipmi_lan_setup_con(&lan_addr, &lan_port, 1,
					auth, priv,
					user, strlen(user),
					passwd, strlen(passwd),
					ipmi_handler->os_hnd, ipmi_handler->ohoi_sel,
					&ipmi_handler->con);
		if (rv) dbg("ipmi_lan_setup_con rv = %d",rv);
	        
		ipmi_handler->islan = 1;
	} else {
		dbg("Unsupported IPMI connection method: %s",name);
		return NULL;
	}

	ipmi_handler->connected = -1;

	rv = ipmi_init_domain(&ipmi_handler->con, 1, ipmi_connection_handler, 
			      ipmi_handler, NULL, &ipmi_handler->domain_id);
	if (rv) {
		fprintf(stderr, "ipmi_init_domain: %s\n", strerror(rv));
		return NULL;
	}

	if (ipmi_handler->connected) {
	  	rv = ipmi_domain_pointer_cb(ipmi_handler->domain_id, ohoi_setup_done, handler);
		if (rv) {
		  	fprintf(stderr, "ipmi_domain_pointer_cb: %s\n", strerror(rv));
			return NULL;
		}

		/* increment global count of plug-in instances */
		ipmi_refcount++;

		return handler;
	} else {
	  	dbg("No connection to IPMI domain, failed to instantiate IPMI handler");
	  	return NULL;
	}
}


/**
 * ipmi_close: close this instance of ipmi plug-in
 * @hnd: pointer to handler
 *
 * This functions closes connection with OpenIPMI and frees
 * all allocated events and sensors
 *
 **/
static void ipmi_close(void *hnd)
{

	struct oh_handler_state *handler = (struct oh_handler_state *) hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;


	if (ipmi_handler->connected) {
		dbg("close connection");
		ohoi_close_connection(ipmi_handler->domain_id, ipmi_handler);
	}
	
	ipmi_close_mv();
	ipmi_refcount--;	
	dbg("ipmi_refcount :%d", ipmi_refcount);
	
	if(ipmi_refcount == 0) {
		/* last connection and in case other instances didn't
		   close correctly we clean up all connections */
		dbg("Last connection :%d closing", ipmi_refcount);
		ipmi_shutdown();
	}
	
	oh_flush_rpt(handler->rptcache);
	g_free(handler->rptcache);
	
	g_slist_foreach(handler->eventq, (GFunc)g_free, NULL);
	g_slist_free(handler->eventq);
	
	g_free(ipmi_handler);
	g_free(handler);
}

/**
 * ipmi_get_event: get events populated earlier by OpenIPMI
 * @hnd: pointer to handler
 * @event: pointer to an openhpi event structure
 * @timeout: time to block
 *
 *
 *
 * Return value: 1 or 0
 **/
static int ipmi_get_event(void *hnd, struct oh_event *event)
{
	struct oh_handler_state *handler = hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	int ret = 1;
	int sel_select_done = 0;

	for (;;) {
		if(g_slist_length(handler->eventq)>0) {
			memcpy(event, handler->eventq->data, sizeof(*event));
			event->did = oh_get_default_domain_id();
			//event->did = SAHPI_UNSPECIFIED_DOMAIN_ID;
			free(handler->eventq->data);
			handler->eventq = g_slist_remove_link(handler->eventq, handler->eventq);
			return 1;
		};

		if (sel_select_done == 0) sel_select_done = 1;
		else break;
		while (ret == 1)
			ret = sel_select(ipmi_handler->ohoi_sel, NULL, 0, NULL, NULL);
	};
	return 0;
}


/**
 * ipmi_discover_resources: discover resources in system
 * @hnd: pointer to handler
 *
 * This function is both sepcific to OpenIPMI and conforms to openhpi
 * plug-in interface in waiting until OpenIPMI discovery is done,
 * then retieving entries from the rptcach of the oh_handler_state and
 * populating the eventq for the infrastructure to fetch.
 *
 * Return value: -1 for failure or 0 for success
 **/
int ipmi_discover_resources(void *hnd)
{
      	
  	int rv = 1;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct oh_event *event;
	SaHpiRptEntryT *rpt_entry;
	SaHpiRdrT	*rdr_entry;
	time_t		tm, tm1;
	int		SDR_done = 0, scan_done = 0, mc_count = 0;
	
	struct ohoi_resource_info	*res_info;

	dbg("ipmi discover_resources");
	
	time(&tm);
	while ((0 == ipmi_handler->SDRs_read_done
					|| 0 == ipmi_handler->bus_scan_done
					|| 0 != ipmi_handler->mc_count)
					|| (rv == 1)) {
	      	rv = sel_select(ipmi_handler->ohoi_sel, NULL, 0 , NULL, NULL);
		if (rv < 0) {
			dbg("error on waiting for discovery");
			return SA_ERR_HPI_ERROR;
		};
		if ((rv == 1) || (SDR_done != ipmi_handler->SDRs_read_done)
							|| (scan_done != ipmi_handler->bus_scan_done)
							|| (mc_count != ipmi_handler->mc_count)) {
		      	/* get new timeout for sel_select loop */
			time(&tm);
			SDR_done = ipmi_handler->SDRs_read_done;
			scan_done = ipmi_handler->bus_scan_done;
			mc_count = ipmi_handler->mc_count;
			} else {
			  	time(&tm1);
				/* OpenIPMI takes about 10 seconds to timeout */
				if ((tm1 - tm) > 30) {
					dbg("timeout on waiting for discovery");
					return SA_ERR_HPI_NO_RESPONSE;
				}
			}
			if (!ipmi_handler->connected) {
				fprintf(stderr, "IPMI connection is down\n");
				return SA_ERR_HPI_NO_RESPONSE;
			}
	}
	dbg("Discovery::MC count: %d", ipmi_handler->mc_count);
	
        rpt_entry = oh_get_resource_next(handler->rptcache, SAHPI_FIRST_ENTRY);
	
        while (rpt_entry) {
		res_info = oh_get_resource_data(handler->rptcache, rpt_entry->ResourceId);

		dbg("res: %d presence: %d", rpt_entry->ResourceId, res_info->presence);
		if (res_info->presence == 1 && res_info->updated) {
		      	event = g_malloc0(sizeof(*event));
			memset(event, 0, sizeof(*event));
			event->type = OH_ET_RESOURCE;
	
			memcpy(&event->u.res_event.entry, rpt_entry, sizeof(SaHpiRptEntryT));
			handler->eventq = g_slist_append(handler->eventq, event);
	
			rdr_entry = oh_get_rdr_next(handler->rptcache,
						    	rpt_entry->ResourceId,
							SAHPI_FIRST_ENTRY);
                	while (rdr_entry) {
			  	event = g_malloc0(sizeof(*event));
				memset(event, 0, sizeof(*event));
				event->type = OH_ET_RDR;
				event->u.rdr_event.parent = rpt_entry->ResourceId;
	
				memcpy(&event->u.rdr_event.rdr, rdr_entry, sizeof(SaHpiRdrT));
				handler->eventq = g_slist_append(handler->eventq, event);
					
				rdr_entry = oh_get_rdr_next(handler->rptcache,
		    							rpt_entry->ResourceId,
		    							rdr_entry->RecordId);
			}
				
			rpt_entry = oh_get_resource_next(handler->rptcache, rpt_entry->ResourceId);
		}else {
		  	dbg("Resource %d not present, skipping", rpt_entry->ResourceId);
			rpt_entry = oh_get_resource_next(handler->rptcache, rpt_entry->ResourceId);
		}
		res_info->updated = 0;
	}
	return 0;
}

/**
 * ipmi_get_el_info: get ipmi SEL info
 * @hnd: pointer to handler
 * @id: resource id
 * @info: output -- pointer to info structure passed from infra.
 *
 * This function retrieves the SEL information from
 * the BMC or an SEL capable MC in an IPMI domain
 * 
 *
 * Return value: 0
 **/
static SaErrorT ipmi_get_el_info(void               *hnd,
                             SaHpiResourceIdT   id,
                             SaHpiEventLogInfoT      *info)
{
        unsigned int count;
        unsigned int size;
		int rv;
		char del_support;

        struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
		struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;

        const struct ohoi_resource_info *ohoi_res_info;

		dbg("starting wait for sel retrieval");

		while (0 == ipmi_handler->SELs_read_done) {
			  	rv = sel_select(ipmi_handler->ohoi_sel, NULL, 0 , NULL, NULL);

				if (rv<0) {
					  	dbg("error on waiting for SEL");
						return -1;
				}
		}

		dbg("done retrieving sel");
        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_MC) {
			  	dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }
	
        ohoi_get_sel_count(ohoi_res_info->u.mc_id, &count);
	
		if (count == 0)
			  	info->Entries = 0;
		else
				info->Entries = count;
		dbg("sel count: %d", count);

		ohoi_get_sel_size(ohoi_res_info->u.mc_id, &size);
        info->Size              = size / 16;
        ohoi_get_sel_updatetime(ohoi_res_info->u.mc_id, &info->UpdateTimestamp);
        ohoi_get_sel_time(ohoi_res_info->u.mc_id, &info->CurrentTime, ipmi_handler);
        info->Enabled           = 1; /* FIXME: how to disable SEL in OpenIPMI */
        ohoi_get_sel_overflow(ohoi_res_info->u.mc_id, &info->OverflowFlag);
        info->OverflowAction    = SAHPI_EL_OVERFLOW_DROP;
        ohoi_get_sel_support_del(ohoi_res_info->u.mc_id, &del_support);
        
        return 0;
}

/**
 * ipmi_set_el_time: set ipmi event log time
 * @hnd: pointer to handler
 * @id: resource id of resource holding sel
 * @time: pointer to time structure
 *
 * This functions set the clocl in the event log.
 * 
 *
 * Return value: 0 for success, -1 for error
 **/
static int ipmi_set_el_time(void               *hnd,
                             SaHpiResourceIdT   id,
                             SaHpiTimeT    time)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;

	struct ohoi_resource_info *ohoi_res_info;
        struct timeval tv;
        
	dbg("sel_set_time called");

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_MC) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        tv.tv_sec = time/1000000000;
        tv.tv_usec= (time%1000000000)/1000;
        ohoi_set_sel_time(ohoi_res_info->u.mc_id, &tv, ipmi_handler);
        return 0;
}


/**
 * ipmi_set_sel_state: set ipmi sel state (enabled)
 * @hnd: pointer to handler
 * @id: resource id of resource with SEL capability
 * @enable: int value for enable
 *
 *
 *
 * Return value: SA_OK for success, SA_ERR_HPI_....  for error
 **/
static SaErrorT ipmi_set_sel_state(void      *hnd, 
                SaHpiResourceIdT   id, 
                SaHpiBoolT                    enable)
{
	struct ohoi_resource_info *ohoi_res_info;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	
	ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_MC) {
                dbg("BUG: try to set sel state in unsupported resource");
                return SA_ERR_HPI_CAPABILITY;
        }	
	return ohoi_set_sel_state(ipmi_handler, ohoi_res_info->u.mc_id, enable);
}



#if 0
/**
 * ipmi_set_sel_state: set ipmi sel state (enabled)
 * @hnd: pointer to handler
 * @id: resource id of resource with SEL capability
 * @enable: int value for enable
 *
 *
 *
 * Return value: 0 for success, -1 for error
 **/
static int ipmi_set_sel_state(void      *hnd, 
                //SaHpiResourceIdT   id, 
                //int                     enable)
{
        ///* need OpenIPMI support */
        //return -1;
}

/**
 * ipmi_add_sel_entry: add an entry to system sel from user
 * @hnd: pointer to handler
 * @id: resource id with SEL capability
 * @Event: SaHpiEventLogEntryT pointer to event to be added
 *
 *
 *
 * Return value: -1 for error, success is OpenIPMI command succeed
 **/
static int ipmi_add_sel_entry(void      *hnd, 
                SaHpiResourceIdT   id, 
                const SaHpiEventLogEntryT    *Event)
{
        //ipmi_mc_t *mc = id.ptr; 
        ipmi_msg_t msg;
        unsigned char *buf;
        
        /* Send a software event into Event Receiver IPMI_1_5 9.4 */
        buf = malloc(sizeof(*Event)+1);
        if (!buf) {
                dbg("Out of memory");
                return -1;
        }
        
        msg.netfn       = 0x05;
        msg.cmd         = 0x02;
        msg.data[0]     = 0x41;
        memcpy(msg.data+1, Event, sizeof(*Event));
        msg.data_len    = sizeof(*Event)+1;
        
        //return ipmi_mc_send_command(mc, 0, &msg, NULL, NULL);
	return(-1);
}

static int ipmi_del_sel_entry(void      *hnd,
        		        SaHpiResourceIdT id,
				SaHpiEventLogEntryIdT sid)
{
        //struct ohoi_sel_entry *entry = id.ptr;
        //ipmi_event_t event;
        //event.record_id = entry->recid;
        //_ipmi_mc_del_event(entry->mc, &event, NULL, NULL);
        return -1;
}
#endif






/**
 * ipmi_get_el_entry: get IPMI SEL entry
 * @hnd: pointer to handler instance
 * @id: resourde id with SEL capability
 * @current: SaHpiEntryIdT of entry to retrieve
 * @prev: previous entry in log relative to current
 * @next: next entry in log
 * @entry: [out]SaHpiEventLogEntryT entry requested
 *
 * This function will get event(s) from SEL capable IPMI device
 * one at a time by the record id starting with HPI's
 * SAHPI_OLDEST_ENTRY or SAHPI_NEWEST_ENTRY.
 *
 * Return value: SA_OK for success -1 for failure
 **/
static int ipmi_get_el_entry(void *hnd, SaHpiResourceIdT id,
				SaHpiEventLogEntryIdT current,
				SaHpiEventLogEntryIdT *prev,
				SaHpiEventLogEntryIdT *next,
				SaHpiEventLogEntryT *entry,
				SaHpiRdrT  *rdr,
				SaHpiRptEntryT  *rptentry)
{
        struct ohoi_resource_info *ohoi_res_info;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
        ipmi_event_t		*event;
	SaHpiRptEntryT	*myrpt;
	SaHpiRdrT		*myrdr;
	SaHpiEventTypeT	event_type;
	struct oh_event	*e;
	char				*Data;
	int				data_len;
	ipmi_sensor_id_t	sid;
	ipmi_mcid_t		mc;
	ipmi_entity_id_t	et;
	ipmi_entity_t		*entity;

	ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_MC) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }
	
	if (rdr)
		rdr->RdrType = SAHPI_NO_RECORD;
	if (rptentry) {
		rptentry->ResourceCapabilities = 0;
		rptentry->ResourceId = SAHPI_UNSPECIFIED_RESOURCE_ID;
	}
	
	switch (current) {
		case SAHPI_OLDEST_ENTRY:
			ohoi_get_sel_first_entry(
				ohoi_res_info->u.mc_id, &event);
			if (!event)
				return SA_ERR_HPI_NOT_PRESENT;

			ohoi_get_sel_next_recid(ohoi_res_info->u.mc_id,
						event, next);

			*prev = SAHPI_NO_MORE_ENTRIES;

			break;

		case SAHPI_NEWEST_ENTRY:

			ohoi_get_sel_last_entry(ohoi_res_info->u.mc_id,
					&event);
			if (!event)
				return SA_ERR_HPI_NOT_PRESENT;

			*next = SAHPI_NO_MORE_ENTRIES;

			ohoi_get_sel_prev_recid(ohoi_res_info->u.mc_id,
						event, prev);

			break;

		case SAHPI_NO_MORE_ENTRIES:
			dbg("SEL is empty!");
			if (!event)
				return SA_ERR_HPI_NOT_PRESENT;

		default:                		
			/* get the entry requested by id */
			ohoi_get_sel_by_recid(ohoi_res_info->u.mc_id,
						*next, &event);
			if (!event)
				return SA_ERR_HPI_NOT_PRESENT;
			ohoi_get_sel_next_recid(ohoi_res_info->u.mc_id,
						event, next);

			ohoi_get_sel_prev_recid(ohoi_res_info->u.mc_id,
						event, prev);

			break; 

	}


	entry->EntryId = ipmi_event_get_record_id(event);
	event_type = ipmi_event_get_type(event);
	Data = ipmi_event_get_data_ptr(event);
	data_len = ipmi_event_get_data_len(event);	

	if (event_type == 0x02) {   // sensor event
		do {

			mc = ipmi_event_get_mcid(event);
			sid.mcid = mc;
			sid.lun = Data[5] & 0x3;
			sid.sensor_num = Data[8];

			e = ohoi_sensor_ipmi_event_to_hpi_event(sid,
							event, &entity);
			if (e == NULL) {
				break;
			}
			if (entity != NULL) {
				et = ipmi_entity_convert_to_id(entity);
				myrpt = ohoi_get_resource_by_entityid(
					handler->rptcache, &et);
				myrdr = ohoi_get_rdr_by_data(handler->rptcache,
			              myrpt->ResourceId, SAHPI_SENSOR_RDR,
				      &sid);
				e->u.hpi_event.event.Source = myrpt->ResourceId;
				e->u.hpi_event.event.EventDataUnion.
					SensorEvent.SensorNum =
					       myrdr->RdrTypeUnion.SensorRec.Num;
				if (rptentry) {
					memcpy(rptentry, myrpt, sizeof (*myrpt));
				}
				if (rdr) {
					memcpy(rdr, myrdr, sizeof (myrdr));
				}
			}
			memcpy(&entry->Event, &e->u.hpi_event.event,
			                           sizeof (SaHpiEventT));
			free(e);
			entry->Event.EventType = SAHPI_ET_SENSOR;
			entry->Timestamp =
					ipmi_event_get_timestamp(event);
			return SA_OK;
		} while(0);
		// if we are here we will handle sensor event as user
		// event
	}
	

	entry->Event.Source = SAHPI_UNSPECIFIED_RESOURCE_ID;
	if (data_len != 13) {
		dbg("Strange data len in ipmi event: %d instead of 13\n",
		                  data_len);
		return SA_ERR_HPI_ERROR;
	}
	
	if ((event_type >= 0xC0) && (event_type <= 0xDF)) {
		// OEM timestamp event type
		entry->Timestamp = ipmi_event_get_timestamp(event);
		entry->Event.EventType = SAHPI_ET_OEM;
		entry->Event.Timestamp = entry->Timestamp;
		entry->Event.EventDataUnion.OemEvent.MId = Data[4] |
		                                     (Data[5] << 8) |
						     (Data[6] << 16);
		entry->Event.Severity = SAHPI_DEBUG; // FIX ME
		entry->Event.EventDataUnion.OemEvent.
		       OemEventData.DataLength = 6;
		memcpy(entry->Event.EventDataUnion.OemEvent.
		     OemEventData.Data, Data + 7, data_len);
		entry->Event.EventDataUnion.OemEvent.OemEventData.
		      DataType = SAHPI_TL_TYPE_BINARY;
		entry->Event.EventDataUnion.OemEvent.OemEventData.
		      Language = SAHPI_LANG_UNDEF;
		return SA_OK;
	};
	
	entry->Event.Source = SAHPI_UNSPECIFIED_RESOURCE_ID;
	entry->Event.EventType = SAHPI_ET_USER;
	entry->Event.Timestamp = SAHPI_TIME_UNSPECIFIED;
	entry->Event.Severity = SAHPI_DEBUG; // FIX ME
	entry->Event.EventDataUnion.UserEvent.UserEventData.DataType = 
	                        SAHPI_TL_TYPE_BINARY;
	entry->Event.EventDataUnion.UserEvent.UserEventData.Language =
	                        SAHPI_LANG_UNDEF;
	entry->Event.EventDataUnion.UserEvent.UserEventData.DataLength = 
	                       ipmi_event_get_data_len(event);
        memcpy(entry->Event.EventDataUnion.UserEvent.UserEventData.Data,
               ipmi_event_get_data_ptr(event), 
               ipmi_event_get_data_len(event));		
	return SA_OK;
}


static SaErrorT ipmi_clear_el(void *hnd, SaHpiResourceIdT id)
{
		struct ohoi_resource_info *ohoi_res_info;
		struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
		struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;

		int rv;
		int i;

        ohoi_res_info = oh_get_resource_data(handler->rptcache, id);
        if (ohoi_res_info->type != OHOI_RESOURCE_MC) {
                dbg("BUG: try to get sel in unsupported resource");
                return SA_ERR_HPI_INVALID_CMD;
        }

	ipmi_handler->sel_clear_done = 0;

        rv = ohoi_clear_sel(ohoi_res_info->u.mc_id, ipmi_handler);

	if (rv != SA_OK) {
		dbg("Error in attempting to clear sel");
		return rv;
	}

/*	bug #1046867. It looks process can hang up. 12/9/2004
	while (0 == ipmi_handler->sel_clear_done) {
		rv = sel_select(ipmi_handler->ohoi_sel, NULL, 0 , NULL, NULL);
		if (rv<0) {
			dbg("error on waiting for SEL");
			return -1;
		}
	}

	return SA_OK;
*/
	for (i =0; i < 6; i++) {
		/* long wait - 1 min */
		rv = ohoi_loop(&ipmi_handler->sel_clear_done, ipmi_handler);
		if (rv == SA_OK) {
			break;
		}
	}
	return rv;	
}

SaErrorT ohoi_get_rdr_data(const struct oh_handler_state *handler,
                           SaHpiResourceIdT              id,
                           SaHpiRdrTypeT                 type,
                           SaHpiUint8T                   num,
                           void                          **pdata)
{
        SaHpiRdrT * rdr;
        rdr = oh_get_rdr_by_type(handler->rptcache,
                                 id,
                                 type,
                                 num);
        if (!rdr) {
                /*XXX No err code for invalid rdr?*/
                return SA_ERR_HPI_INVALID_RESOURCE;
        }

        *pdata = oh_get_rdr_data(handler->rptcache,
                                 id,
                                 rdr->RecordId);
        return SA_OK;
}

#define SENSOR_CHECK(handler, sensor_info, id, num)                           \
do {                                                                          \
	SaErrorT         rv;                                                  \
	SaHpiRdrT *rdr;                                                       \
	                                                                      \
	rdr = oh_get_rdr_by_type(handler->rptcache, id,                       \
				 SAHPI_SENSOR_RDR, num);                      \
	if (!rdr) {                                                           \
		dbg("no rdr");                                                \
		return SA_ERR_HPI_NOT_PRESENT;                                \
	}                                                                     \
	                                                                      \
	rv = ohoi_get_rdr_data(handler, id, SAHPI_SENSOR_RDR, num,            \
						(void *)&sensor_info);        \
	if (rv != SA_OK)                                                      \
		return rv;                                                    \
                                                                              \
	if (!sensor_info)                                                     \
		return SA_ERR_HPI_NOT_PRESENT;	                              \
                                                                              \
} while (0)

#define CHECK_SENSOR_ENABLE(sensor_info)                                      \
do {                                                                          \
	if (sensor_info->enable == SAHPI_FALSE)                               \
		return SA_ERR_HPI_INVALID_REQUEST;                            \
} while (0)

/**
 * ipmi_get_sensor_reading: get sensor reading, type, category and other info.
 * @hnd: pointer to handler instance
 * @id: ResourceId -- parent of this sensor
 * @num: sensor number
 * @data: struct returned with data about the sensor.
 *
 *
 *
 * Return value: 0 for success or negative for error
 **/
static int ipmi_get_sensor_reading(void   *hnd, 
				   SaHpiResourceIdT  id,
				   SaHpiSensorNumT  num,
				   SaHpiSensorReadingT *reading,
				   SaHpiEventStateT  *ev_state)
{
	SaErrorT         rv;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;

	SaHpiSensorReadingT tmp_reading;
	SaHpiEventStateT  tmp_state;

	SENSOR_CHECK(handler, sensor_info, id, num);
	CHECK_SENSOR_ENABLE(sensor_info);
	
	rv = ohoi_get_sensor_reading(sensor_info->sensor_id, &tmp_reading,
				     &tmp_state, ipmi_handler);
	if (rv)
		return rv;

	if (reading)
		*reading = tmp_reading;
	if (ev_state)
		*ev_state = tmp_state;

	return SA_OK;
}

/**
* ipmi_get_sensor_thresholds: for hysteresis sensors, get thresholds.
* @hnd: handler instance
* @id: ResourceId parent of this sensor
* @num: sensor number
* @thres: struct returned with data about sensor thresholds.
*
*
*
* Return value: 0 for success or negative for error
**/
static int ipmi_get_sensor_thresholds(void			*hnd, 
				      SaHpiResourceIdT		id,
				      SaHpiSensorNumT		num,
				      SaHpiSensorThresholdsT	*thres)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;

	SENSOR_CHECK(handler, sensor_info, id, num);
	CHECK_SENSOR_ENABLE(sensor_info);
	
	if (!thres)
		return SA_ERR_HPI_INVALID_PARAMS;

	memset(thres, 0, sizeof(*thres));
	return ohoi_get_sensor_thresholds(sensor_info->sensor_id, thres, ipmi_handler);
}

static int ipmi_set_sensor_thresholds(void				*hnd,
				      SaHpiResourceIdT			id,
				      SaHpiSensorNumT			num,
				      const SaHpiSensorThresholdsT	*thres)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;

	SENSOR_CHECK(handler, sensor_info, id, num);
	CHECK_SENSOR_ENABLE(sensor_info);
	
	if (!thres)
		return SA_ERR_HPI_INVALID_PARAMS;
	
	return ohoi_set_sensor_thresholds(sensor_info->sensor_id, thres, ipmi_handler);	
}

static int ipmi_get_sensor_enable(void *hnd, SaHpiResourceIdT id,
				  SaHpiSensorNumT num,
				  SaHpiBoolT *enable)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_sensor_info *sensor_info;

	SENSOR_CHECK(handler, sensor_info, id, num);
	
	if (!enable)
		return SA_ERR_HPI_INVALID_PARAMS;

	*enable = sensor_info->enable;
	return SA_OK;
}


static int ipmi_set_sensor_enable(void *hnd, SaHpiResourceIdT id,
				  SaHpiSensorNumT num,
				  SaHpiBoolT enable)
{
	
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_sensor_info *sensor_info;

	SENSOR_CHECK(handler, sensor_info, id, num);
	sensor_info->enable = enable;
	return SA_OK;	
}

static int ipmi_get_sensor_event_enable(void *hnd, SaHpiResourceIdT id,
					 SaHpiSensorNumT num,
					 SaHpiBoolT *enable)

{
	SaErrorT         rv;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;
	SaHpiBoolT t_enable;
	SaHpiEventStateT t_assert;
	SaHpiEventStateT t_deassert;

	SENSOR_CHECK(handler, sensor_info, id, num);
	
	if (!enable)
		return SA_ERR_HPI_INVALID_PARAMS;

	rv = ohoi_get_sensor_event_enable_masks(sensor_info->sensor_id,
						&t_enable, &t_assert,
						&t_deassert, ipmi_handler);
	if (rv)
		return rv;

	sensor_info->enable = t_enable;
	sensor_info->assert = t_assert;
	sensor_info->deassert = t_deassert;
	
	*enable = t_enable;
	return SA_OK;
}

static int ipmi_set_sensor_event_enable(void *hnd,
					 SaHpiResourceIdT id,
					 SaHpiSensorNumT num,
					 const SaHpiBoolT enable)
{
	
	SaErrorT         rv;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;

	SENSOR_CHECK(handler, sensor_info, id, num);
	
	rv = ohoi_set_sensor_event_enable_masks(sensor_info->sensor_id,
						enable, sensor_info->assert,
						sensor_info->deassert, ipmi_handler);
	if (rv)
		return rv;

	sensor_info->enable = enable;
	return SA_OK;

}
static int ipmi_get_sensor_event_masks(void *hnd, SaHpiResourceIdT id,
				       SaHpiSensorNumT  num,
				       SaHpiEventStateT *assert,
				       SaHpiEventStateT *deassert)
{
	
	SaErrorT         rv;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;
	SaHpiBoolT t_enable;
	SaHpiEventStateT t_assert;
	SaHpiEventStateT t_deassert;

	SENSOR_CHECK(handler, sensor_info, id, num);
	
	if (!assert || !deassert)
		return SA_ERR_HPI_INVALID_PARAMS;

	rv = ohoi_get_sensor_event_enable_masks(sensor_info->sensor_id,
						&t_enable, &t_assert,
						&t_deassert, ipmi_handler);
	if (rv)
		return rv;

	sensor_info->enable = t_enable;
	sensor_info->assert = t_assert;
	sensor_info->deassert = t_deassert;

	*assert = t_assert;
	*deassert = t_deassert;

	return SA_OK;
}

static int ipmi_set_sensor_event_masks(void *hnd, SaHpiResourceIdT id,
   			  	       SaHpiSensorNumT num,
				       SaHpiSensorEventMaskActionT act,
			 	       SaHpiEventStateT  assert,
				       SaHpiEventStateT  deassert)
{
	SaErrorT         rv;
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler = (struct ohoi_handler *)handler->data;
	struct ohoi_sensor_info *sensor_info;
	SaHpiEventStateT t_assert;
	SaHpiEventStateT t_deassert;
	
	SENSOR_CHECK(handler, sensor_info, id, num);
	
	if (act == SAHPI_SENS_ADD_EVENTS_TO_MASKS) {
		t_assert = assert | sensor_info->assert;
		t_deassert = deassert | sensor_info->deassert;
	} else if (act == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS) {
		t_assert = (assert ^ 0xffff) & sensor_info->assert;
		t_deassert = (deassert ^ 0xffff) & sensor_info->deassert;
	} else 
		return SA_ERR_HPI_INVALID_PARAMS;	
	
	rv = ohoi_set_sensor_event_enable_masks(sensor_info->sensor_id,
						sensor_info->enable, t_assert,
						t_deassert, ipmi_handler);
	if (rv)
		return rv;

	sensor_info->assert = t_assert;
	sensor_info->deassert = t_deassert;

	return SA_OK;
}

static int ipmi_get_watchdog_info(void *hnd,
         SaHpiResourceIdT  id,
         SaHpiWatchdogNumT num,
         SaHpiWatchdogT    *watchdog)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler =(struct ohoi_handler *)handler->data;
	unsigned char reqdata[16];
	unsigned char response[16];
	int rlen;
	int rv;

	/* 
	 * OpenIPMI library doesn't have watchdog calls, so talk 
	 * directly to the driver (via ipmi_drv.c). 
	 * Currently support only default watchdog num, and only local.
	 */
	if (ipmi_handler->islan) return(SA_ERR_HPI_UNSUPPORTED_API);
	if (num != SAHPI_DEFAULT_WATCHDOG_NUM) 
		return SA_ERR_HPI_INVALID_PARAMS;	
	rlen = sizeof(response);
	rv = ipmicmd_mv(WATCHDOG_GET,NETFN_APP,0,reqdata,0,response,rlen,&rlen);
	if (rv != 0) return(rv);
	rv = response[0];  /*completion code*/
	if (rv != 0) return(rv);

#if DEBUG
	dbg("wdog_get: %02x %02x %02x %02x %02x %02x %02x %02x\n",
		response[1], response[2], response[3], response[4], 
		response[5], response[6], response[7], response[8]); 
#endif

	/* Translate IPMI response to HPI format */
	memset(watchdog, 0, sizeof(SaHpiWatchdogT));
	if (response[1] & 0x80) watchdog->Log = SAHPI_FALSE;
	else watchdog->Log = SAHPI_TRUE;
	if (response[1] & 0x40) watchdog->Running = SAHPI_TRUE;
	else watchdog->Running = SAHPI_FALSE;
	switch(response[1] & 0x07)
	{
	case 0x01:
            watchdog->TimerUse = SAHPI_WTU_BIOS_FRB2;
            break;
        case 0x02:
            watchdog->TimerUse = SAHPI_WTU_BIOS_POST;
            break;
        case 0x03:
            watchdog->TimerUse = SAHPI_WTU_OS_LOAD;
            break;
        case 0x04:
            watchdog->TimerUse = SAHPI_WTU_SMS_OS;
            break;
        case 0x05:
            watchdog->TimerUse = SAHPI_WTU_OEM;
            break;
        default:
            watchdog->TimerUse = SAHPI_WTU_UNSPECIFIED;
            break;
	}

	switch (response[2] & 0x70)
	{
        case 0x00:
            watchdog->PretimerInterrupt = SAHPI_WPI_NONE;
            break;
        case 0x10:
            watchdog->PretimerInterrupt = SAHPI_WPI_SMI;
            break;
        case 0x20:
            watchdog->PretimerInterrupt = SAHPI_WPI_NMI;
            break;
        case 0x30:
            watchdog->PretimerInterrupt = SAHPI_WPI_MESSAGE_INTERRUPT;
            break;
        default :
            watchdog->PretimerInterrupt = SAHPI_WPI_NONE;
            // watchdog->PretimerInterrupt = SAHPI_WPI_OEM;
            break;
	}
    
	switch (response[2] & 0x07)
	{
        case 0x00:
            watchdog->TimerAction = SAHPI_WA_NO_ACTION;
            break;
        case 0x01:
            watchdog->TimerAction = SAHPI_WA_RESET;
            break;
        case 0x02:
            watchdog->TimerAction = SAHPI_WA_POWER_DOWN;
            break;
        case 0x03:
            watchdog->TimerAction = SAHPI_WA_POWER_CYCLE;
            break;
        default:
            watchdog->TimerAction = SAHPI_WA_NO_ACTION;
            break;
	}
 
	watchdog->PreTimeoutInterval =
		response[3] * IPMI_WD_PRETIMER_RESOLUTION;

	watchdog->TimerUseExpFlags = 0;
	if (response[4] & 0x02)
	   watchdog->TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_BIOS_FRB2;
	if (response[4] & 0x04)
	   watchdog->TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_BIOS_POST;
	if (response[4] & 0x08)
	   watchdog->TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_OS_LOAD;
	if (response[4] & 0x10)
	   watchdog->TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_SMS_OS;
	if (response[4] & 0x20)
	   watchdog->TimerUseExpFlags |= SAHPI_WATCHDOG_EXP_OEM;

	watchdog->InitialCount = (response[5] + (response[6] * 256)) * IPMI_WD_RESOLUTION;
	watchdog->PresentCount = (response[7] + (response[8] * 256)) * IPMI_WD_RESOLUTION;

	return rv;
}

static int ipmi_set_watchdog_info(void *hnd,
         SaHpiResourceIdT  id,
         SaHpiWatchdogNumT num,
         SaHpiWatchdogT    *watchdog)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler =(struct ohoi_handler *)handler->data;
	unsigned char reqdata[16];
	unsigned char response[16];
	int rlen;
	int tv;
	int rv;

	if (ipmi_handler->islan) return(SA_ERR_HPI_UNSUPPORTED_API);

	if (num != SAHPI_DEFAULT_WATCHDOG_NUM) 
		return SA_ERR_HPI_INVALID_PARAMS;	
	/* translate HPI values to IPMI */
	switch (watchdog->TimerUse)
	{
        case SAHPI_WTU_BIOS_FRB2:
            reqdata[0] = 0x01;
            break;
        case SAHPI_WTU_BIOS_POST:
            reqdata[0] = 0x02;
            break;
        case SAHPI_WTU_OS_LOAD:
            reqdata[0] = 0x03;
            break;
        case SAHPI_WTU_SMS_OS:
            reqdata[0] = 0x04;
            break;
        case SAHPI_WTU_OEM:
            reqdata[0] = 0x05;
            break;
        case SAHPI_WTU_NONE:
        default:
            reqdata[0] = 0x00;
	}
	if (watchdog->Log == SAHPI_FALSE)    reqdata[0] |= 0x80;;
	if (watchdog->Running == SAHPI_TRUE) reqdata[0] |= 0x40;;

	switch (watchdog->TimerAction)
	{
        case SAHPI_WA_RESET:
	    reqdata[1] = 0x01;
            break;
        case SAHPI_WA_POWER_DOWN:
	    reqdata[1] = 0x02;
            break;
        case SAHPI_WA_POWER_CYCLE:
	    reqdata[1] = 0x03;
            break;
        case SAHPI_WA_NO_ACTION:
        default:
	    reqdata[1] = 0x00;
	}

	switch (watchdog->PretimerInterrupt)
	{
        case SAHPI_WPI_SMI:
	    reqdata[1] |= 0x10;
            break;
        case SAHPI_WPI_NMI:
	    reqdata[1] |= 0x20;
            break;
        case SAHPI_WPI_MESSAGE_INTERRUPT:
	    reqdata[1] |= 0x30;
            break;
        case SAHPI_WPI_OEM:
	    /* Undefined, so treat it like SAHPI_WPI_NONE */
        case SAHPI_WPI_NONE:
        default:
	    break;
	}

	tv = watchdog->PreTimeoutInterval / IPMI_WD_PRETIMER_RESOLUTION;
	reqdata[2] = tv & 0x00ff;

	reqdata[3] = 0;
	if (watchdog->TimerUseExpFlags & SAHPI_WATCHDOG_EXP_BIOS_FRB2)
		reqdata[3] |= 0x02;
	if (watchdog->TimerUseExpFlags & SAHPI_WATCHDOG_EXP_BIOS_POST)
		reqdata[3] |= 0x04;
	if (watchdog->TimerUseExpFlags & SAHPI_WATCHDOG_EXP_OS_LOAD)
		reqdata[3] |= 0x08;
	if (watchdog->TimerUseExpFlags & SAHPI_WATCHDOG_EXP_SMS_OS)
		reqdata[3] |= 0x10;
	if (watchdog->TimerUseExpFlags & SAHPI_WATCHDOG_EXP_OEM)
		reqdata[3] |= 0x20;

    	if ((watchdog->InitialCount < IPMI_WD_RESOLUTION) &&
            (watchdog->InitialCount != 0)) 
	     tv = IPMI_WD_RESOLUTION;
	else tv = watchdog->InitialCount / IPMI_WD_RESOLUTION;
	reqdata[4] = tv & 0x00ff;  // tv % 256; 
	reqdata[5] = tv / 256;   // (tv & 0xff00) >> 8;

#if DEBUG
	dbg("wdog_set: %02x %02x %02x %02x %02x %02x\n",
		reqdata[0], reqdata[1], reqdata[2],
		reqdata[3], reqdata[4], reqdata[5]); 
#endif

	rlen = sizeof(response);
	rv = ipmicmd_mv(WATCHDOG_SET,NETFN_APP,0,reqdata,6,response,rlen,&rlen);
	if (rv != 0) return(rv);
	rv = response[0];  /*completion code*/

	return rv;	
}

static int ipmi_reset_watchdog(void *hnd,
         SaHpiResourceIdT  id,
         SaHpiWatchdogNumT num)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	struct ohoi_handler *ipmi_handler =(struct ohoi_handler *)handler->data;
	unsigned char response[16];
	int rlen;
	int rv;

	if (ipmi_handler->islan) return(SA_ERR_HPI_UNSUPPORTED_API);

	if (num != SAHPI_DEFAULT_WATCHDOG_NUM) 
		return SA_ERR_HPI_INVALID_PARAMS;	

	rlen = sizeof(response);
	rv = ipmicmd_mv(WATCHDOG_RESET,NETFN_APP,0,NULL,0,response,rlen,&rlen);
	if (rv != 0) return(rv);
	rv = response[0];  /*completion code*/
	if (rv != 0) return(rv);

	return rv;	
}

static
void ohoi_set_resource_tag(ipmi_entity_t *entity, void *cb_data)
{
		SaHpiTextBufferT *tag = cb_data;
		ipmi_entity_set_entity_id_string(entity, (char *)tag);
		dbg("New resource Tag set");
}

static SaErrorT ipmi_set_res_tag (void 			*hnd,
				  SaHpiResourceIdT	id,
				  SaHpiTextBufferT	*tag)
{
	struct oh_handler_state *handler = (struct oh_handler_state *)hnd;
	SaHpiRptEntryT *rpt_entry;
	struct ohoi_resource_info *res_info;
	int rv;

	res_info = oh_get_resource_data(handler->rptcache, id);
	if (!res_info)
			dbg("No private resource info for resource %d", id);
	
	rpt_entry = oh_get_resource_by_id(handler->rptcache, id);
	if (!rpt_entry) {
		dbg("No rpt for resource %d?", id);
		return  SA_ERR_HPI_NOT_PRESENT;
	}

	/* do it in openIPMI's memory first for subsequest updates */

	/* but first check if it's an entity or an MC */
	if (res_info->type == OHOI_RESOURCE_MC) {
			dbg("Setting custom tag for Management Controllers is not supported");
			return SA_ERR_HPI_INVALID_CMD;
	} else {
			/* can only be an Entity in the ohoi_resource_info struct */
			dbg("Setting new Tag: %s for resource: %d", (char *) tag->Data, id);
			rv = ipmi_entity_pointer_cb(res_info->u.entity_id, ohoi_set_resource_tag,
				       			tag->Data);
			if (rv)
				dbg("Error retrieving entity pointer for resource %d",
					       rpt_entry->ResourceId);
	}
	
	/* change it in our memory as well */
	memcpy(&rpt_entry->ResourceTag.Data, tag->Data, strlen(tag->Data) + 1);
	oh_add_resource(handler->rptcache, rpt_entry, res_info, 1);
	return 0;
}
				  


static struct oh_abi_v2 oh_ipmi_plugin = {
		
	/* basic ABI functions */
	.open	 			= ipmi_open,
	.close				= ipmi_close,
	.get_event			= ipmi_get_event,
	.discover_resources		= ipmi_discover_resources,
	.set_resource_tag		= ipmi_set_res_tag,

	/* SEL support */
	.get_el_info                    = ipmi_get_el_info,
	.set_el_time                    = ipmi_set_el_time,
	//.add_sel_entry                  = ipmi_add_sel_entry,
	//.del_sel_entry                  = ipmi_del_sel_entry,
	.get_el_entry                   = ipmi_get_el_entry,
	.clear_el                       = ipmi_clear_el,
	.set_el_state                 =  ipmi_set_sel_state,

	/* Sensor support */
	.get_sensor_reading		= ipmi_get_sensor_reading,
	.get_sensor_thresholds		= ipmi_get_sensor_thresholds,
	.set_sensor_thresholds		= ipmi_set_sensor_thresholds,
	.get_sensor_enable		= ipmi_get_sensor_enable,
	.set_sensor_enable		= ipmi_set_sensor_enable,
	.get_sensor_event_enables       = ipmi_get_sensor_event_enable,
	.set_sensor_event_enables       = ipmi_set_sensor_event_enable,
	.get_sensor_event_masks		= ipmi_get_sensor_event_masks,
	.set_sensor_event_masks		= ipmi_set_sensor_event_masks,
	
	/* Inventory support */
	.get_idr_info			= ohoi_get_idr_info,
	.get_idr_area_header		= ohoi_get_idr_area_header,
	.add_idr_area			= ohoi_add_idr_area,
	.del_idr_area			= ohoi_del_idr_area,
	.get_idr_field			= ohoi_get_idr_field,
	.add_idr_field			= ohoi_add_idr_field,
	.set_idr_field			= ohoi_set_idr_field,
	.del_idr_field			= ohoi_del_idr_field,
 
	/* hotswap support */
	.get_hotswap_state              = ohoi_get_hotswap_state,
	.set_hotswap_state              = ohoi_set_hotswap_state,
	.request_hotswap_action         = ohoi_request_hotswap_action,
	.get_indicator_state            = ohoi_get_indicator_state,
	.set_indicator_state            = ohoi_set_indicator_state,
        
	/* power support */
	.get_power_state                = ohoi_get_power_state,
	.set_power_state                = ohoi_set_power_state,
	
	/* reset support */
	.get_reset_state                = ohoi_get_reset_state,
	.set_reset_state                = ohoi_set_reset_state,

        /* control support */
	.set_control_state 		= ohoi_set_control_state,
	.get_control_state 		= ohoi_get_control_state,

	/* watchdog support */
	.get_watchdog_info    		= ipmi_get_watchdog_info,
	.set_watchdog_info    		= ipmi_set_watchdog_info,
	.reset_watchdog       		= ipmi_reset_watchdog,
};

int ipmi_get_interface(void **pp, const uuid_t uuid);
int ipmi_get_interface(void **pp, const uuid_t uuid)
{
	if (uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
		*pp = &oh_ipmi_plugin;
		return 0;
	}
printf("uuid_compare false\n");
	*pp = NULL;
	return -1;
}

int get_interface(void **pp, const uuid_t uuid) __attribute__ ((weak, alias("ipmi_get_interface")));
