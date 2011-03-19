/* -*- linux-c -*-
 *
 *
 * (C) Copyright IBM Corp 2003,2004
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * Authors:  
 *     Peter D.Phan pdphan@users.sourceforge.net
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *  
 *     01/13/2004 pdphan  module created
 *			  Change clock for event log on IBM Blade Center E.
 *     03/10/2004 pdphan  Remove reference to IBM Blade Center.
 *     10/12/2004 kouzmich  porting to HPI B.
 *			    check month, day, year, hours, minutes and seconds
 *			    for correctness
 *     07/06/2010 ulikleber  New option -D to select domain
 *     20/01/2011 ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 */
/*
Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

  a.. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer. 
  b.. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution. 
  c.. Neither the name of Intel Corporation nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

static gchar *findate = NULL;
static gchar *fintime = NULL;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "date",  'd', 0, G_OPTION_ARG_STRING, &findate,  "New date", "mm/dd/yyyy" },
  { "time",  't', 0, G_OPTION_ARG_STRING, &fintime,  "New time of day in 24-hr format", "24:12:60" },
  { NULL }
};

#define EXIT1 g_free(findate);g_free(fintime);return 1;


int main(int argc, char **argv)
{
	int month, day, year;
	int day_array[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	struct tm  new_tm_time;
	SaErrorT rv;
	SaHpiSessionIdT sessionid;
	SaHpiDomainInfoT domainInfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiResourceIdT resourceid;
	SaHpiTimeT oldtime;
	SaHpiTimeT newtime;
	SaHpiTimeT readbacktime;
	SaHpiTextBufferT buffer;
        GOptionContext *context;

        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Exercises Event Log clock APIs.\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION   // not applicable
                    - OHC_VERBOSE_OPTION )) {      // no verbose mode
                g_option_context_free (context);
		EXIT1;
	}
        g_option_context_free (context);

	if ( !findate || !fintime) {
		CRIT("Please enter date and time to be set.");
		EXIT1;
	}

	if (findate) {
		if (copt.debug) printf("New date to be set: %s\n",findate);
	        if (sscanf(findate,"%2d/%2d/%4d", &month, &day, &year) != 3) {
			CRIT("%s: Invalid date", argv[0]);
			EXIT1;
		}
		/* check month, day and year for correctness */
		if ((month < 1) || (month > 12)) {
			CRIT("%s: Month out of range: (%d)", argv[0], month);
			EXIT1;
		};
		if (year < 1900) {
			CRIT("%s: Year out of range: (%d)", argv[0], year);
			EXIT1;
		};
		month--;
		if (month == 1) {
		/* if the given year is a leap year */
			if ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0))
				day_array[1] = 29;
		};
		if ((day < 1) || (day > day_array[month])) {
			CRIT("%s: Day out of range: (%d)", argv[0], day);
			EXIT1;
		};

		new_tm_time.tm_mon = month;
		new_tm_time.tm_mday = day;
		new_tm_time.tm_year = year - 1900;
	}

	if (fintime) {
		if (copt.debug)  DBG("New time to be set:  %s",fintime);
	        if (sscanf(fintime,"%2d:%2d:%2d",
                  &new_tm_time.tm_hour, &new_tm_time.tm_min, &new_tm_time.tm_sec) != 3) {
			CRIT("%s: Invalid time", argv[0]);
			EXIT1;
		}
		/* check hours, minutes and seconds for correctness */
		if ((new_tm_time.tm_hour < 0) || (new_tm_time.tm_hour > 24)) {
			CRIT("%s: Hours out of range: (%d)", argv[0], new_tm_time.tm_hour);
			EXIT1;
		};
		if ((new_tm_time.tm_min < 0) || (new_tm_time.tm_min > 60)) {
			CRIT("%s: Minutes out of range: (%d)", argv[0], new_tm_time.tm_min);
			EXIT1;
		};
		if ((new_tm_time.tm_sec < 0) || (new_tm_time.tm_sec > 60)) {
			CRIT("%s: Seconds out of range: (%d)", argv[0], new_tm_time.tm_sec);
			EXIT1;
		}
	}

	if (copt.debug) DBG("Values passed to mktime():\n\tmon %d\n\tday %d\n\tyear %d\n\tHH %d\n\tMM %d\n\tSS %d",
			new_tm_time.tm_mon, new_tm_time.tm_mday, new_tm_time.tm_year,
			new_tm_time.tm_hour, new_tm_time.tm_min, new_tm_time.tm_sec);

	newtime = (SaHpiTimeT) mktime(&new_tm_time) * 1000000000;

	if (copt.debug) 
           DBG("New date and time in SaHpiTimeT %" PRId64 "\n", (int64_t)newtime);

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) EXIT1;

	if (copt.debug) DBG("saHpiDiscover");
	rv = saHpiDiscover(sessionid);
	if (copt.debug) DBG("saHpiDiscover %s", oh_lookup_error(rv));
	rv = saHpiDomainInfoGet(sessionid, &domainInfo);
	if (copt.debug) DBG("saHpiDomainInfoGet %s", oh_lookup_error(rv));
	printf("DomainInfo: RptUpdateCount = %u, RptUpdateTimestamp = %lx\n",
		domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);
        
	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
	{
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (copt.debug) DBG("saHpiRptEntryGet %s", oh_lookup_error(rv));
                if ((rv == SA_OK) && (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                        resourceid = rptentry.ResourceId;
                        if (copt.debug) DBG("RPT %x capabilities = %x", resourceid,
                                           rptentry.ResourceCapabilities);
			rv = saHpiEventLogTimeGet(sessionid, resourceid, &oldtime);
			oh_decode_time(oldtime, &buffer);
			printf ("\nCurrent event log time on HPI target: %s\n", buffer.Data);
			printf ("Setting new event log time on HPI target ...\n");
		 	rv = saHpiEventLogTimeSet(sessionid, resourceid, newtime);
			if (rv != SA_OK) 
			{
                		CRIT("saHpiEventLogTimeSet returned %s", oh_lookup_error(rv));
			}
			rv = saHpiEventLogTimeGet(sessionid, resourceid, &readbacktime);
			oh_decode_time(readbacktime, &buffer);
			printf ("Read-Back-Check event log time: %s\n", buffer.Data);

                }
                // entryid = SAHPI_OLDEST_ENTRY;
                rptentryid = nextrptentryid;
	} 
        
        rv = saHpiSessionClose(sessionid);

        g_free(findate);
        g_free(fintime);
   
        return(0);
}
 
/* end hpisettime.c */
