/*
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2007
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * 04/15/03 Andy Cress - created
 * 04/17/03 Andy Cress - mods for resourceid, first good run
 * 04/23/03 Andy Cress - first run with good ControlStateGet   
 * 04/24/03 Andy Cress - v0.5 with good ControlStateSet
 * 04/29/03 Andy Cress - v0.6 changed control.oem values
 * 06/06/03 Andy Cress - v1.0 check for Analog States
 * 02/23/04 Andy Cress - v1.1 add checking/setting disk LEDs
 * 10/13/04 Andy Cress - v1.2 add ifdefs for HPI_A & HPI_B, added -d/raw
 * < ...for more changes look at svn logs... >
 * 09/06/2010  ulikleber  New option -D to select domain
 * 01/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                        introduce common options for all clients
 *
 * Author(s):
 * 	Andy Cress <andrew.r.cress@intel.com>
 * 	Renier Morales <renier@openhpi.org>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 */
/*M*
Copyright (c) 2003, Intel Corporation
All rights reserved.

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
 *M*/

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

#define uchar unsigned char
#define SAHPI_OEM_ALARM_LED 0x10
#define SAHPI_OEM_DISK_LED 0x20

static char *states[3] = {"off", "ON ", "unknown" };
static gint fid = -1;      // set chassis id 
#define NLEDS  6
static gint leds[NLEDS] = {  /* rdr.RdrTypeUnion.CtrlRec.Oem is an index for this */
/*pwr*/ -1, /*crt*/ -1, /*maj*/ -1, /*min*/ -1, /*diska*/ -1, /*diskb*/ -1};
static gboolean fall = FALSE;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "critical",  'c', 0, G_OPTION_ARG_INT,  &leds[1],  "Set critical alarm on|off",       "1|0" },
  { "major",     'm', 0, G_OPTION_ARG_INT,  &leds[2],  "Set major alarm on|off",          "1|0" },
  { "minor",     'n', 0, G_OPTION_ARG_INT,  &leds[3],  "Set minor alarm on|off",          "1|0" },
  { "diska",     'a', 0, G_OPTION_ARG_INT,  &leds[4],  "Set diska alarm on|off",          "1|0" },
  { "diskb",     'b', 0, G_OPTION_ARG_INT,  &leds[5],  "Set diskb alarm on|off",          "1|0" },
  { "power",     'p', 0, G_OPTION_ARG_INT,  &leds[0],  "Set power alarm on|off",          "1|0" },
  { "chassisid", 'i', 0, G_OPTION_ARG_INT,  &fid,      "Set chassis id on for n seconds", "n"  },
  { "all",       'o', 0, G_OPTION_ARG_NONE, &fall,     "Set all alarms off",               NULL },  
  { NULL }
};


