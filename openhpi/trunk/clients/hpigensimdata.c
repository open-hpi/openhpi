/*
 * hpigensimdata.c
 *
 * Copyright (c) 2010 by Lars Wetzel
 *           (c) 2011    Ulrich Kleber, Lars Wetzel
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Lars Wetzel (klw) <larswetzel@users.sourceforge.net>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 * 
 * Changes:
 * 10/02/09 (klw) Release 0.9
 * 04/20/10 (klw) Fix in fumi data structure FumiNum -> Num
 *          (klw) Fix in fumi data structure spaces are printed correctly
 * 02/01/11 ulikleber  Refactoring to use glib for option parsing and
 *                     introduce common options for all clients
 * 05/18/11 (klw) Fix in fumi data encapsulate FUMI_DATA
 *  
 * Open:
 * - print all events of a system (not clear if necessary) 
 * - check update mode (depends on NewSimulator Fumi implementation)
 * - maybe more selection criteria at the command line (eg. only sensor)
 *   depends on user which needs this client for some other reasons
 * 
 * Changes:
 *     01/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision: 6571 $"
#define GEN_SIM_DATA_VERSION "0.901000"

#define OFFSET_STEP 3
#define OFFSET_FILLER ' '
#define MAX_OFFSETS 20
#define MAX_CHAR 256
#define MAX_SHORT_CHAR 10

#define MODE_INIT 0
#define MODE_UPD  1

#define CONF_SECTION    "CONFIGURATION"
#define SENS_SECTION    "SENSOR"
#define CONT_SECTION    "CONTROL"
#define INV_SECTION     "INVENTORY"
#define WDT_SECTION     "WATCHDOG"
#define ANN_SECTION     "ANNUNCIATOR"
#define DIMI_SECTION    "DIMI"
#define FUMI_SECTION    "FUMI"
#define RPT_SECTION     "RPT"
#define RDR_SECTION     "RDR"
#define RDR_DETAIL      "RDR_DETAIL"

#define CONTROL_GET     "CONTROL_GET"
#define SENSOR_DATA     "SENSOR_DATA"
#define INVENTORY_DATA  "INVENTORY_DATA"
#define INV_AREA_DATA   "INV_AREA"
#define INV_FIELD_DATA  "INV_FIELD"
#define WDT_GET         "WDT_GET"
#define ANN_DATA        "ANNUNCIATOR_DATA"
#define ANNOUNCEMENT    "ANNOUNCEMENT"
#define DIMI_DATA       "DIMI_DATA"
#define DIMI_TEST       "DIMI_TESTCASE"
#define DIMI_TEST_DATA  "DIMI_TEST_DATA"
#define FUMI_DATA       "FUMI_DATA"
#define FUMI_SOURCE     "FUMI_SOURCE_DATA"
#define FUMI_TARGET     "FUMI_TARGET_DATA"
#define FUMI_LOG_TARGET "FUMI_LOG_TARGET_DATA"

typedef struct {
	int mode;
} ConfigurationDataT;

/* 
 * Globals
 */
static gint g_resourceid = (gint) SAHPI_UNSPECIFIED_RESOURCE_ID;
static gchar *g_file = NULL;
static gchar *g_mode = NULL;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "resource", 'r', 0, G_OPTION_ARG_INT,      &g_resourceid, "Select particular resource id for an update file", "res_id"   },
  { "file",     'f', 0, G_OPTION_ARG_FILENAME, &g_file,       "Name of the file to be generated",                 "filename" },
  { "mode",     'm', 0, G_OPTION_ARG_STRING,   &g_mode,       "Write update or initial file",                     "UPD|INIT" },
  { NULL }
};

#define GFREE g_option_context_free (context);g_free(g_file);g_free(g_mode);

char offSet[MAX_OFFSETS][MAX_CHAR];



/* 
 * Function prototypes
 */
static void       print_header(FILE *out, 
                                 int offset, 
                                 ConfigurationDataT data);

static SaErrorT    print_resources(FILE *out, 
                                    int offset, 
                                    SaHpiSessionIdT sessionid, 
                                    SaHpiResourceIdT res_id);

static SaErrorT    print_rpt(FILE *out, 
                              int offset, 
                              SaHpiRptEntryT *rptptr);

static void       print_resourceInfo(FILE *out, 
                                       int offset, 
                                       SaHpiResourceInfoT resinfo);

static SaErrorT    print_rdr(FILE *out, 
                              int offset,
                              SaHpiSessionIdT sessionid,
                              SaHpiResourceIdT resId,
                              SaHpiRdrT *rdrptr);

static void       print_common_rdr(FILE *out, 
                                     int offset, 
                                     SaHpiRdrT *rdrptr);

static SaErrorT    print_control_rdr(FILE *out, 
                                      int offset,
                                      SaHpiSessionIdT sessionid,
                                      SaHpiResourceIdT resId,
                                      SaHpiCtrlRecT *ctrl);

static void       print_control_state_stream(FILE *out, 
                                               int offset, 
                                               SaHpiCtrlStateStreamT data);
                                               
static void       print_control_state_text(FILE *out, 
                                             int offset, 
                                             SaHpiCtrlStateTextT data);
                                             
static void       print_control_state_oem(FILE *out, 
                                            int offset, 
                                            SaHpiCtrlStateOemT data);
                                                                        
static void       print_sensor_rdr(FILE *out, 
                                     int offset, 
                                     SaHpiSensorRecT *sens);
                                     
static SaErrorT    print_sensor_data(FILE *out, 
                                      int offset, 
                                      SaHpiSessionIdT sessionId, 
                                      SaHpiResourceIdT resId, 
                                      SaHpiSensorRecT *sens);
                                      
static int        print_sensor_thresholds(FILE *out, 
                                            int myoffset, 
                                            SaHpiSensorThresholdsT thres,
                                            SaHpiSensorDataFormatT format);
                                            
static int        check_sensor_reading(SaHpiSensorReadingT *read,
                                        SaHpiSensorDataFormatT format);   
                                                                                  
static void       print_sensor_data_format(FILE *out, 
                                             int offset, 
                                             SaHpiSensorDataFormatT data);
                                             
static void       print_sensor_threshold_definition(FILE *out, 
                                                      int offset, 
                                                      SaHpiSensorThdDefnT def);
                                     
static void       print_sensor_reading(FILE *out, 
                                         int offset, 
                                         SaHpiSensorReadingT val);
                                         
static void       print_inventory_rdr(FILE *out, 
                                        int offset, 
                                        SaHpiInventoryRecT *inv);
                                        
static SaErrorT    print_inventory_data(FILE *out, 
                                         int offset, 
                                         SaHpiSessionIdT sessionId, 
                                         SaHpiResourceIdT resId, 
                                         SaHpiInventoryRecT *inv);

static SaErrorT    print_inventory_area_data(FILE *out, 
                                              int offset, 
                                              SaHpiSessionIdT sessionId, 
                                              SaHpiResourceIdT resId, 
                                              SaHpiInventoryRecT *inv, 
                                              SaHpiIdrAreaHeaderT *area);

static void       print_inventory_field(FILE *out, 
                                          int offset, 
                                          SaHpiIdrFieldT field);

static SaErrorT    print_watchdog_rdr(FILE *out, 
                                       int offset,
                                       SaHpiSessionIdT sessionId, 
                                       SaHpiResourceIdT resId,
                                       SaHpiWatchdogRecT *wdt);

static void       print_annunciator_rdr(FILE *out, 
                                          int offset, 
                                          SaHpiAnnunciatorRecT *ann);

static SaErrorT    print_annunciator_data(FILE *out, 
                                           int offset,
                                           SaHpiSessionIdT sessionId, 
                                           SaHpiResourceIdT resId,
                                           SaHpiAnnunciatorRecT *ann);

static void       print_annunciator_cond(FILE *out, 
                                           int offset,
                                           SaHpiConditionT cond);

static void       print_dimi_rdr(FILE *out, 
                                   int offset, 
                                   SaHpiDimiRecT *dimi);
                                   
static SaErrorT    print_dimi_data(FILE *out, 
                                    int offset,
                                    SaHpiSessionIdT sessionId, 
                                    SaHpiResourceIdT resId,
                                    SaHpiDimiRecT *dimi);

static void       print_dimi_testparameter(FILE *out, 
                                             int offset, 
                                             SaHpiDimiTestParamsDefinitionT param);
                                   
static void       print_dimi_test_data(FILE *out, 
                                         int offset, 
                                         SaHpiDimiTestResultsT test); 

static void       print_fumi_rdr(FILE *out, 
                                  int offset, 
                                  SaHpiFumiRecT *fumi);

static SaErrorT    print_fumi_data(FILE *out, 
                                    int offset, 
                                    SaHpiSessionIdT sessionId, 
                                    SaHpiResourceIdT resId, 
                                    SaHpiFumiRecT *fumi);       

static void       print_fumi_source_info(FILE *out, 
                                           int offset, 
                                           SaHpiSessionIdT sessionId, 
                                           SaHpiResourceIdT resId, 
                                           SaHpiFumiRecT *fumi, 
                                           SaHpiBankNumT bnum);
                                           
static void       print_fumi_target_info(FILE *out, 
                                           int offset, 
                                           SaHpiSessionIdT sessionId, 
                                           SaHpiResourceIdT resId, 
                                           SaHpiFumiRecT *fumi, 
                                           SaHpiBankNumT bnum);
                                           
static void       print_fumi_logical_target_info(FILE *out, 
                                                   int offset, 
                                                   SaHpiSessionIdT sessionId, 
                                                   SaHpiResourceIdT resId, 
                                                   SaHpiFumiRecT *fumi);

static void       print_fumi_logical_component_info(FILE *out, 
                                                      int offset, 
                                                      SaHpiFumiLogicalComponentInfoT cinfo);

static void       print_fumi_component_info(FILE *out, 
                                              int offset, 
                                              SaHpiFumiComponentInfoT cinfo);

static void       print_fumi_fw_info(FILE *out, 
                                       int offset,
                                       SaHpiFumiFirmwareInstanceInfoT data);                                                                  

static void       print_textbuffer(FILE *out, 
                                     int offset, 
                                     SaHpiTextBufferT data);

