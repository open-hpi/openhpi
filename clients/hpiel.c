/* -*- linux-c -*-
 *
 * Copyright (c) 2003 Intel Corporation.
 * (C) Copyright IBM Corp 2004
 *
 * Author(s):
 *     Andy Cress  arcress@users.sourceforge.net
 *     Sean Dague <http://dague.net/sean>
 *
 */
/*
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
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <SaHpi.h>

static void ShowSel(SaHpiEventLogEntryT *sel, SaHpiRdrT *rdr, SaHpiRptEntryT *rptentry);

char progver[] = "1.2";
int fdebug = 0;
int fclear = 0;

int main(int argc, char **argv)
{
        int c;
        SaErrorT rv;
        SaHpiVersionT hpiVer;
        SaHpiSessionIdT sessionid;
        SaHpiRptInfoT rptinfo;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT rptentryid;
        SaHpiEntryIdT nextrptentryid;
        SaHpiResourceIdT resourceid;
        SaHpiEventLogEntryIdT entryid;
        SaHpiEventLogEntryIdT nextentryid;
        SaHpiEventLogEntryIdT preventryid;
        SaHpiEventLogInfoT info;
        SaHpiEventLogEntryT  el;
        SaHpiRdrT rdr;
	SaHpiTextBufferT buffer;
        int free = 50;

        printf("%s: version %s\n",argv[0],progver);

        while ( (c = getopt( argc, argv,"cx?")) != EOF )
                switch(c) {
                case 'c': fclear = 1; break;
                case 'x': fdebug = 1; break;
                default:
                        printf("Usage %s [-cx]\n",argv[0]);
                        printf("where -c clears the event log\n");
                        printf("      -x displays eXtra debug messages\n");
                        exit(1);
                }
        rv = saHpiInitialize(&hpiVer);
        if (rv != SA_OK) {
                printf("saHpiInitialize: %s\n", oh_lookup_error(rv));
                exit(-1);
        }
        rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID,&sessionid,NULL);
        if (rv != SA_OK) {
                if (rv == SA_ERR_HPI_ERROR)
                        printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
                else
                        printf("saHpiSessionOpen: %s\n", oh_lookup_error(rv));
                exit(-1);
        }

        rv = saHpiResourcesDiscover(sessionid);
        if (fdebug) printf("saHpiResourcesDiscover %s\n", oh_lookup_error(rv));
        rv = saHpiRptInfoGet(sessionid,&rptinfo);
        if (fdebug) printf("saHpiRptInfoGet %s\n", oh_lookup_error(rv));
        printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
               rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);

        /* walk the RPT list */
        rptentryid = SAHPI_FIRST_ENTRY;
        while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
                                {
                            rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);

			    if (fdebug)
				    printf("saHpiRptEntryGet %s\n", oh_lookup_error(rv));
			    
			    if (rv == SA_OK) {
				    resourceid = rptentry.ResourceId;
				    
				    if (fdebug)
					    printf("RPT %d capabilities = %x\n", resourceid,
						   rptentry.ResourceCapabilities);
				    
				    if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
					    if (fdebug)
						    printf("RPT doesn't have SEL\n");
					    rptentryid = nextrptentryid;
					    continue;  /* no SEL here, try next RPT */
				    }
				    
				    rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
				    printf("rptentry[%d] tag: %s\n", resourceid,rptentry.ResourceTag.Data);
				    
				    /* initialize structure */
				    info.Entries = 0;
				    info.Size = 0;
				    info.Enabled = 0;
				    
				    rv = saHpiEventLogInfoGet(sessionid,resourceid,&info);
				    if (fdebug)
					    printf("saHpiEventLogInfoGet %s\n", oh_lookup_error(rv));
				    if (rv == SA_OK) {
					    printf("EventLog entries=%d, size=%d, enabled=%d\n",
						   info.Entries,info.Size,info.Enabled);
					    free = info.Size - info.Entries;
					    oh_decode_time(info.UpdateTimestamp, &buffer);
					    printf("UpdateTime = %s, ", buffer.Data);
					    oh_decode_time(info.CurrentTime, &buffer);
					    printf("CurrentTime = %s\n", buffer.Data);
					    printf("Overflow = %d\n", info.OverflowFlag);
					    printf("DeleteEntrySupported = %d\n", info.DeleteEntrySupported);
				    }
				    
				    if (fclear) {
					    rv = saHpiEventLogClear(sessionid,resourceid);
					    if (rv == SA_OK)
						    printf("EventLog successfully cleared\n");
					    else
						    printf("EventLog clear, error = %d, %s\n", rv, oh_lookup_error(rv));
					    break;
				    }
				    
				    if (info.Entries != 0){
					    entryid = SAHPI_OLDEST_ENTRY;
					    while ((rv == SA_OK) && (entryid != SAHPI_NO_MORE_ENTRIES))
					    {
						    rv = saHpiEventLogEntryGet(sessionid,resourceid,
									       entryid,&preventryid,&nextentryid,
									       &el,&rdr,NULL);
						    if (fdebug)
							    printf("saHpiEventLogEntryGet %s\n",
								   oh_lookup_error(rv));
						    if (rv == SA_OK) {
							    
							    ShowSel(&el, &rdr, &rptentry);
							    preventryid = entryid;
							    entryid = nextentryid;
						    }
					    }
				    } else
					    printf("SEL is empty\n");
				    
				    if (free < 6) {
					    printf("WARNING: Log free space is very low (%d records)\n",free);
					    printf("\tClear log with hpiel -c\n");
				    }
				    
				    rptentryid = nextrptentryid;
			    }
                                }
	printf("done.\n");
	rv = saHpiSessionClose(sessionid);
	rv = saHpiFinalize();
	
