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
 *     Tariq Shureih <tariq.shureih@intel.com>
 *
 * Log: 
 *     Copied from hpiinv.c and modified for general use
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "SaHpi.h"
#include <pthread.h>

void *sahpi_discover_thread(void *sessionid);

static void usage(void);
static void list_rpt(SaHpiSessionIdT sessionid);
static void list_inv(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
int set_resource_tag(SaHpiSessionIdT sessionid);

#define NCT 25

int thread = 1;
char progver[] = "0.a";
char *chasstypes[NCT] = {
	"Not Defined", "Other", "Unknown", "Desktop", "Low Profile Desktop",
	"Pizza Box", "Mini Tower", "Tower", "Portable", "Laptop",
	"Notebook", "Handheld", "Docking Station", "All in One", "Subnotebook",
	"Space Saving", "Lunch Box", "Main Server", "Expansion", "SubChassis",
	"Buss Expansion Chassis", "Peripheral Chassis", "RAID Chassis",
	"Rack-Mount Chassis", "New"
};
int fasset = 0;
int debug = 0;
int i, j, k = 0;
SaHpiUint32T actualsize;
char progname[] = "hpi_cmd";
char hpi_cmd_ver[] = "0.2";
char *asset_tag;
char outbuff[256];
SaHpiInventoryDataT *inv;
SaHpiInventChassisTypeT chasstype;
SaHpiInventGeneralDataT *dataptr;
SaHpiTextBufferT *strptr;

static void usage(void)
{
	printf("%s version %s\n", (char *)progname, hpi_cmd_ver);
	printf("Available commands:\n"
	       "\tlist_rpt -- Dump all resources in domain RPTabe\n"
	       "\tset_tag -- saHpiResourceTagSet -- set resource tag for given resource (by ID)\n"
	       "\tlist_inv -- saHpiEntityInventoryDataRead -- dump inventory data for a given resource (by ID)\n"
               "\tlist_all_inv -- SaHpiEntityInventoryDataRead -- dump all inventory data in a session\n");
}

static void fixstr(SaHpiTextBufferT * strptr)
{
	size_t datalen;

	memset(outbuff, 0, 256);
	if (!strptr)
		return;

	datalen = strptr->DataLength;
	if (datalen > 0) {
		strncpy((char *)outbuff, (char *)strptr->Data, datalen);
		outbuff[datalen] = 0;
	}

}

static void prtchassinfo(void)
{
	chasstype =
	    (SaHpiInventChassisTypeT) inv->DataRecords[i]->RecordData.
	    ChassisInfo.Type;
	for (k = 0; k < NCT; k++) {
		if ((unsigned int)k == chasstype)
			printf("\tChassis Type        : %s\n", chasstypes[k]);
	}

	dataptr =
	    (SaHpiInventGeneralDataT *) & inv->DataRecords[i]->RecordData.
	    ChassisInfo.GeneralData;
	strptr = dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Manufacturer: %s\n", outbuff);

	strptr = dataptr->ProductName;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Name        : %s\n", outbuff);

	strptr = dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Version     : %s\n", outbuff);

	strptr = dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Model Number: %s\n", outbuff);

	strptr = dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Serial #    : %s\n", outbuff);

	strptr = dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Part Number : %s\n", outbuff);

	strptr = dataptr->FileId;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis FRU File ID : %s\n", outbuff);

	strptr = dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tChassis Asset Tag   : %s\n", outbuff);
	if (dataptr->CustomField[0] != 0) {
		if (dataptr->CustomField[0]->DataLength != 0)
			strncpy((char *)outbuff,
				(char *)dataptr->CustomField[0]->Data,
				dataptr->CustomField[0]->DataLength);
		outbuff[dataptr->CustomField[0]->DataLength] = 0;
		printf("\tChassis OEM Field   : %s\n", outbuff);
	}
}

static void prtprodtinfo(void)
{
	int j;
	dataptr =
	    (SaHpiInventGeneralDataT *) & inv->DataRecords[i]->RecordData.ProductInfo;
	strptr = dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Manufacturer: %s\n", outbuff);

	strptr = dataptr->ProductName;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Name        : %s\n", outbuff);

	strptr = dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Version     : %s\n", outbuff);

	strptr = dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Model Number: %s\n", outbuff);

	strptr = dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Serial #    : %s\n", outbuff);

	strptr = dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Part Number : %s\n", outbuff);

	strptr = dataptr->FileId;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct FRU File ID : %s\n", outbuff);

	strptr = dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tProduct Asset Tag   : %s\n", outbuff);

	for (j = 0; j < 10; j++) {
          int ii;
          if (dataptr->CustomField[j] != NULL) {
                if ((dataptr->CustomField[j]->DataType == 0) && (dataptr->CustomField[j]->DataLength == 16)) {	/*binary GUID */
                        printf("IPMI SystemGUID     : ");
                        for (ii = 0; ii < dataptr->CustomField[j]->DataLength;ii++)
                                printf("%02x",dataptr->CustomField[j]->Data[ii]);
				printf("\n");
                } else {	/* other text field */
                        dataptr->CustomField[j]->Data[dataptr->CustomField[j]->DataLength] = 0;
                        printf("\tProduct OEM Field   : %s\n",dataptr->CustomField[j]->Data);
                }
          } else		/* NULL pointer */
                break;
        } /*end for */
}

static void prtboardinfo(void)
{
	dataptr =
	    (SaHpiInventGeneralDataT *) & inv->DataRecords[i]->RecordData.
	    BoardInfo;
	strptr = dataptr->Manufacturer;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Manufacturer  : %s\n", outbuff);

	strptr = dataptr->ProductName;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Product Name  : %s\n", outbuff);

	strptr = dataptr->ModelNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Model Number  : %s\n", outbuff);

	strptr = dataptr->PartNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Part Number   : %s\n", outbuff);

	strptr = dataptr->ProductVersion;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Version       : %s\n", outbuff);

	strptr = dataptr->SerialNumber;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Serial #      : %s\n", outbuff);

	strptr = dataptr->FileId;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard FRU File ID   : %s\n", outbuff);

	strptr = dataptr->AssetTag;
	fixstr((SaHpiTextBufferT *) strptr);
	printf("\tBoard Asset Tag     : %s\n", outbuff);

	for (j = 0; j < 10 && dataptr->CustomField[j]; j++) {
                if (dataptr->CustomField[j]->DataLength != 0) {
                        strncpy((char *)outbuff,(char *)dataptr->CustomField[j]->Data,
                                dataptr->CustomField[j]->DataLength);
                        
                        outbuff[dataptr->CustomField[j]->DataLength] = 0;
			printf("\tBoard OEM Field     : %s\n", outbuff);
                }
        }
}

static
void list_rpt(SaHpiSessionIdT sessionid)
{
        SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
        
        int rv;

	rptentryid = SAHPI_FIRST_ENTRY;

	rv = SA_OK;

	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {
                
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,
				      &rptentry);
		if (rv != SA_OK) {
			printf("Error getting RPT entry: rv = %d\n", rv);
		}

		printf("Resource Id: %d\n", rptentry.ResourceId);
		printf("Resource Tag: %s\n", (char *)rptentry.ResourceTag.Data);

		rptentryid = nextrptentryid;
	}
}

