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
 *     Racing Guo <racing.guo@intel.com>
 */
#ifndef _INC_IPMI_H_
#define _INC_IPMI_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <OpenIPMI/ipmiif.h>
#include <OpenIPMI/ipmi_sel.h>
#include <OpenIPMI/ipmi_smi.h>
#include <OpenIPMI/ipmi_err.h>
#include <OpenIPMI/ipmi_auth.h>
#include <OpenIPMI/ipmi_lan.h>
#include <OpenIPMI/selector.h>
#include <OpenIPMI/ipmi_int.h>
#include <OpenIPMI/os_handler.h>
#include <OpenIPMI/ipmi_domain.h>
#include <OpenIPMI/ipmi_event.h>
#include <OpenIPMI/ipmi_posix.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <oh_handler.h>



#define IPMI_DATA_WAIT	5
#define OEM_ALARM_BASE  0x10

extern selector_t       *ohoi_sel;

struct ohoi_handler {
	GStaticRecMutex ohoih_lock;
	int SDRs_read_done;
	int bus_scan_done;
    	int SELs_read_done;
	int mc_count;			/* to keep track of num of mcs to wait on sdrs */
	int sel_clear_done;		/* we need to wait for mc_sel_reread for clear to succeed */
//	int FRU_done;			/* we have to track FRUs */

	ipmi_domain_id_t domain_id;

	/* OpenIPMI connection and os_handler */
	os_handler_t *os_hnd;
	ipmi_con_t *con;

	selector_t *ohoi_sel;

	char *entity_root;
	int connected;
	int islan;
};

struct ohoi_resource_info {
	  	
  	int presence;	/* entity presence from OpenIPMI to determine
			   	to push RPT to domain RPTable or not */
	int updated;	/* refcount of resource add/update from
			   	rptcache to domain RPT */
	SaHpiUint8T  sensor_count; 
        SaHpiUint8T  ctrl_count; 

        enum {
                OHOI_RESOURCE_ENTITY = 0,
                OHOI_RESOURCE_MC
        } type;
        union {
                ipmi_entity_id_t entity_id;
                ipmi_mcid_t      mc_id;
        } u;

        ipmi_control_id_t reset_ctrl;
        ipmi_control_id_t power_ctrl;
};

struct ohoi_sensor_info {
	ipmi_sensor_id_t sensor_id;
	int valid;
	SaHpiBoolT enable;
	SaHpiEventStateT  assert;
	SaHpiEventStateT  deassert;
};

/* implemented in ipmi_event.c */
void ohoi_setup_done(ipmi_domain_t *domain, void *user_data);
/* implemented in ipmi_close.c */
void ohoi_close_connection(ipmi_domain_id_t domain_id, void *user_data);

/* implemented in ipmi_sensor.c	*/
int ohoi_get_sensor_reading(ipmi_sensor_id_t sensor_id, 
			    SaHpiSensorReadingT * reading,
			    SaHpiEventStateT * ev_state,
			    void *cb_data);

int ohoi_get_sensor_thresholds(ipmi_sensor_id_t sensor_id,
			       SaHpiSensorThresholdsT *thres,
			       void *cb_data);

int ohoi_set_sensor_thresholds(ipmi_sensor_id_t                 sensor_id, 
                               const SaHpiSensorThresholdsT     *thres,
			       void *cb_data);

int ohoi_set_sensor_enable(ipmi_sensor_id_t sensor_id,
			   SaHpiBoolT   enable,
			   void *cb_data);

int ohoi_get_sensor_event_enable_masks(ipmi_sensor_id_t sensor_id,
				       SaHpiBoolT   *enable,
				       SaHpiEventStateT  *assert,
				       SaHpiEventStateT  *deassert,
				       void *cb_data);

int ohoi_set_sensor_event_enable_masks(ipmi_sensor_id_t sensor_id,
			               SaHpiBoolT enable,
			 	       SaHpiEventStateT  assert,
				       SaHpiEventStateT  deassert,
				       void *cb_data);

