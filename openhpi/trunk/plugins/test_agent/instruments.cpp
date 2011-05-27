/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTINSTRUMENTSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <algorithm>
#include <map>
#include <string>

#include "codec.h"
#include "control.h"
#include "instruments.h"
#include "inventory.h"
#include "sensor.h"


namespace TA {


/**************************************************************
 * Helpers
 *************************************************************/
template <typename Map>
struct Deleter
{
    void operator ()( typename Map::value_type& v )
    {
        delete v.second;
    }
};

template <typename Map>
struct InstrumentCollector
{
    explicit InstrumentCollector( InstrumentList& _all )
        : all( _all )
    {
        // empty
    }

    void operator ()( const typename Map::value_type& v )
    {
        all.push_back( v.second );
    }

    InstrumentList& all;
};

template <typename Map>
struct ObjectCollector
{
    explicit ObjectCollector( cObject::Children& _children )
        : children( _children )
    {
        // empty
    }

    void operator ()( const typename Map::value_type& v )
    {
        children.push_back( v.second );
    }

    cObject::Children& children;
};


/**************************************************************
 * class cInstruments
 *************************************************************/
cInstruments::cInstruments( cResource& resource )
    : m_resource( resource )
{
    // empty
}

cInstruments::~cInstruments()
{

    Deleter<Controls> cdeleter;
    std::for_each( m_controls.begin(), m_controls.end(), cdeleter );
    m_controls.clear();

    Deleter<Sensors> sdeleter;
    std::for_each( m_sensors.begin(), m_sensors.end(), sdeleter );
    m_sensors.clear();

    Deleter<Inventories> ideleter;
    std::for_each( m_invs.begin(), m_invs.end(), ideleter );
    m_invs.clear();
}

cControl * cInstruments::GetControl( SaHpiCtrlNumT num ) const
{
    Controls::const_iterator iter = m_controls.find( num );
    if ( iter != m_controls.end() ) {
        cControl * ctrl = iter->second;
        return ctrl;
    }

    return 0;
}

cSensor * cInstruments::GetSensor( SaHpiSensorNumT num ) const
{
    Sensors::const_iterator iter = m_sensors.find( num );
    if ( iter != m_sensors.end() ) {
        cSensor * sen = iter->second;
        return sen;
    }

    return 0;
}

cInventory * cInstruments::GetInventory( SaHpiIdrIdT num ) const
{
    Inventories::const_iterator iter = m_invs.find( num );
    if ( iter != m_invs.end() ) {
        cInventory * inv = iter->second;
        return inv;
    }

    return 0;
}

void cInstruments::GetAllInstruments( InstrumentList& all ) const
{
    InstrumentCollector<Controls> ccollector( all );
    std::for_each( m_controls.begin(), m_controls.end(), ccollector );

    InstrumentCollector<Sensors> scollector( all );
    std::for_each( m_sensors.begin(), m_sensors.end(), scollector );

    InstrumentCollector<Inventories> icollector( all );
    std::for_each( m_invs.begin(), m_invs.end(), icollector );
}

void cInstruments::GetNewNames( cObject::NewNames& names ) const
{
    names.push_back( cControl::classname + "-XXX" );
    names.push_back( cSensor::classname + "-XXX" );
    names.push_back( cInventory::classname + "-XXX" );
}

void cInstruments::GetChildren( cObject::Children& children ) const
{
    ObjectCollector<Controls> ccollector( children );
    std::for_each( m_controls.begin(), m_controls.end(), ccollector );

    ObjectCollector<Sensors> scollector( children );
    std::for_each( m_sensors.begin(), m_sensors.end(), scollector );

    ObjectCollector<Inventories> icollector( children );
    std::for_each( m_invs.begin(), m_invs.end(), icollector );
}

bool cInstruments::CreateInstrument( const std::string& name )
{
    std::string cname;
    SaHpiUint32T num;
    bool rc = DisassembleNumberedObjectName( name, cname, num );
    if ( !rc ) {
        return false;
    }

    if ( cname == cControl::classname ) {
        if ( !GetControl( num ) ) {
            m_controls[num] = new cControl( m_resource, num );
            return true;
        }
    }
    if ( cname == cSensor::classname ) {
        if ( !GetSensor( num ) ) {
            m_sensors[num] = new cSensor( m_resource, num );
            return true;
        }
    }
    if ( cname == cInventory::classname ) {
        if ( !GetInventory( num ) ) {
            m_invs[num] = new cInventory( m_resource, num );
            return true;
        }
    }

    return false;
}

bool cInstruments::RemoveInstrument( const std::string& name )
{
    std::string classname;
    SaHpiUint32T num;
    bool rc = DisassembleNumberedObjectName( name, classname, num );
    if ( !rc ) {
        return false;
    }

    if ( classname == cControl::classname ) {
        cControl * ctrl = GetControl( num );
        if ( ctrl ) {
            m_controls.erase( num );
            delete ctrl;
            return true;
        }
    }
    if ( classname == cSensor::classname ) {
        cSensor * sen = GetSensor( num );
        if ( sen ) {
            m_sensors.erase( num );
            delete sen;
            return true;
        }
    }
    if ( classname == cInventory::classname ) {
        cInventory * inv = GetInventory( num );
        if ( inv ) {
            m_invs.erase( num );
            delete inv;
            return true;
        }
    }

    return false;
}


}; // namespace TA