static SaErrorT    print_ep(FILE *out, 
                             int offset, 
                             const SaHpiEntityPathT *ep);

/* 
 * Main                
 */
int main(int argc, char **argv) {
	SaErrorT 	     rv = SA_OK;
	SaHpiSessionIdT  sessionid;
	FILE             *outfile = stdout;
	ConfigurationDataT confdata;
        GOptionContext *context;
	
	confdata.mode = MODE_INIT;

        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Version " GEN_SIM_DATA_VERSION
		"\nThe client will print all HPI information in a format that can be parsed by Dynamic"
                "\nSimulator plugin.\n"
                OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127
                    - OHC_VERBOSE_OPTION     // no verbose mode 
                    - OHC_DEBUG_OPTION )) {  // no debug mode 
                GFREE
		return 1;
	}

	if (g_file) {
		outfile = fopen(g_file, "w");
		if (outfile == NULL) {
			CRIT("%s couldn't be opened for writing.", g_file);
                        GFREE
			return 1;
		}
	}

	if (g_mode) {
		int i = 0;
		while (i < MAX_SHORT_CHAR && g_mode[i] != '\0') {
			g_mode[i] = toupper((char) g_mode[i]);
			i++;
		}
		if (!strcmp(g_mode, "UPD")) {
			confdata.mode = MODE_UPD;
		} else if (!strcmp(g_mode, "INIT")) {
			confdata.mode = MODE_INIT;
		} else {
			fprintf(stderr, "\nUnknown mode %s.\n", g_mode);
                        if (g_file)	fclose(outfile);
                        GFREE
			return 1;
		}
	}

	/**
	 * Initialize the offset strings
	 **/
	int i, j;
	for(j=0; j < MAX_OFFSETS; j++) {
		for (i=0; i < MAX_OFFSETS*OFFSET_STEP; i++) {
			offSet[j][i] = OFFSET_FILLER;
		}
		offSet[j][OFFSET_STEP*j] = '\0';
	}

	print_header(outfile, 0, confdata);
	
        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) {
                if (g_file)	fclose(outfile);
                GFREE
		return rv;
        }

	/*
	 * Resource discovery
	 */
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
		printf("saHpiDiscover returns %s\n",oh_lookup_error(rv));
                if (g_file)	fclose(outfile);
                GFREE
		return rv;
	}
	print_resources(outfile, 0, sessionid, (SaHpiResourceIdT) g_resourceid);

	rv = saHpiSessionClose(sessionid);
	if (g_file)	fclose(outfile);
	GFREE	
	return 0;
}


/* 
 *
 */
static void print_header(FILE *out, int offset, ConfigurationDataT data) {
   int myoffset = offset;
   char strMode[MAX_CHAR];
	
   if (data.mode == MODE_INIT) {
      strcpy(strMode, "INIT");
   } else {
      strcpy(strMode, "UPD");
   }

   fprintf(out, "%s%s {\n", offSet[myoffset++], CONF_SECTION);
   fprintf(out, "%sMODE=%s\n", offSet[myoffset], strMode);
   fprintf(out, "%sVERSION=%s\n", offSet[myoffset], GEN_SIM_DATA_VERSION);
   fprintf(out, "%s}\n", offSet[--myoffset]);	
}

static SaErrorT print_resources(FILE *out, int offset, SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid) {
   SaErrorT rv       = SA_OK,
            rvRptGet = SA_OK; 

   SaHpiRptEntryT rptentry;
   SaHpiEntryIdT rptentryid;
   SaHpiEntryIdT nextrptentryid;
   SaHpiEntryIdT entryid;
   SaHpiEntryIdT nextentryid;
   SaHpiRdrT rdr;
   SaHpiTextBufferT working;
   int myoffset = offset;
		
   oh_init_textbuffer(&working);																		
									
   /* walk the RPT list */
   rptentryid = SAHPI_FIRST_ENTRY;
   do {
		
      rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
      if (rvRptGet != SA_OK) {
         fprintf(stderr, "RptEntryGet returns %s\n",oh_lookup_error(rvRptGet));
         return rvRptGet;
      }
		
      if ((resourceid == SAHPI_UNSPECIFIED_RESOURCE_ID) 
         || (resourceid == rptentry.ResourceId)) {
      	
         fprintf(out, "%s%s {\n", offSet[myoffset], RPT_SECTION);
         myoffset++;
         rv = print_rpt(out, myoffset, &rptentry);
         
         if (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) {
            /* walk the RDR list for this RPT entry */
            entryid = SAHPI_FIRST_ENTRY;	
            
            fprintf(out, "%s%s {\n", offSet[myoffset], RDR_SECTION);
            myoffset++;
            do {
            	   rv = saHpiRdrGet(sessionid,rptentry.ResourceId, entryid, &nextentryid, &rdr);
            	   if (rv != SA_OK) {
            	      fprintf(stderr, "saHpiRdrGet[%u, %u] rv = %s\n", rptentry.ResourceId, entryid,
            	                                                       oh_lookup_error(rv));
            	   }

               rv = print_rdr(out, myoffset, sessionid, rptentry.ResourceId, &rdr);
               entryid = nextentryid;

            } while ((rv == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) ;
            
            fprintf(out, "%s}\n", offSet[--myoffset]);	
         }
      }
      
      rptentryid = nextrptentryid;
      fprintf(out, "%s}\n", offSet[--myoffset]);
   } while (rptentryid != SAHPI_LAST_ENTRY);

   return(rv);
}

/**
 * print the rpt entries
 **/

static SaErrorT print_rpt(FILE *out, int offset, SaHpiRptEntryT *rptptr) {
   SaErrorT rv = SA_OK;
   int myoffset = offset;

   fprintf(out, "%sEntryId=%u\n",offSet[myoffset],rptptr->EntryId);
   fprintf(out, "%sResourceId=%u\n",offSet[myoffset],rptptr->ResourceId);
   print_resourceInfo(out, myoffset, rptptr->ResourceInfo);
   
   fprintf(out, "%sResourceEntity={\n",offSet[myoffset]);
   myoffset++;
   rv = print_ep(out, myoffset, &rptptr->ResourceEntity);
   myoffset--;
   fprintf(out, "%s}\n", offSet[myoffset]);
   
   fprintf(out, "%sResourceCapabilities=0x%08X\n",offSet[myoffset],rptptr->ResourceCapabilities);
   fprintf(out, "%sHotSwapCapabilities=0x%08X\n",offSet[myoffset],rptptr->HotSwapCapabilities);
   fprintf(out, "%sResourceSeverity=%u\n",offSet[myoffset],rptptr->ResourceSeverity);
   fprintf(out, "%sResourceFailed=%u\n",offSet[myoffset],rptptr->ResourceFailed);

   fprintf(out, "%sResourceTag={\n",offSet[myoffset]);
   myoffset++;
   print_textbuffer(out, myoffset, rptptr->ResourceTag);
   myoffset--;
   fprintf(out, "%s}\n", offSet[myoffset]);	

   return(rv);
}

static void print_resourceInfo(FILE *out, int offset, SaHpiResourceInfoT resinfo) {
	int myoffset = offset;
	int i;
	fprintf(out,"%sResourceInfo={\n", offSet[offset]);
	myoffset++;
	fprintf(out,"%sResourceRev=0x%02X\n", offSet[myoffset],resinfo.ResourceRev);
	fprintf(out,"%sSpecificVer=0x%02X\n", offSet[myoffset],resinfo.SpecificVer);
	fprintf(out,"%sDeviceSupport=0x%02X\n", offSet[myoffset],resinfo.DeviceSupport);
	fprintf(out,"%sManufacturerId=0x%08X\n", offSet[myoffset],resinfo.ManufacturerId);
	fprintf(out,"%sProductId=0x%04X\n", offSet[myoffset],resinfo.ProductId);
	fprintf(out,"%sFirmwareMajorRev=0x%02X\n", offSet[myoffset],resinfo.FirmwareMajorRev);
	fprintf(out,"%sFirmwareMinorRev=0x%02X\n", offSet[myoffset],resinfo.FirmwareMinorRev);
	fprintf(out,"%sAuxFirmwareRev=0x%02X\n", offSet[myoffset],resinfo.AuxFirmwareRev);
	fprintf(out,"%sGuid=\"", offSet[myoffset]);
	for (i=0; i<16 ; i++) fprintf(out,"%02X", resinfo.Guid[i]);
	fprintf(out,"\"\n");
	fprintf(out,"%s}\n",offSet[offset]);
}

static SaErrorT print_rdr(FILE *out, int offset, SaHpiSessionIdT sessionid, 
                           SaHpiResourceIdT resId, SaHpiRdrT *rdrptr) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   
   switch (rdrptr->RdrType) {
      case SAHPI_CTRL_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], CONT_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         rv = print_control_rdr(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.CtrlRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_SENSOR_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], SENS_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         print_sensor_rdr(out, myoffset, &rdrptr->RdrTypeUnion.SensorRec);
         rv = print_sensor_data(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.SensorRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_INVENTORY_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], INV_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         print_inventory_rdr(out, myoffset, &rdrptr->RdrTypeUnion.InventoryRec);
         rv = print_inventory_data(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.InventoryRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_WATCHDOG_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], WDT_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         rv = print_watchdog_rdr(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.WatchdogRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_ANNUNCIATOR_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], ANN_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         print_annunciator_rdr(out, myoffset, &rdrptr->RdrTypeUnion.AnnunciatorRec);
         rv = print_annunciator_data(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.AnnunciatorRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_DIMI_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], DIMI_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         print_dimi_rdr(out, myoffset, &rdrptr->RdrTypeUnion.DimiRec);
         rv = print_dimi_data(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.DimiRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      case SAHPI_FUMI_RDR:
         fprintf(out, "%s%s {\n",offSet[myoffset++], FUMI_SECTION);
         print_common_rdr(out, myoffset, rdrptr);
         fprintf(out, "%s%s {\n",offSet[myoffset++], RDR_DETAIL);
         print_fumi_rdr(out, myoffset, &rdrptr->RdrTypeUnion.FumiRec);
         rv = print_fumi_data(out, myoffset, sessionid, resId, &rdrptr->RdrTypeUnion.FumiRec);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         break;
      default:
         fprintf(out, "%sUNKNOWN_RDR={\n",offSet[myoffset++]);
   }
   
   fprintf(out, "%s}\n", offSet[--myoffset]);
   return rv;
}

