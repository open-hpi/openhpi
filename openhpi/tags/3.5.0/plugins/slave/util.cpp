/*      -*- linux-c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <oh_utils.h>

#include "util.h"


namespace Slave {


/**************************************************************
 * Utility Functions
 *************************************************************/

bool IsLeavingEvent( const SaHpiEventT& event )
{
    if ( event.EventType == SAHPI_ET_RESOURCE ) {
        const SaHpiResourceEventT& re = event.EventDataUnion.ResourceEvent;
        return ( re.ResourceEventType == SAHPI_RESE_RESOURCE_REMOVED );
    } else if ( event.EventType == SAHPI_ET_HOTSWAP ) {
        const SaHpiHotSwapEventT& hse = event.EventDataUnion.HotSwapEvent;
        return ( hse.HotSwapState == SAHPI_HS_STATE_NOT_PRESENT );
    }
    return false;
}

bool IsUpdateEvent( const SaHpiEventT& event )
{
    if ( event.EventType == SAHPI_ET_RESOURCE ) {
        const SaHpiResourceEventT& re = event.EventDataUnion.ResourceEvent;
        return ( re.ResourceEventType == SAHPI_RESE_RESOURCE_UPDATED );
    }
    return false;
}

bool IsEntityPathValid( const SaHpiEntityPathT& ep )
{
    for ( unsigned int i = 0; i < SAHPI_MAX_ENTITY_PATH; ++i ) {
        if ( ep.Entry[i].EntityType != SAHPI_ENT_UNSPECIFIED ) {
            return true;
        }
    }

    return false;
}

bool IsRptEntryValid( const SaHpiRptEntryT& rpte )
{
    if ( rpte.ResourceId == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        return false;
    }
    if ( rpte.ResourceCapabilities == 0 ) {
        return false;
    }

    return true;
}

bool IsRdrValid( const SaHpiRdrT& rdr )
{
    return ( rdr.RdrType != SAHPI_NO_RECORD );
}


/**************************************************************
 * Various HPI Data structures translation
 * Slave -> Master
 *************************************************************/
void TranslateEntityPath( SaHpiEntityPathT& ep,
                          const SaHpiEntityPathT& root )
{
    if ( !IsEntityPathValid( ep ) ) {
        return;
    }
    oh_concat_ep( &ep, &root );
}

void TranslateDimiTest( SaHpiDimiTestT& dt,
                        const SaHpiEntityPathT& root )
{
    for ( unsigned int i = 0; i < SAHPI_DIMITEST_MAX_ENTITIESIMPACTED; ++i ) {
        TranslateEntityPath( dt.EntitiesImpacted[i].EntityImpacted, root );
    }
}

void TranslateFumiServiceImpact( SaHpiFumiServiceImpactDataT& fsi,
                                 const SaHpiEntityPathT& root )
{
    for ( unsigned int i = 0; i < SAHPI_FUMI_MAX_ENTITIES_IMPACTED; ++i ) {
        TranslateEntityPath( fsi.ImpactedEntities[i].ImpactedEntity, root );
    }
}

void TranslateEvent( SaHpiEventT& event,
                     SaHpiResourceIdT master_rid )
{
    event.Source = master_rid;
}

void TranslateRptEntry( SaHpiRptEntryT& rpte,
                        SaHpiResourceIdT master_rid,
                        const SaHpiEntityPathT& root )
{
    if ( !IsRptEntryValid( rpte ) ) {
        return;
    }

    rpte.EntryId = SAHPI_ENTRY_UNSPECIFIED;
    rpte.ResourceId = master_rid;
    TranslateEntityPath( rpte.ResourceEntity, root );
}

void TranslateRdr( SaHpiRdrT& rdr,
                   const SaHpiEntityPathT& root )
{
    if ( !IsRdrValid( rdr ) ) {
        return;
    }

    rdr.RecordId = SAHPI_ENTRY_UNSPECIFIED;
    TranslateEntityPath( rdr.Entity, root );
}

void TranslateRdrs( const GSList * rdr_list,
                    const SaHpiEntityPathT& root )
{
    for ( const GSList * node = rdr_list; node; node = node->next ) {
        SaHpiRdrT * rdr = (SaHpiRdrT *)node->data;
        TranslateRdr( *rdr, root );
    }
}

void TranslateAnnouncement( SaHpiAnnouncementT& a,
                            SaHpiResourceIdT master_rid,
                            const SaHpiEntityPathT& root )
{
    if ( a.AddedByUser != SAHPI_FALSE ) {
        return;
    }
    TranslateEntityPath( a.StatusCond.Entity, root );
    a.StatusCond.DomainId = SAHPI_UNSPECIFIED_DOMAIN_ID; // TODO
    a.StatusCond.ResourceId = master_rid;
}


}; // namespace Slave