//              exit(0);
	
	return(0);
}

static void ShowSel( SaHpiEventLogEntryT  *sel, SaHpiRdrT *rdr,
		    SaHpiRptEntryT *rptentry )
{
        unsigned char evtype;
        int ec, eci;
        int es, esi;
        char *srctag;
        char *rdrtag;
        const char *pstr;
        char estag[8];
        unsigned char *pd;
        int ix, i, styp;
        int outlen;
        char outbuf[255];
        char mystr[26];
        unsigned char data1, data2, data3;
	SaHpiTextBufferT text;

        if (!sel || !rdr || !rptentry) return;
	
	/*format & print the EventLog entry*/

	oh_decode_time(sel->Event.Timestamp, &text);
        
	if (rptentry->ResourceId == sel->Event.Source)
                srctag = rptentry->ResourceTag.Data;
        else
                srctag = "unspec ";  /* SAHPI_UNSPECIFIED_RESOURCE_ID */

        evtype = sel->Event.EventType;
        if (evtype > NEVTYPES)
                evtype = NEVTYPES - 1;

        if (rdr->RdrType == SAHPI_NO_RECORD) {
                rdrtag = "rdr-unkn";
        } else {
                rdr->IdString.Data[rdr->IdString.DataLength] = 0;
                rdrtag = &rdr->IdString.Data[0];
        }
	
	/* yes, there are BIG MASSIVE security issues with
	 * the following code.  They were there before, and
	 * we need more cleanup to really get rid of them */
        sprintf(outbuf,"%i ", sel->EntryId);
	strncat(outbuf, (char *) text.Data, text.DataLength);
	strcat(outbuf, (const char *) oh_lookup_eventtype(evtype));
        
	outlen = strlen(outbuf);
        pstr = "";

        /*
          sld: there is a lot of stuff specific to IPMI and other HPI implementations
          here.  Scrubing for HPI 1.0 only data would be a good thing soon
        */

        switch(evtype)
        {
        case SAHPI_ET_SENSOR:   /*Sensor*/
                /* decode event category */
                ec = sel->Event.EventDataUnion.SensorEvent.EventCategory;
                for (eci = 0; eci < NUM_EC; eci++)
                        if (eventcats[eci].val == ec) break;
                if (eci >= NUM_EC) eci = 0;
                /* decode event state */
                es = sel->Event.EventDataUnion.SensorEvent.EventState;
                if (eci == 1) { /*SAHPI_EC_THRESHOLD*/
                        for (esi = 0; esi < NUM_ES; esi++)
                                if (eventstates[esi].val == es) break;
                        if (esi >= NUM_ES) esi = 0;
                        strcpy(estag,eventstates[esi].str);
                } else sprintf(estag,"%02x",es);

                /* decode sensor type */
                styp = sel->Event.EventDataUnion.SensorEvent.SensorType;
                /* data3 is not specifically defined in HPI 1.0, implementation hack */
                pd = (unsigned char *)&sel->Event.EventDataUnion.SensorEvent.SensorSpecific;
                data1 = pd[0];
                data2 = pd[1];
                data3 = pd[2];
                if (styp >= NUMST) { styp = 0; }

                if (styp == 0x20) { /*OS Critical Stop*/
                        /* Show first 3 chars of panic string */
                        mystr[0] = '(';
                        mystr[1] = sel->Event.EventDataUnion.SensorEvent.SensorNum & 0x7f;
                        mystr[2] = data2 & 0x007f;
                        mystr[3] = data3 & 0x7f;
                        mystr[4] = ')';
                        mystr[5] = 0;
                        if (sel->Event.EventDataUnion.SensorEvent.SensorNum & 0x80)
                                strcat(mystr,"Oops!");
                        if (data2 & 0x80) strcat(mystr,"Int!");
                        if (data3 & 0x80) strcat(mystr,"NullPtr!");
                        pstr = mystr;
                }
                sprintf(&outbuf[outlen], "%s, %s %s %x [%02x %02x %02x] %s",
                        sensor_types[styp], eventcats[eci].str, estag,
                        sel->Event.EventDataUnion.SensorEvent.SensorNum,
                        data1, data2, data3, pstr);
                break;
        case SAHPI_ET_USER:   /*User, usu 16-byte IPMI SEL record */
                pd = &sel->Event.EventDataUnion.UserEvent.UserEventData.Data[0];
                /* get gen_desc from offset 7 */
                for (ix = 0; ix < NGDESC; ix++)
                        if (gen_desc[ix].val == pd[7]) break;
                if (ix >= NGDESC) ix = 0;
                /* get sensor type description for misc cases */
                styp = pd[10];   /*sensor type*/
                data3 = pd[15];
                /* = *sel->Event.EventDataUnion.SensorEvent.SensorSpecific+1; */
                for (i = 0; i < NSDESC; i++) {
                        if (sens_desc[i].s_typ == styp) {
                                if (sens_desc[i].s_num != 0xff &&
                                    sens_desc[i].s_num != pd[11])
                                        continue;
                                if (sens_desc[i].data1 != 0xff &&
                                    (sens_desc[i].data1 & 0x07) != pd[13])
                                        continue;
                                if (sens_desc[i].data2 != 0xff &&
                                    sens_desc[i].data2 != pd[14])
                                        continue;
                                if (sens_desc[i].data3 != 0xff &&
                                    sens_desc[i].data3 != data3)
                                        continue;
                                /* have a match, use description */
                                pstr = (char *)sens_desc[i].desc;
                                break;
                        }
                } /*end for*/
                if (i >= NSDESC) {
                        if (styp >= NUMST) styp = 0;
                        pstr = sensor_types[styp];
                }
                sprintf(&outbuf[outlen], "%s, %s %02x %02x %02x [%02x %02x %02x]",
                        pstr, gen_desc[ix].str,
                        pd[10],pd[11],pd[12],pd[13],pd[14],data3);
                break;
        case SAHPI_ET_OEM:
                /* only go into this if it is IBM hardware, as others might use
                   the Oem field differently */
                if(sel->Event.EventDataUnion.OemEvent.MId == 2) {
                        /* sld: I'm going to printf directly, as the output buffer isn't
                           big enough for what I want to do */
                        printf("Oem Event:\n");
			
			oh_decode_time(sel->Timestamp, &text);
                        printf("\tTimestamp: ");
			oh_print_textbuffer(&text);
			printf("\n");
			
                        printf("\tSeverity: %d\n", sel->Event.Severity);
                        printf("\tMId:%d, Data: %s\n",
                               sel->Event.EventDataUnion.OemEvent.MId,
                               sel->Event.EventDataUnion.OemEvent.OemEventData.Data);
                }
                break;
        default:
                pd = &sel->Event.EventDataUnion.UserEvent.UserEventData.Data[0];
                styp = pd[10];
                data3 = pd[15];
                /* *sel->Event.EventDataUnion.SensorEvent.SensorSpecific+1 */
                if (styp >= NUMST) {
                        printf("sensor type %d >= max %d\n",styp,NUMST);
                        styp = 0;
                }
                pstr = sensor_types[styp];
                sprintf(&outbuf[outlen], "%s, %x %x, %02x %02x %02x [%02x %02x %02x/%02x]",
                        pstr, pd[0], pd[7], pd[10], pd[11], pd[12],
                        pd[13], pd[14], pd[15], data3);
                break;
        }
        printf("%s\n",outbuf);
}

