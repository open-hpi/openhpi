/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003-2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Sean Dague <sdague@users.sf.net>
 */

#ifndef __SAHPIMACROS_H
#define __SAHPIMACROS_H

/**************************************************************
 *  
 *  These macros are defined for clarity of the sahpi.c
 *  source file.  The provide standard mechanisms for
 *  checking for and populating standard types, as well
 *  as providing consistent debug and error functionality.
 *
 *  Yes, macros are evil, but they make this code much
 *  more readable.
 *
 ***********************************************************/

static enum {
        OH_STAT_UNINIT,
        OH_STAT_READY,
} oh_hpi_state = OH_STAT_UNINIT;

#define OH_STATE_READY_CHECK                                      \
        do {                                                      \
                if (OH_STAT_READY != oh_hpi_state) {              \
                        dbg("Uninitialized HPI");                 \
                        data_access_unlock();                     \
                        return SA_ERR_HPI_ERROR;          \
                }                                                 \
        } while(0)

/*
 * OH_SESSION_SETUP gets the session pointer for the session
 * id.  It returns badly if required.  This is only to be used for sahpi
 * function.
 */

#define OH_SESSION_SETUP(sid, ses)                         \
        do {                                               \
                ses = session_get(sid);                    \
                if (!ses) {                                \
                        dbg("Invalid SessionId %d", sid);  \
                        data_access_unlock();              \
                        return SA_ERR_HPI_INVALID_SESSION; \
                }                                          \
        } while (0)

/*
 * OH_HANDLER_GET gets the hander for the rpt and resource id.  It
 * returns INVALID PARAMS if the handler isn't there
 */
#define OH_HANDLER_GET(rpt,rid,h)                                       \
        do {                                                            \
                struct oh_resource_data *rd;                            \
                if(rpt == NULL) {                                       \
                        dbg("Invalide RPTable");                        \
                        data_access_unlock();                           \
                        return SA_ERR_HPI_INVALID_SESSION;              \
                }                                                       \
                rd = oh_get_resource_data(rpt, rid);                    \
                if(!rd || !rd->handler) {                               \
                        dbg("Can't find handler for Resource %d", rid); \
                        data_access_unlock();                           \
                        return SA_ERR_HPI_INVALID_PARAMS;               \
                }                                                       \
                h = rd->handler;                                        \
        } while(0)

/*
 * OH_RPT_GET - sets up rpt table, and does a sanity check that it
 * is actually valid, returning badly if it isn't
 */

#define OH_RPT_GET(SessionId, rpt)                                     \
        do {                                                           \
                rpt = default_rpt;                                     \
                if(rpt == NULL) {                                      \
                        dbg("RPT for Session %d not found",SessionId); \
                        data_access_unlock();                          \
                        return SA_ERR_HPI_INVALID_SESSION;             \
                }                                                      \
        } while(0)


/*
 * OH_RESOURCE_GET gets the resource for an resource id and rpt
 * it returns invalid resource if no resource id is found
 */
#define OH_RESOURCE_GET(rpt, rid, r)                            \
        do {                                                    \
                r = oh_get_resource_by_id(rpt, rid);            \
                if(!r) {                                        \
                        dbg("Resource %d doesn't exist", rid);  \
                        data_access_unlock();                   \
                        return SA_ERR_HPI_INVALID_RESOURCE;     \
                }                                               \
        } while(0)

#endif
