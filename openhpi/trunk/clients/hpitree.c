/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Peter D. Phan <pdphan@users.sourceforge.net>
 *     Tariq Shureih <tariq.shureih@intel.com>
 *
 * Log: 
 *     Copied from hpifru.c and modified for general use
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <SaHpi.h> 
#include <oh_utils.h>
#include <pthread.h>


/* 
 * Function prototypes
 */
void *sahpi_discover_thread(void *sessionid);
void usage(void);
SaErrorT list_all(SaHpiSessionIdT sessionid);
SaErrorT list_rpt(SaHpiSessionIdT sessionid);
SaErrorT list_inv(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_sens(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_ctrl(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_wdog(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_ann (SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);

SaErrorT walkInventory(SaHpiSessionIdT sessionid,
		   SaHpiResourceIdT resourceid,
		   SaHpiIdrInfoT *idrInfo);


/* 
 * Globals for this driver
 */
char progver[] = "0.1 HPI-B";
char progname[] = "hpitree";
int thread = 1;
int fdebug = 0;
int fthread = 0;

#define get_resourceid \
do { \
	list_rpt(sessionid); \
	printf("Enter resource id:"); \
	p = fgets(input, 255, stdin); \
	if ((p = strchr(input, '\n')) != NULL)	*p = '\0'; \
} while(0)


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
	SaErrorT 	rv = SA_OK;
	
	SaHpiVersionT	hpiVer;
	SaHpiSessionIdT sessionid;

	char input[255], *p;
	pthread_t discover_thread;
	void *thread_done;
	int valid = 0;
	int c;
	    
	printf("\n\n%s ver %s\n",argv[0],progver);
	while ( (c = getopt( argc, argv,"xt?")) != EOF ) {
		switch(c) {
			case 't': fthread = 1; break;
			case 'x': fdebug = 1; break;
			default:
				printf("Usage: %s [-x] [-t]\n", progname);
				printf("   -x  Display debug messages\n");
				printf("   -t  Enable resource (re)discovery thread\n\n\n\n");
				exit(1);
		}
	}

	/* 
	 * House keeping:
	 * 	-- get (check?) hpi implementation version
	 *      -- open hpi session	
	 */
	if (fdebug) printf("saHpiVersionGet\n");
	hpiVer = saHpiVersionGet();
	printf("Hpi Version %d Implemented.\n", hpiVer);

	if (fdebug) printf("saHpiSessionOpen\n");
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
	if (rv != SA_OK) {
		printf("saHpiSessionOpen returns %s\n",oh_lookup_error(rv));
		exit(-1);
	}
	if (fdebug)
	       	printf("saHpiSessionOpen returns with SessionId %d\n", sessionid);

	/*
	 * Initial discovery
	 */
	if (fdebug) printf("saHpiDiscover\n");
	rv = saHpiDiscover(sessionid);
	if ((rv != SA_OK) || fdebug) {
		printf("saHpiDiscover returns %s\n",oh_lookup_error(rv));
	}

	printf("Initial discovery done\n");

	if(fthread) {
		printf("Start (re)discovery thread\n");
		rv = pthread_create(&discover_thread, NULL, sahpi_discover_thread,
							      (void *)sessionid);
		if (rv)
			printf("Error creating event thread\n");
	}

	while (!valid) {
        
		printf("\n\n\t%s ver %s\n",argv[0],progver);
		printf("\tEnter a command or \"help\" for list of commands\n");
		printf("Command> ");

		fflush(stdout);
		p = fgets(input, 255, stdin);
        
		if ((p = strchr(input, '\n')) != NULL)
						*p = '\0';
        	if (!strcmp(input, "all")) {
			list_all(sessionid);
			
        	} else if (!strcmp(input, "rpt")) {
			list_rpt(sessionid);
			
        	} else if (!strcmp(input, "inv")) { 
			get_resourceid;
			list_inv(sessionid, (SaHpiResourceIdT) atoi(input));
			        
		} else if (!strcmp(input, "all_inv")) {
			list_inv(sessionid, 0XFF);
			
        	} else if (!strcmp(input, "sensor")) {
			get_resourceid;
			list_sens(sessionid, (SaHpiResourceIdT) atoi(input));
			
        	} else if (!strcmp(input, "all_sensors")) {
			list_sens(sessionid, 0XFF);
			
        	} else if (!strcmp(input, "cntl")) {
			/*get_resourceid;*/
			list_ctrl(sessionid, (SaHpiResourceIdT) atoi(input));
			
        	} else if (!strcmp(input, "all_cntl")) {
			list_ctrl(sessionid, 0XFF);
									
        	} else if (!strcmp(input, "ann")) {
			/*get_resourceid; */
			list_ann(sessionid, (SaHpiResourceIdT) atoi(input));
									
        	} else if (!strcmp(input, "all_ann")) {
			list_ann(sessionid, 0XFF);
									
        	} else if (!strcmp(input, "wdog")) {
			/*get_resourceid; */
			list_wdog(sessionid, (SaHpiResourceIdT) atoi(input));
									
        	} else if (!strcmp(input, "all_wdog")) {
			list_wdog(sessionid, 0XFF);
			
		} else if (!strcmp(input, "help")) {
			usage();
			
		} else if (!strcmp(input, "?")) {
			usage();
			
		} else if (!strcmp(input, "quit")) {
			valid = 1;
			thread = 0;

			if (fthread) {
				rv = pthread_join(discover_thread, &thread_done);
				if (rv)
					printf("Error joining thread\n");
				printf("Discovery %s\n", (char *)thread_done);
			}
			break;

		} else {
			printf("Type command (help for list) or \"quit\" to exit\n");
			printf("Command> ");
		}
   	}



	rv = saHpiSessionClose(sessionid);
	/* rv = saHpiFinalize();	*/
	
	exit(0);
}

/* 
 * Get user inputs
 * User requests are reflected in global flags
**/
void usage(void)
{
	printf("Available commands:\n"
		"\tall \t\t-- Dump all\n"
	       	"\trpt \t\t-- Dump all resources in domain RPTable\n"		
	       	"\tinv \t\t-- Dump inventory data for a given resource (by ID)\n"
               	"\tall_inv \t-- Dump all inventory data in a session\n"
	       	"\tsensor \t\t-- Dump sensors data for a given resource (by ID)\n"
               	"\tall_sensors \t-- Dump all sensor data in a session\n"
	       	"\tcntl \t\t-- Dump controls data for a given resource (by ID)\n"
               	"\tall_cntl \t-- Dump all controls data in a session\n"
	       	"\tann \t\t-- Dump annuntiator data for a given resource(by ID)\n"
               	"\tall_ann \t-- Dump all annuntiator data in a session\n"
	       	"\twdog \t\t-- Dump watchdog data for a given resource -- (by ID)\n"
               	"\tall_wdog \t-- Dump all watchdog data in a session\n");
}

/* 
 *
 */
SaErrorT list_rpt(SaHpiSessionIdT sessionid)
{
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaErrorT rv = SA_OK;

	rptentryid = SAHPI_FIRST_ENTRY;
	do {
		
		if (fdebug) printf("saHpiRptEntryGet\n");

		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,&rptentry);
		if (rv != SA_OK) {
			printf("Error getting RPT entry: rv = %s\n", oh_lookup_error(rv));
			return (rv);
		} else {
			oh_print_rptentry(&rptentry, 2);
		}

		rptentryid = nextrptentryid;
	} while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));
	return(rv);
}


