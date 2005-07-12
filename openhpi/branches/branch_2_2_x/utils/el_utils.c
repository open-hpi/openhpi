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


/* append a new entry to the EL */
SaErrorT oh_el_append(oh_el *el, const SaHpiEventT *event, const SaHpiRdrT *rdr,
                      const SaHpiRptEntryT *res)
{
        oh_el_entry *entry;
        time_t tt1;
        GList *temp;

        /* check for valid el params and state */
        if (el == NULL || event == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        if ((el->enabled == FALSE) && (event->EventType != SAHPI_ET_USER)) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* alloc the new entry */
        entry = (oh_el_entry *) g_malloc0(sizeof(oh_el_entry));
        if (entry == NULL) {
                el->overflow = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
        if (rdr != NULL) {
                memcpy(&(entry->rdr), rdr, sizeof(SaHpiRdrT));
        }
        if (res != NULL) {
                memcpy(&(entry->res), res, sizeof(SaHpiRptEntryT));
        }

        /* if necessary, wrap the el entries */
        if (el->maxsize != OH_EL_MAX_SIZE && g_list_length(el->elentries) == el->maxsize) {
                gpointer tempdata;
                temp = g_list_first(el->elentries);
                tempdata = temp->data;
                el->elentries = g_list_remove(el->elentries, temp->data);
                g_free(tempdata);
                el->overflow = SAHPI_TRUE;
        }

        /* append the new entry */
        entry->event.EntryId = el->nextId;
        el->nextId++;
        if (el->gentimestamp) {
                time(&tt1);
                el->lastUpdate = ((SaHpiTimeT) tt1 * 1000000000) + el->offset;
        } else {
                el->lastUpdate = event->Timestamp;
        }
        entry->event.Timestamp = el->lastUpdate;
        memcpy(&(entry->event.Event), event, sizeof(SaHpiEventT));
        el->elentries = g_list_append(el->elentries, entry);
        return SA_OK;
}


/* prepend a new entry to the EL */
SaErrorT oh_el_prepend(oh_el *el, const SaHpiEventT *event, const SaHpiRdrT *rdr,
                       const SaHpiRptEntryT *res)
{
        oh_el_entry *entry, *tmpentry;
        SaHpiEventLogEntryT *tmplog;
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
        entry = (oh_el_entry *) g_malloc0(sizeof(oh_el_entry));
        if (entry == NULL) {
                el->overflow = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
        if (rdr != NULL) {
                memcpy(&(entry->rdr), rdr, sizeof(SaHpiRdrT));
        }
        if (res != NULL) {
                memcpy(&(entry->res), res, sizeof(SaHpiRptEntryT));
        }

        /* since we are adding entries in reverse order we have to renumber
         * existing entries
         */
        ellist = g_list_first(el->elentries);
        while (ellist != NULL) {
                tmpentry = (oh_el_entry *) ellist->data;
                tmplog = (SaHpiEventLogEntryT *) &(tmpentry->event);
                tmplog->EntryId++;
                ellist = g_list_next(ellist);
        }

        /* prepend the new entry */
        entry->event.EntryId = 1;
        el->nextId++;
        if (el->gentimestamp) {
                time(&tt1);
                el->lastUpdate = ((SaHpiTimeT) tt1 * 1000000000) + el->offset;
        } else {
                el->lastUpdate = event->Timestamp;
        }
        entry->event.Timestamp = el->lastUpdate;
        memcpy(&(entry->event.Event), event, sizeof(SaHpiEventT));
        el->elentries = g_list_prepend(el->elentries, entry);
        return SA_OK;
}


/* clear all EL entries */
SaErrorT oh_el_clear(oh_el *el)
{
        GList *temp;

        if (el == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

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


/* get an EL entry */
SaErrorT oh_el_get(oh_el *el, SaHpiEventLogEntryIdT entryid, SaHpiEventLogEntryIdT *prev,
                    SaHpiEventLogEntryIdT *next, oh_el_entry **entry)
{
        oh_el_entry *myentry;
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
        myentry = (oh_el_entry *)ellist->data;
        lastid = myentry->event.EntryId;
        ellist = g_list_first(el->elentries);
        myentry = (oh_el_entry *)ellist->data;
        firstid = myentry->event.EntryId;
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
                myentry = (oh_el_entry *) ellist->data;
                if (srchentryid == myentry->event.EntryId) {
                        *entry = myentry;
                        /* is this the first entry? */
                        if (myentry->event.EntryId == firstid) {
                                *prev = SAHPI_NO_MORE_ENTRIES;
                        }
                        else {
                                *prev = myentry->event.EntryId - 1;
                        }
                        /* is this the last entry? */
                        if (myentry->event.EntryId == lastid) {
                                *next = SAHPI_NO_MORE_ENTRIES;
                        }
                        else {
                                *next = myentry->event.EntryId + 1;
                        }
                        return SA_OK;
                }
                else if (entryid < myentry->event.EntryId) {
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
	info->UserEventMaxSize  = SAHPI_MAX_TEXT_BUFFER_LENGTH;
        info->UpdateTimestamp = el->lastUpdate;
        time(&tt1);
        info->CurrentTime = ((SaHpiTimeT) tt1 * 1000000000) + el->offset;
        info->Enabled = el->enabled;
        info->OverflowFlag = el->overflow;
	info->OverflowResetable = SAHPI_TRUE;
        info->OverflowAction = SAHPI_EL_OVERFLOW_OVERWRITE;
        return SA_OK;
}


/* reset EL overflowflag */
SaErrorT oh_el_overflowreset(oh_el *el)
{


        if (el == NULL)
                return SA_ERR_HPI_INVALID_PARAMS;

	el->overflow = SAHPI_FALSE;
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
                write(file, (void *)ellist->data, sizeof(oh_el_entry));
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
        oh_el_entry entry;
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
        while (read(file, &entry, sizeof(oh_el_entry)) == sizeof(oh_el_entry)) {
                el->nextId = entry.event.EntryId;
                /* Need a way to preserve the original entry's timestamp
                 * if that is of use. -- RM
                 */
                retc = oh_el_append(el, &(entry.event.Event), &(entry.rdr),
                                    &(entry.res));
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