void ohoi_get_sel_time(ipmi_mcid_t mc_id, SaHpiTimeT *time, void *cb_data);
void ohoi_set_sel_time(ipmi_mcid_t mc_id, const struct timeval *time, void *cb_data);
void ohoi_get_sel_updatetime(ipmi_mcid_t mc_id, SaHpiTimeT *time);
void ohoi_get_sel_size(ipmi_mcid_t mc_id, int *size);
void ohoi_get_sel_count(ipmi_mcid_t mc_id, int *count);
void ohoi_get_sel_overflow(ipmi_mcid_t mc_id, char *overflow);
void ohoi_get_sel_support_del(ipmi_mcid_t mc_id, char *support_del);
SaErrorT ohoi_clear_sel(ipmi_mcid_t mc_id, void *cb_data);
SaErrorT ohoi_set_sel_state(struct ohoi_handler *ipmi_handler, ipmi_mcid_t mc_id, int enable);
void ohoi_get_sel_first_entry(ipmi_mcid_t mc_id, ipmi_event_t **event);
void ohoi_get_sel_last_entry(ipmi_mcid_t mc_id, ipmi_event_t **event);
void ohoi_get_sel_next_recid(ipmi_mcid_t mc_id, 
                             ipmi_event_t *event,
                             unsigned int *record_id);
void ohoi_get_sel_prev_recid(ipmi_mcid_t mc_id, 
                             ipmi_event_t *event, 
                             unsigned int *record_id);
void ohoi_get_sel_by_recid(ipmi_mcid_t mc_id, SaHpiEventLogEntryIdT entry_id, ipmi_event_t **event);

/* This is used to help plug-in to find resource in rptcache by entity_id */
SaHpiRptEntryT *ohoi_get_resource_by_entityid(RPTable                *table,
                                              const ipmi_entity_id_t *entity_id);

/* This is used to help plug-in to find rdr in rptcache by data*/
SaHpiRdrT *ohoi_get_rdr_by_data(RPTable *table,
                                SaHpiResourceIdT rid,
                                SaHpiRdrTypeT type,
                                void *data);

/* This is used for OpenIPMI to notice sensor change */
void ohoi_sensor_event(enum ipmi_update_e op,
                       ipmi_entity_t      *ent,
                       ipmi_sensor_t      *sensor,
                       void               *cb_data);       
/*
 * This is used to help saHpiEventLogEntryGet()
 * to convert sensor ipmi event to hpi event
 */
struct oh_event *ohoi_sensor_ipmi_event_to_hpi_event(ipmi_sensor_id_t	sid,
			ipmi_event_t	*event, ipmi_entity_t **entity);


/* This is used for OpenIPMI to notice control change */
void ohoi_control_event(enum ipmi_update_e op,
                        ipmi_entity_t      *ent,
                        ipmi_control_t     *control,
                        void		   *cb_data);

/* This is used for OpenIPMI to notice mc change */
void ohoi_mc_event(enum ipmi_update_e op,
                   ipmi_domain_t      *domain,
                   ipmi_mc_t          *mc,
                   void               *cb_data);

/* This is used for OpenIPMI to noice inventroy change */
void ohoi_inventory_event(enum ipmi_update_e    op,
                          ipmi_entity_t         *entity,
                          void                  *cb_data);

/* This is used for OpenIPMI to notice entity change */
void ohoi_entity_event(enum ipmi_update_e       op,
                       ipmi_domain_t            *domain,
                       ipmi_entity_t            *entity,
                       void                     *cb_data);

int ohoi_loop(int *done_flag, struct ohoi_handler *ipmi_handler);
/**
 * loop_indicator_cb:
 * @cb_data: callback data
 *
 * Use to indicate if the loop 
 * can end
 *
 * Return value: non-zero means 
 * end.
 **/
typedef int (*loop_indicator_cb)(const void *cb_data);
int ohoi_loop_until(loop_indicator_cb indicator, const void *cb_data, int timeout, struct ohoi_handler *ipmi_handler); 

SaErrorT ohoi_get_rdr_data(const struct oh_handler_state *handler,
                           SaHpiResourceIdT              id,
                           SaHpiRdrTypeT                 type,
                           SaHpiUint8T                   num,
                           void                          **pdata);

SaErrorT ohoi_get_idr_info(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrInfoT *idrinfo);
SaErrorT ohoi_get_idr_area_header(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrAreaTypeT areatype,
                              SaHpiEntryIdT areaid,  SaHpiEntryIdT *nextareaid, SaHpiIdrAreaHeaderT *header);
SaErrorT ohoi_add_idr_area(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrAreaTypeT areatype, SaHpiEntryIdT *areaid);
SaErrorT ohoi_del_idr_area(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiEntryIdT areaid);
SaErrorT ohoi_get_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid,
                             SaHpiEntryIdT areaid, SaHpiIdrFieldTypeT fieldtype, SaHpiEntryIdT fieldid,
                             SaHpiEntryIdT *nextfieldid, SaHpiIdrFieldT *field);
