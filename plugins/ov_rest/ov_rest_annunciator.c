/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett Packard Enterprise, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

#include "ov_rest_annunciator.h"

/**
 * ov_rest_get_next_announce:
 *      @oh_handler:          Pointer to openhpi handler.
 *      @resource_id:         Resource Id.
 *      @num:                 Annuciator number.
 *      @severity:            Annuciator severity
 *      @unacknowledged_only: True indicates only unacknowledged announcements
 *                            should be returned. False indicates either an
 *                            acknowledged or unacknowledged announcement may
 *                            be returned.
 *      @announcement:        Pointer to hold the returned announcement.
 *
 * Purpose:
 *      Retrieve an announcement from the current set of announcements
 *      held in the Annunciator.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_get_next_announce(void *oh_handler,
                                   SaHpiResourceIdT resource_id,
                                   SaHpiAnnunciatorNumT num,
                                   SaHpiSeverityT severity,
                                   SaHpiBoolT unacknowledged_only,
                                   SaHpiAnnouncementT *announcement)

{
        err("OV REST get next announce not implemented");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_get_announce:
 *      @oh_handler:   Pointer to openhpi handler.
 *      @resource_id:  Resource Id.
 *      @num:          Annuciator number.
 *      @entry:        Identifier of the announcement to retrieve from the
 *                     Annunciator.
 *      @announcement: Pointer to hold the returned announcement.
 *
 * Purpose:
 *      Retrieve of a specific announcement from the current set of
 *      announcements held in the Annunciator.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_get_announce(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT entry,
                              SaHpiAnnouncementT *announcement)
{
        err("OV REST get announce not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_ack_announce:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @num:         Annuciator number.
 *      @entry:       Identifier of the announcement to retrieve from the
 *                    Annunciator.
 *      @severity:    Severity level of announcements to acknowledge.
 *
 * Purpose:
 *      acknowledge a single announcement or a group of announcements by
 *      severity.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_ack_announce(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT entry,
                              SaHpiSeverityT severity)
{
        err("OV REST ack announce not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_get_annunc_mode:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @num:         Annuciator number.
 *      @mode:        Pointer to store the current operating mode of the
 *                    Annunciator.
 *
 * Purpose:
 *      Retrieve the current operating mode of an Annunciator.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_get_annunc_mode(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT *mode)
{
        err("OV REST get annunc mode not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_set_annunc_mode:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @num:         Annuciator number.
 *      @mode:        Mode set for the Annunciator.
 *
 * Purpose:
 *      Change the current operating mode of an Annunciator.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_set_annunc_mode(void *oh_handler,
                                 SaHpiResourceIdT resource_id,
                                 SaHpiAnnunciatorNumT num,
                                 SaHpiAnnunciatorModeT mode)
{
        err("OV REST set annunc mode not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_del_announce:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @num:         Annuciator number.
 *      @entry:       Entry identifier of the announcement to delete
 *      @severity:    Severity level of announcements to delete.
 *
 * Purpose:
 *      Delete a single announcement or a group of announcements from the
 *      current set of an Annunciator
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_del_announce(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiEntryIdT entry,
                              SaHpiSeverityT severity)
{
        err("OV REST del announce not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * ov_rest_add_announce:
 *      @oh_handler:  Pointer to openhpi handler.
 *      @resource_id: Resource Id.
 *      @num:         Annuciator number.
 *      @annoucement: Pointer to structure that contains the new announcement
 *                    to add to the set.
 *
 * Purpose:
 *      add an announcement to the set of items held by an Annunciator
 *      management instrument.
 *
 * Detailed Description: NA
 *
 * Return values:
 *      SA_ERR_HPI_UNSUPPORTED_API - current OV REST plugin implementation
 *                                   does not support this API.
 **/
SaErrorT ov_rest_add_announce(void *oh_handler,
                              SaHpiResourceIdT resource_id,
                              SaHpiAnnunciatorNumT num,
                              SaHpiAnnouncementT *announcement)
{
        err("OV REST add announce not implemented ");
        return SA_ERR_HPI_UNSUPPORTED_API;
}


void * oh_get_next_announce (void *,
                             SaHpiResourceIdT,
                             SaHpiAnnunciatorNumT,
                             SaHpiSeverityT,
                             SaHpiBoolT,
                             SaHpiAnnouncementT)
                __attribute__ ((weak, alias("ov_rest_get_next_announce")));

void * oh_get_announce (void *,
                        SaHpiResourceIdT,
                        SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT,
                        SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("ov_rest_get_announce")));

void * oh_ack_announce (void *,
                        SaHpiResourceIdT,
                        SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT,
                        SaHpiSeverityT)
                __attribute__ ((weak, alias("ov_rest_ack_announce")));


void * oh_add_announce (void *,
                        SaHpiResourceIdT,
                        SaHpiAnnunciatorNumT,
                        SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("ov_rest_add_announce")));

void * oh_del_announce (void *,
                        SaHpiResourceIdT,
                        SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT,
                        SaHpiSeverityT)
                __attribute__ ((weak, alias("ov_rest_del_announce")));

void * oh_get_annunc_mode (void *,
                           SaHpiResourceIdT,
                           SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT *)
                __attribute__ ((weak, alias("ov_rest_get_annunc_mode")));

void * oh_set_annunc_mode (void *,
                           SaHpiResourceIdT,
                           SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT)
                __attribute__ ((weak, alias("ov_rest_set_annunc_mode")));
