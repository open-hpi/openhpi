/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
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
 * Authors:
 *     Andy Cress <arcress@users.sourceforge.net>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *     10/13/2004  kouzmich   porting to HPI B
 *     09/06/2010  ulikleber  New option -D to select domain
 *     20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

#define  uchar  unsigned char
static gboolean f_hard   = TRUE;  // hard reset as default 
static gboolean f_warm   = FALSE; 
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "hard",             'r', 0, G_OPTION_ARG_NONE, &f_hard, "hard resets the system",       NULL },
//{ "power-down",       'd', 0, G_OPTION_ARG_NONE, &f_,     "powers Down the system",       NULL },
  { "warm",             'w', 0, G_OPTION_ARG_NONE, &f_warm, "warm resets the system",       NULL },
//{ "power-cycle",      'c', 0, G_OPTION_ARG_NONE, &f_,     "power cycles the system",      NULL },
//{ "nmi",              'n', 0, G_OPTION_ARG_NONE, &f_,     "sends NMI to the system",      NULL },
//{ "soft-shutdown",    'o', 0, G_OPTION_ARG_NONE, &f_,     "soft-shutdown OS",             NULL },
//{ "service-partition",'s', 0, G_OPTION_ARG_NONE, &f_,     "reboots to Service Partition", NULL },
  { NULL }
};

int
main(int argc, char **argv)
{
  int is_reset = 0;
  SaErrorT rv;
  SaHpiSessionIdT sessionid;
  SaHpiDomainInfoT domainInfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiResourceIdT resourceid;
  SaHpiResetActionT action = SAHPI_COLD_RESET; // hard reset as default 
  GOptionContext *context;
 
  /* Print version strings */
  oh_prog_version(argv[0]);

  /* Parsing options */
  static char usetext[]="- Exercise HPI Reset Management APIs\n  "
                        OH_SVN_REV; 
  OHC_PREPARE_REVISION(usetext);
  context = g_option_context_new (usetext);
  g_option_context_add_main_entries (context, my_options, NULL);

  if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127?
                    - OHC_VERBOSE_OPTION )) {   // no verbose mode implemented
                g_option_context_free (context);
		return 1;
  }
  g_option_context_free (context);

  if (f_warm) action = SAHPI_WARM_RESET;

  rv = ohc_session_open_by_option ( &copt, &sessionid);
  if (rv != SA_OK) return rv;

  rv = saHpiDiscover(sessionid);
  if (copt.debug) DBG("saHpiDiscover rv = %d",rv);
  rv = saHpiDomainInfoGet(sessionid, &domainInfo);
  if (copt.debug) DBG("saHpiDomainInfoGet rv = %d",rv);
  printf("RptInfo: UpdateCount = %x, UpdateTime = %lx\n",
         domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
     SaErrorT rv1;
     rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
     if (rv != SA_OK) printf("saHpiRptEntryGet: rv = %d\n",rv);
     if (rv == SA_OK) {
	/* walk the RDR list for this RPT entry */
	entryid = SAHPI_FIRST_ENTRY;
	resourceid = rptentry.ResourceId;
	rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	printf("rptentry[%u] resourceid=%u tag: %s\n",
		entryid,resourceid, rptentry.ResourceTag.Data);
        if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RESET) {
		is_reset = 1;

		rv1 = saHpiResourceResetStateSet(sessionid, 
	     		resourceid, action);
	     		//resourceid, SAHPI_POWER_OFF);
        	printf("ResetStateSet status = %d...requested %d\n",rv1, action);
	}
     }
     rptentryid = nextrptentryid;
  }
 
  rv = saHpiSessionClose(sessionid);

	if (is_reset == 0)
		printf("No resources with Reset capability found\n");

  return(0);
}
 
/* end hpireset.c */
