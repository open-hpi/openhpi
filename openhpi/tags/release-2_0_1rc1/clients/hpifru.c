/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Bill Barkley
 *     Andy Cress <arcress@users.sourceforge.net>
 * Changes:
 * 02/19/04 ARCress - generalized BMC tag parsing, created IsTagBmc()
 * 05/05/04 ARCress - added OpenHPI Mgt Ctlr tag to IsTagBmc() 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <SaHpi.h>

#define NCT 25

char progver[] = "1.1";
char progname[] = "hpifru";
char bmctag[]  = "Basbrd Mgmt Ctlr";       /* see also IsTagBmc() */
char bmctag2[] = "Management Controller";  /* see also IsTagBmc() */
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
int i,j,k = 0;
SaHpiUint32T buffersize;
SaHpiUint32T actualsize;
char *asset_tag;
char inbuff[1024];
char outbuff[256];
SaHpiInventoryDataT *inv;
SaHpiInventChassisTypeT chasstype;
SaHpiInventGeneralDataT *dataptr;
SaHpiTextBufferT *strptr;

/*
 * findmatch
 * returns offset of the match if found, or -1 if not found.
 */
static int
findmatch(char *buffer, int sbuf, char *pattern, int spattern, char figncase)
{
    int c, j, imatch;
    j = 0;
    imatch = 0;
    for (j = 0; j < sbuf; j++) {
        if (sbuf - j < spattern && imatch == 0) return (-1);
        c = buffer[j];
        if (c == pattern[imatch]) {
            imatch++;
            if (imatch == spattern) return (++j - imatch);
        } else if (pattern[imatch] == '?') {  /*wildcard char*/
            imatch++;
            if (imatch == spattern) return (++j - imatch);
        } else if (figncase == 1) {
            if ((c & 0x5f) == (pattern[imatch] & 0x5f)) {
                imatch++;
                if (imatch == spattern) return (++j - imatch);
            } else
                imatch = 0;
        } else
            imatch = 0;
    }
    return (-1);
}                               /*end findmatch */

static int 
IsTagBmc(char *dstr, int dlen)
{
   int ret = 0;
   if (findmatch(dstr,dlen,bmctag2,sizeof(bmctag2),1) >= 0)
        ret = 1;    /* BMC tag for OpenHPI with ipmi plugin */
   else if (strncmp(dstr, bmctag, dlen) == 0)  /* Sahalee */
	ret = 1;
   else if (findmatch(dstr,dlen,"BMC",3,1) >= 0) /* mBMC or other */
	ret = 1;
   return(ret);
}

static void
fixstr(SaHpiTextBufferT *strptr)
{ 
	size_t datalen;
	if ((datalen=strptr->DataLength) != 0)
		strncpy ((char *)outbuff, (char *)strptr->Data, datalen);
	outbuff[datalen] = 0;
}

static void
prtchassinfo(void)
{
  chasstype = (SaHpiInventChassisTypeT)inv->DataRecords[i]->RecordData.ChassisInfo.Type;
  for (k=0; k<NCT; k++) {
	  if (k == chasstype)
		  printf( "Chassis Type        : %s\n", chasstypes[k]);
  }	  

  dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.ChassisInfo.GeneralData;
  strptr=dataptr->Manufacturer;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Manufacturer: %s\n", outbuff);

  strptr=dataptr->ProductName;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Name        : %s\n", outbuff);

  strptr=dataptr->ProductVersion;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Version     : %s\n", outbuff);

  strptr=dataptr->ModelNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Model Number: %s\n", outbuff);

  strptr=dataptr->SerialNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Serial #    : %s\n", outbuff);

  strptr=dataptr->PartNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Part Number : %s\n", outbuff);

  strptr=dataptr->FileId;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis FRU File ID : %s\n", outbuff);

  strptr=dataptr->AssetTag;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Chassis Asset Tag   : %s\n", outbuff);
  if (dataptr->CustomField[0] != 0)
  {
    if (dataptr->CustomField[0]->DataLength != 0)
      strncpy ((char *)outbuff, (char *)dataptr->CustomField[0]->Data,
              dataptr->CustomField[0]->DataLength);
    outbuff[dataptr->CustomField[0]->DataLength] = 0;
    printf( "Chassis OEM Field   : %s\n", outbuff);
  }
}

