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

#include "annunciator.h"
#include "codec.h"
#include "control.h"
#include "dimi.h"
#include "fumi.h"
#include "handler.h"
#include "instruments.h"
#include "inventory.h"
#include "sensor.h"
#include "watchdog.h"


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
cInstruments::cInstruments( cHandler& handler, cResource& resource )
    : m_handler( handler ),
      m_resource( resource )
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

    Deleter<Watchdogs> wdeleter;
    std::for_each( m_wdts.begin(), m_wdts.end(), wdeleter );
    m_wdts.clear();

    Deleter<Annunciators> adeleter;
    std::for_each( m_anns.begin(), m_anns.end(), adeleter );
    m_anns.clear();

    Deleter<Dimis> ddeleter;
    std::for_each( m_dimis.begin(), m_dimis.end(), ddeleter );
    m_dimis.clear();

    Deleter<Fumis> fdeleter;
    std::for_each( m_fumis.begin(), m_fumis.end(), fdeleter );
    m_fumis.clear();
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

cWatchdog * cInstruments::GetWatchdog( SaHpiWatchdogNumT num ) const
{
    Watchdogs::const_iterator iter = m_wdts.find( num );
    if ( iter != m_wdts.end() ) {
        cWatchdog * wdt = iter->second;
        return wdt;
    }

    return 0;
}

cAnnunciator * cInstruments::GetAnnunciator( SaHpiAnnunciatorNumT num ) const
{
    Annunciators::const_iterator iter = m_anns.find( num );
    if ( iter != m_anns.end() ) {
        cAnnunciator * ann = iter->second;
        return ann;
    }

    return 0;
}

cDimi * cInstruments::GetDimi( SaHpiDimiNumT num ) const
{
    Dimis::const_iterator iter = m_dimis.find( num );
    if ( iter != m_dimis.end() ) {
        cDimi * dimi = iter->second;
        return dimi;
    }

    return 0;
}

cFumi * cInstruments::GetFumi( SaHpiFumiNumT num ) const
{
    Fumis::const_iterator iter = m_fumis.find( num );
    if ( iter != m_fumis.end() ) {
        cFumi * fumi = iter->second;
        return fumi;
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

    InstrumentCollector<Watchdogs> wcollector( all );
    std::for_each( m_wdts.begin(), m_wdts.end(), wcollector );

    InstrumentCollector<Annunciators> acollector( all );
    std::for_each( m_anns.begin(), m_anns.end(), acollector );

    InstrumentCollector<Dimis> dcollector( all );
    std::for_each( m_dimis.begin(), m_dimis.end(), dcollector );

    InstrumentCollector<Fumis> fcollector( all );
    std::for_each( m_fumis.begin(), m_fumis.end(), fcollector );
}

void cInstruments::GetNewNames( cObject::NewNames& names ) const
{
    names.push_back( cControl::classname + "-XXX" );
    names.push_back( cSensor::classname + "-XXX" );
    names.push_back( cInventory::classname + "-XXX" );
    names.push_back( cWatchdog::classname + "-XXX" );
    names.push_back( cAnnunciator::classname + "-XXX" );
    names.push_back( cDimi::classname + "-XXX" );
    names.push_back( cFumi::classname + "-XXX" );
}

void cInstruments::GetChildren( cObject::Children& children ) const
{
    ObjectCollector<Controls> ccollector( children );
    std::for_each( m_controls.begin(), m_controls.end(), ccollector );

    ObjectCollector<Sensors> scollector( children );
    std::for_each( m_sensors.begin(), m_sensors.end(), scollector );

    ObjectCollector<Inventories> icollector( children );
    std::for_each( m_invs.begin(), m_invs.end(), icollector );

    ObjectCollector<Watchdogs> wcollector( children );
    std::for_each( m_wdts.begin(), m_wdts.end(), wcollector );

    ObjectCollector<Annunciators> acollector( children );
    std::for_each( m_anns.begin(), m_anns.end(), acollector );

    ObjectCollector<Dimis> dcollector( children );
    std::for_each( m_dimis.begin(), m_dimis.end(), dcollector );

    ObjectCollector<Fumis> fcollector( children );
    std::for_each( m_fumis.begin(), m_fumis.end(), fcollector );
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
            m_controls[num] = new cControl( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cSensor::classname ) {
        if ( !GetSensor( num ) ) {
            m_sensors[num] = new cSensor( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cInventory::classname ) {
        if ( !GetInventory( num ) ) {
            m_invs[num] = new cInventory( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cWatchdog::classname ) {
        if ( !GetWatchdog( num ) ) {
            m_wdts[num] = new cWatchdog( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cAnnunciator::classname ) {
        if ( !GetAnnunciator( num ) ) {
            m_anns[num] = new cAnnunciator( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cDimi::classname ) {
        if ( !GetDimi( num ) ) {
            m_dimis[num] = new cDimi( m_handler, m_resource, num );
            return true;
        }
    }
    if ( cname == cFumi::classname ) {
        if ( !GetFumi( num ) ) {
            m_fumis[num] = new cFumi( m_handler, m_resource, num );
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
    if ( classname == cWatchdog::classname ) {
        cWatchdog * wdt = GetWatchdog( num );
        if ( wdt ) {
            m_wdts.erase( num );
            delete wdt;
            return true;
        }
    }
    if ( classname == cAnnunciator::classname ) {
        cAnnunciator * ann = GetAnnunciator( num );
        if ( ann ) {
            m_anns.erase( num );
            delete ann;
            return true;
        }
    }
    if ( classname == cDimi::classname ) {
        cDimi * dimi = GetDimi( num );
        if ( dimi ) {
            m_dimis.erase( num );
            delete dimi;
            return true;
        }
    }
    if ( classname == cFumi::classname ) {
        cFumi * fumi = GetFumi( num );
        if ( fumi ) {
            m_fumis.erase( num );
            delete fumi;
            return true;
        }
    }

    return false;
}


}; // namespace TA

