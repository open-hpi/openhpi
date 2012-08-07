/*      -*- linux-c -*-
 *
 * Copyright (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Log: 
 *	Start from hpitop.c 
 *	This routine display highlevel domain topology for a managed 
 *	openHPI complex
 *
 * Changes:
 *     03/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision: 7112 $"

/* 
 * Function prototypes
 */
static SaErrorT show_domains(void); 
static SaErrorT print_domaininfo(SaHpiDomainInfoT info, int shift); 

/* 
 * Globals for this driver
 */
static oHpiCommonOptionsT copt;

/* 
 * Main                
 */
int
main(int argc, char **argv)
{
        GOptionContext *context;

        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Show information about domains"
                              " on the level of the openhpi base library.\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION )) { // not applicable
                g_option_context_free (context);
		return 1;
	}
        g_option_context_free (context);
 
	show_domains();

	return 0;
}


/* 
 *
 */
static 
SaErrorT show_domains(void)
{
	SaErrorT rv       = SA_OK;
	oHpiDomainEntryT domainentry;
	SaHpiEntryIdT domainentryid;
	SaHpiEntryIdT nextdomainentryid;
	SaHpiDomainInfoT relateddomaininfo;
	SaHpiDomainIdT relateddomainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
	SaHpiSessionIdT relatedsessionid;

	/* walk the Domain Table */
	domainentryid = SAHPI_FIRST_ENTRY;
	do {
	   if (copt.debug) DBG("oHpiDomainEntryGet called with entry=%u",
                                        domainentryid);
	   rv = oHpiDomainEntryGet(
              domainentryid,&nextdomainentryid,&domainentry);

	   if ((rv != SA_OK) || copt.debug) 
              printf("oHpiDomainEntryGet returns %s\n", 
                        oh_lookup_error(rv));
		
	   if (rv == SA_OK ) {
              if (copt.debug) DBG("oHpiDomainEntryGet provides domainid=%u,"
                                   " nextentryid=%u\n", 
                                   domainentry.id, nextdomainentryid);

		printf("Domain defined on host(%s:%u) with id: %u\n",
                                          (char *)domainentry.host.Data,
                                          domainentry.port,
                                          domainentry.id);

		if (copt.verbose) {
		    /* display the domaininfo for that related domain */
		    relateddomainid = domainentry.id;
        	    rv = saHpiSessionOpen(relateddomainid,
					  &relatedsessionid,NULL);
		    if (rv != SA_OK) {
			printf("Domain %u cannot be opened\n",
				relateddomainid);
			continue;
		    }
		    if (copt.debug) {
			DBG("saHpiSessionOpen returns with SessionId %u\n", 
				relatedsessionid);
			DBG("saHpiDomainInfoGet for domain %u\n",
				relateddomainid);
		    }
		    rv = saHpiDomainInfoGet(relatedsessionid,
					    &relateddomaininfo);
		    if (rv!=SA_OK) {
			printf("\nDomaininfo of domain %u cannot be "
                                "retrieved.\n",
				relateddomainid);
                        if (copt.debug)
			   DBG("saHpiDomainInfoGet for domain "
				"%u failed with returncode %s\n",
				relateddomainid, oh_lookup_error(rv));
		    }
		    else {
			/* Print info about related domain */
			rv = print_domaininfo(relateddomaininfo,1);
		    }

		    rv = saHpiSessionClose(relatedsessionid);
		    if (copt.debug) 
			DBG("saHpiSessionClose returns %s\n",
				oh_lookup_error(rv));

		} //verbose
	   }
	   else if (rv == SA_ERR_HPI_NOT_PRESENT) {
                   if (domainentryid == SAHPI_FIRST_ENTRY)
				printf("Domain list is empty. \n");
                   else printf("Internal error while walking the Domainlist\n");
		}
		else printf("Internal error while walking the Domainlist\n");

		domainentryid = nextdomainentryid;
	} while ((rv == SA_OK) && (domainentryid != SAHPI_LAST_ENTRY));
	
	return(rv);
}

/*
*
*/
static SaErrorT print_domaininfo(SaHpiDomainInfoT info, int shift)
{
        SaHpiTextBufferT        buf;
        SaErrorT                rv;
	int			i;

	for (i=0;i<shift;i++)printf("    ");
        printf("Domain: %u   Capabil: 0x%x   IsPeer: %d   Tag: ",
                info.DomainId, info.DomainCapabilities,
                info.IsPeer);
	rv = oh_print_text(&(info.DomainTag));

	printf("\n");
	for (i=0;i<shift;i++)printf("    ");
        printf("            Guid: ");
        for (i=0; i<16; i++) {
            if ((i == 4) || (i == 6) || (i == 8) || (i == 10)) printf("-");
            printf("%02x",info.Guid[i]);
        }

        printf("\n");
	rv = oh_decode_time(info.DrtUpdateTimestamp, &buf);
	for (i=0;i<shift;i++)printf("    ");
        printf("            DRT update count: %u   DRT Timestamp : ",
                info.DrtUpdateCount);
	oh_print_text(&buf);
        printf("\n");

	rv = oh_decode_time(info.RptUpdateTimestamp, &buf);
	for (i=0;i<shift;i++)printf("    ");
        printf("            RPT update count: %u   RPT Timestamp : ",
                info.RptUpdateCount);
	oh_print_text(&buf);
        printf("\n");

	rv = oh_decode_time(info.DatUpdateTimestamp, &buf);
	for (i=0;i<shift;i++)printf("    ");
        printf("            DAT update count: %u   DAT Timestamp : ",
                info.DatUpdateCount);
	oh_print_text(&buf);
        printf("\n");

	for (i=0;i<shift;i++)printf("    ");
        printf("                ActiveAlarms: %u   CriticalAlarms: %u   "
		"Major: %u Minor: %u\n",
                info.ActiveAlarms, info.CriticalAlarms, info.MajorAlarms,
                info.MinorAlarms);
	for (i=0;i<shift;i++)printf("    ");
        printf("                Limit: %u   DatOverflow : %d\n",
		info.DatUserAlarmLimit, info.DatOverflow);
	
	return rv;
}


/* end hpidomain.c */
