/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 * (C) Copyright IBM Corp. 2004, 2005
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
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Log: 
 *	Start from hpitree.c 
 *	This routine display highlevel topology for a managed openHPI complex
 *
 * Changes:
 *    09/02/2010  lwetzel    Fixed Bug: ResourceId 255 (0xFF) is a valid ResourceId 
 *    07/06/2010  ulikleber  New option -D to select domain
 *    20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

/* 
 * Function prototypes
 */
static SaErrorT list_resources(SaHpiSessionIdT sessionid, SaHpiResourceIdT res_id);
static SaErrorT list_rpt(SaHpiRptEntryT *rptptr);
static SaErrorT list_rdr(SaHpiSessionIdT sessionid, 
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_rpt(SaHpiRptEntryT *rptptr,SaHpiResourceIdT resourceid);

static SaErrorT show_rdr(oh_big_textbuffer *buffer);

static SaErrorT show_inv(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_sens(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_ann(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_ctrl(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_wdog(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);
			
static SaErrorT show_fumi(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

static SaErrorT show_dimi(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid);

void same_system(oh_big_textbuffer *bigbuf);
void show_trailer(char *system);
						
#define EPATHSTRING_END_DELIMITER "}"
#define rpt_startblock "    |\n    +--- "
#define rdr_startblock "    |    |__ "

/* 
 * Globals for this driver
 */
static gboolean f_rpt      = FALSE;
static gboolean f_inv      = FALSE;
static gboolean f_sensor   = FALSE;
static gboolean f_wdog     = FALSE;
static gboolean f_dimi     = FALSE;
static gboolean f_fumi     = FALSE;
static gboolean f_ann      = FALSE;
static gboolean f_ctrl     = FALSE;
static gboolean f_overview = TRUE;
static gboolean f_allres   = TRUE;
static gint resourceid = 0;
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "rpts",         'r', 0, G_OPTION_ARG_NONE, &f_rpt,      "Display only rpts",                         NULL },
  { "sensors",      's', 0, G_OPTION_ARG_NONE, &f_sensor,   "Display only sensors",                      NULL },
  { "controls",     'c', 0, G_OPTION_ARG_NONE, &f_ctrl,     "Display only controls",                     NULL },
  { "watchdogs",    'w', 0, G_OPTION_ARG_NONE, &f_wdog,     "Display only watchdogs",                    NULL },
  { "inventories",  'i', 0, G_OPTION_ARG_NONE, &f_inv,      "Display only inventories",                  NULL },
  { "annunciators", 'a', 0, G_OPTION_ARG_NONE, &f_ann,      "Display only annunciators",                 NULL },
  { "fumis",        'f', 0, G_OPTION_ARG_NONE, &f_fumi,     "Display only fumis",                        NULL },
  { "dimis",        'd', 0, G_OPTION_ARG_NONE, &f_dimi,     "Display only dimis",                        NULL },
  { "resource",     'n', 0, G_OPTION_ARG_INT,  &resourceid, "Display only resource nn and its topology", "nn" },
  { NULL }
};


char previous_system[SAHPI_MAX_TEXT_BUFFER_LENGTH] = "";

/* 
 * Main                
 */
int
main(int argc, char **argv)
{
        SaErrorT rv = SA_OK;
	SaHpiSessionIdT sessionid;
        GOptionContext *context;

        /* Print version strings */
	oh_prog_version(argv[0]);

        /* Parsing options */
        static char usetext[]="- Display system topology\n  "
                              OH_SVN_REV; 
        OHC_PREPARE_REVISION(usetext);
        context = g_option_context_new (usetext);
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION  //TODO: Feature 880127
                    - OHC_VERBOSE_OPTION )) { // no verbose mode implemented
                g_option_context_free (context);
		return 1;
	}
        g_option_context_free (context);

        if (f_rpt || f_sensor || f_ctrl || f_wdog || f_inv || 
                     f_ann || f_fumi || f_dimi || resourceid>0)
            f_overview = FALSE;

	memset (previous_system, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);

        rv = ohc_session_open_by_option ( &copt, &sessionid);
	if (rv != SA_OK) return rv;

	/*
	 * Resource discovery
	 */
	if (copt.debug) DBG("saHpiDiscover");
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) {
		CRIT("saHpiDiscover returns %s",oh_lookup_error(rv));
		return rv;
	}

	if (copt.debug)  DBG("Discovery done");
	list_resources(sessionid, resourceid);

	rv = saHpiSessionClose(sessionid);
	
	return 0;
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
		
		if (copt.debug) DBG("saHpiRptEntryGet");
		rvRptGet = saHpiRptEntryGet(sessionid,rptentryid,&nextrptentryid,&rptentry);
		if ((rvRptGet != SA_OK) || copt.debug) 
		       	DBG("RptEntryGet returns %s",oh_lookup_error(rvRptGet));
		
		rv = show_rpt(&rptentry, resourceid);
						
		if (rvRptGet == SA_OK 
                   	&& (rptentry.ResourceCapabilities & SAHPI_CAPABILITY_RDR) 
			&& ((f_allres) || (resourceid == rptentry.ResourceId)))
		{
			l_resourceid = rptentry.ResourceId;
			if (!f_allres) 
				 nextrptentryid = SAHPI_LAST_ENTRY;

			/* walk the RDR list for this RPT entry */
			entryid = SAHPI_FIRST_ENTRY;			

			if (copt.debug) DBG("rptentry[%u] resourceid=%u", entryid,resourceid);

			do {
				rvRdrGet = saHpiRdrGet(sessionid,l_resourceid, entryid,&nextentryid, &rdr);
				if (copt.debug) DBG("saHpiRdrGet[%u] rv = %s",entryid,oh_lookup_error(rvRdrGet));


				if (rvRdrGet == SA_OK)
				{
					// Add zero terminator to RDR Id String
					SaHpiUint32T last = rdr.IdString.DataLength;
					if ( last >= SAHPI_MAX_TEXT_BUFFER_LENGTH ) {
						last = SAHPI_MAX_TEXT_BUFFER_LENGTH - 1;
					}
					rdr.IdString.Data[last] = '\0';

					if (f_overview) list_rdr(sessionid, &rptentry, &rdr, l_resourceid);
					if (f_inv) show_inv(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_sensor) show_sens(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_ctrl) show_ctrl(sessionid, &rptentry, &rdr, l_resourceid); 
					if (f_wdog) show_wdog(sessionid, &rptentry, &rdr, l_resourceid);
					if (f_fumi) show_fumi(sessionid, &rptentry, &rdr, l_resourceid);
					if (f_dimi) show_dimi(sessionid, &rptentry, &rdr, l_resourceid);
					if (f_ann) show_ann(sessionid, &rptentry, &rdr, l_resourceid); 						
				}
				entryid = nextentryid;
			} while ((rvRdrGet == SA_OK) && (entryid != SAHPI_LAST_ENTRY)) ;
		}
		rptentryid = nextrptentryid;
	} while ((rvRptGet == SA_OK) && (rptentryid != SAHPI_LAST_ENTRY));
	
	show_trailer(previous_system);
	
	return(rv);
}