static void print_common_rdr(FILE *out, int offset, SaHpiRdrT *rdrptr) {
   int myoffset = offset;
   
   fprintf(out,"%sRecordId=%u\n",offSet[myoffset], rdrptr->RecordId);
   fprintf(out,"%sRdrType=%u\n",offSet[myoffset], rdrptr->RdrType);
   fprintf(out,"%sEntity={\n",offSet[myoffset++]);
   print_ep(out, myoffset, &rdrptr->Entity);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out,"%sIsFru=%u\n",offSet[myoffset], rdrptr->IsFru);
   fprintf(out,"%sIdString={\n",offSet[myoffset]);
   myoffset++;
   print_textbuffer(out, myoffset, rdrptr->IdString);
   myoffset--;
   fprintf(out, "%s}\n", offSet[myoffset]);	
}

/**
 * Fumi functions
 **/
static void print_fumi_rdr(FILE *out, int offset, SaHpiFumiRecT *fumi) {
   int myoffset = offset; 

   fprintf(out,"%sNum=%u\n",offSet[myoffset], fumi->Num);
   fprintf(out,"%sAccessProt=%u\n",offSet[myoffset], fumi->AccessProt);
   fprintf(out,"%sCapability=%u\n",offSet[myoffset], fumi->Capability);
   fprintf(out,"%sNumBanks=%u\n",offSet[myoffset], fumi->NumBanks);
   fprintf(out,"%sOem=%u\n", offSet[myoffset], fumi->Oem);
}

static SaErrorT print_fumi_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                 SaHpiResourceIdT resId, SaHpiFumiRecT *fumi) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   int printData = TRUE;
   int i;
   
   // Get the fumi specification
   SaHpiFumiSpecInfoT fumispec;
   rv = saHpiFumiSpecInfoGet(sessionId, resId, fumi->Num, &fumispec);
   if (rv != SA_OK) {
      fprintf(stderr, "saHpiFumiSpecInfoGet returns %s for ResId %u Num %u -"
                      " no fumi data will be printed\n",
                      oh_lookup_error(rv), resId, fumi->Num);
      printData = FALSE;
   }
   
   SaHpiFumiServiceImpactDataT fumiimpact;
   rv = saHpiFumiServiceImpactGet(sessionId, resId, fumi->Num, &fumiimpact);
   if (rv != SA_OK) {
      fprintf(stderr, "saHpiFumiServiceImpactGet returns %s for ResId %u Num %u -" 
                      " no fumi data will be printed\n",
                      oh_lookup_error(rv), resId, fumi->Num);
      printData = FALSE;
      rv  = SA_OK; // Should not lead to an interruption
   }

   if (printData) {
      fprintf(out,"%s%s={\n", offSet[myoffset++], FUMI_DATA);
      
      // Fumi Spec Info
      fprintf(out,"%sSpecInfoType=%u\n", offSet[myoffset], fumispec.SpecInfoType);
      if (fumispec.SpecInfoType == SAHPI_FUMI_SPEC_INFO_SAF_DEFINED) {
         fprintf(out,"%sSafDefined={\n",offSet[myoffset++]);
         fprintf(out,"%sSpecID=%u\n",offSet[myoffset], fumispec.SpecInfoTypeUnion.SafDefined.SpecID);
         fprintf(out,"%sRevisionID=%u\n",offSet[myoffset], fumispec.SpecInfoTypeUnion.SafDefined.RevisionID);
         fprintf(out, "%s}\n",offSet[--myoffset]);
      } else if (fumispec.SpecInfoType == SAHPI_FUMI_SPEC_INFO_OEM_DEFINED) {
         fprintf(out,"%sOemDefined={\n",offSet[myoffset++]);
         fprintf(out,"%sMid=0x%08X\n", offSet[myoffset], fumispec.SpecInfoTypeUnion.OemDefined.Mid);
         fprintf(out,"%sBodyLength=%u\n",offSet[myoffset], fumispec.SpecInfoTypeUnion.OemDefined.BodyLength);
         fprintf(out,"%sBody=\"",offSet[myoffset]);
         for (i = 0; i < fumispec.SpecInfoTypeUnion.OemDefined.BodyLength; i++) 
            fprintf(out,"%02X", fumispec.SpecInfoTypeUnion.OemDefined.Body[i]);
         fprintf(out,"\"\n");
         fprintf(out, "%s}\n", offSet[--myoffset]);
      }
      
      // Fumi AutoRollback
      if (fumi->Capability & SAHPI_FUMI_CAP_AUTOROLLBACK) {
         SaHpiBoolT disabled;
         rv = saHpiFumiAutoRollbackDisableGet(sessionId, resId, fumi->Num, &disabled);
         if (rv != SA_OK) {
            fprintf(stderr, " saHpiFumiAutoRollbackDisableGet returns %s for ResId %u Num %u -" 
                            " no data is printed\n",
                            oh_lookup_error(rv), resId, fumi->Num);
         } else {
            fprintf(out,"%sAutoRollbackDisable=%u\n", offSet[myoffset], disabled);
         }
      }
      
      // Fumi Service Impact
      fprintf(out,"%sNumEntities=%u\n", offSet[myoffset], fumiimpact.NumEntities);
      for ( i = 0; i < fumiimpact.NumEntities; i++) {
         fprintf(out,"%sImpactedEntities={\n", offSet[myoffset++]);
         fprintf(out,"%sImpactedEntity={\n", offSet[myoffset++]);
         print_ep(out, myoffset, &fumiimpact.ImpactedEntities[i].ImpactedEntity);
         fprintf(out, "%s}\n", offSet[--myoffset]); //ImpactedEntity
         fprintf(out,"%sServiceImpact=%u\n",offSet[myoffset],fumiimpact.ImpactedEntities[i].ServiceImpact);
         fprintf(out, "%s}\n", offSet[--myoffset]); // ImpactedEntities
      }
      
      // Fumi Source and Target Information
      for (i = 0; i <= fumi->NumBanks; i++) {
         print_fumi_target_info(out, myoffset, sessionId, resId, fumi, i);
         print_fumi_source_info(out, myoffset, sessionId, resId, fumi, i);
      }

      // Fumi logical Target Information
      print_fumi_logical_target_info(out, myoffset, sessionId, resId, fumi);

      fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_DATA
   }
   return rv;
}

static void print_fumi_logical_target_info(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                             SaHpiResourceIdT resId, SaHpiFumiRecT *fumi) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   SaHpiFumiLogicalBankInfoT logbank;
   int printHeader = FALSE;
   
   rv = saHpiFumiLogicalTargetInfoGet(sessionId, resId, fumi->Num, &logbank);
   if (rv == SA_OK) {
      printHeader = TRUE;
      fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_LOG_TARGET);
      
      fprintf(out,"%sFirmwarePersistentLocationCount=%u\n",offSet[myoffset], logbank.FirmwarePersistentLocationCount);
      fprintf(out,"%sBankStateFlags=0x%08X\n",offSet[myoffset], logbank.BankStateFlags);
      fprintf(out,"%sPendingFwInstance={\n",offSet[myoffset++]);
      print_fumi_fw_info(out, myoffset, logbank.PendingFwInstance);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sRollbackFwInstance={\n",offSet[myoffset++]);
      print_fumi_fw_info(out, myoffset, logbank.RollbackFwInstance);
      fprintf(out, "%s}\n", offSet[--myoffset]);
   }
   
   if (fumi->Capability & SAHPI_FUMI_CAP_COMPONENTS) {
      SaHpiEntryIdT entry = SAHPI_FIRST_ENTRY;
      SaHpiEntryIdT next;
      SaHpiFumiLogicalComponentInfoT cinfo;
      rv = SA_OK;
      while ((entry != SAHPI_LAST_ENTRY) && (rv == SA_OK)) {
         rv = saHpiFumiLogicalTargetComponentInfoGet(sessionId, resId, fumi->Num,  
                                                     entry, &next, &cinfo);
         if (rv == SA_OK) {
            
            fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_LOG_TARGET);
            printHeader = TRUE;
            print_fumi_logical_component_info(out, myoffset, cinfo);
            fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_TARGET  
         }
         entry = next;
      }
   }
   
   if (printHeader)
      fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_TARGET  
}

static void print_fumi_target_info(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                 SaHpiResourceIdT resId, SaHpiFumiRecT *fumi, SaHpiBankNumT bnum) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   SaHpiFumiBankInfoT bank;
   int printHeader = FALSE;
   
   rv = saHpiFumiTargetInfoGet(sessionId, resId, fumi->Num, bnum, &bank);
   if (rv == SA_OK) {
      printHeader = TRUE;
      fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_TARGET);
      
      fprintf(out,"%sBankId=%u\n", offSet[myoffset], bank.BankId);
      fprintf(out,"%sBankSize=%u\n", offSet[myoffset], bank.BankSize);
      fprintf(out,"%sPosition=%u\n", offSet[myoffset], bank.Position);
      fprintf(out,"%sBankState=%u\n", offSet[myoffset], bank.BankState);
      fprintf(out,"%sIdentifier={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, bank.Identifier);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sDescription={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, bank.Description);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sDateTime={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, bank.DateTime);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out, "%sMajorVersion=%u\n", offSet[myoffset], bank.MajorVersion);
      fprintf(out, "%sMinorVersion=%u\n", offSet[myoffset], bank.MinorVersion);
      fprintf(out, "%sAuxVersion=%u\n", offSet[myoffset], bank.AuxVersion);
   }

   if (fumi->Capability & SAHPI_FUMI_CAP_COMPONENTS) {
      SaHpiEntryIdT entry = SAHPI_FIRST_ENTRY;
      SaHpiEntryIdT next;
      SaHpiFumiComponentInfoT cinfo;
      rv = SA_OK;
      while ((entry != SAHPI_LAST_ENTRY) && (rv == SA_OK)) {
         rv = saHpiFumiTargetComponentInfoGet(sessionId, resId, fumi->Num, bnum, 
                                              entry, &next, &cinfo);
         if (rv == SA_OK) {
            
            fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_TARGET);
            printHeader = TRUE;
            print_fumi_component_info(out, myoffset, cinfo);
            fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_TARGET
         }
         entry = next;
      }
   }
   
   if (printHeader)
      fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_TARGET

}


