/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2004
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
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
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Log: 
 *     Copied from hpifru.c and modified for general use
 *
 * Changes:
 *     11/03/2004  kouzmich   Fixed Bug #1057934
 *     09/02/2010  lwetzel    Fixed Bug ResourceId 255 (0xFF) is a valid ResourceId 
 *     07/06/2010  ulikleber  New option -D to select domain
 *     20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

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

static 
void sensor_readingthreshold(SaHpiSessionIdT sessionid,
			SaHpiResourceIdT resourceid,
			SaHpiRdrT *rdrptr);


/* 
 * Globals for this driver
 */
static gboolean f_listall  = FALSE;
static gboolean f_rpt      = FALSE;
static gboolean f_rdr      = FALSE;
static gboolean f_inv      = FALSE;
static gboolean f_sensor   = FALSE;
static gboolean f_wdog     = FALSE;
//static gboolean f_dimi     = FALSE;   why is this implemented only in hpitop?
//static gboolean f_fumi     = FALSE;   why is this implemented only in hpitop?
static gboolean f_ann      = FALSE;
static gboolean f_ctrl     = FALSE;
static gboolean f_overview = FALSE;
static gboolean f_allres   = TRUE;
static gint resourceid = 0;   
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "all",          'a', 0, G_OPTION_ARG_NONE, &f_listall,  "Display all rpts and rdrs (default)",       NULL },
  { "controls",     'c', 0, G_OPTION_ARG_NONE, &f_ctrl,     "Display only controls",                     NULL },
  { "rdrs",         'd', 0, G_OPTION_ARG_NONE, &f_rdr,      "Display rdr records",                       NULL },
  { "inventories",  'i', 0, G_OPTION_ARG_NONE, &f_inv,      "Display only inventories",                  NULL },
  { "rpts",         'r', 0, G_OPTION_ARG_NONE, &f_rpt,      "Display only rpts",                         NULL },
  { "sensors",      's', 0, G_OPTION_ARG_NONE, &f_sensor,   "Display only sensors",                      NULL },
  { "watchdogs",    'w', 0, G_OPTION_ARG_NONE, &f_wdog,     "Display only watchdogs",                    NULL },
  { "overview",     'o', 0, G_OPTION_ARG_NONE, &f_overview, "Display system overview: rpt & rdr headers",NULL },

//  { "fumis",        'f', 0, G_OPTION_ARG_NONE, &f_fumi,     "Display only fumis",                        NULL },
//  { "dimis",        'd', 0, G_OPTION_ARG_NONE, &f_dimi,     "Display only dimis",                        NULL },
//  { "annunciators", 'a', 0, G_OPTION_ARG_NONE, &f_ann,      "Display only annunciators",                 NULL },
  // Annunciators not implemented

  { "resource",     'n', 0, G_OPTION_ARG_INT,  &resourceid, "Select particular resource id to display\n\t\t\t"
                                                            "(Used with [-cdirs] options)",              NULL },
  { NULL }
};


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
	SaErrorT 	rv = SA_OK;
	SaHpiSessionIdT sessionid;
//	SaHpiResourceIdT resourceid = 0;
        GError *error = NULL;
        GOptionContext *context;
	    
	oh_prog_version(argv[0], OH_SVN_REV);

        context = g_option_context_new ("- Display system topology");
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION //TODO: Feature 880127?
                    - OHC_VERBOSE_OPTION,    // no verbose mode implemented
                error)) { 
                g_print ("option parsing failed: %s\n", error->message);
		exit(1);
	}
 
	if (f_rpt || f_sensor || f_inv || f_ctrl || f_rdr || f_wdog) 
           f_listall = TRUE;

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) exit(-1);

	/*
	 * Resource discovery
	 */
	if (copt.debug) printf("saHpiDiscover\n");
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
		
		if (copt.debug) printf("saHpiRptEntryGet\n");
		rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if ((rvRptGet != SA_OK) || copt.debug) 
		       	printf("RptEntryGet returns %s\n",oh_lookup_error(rvRptGet));
		
		rv = list_rpt(&rptentry, resourceid);
						
		if (rvRptGet == SA_OK 
                   	&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
			&& ((f_allres) || (resourceid == rptentry.ResourceId)))
		{
			l_resourceid = rptentry.ResourceId;
			if (!f_allres) 
				 nextrptentryid = SAHPI_LAST_ENTRY;

			/* walk the RDR list for this RPT entry */
			entryid = SAHPI_FIRST_ENTRY;			

			if (copt.debug) printf("rptentry[%u] resourceid=%u\n", entryid,resourceid);

			do {
				rvRdrGet = saHpiRdrGet(sessionid,l_resourceid, entryid,&nextentryid, &rdr);
				if (copt.debug) printf("saHpiRdrGet[%u] rv = %s\n",entryid,oh_lookup_error(rvRdrGet));


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

	if ((f_allres) ||
		(resourceid == rptptr->ResourceId)) {

		/* Always print resource header */
		if (!f_listall && !f_rpt) 
		printf("\nRPT Tag: %s, ResourceId %u\n",rptptr->ResourceTag.Data, rptptr->ResourceId);
		oh_print_ep(&rptptr->ResourceEntity, 0);
		printf("\n");

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
			printf("saHpiIdrInfoGet: ResourceId %u IdrId %u, error %s\n",
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
			countFields = 0;	/* Bug #1057934 */
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
 
				if (copt.debug) printf("saHpiIdrFieldGet  error %s\n",oh_lookup_error(rvField));
				fieldId = nextFieldId;
			} while ((rvField == SA_OK) && (fieldId != SAHPI_LAST_ENTRY));
		
			if ( countFields != areaHeader.NumFields) 
				printf("Area Header error! areaHeader.NumFields %u, countFields %u\n",
					areaHeader.NumFields, countFields);
		} else {
			printf("saHpiIdrAreaHeaderGet error %s\n",oh_lookup_error(rv));
		}
		areaId = nextareaId;
	
	} while ((rv == SA_OK) && (areaId != SAHPI_LAST_ENTRY)); 

	if ((rv == SA_OK) && (countAreas != numAreas)) 
		printf("idrInfo error! idrInfo.NumAreas = %u; countAreas = %u\n", 
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
					
		if (&rdrptr->RdrTypeUnion.SensorRec == 0) printf("ERROR! Sensor pointer NULL\n");
		
		printf("  Sensor Name: ");
		rv = oh_print_text(&(rdrptr->IdString));
		if (rv) printf("oh_print_text Error=%s\n", oh_lookup_error(rv));
		
		rv = oh_print_ep(&(rdrptr->Entity), 4);
		if (rv) printf("oh_print_ep Error=%s\n", oh_lookup_error(rv));

		rv = oh_print_sensorrec(&rdrptr->RdrTypeUnion.SensorRec, 4);
		if (rv) printf("oh_print_sensorrec Error=%s\n", oh_lookup_error(rv));
		
		sensor_readingthreshold(sessionid,
					l_resourceid,
					rdrptr);
		
	} 
	
	return(rv);
}

