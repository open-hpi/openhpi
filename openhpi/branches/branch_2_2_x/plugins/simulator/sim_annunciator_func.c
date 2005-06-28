/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        W. David Ashley <dashley@us.ibm.com>
 */

#include <sim_init.h>


SaErrorT sim_get_next_announce(void *hnd,
			       SaHpiResourceIdT rid,
			       SaHpiAnnunciatorNumT aid,
			       SaHpiSeverityT sev,
			       SaHpiBoolT unackonly,
			       SaHpiAnnouncementT *announcement)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;
        oh_announcement *ann;

	if (!hnd || !announcement || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	/* Check if resource exists and has annunciator capabilities */
	SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
		return(SA_ERR_HPI_INVALID_RESOURCE);
	}

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
		return(SA_ERR_HPI_CAPABILITY);
	}
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* get our announcement list */
        ann = (oh_announcement *)oh_get_rdr_data(state->rptcache, rid,
                                                 rdr->RecordId);
        if (ann == NULL) {
                dbg("No annunciator data.");
                return(SA_ERR_HPI_NOT_PRESENT);
        }

        /* perform function */
        return oh_announcement_get_next(ann, sev, unackonly, announcement);
}


SaErrorT sim_get_announce(void *hnd,
			  SaHpiResourceIdT rid,
			  SaHpiAnnunciatorNumT aid,
			  SaHpiEntryIdT entry,
			  SaHpiAnnouncementT *announcement)
 {
         struct oh_handler_state *state = (struct oh_handler_state *)hnd;
         oh_announcement *ann;

         if (!hnd || !announcement) {
                 dbg("Invalid parameter.");
                 return SA_ERR_HPI_INVALID_PARAMS;
         }

         /* Check if resource exists and has annunciator capabilities */
         SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
         if (!rpt) {
                 return(SA_ERR_HPI_INVALID_RESOURCE);
         }

         if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                 return(SA_ERR_HPI_CAPABILITY);
         }
         SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                             SAHPI_ANNUNCIATOR_RDR, aid);
         if (rdr == NULL)
                 return(SA_ERR_HPI_NOT_PRESENT);

         /* get our announcement list */
         ann = (oh_announcement *)oh_get_rdr_data(state->rptcache, rid,
                                                  rdr->RecordId);
         if (ann == NULL) {
                 dbg("No annunciator data.");
                 return(SA_ERR_HPI_NOT_PRESENT);
         }

         /* perform function */
         return oh_announcement_get(ann, entry, announcement);
 }


SaErrorT sim_ack_announce(void *hnd,
			  SaHpiResourceIdT rid,
			  SaHpiAnnunciatorNumT aid,
			  SaHpiEntryIdT entry,
			  SaHpiSeverityT sev)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;
        oh_announcement *ann;

	if (!hnd || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        /* Check if resource exists and has annunciator capabilities */
        SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                return(SA_ERR_HPI_CAPABILITY);
        }
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* get our announcement list */
        ann = (oh_announcement *)oh_get_rdr_data(state->rptcache, rid,
                                                 rdr->RecordId);
        if (ann == NULL) {
                dbg("No annunciator data.");
                return(SA_ERR_HPI_NOT_PRESENT);
        }

        /* perform function */
        return oh_announcement_ack(ann, entry, sev);
}


SaErrorT sim_add_announce(void *hnd,
			  SaHpiResourceIdT rid,
			  SaHpiAnnunciatorNumT aid,
			  SaHpiAnnouncementT *announcement)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;
        oh_announcement *ann;

	if (!hnd || !announcement) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        /* Check if resource exists and has annunciator capabilities */
        SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                return(SA_ERR_HPI_CAPABILITY);
        }
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* get our announcement list */
        ann = (oh_announcement *)oh_get_rdr_data(state->rptcache, rid,
                                                 rdr->RecordId);
        if (ann == NULL) {
                dbg("No annunciator data.");
                return(SA_ERR_HPI_NOT_PRESENT);
        }

        /* perform function */
        return oh_announcement_append(ann, announcement);
}


SaErrorT sim_del_announce(void *hnd,
			  SaHpiResourceIdT rid,
			  SaHpiAnnunciatorNumT aid,
			  SaHpiEntryIdT entry,
			  SaHpiSeverityT sev)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;
        oh_announcement *ann;

	if (!hnd || oh_lookup_severity(sev) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        /* Check if resource exists and has annunciator capabilities */
        SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                return(SA_ERR_HPI_CAPABILITY);
        }
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* get our announcement list */
        ann = (oh_announcement *)oh_get_rdr_data(state->rptcache, rid,
                                                 rdr->RecordId);
        if (ann == NULL) {
                dbg("No annunciator data.");
                return(SA_ERR_HPI_NOT_PRESENT);
        }

        /* perform function */
        return oh_announcement_del(ann, entry, sev);
}


SaErrorT sim_get_annunc_mode(void *hnd,
			     SaHpiResourceIdT rid,
			     SaHpiAnnunciatorNumT aid,
			     SaHpiAnnunciatorModeT *mode)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	if (!hnd || !mode) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        /* Check if resource exists and has annunciator capabilities */
        SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                return(SA_ERR_HPI_CAPABILITY);
        }
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* we allow everybody to modify annunciator announcements */
        *mode = SAHPI_ANNUNCIATOR_MODE_SHARED;

	return(SA_OK);
}


SaErrorT sim_set_annunc_mode(void *hnd,
			     SaHpiResourceIdT rid,
			     SaHpiAnnunciatorNumT aid,
			     SaHpiAnnunciatorModeT mode)
{
        struct oh_handler_state *state = (struct oh_handler_state *)hnd;

	if (!hnd || oh_lookup_annunciatormode(mode) == NULL) {
		dbg("Invalid parameter.");
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        /* Check if resource exists and has annunciator capabilities */
        SaHpiRptEntryT *rpt = oh_get_resource_by_id(state->rptcache, rid);
        if (!rpt) {
                return(SA_ERR_HPI_INVALID_RESOURCE);
        }

        if (!(rpt->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)) {
                return(SA_ERR_HPI_CAPABILITY);
        }
        SaHpiRdrT *rdr = oh_get_rdr_by_type(state->rptcache, rid,
                                            SAHPI_ANNUNCIATOR_RDR, aid);
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);

        /* our mode is not changeable */
	return(SA_ERR_HPI_READ_ONLY);
}



void * oh_get_next_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                             SaHpiSeverityT, SaHpiBoolT, SaHpiAnnouncementT)
                __attribute__ ((weak, alias("sim_get_next_announce")));

void * oh_get_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("sim_get_announce")));

void * oh_ack_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("sim_ack_announce")));


void * oh_add_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("sim_add_announce")));

void * oh_del_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                       SaHpiEntryIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("sim_del_announce")));

void * oh_get_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT *)
                __attribute__ ((weak, alias("sim_get_annunc_mode")));

void * oh_set_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT)
                __attribute__ ((weak, alias("sim_set_annunc_mode")));