static void print_fumi_source_info(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                 SaHpiResourceIdT resId, SaHpiFumiRecT *fumi, SaHpiBankNumT bnum) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   SaHpiFumiSourceInfoT source;
   int printHeader = FALSE;
   
   rv = saHpiFumiSourceInfoGet(sessionId, resId, fumi->Num, bnum, &source);
   if (rv == SA_OK) {
      printHeader = TRUE;
      fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_SOURCE);
      fprintf(out,"%sForBank=%u\n", offSet[myoffset], bnum);
      fprintf(out,"%sSourceUri={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, source.SourceUri);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sSourceStatus=%u\n", offSet[myoffset], source.SourceStatus);
      fprintf(out,"%sIdentifier={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, source.Identifier);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sDescription={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, source.Description);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out,"%sDateTime={\n",offSet[myoffset++]);
      print_textbuffer(out, myoffset, source.DateTime);
      fprintf(out, "%s}\n", offSet[--myoffset]);
      fprintf(out, "%sMajorVersion=%u\n", offSet[myoffset], source.MajorVersion);
      fprintf(out, "%sMinorVersion=%u\n", offSet[myoffset], source.MinorVersion);
      fprintf(out, "%sAuxVersion=%u\n", offSet[myoffset], source.AuxVersion);
   }
   
   if (fumi->Capability & SAHPI_FUMI_CAP_COMPONENTS) {
      SaHpiEntryIdT entry = SAHPI_FIRST_ENTRY;
      SaHpiEntryIdT next;
      SaHpiFumiComponentInfoT cinfo;
      rv = SA_OK;
      while ((entry != SAHPI_LAST_ENTRY) && (rv == SA_OK)) {
         rv = saHpiFumiSourceComponentInfoGet(sessionId, resId, fumi->Num, bnum, 
                                              entry, &next, &cinfo);
         if (rv == SA_OK) {
            
            fprintf(out,"%s%s={\n",offSet[myoffset++], FUMI_SOURCE);
            printHeader = TRUE;
            print_fumi_component_info(out, myoffset, cinfo);
            fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_SOURCE
         }
         entry = next;
      }
   }
   
   if (printHeader)
      fprintf(out, "%s}\n", offSet[--myoffset]); // FUMI_SOURCE
   
}

static void print_fumi_logical_component_info(FILE *out, int offset, SaHpiFumiLogicalComponentInfoT cinfo) {
   int myoffset = offset;
   
   fprintf(out, "%sEntryId=%u\n", offSet[myoffset], cinfo.EntryId);
   fprintf(out, "%sComponentId=%u\n", offSet[myoffset], cinfo.ComponentId);
   fprintf(out, "%sPendingFwInstance={\n",offSet[myoffset++]);
   print_fumi_fw_info(out, myoffset, cinfo.PendingFwInstance);
   fprintf(out, "%s}\n", offSet[--myoffset]); // PendingFwInstance
   fprintf(out, "%sRollbackFwInstance={\n",offSet[myoffset++]);
   print_fumi_fw_info(out, myoffset, cinfo.RollbackFwInstance);
   fprintf(out, "%s}\n", offSet[--myoffset]); // RollbackFwInstance
   fprintf(out, "%sComponentFlags=%u\n",offSet[myoffset], cinfo.ComponentFlags);
}

static void print_fumi_component_info(FILE *out, int offset, SaHpiFumiComponentInfoT cinfo) {
   int myoffset = offset;
   
   fprintf(out, "%sEntryId=%u\n", offSet[myoffset], cinfo.EntryId);
   fprintf(out, "%sComponentId=%u\n", offSet[myoffset], cinfo.ComponentId);
   fprintf(out, "%sMainFwInstance={\n",offSet[myoffset++]);
   print_fumi_fw_info(out, myoffset, cinfo.MainFwInstance);
   fprintf(out, "%s}\n", offSet[--myoffset]); // MainFwInstance
   fprintf(out, "%sComponentFlags=%u\n",offSet[myoffset], cinfo.ComponentFlags);
}

static void print_fumi_fw_info(FILE *out, int offset, SaHpiFumiFirmwareInstanceInfoT data) {
   int myoffset = offset;
   
   fprintf(out,"%sInstancePresent=%u\n",offSet[myoffset], data.InstancePresent);
   fprintf(out,"%sIdentifier={\n",offSet[myoffset++]);
   print_textbuffer(out, myoffset, data.Identifier);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out,"%sDescription={\n",offSet[myoffset++]);
   print_textbuffer(out, myoffset, data.Description);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out,"%sDateTime={\n",offSet[myoffset++]);
   print_textbuffer(out, myoffset, data.DateTime);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sMajorVersion=%u\n", offSet[myoffset], data.MajorVersion);
   fprintf(out, "%sMinorVersion=%u\n", offSet[myoffset], data.MinorVersion);
   fprintf(out, "%sAuxVersion=%u\n", offSet[myoffset], data.AuxVersion);
}


/**
 * Dimi functions
 **/

static void print_dimi_rdr(FILE *out, int offset, SaHpiDimiRecT *dimi) {
   int myoffset = offset;
   
   fprintf(out,"%sDimiNum=%u\n",offSet[myoffset], dimi->DimiNum);                               	
   fprintf(out,"%sOem=%u\n", offSet[myoffset], dimi->Oem);
}
                               
static SaErrorT print_dimi_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                 SaHpiResourceIdT resId, SaHpiDimiRecT *dimi) {
   int myoffset = offset;
   int i,j;
   SaErrorT rv  = SA_OK;
   SaHpiDimiInfoT testinfo;
   
   rv = saHpiDimiInfoGet(sessionId, resId, dimi->DimiNum, &testinfo);
   if (rv != SA_OK) {
      fprintf(stderr, "saHpiDimiInfoGet returns %s for ResId %u Num %u - will be Ignored, all is set to 0\n",
              oh_lookup_error(rv), resId, dimi->DimiNum);
      testinfo.NumberOfTests = 0;
      testinfo.TestNumUpdateCounter = 0;
      rv = SA_OK;
   }
   fprintf(out,"%s%s={\n", offSet[myoffset++], DIMI_DATA);
   fprintf(out,"%sNumberOfTests=%u\n", offSet[myoffset], testinfo.NumberOfTests);
   fprintf(out,"%sTestNumUpdateCounter=%u\n", offSet[myoffset], testinfo.TestNumUpdateCounter);
   
   for (i = 0; i < testinfo.NumberOfTests; i++) {
      SaHpiDimiTestT testcase;
      rv = saHpiDimiTestInfoGet(sessionId, resId, dimi->DimiNum, i, &testcase);
      if (rv != SA_OK) {
         fprintf(stderr, "saHpiDimiTestInfoGet returns %s for ResId %u DimiNum %u Test %u-" 
                          "will be Ignored, no data is printed\n",
                 oh_lookup_error(rv), resId, dimi->DimiNum, i);
         rv = SA_OK;
         
      } else {
      	
         // Testcase
         fprintf(out,"%s%s={\n", offSet[myoffset++], DIMI_TEST);
         fprintf(out,"%sTestName={\n", offSet[myoffset++]);
         print_textbuffer(out, myoffset, testcase.TestName);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         fprintf(out,"%sServiceImpact=%u\n",offSet[myoffset], testcase.ServiceImpact);
         for (j = 0; j < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; j++) {
            fprintf(out,"%sEntitiesImpacted={\n", offSet[myoffset++]);
            fprintf(out,"%sEntityImpacted={\n", offSet[myoffset++]);
            print_ep(out, myoffset, &testcase.EntitiesImpacted[j].EntityImpacted);
            fprintf(out, "%s}\n", offSet[--myoffset]);
            fprintf(out,"%sServiceImpact=%u\n",offSet[myoffset],testcase.EntitiesImpacted[j].ServiceImpact); 
            fprintf(out, "%s}\n", offSet[--myoffset]);
         }
         
         fprintf(out,"%sNeedServiceOS=%u\n",offSet[myoffset], testcase.NeedServiceOS);
         fprintf(out,"%sServiceOS={\n", offSet[myoffset++]);
         print_textbuffer(out, myoffset, testcase.ServiceOS);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         fprintf(out,"%sExpectedRunDuration=%ld\n",offSet[myoffset], (long int) testcase.ExpectedRunDuration);
         fprintf(out,"%sTestCapabilities=%u\n",offSet[myoffset], testcase.TestCapabilities);
         for (j = 0; j < SAHPI_DIMITEST_MAX_PARAMETERS; j++) {
            fprintf(out,"%sTestParameters={\n", offSet[myoffset++]);
            print_dimi_testparameter(out, myoffset, testcase.TestParameters[j]);
            fprintf(out, "%s}\n", offSet[--myoffset]); // TestParameters
         }
         
         // Testcase readiness
         SaErrorT rv_testdata = SA_OK;
         SaHpiDimiReadyT testready;
         rv_testdata = saHpiDimiTestReadinessGet(sessionId, resId, dimi->DimiNum, i, &testready);
         if (rv_testdata != SA_OK) {
            fprintf(stderr, "saHpiDimiTestReadinessGet returns %s for ResId %u DimiNum %u Test %u-" 
                            "SAHPI_DIMI_WRONG_STATE is used instead\n",
                    oh_lookup_error(rv), resId, dimi->DimiNum, i);
            testready = SAHPI_DIMI_WRONG_STATE;
         }
         fprintf(out,"%sTestReadiness=%u\n", offSet[myoffset], testready);
         
         // Testcase data 
         SaHpiDimiTestResultsT testresult;
         rv_testdata = saHpiDimiTestResultsGet(sessionId, resId, dimi->DimiNum, i, &testresult);
         if (rv_testdata != SA_OK) {
            fprintf(stderr, "saHpiDimiTestResultsGet returns %s for ResId %u DimiNum %u Test %u -" 
                            "will be Ignored, no data is printed\n",
                    oh_lookup_error(rv), resId, dimi->DimiNum, i);
         } else {
            fprintf(out,"%s%s={\n", offSet[myoffset++], DIMI_TEST_DATA);
            print_dimi_test_data(out, myoffset, testresult);
            fprintf(out, "%s}\n", offSet[--myoffset]); // DIMI_TEST_DATA
         }
         fprintf(out, "%s}\n", offSet[--myoffset]); // DIMI_TEST
      }
   }

   fprintf(out, "%s}\n", offSet[--myoffset]); // DIMI_DATA
   return rv;
}

