/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004, 2006
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
 *      Renier Morales <renier@openhpi.org>
 */

#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>

/* allocate and initialize an EL */
oh_el *oh_el_create(SaHpiUint32T size)
{
        oh_el *el;

        el = g_new0(oh_el, 1);
        if (el != NULL) {
		el->basetime = 0;
		el->sysbasetime = 0;
		el->nextid = SAHPI_OLDEST_ENTRY + 1;
		el->gentimestamp = SAHPI_TRUE;
		
		el->info.Entries = 0;
		el->info.Size = size;
		el->info.UserEventMaxSize = SAHPI_MAX_TEXT_BUFFER_LENGTH;
		el->info.UpdateTimestamp = SAHPI_TIME_UNSPECIFIED;
		el->info.CurrentTime = SAHPI_TIME_UNSPECIFIED;
                el->info.Enabled = SAHPI_TRUE;
                el->info.OverflowFlag = SAHPI_FALSE;
		el->info.OverflowResetable = SAHPI_TRUE;
        	el->info.OverflowAction = SAHPI_EL_OVERFLOW_OVERWRITE;
		
                el->list = NULL;
        }
        return el;
}


/* close and free all memory associated with an EL */
SaErrorT oh_el_close(oh_el *el)
{
        if (el == NULL) return SA_ERR_HPI_INVALID_PARAMS;

	oh_el_clear(el);	
        g_free(el);
	
        return SA_OK;
}


