/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
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


