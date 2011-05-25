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
class cResource;
class cControl;
class cSensor;

class cInstruments
{
public:

    cControl * GetControl( SaHpiCtrlNumT num ) const;
    cSensor * GetSensor( SaHpiSensorNumT num ) const;

    void GetAllInstruments( InstrumentList& all ) const;

protected:

    explicit cInstruments( cResource& resource );
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

    cResource& m_resource;

    typedef std::map<SaHpiCtrlNumT, cControl *> Controls;
    Controls m_controls;

    typedef std::map<SaHpiSensorNumT, cSensor *> Sensors;
    Sensors m_sensors;
};


}; // namespace TA


#endif // INSTRUMENTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

