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

#ifndef INSTRUMENTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define INSTRUMENTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <map>
#include <string>

#include <SaHpi.h>

#include "instrument.h"
#include "object.h"


namespace TA {


/**************************************************************
 * class cInstruments
 *************************************************************/
class cHandler;
class cResource;
class cControl;
class cSensor;
class cInventory;
class cWatchdog;
class cAnnunciator;
class cDimi;
class cFumi;

class cInstruments
{
public:

    cControl * GetControl( SaHpiCtrlNumT num ) const;
    cSensor * GetSensor( SaHpiSensorNumT num ) const;
    cInventory * GetInventory( SaHpiIdrIdT num ) const;

    cWatchdog * GetWatchdog( SaHpiWatchdogNumT num ) const;
    cAnnunciator * GetAnnunciator( SaHpiAnnunciatorNumT num ) const;
    cDimi * GetDimi( SaHpiDimiNumT num ) const;
    cFumi * GetFumi( SaHpiFumiNumT num ) const;

    void GetAllInstruments( InstrumentList& all ) const;

protected:

    explicit cInstruments( cHandler& handler, cResource& resource );
    virtual ~cInstruments();

protected:

    void GetNewNames( cObject::NewNames& names ) const;
    void GetChildren( cObject::Children& children ) const;

    bool CreateInstrument( const std::string& name );
    bool RemoveInstrument( const std::string& name );

private:

    cInstruments( const cInstruments& );
    cInstruments& operator =( const cInstruments& );

private: // data

    cHandler&  m_handler;
    cResource& m_resource;

    typedef std::map<SaHpiCtrlNumT, cControl *> Controls;
    Controls m_controls;

    typedef std::map<SaHpiSensorNumT, cSensor *> Sensors;
    Sensors m_sensors;

    typedef std::map<SaHpiIdrIdT, cInventory *> Inventories;
    Inventories m_invs;

    typedef std::map<SaHpiWatchdogNumT, cWatchdog *> Watchdogs;
    Watchdogs m_wdts;

    typedef std::map<SaHpiAnnunciatorNumT, cAnnunciator *> Annunciators;
    Annunciators m_anns;

    typedef std::map<SaHpiDimiNumT, cDimi *> Dimis;
    Dimis m_dimis;

    typedef std::map<SaHpiFumiNumT, cFumi *> Fumis;
    Fumis m_fumis;
};


}; // namespace TA


#endif // INSTRUMENTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