static void
prtprodtinfo(void)
{
  int j;
  dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.ProductInfo;
  strptr=dataptr->Manufacturer;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Manufacturer: %s\n", outbuff);

  strptr=dataptr->ProductName;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Name        : %s\n", outbuff);

  strptr=dataptr->ProductVersion;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Version     : %s\n", outbuff);

  strptr=dataptr->ModelNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Model Number: %s\n", outbuff);

  strptr=dataptr->SerialNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Serial #    : %s\n", outbuff);

  strptr=dataptr->PartNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Part Number : %s\n", outbuff);

  strptr=dataptr->FileId;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product FRU File ID : %s\n", outbuff);

  strptr=dataptr->AssetTag;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Product Asset Tag   : %s\n", outbuff);

  for (j = 0; j < 10; j++) {
     int ii;
     if (dataptr->CustomField[j] != NULL) {
      if ((dataptr->CustomField[j]->DataType == 0) &&
          (dataptr->CustomField[j]->DataLength == 16)) { /*binary GUID*/
         printf( "IPMI SystemGUID     : ");
         for (ii=0; ii< dataptr->CustomField[j]->DataLength; ii++)
              printf("%02x", dataptr->CustomField[j]->Data[ii]);
         printf("\n");
      } else {  /* other text field */
         dataptr->CustomField[j]->Data[
              dataptr->CustomField[j]->DataLength] = 0;
         printf( "Product OEM Field   : %s\n",
              dataptr->CustomField[j]->Data);
      }
     } else /* NULL pointer */
      break;
  } /*end for*/
}

static void
prtboardinfo(void)
{
  dataptr = (SaHpiInventGeneralDataT *)&inv->DataRecords[i]->RecordData.BoardInfo;
  strptr=dataptr->Manufacturer;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Manufacturer  : %s\n", outbuff);

  strptr=dataptr->ProductName;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Product Name  : %s\n", outbuff);

  strptr=dataptr->ModelNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Model Number  : %s\n", outbuff);

  strptr=dataptr->PartNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Part Number   : %s\n", outbuff);

  strptr=dataptr->ProductVersion;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Version       : %s\n", outbuff);

  strptr=dataptr->SerialNumber;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Serial #      : %s\n", outbuff);

  strptr=dataptr->FileId;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board FRU File ID   : %s\n", outbuff);

  strptr=dataptr->AssetTag;
  fixstr((SaHpiTextBufferT *)strptr);
  printf( "Board Asset Tag     : %s\n", outbuff);

  if (dataptr->CustomField[0] != 0)
  {
    if (dataptr->CustomField[0]->DataLength != 0)
      strncpy ((char *)outbuff, (char *)dataptr->CustomField[0]->Data,
              dataptr->CustomField[0]->DataLength);
    outbuff[dataptr->CustomField[0]->DataLength] = 0;
    printf( "Board OEM Field     : %s\n", outbuff);
  }
}

