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

#include <algorithm>
#include <string>

#include "announcement.h"
#include "annunciator.h"
#include "codec.h"
#include "vars.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultAnnunciatorRec( SaHpiAnnunciatorNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiAnnunciatorRecT& rec = data.AnnunciatorRec;

    rec.AnnunciatorNum  = num;
    rec.AnnunciatorType = SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE;
    rec.ModeReadOnly    = SAHPI_FALSE;
    rec.MaxConditions   = 0;
    rec.Oem             = 0;

    return data;
}

struct AnnouncementIdPred
{
    explicit AnnouncementIdPred( SaHpiEntryIdT _id )
        : id( _id )
    {
        // empty
    }

    bool operator ()( const cAnnouncement * a ) const
    {
        return ( id == SAHPI_FIRST_ENTRY ) ||
               ( id == a->GetId() );
    }

    SaHpiEntryIdT id;
};

struct AnnouncementNextTimestampPred
{
    explicit AnnouncementNextTimestampPred( const SaHpiTimeT& _timestamp )
        : timestamp( _timestamp )
    {
        // empty
    }

    bool operator ()( const cAnnouncement * a ) const
    {
        return ( timestamp < a->GetTimestamp() );
    }

    SaHpiTimeT timestamp;
};

struct AnnouncementSeverityPred
{
    explicit AnnouncementSeverityPred( SaHpiSeverityT _sev,
                                       bool _only_unack = false )
        : sev( _sev ), only_unack( _only_unack )
    {
        // empty
    }

    bool operator ()( const cAnnouncement * a ) const
    {
        if ( only_unack && a->IsAcknowledged() ) {
            return false;
        }
        return ( sev == SAHPI_ALL_SEVERITIES ) || ( sev == a->GetSeverity() );
    }

    SaHpiSeverityT sev;
    bool only_unack;
};

struct AnnouncementAck
{
    explicit AnnouncementAck( SaHpiSeverityT _sev )
        : sev( _sev )
    {
        // empty
    }

    void operator ()( cAnnouncement * a )
    {
        if ( ( sev == SAHPI_ALL_SEVERITIES ) || ( sev == a->GetSeverity() ) ) {
            a->Acknowledge();
        }
    }

    SaHpiSeverityT sev;
};

struct AnnouncementDelete
{
    explicit AnnouncementDelete( SaHpiSeverityT _sev )
        : sev( _sev )
    {
        // empty
    }

    void operator ()( cAnnouncement * a )
    {
        if ( ( sev == SAHPI_ALL_SEVERITIES ) || ( sev == a->GetSeverity() ) ) {
            delete a;
        }
    }

    SaHpiSeverityT sev;
};

struct AnnouncementMaxId
{
    explicit AnnouncementMaxId()
        : value( 0 )
    {
        // empty
    }

    void operator ()( const cAnnouncement * a )
    {
        value = std::max( value, a->GetId() );
    }

    SaHpiEntryIdT value;
};

struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( cAnnouncement * a )
    {
        children.push_back( a );
    }

    cObject::Children& children;
};



/**************************************************************
 * class cAnnunciator
 *************************************************************/
const std::string cAnnunciator::classname( "ann" );

cAnnunciator::cAnnunciator( cHandler& handler,
                            cResource& resource,
                            SaHpiAnnunciatorNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_ANNUNCIATOR_RDR,
                   MakeDefaultAnnunciatorRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.AnnunciatorRec ),
      m_mode( SAHPI_ANNUNCIATOR_MODE_SHARED )
{
    // empty
}

cAnnunciator::~cAnnunciator()
{
    // empty
}

cAnnouncement * cAnnunciator::GetAnnouncement( SaHpiEntryIdT aid ) const
{
    Announcements::const_iterator i;
    i = std::find_if( m_as.begin(), m_as.end(), AnnouncementIdPred( aid ) );
    if ( i == m_as.end() ) {
        return 0;
    }

    return *i;
}


