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
SaErrorT list_all(SaHpiSessionIdT sessionid);
SaErrorT list_rpt(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_rdr(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_inv(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_sens(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_ctrl(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_wdog(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
SaErrorT list_ann (SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);


SaErrorT walkInventory(SaHpiSessionIdT sessionid,
		   SaHpiResourceIdT resourceid,
		   SaHpiIdrInfoT *idrInfo);

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
	while ( (c = getopt( argc, argv,"adrsicn:x?")) != EOF ) {
		switch(c) {
			case 'a': f_listall = 1; break;
			case 'c': f_ctrl    = 1; break;
			case 'd': f_rdr    = 1; break;
			case 'i': f_inv     = 1; break;
			case 'r': f_rpt     = 1; break;
			case 's': f_sensor  = 1; break; 
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
				printf("\t           -r     Display only rpts\n");
				printf("\t           -s     Display only sensors\n");
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
	if(f_listall) 
		list_all(sessionid);
	else {	
		if(f_rpt) list_rpt(sessionid,resourceid);
		if(f_rdr) list_rdr(sessionid,resourceid);
		if(f_sensor) list_sens(sessionid,resourceid);
		if(f_inv) list_inv(sessionid, resourceid); 
		if(f_ctrl) list_ctrl(sessionid, resourceid);
	}	

	rv = saHpiSessionClose(sessionid);
	
	exit(0);
}

/*
 *
 */
SaErrorT list_rpt(SaHpiSessionIdT sessionid,SaHpiResourceIdT resourceid)
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
			if (resourceid == 255) 
				oh_print_rptentry(&rptentry, 2);
			else {
				if (resourceid == rptentry.ResourceId) {
					oh_print_rptentry(&rptentry, 2);
					nextrptentryid = SAHPI_LAST_ENTRY;
				}	 
			}
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
								 	"\nIdr for %s, ResourceId: %d\n",
									 rptentry.ResourceTag.Data,l_resourceid);
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
								 	"\nSensor for %s, ResourceId: %d\n",
										rptentry.ResourceTag.Data,l_resourceid);
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
SaErrorT list_rdr(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid)
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
						snprintf(working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
							 "\nRdr for %s, ResourceId: %d\n",
							 rptentry.ResourceTag.Data,l_resourceid);
						oh_print_text(&working);

						oh_print_rdr(&rdr, 2);
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
