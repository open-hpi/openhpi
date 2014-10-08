/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
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
 * Filename: hpiwdt.c
 * Authors:  Andy Cress <arcress@users.sourceforge.net>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 * 
 * Changes:
 * 03/15/04 Andy Cress - v1.0 added strings for use & actions in show_wdt
 * 10/13/04  kouzmich  - porting to HPI B
 * 12/02/04 Andy Cress - v1.1 fixed domain/RPT loop, added some decoding
 * 09/06/2010 ulikleber New option -D to select domain
 * 01/02/2011 ulikleber Refactoring to use glib for option parsing and
 *                      introduce common options for all clients
 *
 */
/* 
 * This tool reads and enables the watchdog timer via HPI.
 * Note that there are other methods for doing this, and the
 * standard interface is for the driver to expose a /dev/watchdog
 * device interface.
 * WARNING: If you enable the watchdog, make sure you have something
 * set up to keep resetting the timer at regular intervals, or it
 * will reset your system.
 */

#include "oh_clients.h"

#define  uchar  unsigned char
#define OH_SVN_REV "$Revision$"

#define NUSE  6
char *usedesc[NUSE] = {"reserved", "BIOS FRB2", "BIOS/POST",
                    "OS Load", "SMS/OS", "OEM" };
#define NACT  5
char *actions[NACT] = {"No action", "Hard Reset", "Power down",
		    "Power cycle", "Reserved" };

static gboolean fenable    = FALSE;
static gboolean fdisable = FALSE;
static gboolean freset     = FALSE;
static gint     ftimeout   = 0;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "enable",  'e', 0, G_OPTION_ARG_NONE, &fenable,  "enables the watchdog timer",  NULL },
  { "disable", 'd', 0, G_OPTION_ARG_NONE, &fdisable, "disables the watchdog timer", NULL },
  { "reset",   'r', 0, G_OPTION_ARG_NONE, &freset,   "resets the watchdog timer",   NULL },
  { "timeout", 't', 0, G_OPTION_ARG_INT,  &ftimeout, "sets timeout to n seconds",   "n" },
  { NULL }
};

static void
show_wdt(SaHpiWatchdogNumT  wdnum, SaHpiWatchdogT *wdt)
{
  int icount, pcount;
  char ustr[12]; 
  char astr[16]; 
  char estr[30]; 
  char *pstr;
  icount = wdt->InitialCount /1000;        /*1000 msec = 1 sec*/
  pcount = wdt->PresentCount /1000;

  if (wdt->TimerUse > NUSE) sprintf(ustr,"%u", wdt->TimerUse );
  else strcpy(ustr, usedesc[wdt->TimerUse]);
  if (wdt->TimerAction > NACT) sprintf(astr,"%u", wdt->TimerAction );
  else strcpy(astr, actions[wdt->TimerAction]);
  printf("Watchdog: Num=%u, Log=%d, Running=%d, TimerUse=%s, TimerAction=%s\n",
	wdnum,wdt->Log,wdt->Running,ustr, astr);
  if (wdt->TimerUseExpFlags == 0) strcpy(estr,"none");
  else {
	estr[0] = 0;
	if (wdt->TimerUseExpFlags & 0x01) strcat(estr,"FRB2 ");
	if (wdt->TimerUseExpFlags & 0x02) strcat(estr,"POST ");
	if (wdt->TimerUseExpFlags & 0x04) strcat(estr,"OS_Load ");
	if (wdt->TimerUseExpFlags & 0x08) strcat(estr,"SMS_OS ");
	if (wdt->TimerUseExpFlags & 0x10) strcat(estr,"OEM ");
  }
  printf("          ExpiredUse=%s, Timeout=%u sec, Counter=%u sec\n",
	estr, icount,pcount);
  switch(wdt->PretimerInterrupt) {
  case 1:  pstr = "SMI";    break;
  case 2:  pstr = "NMI";    break;
  case 3:  pstr = "MsgInt"; break;
  default: pstr = "none";   break;
  }
  printf("          PreTimerInterrupt=%s, PreTimeoutInterval=%u msec\n",
	 pstr,wdt->PreTimeoutInterval);
  return;
}

