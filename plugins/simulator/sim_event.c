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
 * Authours:
 *      Racing Guo <racing.guo@intel.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <fam.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <uid_utils.h>

#include "sim_util.h"
#include "sim_parser.h"
#include "sim_sensor.h"
#include "sim_event.h"

/*#define trace(x,...) printf(x, __VA_ARGS__)*/
/*#define error(x,...) printf(x, __VA_ARGS__)*/
static  void * REQ_RES = (void *)0;
static  void * REQ_RDR = (void *)1;

static int str2uint32(char *str, SaHpiUint32T *val)
{
#define BYTES_NUM 8
        int i, len;
        SaHpiUint32T  value = 0;

        len = strlen(str);
        if (len > BYTES_NUM || len <= 0)  return -1;
        for (i = 0; i< len; i++) {
                if ((str[i] >= '0') && (str[i] <= '9')) {
                        value = (str[i] - '0') + value*16;
                }else if ((str[i] >= 'a') && (str[i] <= 'f')) {
                        value = (str[i] - 'a'+ 10) + value*16;
                }else {
                        return -1;
                }
        }
        *val = value;
        return 0;
}

static int fhs_event_add_resource(struct fe_handler *feh, char *res)
{
        int retval;
        FAMRequest fr;
        DIR *pdir;
        struct dirent *pd;
        char  *path, *root_path;
        int len;
        struct oh_event *event;
        SaHpiResourceIdT  rid;
        SaHpiRptEntryT *rpt;
        
 
        root_path = g_hash_table_lookup(feh->ohh->config, "root_path");
        len = strlen(root_path) + strlen(res) + 30;
        path = g_malloc(len);

        sprintf(path, "%s/%s", root_path, res);
        pdir = opendir(path);
        if (!pdir) {
            error("%s is not directory\n", path);
            g_free(path);
            return -1;
        }

        event = g_malloc0(sizeof(*event));
        event->type = OH_ET_RESOURCE;
        rpt = &event->u.res_event.entry;
        sprintf(path, "%s/%s/rpt", root_path, res);
        retval = sim_parser_get_rpt(path, rpt);
        if (retval) {
              g_free(path);
              g_free(event);
              return -1;
        } 
        rid = oh_uid_from_entity_path(&rpt->ResourceEntity);
        rpt->ResourceId = rid;

        retval = sim_util_add_resource(feh->ohh->rptcache, rpt, g_strdup(res));
        if (retval) {
              g_free(path);
              g_free(event);
              return -1;
        }

        sim_util_insert_event(&feh->ohh->eventq, event);
        trace("add resource:%x\n", rid);        

        for (pd = readdir(pdir); pd; pd = readdir(pdir)) {
                DIR *tmp;
                unsigned int index;
                sim_rdr_id_t  *sid = NULL;
                SaHpiRdrT *rdr;

                if (str2uint32(pd->d_name, &index)) continue;

                event = g_malloc0(sizeof(*event));
                event->type = OH_ET_RDR;
                rdr = &event->u.rdr_event.rdr;
                sprintf(path, "%s/%s/%s/rdr", root_path, res, pd->d_name);
                retval = sim_parser_get_rdr(path, rdr);
                if (retval) {
                      g_free(event);
                      continue;
                }
                sprintf(path, "%s/%s/%s/sensor", root_path, res, pd->d_name); 
                tmp = opendir(path);
                if (tmp) {
                        closedir(tmp);
                        FAMMonitorDirectory(&feh->fc, path, &fr, REQ_RDR);
                        sid = g_malloc0(sizeof(*sid));
                        sid->rid = rid;
                        sid->index = index;
                        sid->reqnum = fr.reqnum;
                        trace("monitor sensor:(%x,%s) req:%d\n", rid, pd->d_name, sid->reqnum);
                }
           
                retval = sim_util_add_rdr(feh->ohh->rptcache, rid, rdr, sid);
                if (!retval) 
                        sim_util_insert_event(&feh->ohh->eventq, event);
        }
        closedir(pdir);
        g_free(path);
        return 0;
}
static void fhs_event_remove_resource(struct fe_handler *feh, char *res)
{
        struct oh_event    *event;
        SaHpiResourceIdT  rid;
        int retval;
    
        event = g_malloc0(sizeof(*event));
     
        if (event == NULL)
                return;
        retval = sim_util_get_res_id(feh->ohh->rptcache, res, &rid);
        if (retval)
                return;

        event->type = OH_ET_RESOURCE_DEL;
        event->u.res_del_event.resource_id = rid;

        sim_util_insert_event(&feh->ohh->eventq, event);
        sim_util_remove_resource(feh->ohh->rptcache, rid, NULL);
        trace("remove resource :%x\n", rid);

        return;
}

