/*      -*- linux-c -*-
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
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <snmp_bc_plugin.h>

/**
 * snmp_bc_get_next_announce:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @sev: Severity.
 * @unackonly: Boolean to get unacknowledged annunicators only.
 *
 * Gets the next annunicator.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL @sev invalid.
 **/
SaErrorT snmp_bc_get_next_announce(void *hnd,
				   SaHpiResourceIdT rid,
				   SaHpiAnnunciatorNumT aid,
				   SaHpiSeverityT sev,
				   SaHpiBoolT unackonly,
				   SaHpiAnnouncementT *announcement)
{
	if (!hnd || !announcement || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}


	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}
        
/**
 * snmp_bc_get_announce:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @entry: Annunicator's announcement ID.
 * @announcment: Location to store annunicator's announcement.
 *
 * Gets the annunicator's announcement information.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_announce(void *hnd,
			      SaHpiResourceIdT rid,
			      SaHpiAnnunciatorNumT aid,
			      SaHpiEntryIdT entry,
			      SaHpiAnnouncementT *announcement)
 {
	if (!hnd || !announcement) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}

	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
 }

/**
 * snmp_bc_ack_announce:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @entry: Annunicator's announcement ID.
 * @sev: Severity.
 *
 * Acknowledge an annunicator's announcement(s).
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL; @sev invalid.
 **/
SaErrorT snmp_bc_ack_announce(void *hnd,
			      SaHpiResourceIdT rid,
			      SaHpiAnnunciatorNumT aid,
			      SaHpiEntryIdT entry,
			      SaHpiSeverityT sev)
{
	if (!hnd || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}


	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}
 
/**
 * snmp_bc_add_announce:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @announcement: Pointer to annunicator's announcement data.
 *
 * Add an announcement to an annunicator.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_add_announce(void *hnd,
			      SaHpiResourceIdT rid, 
			      SaHpiAnnunciatorNumT aid,
			      SaHpiAnnouncementT *announcement)
{
	if (!hnd || !announcement) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}


	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_del_announce:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @entry: Annunicator's announcement ID.
 * @sev: Severity.
 *
 * Delete announcement(s) from an annunicator.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL; @sev invalid.
 **/
SaErrorT snmp_bc_del_announce(void *hnd,
			      SaHpiResourceIdT rid,
			      SaHpiAnnunciatorNumT aid,
			      SaHpiEntryIdT entry,
			      SaHpiSeverityT sev)
{
	if (!hnd || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}
	

	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}
        
/**
 * snmp_bc_get_annunc_mode:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @mode: Location to store mode information.
 *
 * Gets an annunciator's current operating mode.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT snmp_bc_get_annunc_mode(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiAnnunciatorNumT aid,
				 SaHpiAnnunciatorModeT *mode)
{
	if (!hnd || !mode) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	}


	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}

/**
 * snmp_bc_get_annunc_mode:
 * @hnd: Handler data pointer.
 * @rid: Resource ID.
 * @aid: Annunciator ID.
 * @mode: Anninciator mode to set.
 *
 * Sets an annunciator's current operating mode.
 * 
 * Return values:
 * SA_OK - Normal case.
 * SA_ERR_HPI_CAPABILITY - Resource doesn't have SAHPI_CAPABILITY_ANNUNICATOR.
 * SA_ERR_HPI_INVALID_RESOURCE - Resource doesn't exist.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL; @mode invalid.
 **/
SaErrorT snmp_bc_set_annunc_mode(void *hnd,
				 SaHpiResourceIdT rid,
				 SaHpiAnnunciatorNumT aid,
				 SaHpiAnnunciatorModeT mode)
{
	if (!hnd || oh_lookup_annunciatormode(mode) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        struct snmp_bc_hnd *custom_handle = (struct snmp_bc_hnd *)handle->data;

	if (!custom_handle) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	g_static_rec_mutex_lock(&handle->handler_lock);
	/* Check if resource exists and has managed hotswap capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(handle->rptcache, rid);
        if (!rpt) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_INVALID_RESOURCE);
	} 
	
        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		g_static_rec_mutex_unlock(&handle->handler_lock);
		return(SA_ERR_HPI_CAPABILITY);
	} 
	

	dbg("Annunciators not supported by platform");
	g_static_rec_mutex_unlock(&handle->handler_lock);
	return(SA_ERR_HPI_INTERNAL_ERROR);
}
