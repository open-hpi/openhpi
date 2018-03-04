/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Peter Phan  <pdphan@users.sf.net>
 */

#ifndef __SNMP_BC_TIME_H
#define __SNMP_BC_TIME_H

/*                                                                 
 * Set timezone constants
 */                                                               
typedef enum {
	SNMP_BC_DST_NONE = 0,
	SNMP_BC_DST_USA,
	SNMP_BC_DST_ESA,
	SNMP_BC_DST_MID,
	SNMP_BC_DST_EEC,
	SNMP_BC_DST_EEU,
	SNMP_BC_DST_EGT,
	SNMP_BC_DST_FLE,
	SNMP_BC_DST_IRN,
	SNMP_BC_DST_AUS,
	SNMP_BC_DST_TAS,
	SNMP_BC_DST_NWZ,
	SNMP_BC_DST_AUTOMATIC              // Must be last in list, used to validate entry
} SNMP_BC_DST_STANDARDS;
    
typedef enum {
	FIRST_WEEK = 1,
	SECOND_WEEK,
	THIRD_WEEK,
	FOURTH_WEEK,
	LAST_WEEK
} SNMP_BC_DST_WEEK;
    
typedef enum {
	SUNDAY = 1,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
} SNMP_BC_DST_WEEKDAY;
    
typedef enum {
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
} SNMP_BC_DST_MONTH;

/*                                                               
 * Daylight saving time standards table entry
 *
 * This structure contains the definition of how daylight saving
 * time is observed for the supported timezones.
 */
typedef struct tag_DST_ENTRY {
	unsigned char start_hour;           // Hour daylight saving begins
	unsigned char start_day;            // Specific day daylight saving begins
	unsigned char start_week;           // Week number daylight saving begins
	unsigned char start_weekday;        // Day of week daylight saving begins
	unsigned char start_month;          // Month daylight saving begins
	unsigned char end_hour;             // Hour daylight saving ends
	unsigned char end_day;              // Specific day daylight saving ends
	unsigned char end_week;             // Week number daylight saving ends
	unsigned char end_weekday;          // Day of week daylight saving ends
	unsigned char end_month;            // Month daylight saving ends
} DST_ENTRY;

/* 
 * Function Prototyping
 */
 

gboolean is_dst_in_effect(struct tm *, gchar **);
gboolean is_leap_year(guchar );
guchar   get_day_of_month(guchar, guchar, guchar, guchar);
SaErrorT snmp_bc_set_dst(struct oh_handler_state *, struct tm *);
SaErrorT snmp_bc_set_sp_time(struct snmp_bc_hnd *, struct tm *);
SaErrorT snmp_bc_get_sp_time(struct oh_handler_state *, struct tm *);

#endif
