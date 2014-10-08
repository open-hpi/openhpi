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

#ifndef OBJECT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define OBJECT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>

#include <SaHpi.h>

#include "vars.h"


namespace TA {


/**************************************************************
 * class cObject
 *************************************************************/
class cObject
{
public:

    typedef std::list<std::string> NewNames;
    typedef std::list<cObject *> Children;

public:

    const std::string& GetName() const
    {
        return m_name;
    }

    bool IsVisible() const
    {
        return m_visible != SAHPI_FALSE;
    }

    bool SetVisible( bool value );

    cObject * GetChild( const std::string& name ) const;
    bool GetVar( const std::string& name, Var& var );

public: // Extensible interface

    virtual void GetNB( std::string& nb ) const;
    virtual void BeforeVisibilityChange();
    virtual void AfterVisibilityChange();
    virtual void GetNewNames( NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( Children& children ) const;
    virtual void GetVars( cVars& vars );
    virtual void BeforeVarSet( const std::string& var_name );
    virtual void AfterVarSet( const std::string& var_name );

protected:

    explicit cObject( const std::string& name, SaHpiBoolT visible = SAHPI_TRUE );
    virtual ~cObject();

private:

    cObject( const cObject& );
    cObject& operator =( const cObject& );

private: // data

    const std::string m_name;
    SaHpiBoolT        m_visible;
    SaHpiBoolT        m_new_visible;
    bool              m_visible_ro;
};


}; // namespace TA


#endif // OBJECT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

