/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTWATCHDOGLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "instrument.h"
#include "timers.h"


namespace TA {


/**************************************************************
 * class cWatchdog
 *************************************************************/
class cWatchdog : public cInstrument, private cTimerCallback
{
public:

    static const std::string classname;

    explicit cWatchdog( cHandler& handler,
                        cResource& resource,
                        SaHpiWatchdogNumT num );
    virtual ~cWatchdog();

public:  // HPI interface

    SaErrorT Get( SaHpiWatchdogT& wdt ) const;
    SaErrorT Set( const SaHpiWatchdogT& wdt );
    SaErrorT Reset();

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );

private:

    cWatchdog( const cWatchdog& );
    cWatchdog& operator =( const cWatchdog& );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_WATCHDOG;
    }

private: // cTimerCallback virtual functions

    virtual void TimerEvent();

private:

    void ProcessTick();
    void PostEvent( SaHpiWatchdogActionEventT ae );

private: // data

    const SaHpiWatchdogRecT& m_rec;
    SaHpiWatchdogT m_wdt;
};


}; // namespace TA


#endif // WATCHDOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