static void print_dimi_testparameter(FILE *out, int offset, SaHpiDimiTestParamsDefinitionT param) {
   int myoffset = offset;
   
   fprintf(out,"%sParamName=\"%s\"\n", offSet[myoffset], (char *) param.ParamName);
   fprintf(out,"%sParamInfo={\n", offSet[myoffset++]);
   print_textbuffer(out, myoffset, param.ParamInfo);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out,"%sParamType=%u\n", offSet[myoffset],param.ParamType);
   switch (param.ParamType) {
      case SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN:
         fprintf(out,"%sDefaultParam=%u\n", offSet[myoffset], param.DefaultParam.parambool);
         break;
      case SAHPI_DIMITEST_PARAM_TYPE_INT32:
         fprintf(out,"%sMinValue=%d\n", offSet[myoffset], param.MinValue.IntValue);
         fprintf(out,"%sMaxValue=%d\n", offSet[myoffset], param.MaxValue.IntValue);
         fprintf(out,"%sDefaultParam=%d\n", offSet[myoffset], param.DefaultParam.paramint);
         break;
      case SAHPI_DIMITEST_PARAM_TYPE_FLOAT64:
         fprintf(out,"%sMinValue=%lf\n", offSet[myoffset], param.MinValue.FloatValue);
         fprintf(out,"%sMaxValue=%lf\n", offSet[myoffset], param.MaxValue.FloatValue);
         fprintf(out,"%sDefaultParam=%lf\n", offSet[myoffset], param.DefaultParam.paramfloat);
         break;
      case SAHPI_DIMITEST_PARAM_TYPE_TEXT:
         fprintf(out,"%sDefaultParam={\n", offSet[myoffset++]);
         print_textbuffer(out, myoffset, param.DefaultParam.paramtext);
         fprintf(out, "%s}\n", offSet[--myoffset]);
      default:
         fprintf(out,"%sUNKNOWN_PARAM_TYPE\n", offSet[myoffset]);
   }
}
static void print_dimi_test_data(FILE *out, int offset, SaHpiDimiTestResultsT test) {
   int myoffset = offset;

   fprintf(out,"%sResultTimeStamp=%ld\n", offSet[myoffset], (long int) test.ResultTimeStamp);
   fprintf(out,"%sRunDuration=%ld\n", offSet[myoffset], (long int) test.RunDuration);
   fprintf(out,"%sLastRunStatus=%u\n", offSet[myoffset], test.LastRunStatus);
   fprintf(out,"%sTestErrorCode=%u\n", offSet[myoffset], test.TestErrorCode);
   fprintf(out,"%sTestResultString={\n", offSet[myoffset++]);
   print_textbuffer(out, myoffset, test.TestResultString);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out,"%sTestResultStringIsURI=%d\n", offSet[myoffset], test.TestResultStringIsURI);
}


/**
 * Annunciator functions
 **/
static void print_annunciator_rdr(FILE *out, int offset, SaHpiAnnunciatorRecT *ann) {
   int myoffset = offset;
   
   fprintf(out,"%sAnnunciatorNum=%u\n",offSet[myoffset], ann->AnnunciatorNum);
   fprintf(out,"%sAnnunciatorType=0x%02X\n",offSet[myoffset], ann->AnnunciatorType);
   fprintf(out,"%sModeReadOnly=%d\n",offSet[myoffset], ann->ModeReadOnly);
   fprintf(out,"%sMaxConditions=%u\n", offSet[myoffset], ann->MaxConditions);
   fprintf(out,"%sOem=%u\n", offSet[myoffset], ann->Oem);
}

static SaErrorT print_annunciator_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                        SaHpiResourceIdT resId, SaHpiAnnunciatorRecT *ann) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   SaErrorT rv_loop = SA_OK;
   SaHpiAnnunciatorModeT annMode;

   // Due to the fact that the Annunciator is supported only by few implementations
   // An error in the following function will not stop the processing
   rv = saHpiAnnunciatorModeGet(sessionId, resId, ann->AnnunciatorNum, &annMode);
   if (rv != SA_OK) {
       fprintf(stderr, "AnnunciatorModeGet returns %s for ResId %u Num %u - will be Ignored\n",
               oh_lookup_error(rv), resId, ann->AnnunciatorNum);
       annMode = SAHPI_ANNUNCIATOR_MODE_AUTO;
       rv = SA_OK;
   }
   
   fprintf(out,"%s%s={\n", offSet[myoffset++], ANN_DATA);
   fprintf(out,"%sMode=%u\n", offSet[myoffset], annMode);

   SaHpiAnnouncementT announcement;
   announcement.EntryId = SAHPI_FIRST_ENTRY;
   
   while (rv_loop == SA_OK) {
      rv_loop = saHpiAnnunciatorGetNext(sessionId, resId, ann->AnnunciatorNum, SAHPI_ALL_SEVERITIES,
                                        SAHPI_FALSE, &announcement);
      if (rv_loop == SA_OK) {
         fprintf(out,"%s%s={\n", offSet[myoffset++], ANNOUNCEMENT);
         fprintf(out,"%sEntryId=%u\n", offSet[myoffset], announcement.EntryId);
         fprintf(out,"%sTimestamp=%ld\n", offSet[myoffset], (long int) announcement.Timestamp);
         fprintf(out,"%sAddedByUser=%d\n", offSet[myoffset], announcement.AddedByUser);
         fprintf(out,"%sSeverity=%u\n", offSet[myoffset], announcement.Severity);
         fprintf(out,"%sAcknowledged=%d\n", offSet[myoffset], announcement.Acknowledged);
         fprintf(out,"%sStatusCond={\n", offSet[myoffset++]);
         print_annunciator_cond(out, myoffset, announcement.StatusCond);
         fprintf(out, "%s}\n", offSet[--myoffset]); // StatusCond
         fprintf(out, "%s}\n", offSet[--myoffset]); // ANNOUNCEMENT
      }
   }
   
   fprintf(out, "%s}\n", offSet[--myoffset]); // ANN_DATA
   return rv;
}

static void print_annunciator_cond(FILE *out, int offset, SaHpiConditionT cond) {
   int myoffset = offset;
   int i;
   
   fprintf(out,"%sType=%u\n", offSet[myoffset], cond.Type);
   fprintf(out,"%sEntity={\n",offSet[myoffset++]);
   print_ep(out, myoffset, &cond.Entity);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sDomainId=%u\n", offSet[myoffset], cond.DomainId);
   fprintf(out, "%sResourceId=%u\n", offSet[myoffset], cond.ResourceId);
   if (cond.Type == SAHPI_STATUS_COND_TYPE_SENSOR) {
      fprintf(out, "%sSensorNum=%u\n", offSet[myoffset], cond.SensorNum);
      fprintf(out, "%sEventState=0x%04X\n", offSet[myoffset], cond.EventState);
   }
   fprintf(out,"%sName={\n",offSet[myoffset++]);
   fprintf(out, "%sLength=%u\n",offSet[myoffset], cond.Name.Length);
   fprintf(out, "%sValue=\"",offSet[myoffset]);
   for (i = 0; i < cond.Name.Length; i++)
      fprintf(out, "%c", cond.Name.Value[i]);
   fprintf(out, "\"\n");
   fprintf(out, "%s}\n", offSet[--myoffset]);
   if (cond.Type == SAHPI_STATUS_COND_TYPE_OEM) {
      fprintf(out, "%sMid=0x%08X\n", offSet[myoffset], cond.Mid);
   }
   fprintf(out,"%sData={\n",offSet[myoffset++]);
   print_textbuffer(out, myoffset, cond.Data);
   fprintf(out, "%s}\n", offSet[--myoffset]);
}


/** 
 * Watchdog functions
 **/
static SaErrorT print_watchdog_rdr(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                 SaHpiResourceIdT resId, SaHpiWatchdogRecT *wdt) {
   int myoffset = offset;
   SaErrorT rv  = SA_OK;
   
   fprintf(out,"%sWatchdogNum=%u\n", offSet[myoffset], wdt->WatchdogNum);
   fprintf(out,"%sOem=%u\n", offSet[myoffset], wdt->Oem);
 
   SaHpiWatchdogT wdtTimer;
   rv = saHpiWatchdogTimerGet(sessionId, resId, wdt->WatchdogNum, &wdtTimer);
   if (rv != SA_OK) {
      fprintf(stderr, "WatchdogTimerGet returns %s for ResId %u Num %u\n",
              oh_lookup_error(rv), resId, wdt->WatchdogNum);
      return rv;
   }
   
   fprintf(out,"%s%s={\n",offSet[myoffset++],WDT_GET);
   fprintf(out,"%sLog=%d\n", offSet[myoffset], wdtTimer.Log);
   fprintf(out,"%sRunning=%d\n", offSet[myoffset], wdtTimer.Running);
   fprintf(out,"%sTimerUse=0x%02X\n", offSet[myoffset], wdtTimer.TimerUse);
   fprintf(out,"%sTimerAction=0x%02X\n", offSet[myoffset], wdtTimer.TimerAction);
   fprintf(out,"%sPretimerInterrupt=0x%02X\n", offSet[myoffset], wdtTimer.PretimerInterrupt);
   fprintf(out,"%sPreTimeoutInterval=%u\n", offSet[myoffset], wdtTimer.PreTimeoutInterval);
   fprintf(out,"%sTimerUseExpFlags=0x%02X\n", offSet[myoffset], wdtTimer.TimerUseExpFlags);
   fprintf(out,"%sInitialCount=%u\n", offSet[myoffset], wdtTimer.InitialCount);
   fprintf(out,"%sPresentCount=%u\n", offSet[myoffset], wdtTimer.PresentCount);
   fprintf(out, "%s}\n", offSet[--myoffset]); // WDT_GET
   
   return rv;
}


