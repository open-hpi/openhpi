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

#include "ipmi.h"
#include <oh_utils.h>
#include <string.h>

static inline int domain_id_is_equal(const ipmi_domain_id_t id1,
                                     const ipmi_domain_id_t id2)
{
        return (id1.domain == id2.domain);
}

static inline int entity_id_is_equal(const ipmi_entity_id_t id1, 
                                     const ipmi_entity_id_t id2)
{
	return (domain_id_is_equal(id1.domain_id, id2.domain_id)
                && (id1.entity_id == id2.entity_id)
                && (id1.entity_instance == id2.entity_instance)
                && (id1.channel == id2.channel)
                && (id1.address == id2.address)
                && (id1.seq == id2.seq));
}

static inline int mc_id_is_equal(const ipmi_mcid_t id1,
                                 const ipmi_mcid_t id2)
{
        return (domain_id_is_equal(id1.domain_id, id2.domain_id)
                && (id1.mc_num == id2.mc_num)
                && (id1.channel== id2.channel)
                && (id1.seq == id2.seq));
}

static inline int ohoi_resource_info_is_equal(
                const struct ohoi_resource_info info1,
                const struct ohoi_resource_info info2)
{
     /* We don't compare rdr_count */

        if (info1.type != info2.type)
                return 0;
        switch (info1.type) {
                case OHOI_RESOURCE_ENTITY:
                        return (entity_id_is_equal(info1.u.entity_id, 
                                                   info2.u.entity_id));
                case OHOI_RESOURCE_MC:
                        return (mc_id_is_equal(info1.u.mc_id,
                                               info2.u.mc_id));
                default:
                        dbg("UNKNOWN OHOI RESOURCE TYPE!");
                        return 0;
        }
}

/*XXX algorithm here is so ulgy! */
SaHpiRptEntryT *ohoi_get_resource_by_entityid(RPTable                *table,
                                              const ipmi_entity_id_t *entity_id)
{
        struct ohoi_resource_info res_info1;
        SaHpiRptEntryT *rpt_entry;
        
        res_info1.type            = OHOI_RESOURCE_ENTITY;
        res_info1.u.entity_id     = *entity_id;
        
        rpt_entry = oh_get_resource_next(table, SAHPI_FIRST_ENTRY);
        while (rpt_entry) {
                struct ohoi_resource_info *ohoi_res_info;
                ohoi_res_info = oh_get_resource_data(table, rpt_entry->ResourceId);
                if (ohoi_resource_info_is_equal(res_info1, *ohoi_res_info)) {
                        return rpt_entry;
		}
                rpt_entry = oh_get_resource_next(table, 
                                                 rpt_entry->ResourceId);            
        }

	dbg("Not found resource by entity_id");
        return NULL;
}

SaHpiRdrT *ohoi_get_rdr_by_data(RPTable *table,
                                SaHpiResourceIdT rid,
                                SaHpiRdrTypeT  type, 
                                void  *data)
{
        SaHpiRdrT *rdr;
        
        rdr = oh_get_rdr_next(table, rid, SAHPI_FIRST_ENTRY);

        while (rdr) {
              void * data2;

              data2 = oh_get_rdr_data(table, rid, rdr->RecordId);
              if ((type == SAHPI_SENSOR_RDR) && (rdr->RdrType == type)) {
                      ipmi_sensor_id_t *tid1 = data;
                      ipmi_sensor_id_t *tid2 = data2;
                      if (tid1 && tid2) {
                              if (!ipmi_cmp_sensor_id(*tid1, *tid2))
                                      return rdr;
                      }
              }

              rdr = oh_get_rdr_next(table, rid, rdr->RecordId);
        }
        return NULL;
}