static
void list_inv(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id)
{
   SaHpiRptEntryT rptentry;
   SaHpiEntryIdT entryid;
   SaHpiEntryIdT nextentryid;
   SaHpiResourceIdT resourceid;
   SaHpiRdrT rdr;
   SaHpiEirIdT eirid;
   SaErrorT rv;
   int prodrecindx = 0;
  	
   rv = SA_OK;

   rv = saHpiRptEntryGetByResourceId(sessionid, res_id, &rptentry);

	if (rv == SA_OK && (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
	    		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {

	  	/* walk the RDR list for this RPT entry */
	   	entryid = SAHPI_FIRST_ENTRY;

		resourceid = rptentry.ResourceId;

		printf("Resource Tag: %s\n", rptentry.ResourceTag.Data);

		while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) {
                        
		  	rv = saHpiRdrGet(sessionid, resourceid, entryid,
					 	&nextentryid, &rdr);

			if (rv == SA_OK) {
			  	if (rdr.RdrType == SAHPI_INVENTORY_RDR) {
				  	unsigned int invsize = 0;

					eirid = rdr.RdrTypeUnion.InventoryRec.EirId;
					rdr.IdString.Data[rdr.IdString.DataLength] = 0;

					actualsize = 0;
					
					rv = saHpiEntityInventoryDataRead(sessionid, resourceid, eirid, 0, NULL, &actualsize);

					invsize = actualsize;

					inv = (SaHpiInventoryDataT *)malloc(invsize);
					memset(inv, 0, invsize);

					rv = saHpiEntityInventoryDataRead(sessionid, resourceid, eirid, invsize, inv, &actualsize);

					if (rv == SA_OK) {
					  	/* Walk thru the list of inventory data */
					  	if (inv->Validity == SAHPI_INVENT_DATA_VALID) {
                                                        for (i = 0; inv->DataRecords[i] != NULL; i++) {
                                                                switch(inv->DataRecords[i]->RecordType) {
                                                                        case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
                                                                                if (debug)
                                                                                        printf("Internal Use\n");
                                                                                break;
                                                                        case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
                                                                                if (debug)
                                                                                        printf("Product Info\n");
                                                                                prodrecindx = 0;
                                                                                prtprodtinfo();
                                                                                break;
                                                                        case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
                                                                                if (debug)
                                                                                        printf("Chassis Info\n");
                                                                                prtchassinfo();
                                                                                break; 
                                                                        case SAHPI_INVENT_RECTYPE_BOARD_INFO:
                                                                                if (debug)
                                                                                        printf("Board Info\n");
                                                                                prtboardinfo();
                                                                                break;
                                                                        case SAHPI_INVENT_RECTYPE_OEM:
										if (debug)
                                                                                        printf("OEM Record\n");
                                                                                break;
                                                                        default:
                                                                                printf(" Invalid Invent Rec Type =%x\n",
                                                                                       inv->DataRecords[i]->RecordType);
                                                                                break;
                                                                }
                                                        }
                                                }
					} else {
					  	printf(" InventoryDataRead returns HPI Error: rv=%d\n", rv);
                                        }

                                        free(inv);
                                }
                        }
                        
			entryid = nextentryid;
                }
        }
}


			      
			      

int main(int argc, char **argv)
{
   int c;
   SaErrorT rv;
   SaErrorT rvx;
   SaHpiVersionT hpiVer;
   SaHpiSessionIdT sessionid;
   SaHpiRptInfoT rptinfo;
   SaHpiRptEntryT rptentry;
   SaHpiEntryIdT rptentryid;
   SaHpiEntryIdT nextrptentryid;
   SaHpiResourceIdT resourceid;

   char input[255], *p;
   pthread_t discover_thread;
   void *thread_done;
   int valid = 0;

   printf("%s ver %s\n", argv[0], progver);

   while ((c = getopt(argc, argv, "x?")) != EOF) {
        switch (c) {
                case 'x':
                        debug = 1;
                        break;
                default:
                        printf("Usage: %s [-x]\n", progname);
                        printf("   -x  Display debug messages\n");
                        exit(1);
        }
   }

   rv = saHpiInitialize(&hpiVer);
   
   if (rv != SA_OK) {
     	printf("saHpiInitialize error %d\n", rv);
	exit(-1);
   }

   rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &sessionid, NULL);
   if (rv != SA_OK) {
     	printf("saHpiSessionOpen error %d\n", rv);
	exit(-1);
   }

   rv = saHpiResourcesDiscover(sessionid);
   if (debug)
     	printf("saHpiResourcesDiscover rv = %d\n", rv);

restart:
   rv = saHpiRptInfoGet(sessionid, &rptinfo);
   if (debug)
     	printf("saHpiRptInfoGet rv = %d\n", rv);

   printf("RptInfo: UpdateCount = %d, UpdateTime = %lx\n",
	  rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
   
   /* walk the RPT list */

   rptentryid = SAHPI_FIRST_ENTRY;
   rvx = SA_OK;

   while ((rvx == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {
        rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,
                              &rptentry);
	if (rvx != SA_OK)
	  	printf("RptEntryGet: rv = %d\n", rv);

	if (rvx == SA_OK && (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR)
	    		&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)) {

                resourceid = rptentry.ResourceId;

                list_inv(sessionid, resourceid);

        }

        rptentryid = nextrptentryid;
   }

   printf("Initial discovery done\n");
   printf("\tEnter a command or \"help\" for list of commands\n");
   printf("Command> ");

   rv = pthread_create(&discover_thread, NULL, sahpi_discover_thread,
		      (void *)sessionid);

   if (rv)
     printf("Error creating event thread\n");

   while (!valid) {
        
        fflush(stdout);
        
        p = fgets(input, 255, stdin);
        
        if ((p = strchr(input, '\n')) != NULL)
            *p = '\0';
        
        if (!strcmp(input, "list_all_inv")) {
            goto restart;
        }
     
        if (!strcmp(input, "list_inv")) {
            list_rpt(sessionid);
            printf("Enter resource id:");
            
            p = fgets(input, 255, stdin);
            if ((p = strchr(input, '\n')) != NULL)
                *p = '\0';

            list_inv(sessionid, (SaHpiResourceIdT) atoi(input));
        }

        if (!strcmp(input, "set_tag")) {
            rv = set_resource_tag(sessionid);
            if (rv)
                printf("Error setting tag\n");
        }

        if (!strcmp(input, "list_rpt")) {
            list_rpt(sessionid);
        }

        if (!strcmp(input, "help")) {
            usage();
        }

        if (!strcmp(input, "quit")) {
            valid = 1;
            thread = 0;

            rv = pthread_join(discover_thread, &thread_done);
            if (rv)
                printf("Error joining thread\n");

            printf("Discovery %s\n", (char *)thread_done);
            break;

        } else {
            printf("Type command (help for list) or \"quit\" to exit\n");
            printf("Command> ");
        }
   }

   rv = saHpiSessionClose(sessionid);
   rv = saHpiFinalize();

   exit(0);
}

void *sahpi_discover_thread(void *sessionid)
{
	int rv;
	struct timeval to;

	while (thread == 1) {
		rv = saHpiResourcesDiscover((SaHpiSessionIdT) sessionid);
		if (rv != SA_OK)
			printf("discovery failed\n");

		to.tv_sec = 3;  /*check everything 3 seconds */
		to.tv_usec = 0;

		select(0, 0, NULL, NULL, &to);
	}

	pthread_exit("Thread exit\n");
}

int set_resource_tag(SaHpiSessionIdT sessionid)
{
	SaHpiRptEntryT rptentry;
	SaHpiEntryIdT rptentryid;
	SaHpiEntryIdT nextrptentryid;
	SaHpiTextBufferT new_resource_tag;
	SaHpiTextBufferT old_resource_tag;

	int rv;
	SaHpiResourceIdT resource_id;
	char id_buf[SAHPI_MAX_TEXT_BUFFER_LENGTH], *res_id;
	char tag_buf[SAHPI_MAX_TEXT_BUFFER_LENGTH], *res_tag;

	rptentryid = SAHPI_FIRST_ENTRY;
	rv = SA_OK;

	while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY)) {
		rv = saHpiRptEntryGet(sessionid, rptentryid, &nextrptentryid,
				      &rptentry);
		if (rv != SA_OK) {
			printf("Error getting RPT entry: rv = %d\n", rv);
			return (-1);
		}
		printf("Resource Id: %d\n", rptentry.ResourceId);
		printf("Resource Tag: %s\n", (char *)rptentry.ResourceTag.Data);

		rptentryid = nextrptentryid;
	}

	res_id = NULL;
	res_tag = NULL;
	printf("\nResource Id to change: ");
	res_id = fgets(id_buf, 255, stdin);
	if ((res_id = strchr(id_buf, '\n')) != NULL)
		*res_id = '\0';

	resource_id = (SaHpiResourceIdT) atoi(id_buf);

	printf("New Resource Tag: ");
	res_tag = fgets(tag_buf, SAHPI_MAX_TEXT_BUFFER_LENGTH, stdin);
	if ((res_tag = strchr(tag_buf, '\n')) != NULL)
		*res_tag = '\0';

	rv = saHpiRptEntryGetByResourceId(sessionid, resource_id, &rptentry);
	if (rv != SA_OK) {
		printf("Erro getting RPT entry for resource: %d\n",
		       resource_id);
		return (-1);
	}

        if (rptentry.ResourceEntity.Entry[0].EntityType == SAHPI_ENT_SYS_MGMNT_MODULE) {
                printf("On some systems, changing the Tag is no allowed\n");
                printf("for System Management Modules\n");
                printf("Failed to change tag for resource: %d\n", resource_id);
                return(1);
        }
          

	old_resource_tag = rptentry.ResourceTag;

	new_resource_tag.DataType = SAHPI_TL_TYPE_ASCII6;
	new_resource_tag.Language = SAHPI_LANG_ENGLISH;
	new_resource_tag.DataLength = strlen(tag_buf);
	memcpy(new_resource_tag.Data, tag_buf, strlen(tag_buf) + 1);

	rv = saHpiResourceTagSet(sessionid, resource_id, &new_resource_tag);
	if (rv != SA_OK) {
		printf("Erro setting custom tag for resource: %d\n",
		       resource_id);
		return -1;
	}

	memset(&rptentry, 0, 0);
	rv = saHpiRptEntryGetByResourceId(sessionid, resource_id, &rptentry);
	if (rv != SA_OK) {
		printf("Erro getting RPT entry for resource: %d\n",
		       resource_id);
		return (-1);
	} else {
		printf("Old resource Tag was: %s, new Tag: %s\n",
		       (char *)old_resource_tag.Data,
		       (char *)rptentry.ResourceTag.Data);
	}

	return 0;
}

 /* end hpi_cmd.c */