/*
 *
 */
void same_system(oh_big_textbuffer *bigbuf)
{

	int size = strcspn((char *)bigbuf->Data, EPATHSTRING_END_DELIMITER);
	int old_size = strcspn(previous_system, EPATHSTRING_END_DELIMITER); 
	if  ( (old_size != size) || 
			(strncmp((char *)bigbuf->Data, previous_system, size+1) != 0)) {
		if (previous_system[0] == '{') show_trailer(previous_system);
		memset (previous_system, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH); 
		strncpy (previous_system, (char *)(bigbuf->Data), size+1);
		previous_system[size+2] = '\0';
		printf("\n\n%s\n", previous_system);
	} 

}


/*
 *
 */
void show_trailer(char *system)
{
	printf("    |\nEnd of %s\n\n", system);

}
/*
 *
 */
static 
SaErrorT show_rpt(SaHpiRptEntryT *rptptr,SaHpiResourceIdT resourceid)
{
	SaErrorT rv = SA_OK, err = SA_OK;
	oh_big_textbuffer bigbuf1, bigbuf2;
	SaHpiTextBufferT  textbuffer;
	
	
	err = oh_init_textbuffer(&textbuffer);
	if (err) return(err);
	err = oh_init_bigtext(&bigbuf1);
	if (err) return(err);
	err = oh_init_bigtext(&bigbuf2);
	if (err) return(err);

		
	if ((f_allres) ||
		(resourceid == rptptr->ResourceId)) {

		rv  = oh_decode_entitypath(&rptptr->ResourceEntity, &bigbuf2);
		same_system(&bigbuf2);
		/* printf("ResourceId: %u", rptptr->ResourceId);  */
		err = oh_append_bigtext(&bigbuf1, rpt_startblock);
		
		err = oh_append_bigtext(&bigbuf1, (char *)bigbuf2.Data);
		if (err) return(err);
		err = oh_append_bigtext(&bigbuf1, "\n");
		if (err) return(err);
		
		printf("%s", bigbuf1.Data);
										
		if (f_rpt) list_rpt(rptptr);
	}
	return(rv);
}


