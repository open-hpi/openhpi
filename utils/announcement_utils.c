/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
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
 *      David Ashley <dashley@us.ibm.com>
 *      Renier Morales <renierm@users.sf.net>
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>

/* allocate and initialize an announcement list */
oh_announcement *oh_announcement_create(void)
{
        oh_announcement *ann;

        ann = (oh_announcement *)g_malloc0(sizeof(oh_announcement));
        if (ann != NULL) {
                ann->nextId = SAHPI_OLDEST_ENTRY + 1; // always start at 1
                ann->annentries = NULL;
        }
        return ann;
}


/* close and free all memory associated with an announcement list */
SaErrorT oh_announcement_close(oh_announcement *ann)
{

        if (ann == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oh_announcement_clear(ann);
        free(ann);
        return SA_OK;
}


/* append a new entry to the announcement list */
SaErrorT oh_announcement_append(oh_announcement *ann,
                                const SaHpiAnnunciatorNumT num,
                                SaHpiAnnouncementT *myann)
{
        oh_ann_entry *entry;
        time_t tt1;

        /* check for valid el params and state */
        if (ann == NULL || myann == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* alloc and copy the new entry */
        entry = (oh_ann_entry *) g_malloc0(sizeof(oh_ann_entry));
        if (entry == NULL) {
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
        memcpy(&entry->annentry, myann, sizeof(SaHpiAnnouncementT));
        entry->num = num;

        /* initialize the struct and append the new entry */
        entry->annentry.EntryId = ann->nextId;
        ann->nextId++;
        time(&tt1);
        entry->annentry.Timestamp = ((SaHpiTimeT) tt1) * 1000000000;
        entry->annentry.AddedByUser = TRUE;
        ann->annentries = g_list_append(ann->annentries, entry);

        /* copy the new generated info back to the user's announcement */
        memcpy(myann, &entry->annentry, sizeof(SaHpiAnnouncementT));

        return SA_OK;
}


/* clear all announcement entries */
SaErrorT oh_announcement_clear(oh_announcement *ann)
{
        GList *temp;

        if (ann == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        /* free the list data elements */
        temp = g_list_first(ann->annentries);
        while (temp != NULL) {
                g_free(temp->data);
                temp = g_list_next(temp);
        }
        /* free the list nodes */
        g_list_free(ann->annentries);
        /* reset the control structure */
        ann->nextId = SAHPI_OLDEST_ENTRY + 1; // always start at 1
        ann->annentries = NULL;

        return SA_OK;
}


/* get an announcement entry */
SaErrorT oh_announcement_get(oh_announcement *ann, SaHpiAnnunciatorNumT num,
                             SaHpiEntryIdT srchid, SaHpiAnnouncementT *entry)
{
        oh_ann_entry *myentry;
        GList *annlist;

        if (ann == NULL || entry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (g_list_length(ann->annentries) == 0) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        annlist = g_list_first(ann->annentries);
        while (annlist != NULL) {
                myentry = (oh_ann_entry *) annlist->data;
                if (myentry->num == num && srchid == myentry->annentry.EntryId) {
                        memcpy(entry, &myentry->annentry, sizeof(SaHpiAnnouncementT));
                        return SA_OK;
                }
                annlist = g_list_next(annlist);
        }
        return SA_ERR_HPI_NOT_PRESENT;
}


/* get next announcement entry */
SaErrorT oh_announcement_get_next(oh_announcement *ann,
                                  SaHpiAnnunciatorNumT num,
                                  SaHpiAnnouncementT *entry)
{
        oh_ann_entry *myentry;
        GList *annlist;
        int notinset;

        if (ann == NULL || entry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (g_list_length(ann->annentries) == 0) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* get the first physical entry */
        annlist = g_list_first(ann->annentries);
        myentry = (oh_ann_entry *) annlist->data;

        if (entry->EntryId == SAHPI_FIRST_ENTRY) {
                /* find the first logical entry */
                notinset = FALSE;
                while (annlist != NULL) {
                        if (myentry->num != num)
                                notinset = TRUE;
                        if (notinset == FALSE &&
                            entry->Severity != SAHPI_ALL_SEVERITIES &&
                            entry->Severity == myentry->annentry.Severity)
                                notinset = TRUE;
                        if (notinset == FALSE &&
                            entry->Acknowledged == TRUE &&
                            myentry->annentry.Acknowledged != TRUE)
                                notinset = TRUE;
                        if (notinset == FALSE)
                                break;
                        notinset = FALSE;
                        annlist = g_list_next(annlist);
                        myentry = (oh_ann_entry *) annlist->data;
                }
                if (annlist == NULL) {
                        return SA_ERR_HPI_NOT_PRESENT;
                }
                memcpy(entry, &myentry->annentry, sizeof(SaHpiAnnouncementT));
                return SA_OK;
        }

        /* find the start matching entry */
        while (annlist != NULL &&
               entry->EntryId != myentry->annentry.EntryId &&
               entry->Timestamp != myentry->annentry.Timestamp &&
               myentry->num != num) {
                annlist = g_list_next(annlist);
                myentry = (oh_ann_entry *) annlist->data;
        }
        if (annlist == NULL) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* find the next logical entry */
        notinset = FALSE;
        annlist = g_list_next(annlist);
        myentry = (oh_ann_entry *) annlist->data;
        while (annlist != NULL) {
                if (myentry->num != num)
                        notinset = TRUE;
                if (notinset == FALSE &&
                    entry->Severity != SAHPI_ALL_SEVERITIES &&
                    entry->Severity == myentry->annentry.Severity)
                        notinset = TRUE;
                if (notinset == FALSE &&
                    entry->Acknowledged == TRUE &&
                    myentry->annentry.Acknowledged != TRUE)
                        notinset = TRUE;
                if (notinset == FALSE)
                        break;
                notinset = FALSE;
                annlist = g_list_next(annlist);
                myentry = (oh_ann_entry *) annlist->data;
        }
        if (annlist == NULL) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* found a matching announcement */
        memcpy(entry, &myentry->annentry, sizeof(SaHpiAnnouncementT));
        return SA_OK;
}

