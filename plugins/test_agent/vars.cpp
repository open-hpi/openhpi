/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTVARLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include "vars.h"


namespace TA {


/**************************************************************
 * cVars
 *************************************************************/
cVars::cVars()
    : m_cond( true ),
      m_ro_cond( false )
{
    // empty
}

cVars::~cVars()
{
    // empty
}

cVars& cVars::operator <<( const IF& i_f )
{
    m_cond = i_f.cond;

    return *this;
}

cVars& cVars::operator <<( const std::string& name )
{
    m_pending.name = name;

    return *this;
}

cVars& cVars::operator <<( eDataType type )
{
    m_pending.type = type;

    return *this;
}

cVars& cVars::operator <<( const READONLY& )
{
    m_ro_cond = true;

    return *this;
}

cVars& cVars::operator <<( const READONLY_IF& rif )
{
    m_ro_cond = rif.cond;

    return *this;
}

cVars& cVars::operator <<( const DATA& d )
{
    m_pending.rdata = d.rdata;
    m_pending.wdata = d.wdata;

    return *this;
}

cVars& cVars::operator <<( const VAR_END& )
{
    if ( m_cond ) {
        if ( m_ro_cond ) {
            m_pending.wdata = 0;
        }
        push_back( m_pending );
    }
    m_cond    = true;
    m_ro_cond = false;
    m_pending = Var();

    return *this;
}


}; // namespace TA

