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

/* 
 * Function prototypes
 */
static SaErrorT list_resources(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
static SaErrorT list_rpt(SaHpiRptEntryT *rptptr,SaHpiResourceIdT resourceid);
static SaErrorT list_rdr(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT list_inv(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT list_sens(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT list_ctrl(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT list_wdog(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT list_ann (SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT walkInventory(SaHpiSessionIdT sessionid,
		   SaHpiResourceIdT resourceid,
		   SaHpiIdrInfoT *idrInfo);

static 
SaErrorT getcontrolstate(SaHpiSessionIdT sessionid,
			SaHpiResourceIdT l_resourceid,
			SaHpiCtrlNumT num);


#define all_resources 255

/* 
 * Globals for this driver
 */
char progver[] = "0.2 HPI-B";
char progname[] = "hpitree";
int fdebug = 0;
int f_listall = 0;
int f_rpt     = 0;
int f_sensor  = 0;
int f_inv     = 0;
int f_ctrl    = 0;
int f_rdr     = 0;
int f_wdog    = 0;
int f_ann     = 0;
int f_overview = 0;
/* 
 * Main                
 */
int
main(int argc, char **argv)
{
	SaErrorT 	rv = SA_OK;
	
	SaHpiVersionT	hpiVer;
	SaHpiSessionIdT sessionid;
	SaHpiResourceIdT resourceid = all_resources;

	int c;
	    
	printf("\n\n%s ver %s\n",argv[0],progver);
	while ( (c = getopt( argc, argv,"adrsoiwcn:x?")) != EOF ) {
		switch(c) {
			case 'a': f_listall = 1; break;
			case 'c': f_ctrl    = 1; break;
			case 'd': f_rdr     = 1; break;
			case 'i': f_inv     = 1; break;
			case 'r': f_rpt     = 1; break;
			case 's': f_sensor  = 1; break; 
			case 'w': f_wdog    = 1; break;
			case 'o': f_overview = 1; break; 			 
			case 'n':
				if (optarg)
					resourceid = atoi(optarg);
				else 
					resourceid = all_resources;
				break;
			case 'x': fdebug = 1; break;
			default:
				printf("\n\tUsage: %s [-option]\n\n", progname);
				printf("\t      (No Option) Display all rpts and rdrs\n");
				printf("\t           -a     Display all rpts and rdrs\n");
				printf("\t           -c     Display only controls\n");
				printf("\t           -d     Display rdr records\n");
				printf("\t           -i     Display only inventories\n");
				printf("\t           -o     Display system overview: rpt & rdr headers\n");				
				printf("\t           -r     Display only rpts\n");
				printf("\t           -s     Display only sensors\n");
				printf("\t           -w     Display only watchdog\n");				
				printf("\t           -n     Select particular resource id to display\n");
				printf("\t                  (Used with [-cdirs] options)\n");
				printf("\t           -x     Display debug messages\n");
				printf("\n\n\n\n");
				exit(1);
		}
	}
 
	if (argc == 1) f_listall = 1;

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
	 * Resource discovery
	 */
	if (fdebug) printf("saHpiDiscover\n");
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
		printf("saHpiDiscover returns %s\n",oh_lookup_error(rv));
		exit(-1);
	}

	printf("Discovery done\n");
	list_resources(sessionid, resourceid);

	rv = saHpiSessionClose(sessionid);
	
	exit(0);
}


/* 
 *
 */
static 
SaErrorT list_resources(SaHpiSessionIdT sessionid,SaHpiResourceIdT resourceid)
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
		
		rv = list_rpt(&rptentry, resourceid);
						
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
					if (f_listall || f_rdr || f_overview)
							list_rdr(sessionid, &rptentry, &rdr, l_resourceid);
					if (f_listall || f_inv)
							list_inv(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_listall || f_sensor)
							list_sens(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_listall || f_ctrl)
							list_ctrl(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_listall || f_wdog)
							list_wdog(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_listall || f_ann)
							list_ann(sessionid, &rptentry, &rdr, l_resourceid); 
												
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
static 
SaErrorT list_rpt(SaHpiRptEntryT *rptptr,SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		

	if ((resourceid == all_resources) ||
		(resourceid == rptptr->ResourceId)) {

		/* Always print resource header */
		entitypath2string(&rptptr->ResourceEntity, working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
		printf("\n+%s\n",working.Data);
		
		if (!f_listall && !f_rpt) 
		printf(" Desc: %s, ResourceId %d\n\n",rptptr->ResourceTag.Data, rptptr->ResourceId);


		/* Print details when asked */
		if (f_listall || f_rpt) oh_print_rptentry(rptptr, 2);
	}
	return(rv);
}



/* 
 *
 */
static 
SaErrorT list_inv(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
 	SaErrorT  rvInvent = SA_OK;
	SaHpiIdrInfoT	idrInfo;
	SaHpiIdrIdT	idrid;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		


	if (rdrptr->RdrType == SAHPI_INVENTORY_RDR) {
		idrid = rdrptr->RdrTypeUnion.InventoryRec.IdrId;
		rvInvent = saHpiIdrInfoGet(
					sessionid,
					l_resourceid,
					idrid,
					&idrInfo);		

		if (rvInvent !=SA_OK) {
			printf("saHpiIdrInfoGet: ResourceId %d IdrId %d, error %s\n",
					l_resourceid, idrid, oh_lookup_error(rvInvent));
		} else {
			oh_print_idrinfo(&idrInfo, 4);
			walkInventory(sessionid, l_resourceid, &idrInfo);
		}
	}
	return(rvInvent);

}
/* 
 * This routine walks the complete inventory idr for this resource.
 * It does not look for a particular IdrAreaType or IdrFieldType.
 * Particular type tests are coverred in respecting routines.
 *
**/
static 
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
	areaId = SAHPI_FIRST_ENTRY;

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
							areaHeader.AreaId, 
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
static 
SaErrorT list_sens(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv  = SA_OK;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		

	if (rdrptr->RdrType == SAHPI_SENSOR_RDR) {							
		rv = oh_print_sensorrec(&rdrptr->RdrTypeUnion.SensorRec, 4);	
		
	} 

	return(rv);
}

/* 
 *
 */
static 
SaErrorT list_rdr(SaHpiSessionIdT sessionid, 
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		

	if (!f_overview) {
		rv = oh_print_rdr(rdrptr, 4);
	} else {
		snprintf(working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
			"    Found %s, RecordId %d",
			oh_lookup_rdrtype(rdrptr->RdrType), rdrptr->RecordId);
		rv = oh_print_text(&working);
	}
	return(rv);
}

/* 
 *
 */
static 
SaErrorT list_ctrl(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		
																
	if (rdrptr->RdrType == SAHPI_CTRL_RDR){
		rv = oh_print_ctrlrec(&rdrptr->RdrTypeUnion.CtrlRec, 4);
		
		rv = getcontrolstate(sessionid, l_resourceid,
				     rdrptr->RdrTypeUnion.CtrlRec.Num);
	} 
	return(rv);
}


/* 
 *
 */
static 
SaErrorT getcontrolstate(SaHpiSessionIdT sessionid,
			SaHpiResourceIdT l_resourceid,
			SaHpiCtrlNumT num)
{
	SaErrorT rv       = SA_OK;
	SaHpiCtrlModeT 	mode;
	SaHpiCtrlStateT state;
	
	rv = saHpiControlGet(sessionid, l_resourceid, num, &mode, &state);
				
	if (rv == SA_OK) 
		oh_print_ctrlstate(&state, 4); 		
	
	return(rv);
}

/* 
 *
 */
static  
SaErrorT list_wdog(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	SaHpiTextBufferT working;		
	oh_init_textbuffer(&working);																		
																
	if (rdrptr->RdrType == SAHPI_WATCHDOG_RDR) {
		rv = oh_print_watchdogrec(&rdrptr->RdrTypeUnion.WatchdogRec, 4);
	} 
	return(rv);
}

/* 
 *
 */
static  
SaErrorT list_ann (SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)

{
	/* Wave */
	return(SA_OK);
}


 /* end hpitree.c */
