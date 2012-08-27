/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTFIELDLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include "codec.h"
#include "field.h"
#include "utils.h"


namespace TA {


/**************************************************************
 * class cField
 *************************************************************/
const std::string cField::classname( "field" );

cField::cField( volatile SaHpiUint32T& update_count,
                SaHpiEntryIdT id )
    : cObject( AssembleNumberedObjectName( classname, id ) ),
      m_id( id ),
      m_type( SAHPI_IDR_FIELDTYPE_CUSTOM ),
      m_readonly( SAHPI_FALSE ),
      m_update_count( update_count )
{
    MakeHpiTextBuffer( m_data, "" );
}

cField::~cField()
{
    // empty
}

void cField::Get( SaHpiEntryIdT& id,
                  SaHpiIdrFieldTypeT& type,
                  SaHpiBoolT& readonly,
                  SaHpiTextBufferT& data ) const
{
    id       = m_id;
    type     = m_type;
    readonly = m_readonly ? SAHPI_TRUE : SAHPI_FALSE;
    data     = m_data;
}


void cField::Set( SaHpiIdrFieldTypeT type, const SaHpiTextBufferT& data )
{
    m_type = type;
    m_data = data;
}


void cField::GetVars( cVars& vars )
{
    cObject::GetVars( vars );

    vars << "FieldId"
         << dtSaHpiEntryIdT
         << DATA( m_id )
         << READONLY()
         << VAR_END();
    vars << "FieldType"
         << dtSaHpiIdrFieldTypeT
         << DATA( m_type )
         << VAR_END();
    vars << "ReadOnly"
         << dtSaHpiBoolT
         << DATA( m_readonly )
         << VAR_END();
    vars << "Field"
         << dtSaHpiTextBufferT
         << DATA( m_data )
         << VAR_END();
}

void cField::AfterVarSet( const std::string& var_name )
{
    cObject::AfterVarSet( var_name );

    ++m_update_count;
}


}; // namespace TA

