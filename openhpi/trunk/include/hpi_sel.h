/*
 * Copyright (c) 2003, Service Availability Forum
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or 
 * without modification, are permitted provided that the following 
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Service Availalability Forum nor the names 
 * of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


/*******************************************************************************
********************************************************************************
********** 						              **********
********** System Event Log 					      **********
********** 						   	      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_SEL_H
#define HPI_SEL_H

/* This section defines the types associated with the SEL. */
/*
** Event Log Information
**
** The Entries entry denotes the number of active entries contained in the log.
** The Size entry denotes the total number of entries the log is able to hold.
** The UpdateTimestamp entry denotes the timestamp of the last addition,
** deletion or log clear operation.
** The CurrentTime entry denotes the log's idea of the current time; i.e the
** timestamp that would be placed on an entry if it was added now.
** The Enabled entry indicates whether the log is enabled. If the event log
** is "disabled" no events generated within the HPI implementation will be
** added to the event log. Events may still be added to the event log with
** the saHpiEventLogEntryAdd() function. When the event log is "enabled"
** events may be automatically added to the event log as they are generated
** in a resource or a domain, however, it is implementation-specific which
** events are automatically added to any event log.
** The OverflowFlag entry indicates the log has overflowed. Events have been
** dropped or overwritten due to a table overflow.
** The OverflowAction entry indicates the behavior of the SEL when an overflow
** occurs.
** The DeleteEntrySupported indicates whether the delete command is supported for
** event log entries. Note that clearing an entire log is valid even if this
** flag is not set.
*/
typedef enum {
	SAHPI_SEL_OVERFLOW_DROP, /* New entries are dropped when log is full*/
	SAHPI_SEL_OVERFLOW_WRAP, /* Log wraps when log is full */
	SAHPI_SEL_OVERFLOW_WRITELAST /* Last entry overwritten when log is full */
} SaHpiSelOverflowActionT;
typedef struct {
	SaHpiUint32T Entries;
	SaHpiUint32T Size;
	SaHpiTimeT UpdateTimestamp;
	SaHpiTimeT CurrentTime;
	SaHpiBoolT Enabled;
	SaHpiBoolT OverflowFlag;
	SaHpiSelOverflowActionT OverflowAction;
	SaHpiBoolT DeleteEntrySupported;
} SaHpiSelInfoT;

/*
** Event Log Entry
** These types define the event log entry.
*/
typedef SaHpiUint32T SaHpiSelEntryIdT;

/* Reserved values for event log entry IDs */
#define SAHPI_OLDEST_ENTRY (SaHpiSelEntryIdT)0x00000000
#define SAHPI_NEWEST_ENTRY (SaHpiSelEntryIdT)0xFFFFFFFF
#define SAHPI_NO_MORE_ENTRIES (SaHpiSelEntryIdT)0xFFFFFFFE

typedef struct {
	SaHpiSelEntryIdT EntryId; /* Entry ID for record */
	SaHpiTimeT Timestamp; /* Time at which the event was placed
			       * in the event log. The value
			       * SAHPI_TIME_UNSPECIFIED indicates that
			       * the time of the event cannot be
			       * determined; otherwise, if less than
			       * or equal to SAHPI_TIME_MAX_RELATIVE,
			       * then it relative; if it is greater than
			       * SAHPI_TIME_MAX_RELATIVE, then it is absolute. */
	SaHpiEventT Event; /* Logged Event */
} SaHpiSelEntryT;

#endif /* HPI_SEL_H */