/* 
 *
 */
SaErrorT list_all(SaHpiSessionIdT sessionid)
{
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaErrorT rv = SA_OK;

	rptentryid = SAHPI_FIRST_ENTRY;
	do {
		
		if (fdebug) printf("saHpiRptEntryGet\n");

		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,&rptentry);
		if (rv != SA_OK) {
			printf("Error getting RPT entry: rv = %s\n", oh_lookup_error(rv));
			return (rv);
		} else {
			oh_print_rptentry(&rptentry, 2);
			list_inv(sessionid, rptentry.ResourceId);
			list_sens(sessionid, rptentry.ResourceId);
		}

		rptentryid = nextrptentryid;
	} while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));
	return(rv);
}

/* 
 *
 */
SaErrorT list_inv(SaHpiSessionIdT sessionid,SaHpiResourceIdT resourceid)
{
	SaErrorT rv       = SA_OK,
	         rvRdrGet = SA_OK,
 		 rvRptGet = SA_OK, 
 		 rvInvent = SA_OK;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiEntryIdT entryid;
	SaHpiEntryIdT nextentryid;
	SaHpiRdrT rdr;
	SaHpiIdrInfoT	idrInfo;
	SaHpiIdrIdT	idrid;
	SaHpiResourceIdT l_resourceid;
	SaHpiTextBufferT working;
		
	oh_init_textbuffer(&working);																		
																
	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	do {
		
		if (fdebug) printf("saHpiRptEntryGet\n");
		rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if ((rvRptGet != SA_OK) || fdebug) 
		       	printf("RptEntryGet returns %s\n",oh_lookup_error(rvRptGet));
			if (rvRptGet == SA_OK 
                   		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
				&& ((resourceid == 0xFF) || (resourceid == rptentry.ResourceId)))
			{
				l_resourceid = rptentry.ResourceId;
				if (resourceid != 0xFF) 
					 nextrptentryid = SAHPI_LAST_ENTRY;

				/* walk the RDR list for this RPT entry */
				entryid = SAHPI_FIRST_ENTRY;			

				if (fdebug) printf("rptentry[%d] resourceid=%d\n", entryid,resourceid);

				do {
					rvRdrGet = saHpiRdrGet(sessionid,l_resourceid, entryid,&nextentryid, &rdr);
					if (fdebug) printf("saHpiRdrGet[%d] rv = %s\n",entryid,oh_lookup_error(rvRdrGet));

					if (rvRdrGet == SA_OK)
					{
						if (rdr.RdrType == SAHPI_INVENTORY_RDR)
						{
														
							idrid = rdr.RdrTypeUnion.InventoryRec.IdrId;
							rvInvent = saHpiIdrInfoGet(
										sessionid,
										l_resourceid,
										idrid,
										&idrInfo);		

							if (rvInvent !=SA_OK) {
								printf("saHpiIdrInfoGet: ResourceId %d IdrId %d, error %s\n",
									l_resourceid, idrid, oh_lookup_error(rvInvent));
							} else {
								snprintf(working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
								 	"\nIdr for ResourceId:\t%d\n", l_resourceid);
								oh_print_text(&working);
								oh_print_idrinfo(&idrInfo, 4);
								walkInventory(sessionid, l_resourceid, &idrInfo);
							}
													} 
					}
					entryid = nextentryid;
				} while ((rvRdrGet == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) ;
			}
			rptentryid = nextrptentryid;
		} while ((rvRptGet == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));

	return(rv);
}

/* 
 * This routine walks the complete inventory idr for this resource.
 * It does not look for a particular IdrAreaType or IdrFieldType.
 * Particular type tests are coverred in respecting routines.
 *
**/
SaErrorT walkInventory(	SaHpiSessionIdT sessionid,
			SaHpiResourceIdT resourceid,
			SaHpiIdrInfoT	*idrInfo)
{

	SaErrorT 	rv = SA_OK, 
			rvField = SA_OK;

	SaHpiUint32T	numAreas;
	SaHpiUint32T	countAreas = 0;
	SaHpiUint32T	countFields = 0;

	SaHpiEntryIdT	areaId;
	SaHpiEntryIdT	nextareaId;
	SaHpiIdrAreaTypeT areaType;
	SaHpiIdrAreaHeaderT  areaHeader;

	SaHpiEntryIdT	fieldId;
	SaHpiEntryIdT	nextFieldId;
	SaHpiIdrFieldTypeT fieldType;
	SaHpiIdrFieldT	      thisField;



	numAreas = idrInfo->NumAreas;
	areaType = SAHPI_IDR_AREATYPE_UNSPECIFIED;
	/* areaId = SAHPI_FIRST_ENTRY; */
	areaId = 1;

	do {
		rv = saHpiIdrAreaHeaderGet(sessionid,
					   resourceid,
					   idrInfo->IdrId,
					   areaType,
					   areaId,
					   &nextareaId,
					   &areaHeader);
		if (rv == SA_OK) {
			countAreas++;
			oh_print_idrareaheader(&areaHeader, 8);

			fieldType = SAHPI_IDR_FIELDTYPE_UNSPECIFIED;
			fieldId = SAHPI_FIRST_ENTRY;
		
			do {
				rvField = saHpiIdrFieldGet(
							sessionid,	
							resourceid,
							idrInfo->IdrId,
							areaId, 
							fieldType,
							fieldId,
							&nextFieldId,
							&thisField);
				if (rvField == SA_OK) {
					countFields++; 
					oh_print_idrfield(&thisField, 12);
				}
 
				if (fdebug) printf("saHpiIdrFieldGet  error %s\n",oh_lookup_error(rvField));
				fieldId = nextFieldId;
			} while ((rvField == SA_OK) && (fieldId != SAHPI_LAST_ENTRY));
			
			if ( countFields != areaHeader.NumFields) 
				printf("Area Header error! areaHeader.NumFields %d, countFields %d\n",
					areaHeader.NumFields, countFields);
		} else {
			printf("saHpiIdrAreaHeaderGet error %s\n",oh_lookup_error(rv));
		}
		areaId = nextareaId;
	
	} while ((rv == SA_OK) && (areaId != SAHPI_LAST_ENTRY)); 

	if ((rv == SA_OK) && (countAreas != numAreas)) 
		printf("idrInfo error! idrInfo.NumAreas = %d; countAreas = %d\n", 
				numAreas, countAreas);
 	
	return(rv);
}

/* 
 *
 */
void *sahpi_discover_thread(void *sessionid)
{
	int rv;
	struct timeval to;

	while (thread == 1) {
		rv = saHpiDiscover((SaHpiSessionIdT) sessionid);
		if (rv != SA_OK)
			printf("discovery failed\n");

		to.tv_sec = 3;  /*check everything 3 seconds */
		to.tv_usec = 0;

		select(0, 0, NULL, NULL, &to);
	}

	pthread_exit("Thread exit\n");
}

/* 
 *
 */
SaErrorT list_sens(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid)
{
	SaErrorT rv       = SA_OK,
	         rvRdrGet = SA_OK,
 		 rvRptGet = SA_OK;

	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiEntryIdT entryid;
	SaHpiEntryIdT nextentryid;
	SaHpiRdrT rdr;
	SaHpiResourceIdT l_resourceid;
	SaHpiTextBufferT working;
		
	oh_init_textbuffer(&working);																		
																
	/* walk the RPT list */
	rptentryid = SAHPI_FIRST_ENTRY;
	do {
		
		if (fdebug) printf("saHpiRptEntryGet\n");
		rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if ((rvRptGet != SA_OK) || fdebug) 
		       	printf("RptEntryGet returns %s\n",oh_lookup_error(rvRptGet));
			if (rvRptGet == SA_OK 
                   		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
				&& ((resourceid == 0xFF) || (resourceid == rptentry.ResourceId)))
			{
				l_resourceid = rptentry.ResourceId;
				if (resourceid != 0xFF) 
					 nextrptentryid = SAHPI_LAST_ENTRY;

				/* walk the RDR list for this RPT entry */
				entryid = SAHPI_FIRST_ENTRY;			

				if (fdebug) printf("rptentry[%d] resourceid=%d\n", entryid,resourceid);

				do {
					rvRdrGet = saHpiRdrGet(sessionid,l_resourceid, entryid,&nextentryid, &rdr);
					if (fdebug) printf("saHpiRdrGet[%d] rv = %s\n",entryid,oh_lookup_error(rvRdrGet));

					if (rvRdrGet == SA_OK)
					{
						if (rdr.RdrType == SAHPI_SENSOR_RDR)
						{							
							snprintf(working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
								 	"\nSensor for ResourceId:\t%d\n", l_resourceid);
							oh_print_text(&working);
							oh_print_sensorrec(&rdr.RdrTypeUnion.SensorRec);
						} 
					}
					entryid = nextentryid;
				} while ((rvRdrGet == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) ;
			}
			rptentryid = nextrptentryid;
		} while ((rvRptGet == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));

	return(rv);
}


/* 
 *
 */
SaErrorT list_ctrl(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid)
{
	printf("\n\t>>>>>>>>>>>>>>>>>> Coming soon.<<<<<<<<<<<<<<<<<<<\n");
	return(SA_OK);
}

/* 
 *
 */
SaErrorT list_wdog(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid)
{
	printf("\n\t>>>>>>>>>>>>>>>>>> Coming soon.<<<<<<<<<<<<<<<<<<<\n");
	return(SA_OK);
}

/* 
 *
 */
SaErrorT list_ann (SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid)
{
	printf("\n\t>>>>>>>>>>>>>>>>>> Coming soon.<<<<<<<<<<<<<<<<<<<\n");
	return(SA_OK);
}


 /* end hpitree.c */