/**
 * Inventory functions
 **/

static void print_inventory_rdr(FILE *out, int offset, SaHpiInventoryRecT *inv) {
   int myoffset = offset;
   
   fprintf(out,"%sIdrId=%u\n", offSet[myoffset], inv->IdrId);
   fprintf(out,"%sPersistent=%d\n", offSet[myoffset], inv->Persistent);
   fprintf(out,"%sOem=%u\n", offSet[myoffset], inv->Oem);
}

static SaErrorT print_inventory_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                      SaHpiResourceIdT resId, SaHpiInventoryRecT *inv) {
   int            myoffset = offset;
   SaErrorT       rv  = SA_OK;                                 	
   SaHpiIdrInfoT  idrInfo;
   
   rv = saHpiIdrInfoGet(sessionId, resId, inv->IdrId, &idrInfo);
   if (rv != SA_OK) {
      fprintf(stderr, "IdrInfoGet returns %s for ResId %u IdrId %u\n",
              oh_lookup_error(rv), resId, inv->IdrId);
      return rv;
   }
   
   fprintf(out,"%s%s={\n",offSet[myoffset++],INVENTORY_DATA);
   fprintf(out,"%sIdrId=%u\n", offSet[myoffset], idrInfo.IdrId);
   fprintf(out,"%sUpdateCount=%u\n", offSet[myoffset], idrInfo.UpdateCount);
   fprintf(out,"%sReadOnly=%d\n", offSet[myoffset], idrInfo.ReadOnly);
   fprintf(out,"%sNumAreas=%u\n", offSet[myoffset], idrInfo.NumAreas);
   
   SaHpiEntryIdT areaId = SAHPI_FIRST_ENTRY;
   SaHpiEntryIdT nextId;
   SaHpiIdrAreaHeaderT header;
   int numAreas = 0; 

   while ((areaId != SAHPI_LAST_ENTRY) &&
           (rv == SA_OK) &&
           (numAreas <= idrInfo.NumAreas))  {
   	
      rv = saHpiIdrAreaHeaderGet(sessionId, resId, inv->IdrId, SAHPI_IDR_AREATYPE_UNSPECIFIED,
                                 areaId, &nextId, &header);
      if (rv != SA_OK) {
         fprintf(stderr, "IdrAreaHeaderGet returns %s for ResId %u IdrId %u areaId %u\n",
                 oh_lookup_error(rv), resId, inv->IdrId, areaId);
         break;
      }

      numAreas++;
      areaId = nextId;
      rv = print_inventory_area_data(out, myoffset, sessionId, resId, inv, &header);
   }
   
   fprintf(out, "%s}\n", offSet[--myoffset]); //INVENTORY_DATA 
   return rv;
}

static SaErrorT print_inventory_area_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                           SaHpiResourceIdT resId, SaHpiInventoryRecT *inv, 
                                           SaHpiIdrAreaHeaderT *area) {
   int      myoffset = offset;
   SaErrorT rv  = SA_OK;     

   fprintf(out,"%s%s={\n", offSet[myoffset++], INV_AREA_DATA);
   fprintf(out,"%sAreaId=%u\n", offSet[myoffset], area->AreaId);
   fprintf(out,"%sType=%u\n", offSet[myoffset], area->Type);
   fprintf(out,"%sReadOnly=%d\n", offSet[myoffset], area->ReadOnly);
   fprintf(out,"%sNumFields=%u\n", offSet[myoffset], area->NumFields);

   SaHpiEntryIdT  fieldId = SAHPI_FIRST_ENTRY;
   SaHpiEntryIdT  nextId;
   SaHpiIdrFieldT field;
   int numFields = 0;
   
   while ((fieldId != SAHPI_LAST_ENTRY) &&
           (rv == SA_OK) &&
           (numFields <= area->NumFields))  {
      
      rv = saHpiIdrFieldGet(sessionId, resId, inv->IdrId, area->AreaId, 
                            SAHPI_IDR_FIELDTYPE_UNSPECIFIED, fieldId, &nextId, &field);
      if (rv != SA_OK) {
         fprintf(stderr, "IdrFieldGet returns %s for ResId %u IdrId %u areaId %u fieldId %u\n",
                 oh_lookup_error(rv), resId, inv->IdrId, area->AreaId, fieldId);
         break;
      }
      fieldId = nextId;
      numFields++;
      
      fprintf(out,"%s%s={\n", offSet[myoffset++], INV_FIELD_DATA);
      print_inventory_field(out, myoffset, field);
      fprintf(out,"%s}\n", offSet[--myoffset]); // INV_FIELD_DATA
   }
   fprintf(out,"%s}\n", offSet[--myoffset]); // INV_AREA_DATA

   return rv;
}

static void print_inventory_field(FILE *out, int offset, SaHpiIdrFieldT field) {
   int myoffset = offset;
   
   fprintf(out,"%sAreaId=%u\n", offSet[myoffset], field.AreaId);
   fprintf(out,"%sFieldId=%u\n", offSet[myoffset], field.FieldId);
   fprintf(out,"%sType=%u\n", offSet[myoffset], field.Type);
   fprintf(out,"%sReadOnly=%d\n", offSet[myoffset], field.ReadOnly);
   fprintf(out,"%sField={\n", offSet[myoffset++]);
   print_textbuffer(out, myoffset, field.Field);
   fprintf(out,"%s}\n", offSet[--myoffset]);
}


/** 
 * Control functions
 **/ 
