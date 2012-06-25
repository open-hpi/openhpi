/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTOBJECTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <list>
#include <iterator>
#include <string>

#include "object.h"
#include "vars.h"


namespace TA {


/**************************************************************
 * class cObject
 *************************************************************/
cObject::cObject( const std::string& name, SaHpiBoolT visible )

    : m_name( name ),
      m_visible( visible ),
      m_new_visible( visible ),
      m_visible_ro( visible != SAHPI_FALSE )
      // If object is visible at creation time,
      // It will be always visible
{
    // empty
}

cObject::~cObject()
{
    // empty
}

bool cObject::SetVisible( bool value )
{
    if ( m_visible_ro ) {
        return false;
    }
    m_new_visible = value ? SAHPI_TRUE : SAHPI_FALSE;
    if ( m_visible != m_new_visible ) {
        BeforeVisibilityChange();
        m_visible = m_new_visible;
        AfterVisibilityChange();
    }

    return true;
}

cObject * cObject::GetChild( const std::string& name ) const
{
    Children children;
    GetChildren( children );
    Children::const_iterator i    = children.begin();
    Children::const_iterator end  = children.end();
    for ( ; i != end; ++i ) {
        if ( name == (*i)->GetName() ) {
            return *i;
        }
    }

    return 0;
}

bool cObject::GetVar( const std::string& name, Var& var )
{
    cVars vars;
    GetVars( vars );
    VarIter vi, vend;
    for ( vi = vars.begin(), vend = vars.end(); vi != vend; ++vi ) {
        if ( name == vi->name ) {
            var = *vi;
            return true;
        }
    }

    return false;
}


// Extensible interface
void cObject::GetNB( std::string& nb ) const
{
    nb.clear();
}

void cObject::BeforeVisibilityChange()
{
    // extend in inherited classes
}

void cObject::AfterVisibilityChange()
{
    // extend in inherited classes
}

void cObject::GetNewNames( NewNames& names ) const
{
    // extend in inherited classes
}

bool cObject::CreateChild( const std::string& name )
{
    // extend in inherited classes
    return false;
}

bool cObject::RemoveChild( const std::string& name )
{
    // extend in inherited classes
    return false;
}

void cObject::GetChildren( Children& children ) const
{
    // extend in inherited classes
}

void cObject::GetVars( cVars& vars )
{
    vars << "Visible"
         << dtSaHpiBoolT
         << DATA( m_visible, m_new_visible )
         << READONLY_IF( m_visible_ro )
         << VAR_END();

    // extend in inherited classes
}

void cObject::BeforeVarSet( const std::string& var_name )
{
    m_new_visible = m_visible;
    // extend in inherited classes
}

void cObject::AfterVarSet( const std::string& var_name )
{
    if ( m_visible != m_new_visible ) {
        BeforeVisibilityChange();
        m_visible = m_new_visible;
        AfterVisibilityChange();
    }
    // extend in inherited classes
}


}; // namespace TA

