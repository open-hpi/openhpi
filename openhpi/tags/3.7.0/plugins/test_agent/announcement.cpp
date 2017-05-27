/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTWATCHDOGLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <oh_utils.h>

#include "announcement.h"
#include "codec.h"
#include "structs.h"
#include "utils.h"
#include "vars.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static void MakeDefaultAnnouncement( SaHpiAnnouncementT& a, SaHpiEntryIdT id )
{
    a.EntryId      = id;
    oh_gettimeofday( &a.Timestamp );
    a.AddedByUser  = SAHPI_FALSE;
    a.Severity     = SAHPI_INFORMATIONAL;
    a.Acknowledged = SAHPI_FALSE;

    SaHpiConditionT& c = a.StatusCond;

    c.Type        = SAHPI_STATUS_COND_TYPE_OEM;
    oh_init_ep( &c.Entity );
    c.DomainId    = SAHPI_UNSPECIFIED_DOMAIN_ID;
    c.ResourceId  = SAHPI_UNSPECIFIED_RESOURCE_ID;
    c.SensorNum   = 0;
    c.EventState  = SAHPI_ES_UNSPECIFIED;
    c.Name.Length = 0;
    c.Mid         = 12345;
    MakeHpiTextBuffer( c.Data, "" );
}

static void MakeUserAnnouncement( SaHpiAnnouncementT& a,
                                  SaHpiEntryIdT id,
                                  const SaHpiAnnouncementT& user_data )
{
    a             = user_data;
    a.EntryId     = id;
    oh_gettimeofday( &a.Timestamp );
    a.AddedByUser = SAHPI_TRUE;
}

/**************************************************************
 * class cAnnouncement
 *************************************************************/
const std::string cAnnouncement::classname( "announcement" );

cAnnouncement::cAnnouncement( SaHpiEntryIdT id )
    : cObject( AssembleNumberedObjectName( classname, id ) )
{
    MakeDefaultAnnouncement( m_data, id );
}

cAnnouncement::cAnnouncement( SaHpiEntryIdT id,
                              const SaHpiAnnouncementT& user_data )
    : cObject( AssembleNumberedObjectName( classname, id ) )
{
    MakeUserAnnouncement( m_data, id, user_data );
}


cAnnouncement::~cAnnouncement()
{
    // empty
}


// cObject virtual functions
void cAnnouncement::GetVars( cVars& vars )
{
    cObject::GetVars( vars );
    Structs::GetVars( m_data, vars );
}


}; // namespace TA

