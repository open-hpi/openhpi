/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTAREALITY or FITNESS FOR A PARTICULAR PURPOSE.  This
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
#include "field.h"


namespace TA {


/**************************************************************
 * Functors for working with fields
 *************************************************************/
struct FieldDeleter
{
    void operator ()( cField * field )
    {
        delete field;
    }
};

struct FieldIdPred
{
    explicit FieldIdPred( SaHpiEntryIdT _id )
        : id( _id )
    {
        // empty
    }

    bool operator ()( const cField * field ) const
    {
        return ( id == SAHPI_FIRST_ENTRY ) ||
               ( id == field->GetId() );
    }

    SaHpiEntryIdT id;
};

struct FieldIdTypePred
{
    explicit FieldIdTypePred( SaHpiEntryIdT _id, SaHpiIdrFieldTypeT _type )
        : id( _id ), type( _type )
    {
        // empty
    }

    bool operator ()( const cField * field ) const
    {
        if ( type == field->GetType() ) {
            return ( id == SAHPI_FIRST_ENTRY ) ||
                   ( id == field->GetId() );
        } else {
            return false;
        }
    }

    SaHpiEntryIdT     id;
    SaHpiIdrFieldTypeT type;
};

struct FieldReadOnlyPred
{
    bool operator ()( const cField * field ) const
    {
        return field->IsReadOnly();
    }
};

struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( cField * field )
    {
        children.push_back( field );
    }

    cObject::Children& children;
};

struct FieldMaxId
{
    explicit FieldMaxId()
        : value( 0 )
    {
        // empty
    }

    void operator ()( const cField * field )
    {
        value = std::max( value, field->GetId() );
    }

    SaHpiEntryIdT value;
};


/**************************************************************
 * class cArea
 *************************************************************/
const std::string cArea::classname( "area" );

cArea::cArea( volatile SaHpiUint32T& update_count,
              SaHpiEntryIdT id,
              SaHpiIdrAreaTypeT type )
    : cObject( AssembleNumberedObjectName( classname, id ) ),
      m_id( id ),
      m_type( type ),
      m_readonly( SAHPI_FALSE ),
      m_update_count( update_count )
{
    // empty
}

cArea::~cArea()
{
    FieldDeleter deleter;
    std::for_each( m_fields.begin(), m_fields.end(), deleter );
    m_fields.clear();
}


void cArea::GetHeader( SaHpiIdrAreaHeaderT& hdr ) const
{
    hdr.AreaId    = m_id;
    hdr.Type      = m_type;
    hdr.ReadOnly  = m_readonly;
    hdr.NumFields = m_fields.size();
}

bool cArea::CanBeDeleted() const
{
    if ( m_readonly != SAHPI_FALSE ) {
        return false;
    }

    // contains ReadOnly field?
    Fields::const_iterator i;
    i = std::find_if( m_fields.begin(), m_fields.end(), FieldReadOnlyPred() );
    if ( i != m_fields.end() ) {
        return false;
    }

    return true;
}

cField * cArea::GetField( SaHpiEntryIdT fid ) const
{
    Fields::const_iterator i;
    i = std::find_if( m_fields.begin(), m_fields.end(), FieldIdPred( fid ) );
    if ( i == m_fields.end() ) {
        return 0;
    }

    return *i;
}


