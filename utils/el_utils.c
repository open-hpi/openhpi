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

/* allocate and initialize an EL */
oh_el *oh_el_create(SaHpiUint32T size)
{
        oh_el *el;

        el = (oh_el *) g_malloc0(sizeof(oh_el));
        if (el != NULL) {
                el->enabled = TRUE;
                el->overflow = FALSE;
                el->gentimestamp = TRUE;
                el->lastUpdate = SAHPI_TIME_UNSPECIFIED;
                el->offset = 0;
                el->maxsize = size;
                el->nextId = SAHPI_OLDEST_ENTRY + 1; // always start at 1
                el->elentries = NULL;
        }
        return el;
}


/* close and free all memory associated with an EL */
SaErrorT oh_el_close(oh_el *el)
{

        if (el == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        oh_el_clear(el);
        free(el);
        return SA_OK;
}


/* add a new entry to the EL
 *
 * This API will be removed in a later version.
 * You should use oh_el_append instead.
 */
SaErrorT oh_el_add(oh_el *el, SaHpiEventT *event)
{
        return oh_el_append(el, event);
}


/* append a new entry to the EL */
SaErrorT oh_el_append(oh_el *el, SaHpiEventT *event)
{
        SaHpiEventLogEntryT *entry;
        time_t tt1;
        GList *temp;

        /* check for valid el params and state */
        if (el == NULL || event == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (el->enabled == FALSE) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* alloc the new entry */
        entry = (SaHpiEventLogEntryT *) g_malloc0(sizeof(SaHpiEventLogEntryT));
        if (entry == NULL) {
                el->overflow = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        /* if necessary, wrap the el entries */
        if (el->maxsize != OH_EL_MAX_SIZE && g_list_length(el->elentries) == el->maxsize) {
                gpointer tempdata;
                temp = g_list_first(el->elentries);
                tempdata = temp->data;
                el->elentries = g_list_remove(el->elentries, temp->data);
                g_free(tempdata);
        }

        /* append the new entry */
        entry->EntryId = el->nextId;
        el->nextId++;
        if (el->gentimestamp) {
                time(&tt1);
                el->lastUpdate = (SaHpiTimeT) (tt1 * 1000000000) + el->offset;
        } else {
                el->lastUpdate = event->Timestamp;
        }
        entry->Timestamp = el->lastUpdate;
        memcpy(&(entry->Event), event, sizeof(SaHpiEventT));
        el->elentries = g_list_append(el->elentries, entry);
        return SA_OK;
}


/* prepend a new entry to the EL */
SaErrorT oh_el_prepend(oh_el *el, SaHpiEventT *event)
{
        SaHpiEventLogEntryT *entry, *tmpentry;
        time_t tt1;
        GList *ellist;

        /* check for valid el params and state */
        if (el == NULL || event == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (el->enabled == FALSE) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* see if el is full */
        if (el->maxsize != OH_EL_MAX_SIZE && g_list_length(el->elentries) == el->maxsize) {
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        /* alloc the new entry */
        entry = (SaHpiEventLogEntryT *) g_malloc0(sizeof(SaHpiEventLogEntryT));
        if (entry == NULL) {
                el->overflow = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        /* since we are adding entries in reverse order we have to renumber
         * existing entries
         */
        ellist = g_list_first(el->elentries);
        while (ellist != NULL) {
                tmpentry = (SaHpiEventLogEntryT *) ellist->data;
                tmpentry->EntryId++;
                ellist = g_list_next(ellist);
        }

        /* prepend the new entry */
        entry->EntryId = 1;
        el->nextId++;
        if (el->gentimestamp) {
                time(&tt1);
                el->lastUpdate = (SaHpiTimeT) (tt1 * 1000000000) + el->offset;
        } else {
                el->lastUpdate = event->Timestamp;
        }
        entry->Timestamp = el->lastUpdate;
        memcpy(&(entry->Event), event, sizeof(SaHpiEventT));
        el->elentries = g_list_prepend(el->elentries, entry);
        return SA_OK;
}


/* delete an entry in the EL (not supported, per errata) */
SaErrorT oh_el_delete(oh_el *el, SaHpiEntryIdT *entryid)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}


/* clear all EL entries */
SaErrorT oh_el_clear(oh_el *el)
{
        GList *temp;

        if (el == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        if (el->enabled) {
                /* free the list data elements */
                temp = g_list_first(el->elentries);
                while (temp != NULL) {
                        g_free(temp->data);
                        temp = g_list_next(temp);
                }
                /* free the list nodes */
                g_list_free(el->elentries);
                /* reset the control structure */
                el->overflow = FALSE;
                el->lastUpdate = SAHPI_TIME_UNSPECIFIED;
                el->nextId = SAHPI_OLDEST_ENTRY + 1; // always start at 1
                el->elentries = NULL;
                return SA_OK;
        }
        return SA_ERR_HPI_INVALID_REQUEST;
}


/* get an EL entry */
SaErrorT oh_el_get(oh_el *el, SaHpiEventLogEntryIdT entryid, SaHpiEventLogEntryIdT *prev,
                    SaHpiEventLogEntryIdT *next, SaHpiEventLogEntryT **entry)
{
        SaHpiEventLogEntryT *myentry;
        GList *ellist;
        SaHpiEventLogEntryIdT srchentryid, firstid, lastid;

        if (el == NULL || prev == NULL || next == NULL || entry == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (g_list_length(el->elentries) == 0) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        /* get the first and last entry ids for possible translation */
        ellist = g_list_last(el->elentries);
        myentry = (SaHpiEventLogEntryT *)ellist->data;
        lastid = myentry->EntryId;
        ellist = g_list_first(el->elentries);
        myentry = (SaHpiEventLogEntryT *)ellist->data;
        firstid = myentry->EntryId;
        if (entryid == SAHPI_NEWEST_ENTRY) {
                srchentryid = lastid;
        }
        else if (entryid == SAHPI_OLDEST_ENTRY) {
                srchentryid = firstid;
        }
        else {
                srchentryid = entryid;
        }

        ellist = g_list_first(el->elentries);
        while (ellist != NULL) {
                myentry = (SaHpiEventLogEntryT *) ellist->data;
                if (srchentryid == myentry->EntryId) {
                        *entry = myentry;
                        /* is this the first entry? */
                        if (myentry->EntryId == firstid) {
                                *prev = SAHPI_NO_MORE_ENTRIES;
                        }
                        else {
                                *prev = myentry->EntryId - 1;
                        }
                        /* is this the last entry? */
                        if (myentry->EntryId == lastid) {
                                *next = SAHPI_NO_MORE_ENTRIES;
                        }
                        else {
                                *next = myentry->EntryId + 1;
                        }
                        return SA_OK;
                }
                else if (entryid < myentry->EntryId) {
                        return SA_ERR_HPI_NOT_PRESENT;
                }
                ellist = g_list_next(ellist);
        }
        return SA_ERR_HPI_NOT_PRESENT;
}


/* get EL info */
SaErrorT oh_el_info(oh_el *el, SaHpiEventLogInfoT *info)
{
        time_t tt1;

        if (el == NULL || info == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        info->Entries = g_list_length(el->elentries);
        info->Size = el->maxsize;
        info->UpdateTimestamp = el->lastUpdate;
        time(&tt1);
        info->CurrentTime = (SaHpiTimeT) (tt1 * 1000000000) + el->offset;
        info->Enabled = el->enabled;
        info->OverflowFlag = el->overflow;
        info->OverflowAction = SAHPI_EL_OVERFLOW_OVERWRITE;
        return SA_OK;
}


/* write a EL entry list to a file */
SaErrorT oh_el_map_to_file(oh_el *el, char *filename)
{
        int file;
        GList *ellist;

        if (el == NULL || filename == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0660 );
        if (file < 0) {
                dbg("EL file '%s' could not be opened", filename);
                return SA_ERR_HPI_ERROR;
        }

        ellist = g_list_first(el->elentries);
        while (ellist != NULL) {
                write(file, (void *)ellist->data, sizeof(SaHpiEventLogEntryT));
                ellist = g_list_next(ellist);
        }

        if(close(file) != 0) {
                dbg("Couldn't close file '%s'.", filename);
                return SA_ERR_HPI_ERROR;
        }

        return SA_OK;
}


/* read a EL entry list from a file */
SaErrorT oh_el_map_from_file(oh_el *el, char *filename)
{
        int file;
        SaHpiEventLogEntryT entry;
        SaErrorT retc;

        /* check el params and state */
        if (el == NULL || filename == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if (el->enabled == FALSE) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        file = open(filename, O_RDONLY);
        if (file < 0) {
                dbg("EL file '%s' could not be opened", filename);
                return SA_ERR_HPI_ERROR;
        }

        oh_el_clear(el); // ensure list is empty
        while (read(file, &entry, sizeof(SaHpiEventLogEntryT)) == sizeof(SaHpiEventLogEntryT)) {
                el->nextId = entry.EntryId;
                /* Need a way to preserve the original entry's timestamp
                 * if that is of use. -- RM
                 */
                retc = oh_el_add(el, &(entry.Event));
                if (retc) {
                        close(file);
                        return retc;
                }
        }

        if(close(file) != 0) {
                dbg("Couldn't close file '%s'.", filename);
                return SA_ERR_HPI_ERROR;
        }

        return SA_OK;
}


/* set the EL timestamp offset */
SaErrorT oh_el_timeset(oh_el *el, SaHpiTimeT timestamp)
{
        if (el == NULL || timestamp == SAHPI_TIME_UNSPECIFIED) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (timestamp > SAHPI_TIME_MAX_RELATIVE) {
                /* We accept absolute timestamp here to be
                   compliant with the spec. But we set it to zero
                   anyway because we use time() to get the current
                   time (absolute) to stamp new entries with.
                */
                timestamp = 0;
        }

        el->offset = timestamp;
        return SA_OK;
}


/* set the timestamp generate flag */
SaErrorT oh_el_setgentimestampflag(oh_el *el, SaHpiBoolT flag)
{
        if (el == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        el->gentimestamp = flag;
        return SA_OK;
}
