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
 *        Renier Morales <renierm@users.sf.net>
 *        Daniel de Araujo <ddearauj@us.ibm.com>
 */
 
#include <rtas_inventory.h>
 
SaErrorT rtas_get_idr_info(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrInfoT *idrinfo)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_get_idr_area_header(void *hnd,
                                     SaHpiResourceIdT rid,
                                     SaHpiIdrIdT idrid,
                                     SaHpiIdrAreaTypeT areatype,
                                     SaHpiEntryIdT areaid,
                                     SaHpiEntryIdT *nextareaid,
                                     SaHpiIdrAreaHeaderT *header)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_add_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiIdrAreaTypeT areatype,
                              SaHpiEntryIdT *areaid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_del_idr_area(void *hnd,
                              SaHpiResourceIdT rid,
                              SaHpiIdrIdT idrid,
                              SaHpiEntryIdT areaid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_get_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiIdrFieldTypeT fieldtype,
                               SaHpiEntryIdT fieldid,
                               SaHpiEntryIdT *nextfieldid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_add_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_set_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiIdrFieldT *field)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

SaErrorT rtas_del_idr_field(void *hnd,
                               SaHpiResourceIdT rid,
                               SaHpiIdrIdT idrid,
                               SaHpiEntryIdT areaid,
                               SaHpiEntryIdT fieldid)
{
        return SA_ERR_HPI_INTERNAL_ERROR;
}

void * oh_get_idr_info (void *hnd, SaHpiResourceIdT, SaHpiIdrIdT,SaHpiIdrInfoT)
        __attribute__ ((weak, alias("rtas_get_idr_info")));
void * oh_get_idr_area_header (void *, SaHpiResourceIdT, SaHpiIdrIdT,
                               SaHpiIdrAreaTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                               SaHpiIdrAreaHeaderT)
        __attribute__ ((weak, alias("rtas_get_idr_area_header")));
void * oh_add_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT)
        __attribute__ ((weak, alias("rtas_add_idr_area")));
void * oh_del_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT)
        __attribute__ ((weak, alias("rtas_del_idr_area")));
void * oh_get_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldT)
        __attribute__ ((weak, alias("rtas_get_idr_field")));
void * oh_add_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
        __attribute__ ((weak, alias("rtas_add_idr_field")));
void * oh_set_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
        __attribute__ ((weak, alias("rtas_set_idr_field")));
void * oh_del_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiEntryIdT)
        __attribute__ ((weak, alias("rtas_del_idr_field")));
