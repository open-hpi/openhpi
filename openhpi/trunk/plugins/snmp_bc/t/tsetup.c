/* -*- linux-c -*-
 * 
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Peter D Phan <pdphan@users.sourceforge.net>
 */


#include <snmp_bc_plugin.h>
#include <sahpimacros.h>
#include <tsetup.h>

SaErrorT tsetup (SaHpiSessionIdT *sessionid_ptr)
{

	SaErrorT err = SA_OK;	
	/* ************************	 	 
	 * Hook in simulation environment
	 * ***********************/

        err = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, sessionid_ptr, NULL);
        if (err != SA_OK) {
		printf("  Error! Testcase failed. Line=%d\n", __LINE__);
		printf("  Received error=%s\n", oh_lookup_error(err));
        }
 
        if (!err) err = saHpiDiscover(*sessionid_ptr);
        if (err != SA_OK) {
		printf("  Error! Testcase failed. Line=%d\n", __LINE__);
		printf("  Received error=%s\n", oh_lookup_error(err));
		err = saHpiSessionClose(*sessionid_ptr);
        }
	
	return err;	 
}

SaErrorT tfind_resource(SaHpiSessionIdT *sessionid_ptr,
			SaHpiCapabilitiesT search_rdr_type,
			struct oh_handler *h,
			SaHpiRptEntryT *rptentry)
{

	if (!sessionid_ptr) {
		printf("  Error! Invalid test setup. Line=%d\n", __LINE__);
		return(SA_ERR_HPI_INVALID_PARAMS);
	} 
	
	SaHpiCapabilitiesT cap_mask =	(SAHPI_CAPABILITY_RESOURCE 	  |
					SAHPI_CAPABILITY_AGGREGATE_STATUS |
					SAHPI_CAPABILITY_CONFIGURATION	  |
					SAHPI_CAPABILITY_MANAGED_HOTSWAP  |
					SAHPI_CAPABILITY_WATCHDOG	  |
					SAHPI_CAPABILITY_CONTROL	  |
					SAHPI_CAPABILITY_FRU		  |
					SAHPI_CAPABILITY_ANNUNCIATOR	  |
					SAHPI_CAPABILITY_POWER		  |
					SAHPI_CAPABILITY_RESET		  |
					SAHPI_CAPABILITY_INVENTORY_DATA	  |
					SAHPI_CAPABILITY_EVENT_LOG	  |
					SAHPI_CAPABILITY_RDR		  |
					SAHPI_CAPABILITY_SENSOR); 
			
	if ((search_rdr_type & cap_mask) == 0) {
		printf("  Error! Invalid resource type. Line=%d\n", __LINE__);
		return(SA_ERR_HPI_INVALID_PARAMS);	
	}
	
	struct oh_domain *d;
        SaHpiDomainIdT did;						
        struct oh_handler *lh;
        SaHpiRptEntryT *l_rptentry = NULL;
	

        OH_CHECK_INIT_STATE(*sessionid_ptr);
        OH_GET_DID(*sessionid_ptr, did);

        OH_GET_DOMAIN(did, d); /* Lock domain */
	SaHpiEntryIdT entryid = SAHPI_FIRST_ENTRY;

	/* ************************	 	 
	 * Find a resource 
	 * ***********************/
		
	do {
                l_rptentry = oh_get_resource_next(&(d->rpt), entryid);
		if (l_rptentry) {
			if (l_rptentry->ResourceCapabilities & search_rdr_type) break;
			else entryid = l_rptentry->EntryId;
		} 
			
	} while (l_rptentry != NULL);

	if (!l_rptentry) {
		oh_release_domain(d); /* Unlock domain */	
		printf("  Error! Can not find resource for this test.\n");	
		printf("  Error! Testcase setup failed. Line=%d\n", __LINE__);
		return(SA_ERR_HPI_ENTITY_NOT_PRESENT);
	} else {		
        	OH_HANDLER_GET(d, l_rptentry->ResourceId, lh);	
		oh_release_domain(d); /* Unlock domain */
		memcpy(rptentry,l_rptentry, sizeof(SaHpiRptEntryT));	
		memcpy(h, lh, sizeof(struct oh_handler));	
		return(SA_OK);		
	}

}


SaErrorT tcleanup(SaHpiSessionIdT *sessionid_ptr)
{

	SaErrorT err = SA_OK;
	/***************************
	 * Close session, free memory
	 ***************************/
	 err = saHpiSessionClose(*sessionid_ptr);
	 return(err);

}


