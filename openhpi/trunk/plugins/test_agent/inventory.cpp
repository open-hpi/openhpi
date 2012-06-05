/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTINVENTORYLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <algorithm>
#include <string>

#include "area.h"
#include "codec.h"
#include "inventory.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultInvRec( SaHpiIdrIdT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiInventoryRecT& rec = data.InventoryRec;

    rec.IdrId      = num;
    rec.Persistent = SAHPI_FALSE;
    rec.Oem        = 0;

    return data;
}


/**************************************************************
 * Functors for working with areas
 *************************************************************/
struct AreaDeleter
{
    void operator ()( cArea * area )
    {
        delete area;
    }
};

struct AreaIdPred
{
    explicit AreaIdPred( SaHpiEntryIdT _id )
        : id( _id )
    {
        // empty
    }

    bool operator ()( const cArea * area ) const
    {
        return ( id == SAHPI_FIRST_ENTRY ) ||
               ( id == area->GetId() );
    }

    SaHpiEntryIdT id;
};

struct AreaIdTypePred
{
    explicit AreaIdTypePred( SaHpiEntryIdT _id, SaHpiIdrAreaTypeT _type )
        : id( _id ), type( _type )
    {
        // empty
    }

    bool operator ()( const cArea * area ) const
    {
        if ( type == area->GetType() ) {
            return ( id == SAHPI_FIRST_ENTRY ) ||
                   ( id == area->GetId() );
        } else {
            return false;
        }
    }

    SaHpiEntryIdT     id;
    SaHpiIdrAreaTypeT type;
};

struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( cArea * area )
    {
        children.push_back( area );
    }

    cObject::Children& children;
};

struct AreaMaxId
{
    explicit AreaMaxId()
        : value( 0 )
    {
        // empty
    }

    void operator ()( const cArea * area )
    {
        value = std::max( value, area->GetId() );
    }

    SaHpiEntryIdT value;
};


/**************************************************************
 * class cInventory
 *************************************************************/
const std::string cInventory::classname = "inv";

cInventory::cInventory( cHandler& handler, cResource& resource, SaHpiIdrIdT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_INVENTORY_RDR,
                   MakeDefaultInvRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.InventoryRec ),
      m_readonly( SAHPI_FALSE ),
      m_update_count( 0 )
{
    // empty
}

cInventory::~cInventory()
{
    AreaDeleter deleter;
    std::for_each( m_areas.begin(), m_areas.end(), deleter );
    m_areas.clear();
}


cArea * cInventory::GetArea( SaHpiEntryIdT aid ) const
{
    Areas::const_iterator i;
    i = std::find_if( m_areas.begin(), m_areas.end(), AreaIdPred( aid ) );
    if ( i == m_areas.end() ) {
        return 0;
    }

    return *i;
}


// HPI Interface
SaErrorT cInventory::GetInfo( SaHpiIdrInfoT& info ) const
{
    info.IdrId       = m_rec.IdrId;
    info.UpdateCount = m_update_count;
    info.ReadOnly    = m_readonly;
    info.NumAreas    = m_areas.size();

    return SA_OK;
}

SaErrorT cInventory::GetArea( SaHpiIdrAreaTypeT atype,
                              SaHpiEntryIdT aid,
                              SaHpiEntryIdT& next_aid,
                              SaHpiIdrAreaHeaderT& hdr ) const
{
    if ( aid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    next_aid = SAHPI_LAST_ENTRY;

    Areas::const_iterator i, j;
    if ( atype == SAHPI_IDR_AREATYPE_UNSPECIFIED ) {
        i = std::find_if( m_areas.begin(), m_areas.end(), AreaIdPred( aid ) );
        if ( i == m_areas.end() ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        (*i)->GetHeader( hdr );
        ++i;
        if ( i != m_areas.end() ) {
            next_aid = (*i)->GetId();
        }
    } else {
        i = std::find_if( m_areas.begin(),
                          m_areas.end(),
                          AreaIdTypePred( aid, atype ) );
        if ( i == m_areas.end() ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        (*i)->GetHeader( hdr );
        ++i;
        if ( i != m_areas.end() ) {
            j = std::find_if( i,
                              m_areas.end(),
                              AreaIdTypePred( SAHPI_FIRST_ENTRY, atype ) );
            if ( j != m_areas.end() ) {
                next_aid = (*j)->GetId();
            }
        }
    }

    return SA_OK;
}

SaErrorT cInventory::AddArea( SaHpiIdrAreaTypeT atype,
                              SaHpiEntryIdT& aid )
{
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( atype == SAHPI_IDR_AREATYPE_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    AreaMaxId max_id = std::for_each( m_areas.begin(),
                                      m_areas.end(),
                                      AreaMaxId() );
    aid = max_id.value + 1;
    m_areas.push_back( new cArea( m_update_count, aid, atype ) );
    ++m_update_count;

    return SA_OK;
}

SaErrorT cInventory::AddAreaById( SaHpiEntryIdT aid,
                                  SaHpiIdrAreaTypeT atype )
{
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( atype == SAHPI_IDR_AREATYPE_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    if ( aid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    cArea * area;

    if ( aid == SAHPI_FIRST_ENTRY ) {
        AreaMaxId max_id = std::for_each( m_areas.begin(),
                                          m_areas.end(),
                                          AreaMaxId() );
        SaHpiEntryIdT new_aid = max_id.value + 1;
        area = new cArea( m_update_count, new_aid, atype );
        m_areas.push_front( area );
    } else {
        if ( GetArea( aid ) ) {
            return SA_ERR_HPI_DUPLICATE;
        }
        area = new cArea( m_update_count, aid, atype );
        m_areas.push_back( area );
    }

    ++m_update_count;

    return SA_OK;
}

SaErrorT cInventory::DeleteAreaById( SaHpiEntryIdT aid )
{
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( aid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    cArea * area = GetArea( aid );

    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    if ( !area->CanBeDeleted() ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    // NB: if we pass aid to AreaIdPred
    //     and aid is SAHPI_FIRST_ENTRY
    //     then all areas will be deleted.
    m_areas.remove_if( AreaIdPred( area->GetId() ) );
    delete area;
    ++m_update_count;

    return SA_OK;
}


void cInventory::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( cArea::classname + "-XXX" );
}

bool cInventory::CreateChild( const std::string& name )
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

    if ( cname == cArea::classname ) {
        if ( !GetArea( id ) ) {
            m_areas.push_back( new cArea( m_update_count, id ) );
            ++m_update_count;
            return true;
        }
    }

    return false;
}

bool cInventory::RemoveChild( const std::string& name )
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

    if ( cname == cArea::classname ) {
        cArea * area = GetArea( id );
        if ( area ) {
            // NB: if we pass id to AreaIdPred
            //     and id is SAHPI_FIRST_ENTRY
            //     then all areas will be deleted.
            m_areas.remove_if( AreaIdPred( id ) );
            delete area;
            ++m_update_count;
            return true;
        }
    }

    return false;
}

void cInventory::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );

    ObjectCollector collector( children );
    std::for_each( m_areas.begin(), m_areas.end(), collector );
}

void cInventory::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    vars << "ReadOnly"
         << dtSaHpiBoolT
         << DATA( m_readonly )
         << VAR_END();
}

void cInventory::AfterVarSet( const std::string& var_name )
{
    cInstrument::AfterVarSet( var_name );

    ++m_update_count;
}


}; // namespace TA