int
main(int argc, char **argv)
{
  SaErrorT rv;
  SaHpiSessionIdT sessionid;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiEntryIdT nextentryid;
  SaHpiResourceIdT resourceid;
  SaHpiRdrT rdr;
  SaHpiCtrlTypeT  ctltype;
  SaHpiCtrlNumT   ctlnum;
  SaHpiCtrlStateT ctlstate;
  //int raw_val = 0;  Option d for raw alarm byte not implemented
  int b, j;
  GError *error = NULL;
  GOptionContext *context;

  /* Print version strings */
  oh_prog_version(argv[0]);

  /* Parsing options */
  static char usetext[]="- Control alarm management instruments\n  "
                        OH_SVN_REV; 
  OHC_PREPARE_REVISION(usetext);
  context = g_option_context_new (usetext);
  g_option_context_add_main_entries (context, my_options, NULL);

  if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127 ?
                    - OHC_VERBOSE_OPTION,    // no verbose mode implemented
                error)) { 
                g_option_context_free (context);
		return 1;
	}
  g_option_context_free (context);

  if (fall) { /* set all alarms off */
        fid=0;
        for (j = 0; j < NLEDS; j++) leds[j] = 0; 
  }

  rv = ohc_session_open_by_option ( &copt, &sessionid);
  if (rv != SA_OK) return rv;

  rv = saHpiDiscover(sessionid);
  if (copt.debug) printf("saHpiDiscover complete, rv = %d\n",rv);

  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
     rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
     if (rv != SA_OK) printf("RptEntryGet: rv = %d\n",rv);
     if (rv == SA_OK) {
	/* Walk the RDR list for this RPT entry */
	entryid = SAHPI_FIRST_ENTRY;
	resourceid = rptentry.ResourceId;
	/* 
	 * Don't stringify here, since OpenHPI returns a valid string, but
	 * a DataLength of zero here (for processor, bios).
	 * rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
	 */
	//if (copt.debug) 
	   printf("rptentry[%u] resourceid=%u tlen=%u tag: %s\n",
		entryid, resourceid, rptentry.ResourceTag.DataLength, 
		rptentry.ResourceTag.Data);
	while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
	{
		rv = saHpiRdrGet(sessionid,resourceid,
				entryid,&nextentryid, &rdr);
  		if (copt.debug) printf("saHpiRdrGet[%u] rv = %d\n",entryid,rv);
		if (rv == SA_OK) {
		   if (rdr.RdrType == SAHPI_CTRL_RDR) { 
			/*type 1 includes alarm LEDs*/
			ctlnum = rdr.RdrTypeUnion.CtrlRec.Num;
			rdr.IdString.Data[rdr.IdString.DataLength] = 0;
			if (copt.debug) printf("Ctl[%u]: %u %u %s\n",
				ctlnum, rdr.RdrTypeUnion.CtrlRec.Type,
				rdr.RdrTypeUnion.CtrlRec.OutputType,
				rdr.IdString.Data);
			rv = saHpiControlTypeGet(sessionid,resourceid,
					ctlnum,&ctltype);
  			if (copt.debug) printf("saHpiControlTypeGet[%u] rv = %d, type = %u\n",ctlnum,rv,ctltype);
			rv = saHpiControlGet(sessionid, resourceid, ctlnum,
					NULL, &ctlstate);
  			if (copt.debug) 
			   printf("saHpiControlStateGet[%u] rv = %d v = %x\n",
				ctlnum,rv,ctlstate.StateUnion.Digital);
			printf("RDR[%u]: ctltype=%u:%u oem=%02x %s  \t",
				rdr.RecordId, 
				rdr.RdrTypeUnion.CtrlRec.Type,
				rdr.RdrTypeUnion.CtrlRec.OutputType,
				rdr.RdrTypeUnion.CtrlRec.Oem,
				rdr.IdString.Data);
			if (rv == SA_OK) {
			   if (ctlstate.Type == SAHPI_CTRL_TYPE_ANALOG)
			        b = 2;  /*Identify*/
			   else {
			      b = ctlstate.StateUnion.Digital;
			      if (b > 2) b = 2; 
			   }
			   printf("state = %s\n",states[b]);
			} else { printf("\n"); }
			if (rdr.RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_ANALOG &&
			    rdr.RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_LED) {
			    /* This is a Chassis Identify */
			    if (fid>=0) {
				printf("Setting ID led to %u sec\n", fid);
				ctlstate.Type = SAHPI_CTRL_TYPE_ANALOG;
				ctlstate.StateUnion.Analog = fid;
				rv = saHpiControlSet(sessionid, resourceid,
						ctlnum, SAHPI_CTRL_MODE_MANUAL,
						&ctlstate);
				printf("saHpiControlStateSet[%u] rv = %d\n",ctlnum,rv);
			    }
			} else 
			if (rdr.RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL &&
			    (rdr.RdrTypeUnion.CtrlRec.Oem & 0xf0) == SAHPI_OEM_ALARM_LED &&
			    rdr.RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_LED) {
				/* this is an alarm LED */
				b = (uchar)rdr.RdrTypeUnion.CtrlRec.Oem & 0x0f;
				if ((b < NLEDS) && leds[b]>=0) {
				   printf("Setting alarm led %u to %u\n",b,leds[b]);
				   if (leds[b] == 0) 
					ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_OFF;
				   else 
					ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_ON;
				   rv = saHpiControlSet(sessionid, resourceid,
						ctlnum, SAHPI_CTRL_MODE_MANUAL,
						&ctlstate);
  				   /* if (copt.debug)  */
					printf("saHpiControlStateSet[%u] rv = %d\n",ctlnum,rv);
				}
			}
			else if (rdr.RdrTypeUnion.CtrlRec.Type == SAHPI_CTRL_TYPE_DIGITAL &&
			    (rdr.RdrTypeUnion.CtrlRec.Oem & 0xf0) == SAHPI_OEM_DISK_LED &&
			    rdr.RdrTypeUnion.CtrlRec.OutputType == SAHPI_CTRL_LED) {
				/* this is a disk LED */
				b = (uchar)rdr.RdrTypeUnion.CtrlRec.Oem & 0x0f;
				if ((b < NLEDS) && leds[b]>=0) {
				   printf("Setting disk led %u to %u\n",b,leds[b]);
				   if (leds[b] == 0) 
					ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_OFF;
				   else 
					ctlstate.StateUnion.Digital = SAHPI_CTRL_STATE_ON;
				   rv = saHpiControlSet(sessionid, resourceid,
						ctlnum, SAHPI_CTRL_MODE_MANUAL,
						&ctlstate);
				   printf("saHpiControlStateSet[%u] rv = %d\n",ctlnum,rv);
				}
			}

			rv = SA_OK;  /* ignore errors & continue */
		    }
		    j++;
		    entryid = nextentryid;
		}
	}
	rptentryid = nextrptentryid;
     }
  }
 
  rv = saHpiSessionClose(sessionid);

  return(0);
}
 
/*-----------Sample output-----------------------------------
hpialarmpanel ver 0.6
RptInfo: UpdateCount = 5, UpdateTime = 8a2dc6c0
rptentry[0] resourceid=1 tag: Mullins
RDR[45]: ctltype=2:1 oem=0  Chassis Identify Control
RDR[48]: ctltype=0:1 oem=10 Front Panel Power Alarm LED         state = off
RDR[51]: ctltype=0:1 oem=13 Front Panel Minor Alarm LED         state = ON
RDR[46]: ctltype=0:0 oem=0  Cold Reset Control
RDR[49]: ctltype=0:1 oem=11 Front Panel Critical Alarm LED      state = off
RDR[50]: ctltype=0:1 oem=12 Front Panel Major Alarm LED         state = off
 *-----------------------------------------------------------*/
/* end hpialarmpanel.c */