/* append a new entry to the EL */
SaErrorT oh_el_append(oh_el *el,
		       const SaHpiEventT *event,
		       const SaHpiRdrT *rdr,
		       const SaHpiRptEntryT *res)
{
        oh_el_entry *entry;
	SaHpiTimeT cursystime;

        /* check for valid el params and state */
        if (el == NULL || event == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (el->info.Enabled == FALSE &&
		    event->EventType != SAHPI_ET_USER) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* alloc the new entry */
        entry = g_new0(oh_el_entry, 1);
        if (entry == NULL) {
                el->info.OverflowFlag = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
	
        if (rdr) entry->rdr = *rdr;
        if (res) entry->res = *res;

        /* if necessary, wrap the el entries */
        if (el->info.Size != OH_EL_MAX_SIZE &&
	    g_list_length(el->list) == el->info.Size) {
		g_free(el->list->data);
                el->list = g_list_delete_link(el->list, el->list);
                el->info.OverflowFlag = SAHPI_TRUE;
        }

        /* Set the event log entry id and timestamp */
        entry->event.EntryId = el->nextid++;
	if (el->gentimestamp) {
		oh_gettimeofday(&cursystime);
        	el->info.UpdateTimestamp =
			el->basetime + (cursystime - el->sysbasetime);
	} else {
		el->info.UpdateTimestamp = event->Timestamp;
		/* Setting time based on the event to have some sense of what
		 * the current time is going to be when providing the el info.
		 */
		oh_el_timeset(el, event->Timestamp);
	}
	entry->event.Timestamp = el->info.UpdateTimestamp;

	/* append the new entry */
	entry->event.Event = *event;
        el->list = g_list_append(el->list, entry);
	
        return SA_OK;
}


/* prepend a new entry to the EL */
SaErrorT oh_el_prepend(oh_el *el,
			const SaHpiEventT *event,
			const SaHpiRdrT *rdr,
			const SaHpiRptEntryT *res)
{
        GList *node = NULL;
	oh_el_entry *entry;        
	SaHpiTimeT cursystime;

        /* check for valid el params and state */
        if (el == NULL || event == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (el->info.Enabled == FALSE &&
		    event->EventType != SAHPI_ET_USER) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        /* see if el is full */
        if (el->info.Size != OH_EL_MAX_SIZE &&
	    g_list_length(el->list) == el->info.Size) {
                return SA_ERR_HPI_OUT_OF_SPACE;
        }

        /* alloc the new entry */
        entry = g_new0(oh_el_entry, 1);
        if (entry == NULL) {
                el->info.OverflowFlag = TRUE;
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
        
	if (rdr) entry->rdr = *rdr;
        if (res) entry->res = *res;

        /* since we are adding entries in reverse order we have to renumber
         * existing entries
         */        
	for (node = el->list; node; node = node->next) {
                oh_el_entry *tmpentry = (oh_el_entry *)node->data;
		tmpentry->event.EntryId++;
        }
	el->nextid++;

        /* prepare & prepend the new entry */
        entry->event.EntryId = SAHPI_OLDEST_ENTRY + 1;
	if (el->gentimestamp) {
		oh_gettimeofday(&cursystime);
        	el->info.UpdateTimestamp =
			el->basetime + (cursystime - el->sysbasetime);
	} else {
		el->info.UpdateTimestamp = event->Timestamp;
		/* Setting time based on the event to have some sense of what
		 * the current time is going to be when providing the el info.
		 */
		oh_el_timeset(el, event->Timestamp);
	}
        entry->event.Timestamp = el->info.UpdateTimestamp;
	
	/* prepend the new entry to the list */
	entry->event.Event = *event;
        el->list = g_list_prepend(el->list, entry);
	
        return SA_OK;
}


/* clear all EL entries */
SaErrorT oh_el_clear(oh_el *el)
{
        GList *node;

        if (el == NULL) return SA_ERR_HPI_INVALID_PARAMS;

        /* free the data for every element in the list */
	for (node = el->list; node; node = node->next) {        
                g_free(node->data);
        }
	
        /* free the list nodes */
        g_list_free(el->list);
        
	/* reset the control structure */
        el->info.OverflowFlag = SAHPI_FALSE;
        el->info.UpdateTimestamp = SAHPI_TIME_UNSPECIFIED;
	el->info.Entries = 0;
        el->nextid = SAHPI_OLDEST_ENTRY + 1; // always start at 1
        el->list = NULL;

        return SA_OK;
}


/* get an EL entry */
SaErrorT oh_el_get(oh_el *el,
		   SaHpiEventLogEntryIdT entryid,
		   SaHpiEventLogEntryIdT *prev,
                   SaHpiEventLogEntryIdT *next,
		   oh_el_entry **entry)
{
        SaHpiEventLogEntryIdT eid;
	GList *node = NULL;
	oh_el_entry *elentry = NULL;	
	
	if (!el || !prev || !next || !entry ||
	    entryid == SAHPI_NO_MORE_ENTRIES) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	
        if (g_list_length(el->list) == 0) {
                return SA_ERR_HPI_NOT_PRESENT;
        }
	
	/* FIXME: There is a bug here because this does not take into account
	 * the case when oh_el_prepend would have been used. In such case the
	 * OLDEST entry would technically not be the first one in the list.
	 * To be continued...
	 * 	-- Renier Morales (08/30/06)
	 */
        if (entryid == SAHPI_OLDEST_ENTRY) {
		node = g_list_first(el->list);
	} else if (entryid == SAHPI_NEWEST_ENTRY) {
		node = g_list_last(el->list);
	}

	if (node) {
		elentry = (oh_el_entry *)node->data;
		eid = elentry->event.EntryId;
	} else {
		eid = entryid;
	}
	
	for (node = el->list; node; node = node->next) {
		elentry = (oh_el_entry *)node->data;
		if (eid == elentry->event.EntryId) {
			*entry = elentry;
			if (node->prev) {
				elentry = (oh_el_entry *)node->prev->data;
				*prev = elentry->event.EntryId;
			} else {
				*prev = SAHPI_NO_MORE_ENTRIES;
			}
			if (node->next) {
				elentry = (oh_el_entry *)node->next->data;
				*next = elentry->event.EntryId;
			} else {
				*next = SAHPI_NO_MORE_ENTRIES;
			}
			return SA_OK;
		}
	}

	return SA_ERR_HPI_NOT_PRESENT;
}


/* get EL info */
SaErrorT oh_el_info(oh_el *el, SaHpiEventLogInfoT *info)
{
	SaHpiTimeT cursystime;

        if (el == NULL || info == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
        
        *info = el->info;
	info->Entries = g_list_length(el->list);
	oh_gettimeofday(&cursystime);	
        info->CurrentTime = el->basetime + (cursystime - el->sysbasetime);
        
        return SA_OK;
}


/* reset EL overflowflag */
SaErrorT oh_el_overflowreset(oh_el *el)
{
        if (el == NULL) return SA_ERR_HPI_INVALID_PARAMS;

	if (el->info.OverflowResetable) {
        	el->info.OverflowFlag = SAHPI_FALSE;
		return SA_OK;
	} else {
		return SA_ERR_HPI_INVALID_CMD;
	}
}

SaErrorT oh_el_overflowset(oh_el *el, SaHpiBoolT flag)
{
	if (!el) return SA_ERR_HPI_INVALID_PARAMS;
	
	el->info.OverflowFlag = flag;
	
	return SA_OK;
}


/* write a EL entry list to a file */
SaErrorT oh_el_map_to_file(oh_el *el, char *filename)
{
        FILE *fp;
        GList *node = NULL;

        if (el == NULL || filename == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        fp = fopen(filename, "wb");
        if (!fp) {
                CRIT("EL file '%s' could not be opened", filename);
                return SA_ERR_HPI_ERROR;
        }
        
	for (node = el->list; node; node = node->next) {
                if (fwrite((void *)node->data, sizeof(oh_el_entry), 1, fp) != 1) {
			CRIT("Couldn't write to file '%s'.", filename);
			fclose(fp);
                	return SA_ERR_HPI_ERROR;
		}
        }

        fclose(fp);

        return SA_OK;
}


/* read a EL entry list from a file */
SaErrorT oh_el_map_from_file(oh_el *el, char *filename)
{
        FILE *fp;
        oh_el_entry entry;

        /* check el params and state */
        if (el == NULL || filename == NULL) {
                return SA_ERR_HPI_INVALID_PARAMS;
        } else if (el->info.Enabled == FALSE) {
                return SA_ERR_HPI_INVALID_REQUEST;
        }

        fp = fopen(filename, "rb");
        if (!fp) {
                CRIT("EL file '%s' could not be opened", filename);
                return SA_ERR_HPI_ERROR;
        }

        oh_el_clear(el); // ensure list is empty
        while (fread(&entry, sizeof(oh_el_entry), 1, fp) == 1) {
		oh_el_entry *elentry = g_new0(oh_el_entry, 1);
		el->nextid = entry.event.EntryId;
		el->nextid++;
		*elentry = entry;
		el->list = g_list_append(el->list, elentry);
        }

        fclose(fp);

        return SA_OK;
}


/* set the EL timestamp offset */
SaErrorT oh_el_timeset(oh_el *el, SaHpiTimeT timestamp)
{
	if (el == NULL || timestamp == SAHPI_TIME_UNSPECIFIED) {
                return SA_ERR_HPI_INVALID_PARAMS;
        }
	
	oh_gettimeofday(&el->sysbasetime);
	el->basetime = timestamp;

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

SaErrorT oh_el_enableset(oh_el *el, SaHpiBoolT flag)
{
	if (!el) return SA_ERR_HPI_INVALID_PARAMS;

	el->info.Enabled = flag;
	
	return SA_OK;
}
