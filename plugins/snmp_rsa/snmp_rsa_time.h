/*      -*- linux-c -*-
 *
 *
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 *
 * Authors:
 *      peter d phan   <pdphan@users.sf.net>
 *      
 * Logs:
 *	 Duplicate enums from IBM Blade Center
 */

#ifndef SNMP_RSA_TIME_H
#define SNMP_RSA_TIME_H

/*                                                                 
 * Set timezone constants
 */                                                               
typedef enum {
	DST_NONE = 0,
	DST_USA,
	DST_ESA,
	DST_MID,
	DST_EEC,
	DST_EEU,
	DST_EGT,
	DST_FLE,
	DST_IRN,
	DST_AUS,
	DST_TAS,
	DST_NWZ,
	DST_AUTOMATIC              // Must be last in list, used to validate entry
} RSA_DST_STANDARDS;
    
typedef enum {
	FIRST_WEEK = 1,
	SECOND_WEEK,
	THIRD_WEEK,
	FOURTH_WEEK,
	LAST_WEEK
} RSA_DST_WEEK;
    
typedef enum {
	SUNDAY = 1,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
} RSA_DST_WEEKDAY;
    
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
} RSA_DST_MONTH;

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
} RSA_DST_ENTRY;

/* 
 * Function Prototyping
 */
 
gboolean rsa_is_dst_in_effect(struct tm *, gchar **);
gboolean rsa_is_leap_year(guchar );
guchar   rsa_get_day_of_month(guchar, guchar, guchar, guchar);
int set_rsa_dst(struct oh_handler_state *, struct tm *);
int set_rsa_time(struct snmp_session *, struct tm *);
int get_rsa_time(struct oh_handler_state *, struct tm *);

#endif