/*
 *
 */
static 
SaErrorT list_rpt(SaHpiRptEntryT *rptptr)
{
	SaErrorT rv = SA_OK, err = SA_OK;
	oh_big_textbuffer bigbuf1;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiTextBufferT  textbuffer;
	
	
	err = oh_init_textbuffer(&textbuffer);				
	if (err) return(err);
	err = oh_init_bigtext(&bigbuf1);
	if (err) return(err);

	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "    |    ResourceId:  %u\n", rptptr->ResourceId);
	oh_append_bigtext(&bigbuf1, str);
	
	oh_decode_capabilities(rptptr->ResourceCapabilities, &textbuffer);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "    |    Capability:  %s\n",(char *)textbuffer.Data);
	oh_append_bigtext(&bigbuf1, str);

	if ( rptptr->ResourceTag.DataType == SAHPI_TL_TYPE_TEXT ) {
	   oh_append_bigtext(&bigbuf1, "    |    Tag:  ");
	   snprintf(str,  rptptr->ResourceTag.DataLength+1, "%s", rptptr->ResourceTag.Data);
	   oh_append_bigtext(&bigbuf1, str);
	   oh_append_bigtext(&bigbuf1, "\n");
	}
	
	printf("%s", bigbuf1.Data);

	return(rv);
}

/* 
 *
 */
static 
SaErrorT show_inv(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
 	SaErrorT  rvInvent = SA_OK;
	SaHpiIdrInfoT	idrInfo;
	SaHpiIdrIdT	idrid;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rvInvent = oh_init_bigtext(&buffer);
									

	if (rdrptr->RdrType == SAHPI_INVENTORY_RDR) {
		idrid = rdrptr->RdrTypeUnion.InventoryRec.IdrId;
		rvInvent = saHpiIdrInfoGet(
					sessionid,
					l_resourceid,
					idrid,
					&idrInfo);		

		if (rvInvent !=SA_OK) {
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Inventory: ResourceId %u IdrId %u, error %s\n",
					l_resourceid, idrid, oh_lookup_error(rvInvent));
			oh_append_bigtext(&buffer, str);
			show_rdr(&buffer);			
		} else {

			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Inventory Num: %u, ", rdrptr->RdrTypeUnion.InventoryRec.IdrId);
			oh_append_bigtext(&buffer, str);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Num Areas: %u, ", idrInfo.NumAreas);
			oh_append_bigtext(&buffer, str);
			if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
			   oh_append_bigtext(&buffer, "Tag: ");
			   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", rdrptr->IdString.Data);
			   oh_append_bigtext(&buffer, str);
			   oh_append_bigtext(&buffer, "\n");
			}
			show_rdr(&buffer);
		}
	}
	return(rvInvent);

}

/* 
 *
 */
static 
SaErrorT show_sens(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv  = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);

	if (rdrptr->RdrType == SAHPI_SENSOR_RDR) {							
		/* Sensor Num */
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Sensor Num: %u, ", rdrptr->RdrTypeUnion.SensorRec.Num);
		oh_append_bigtext(&buffer, str);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s, ", 
		 	oh_lookup_sensortype(rdrptr->RdrTypeUnion.SensorRec.Type));
		oh_append_bigtext(&buffer, str);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Category: %s, ", 
		 	oh_lookup_eventcategory(rdrptr->RdrTypeUnion.SensorRec.Category));
		oh_append_bigtext(&buffer, str);
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
	} 
	
	return(rv);
}


