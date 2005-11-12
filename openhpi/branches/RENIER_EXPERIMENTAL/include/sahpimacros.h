/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004, 2005
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
 *     Renier Morales <renierm@users.sf.net>
 */

#ifndef __SAHPIMACROS_H
#define __SAHPIMACROS_H

/**************************************************************
 *
 *  These macros are defined for clarity of the sahpi.c
 *  source file.  They provide standard mechanisms for
 *  checking for and populating standard types, as well
 *  as providing consistent debug and error functionality.
 *
 *  Yes, macros are evil, but they make this code much
 *  more readable.
 *
 ***********************************************************/

#define OH_CHECK_INIT_STATE(sid) \
        do { \
                SaHpiBoolT state; \
                SaErrorT init_error; \
                if (oh_initialized() != SA_OK) { \
                        dbg("Session %d not initalized", sid); \
                        return SA_ERR_HPI_INVALID_SESSION; \
                } else if ((init_error = oh_get_session_subscription(sid,&state)) != SA_OK) { \
                        dbg("Session %d is not valid", sid); \
                        return SA_ERR_HPI_INVALID_SESSION; \
                } \
        } while (0)


/*
 * OH_GET_DID gets the domain id of a session and
 * checks its validity (nonzero). *
 */
#define OH_GET_DID(sid, did) \
        do { \
                if (!(did = oh_get_session_domain(sid))) { \
                        dbg("No domain for session id %d",sid); \
                        return SA_ERR_HPI_INVALID_SESSION; \
                } \
        } while (0)

/*
 * OH_GET_DOMAIN gets the domain object which locks it.
 * Need to call oh_release_domain(domain) after this to unlock it.
 */
#define OH_GET_DOMAIN(did, d) \
        do { \
                if (!(d = oh_get_domain(did))) { \
                        dbg("Domain %d doesn't exist", did); \
                        return SA_ERR_HPI_INVALID_DOMAIN; \
                } \
        } while (0)

/*
 * OH_HANDLER_GET gets the hander for the rpt and resource id.  It
 * returns INVALID PARAMS if the handler isn't there
 */
#define OH_HANDLER_GET(d, rid, h) \
        do { \
                struct oh_resource_data *rd; \
                rd = oh_get_resource_data(&(d->rpt), rid); \
                if(!rd || !rd->hid) { \
                        dbg("Can't find handler for Resource %d in Domain %d", rid, d->id); \
                        oh_release_domain(d); \
                        return SA_ERR_HPI_INVALID_RESOURCE; \
                } \
                h = oh_get_handler(rd->hid); \
        } while (0)

/*
 * OH_RESOURCE_GET gets the resource for an resource id and rpt
 * it returns invalid resource if no resource id is found
 */

#define OH_RESOURCE_GET(d, rid, r) \
        do { \
                r = oh_get_resource_by_id(&(d->rpt), rid); \
                if (!r) { \
                        dbg("Resource %d in Domain %d doesn't exist", rid, d->id); \
                        oh_release_domain(d); \
                        return SA_ERR_HPI_INVALID_RESOURCE; \
                } \
        } while (0)

/*
 * OH_RESOURCE_GET_CHECK gets the resource for an resource id and rpt
 * it returns invalid resource if no resource id is found. It will
 * return NO_RESPONSE if the resource is marked as being failed.
 */
#define OH_RESOURCE_GET_CHECK(d, rid, r) \
        do { \
                r = oh_get_resource_by_id(&(d->rpt), rid); \
                if (!r) { \
                        dbg("Resource %d in Domain %d doesn't exist", rid, d->id); \
                        oh_release_domain(d); \
                        return SA_ERR_HPI_INVALID_RESOURCE; \
                } else if (r->ResourceFailed != SAHPI_FALSE) { \
                        dbg("Resource %d in Domain %d is Failed", rid, d->id); \
                        oh_release_domain(d); \
                        return SA_ERR_HPI_NO_RESPONSE; \
                } \
        } while (0)

#endif
