/*      -*- linux-c -*-
 *
 * Copyright (C) Copyright Nokia Siemens Networks 2010
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <uuid/uuid.h>
#include <SaHpi.h> 
#include <oh_utils.h>
#include <oh_clients.h>
#include <oHpi.h>

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
int fdebug     = 0;
int fverbose   = 0;


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
	SaErrorT 	rv = SA_OK;
	
	SaHpiDomainIdT domainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
	SaHpiSessionIdT sessionid;
	SaHpiBoolT printusage = FALSE;
	SaHpiTextBufferT domtag;
	int c;
	oh_init_textbuffer(&domtag);
	    
	oh_prog_version(argv[0], OH_SVN_REV);
	while ( (c = getopt( argc, argv,"D:t:vx?")) != EOF ) {
		switch(c) {
			case 'D':
                                if (optarg) {
                                        domainid = atoi(optarg);
                                }
				else printusage = TRUE;
                                break;
			case 't':
                                if (optarg) {
                                        oh_append_textbuffer(&domtag, optarg);
                                }
				else printusage = TRUE;
                                break;

			case 'x': fdebug = 1; break;
			case 'v': fverbose = 1; break;
			default: printusage = TRUE; break;
		}
	}
	if (printusage == TRUE)
	{
		printf("\n\tUsage: %s [-option]\n\n", argv[0]);
		printf("\t      (No Option) Display domain topology via "
				"default domain: drt and headers\n");	
		printf("\t           -D nn  Select domain id nn\n");
		printf("\t           -v     Display in verbose mode including "
				"domain info for directly related domains\n");
		printf("\t           -x     Display debug messages\n");
		printf("\n\n\n\n");
		exit(1);
	}
 
	if (fdebug) {
		if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
			printf("saHpiSessionOpen\n");
		else printf("saHpiSessionOpen to domain %u\n",domainid);
	}
        rv = saHpiSessionOpen(domainid,&sessionid,NULL);
	if (rv != SA_OK) {
		printf("saHpiSessionOpen returns %s\n",oh_lookup_error(rv));
		exit(-1);
	}
	if (fdebug)
	       	printf("saHpiSessionOpen returns with SessionId %u\n", 
			sessionid);

	/*
	 * Resource discovery ------ do we need it?
	 *
	if (fdebug) printf("saHpiDiscover\n");
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
		printf("saHpiDiscover returns %s\n",oh_lookup_error(rv));
		exit(-1);
	} */

	if (domtag.DataLength>0){
		if (fdebug) printf ("Let's go change the tag\n");
		set_domaintag(sessionid, domtag);
	}


	if (fdebug) printf ("Let's go and list the domains!\n");

	show_domain(sessionid);

	rv = saHpiSessionClose(sessionid);
	
	exit(0);
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

	if (fdebug) printf("saHpiDomainInfoGet\n");
	rv = saHpiDomainInfoGet(sessionid,&domaininfo);
	if (rv!=SA_OK) {
		printf("saHpiDomainInfoGet failed with returncode %s\n",
			oh_lookup_error(rv));
		return rv;
	}
	
	/* Print info about this domain */
	rv = print_domaininfo(domaininfo,0);

	/* walk the DRT */
	drtentryid = SAHPI_FIRST_ENTRY;
	do {
	   if (fdebug) printf("saHpiDrtEntryGet\n");
	   rv = saHpiDrtEntryGet(sessionid,
			drtentryid,&nextdrtentryid,&drtentry);
	   if ((rv != SA_OK && rv != SA_ERR_HPI_NOT_PRESENT) || fdebug) 
		       	printf("DrtEntryGet returns %s\n",oh_lookup_error(rv));
		
	   if (rv == SA_OK ) {
		if (fverbose) {
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
		    if (fdebug) {
			printf("saHpiSessionOpen returns with SessionId %u\n", 
				relatedsessionid);
			printf("saHpiDomainInfoGet for related domain %u\n",
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
		    if (fdebug) 
			printf("saHpiSessionClose returns %s\n",
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
			else printf("Internal error while walking the DRT\n");
		}
		else printf("Internal error while walking the DRT\n");

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
                                          (char *)ohdomainentry.daemonhost.Data,
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


	if (fdebug) printf("saHpiDomainInfoGet\n");
	rv = saHpiDomainInfoGet(sessionid,&domaininfo);
	if (rv!=SA_OK) {
		printf("saHpiDomainInfoGet failed with returncode %s\n",
			oh_lookup_error(rv));
		return rv;
	}
	
	printf("Old domain Tag: ");
	rv = oh_print_text(&(domaininfo.DomainTag));
	printf("\n");

	rv = saHpiDomainTagSet (sessionid, &domtag);
	if (rv!=SA_OK || fdebug)
		printf("saHpiDomainTagSet failed with returncode %s. "
			"Tag not changed.\n",oh_lookup_error(rv));
	
	return rv;
}

/* end hpidomain.c */
