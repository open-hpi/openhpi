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
 *     Andy Cress <arcress@users.sourceforge.net>
 *     Peter D. Phan <pdphan@users.sourceforge.net>
 *     Renier Morales <renierm@users.sf.net>
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

#define NCT 25

char progver[] = "0.2 HPI-B";
char *chasstypes[NCT] = {
	"Not Defined", "Other", "Unknown", "Desktop", "Low Profile Desktop",
	"Pizza Box", "Mini Tower", "Tower", "Portable", "Laptop",
	"Notebook", "Handheld", "Docking Station", "All in One", "Subnotebook",
	"Space Saving", "Lunch Box", "Main Server", "Expansion", "SubChassis",
	"Buss Expansion Chassis", "Peripheral Chassis", "RAID Chassis", 
	"Rack-Mount Chassis", "New"
};
int fasset = 0;
int fdebug = 0;
int fxdebug = 0;
char progname[] = "hpiinv";
char *asset_tag;
char outbuff[256];

/* 
 * Function prototypes
**/
void walkInventory(	SaHpiSessionIdT sessionid,
			SaHpiResourceIdT resourceid,
			SaHpiIdrInfoT	*idrInfo);

/* 
 * Function prototypes
**/
int
main(int argc, char **argv)
{
	int asset_len=0;
	int c;
	SaErrorT rv = SA_OK,
		 rvRdrGet = SA_OK,
		 rvRptGet = SA_OK, 
		 rvInvent = SA_OK;
	SaHpiSessionIdT sessionid;
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiEntryIdT entryid;
	SaHpiEntryIdT nextentryid;
	SaHpiResourceIdT resourceid;
	SaHpiRdrT rdr;
	SaHpiIdrInfoT	idrInfo;
	SaHpiIdrIdT	idrid;

	int inv_discovered = 0;
		
	printf("%s ver %s\n",argv[0],progver);

	while ( (c = getopt( argc, argv,"a:xz?")) != EOF ) {
		switch(c) {
			case 'z': fxdebug = 1; /* fall thru to include next setting */
			case 'x': fdebug = 1; break;
			case 'a':
				fasset = 1;
				if (optarg) 
				{
					asset_tag = (char *)strdup(optarg);
					asset_len = strlen(optarg);
				}
				break;
			default:
				printf("Usage: %s [-x] [-a asset_tag]\n", progname);
				printf("   -a  Sets the asset tag\n");
				printf("   -x  Display debug messages\n");
				printf("   -z  Display extra debug messages\n");
				exit(1);
		}
	}
	
        rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID,&sessionid,NULL);
	if (rv != SA_OK) {
		printf("saHpiSessionOpen error %s\n",oh_lookup_error(rv));
		exit(-1);
	}

	do {
		rv = saHpiDiscover(sessionid);
		if (fxdebug) printf("saHpiDiscover rv = %s\n",oh_lookup_error(rv));
		if (rv != SA_OK) {
			printf("saHpiDiscover error %s\n",oh_lookup_error(rv));
			// continue anyway exit(-1);
		}

		/* walk the RPT list */
		rptentryid = SAHPI_FIRST_ENTRY;
		do {
			rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
			if (rvRptGet != SA_OK) printf("RptEntryGet error %s\n",oh_lookup_error(rvRptGet));

			if (rvRptGet == SA_OK 
                    		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
                    		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA))
			{
				/* walk the RDR list for this RPT entry */
				entryid = SAHPI_FIRST_ENTRY;			
				resourceid = rptentry.ResourceId;

				if (fdebug) printf("rptentry[%d] resourceid=%d\n", entryid,resourceid);

				 do {
					rvRdrGet = saHpiRdrGet(sessionid,resourceid, entryid,&nextentryid, &rdr);
					if (fxdebug) printf("saHpiRdrGet[%d] rv = %s\n",entryid,oh_lookup_error(rvRdrGet));

					if (rvRdrGet == SA_OK)
					{
						if (rdr.RdrType == SAHPI_INVENTORY_RDR)
						{
                                                	inv_discovered = 1;
														
							idrid = rdr.RdrTypeUnion.InventoryRec.IdrId;
							rvInvent = saHpiIdrInfoGet(
										sessionid,
										resourceid,
										idrid,
										&idrInfo);		

							if (rvInvent !=SA_OK) {
								printf("saHpiIdrInfoGet IdrId %d,  error %s\n", idrid, oh_lookup_error(rvInvent));
							} else {
								printf("\nIdr for %s, ResourceId: %d\n",
									 rptentry.ResourceTag.Data,resourceid);
								oh_print_idrinfo(&idrInfo, 4);
								walkInventory(sessionid, resourceid, &idrInfo);
							}
													} 
					}
					entryid = nextentryid;
				} while ((rvRdrGet == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) ;
			}
			rptentryid = nextrptentryid;
		} while ((rvRptGet == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));


	/* 
	   Because INVENTORY RDR will be added after some time, 
	   we need to monitor RptInfo here; IMPI plugin.
	
	   Try again if none was found this pass 
	*/        
	} while (!inv_discovered);
	rv = saHpiSessionClose(sessionid);
	/* rv = saHpiFinalize();	*/
	
	exit(0);
}


/* 
 * This routine walks the complete inventory idr for this resource.
 * It does not look for a particular IdrAreaType or IdrFieldType.
 * Particular type tests are coverred in respecting routines.
 *
**/
void walkInventory(	SaHpiSessionIdT sessionid,
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
 	
}


 /* end hpiinv.c */
