/* -*- linux-c -*-
 *
 *
 * (C) Copyright IBM Corp 2003,2004
 * Authors:  
 *     Peter D.Phan pdphan@users.sourceforge.net
 *  
 *     01/13/2004 pdphan  module created
 *			  Change clock for event log on IBM Blade Center E.
 *     03/10/2004 pdphan  Remove reference to IBM Blade Center.
 */
/*M*

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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "SaHpi.h"

char progver[] = "1.0";
int fdebug = 0;
int findate = 0;
int fintime = 0;


#define NECODES  27
struct { int code; char *str;
} ecodes[NECODES] = { 
 {  0,    "Success" },
 { -1001, "HPI unspecified error" },
 { -1002, "HPI unsupported function" },
 { -1003, "HPI busy" },
 { -1004, "HPI request invalid" },
 { -1005, "HPI command invalid" },
 { -1006, "HPI timeout" },
 { -1007, "HPI out of space" },
 { -1008, "HPI data truncated" },
 { -1009, "HPI data length invalid" },
 { -1010, "HPI data exceeds limits" },
 { -1011, "HPI invalid params" },
 { -1012, "HPI invalid data" },
 { -1013, "HPI not present" },
 { -1014, "HPI invalid data field" },
 { -1015, "HPI invalid sensor command" },
 { -1016, "HPI no response" },
 { -1017, "HPI duplicate request" },
 { -1018, "HPI updating" },
 { -1019, "HPI initializing" },
 { -1020, "HPI unknown error" },
 { -1021, "HPI invalid session" },
 { -1022, "HPI invalid domain" },
 { -1023, "HPI invalid resource id" },
 { -1024, "HPI invalid request" },
 { -1025, "HPI entity not present" },
 { -1026, "HPI uninitialized" }
};

char def_estr[15] = "HPI error %d   ";

static char *decode_error(SaErrorT code)
{
	int i;
	char *str = NULL;
	for (i = 0; i < NECODES; i++) {
		if (code == ecodes[i].code) { str = ecodes[i].str; break; }
	}

	if (str == NULL) { 
		sprintf(&def_estr[10],"%d",code);
		str = &def_estr[0];
	}

	return(str);
}

static void saftime2str(SaHpiTimeT time, char * str, size_t size)
{
	struct tm t;
	time_t tt; 
	tt = time / 1000000000;
	localtime_r(&tt, &t);
	strftime(str, size, "%b %d, %Y - %H:%M:%S", &t);
}

static void usage(char **argv)
{
	printf("Usage\n\t%s -d mm/dd/yyyy -t HH:MM:SS [-x]\n",argv[0]);
	printf("\twhere -d date in mm/dd/yyyy format\n");
	printf("\t      -t time of day in 24-hr format\n");
	printf("\t      -x displays eXtra debug messages\n");

	return;
}

int main(int argc, char **argv)
{
	int c;
	char i_newdate[20];
	char i_newtime[20];
	char timestr[40];
	struct tm  new_tm_time;
	SaErrorT rv;
	SaHpiVersionT hpiVer;
	SaHpiSessionIdT sessionid;
	SaHpiRptInfoT rptinfo;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiResourceIdT resourceid;
	SaHpiSelEntryIdT entryid;
	SaHpiTimeT oldtime;
	SaHpiTimeT newtime;
	SaHpiTimeT readbacktime;
        
	printf("%s: version %s\n",argv[0],progver); 
        
	while ( (c = getopt( argc, argv,"d:t:x")) != EOF )
	{
		switch(c) {
			case 'd': 
				findate = 1;
				strcpy(i_newdate, optarg);
				break;
			case 't': 
				fintime = 1;
				strcpy(i_newtime, optarg);
				break;
			case 'x':
				fdebug = 1;
				break;
			default:
				usage(argv);
				exit(1);
		}
	}
	
	if ( !findate || !fintime) {
		usage(argv);
		exit(1);
	}

	if (findate) {
		if (fdebug) printf("New date to be set: %s\n",i_newdate);
	        sscanf(i_newdate,"%2d/%2d/%4d",
                  &new_tm_time.tm_mon, &new_tm_time.tm_mday, &new_tm_time.tm_year);
		new_tm_time.tm_mon --;
		new_tm_time.tm_year -= 1900;
	} else {
		printf("No Date\n");
	}

	if (fintime) {
		if (fdebug)  printf("New time to be set:  %s\n",i_newtime);
	        sscanf(i_newtime,"%2d:%2d:%2d",
                  &new_tm_time.tm_hour, &new_tm_time.tm_min, &new_tm_time.tm_sec);
	} else {
		printf("No Time\n");
	}

	if (fdebug) printf("Values passed to mktime():\n\tmon %d\n\tday %d\n\tyear %d\n\tHH %d\n\tMM %d\n\tSS %d\n",
			new_tm_time.tm_mon, new_tm_time.tm_mday, new_tm_time.tm_year,
			new_tm_time.tm_hour, new_tm_time.tm_min, new_tm_time.tm_sec);

	newtime = (SaHpiTimeT) mktime(&new_tm_time) * 1000000000;
	if (fdebug) printf("New date and time in SaHpiTimeT %lli\n", (long long int)newtime);
	rv = saHpiInitialize(&hpiVer);
	if (rv != SA_OK) {
		printf("saHpiInitialize: %s\n",decode_error(rv));
		exit(-1);
	}

	rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID,&sessionid,NULL);
	if (rv != SA_OK) {
		if (rv == SA_ERR_HPI_ERROR) 
			printf("saHpiSessionOpen: error %d, SpiLibd not running\n",rv);
		else
			printf("saHpiSessionOpen: %s\n",decode_error(rv));
		exit(-1);
	}
 
	rv = saHpiResourcesDiscover(sessionid);
	if (fdebug) printf("saHpiResourcesDiscover %s\n",decode_error(rv));
	rv = saHpiRptInfoGet(sessionid,&rptinfo);
	if (fdebug) printf("saHpiRptInfoGet %s\n",decode_error(rv));
	printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
		rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
        
	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
	{
                rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
                if (fdebug) printf("saHpiRptEntryGet %s\n",decode_error(rv));
                if ((rv == SA_OK) && (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_SEL)) {
                        resourceid = rptentry.ResourceId;
                        if (fdebug) printf("RPT %x capabilities = %x\n", resourceid,
                                           rptentry.ResourceCapabilities);
			rv = saHpiEventLogTimeGet(sessionid, resourceid, &oldtime);
			saftime2str(oldtime, timestr, sizeof(timestr));
			printf ("\nCurrent event log time on HPI target: %s\n", timestr);
			printf ("Setting new event log time on HPI target ...\n");
		 	rv = saHpiEventLogTimeSet(sessionid, resourceid, newtime);
			if (rv != SA_OK) 
			{
                		printf("saHpiEventLogTimeSet %s\n",decode_error(rv));
			}
			rv = saHpiEventLogTimeGet(sessionid, resourceid, &readbacktime);
			saftime2str(readbacktime, timestr, sizeof(timestr));
			printf ("Read-Back-Check event log time: %s\n", timestr);

                }
                entryid = SAHPI_OLDEST_ENTRY;
                rptentryid = nextrptentryid;
	} 
        
        rv = saHpiSessionClose(sessionid);
        rv = saHpiFinalize();
        
        exit(0);
        return(0);
}
 
/* end hpisettime.c */