// HPI Interface
SaErrorT cArea::GetField( SaHpiIdrFieldTypeT ftype,
                          SaHpiEntryIdT fid,
                          SaHpiEntryIdT& next_fid,
                          SaHpiIdrFieldT& field ) const
{
    if ( fid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    next_fid = SAHPI_LAST_ENTRY;

    Fields::const_iterator i, j;
    if ( ftype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) {
        i = std::find_if( m_fields.begin(), m_fields.end(), FieldIdPred( fid ) );
        if ( i == m_fields.end() ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        field.AreaId = m_id;
        (*i)->Get( field.FieldId, field.Type, field.ReadOnly, field.Field );
        ++i;
        if ( i != m_fields.end() ) {
            next_fid = (*i)->GetId();
        }
    } else {
        i = std::find_if( m_fields.begin(),
                          m_fields.end(),
                          FieldIdTypePred( fid, ftype ) );
        if ( i == m_fields.end() ) {
            return SA_ERR_HPI_NOT_PRESENT;
        }
        field.AreaId = m_id;
        (*i)->Get( field.FieldId, field.Type, field.ReadOnly, field.Field );
        ++i;
        if ( i != m_fields.end() ) {
            j = std::find_if( i,
                              m_fields.end(),
                              FieldIdTypePred( SAHPI_FIRST_ENTRY, ftype ) );
            if ( j != m_fields.end() ) {
                next_fid = (*j)->GetId();
            }
        }
    }

    return SA_OK;
}

SaErrorT cArea::AddField( SaHpiIdrFieldTypeT ftype,
                          const SaHpiTextBufferT& fdata,
                          SaHpiEntryIdT& fid )
{
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( ftype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    FieldMaxId max_id = std::for_each( m_fields.begin(),
                                       m_fields.end(),
                                       FieldMaxId() );
    fid = max_id.value + 1;
    cField * field = new cField( m_update_count, fid );
    field->Set( ftype, fdata );
    m_fields.push_back( field );
    ++m_update_count;

    fid = field->GetId();

    return SA_OK;
}

SaErrorT cArea::AddFieldById( SaHpiEntryIdT fid,
                              SaHpiIdrFieldTypeT ftype,
                              const SaHpiTextBufferT& fdata )
{
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( ftype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_DATA;
    }
    if ( fid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    cField * field;

    if ( fid == SAHPI_FIRST_ENTRY ) {
        FieldMaxId max_id = std::for_each( m_fields.begin(),
                                           m_fields.end(),
                                           FieldMaxId() );
        SaHpiEntryIdT new_fid = max_id.value + 1;
        field = new cField( m_update_count, new_fid );
        m_fields.push_front( field );
    } else {
        if ( GetField( fid ) ) {
            return SA_ERR_HPI_DUPLICATE;
        }
        field = new cField( m_update_count, fid );
        m_fields.push_back( field );
    }

    field->Set( ftype, fdata );
    ++m_update_count;

    return SA_OK;
}

SaErrorT cArea::SetField( SaHpiEntryIdT fid,
                          SaHpiIdrFieldTypeT ftype,
                          const SaHpiTextBufferT& fdata )
{
    if ( ftype == SAHPI_IDR_FIELDTYPE_UNSPECIFIED ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    cField * field = GetField( fid );

    if ( !field ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    if ( field->IsReadOnly() ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    field->Set( ftype, fdata );

    return SA_OK;
}

SaErrorT cArea::DeleteFieldById( SaHpiEntryIdT fid )
{
    if ( fid == SAHPI_LAST_ENTRY ) {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    cField * field = GetField( fid );

    if ( !field ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }
    if ( m_readonly != SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }
    if ( field->IsReadOnly() ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    // NB: if we pass id to FieldIdPred
    //     and id is SAHPI_FIRST_ENTRY
    //     then all fields will be deleted.
    m_fields.remove_if( FieldIdPred( field->GetId() ) );
    delete field;
    ++m_update_count;

    return SA_OK;
}


void cArea::GetNewNames( cObject::NewNames& names ) const
{
    cObject::GetNewNames( names );
    names.push_back( cField::classname + "-XXX" );
}

bool cArea::CreateChild( const std::string& name )
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

    if ( cname == cField::classname ) {
        if ( !GetField( id ) ) {
            m_fields.push_back( new cField( m_update_count, id ) );
            ++m_update_count;
            return true;
        }
    }

    return false;
}

bool cArea::RemoveChild( const std::string& name )
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

    if ( cname == cField::classname ) {
        cField * field = GetField( id );
        if ( field ) {
            // NB: if we pass id to FieldIdPred
            //     and id is SAHPI_FIRST_ENTRY
            //     then all fields will be deleted.
            m_fields.remove_if( FieldIdPred( id ) );
            delete field;
            ++m_update_count;
            return true;
        }
    }

    return false;
}

void cArea::GetChildren( Children& children ) const
{
    cObject::GetChildren( children );

    ObjectCollector collector( children );
    std::for_each( m_fields.begin(), m_fields.end(), collector );
}

void cArea::GetVars( cVars& vars )
{
    cObject::GetVars( vars );

    vars << "AreaId"
         << dtSaHpiEntryIdT
         << DATA( m_id )
         << READONLY()
         << VAR_END();
    vars << "AreaType"
         << dtSaHpiIdrAreaTypeT
         << DATA( m_type )
         << VAR_END();
    vars << "ReadOnly"
         << dtSaHpiBoolT
         << DATA( m_readonly )
         << VAR_END();
}

void cArea::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );

    ++m_update_count;
}


}; // namespace TA