/* 
 * This routine get sensor reading and threshold, and display them.
 *
**/
static 
void sensor_readingthreshold(SaHpiSessionIdT sessionid,
			SaHpiResourceIdT resourceid,
			SaHpiRdrT *rdrptr)
{

	SaHpiSensorRecT *sensorrec;
        SaHpiSensorNumT sensornum;
        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thresh;
        SaHpiEventStateT events;
        SaHpiTextBufferT text;
        SaErrorT rv;
        
	
	sensorrec = &rdrptr->RdrTypeUnion.SensorRec;
        sensornum = sensorrec->Num;
        rv = saHpiSensorReadingGet(sessionid,resourceid, sensornum, &reading, &events);
        if (rv != SA_OK)  {
                printf("\nReadingGet ret=%s\n", oh_lookup_error(rv));
                return;
        }
        
        if((rv = oh_decode_eventstate(events, sensorrec->Category, &text)) == SA_OK) {
                printf("\t  Sensor %u state = %s\n", sensornum, text.Data);
        } else {
                printf("\n\t  Sensor %u state FAILED, %s\n", sensornum, oh_lookup_error(rv));
        }
        
        if (!reading.IsSupported ) {
                printf("\t  Reading not supported for sensor %u!\n\n", sensornum);
                return;
        } 
        
        if((rv = oh_decode_sensorreading(reading, sensorrec->DataFormat, &text)) == SA_OK) {
                printf("\t  Sensor %u reading = %s\n", sensornum, text.Data);
        } else {
                printf("\n\t  Sensor %u reading FAILED, %s\n", sensornum, oh_lookup_error(rv));
        }
	
	rv = saHpiSensorThresholdsGet(sessionid,resourceid, sensornum, &thresh);
	if (rv != SA_OK)  {
		if (rv == SA_ERR_HPI_INVALID_CMD) {
			printf("\t  Readable thresholds not supported\n\n");
			return;
		}
		else {
			printf("\t  ThresholdsGet ret=%s\n\n", oh_lookup_error(rv));
			return;
		}
	}
	printf( "\t    Thresholds:\n" );

	if (thresh.LowCritical.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.LowCritical, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tLow Critical Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tLow Critical Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.LowMajor.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.LowMajor, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tLow Major Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tLow Major Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.LowMinor.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.LowMinor, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tLow Minor Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tLow Minor Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}

	if (thresh.UpCritical.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.UpCritical, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tUp Critical Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tUp Critical Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.UpMajor.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.UpMajor, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tUp Major Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tUp Major Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.UpMinor.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.UpMinor, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tUp Minor Threshold: %s\n", text.Data);
		} else {
			printf( "\t\tUp Minor Threshold: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.PosThdHysteresis.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.PosThdHysteresis, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tPos Threshold Hysteresis: %s\n", text.Data);
		} else {
			printf( "\t\tPos Threshold Hysteresis: FAILED %s\n", oh_lookup_error(rv));
		}
	}
	
	if (thresh.NegThdHysteresis.IsSupported) {
		if((rv = oh_decode_sensorreading(thresh.NegThdHysteresis, 
				sensorrec->DataFormat, &text)) == SA_OK) {
			printf( "\t\tNeg Threshold Hysteresis: %s\n", text.Data);
		} else {
			printf( "\t\tNeg Threshold Hysteresis: FAILED %s\n", oh_lookup_error(rv));
		}
	}

	printf("\n\n\n");
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
		snprintf((char *)working.Data, SAHPI_MAX_TEXT_BUFFER_LENGTH,
			"    Found %s, RecordId %u",
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