/* 
 *
 */
static 
SaErrorT show_rdr(oh_big_textbuffer *buffer)
{
	
	SaErrorT rv = SA_OK;
	oh_big_textbuffer buffer1;
	rv = oh_init_bigtext(&buffer1);

	oh_append_bigtext(&buffer1, rdr_startblock);
	oh_append_bigtext(&buffer1, (char *) buffer->Data);		
	printf("%s", buffer1.Data);

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
	SaErrorT rv = SA_OK;
	show_inv(sessionid, rptptr, rdrptr, l_resourceid); 
	show_sens(sessionid, rptptr, rdrptr, l_resourceid); 
	show_ctrl(sessionid, rptptr, rdrptr,  l_resourceid); 
	show_wdog(sessionid, rptptr, rdrptr,  l_resourceid); 
	show_ann(sessionid, rptptr, rdrptr,  l_resourceid);
	show_dimi(sessionid, rptptr, rdrptr,  l_resourceid);
	show_fumi(sessionid, rptptr, rdrptr,  l_resourceid);
	return(rv);
}


/* 
 *
 */
static 
SaErrorT show_ctrl(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);
																
	if (rdrptr->RdrType == SAHPI_CTRL_RDR){
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Control Num: %u, ", rdrptr->RdrTypeUnion.CtrlRec.Num);
		oh_append_bigtext(&buffer, str);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s, ", 
			oh_lookup_ctrltype(rdrptr->RdrTypeUnion.CtrlRec.Type));
		oh_append_bigtext(&buffer, str);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Output Type: %s, ", 
			oh_lookup_ctrloutputtype(rdrptr->RdrTypeUnion.CtrlRec.OutputType));
		oh_append_bigtext(&buffer, str);						
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
	} 
	return(rv);
}


/* 
 *
 */
static  
SaErrorT show_wdog(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);						
																
	if (rdrptr->RdrType == SAHPI_WATCHDOG_RDR) {
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Watchdog Num: %u, ", rdrptr->RdrTypeUnion.WatchdogRec.WatchdogNum);
		oh_append_bigtext(&buffer, str);
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
		
	} 
	return(rv);
}


/* 
 *
 */
static  
SaErrorT show_dimi(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);						
																
	if (rdrptr->RdrType == SAHPI_DIMI_RDR) {
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DIMI Num: %u, ", 
		                                 rdrptr->RdrTypeUnion.DimiRec.DimiNum);
		oh_append_bigtext(&buffer, str);
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
		
	} 
	return(rv);
}


/* 
 *
 */
static  
SaErrorT show_fumi(SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)
{
	SaErrorT rv       = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);						
																
	if (rdrptr->RdrType == SAHPI_FUMI_RDR) {
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "FUMI Num: %u, ", 
		                                     rdrptr->RdrTypeUnion.FumiRec.Num);
		oh_append_bigtext(&buffer, str);
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
		
	} 
	return(rv);
}

/* 
 *
 */
static  
SaErrorT show_ann (SaHpiSessionIdT sessionid,
			SaHpiRptEntryT *rptptr,	
			SaHpiRdrT *rdrptr, 
			SaHpiResourceIdT l_resourceid)

{
	SaErrorT rv       = SA_OK;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer buffer;
	rv = oh_init_bigtext(&buffer);						
																
	if (rdrptr->RdrType == SAHPI_ANNUNCIATOR_RDR) {
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Annunciator Num: %u, ",
					 rdrptr->RdrTypeUnion.AnnunciatorRec.AnnunciatorNum);
		oh_append_bigtext(&buffer, str);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s, ",
			oh_lookup_annunciatortype(rdrptr->RdrTypeUnion.AnnunciatorRec.AnnunciatorType));
		oh_append_bigtext(&buffer, str);
		if ( rdrptr->IdString.DataType == SAHPI_TL_TYPE_TEXT ) {
		   oh_append_bigtext(&buffer, "Tag: ");
		   snprintf(str,  rdrptr->IdString.DataLength+1, "%s", 
		 	rdrptr->IdString.Data);
		   oh_append_bigtext(&buffer, str);
		   oh_append_bigtext(&buffer, "\n");
		}
		show_rdr(&buffer);
		
	} 
	return(rv);
}

 /* end hpitop.c */
