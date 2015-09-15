/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004, 2007
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Peter D Phan <pdphan@us.ibm.com>
 *      Steve Sherman <stevees@us.ibm.com>
 *	Renier Morales <renier@openhpi.org>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 *     10/13/2004 kouzmich  changed -t option for infinite wait
 *			    added -d option for call saHpiDiscover after saHpiSubscribe
 *     11/17/2004 kouzmich  linux style and checking timeout error
 *     09/06/2010 ulikleber New option -D to select domain
 *     01/02/2011 ulikleber Refactoring to use glib for option parsing and
 *                          introduce common options for all clients
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

#define HPI_NSEC_PER_SEC 1000000000LL

static gchar *timeout_str    = NULL;
static gboolean do_discover_after_subscribe = FALSE;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "timeout", 't', 0, G_OPTION_ARG_STRING, &timeout_str, "Wait n seconds for event or infinite wait\n"
"                               (BLOCK or SAHPI_TIMEOUT_BLOCK)",
                                                      "n|BLOCK" },
  { "discover", 'd', 0, G_OPTION_ARG_NONE, &do_discover_after_subscribe,      "Call saHpiDiscover() after saHpiSubscribe()",
                                                      NULL },
  { NULL }
};


int main(int argc, char **argv)
{
	int test_fail = 0, wait = 0;
	SaErrorT rv;
	SaHpiSessionIdT sessionid;
	SaHpiDomainInfoT domainInfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiResourceIdT resourceid;
	SaHpiEventLogInfoT info;
	SaHpiRdrT rdr;
	SaHpiTimeoutT timeout; 
	SaHpiEventT event;
        GOptionContext *context;

	memset(&rptentry, 0, sizeof(rptentry));

        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Poll for HPI events\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION  //TODO: Feature 880127
                    - OHC_VERBOSE_OPTION )) { // no verbose mode implemented
                g_option_context_free (context);
		return 1;
	}
        g_option_context_free (context);
 
	if (timeout_str) {
		int i = 0;
		while (i < 20 && timeout_str[i] != '\0') {
			timeout_str[i] = toupper((char) timeout_str[i]);
			i++;
		}
		if ((strcmp(timeout_str, "SAHPI_TIMEOUT_BLOCK") == 0) ||
		    (strcmp(timeout_str, "BLOCK") == 0)) {
			timeout = SAHPI_TIMEOUT_BLOCK;
		} else {
			wait = atoi(timeout_str);
                	timeout = (SaHpiTimeoutT)(wait * HPI_NSEC_PER_SEC);
		}
                g_free (timeout_str);
	} else
		timeout = (SaHpiTimeoutT) SAHPI_TIMEOUT_IMMEDIATE;

	printf("************** timeout:[%" PRId64 "] ****************\n", 
                (uint64_t) timeout);    

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) return -1;
 
	if (!do_discover_after_subscribe) {
        	if (copt.debug) DBG("saHpiDiscover");
        	rv = saHpiDiscover(sessionid);
        	if (rv != SA_OK) {
        		CRIT("saHpiDiscover: %s", oh_lookup_error(rv));
        		return rv;
        	}
        }
	
        if (copt.debug) DBG( "Subscribe to events\n");
        rv = saHpiSubscribe( sessionid );
	if (rv != SA_OK) {
		CRIT("saHpiSubscribe: %s", oh_lookup_error(rv));
		return rv;
	}

	if (do_discover_after_subscribe) {
		if (copt.debug) DBG("saHpiDiscover after saHpiSubscribe");
		rv = saHpiDiscover(sessionid);
		if (rv != SA_OK) {
			CRIT("saHpiDiscover after saHpiSubscribe: %s", oh_lookup_error(rv));
			return rv;
		}
	}

	rv = saHpiDomainInfoGet(sessionid, &domainInfo);

	if (copt.debug) DBG("saHpiDomainInfoGet %s", oh_lookup_error(rv));
	printf("DomainInfo: UpdateCount = %u, UpdateTime = %lx\n",
		domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {       
		printf("**********************************************\n");

		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
		if (copt.debug) DBG("saHpiRptEntryGet %s", oh_lookup_error(rv));

		if (rv == SA_OK) {
			resourceid = rptentry.ResourceId;
			if (copt.debug)
				DBG("RPT %x capabilities = %x", resourceid,
					rptentry.ResourceCapabilities);

			if ( (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
				/* Using EventLogInfo to build up event queue - for now */
				rv = saHpiEventLogInfoGet(sessionid, resourceid, &info);
				if (copt.debug) 
					DBG("saHpiEventLogInfoGet %s", oh_lookup_error(rv));
				if (rv == SA_OK) 
					oh_print_eventloginfo(&info, 4);
			} else {
				if (copt.debug) 
					DBG("RPT doesn't have SEL");
			}

			rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0; 
			printf("rptentry[%u] tag: %s\n", resourceid, rptentry.ResourceTag.Data);

			rptentryid = nextrptentryid;
		}
		printf("**********************************************\n");
	}

	printf( "Go and get the event\n");
	while (1) {
		rdr.RdrType = SAHPI_NO_RECORD;
                rptentry.ResourceId = 0;

		rv = saHpiEventGet( sessionid, timeout, &event, &rdr, &rptentry, NULL);
		if (rv != SA_OK) {
			if (rv != SA_ERR_HPI_TIMEOUT) {
				printf("ERROR during EventGet : %s\n", oh_lookup_error(rv));
				test_fail = 1;
			} else {
				if (timeout == SAHPI_TIMEOUT_BLOCK) {
					printf("ERROR: Timeout while infinite wait\n");
					test_fail = 1;
				} else if (timeout != SAHPI_TIMEOUT_IMMEDIATE) {
					printf("ERROR: Time, %u seconds, expired waiting"
						" for event\n", wait);
					test_fail = 1;
				}
			}
			break;
		} else {
			if (rdr.RdrType != SAHPI_NO_RECORD)
				oh_print_event(&event, &rdr.Entity, 4);
                        else if (rptentry.ResourceId != 0)
                                oh_print_event(&event, &rptentry.ResourceEntity, 4);
                        else {
                                rptentryid = event.Source;
                                rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid, &rptentry);
                                if(rv == SA_OK)
                                        oh_print_event(&event, &rptentry.ResourceEntity, 4);
                                else {
                                        printf("Wrong resource Id <%u> detected", event.Source);
                                        oh_print_event(&event, NULL, 4);
                                }
                        }
		}
	}

	if (test_fail == 0)
		printf("	Test PASS.\n");
	else
		printf("	Test FAILED.\n");

	/* Unsubscribe to future events */
	if (copt.debug) DBG( "Unsubscribe");
	rv = saHpiUnsubscribe( sessionid );

	rv = saHpiSessionClose(sessionid);
        
	return(0);
}
 
/* end hpigetevents.c */