// HPI interface
SaErrorT cAnnunciator::GetAnnouncement( SaHpiEntryIdT aid,
                                        SaHpiAnnouncementT& data ) const
{
    if ( ( aid == SAHPI_FIRST_ENTRY ) || ( aid == SAHPI_LAST_ENTRY ) ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    cAnnouncement * a = GetAnnouncement( aid );
    if ( !a ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    data = a->GetData();

    return SA_OK;
}

SaErrorT cAnnunciator::GetNextAnnouncement( SaHpiSeverityT sev,
                                            SaHpiBoolT only_unack,
                                            SaHpiAnnouncementT& data ) const
{
    Announcements::const_iterator i;

    if ( data.EntryId != SAHPI_FIRST_ENTRY ) {
        i = std::find_if( m_as.begin(), m_as.end(), AnnouncementIdPred( data.EntryId ) );
        if ( i != m_as.end() ) {
            if ( (*i)->GetTimestamp() != data.Timestamp ) {
                return SA_ERR_HPI_INVALID_DATA;
            }
            ++i;
        } else {
            // announcement with specified id might have been removed
            i = std::find_if( m_as.begin(),
                              m_as.end(),
                              AnnouncementNextTimestampPred( data.EntryId ) );
        }
    } else {
        i = m_as.begin();
    }

    AnnouncementSeverityPred pred( sev, ( only_unack != SAHPI_FALSE ) );
    Announcements::const_iterator j;
    j = std::find_if( i, m_as.end(), pred );
    if ( j == m_as.end() ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    data = (*j)->GetData();

    return SA_OK;
}

SaErrorT cAnnunciator::AckAnnouncement( SaHpiEntryIdT aid, SaHpiSeverityT sev )
{
    if ( aid != SAHPI_ENTRY_UNSPECIFIED ) {
        cAnnouncement * a = GetAnnouncement( aid );
        if ( !a ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        a->Acknowledge();
    } else {
        std::for_each( m_as.begin(), m_as.end(), AnnouncementAck( sev ) );
    }

    return SA_OK;
}

SaErrorT cAnnunciator::AddAnnouncement( SaHpiAnnouncementT& data )
{
    if ( m_mode == SAHPI_ANNUNCIATOR_MODE_AUTO ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    AnnouncementMaxId max_id = std::for_each( m_as.begin(),
                                              m_as.end(),
                                              AnnouncementMaxId() );
    cAnnouncement * a = new cAnnouncement( max_id.value + 1, data );
    m_as.push_back( a );
    data = a->GetData();

    return SA_OK;
}

SaErrorT cAnnunciator::DeleteAnnouncement( SaHpiEntryIdT aid, SaHpiSeverityT sev )
{
    if ( m_mode == SAHPI_ANNUNCIATOR_MODE_AUTO ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    if ( aid != SAHPI_ENTRY_UNSPECIFIED ) {
        cAnnouncement * a = GetAnnouncement( aid );
        if ( !a ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        m_as.remove_if( AnnouncementIdPred( a->GetId() ) );
        delete a;
    } else {
        std::for_each( m_as.begin(), m_as.end(), AnnouncementDelete( sev ) );
        m_as.remove_if( AnnouncementSeverityPred( sev ) );
    }

    return SA_OK;
}

SaErrorT cAnnunciator::GetMode( SaHpiAnnunciatorModeT& mode ) const
{
    mode = m_mode;

    return SA_OK;
}

SaErrorT cAnnunciator::SetMode( SaHpiAnnunciatorModeT mode )
{
    if ( m_rec.ModeReadOnly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    m_mode = mode;

    return SA_OK;
}


// cObject virtual functions
void cAnnunciator::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( cAnnouncement::classname + "-XXX" );
}

bool cAnnunciator::CreateChild( const std::string& name )
{
    bool rc;

    rc = cObject::CreateChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( id == SAHPI_FIRST_ENTRY ) {
        return false;
    }
    if ( id == SAHPI_LAST_ENTRY ) {
        return false;
    }

    if ( cname == cAnnouncement::classname ) {
        if ( !GetAnnouncement( id ) ) {
            m_as.push_back( new cAnnouncement( id ) );
            return true;
        }
    }

    return false;
}

bool cAnnunciator::RemoveChild( const std::string& name )
{
    bool rc;

    rc = cObject::RemoveChild( name );
    if ( rc ) {
        return true;
    }

    std::string cname;
    SaHpiUint32T id;
    rc = DisassembleNumberedObjectName( name, cname, id );
    if ( !rc ) {
        return false;
    }

    if ( id == SAHPI_FIRST_ENTRY ) {
        return false;
    }
    if ( id == SAHPI_LAST_ENTRY ) {
        return false;
    }

    if ( cname == cAnnouncement::classname ) {
        cAnnouncement * a = GetAnnouncement( id );
        if ( a ) {
            // NB: if we pass id to AnnouncementIdPred
            //     and id is SAHPI_FIRST_ENTRY
            //     then all areas will be deleted.
            m_as.remove_if( AnnouncementIdPred( id ) );
            delete a;
            return true;
        }
    }

    return false;

}

void cAnnunciator::GetChildren( cObject::Children& children ) const
{
    cObject::GetChildren( children );

    ObjectCollector collector( children );
    std::for_each( m_as.begin(), m_as.end(), collector );
}

void cAnnunciator::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    vars << "Mode"
         << dtSaHpiAnnunciatorModeT
         << DATA( m_mode )
         << VAR_END();
}


}; // namespace TA