SaErrorT ohoi_add_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrFieldT *field );
SaErrorT ohoi_set_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiIdrFieldT *field );
SaErrorT ohoi_del_idr_field(void *hnd, SaHpiResourceIdT rid, SaHpiIdrIdT idrid, SaHpiEntryIdT areaid, SaHpiEntryIdT fieldid);

int ohoi_hot_swap_cb(ipmi_entity_t  *ent,
                     enum ipmi_hot_swap_states last_state,
		     enum ipmi_hot_swap_states curr_state,
		     void   *cb_data,
		     ipmi_event_t  *event);

SaErrorT ohoi_get_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT *state);

SaErrorT ohoi_set_hotswap_state(void *hnd, SaHpiResourceIdT id, 
                                SaHpiHsStateT state);

SaErrorT ohoi_request_hotswap_action(void *hnd, SaHpiResourceIdT id, 
                                     SaHpiHsActionT act);

SaErrorT ohoi_get_indicator_state(void *hnd, SaHpiResourceIdT id, 
                                  SaHpiHsIndicatorStateT *state);

SaErrorT ohoi_set_indicator_state(void *hnd, SaHpiResourceIdT id, 
				  SaHpiHsIndicatorStateT state);

SaErrorT ohoi_set_power_state(void *hnd, SaHpiResourceIdT id, 
                              SaHpiPowerStateT state);

SaErrorT ohoi_get_power_state(void *hnd, SaHpiResourceIdT id,
			      SaHpiPowerStateT *state);
	
SaErrorT ohoi_set_reset_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiResetActionT act);

SaErrorT ohoi_get_reset_state(void *hnd, SaHpiResourceIdT id, 
		              SaHpiResetActionT *act);

SaErrorT ohoi_get_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlModeT *mode,
                                SaHpiCtrlStateT *state);

SaErrorT ohoi_set_control_state(void *hnd, SaHpiResourceIdT id,
                                SaHpiCtrlNumT num,
                                SaHpiCtrlModeT mode,
                                SaHpiCtrlStateT *state);

void ipmi_connection_handler(ipmi_domain_t	*domain,
			      int		err,
			      unsigned int	conn_num,
			      unsigned int	port_num,
			      int		still_connected,
			      void		*cb_data);
/* misc macros for debug */
#define dump_entity_id(s, x) \
        do { \
                dbg("%s domain id: %p, entity id: %x, entity instance: %x, channel: %x, address: %x, seq: %lx", \
                     s,                         \
                     (x).domain_id.domain,      \
                     (x).entity_id,             \
                     (x).entity_instance,       \
                     (x).channel,               \
                     (x).address,               \
                     (x).seq);                  \
        } while(0)


/* dump rpttable to make debug easy 
   if you don't like it, feel free to delete it.
   IMO, it is a good idea to place dump_rpttable in rpt_utils.c

*/

static inline void  dump_rpttable(RPTable *table)
{
       SaHpiRptEntryT *rpt;
       rpt = oh_get_resource_next(table, SAHPI_FIRST_ENTRY);

       printf("\n");
       while (rpt) {
               SaHpiRdrT  *rdr;

               printf("Resource Id:%d\n", rpt->ResourceId);
               rdr = oh_get_rdr_next(table, rpt->ResourceId, SAHPI_FIRST_ENTRY);
               while (rdr) {
                       unsigned char *data;
                       int i;
                       data = oh_get_rdr_data(table, rpt->ResourceId, rdr->RecordId);
			/*FIXME:: causes issue on IA64 */
			/* commenting out for now until fixed */
                       //printf("(Rdr id:%d type:%d) data pointer:%u\n", 
                               //rdr->RecordId,
                               //rdr->RdrType,
                               //(unsigned)(void *)data);
                       if (data)
                               for (i = 0; i < 30; i++)
                                       printf("%u ", data[i]);
                       printf("\n");
                       rdr = oh_get_rdr_next(table, rpt->ResourceId, rdr->RecordId);
               }
               printf("\n");
               rpt = oh_get_resource_next(table, rpt->ResourceId);
      }
}

#endif

/* called when a resource is removed (swapped?) */
int ipmi_discover_resources(void *hnd);
void entity_rpt_set_updated(struct ohoi_resource_info *res_info,
		struct ohoi_handler *hnd);
void entity_rpt_set_presence(struct ohoi_resource_info *res_info,
		struct ohoi_handler *hnd, int presence);
void ohoi_remove_entity(struct oh_handler_state *handler,
			SaHpiResourceIdT res_id);

