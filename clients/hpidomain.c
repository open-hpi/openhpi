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
 *    20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision: 7112 $"

/* 
 * Function prototypes
 */
static SaErrorT show_domain(SaHpiSessionIdT sessionid); 
static SaErrorT print_domaininfo(SaHpiDomainInfoT info, int shift); 
static SaErrorT set_domaintag(SaHpiSessionIdT sessionid, 
			SaHpiTextBufferT domtag);

/* 
 * Globals for this driver
 */
static gchar *f_domtag = NULL;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "tag",  't', 0, G_OPTION_ARG_STRING, &f_domtag,  "Set domain tag to the specified string", "tttt" },
  { NULL }
};


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
	SaErrorT 	rv = SA_OK;
	SaHpiSessionIdT sessionid;
	SaHpiTextBufferT domtag;
        GOptionContext *context;

	oh_init_textbuffer(&domtag);
	    
        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Display info about domains or set domain tag\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION)) { //TODO: Feature 880127?
                g_option_context_free (context);
		return 1;
	}
        g_option_context_free (context);

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) {
           g_free(f_domtag);
           return rv;
        }

	if (f_domtag){
		oh_append_textbuffer(&domtag, f_domtag);
                g_free (f_domtag);
                if (copt.debug) DBG ("Let's go change the tag to %s",
                                        f_domtag);
		set_domaintag(sessionid, domtag);
	}


	if (copt.debug) DBG ("Let's go and list the domains!");

	show_domain(sessionid);

	rv = saHpiSessionClose(sessionid);
	
	return 0;
}


/* 
 *
 */
static 
SaErrorT show_domain(SaHpiSessionIdT sessionid)
{
	SaErrorT rv       = SA_OK;
	SaHpiDomainInfoT domaininfo;
	SaHpiDrtEntryT drtentry;
	SaHpiEntryIdT drtentryid;
	SaHpiEntryIdT nextdrtentryid;
	SaHpiDomainInfoT relateddomaininfo;
	SaHpiDomainIdT relateddomainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
	SaHpiSessionIdT relatedsessionid;

	if (copt.debug) DBG("saHpiDomainInfoGet");
	rv = saHpiDomainInfoGet(sessionid,&domaininfo);
	if (rv!=SA_OK) {
		CRIT("saHpiDomainInfoGet failed with returncode %s",
			oh_lookup_error(rv));
		return rv;
	}
	
	/* Print info about this domain */
	rv = print_domaininfo(domaininfo,0);

	/* walk the DRT */
	drtentryid = SAHPI_FIRST_ENTRY;
	do {
	   if (copt.debug) DBG("saHpiDrtEntryGet");
	   rv = saHpiDrtEntryGet(sessionid,
			drtentryid,&nextdrtentryid,&drtentry);
	   if ((rv != SA_OK && rv != SA_ERR_HPI_NOT_PRESENT) || copt.debug) 
		       	DBG("DrtEntryGet returns %s",oh_lookup_error(rv));
		
	   if (rv == SA_OK ) {
		if (copt.verbose) {
		    /* display the domaininfo for that related domain */
		    relateddomainid = drtentry.DomainId;
        	    rv = saHpiSessionOpen(relateddomainid,
					  &relatedsessionid,NULL);
		    if (rv != SA_OK) {
			printf("Related domain found: %u   IsPeer: %u\n",
				drtentry.DomainId,drtentry.IsPeer);
			printf("saHpiSessionOpen to related domain %u "
				"returns %s\n",
				relateddomainid,oh_lookup_error(rv));
			continue;
		    }
		    if (copt.debug) {
			DBG("saHpiSessionOpen returns with SessionId %u", 
				relatedsessionid);
			DBG("saHpiDomainInfoGet for related domain %u",
				relateddomainid);
		    }
		    rv = saHpiDomainInfoGet(relatedsessionid,
					    &relateddomaininfo);
		    if (rv!=SA_OK) {
			printf("\nRelated domain found: %u   IsPeer: %u\n",
				drtentry.DomainId,drtentry.IsPeer);
			printf("saHpiDomainInfoGet  for related domain "
				"%u failed with returncode %s\n",
				relateddomainid,oh_lookup_error(rv));
		    }
		    else {
			printf("\nRelated domain found:\n");
			/* Print info about related domain */
			rv = print_domaininfo(relateddomaininfo,1);
		    }

		    rv = saHpiSessionClose(relatedsessionid);
		    if (copt.debug) 
			DBG("saHpiSessionClose returns %s",
				oh_lookup_error(rv));

		}
		else {
		    printf("Related domain found: %u   IsPeer: %u\n",
			drtentry.DomainId,drtentry.IsPeer);
		}
	   }
	   else if (rv == SA_ERR_HPI_NOT_PRESENT) {
			if (drtentryid == SAHPI_FIRST_ENTRY)
				printf("            DRT is empty. \n");
			else CRIT("Internal error while walking the DRT");
		}
		else CRIT("Internal error while walking the DRT");

		drtentryid = nextdrtentryid;
	} while ((rv == SA_OK) && (drtentryid != SAHPI_LAST_ENTRY));
	
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
        oHpiDomainEntryT        ohdomainentry;

	for (i=0;i<shift;i++)printf("    ");
        printf("Domain: %u   Capabil: 0x%x   IsPeer: %u   Tag: ",
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
        printf("                Limit: %u   DatOverflow : %u\n",
		info.DatUserAlarmLimit, info.DatOverflow);

        // Now print also OpenHPI specific info for the domain
        rv = oHpiDomainEntryGetByDomainId ( info.DomainId, &ohdomainentry );
        if (rv==SA_OK) {
                for (i=0;i<shift;i++)printf("    ");
                printf("            Serving Daemon on Host: %s:%u\n",
                                          (char *)ohdomainentry.host.Data,
                                          ohdomainentry.port );
        }

	return rv;
}

/*
*
*/
static SaErrorT set_domaintag(SaHpiSessionIdT sessionid, 
			      SaHpiTextBufferT domtag)
{
	SaErrorT rv       = SA_OK;
	SaHpiDomainInfoT domaininfo;


	if (copt.debug) DBG("saHpiDomainInfoGet");
	rv = saHpiDomainInfoGet(sessionid,&domaininfo);
	if (rv!=SA_OK) {
		CRIT("saHpiDomainInfoGet failed with returncode %s",
			oh_lookup_error(rv));
		return rv;
	}
	
	printf("Old domain Tag: ");
	rv = oh_print_text(&(domaininfo.DomainTag));
	printf("\n");

	rv = saHpiDomainTagSet (sessionid, &domtag);
	if (rv!=SA_OK)
		CRIT("saHpiDomainTagSet failed with returncode %s. "
			"Tag not changed.",oh_lookup_error(rv));
        else if (copt.debug) DBG("saHpiDomainTagSet completed.");
	
	return rv;
}

/* end hpidomain.c */
