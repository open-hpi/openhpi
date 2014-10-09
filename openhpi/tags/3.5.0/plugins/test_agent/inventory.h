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

#ifndef INVENTORY_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define INVENTORY_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cInventory
 *************************************************************/
class cArea;

class cInventory : public cInstrument
{
public:

    static const std::string classname;

    explicit cInventory( cHandler& handler, cResource& resource, SaHpiIdrIdT num );
    virtual ~cInventory();

public:

    cArea * GetArea( SaHpiEntryIdT aid ) const;

public:  // HPI interface

    SaErrorT GetInfo( SaHpiIdrInfoT& info ) const;
    SaErrorT GetArea( SaHpiIdrAreaTypeT atype,
                      SaHpiEntryIdT aid,
                      SaHpiEntryIdT& next_aid,
                      SaHpiIdrAreaHeaderT& hdr ) const;
    SaErrorT AddArea( SaHpiIdrAreaTypeT atype,
                      SaHpiEntryIdT& aid );
    SaErrorT AddAreaById( SaHpiEntryIdT aid,
                          SaHpiIdrAreaTypeT atype );
    SaErrorT DeleteAreaById( SaHpiEntryIdT aid );

protected: // cObject virtual functions

    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;
    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cInventory( const cInventory& );
    cInventory& operator =( const cInventory& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_INVENTORY_DATA;
    }

private: // data

    const SaHpiInventoryRecT& m_rec;
    SaHpiBoolT                m_readonly;
    volatile SaHpiUint32T     m_update_count;

    typedef std::list<cArea *> Areas;
    Areas m_areas;
};


}; // namespace TA


#endif // INVENTORY_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