int
main(int argc, char **argv)
{
  SaErrorT rv;
  SaHpiSessionIdT sessionid;
  SaHpiDomainInfoT domainInfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiResourceIdT resourceid;
  SaHpiWatchdogNumT  wdnum;
  SaHpiWatchdogT     wdt;
  GOptionContext *context;

  /* Print version strings */
  oh_prog_version(argv[0]);

  /* Parsing options */
  static char usetext[]="- Read and Enables the watchdog timer.\n  "
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
  if (ftimeout == 0) ftimeout = 120;
  else fenable = TRUE;
  
  rv = ohc_session_open_by_option ( &copt, &sessionid);
  if (rv != SA_OK) return rv;

  rv = saHpiDiscover(sessionid);
  if (copt.debug) DBG("saHpiDiscover rv = %s",oh_lookup_error(rv));
  rv = saHpiDomainInfoGet(sessionid, &domainInfo);
  if (copt.debug) DBG("saHpiDomainInfoGet rv = %s",oh_lookup_error(rv));
  printf("DomainInfo: UpdateCount = %x, UpdateTime = %lx\n",
       domainInfo.RptUpdateCount, (unsigned long)domainInfo.RptUpdateTimestamp);

  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
     rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
     if (rv != SA_OK) printf("RptEntryGet: rv = %s\n",oh_lookup_error(rv));
     if (rv == SA_OK) {
	/* handle WDT for this RPT entry */
	resourceid = rptentry.ResourceId;
	rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	if (copt.debug)
	   printf("rptentry[%u] resourceid=%u capab=%x tag: %s\n",
		rptentryid, resourceid, rptentry.ResourceCapabilities, 
		rptentry.ResourceTag.Data);

	if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG) {
	   printf("%s has watchdog capability\n",rptentry.ResourceTag.Data);

	   wdnum = SAHPI_DEFAULT_WATCHDOG_NUM;
	   rv = saHpiWatchdogTimerGet(sessionid,resourceid,wdnum,&wdt);
	   if (copt.debug) DBG("saHpiWatchdogTimerGet rv = %s",oh_lookup_error(rv));
	   if (rv != 0) {
		printf("saHpiWatchdogTimerGet error = %s\n",oh_lookup_error(rv));
		rv = 0;
		rptentryid = nextrptentryid; 
		continue;
	   }
	   show_wdt(wdnum,&wdt);

	   if (fdisable) {
	      printf("Disabling watchdog timer ...\n");
	      /* clear FRB2, timeout back to 120 sec */
	      /* TODO: add setting wdt values here */
	      wdt.TimerUse = SAHPI_WTU_NONE;    /* 1=FRB2 2=POST 3=OSLoad 4=SMS_OS 5=OEM */
	      wdt.TimerAction = SAHPI_WA_NO_ACTION; /* 0=none 1=reset 2=powerdown 3=powercycle */
	      wdt.PretimerInterrupt = SAHPI_WPI_NONE; /* 0=none 1=SMI 2=NMI 3=message */
	      wdt.PreTimeoutInterval = 60000; /*msec*/
	      wdt.InitialCount = 120000; /*msec*/
	      wdt.PresentCount = 120000; /*msec*/

	      rv = saHpiWatchdogTimerSet(sessionid,resourceid,wdnum,&wdt);
	      if (copt.debug) DBG("saHpiWatchdogTimerSet rv = %s",oh_lookup_error(rv));
	      if (rv == 0) show_wdt(wdnum,&wdt);
	   } else if (fenable) {
	      printf("Enabling watchdog timer ...\n");
	      /* hard reset action, no pretimeout, clear SMS/OS when done */
	      /* use ftimeout for timeout */
	      wdt.TimerUse = SAHPI_WTU_SMS_OS;    /* 1=FRB2 2=POST 3=OSLoad 4=SMS_OS 5=OEM */
	      wdt.TimerAction = SAHPI_WA_RESET; /* 0=none 1=reset 2=powerdown 3=powercycle */
	      wdt.PretimerInterrupt = SAHPI_WPI_NMI; /* 0=none 1=SMI 2=NMI 3=message */
	      wdt.PreTimeoutInterval = (ftimeout / 2) * 1000; /*msec*/
	      wdt.InitialCount = ftimeout * 1000; /*msec*/
	      wdt.PresentCount = ftimeout * 1000; /*msec*/

	      rv = saHpiWatchdogTimerSet(sessionid,resourceid,wdnum,&wdt);
	      if (copt.debug) DBG("saHpiWatchdogTimerSet rv = %s",oh_lookup_error(rv));
	      if (rv == 0) show_wdt(wdnum,&wdt);
	   }
	   if (freset && !fdisable) {
	      printf("Resetting watchdog timer ...\n");
	      rv = saHpiWatchdogTimerReset(sessionid,resourceid,wdnum);
	      if (copt.debug) DBG("saHpiWatchdogTimerReset rv = %s",oh_lookup_error(rv));
	   }
	} /*watchdog capability*/
	rptentryid = nextrptentryid;  /* get next RPT (usu only one anyway) */
     }  /*endif RPT ok*/
  }  /*end while loop*/
 
  rv = saHpiSessionClose(sessionid);

  return 0;
}
 
/* end hpiwdt.c */