static void fhs_event_sensor_update(struct fe_handler *feh, int reqnum)
{
        sim_rdr_id_t  *sid;

        sid = sim_util_get_rdr_id(feh->ohh->rptcache, reqnum);
        if (sid) {
                trace("update sensor(%x,%x)\n", sid->rid, sid->eid);  
                sim_sensor_update(feh->ohh, sid->rid, sid->eid);
        }
        return;
}

#define IS_DIR(name)  ((*(name)=='/')?1:0)
static void* fhs_event_process(void *data)
{
        struct fe_handler *feh = (struct fe_handler*)data;
        FAMEvent fe;

        while(!feh->closing) { 
/*
        The old code is:
               FAMNextEvent(&fc, &fe);
        The new code is
                if (1 == FAMPending(&fc)) {
                        FAMNextEvent(&fc, &fe);
                }else  continue;

        In the old code, if this thread is processing FAMNextEvent and 
        then feh->closing is set by another thread, this thread is still
        hung up. so the synchronized call is changed as asychronized call
        (polling call) though I don't like this mode.
 
*/  
                if (1 == FAMPending(&feh->fc)) {
                        FAMNextEvent(&feh->fc, &fe);
                }else  continue; 

                if ((fe.userdata == REQ_RES) && (fe.code == FAMCreated)) {
                        if (!IS_DIR(fe.filename))
                                fhs_event_add_resource(feh, fe.filename);
                }else if ((fe.userdata == REQ_RES) && (fe.code == FAMDeleted)) {
                     	if (!IS_DIR(fe.filename)) {
                             	fhs_event_remove_resource(feh, fe.filename);
                        } else
                  		error("why delete root path:%s\n", fe.filename);
                }else if (fe.userdata == REQ_RDR) {
                        if (((fe.code == FAMChanged) || (fe.code == FAMExists))&&
                            ((!strcmp(fe.filename, "reading"))||
                             (!strcmp(fe.filename, "thres")))) {
                                fhs_event_sensor_update(feh, fe.fr.reqnum);
                        }
                }
        }
           
        return 0;
}

struct fe_handler* fhs_event_init(struct oh_handler_state *hnd)
{
        int retval;     
        struct fe_handler *feh;
        FAMRequest fr;
        char*  root_path;
        DIR *pdir;
        struct dirent *pd;

        root_path = g_hash_table_lookup(hnd->config, "root_path");

        feh = (struct fe_handler *)g_malloc0(sizeof(*feh));
        if (feh == NULL)
                return NULL;
        feh->ohh = hnd;
        
        FAMOpen(&feh->fc);
        
        pdir = opendir(root_path);
        for (pd = readdir(pdir); pd; pd = readdir(pdir))
            fhs_event_add_resource(feh, pd->d_name);

        FAMMonitorDirectory(&feh->fc, root_path, &fr, REQ_RES);
  
        retval = pthread_create(&feh->tid, NULL, fhs_event_process, feh);
        if (retval) {
                free(feh);
                return NULL;
        }
        return feh;
}

void fhs_event_finish(struct fe_handler *feh)
{     
        feh->closing = 1;
        pthread_join(feh->tid, NULL);
        FAMClose(&feh->fc);
        g_free(feh);
}