int
main(int argc, char **argv)
{
  int prodrecindx=0;
  int asset_len=0;
  int c;
  SaErrorT rv;
  SaHpiVersionT hpiVer;
  SaHpiSessionIdT sessionid;
  SaHpiRptInfoT rptinfo;
  SaHpiRptEntryT rptentry;
  SaHpiEntryIdT rptentryid;
  SaHpiEntryIdT nextrptentryid;
  SaHpiEntryIdT entryid;
  SaHpiEntryIdT nextentryid;
  SaHpiResourceIdT resourceid;
  SaHpiRdrT rdr;
  SaHpiEirIdT eirid;

  printf("%s ver %s\n",progname,progver);

  while ( (c = getopt( argc, argv,"a:xz?")) != EOF )
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
	  /*
	  printf( "String Length = %d\n", asset_len);
	  printf( "String Length = %d\n", strlen(optarg));
	  */
          break;
    default:
          printf("Usage: %s [-x] [-a asset_tag]\n", progname);
          printf("   -a  Sets the asset tag\n");
          printf("   -x  Display debug messages\n");
          printf("   -z  Display extra debug messages\n");
          exit(1);
  }
  inv = (SaHpiInventoryDataT *)(void *)&inbuff[0];
  rv = saHpiInitialize(&hpiVer);
  if (rv != SA_OK) {
    printf("saHpiInitialize error %d\n",rv);
    exit(-1);
  }
  rv = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID,&sessionid,NULL);
  if (rv != SA_OK) {
    printf("saHpiSessionOpen error %d\n",rv);
    exit(-1);
  }
 
  rv = saHpiResourcesDiscover(sessionid);
  if (fxdebug) printf("saHpiResourcesDiscover rv = %d\n",rv);
  rv = saHpiRptInfoGet(sessionid,&rptinfo);
  if (fxdebug) printf("saHpiRptInfoGet rv = %d\n",rv);
  if (fdebug) printf("RptInfo: UpdateCount = %x, UpdateTime = %lx\n",
         rptinfo.UpdateCount, (unsigned long)rptinfo.UpdateTimestamp);
 
  /* walk the RPT list */
  rptentryid = SAHPI_FIRST_ENTRY;
  while ((rv == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY))
  {
    rv = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
    if (rv != SA_OK) printf("RptEntryGet: rv = %d\n",rv);
    if (rv == SA_OK)
    {
      /* walk the RDR list for this RPT entry */
      entryid = SAHPI_FIRST_ENTRY;
      rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;
      resourceid = rptentry.ResourceId;
      printf("Resource Tag: %s\n", rptentry.ResourceTag.Data);
      if (fdebug) printf("rptentry[%d] resourceid=%d\n", rptentryid,resourceid);
      while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY))
      {
        rv = saHpiRdrGet(sessionid,resourceid, entryid,&nextentryid, &rdr);
  	if (fxdebug) printf("saHpiRdrGet[%d] rv = %d\n",entryid,rv);
	if (rv == SA_OK)
	{
	  if (rdr.RdrType == SAHPI_INVENTORY_RDR)
	  { 
	    /*type 3 includes inventory records*/
	    eirid = rdr.RdrTypeUnion.InventoryRec.EirId;
	    rdr.IdString.Data[rdr.IdString.DataLength] = 0;	    
	    if (fdebug) printf( "RDR[%d]: type=%d num=%d %s\n", rdr.RecordId,
		    rdr.RdrType, eirid, rdr.IdString.Data);
	    buffersize = sizeof(inbuff);
	    if (fdebug) printf("BufferSize=%d InvenDataRecSize=%zd\n",
		    buffersize, sizeof(inbuff));
	    if ( IsTagBmc(rdr.IdString.Data, rdr.IdString.DataLength) )
	    {
	      rv = saHpiEntityInventoryDataRead( sessionid, resourceid,
		  eirid, buffersize, inv, &actualsize);
  	      if (fxdebug) printf(
		    "saHpiEntityInventoryDataRead[%d] rv = %d\n", eirid, rv);
	      if (fdebug) printf("ActualSize=%d\n", actualsize);
	      if (rv == SA_OK)
	      {
	 	/* Walk thru the list of inventory data */
		for (i=0; inv->DataRecords[i] != NULL; i++)
		{
		  if (inv->Validity == SAHPI_INVENT_DATA_VALID) 
		  {
		    if (fdebug) printf( "Index = %d type=%x len=%d\n", i, 
			  inv->DataRecords[i]->RecordType, 
			  inv->DataRecords[i]->DataLength);
		    switch (inv->DataRecords[i]->RecordType)
		    {
		      case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
			  if (fdebug) printf( "Internal Use\n");
			  break;
		      case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
			  if (fdebug) printf( "Product Info\n");
			  prodrecindx = i;
			  prtprodtinfo();
			  break;
		      case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
			  if (fdebug) printf( "Chassis Info\n");
			  prtchassinfo();
			  break;
		      case SAHPI_INVENT_RECTYPE_BOARD_INFO:
			  if (fdebug) printf( "Board Info\n");
			  prtboardinfo();
			  break;
		      case SAHPI_INVENT_RECTYPE_OEM:
			  if (fdebug) printf( "OEM Record\n");
			  break;
		      default:
			  printf(" Invalid Invent Rec Type =%x\n",  
				  inv->DataRecords[i]->RecordType);
			  break;
		      }
		    }
		  }
		  if (IsTagBmc(rdr.IdString.Data,rdr.IdString.DataLength)
		      && (fasset == 1))
		  {
		    SaHpiTextBufferT aTag;
		    if (fdebug) printf( "Inventory = %s\n", rdr.IdString.Data);
		    /* prodrecindx contains index for Prod Rec Type */
		    dataptr = (SaHpiInventGeneralDataT *)
			&inv->DataRecords[prodrecindx]->RecordData.ProductInfo;
		    dataptr->AssetTag = &aTag;
		    strptr=dataptr->AssetTag;
		    strptr->DataType = SAHPI_TL_TYPE_LANGUAGE;
		    strptr->Language = SAHPI_LANG_ENGLISH;
		    strptr->DataLength = (SaHpiUint8T)asset_len;
		    strncpy( (char *)strptr->Data, (char *)asset_tag,asset_len);
		    strptr->Data[asset_len] = 0;

		    printf( "Writing new asset tag: %s\n",(char *)strptr->Data);
                    rv = saHpiEntityInventoryDataWrite( sessionid,
			  resourceid, eirid, inv);
		    printf("Return Write Status = %d\n", rv);
  
		    if (rv == SA_OK)
		    {
		      printf ("Good write - re-reading!\n");
	              rv = saHpiEntityInventoryDataRead( sessionid, resourceid,
		          eirid, buffersize, inv, &actualsize);
  	              if (fxdebug) printf(
		      "saHpiEntityInventoryDataRead[%d] rv = %d\n", eirid, rv);
	              if (fdebug) printf("ActualSize=%d\n", actualsize);
	              if (rv == SA_OK)
	              {
	 	        /* Walk thru the list of inventory data */
		        for (i=0; inv->DataRecords[i] != NULL; i++)
		        {
		          if (inv->Validity == SAHPI_INVENT_DATA_VALID) 
		          {
		            if (fdebug) printf( "Index = %d type=%x len=%d\n",
				i, inv->DataRecords[i]->RecordType, 
			        inv->DataRecords[i]->DataLength);
		            switch (inv->DataRecords[i]->RecordType)
		            {
		              case SAHPI_INVENT_RECTYPE_INTERNAL_USE:
			        if (fdebug) printf( "Internal Use\n");
			        break;
		              case SAHPI_INVENT_RECTYPE_PRODUCT_INFO:
			        if (fdebug) printf( "Product Info\n");
			        prtprodtinfo();
			        break;
		              case SAHPI_INVENT_RECTYPE_CHASSIS_INFO:
			        if (fdebug) printf( "Chassis Info\n");
			        prtchassinfo();
			        break;
		              case SAHPI_INVENT_RECTYPE_BOARD_INFO:
			        if (fdebug) printf( "Board Info\n");
			        prtboardinfo();
			        break;
		              case SAHPI_INVENT_RECTYPE_OEM:
			        if (fdebug) printf( "OEM Record\n");
			        break;
		              default:
			        printf(" Invalid Invent Rec Type =%x\n",  
				  inv->DataRecords[i]->RecordType);
			        break;
		            }
			  }
		        }    
		     } /*re-walk the inventory record list */
		  } /* Good write - re-read */
	        } /* check asset tag flag */
	      } else { printf("Returned HPI Error: rv=%d\n", rv); }
	    }
	  } /* Inventory Data Records - Type 3 */
	  entryid = nextentryid;
        }
      }
      rptentryid = nextrptentryid;
    }
  }
  rv = saHpiSessionClose(sessionid);
  rv = saHpiFinalize();
  exit(0);
}
 /* end hpifru.c */
