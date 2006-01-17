/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Renier Morales <renierm@users.sf.net>
 *        Daniel de Araujo <ddearauj@us.ibm.com>
 */
#include <rtas_event.h> 

SaErrorT rtas_get_event(void *hnd, struct oh_event *event)
{
        struct oh_handler_state *h = (struct oh_handler_state *)hnd;

        if (!event || !hnd) {
                dbg("Invalid parameter");
                return(SA_ERR_HPI_INVALID_PARAMS);
        }

        if (g_slist_length(h->eventq) > 0) {
                memcpy(event, h->eventq->data, sizeof(struct oh_event));
                free(h->eventq->data);
                h->eventq = g_slist_remove_link(h->eventq, h->eventq);
                return 1;
        }

        return SA_OK;
}

void * oh_get_event (void *, struct oh_event *)
        __attribute__ ((weak, alias("rtas_get_event")));