static SaErrorT print_control_rdr(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                   SaHpiResourceIdT resId, SaHpiCtrlRecT *ctrl) {
   int            myoffset = offset;
   int            i;
   SaErrorT        rv  = SA_OK;
   SaHpiCtrlModeT  modeGet;
   SaHpiCtrlStateT stateGet;
   SaHpiBoolT allowGet = ( ctrl->WriteOnly != SAHPI_FALSE ) ? SAHPI_FALSE : SAHPI_TRUE;
   
   if (allowGet)
      rv = saHpiControlGet(sessionId, resId, ctrl->Num, &modeGet, &stateGet);
   if (rv != SA_OK) {
      fprintf(stderr, "ControlGet returns %s for ResId %u ControlNum %u\n",
              oh_lookup_error(rv), resId, ctrl->Num);
   }
    
   fprintf(out, "%sNum=%u\n", offSet[myoffset], ctrl->Num);
   fprintf(out, "%sOutputType=%u\n", offSet[myoffset], ctrl->OutputType);
   fprintf(out, "%sType=%u\n", offSet[myoffset], ctrl->Type);
   
   switch (ctrl->Type) {
      case SAHPI_CTRL_TYPE_DIGITAL:
         fprintf(out, "%sTypeUnion.Digital={\n", offSet[myoffset]);
         myoffset++;
         fprintf(out,"%sDefault=%u\n", offSet[myoffset], ctrl->TypeUnion.Digital.Default);
         if (allowGet)
            if (stateGet.StateUnion.Digital != ctrl->TypeUnion.Digital.Default)
               fprintf(out,"%s%s=%u\n", offSet[myoffset], CONTROL_GET, stateGet.StateUnion.Digital);
         break;
      
      case SAHPI_CTRL_TYPE_DISCRETE:
         fprintf(out, "%sTypeUnion.Discrete={\n", offSet[myoffset]);
         myoffset++;
         fprintf(out,"%sDefault=%u\n", offSet[myoffset], ctrl->TypeUnion.Discrete.Default);
         if (allowGet)
            if (stateGet.StateUnion.Discrete != ctrl->TypeUnion.Discrete.Default)
               fprintf(out,"%s%s=%u\n", offSet[myoffset], CONTROL_GET, stateGet.StateUnion.Discrete);
         break;
      
      case SAHPI_CTRL_TYPE_ANALOG:
         fprintf(out, "%sTypeUnion.Analog={\n", offSet[myoffset]);
         myoffset++;
         fprintf(out,"%sMin=%d\n", offSet[myoffset], ctrl->TypeUnion.Analog.Min);
         fprintf(out,"%sMax=%d\n", offSet[myoffset], ctrl->TypeUnion.Analog.Max);
         fprintf(out,"%sDefault=%d\n", offSet[myoffset], ctrl->TypeUnion.Analog.Default);
         if (allowGet)
            if (stateGet.StateUnion.Analog != ctrl->TypeUnion.Analog.Default)
               fprintf(out,"%s%s=%u\n", offSet[myoffset], CONTROL_GET, stateGet.StateUnion.Analog);
         break;
      
      case SAHPI_CTRL_TYPE_STREAM:
         fprintf(out, "%sTypeUnion.Stream={\n", offSet[myoffset]);
         myoffset++;
         fprintf(out, "%sDefault={\n",offSet[myoffset]);
         myoffset++;
         print_control_state_stream(out, myoffset, ctrl->TypeUnion.Stream.Default);
         myoffset--;
         fprintf(out, "%s}\n", offSet[myoffset]);
         
         if (allowGet) {
            if (memcmp(&ctrl->TypeUnion.Stream.Default, 
                       &stateGet.StateUnion.Stream, sizeof(SaHpiCtrlStateStreamT))) {
               fprintf(out, "%s%s={\n",offSet[myoffset], CONTROL_GET);
               myoffset++;
               print_control_state_stream(out, myoffset, stateGet.StateUnion.Stream);
               myoffset--;
               fprintf(out, "%s}\n", offSet[myoffset]);
            }
         }
         break;
         
      case SAHPI_CTRL_TYPE_TEXT:
         fprintf(out, "%sTypeUnion.Text={\n", offSet[myoffset]);
         myoffset++;
         fprintf(out, "%sMaxChars=%u\n",offSet[myoffset], ctrl->TypeUnion.Text.MaxChars);
         fprintf(out, "%sMaxLines=%u\n",offSet[myoffset], ctrl->TypeUnion.Text.MaxLines);
         fprintf(out, "%sLanguage=%u\n",offSet[myoffset], ctrl->TypeUnion.Text.Language);
         fprintf(out, "%sDataType=%u\n",offSet[myoffset], ctrl->TypeUnion.Text.DataType);
         fprintf(out, "%sDefault={\n",offSet[myoffset]);
         myoffset++;
         print_control_state_text(out, myoffset, ctrl->TypeUnion.Text.Default);
         myoffset--;
         fprintf(out, "%s}\n", offSet[myoffset]);
         
         if(allowGet) {
            // To make sure we've got all lines, we call ControlGet a second time
            stateGet.StateUnion.Text.Line = SAHPI_TLN_ALL_LINES;
            rv = saHpiControlGet(sessionId, resId, ctrl->Num, &modeGet, &stateGet);
            if (rv != SA_OK) {
               fprintf(stderr, "ControlGet returns %s for ResId %u ControlNum %u\n",
                               oh_lookup_error(rv), resId, ctrl->Num);
            }
         
            if (memcmp(&ctrl->TypeUnion.Text.Default, 
                       &stateGet.StateUnion.Text, sizeof(SaHpiCtrlStateTextT))) {
               fprintf(out, "%s%s={\n",offSet[myoffset], CONTROL_GET);
               myoffset++;
               print_control_state_text(out, myoffset, stateGet.StateUnion.Text);
               myoffset--;
               fprintf(out, "%s}\n", offSet[myoffset]);      	
            }
         }
         break;
         
      case SAHPI_CTRL_TYPE_OEM:
         fprintf(out, "%sTypeUnion.Oem={\n",offSet[myoffset++]);
         fprintf(out, "%sMId=0x%08X\n", offSet[myoffset], ctrl->TypeUnion.Oem.MId);
         fprintf(out, "%sConfigData=\"", offSet[myoffset]);
         for (i = 0; i < SAHPI_CTRL_OEM_CONFIG_LENGTH; i++)
            fprintf(out,"%02X", ctrl->TypeUnion.Oem.ConfigData[i]);
         fprintf(out,"\"\n");
         fprintf(out, "%sDefault={\n",offSet[myoffset++]);
         print_control_state_oem(out, myoffset, ctrl->TypeUnion.Oem.Default);
         fprintf(out, "%s}\n", offSet[--myoffset]); 

         if(allowGet) {         
            if (memcmp(&ctrl->TypeUnion.Oem.Default, 
                       &stateGet.StateUnion.Oem, sizeof(SaHpiCtrlStateOemT))) {
               fprintf(out, "%s%s={\n",offSet[myoffset++], CONTROL_GET);
               print_control_state_oem(out, myoffset, stateGet.StateUnion.Oem);
               fprintf(out, "%s}\n", offSet[--myoffset]); 
            }
         }
         break;

      default:
         fprintf(out, "%sCTRL_TYPE_UNKNOWN={\n",offSet[myoffset++]);
   }
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sDefaultMode={\n", offSet[myoffset++]);
   fprintf(out, "%sMode=%u\n", offSet[myoffset], ctrl->DefaultMode.Mode);
   fprintf(out, "%sReadOnly=%d\n", offSet[myoffset], ctrl->DefaultMode.ReadOnly);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   if(allowGet) {
      // Check whether the mode was changed 
      if ( ctrl->DefaultMode.Mode != modeGet ) {
         fprintf(out, "%s%s={\n",offSet[myoffset++], CONTROL_GET);
         fprintf(out, "%sMode=%u\n", offSet[myoffset], modeGet);
         fprintf(out, "%s}\n", offSet[--myoffset]);
      }
   } 
   
   fprintf(out, "%sWriteOnly=%d\n", offSet[myoffset], ctrl->WriteOnly);
   fprintf(out, "%sOem=%d\n", offSet[myoffset], ctrl->Oem);

   return rv;	

}

static void print_control_state_stream(FILE *out, int offset, SaHpiCtrlStateStreamT data) {
   int i;

   fprintf(out,"%sRepeat=%d\n", offSet[offset], data.Repeat);
   fprintf(out,"%sStreamLength=%u\n", offSet[offset], data.StreamLength);
   fprintf(out,"%sStream=\"", offSet[offset]);
   for (i = 0; i < data.StreamLength; i++)
      fprintf(out,"%02X", data.Stream[i]);
   fprintf(out,"\"\n");
}

static void print_control_state_text(FILE *out, int offset, SaHpiCtrlStateTextT data) {
   int myoffset = offset;	

   fprintf(out, "%sLine=%u\n",offSet[myoffset], data.Line);
   fprintf(out, "%sText={\n",offSet[myoffset++]);
   print_textbuffer(out, myoffset, data.Text);
   fprintf(out, "%s}\n", offSet[--myoffset]);
}

static void print_control_state_oem(FILE *out, int offset, SaHpiCtrlStateOemT data) {
   int i;
   
   fprintf(out, "%sMId=0x%08X\n", offSet[offset], data.MId);
   fprintf(out, "%sBodyLength=%u\n",offSet[offset], data.BodyLength);
   fprintf(out, "%sBody=\"",offSet[offset]);
   for (i = 0; i < data.BodyLength; i++)
      fprintf(out,"%02X", data.Body[i]);
   fprintf(out,"\"\n");
}

/** 
 * Sensor functions
 **/ 
static void print_sensor_rdr(FILE *out, int offset, SaHpiSensorRecT *sens) {
   int myoffset = offset;
   
   fprintf(out, "%sNum=%u\n", offSet[myoffset], sens->Num);
   fprintf(out, "%sType=%u\n", offSet[myoffset], sens->Type);
   fprintf(out, "%sCategory=0x%02X\n", offSet[myoffset], sens->Category);
   fprintf(out, "%sEnableCtrl=%u\n", offSet[myoffset], sens->EnableCtrl);
   fprintf(out, "%sEventCtrl=%u\n", offSet[myoffset], sens->EventCtrl);
   fprintf(out, "%sEvents=0x%04X\n", offSet[myoffset], sens->Events);
   
   fprintf(out, "%sDataFormat={\n", offSet[myoffset++]);
   print_sensor_data_format(out, myoffset, sens->DataFormat);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sThresholdDefn={\n", offSet[myoffset++]);
   print_sensor_threshold_definition(out, myoffset, sens->ThresholdDefn);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   
   fprintf(out, "%sOem=%u\n", offSet[myoffset], sens->Oem);
}

static void print_sensor_threshold_definition(FILE *out, int offset, SaHpiSensorThdDefnT def) {
   int myoffset = offset;
   
   fprintf(out, "%sIsAccessible=%d\n", offSet[myoffset], def.IsAccessible);
   if (def.IsAccessible) {
      fprintf(out, "%sReadThold=0x%02X\n", offSet[myoffset], def.ReadThold);
      fprintf(out, "%sWriteThold=0x%02X\n", offSet[myoffset], def.WriteThold);
      fprintf(out, "%sNonlinear=%d\n", offSet[myoffset], def.Nonlinear);
   }
}
 
static void print_sensor_data_format(FILE *out, int offset, SaHpiSensorDataFormatT data) {
   int myoffset = offset;
   
   fprintf(out, "%sIsSupported=%d\n", offSet[myoffset], data.IsSupported);
   fprintf(out, "%sReadingType=%u\n", offSet[myoffset], data.ReadingType);
   fprintf(out, "%sBaseUnits=%u\n", offSet[myoffset], data.BaseUnits);
   fprintf(out, "%sModifierUnits=%u\n", offSet[myoffset], data.ModifierUnits);
   fprintf(out, "%sModifierUse=%u\n", offSet[myoffset], data.ModifierUse);
   fprintf(out, "%sPercentage=%u\n", offSet[myoffset], data.Percentage);
   fprintf(out, "%sRange={\n", offSet[myoffset++]);
   fprintf(out, "%sFlags=0x%02X\n", offSet[myoffset], data.Range.Flags);
   fprintf(out, "%sMax={\n", offSet[myoffset++]);
   print_sensor_reading(out, myoffset, data.Range.Max);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sMin={\n", offSet[myoffset++]);
   print_sensor_reading(out, myoffset, data.Range.Min);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sNominal={\n", offSet[myoffset++]);
   print_sensor_reading(out, myoffset, data.Range.Nominal);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sNormalMax={\n", offSet[myoffset++]);
   print_sensor_reading(out, myoffset, data.Range.NormalMax);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%sNormalMin={\n", offSet[myoffset++]);
   print_sensor_reading(out, myoffset, data.Range.NormalMin);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   fprintf(out, "%s}\n", offSet[--myoffset]);   // End of Range
   fprintf(out, "%sAccuracyFactor=%lf\n", offSet[myoffset], data.AccuracyFactor);
   
}

// READING
static SaErrorT print_sensor_data(FILE *out, int offset, SaHpiSessionIdT sessionId, 
                                   SaHpiResourceIdT resId, SaHpiSensorRecT *sens) {
   int rv = SA_OK;
   int myoffset = offset;
   int corflag = 0;
   
   SaHpiSensorReadingT read;
   SaHpiEventStateT    event;
   SaHpiBoolT          sensEnabled;
   
   rv = saHpiSensorEnableGet(sessionId, resId, sens->Num, &sensEnabled);
   if (rv != SA_OK) {
      fprintf(stderr, "saHpiSensorEnableGet returns %s for ResId %u sensNum %u\n",
              oh_lookup_error(rv), resId, sens->Num);
      return rv;
   }
   
   if (!sensEnabled) {
   	  // Check whether it is temporarily not enabled
   	  if (sens->EnableCtrl) {
   	     rv = saHpiSensorEnableSet(sessionId, resId, sens->Num, SAHPI_TRUE);
         if (rv != SA_OK) {
            fprintf(stderr, "saHpiSensorEnableSet returns %s for ResId %u sensNum %u\n",
                    oh_lookup_error(rv), resId, sens->Num);
            return rv;
         }
   	  } else {
   	  	// Sensor isn't enabled and the state couldn't be changed -> forget DATA
   	     return SA_OK;
   	  }
   }
   
   rv = saHpiSensorReadingGet(sessionId, resId, sens->Num, &read, &event);
   if (rv != SA_OK) {
      fprintf(stderr, "SensorReadingGet returns %s for ResId %u sensNum %u\n",
              oh_lookup_error(rv), resId, sens->Num);
      return rv;
   }
   
   fprintf(out,"%s%s {\n",offSet[myoffset++],SENSOR_DATA);
   
   // SensorEnabled
   fprintf(out,"%sSensorEnable=%d\n",offSet[myoffset], sensEnabled);
   
   // SensorEventEnable
   SaHpiBoolT evtenb;
   rv = saHpiSensorEventEnableGet(sessionId, resId, sens->Num, &evtenb);
   if (rv != SA_OK) {
      fprintf(stderr, "SensorEventEnableGet returns %s for ResId %u sensNum %u\n",
              oh_lookup_error(rv), resId, sens->Num);
   } else {
      fprintf(out,"%sSensorEventEnable=%d\n",offSet[myoffset], evtenb);
   }
   
   // SensorReading
   fprintf(out,"%sEventState=0x%04X\n", offSet[myoffset], event);
   fprintf(out,"%sSensorReading={\n",offSet[myoffset++]);
   if ((sens->DataFormat.IsSupported == SAHPI_TRUE) && 
        (read.IsSupported == SAHPI_FALSE)) {
      read.IsSupported = SAHPI_TRUE;
      read.Type = sens->DataFormat.ReadingType;
      fprintf(stderr, 
            "WARNING: SensorReading.IsSupported value must be changed for ResId %u sensNum %u\n",
            resId, sens->Num);  
   }
   print_sensor_reading(out, myoffset, read);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   
   // SensorThresholds
   if (sens->ThresholdDefn.IsAccessible) {
   	  SaHpiSensorThresholdsT thres;
   	  
      rv = saHpiSensorThresholdsGet(sessionId, resId, sens->Num, &thres);
      if (rv != SA_OK) {
         fprintf(stderr, "saHpiSensorThresholdsGet returns %s for ResId %u sensNum %u\n",
                 oh_lookup_error(rv), resId, sens->Num);
      } else {
         fprintf(out,"%sSensorThresholds={\n",offSet[myoffset++]);
         corflag = print_sensor_thresholds(out, myoffset, thres, sens->DataFormat);
         fprintf(out, "%s}\n", offSet[--myoffset]);
         if (corflag)
            fprintf(stderr, 
            "WARNING: Reading for %d threshold value(s) must be changed for ResId %u sensNum %u\n",
            corflag, resId, sens->Num);
      }
   }
   
   // SensorEventMasks
   SaHpiEventStateT assert, deassert;
   rv = saHpiSensorEventMasksGet(sessionId, resId, sens->Num, &assert, &deassert);
   if (rv != SA_OK) {
      fprintf(stderr, "saHpiSensorEventMasksGet returns %s for ResId %u sensNum %u\n",
              oh_lookup_error(rv), resId, sens->Num);
   } else {
      fprintf(out,"%sAssertEventMask=0x%04X\n",offSet[myoffset], assert);
      fprintf(out,"%sDeassertEventMask=0x%04X\n",offSet[myoffset], deassert);
   }

   fprintf(out, "%s}\n", offSet[--myoffset]);  // SENSOR_DATA
   if (!sensEnabled) {
      // The enabled status was changed, set it back
      rv = saHpiSensorEnableSet(sessionId, resId, sens->Num, sensEnabled);
      if (rv != SA_OK) {
         fprintf(stderr, "saHpiSensorEnableSet returns %s for ResId %u sensNum %u\n",
                 oh_lookup_error(rv), resId, sens->Num);
         return rv;
      }
   }
   
   return rv;                                   	
}

static int check_sensor_reading(SaHpiSensorReadingT *read, 
                                  SaHpiSensorDataFormatT format) {

   if (format.IsSupported) {
     if (read->IsSupported == SAHPI_TRUE) {
   	    if (read->Type != format.ReadingType) {
   	    	  read->Type = format.ReadingType;
          return 1;
   	    }
     }
   }
   
   return 0;                 	
}
 
static int print_sensor_thresholds(FILE *out, int offset, SaHpiSensorThresholdsT thres,
                                     SaHpiSensorDataFormatT format) {
   int myoffset = offset;
   int check_val = 0;
   
   fprintf(out, "%sLowCritical={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.LowCritical, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.LowCritical);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sLowMajor={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.LowMajor, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.LowMajor);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sLowMinor={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.LowMinor, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.LowMinor);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sUpCritical={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.UpCritical, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.UpCritical);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sUpMajor={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.UpMajor, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.UpMajor);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sUpMinor={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.UpMinor, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.UpMinor);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sPosThdHysteresis={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.PosThdHysteresis, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.PosThdHysteresis);
   fprintf(out, "%s}\n", offSet[--myoffset]);

   fprintf(out, "%sNegThdHysteresis={\n", offSet[myoffset++]);
   if (check_sensor_reading(&thres.NegThdHysteresis, format)) check_val++;
   print_sensor_reading(out, myoffset, thres.NegThdHysteresis);
   fprintf(out, "%s}\n", offSet[--myoffset]);
   
   return check_val;
}


static void print_sensor_reading(FILE *out, int offset, SaHpiSensorReadingT val) {
   int myoffset = offset;
   int i;

   fprintf(out, "%sIsSupported=%u\n", offSet[myoffset], val.IsSupported);
   if (val.IsSupported) {
      fprintf(out, "%sType=%u\n",offSet[myoffset], val.Type);
      switch( val.Type ) {
         case SAHPI_SENSOR_READING_TYPE_INT64:
            fprintf(out, "%svalue.SensorInt64=%" PRId64 "\n", offSet[myoffset], 
                          (int64_t) val.Value.SensorInt64);
            break;
         case SAHPI_SENSOR_READING_TYPE_UINT64:
            fprintf(out, "%svalue.SensorUint64=%" PRIu64 "\n", offSet[myoffset], 
                          (uint64_t) val.Value.SensorUint64);
            break;
         case SAHPI_SENSOR_READING_TYPE_FLOAT64:
            fprintf(out, "%svalue.SensorFloat64=%lf\n", offSet[myoffset], val.Value.SensorFloat64);
            break;
         case SAHPI_SENSOR_READING_TYPE_BUFFER:
            fprintf(out, "%svalue.SensorBuffer=\"", offSet[myoffset]);
            for (i = 0; i < SAHPI_SENSOR_BUFFER_LENGTH; i++) 
               fprintf(out, "%02X", val.Value.SensorBuffer[i]);
            fprintf(out, "\"\n");
            break;
      	 default:
      	    fprintf(out, "%sERROR MISSING_READING_TYPE\n", offSet[myoffset]);
      }
   }
}


/**
 * Common data structure functions
 **/
 
static void print_textbuffer(FILE *out, int offset, SaHpiTextBufferT data) {
	fprintf(out,"%sDataType=%u\n",offSet[offset], data.DataType);
	fprintf(out,"%sLanguage=%u\n",offSet[offset], data.Language);
	fprintf(out,"%sDataLength=%u\n",offSet[offset], data.DataLength);
	fprintf(out,"%sData=\"",offSet[offset]);
	switch(data.DataType) {
		case SAHPI_TL_TYPE_UNICODE:
			// TODO
			fprintf(out, "UNICODE DATA TYPE NOT SUPPORTED");
			break;
		case SAHPI_TL_TYPE_BCDPLUS:
		case SAHPI_TL_TYPE_ASCII6:
		case SAHPI_TL_TYPE_TEXT:
			fwrite(data.Data, data.DataLength, 1, out);
			break;
		case SAHPI_TL_TYPE_BINARY:
			// TODO
			fprintf(out, "BINARY DATA TYPE NOT SUPPORTED");
			break;
		default:
			fprintf(out, "UNSUPPORTED DATA TYPE");
			break;
	}
	fprintf(out,"\"\n");

}

/**
 * print_ep:
 * @ep: Pointer to entity path stucture.
 *
 * Prints the string form of an entity path structure.
 *
 * Returns:
 * SA_OK - normal operations.
 * SA_ERR_HPI_INVALID_PARAMS - @ep is NULL.
 * SA_ERR_HPI_INVALID_DATA - Location value too big for OpenHpi.
 * SA_ERR_HPI_OUT_OF_SPACE - No memory for internal storage.
 **/
SaErrorT print_ep(FILE *out, int offset, const SaHpiEntityPathT *ep) {
   oh_big_textbuffer bigbuf1, bigbuf2;
   SaErrorT err;
	
   if (!ep) {
      fprintf(stderr, "Invalid parameter for entity path.");
      return(SA_ERR_HPI_INVALID_PARAMS);
   }

   err = oh_init_bigtext(&bigbuf1);
   if (err) return(err);
   err = oh_init_bigtext(&bigbuf2);
   if (err) return(err);

   err = oh_append_bigtext(&bigbuf1, offSet[offset]);
   if (err) return(err);
   err = oh_append_bigtext(&bigbuf1, "\"");
   if (err) return(err);
	
   err = oh_decode_entitypath(ep, &bigbuf2);
   if (err) return(err);

   err = oh_append_bigtext(&bigbuf1, (char *)bigbuf2.Data);
   if (err) return(err);
   err = oh_append_bigtext(&bigbuf1, "\"\n");
   if (err) return(err);

   fprintf(out, "%s", bigbuf1.Data);

   return(SA_OK);
}

/** an oh_encode function for entitypath still exist, using the original functions instead
 * of the following
 * 
static void print_entitypath(FILE *out, int offset, const SaHpiEntityPathT *ep) {
	int i=0, lastentry = FALSE;
	int myoffset=offset;
	myoffset++;
	while ((i < SAHPI_MAX_ENTITY_PATH) && (lastentry == FALSE)) {
		if (ep->Entry[i].EntityType == SAHPI_ENT_ROOT) lastentry = TRUE;
		fprintf(out,"%s{\n",offSet[offset]);
		fprintf(out,"%sEntityType=%d\n",offSet[myoffset], ep->Entry[i].EntityType);
		fprintf(out,"%sEntityLocation=%d\n",offSet[myoffset], ep->Entry[i].EntityLocation);
		fprintf(out,"%s}\n",offSet[offset]);
		i++;
	}
}
**/



