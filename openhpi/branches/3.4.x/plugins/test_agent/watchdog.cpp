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

#include <string>

#include "codec.h"
#include "handler.h"
#include "resource.h"
#include "structs.h"
#include "watchdog.h"


namespace TA {


/**************************************************************
 * Helper functions
 *************************************************************/
static SaHpiRdrTypeUnionT MakeDefaultWatchdogRec( SaHpiWatchdogNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiWatchdogRecT& rec = data.WatchdogRec;

    rec.WatchdogNum = num;
    rec.Oem         = 0;

    return data;
}

static SaHpiWatchdogT MakeDefaultWatchdog()
{
    SaHpiWatchdogT wdt;
    wdt.Log                = SAHPI_TRUE;
    wdt.Running            = SAHPI_FALSE;
    wdt.TimerUse           = SAHPI_WTU_OEM;
    wdt.TimerAction        = SAHPI_WA_NO_ACTION;
    wdt.PretimerInterrupt  = SAHPI_WPI_OEM;
    wdt.PreTimeoutInterval = 1000;
    wdt.TimerUseExpFlags   = 0;
    wdt.InitialCount       = 2000;
    wdt.PresentCount       = 0;

    return wdt;
}

static SaHpiWatchdogExpFlagsT TimerUseToExpFlags( SaHpiWatchdogTimerUseT use )
{
    switch ( use ) {
        case SAHPI_WTU_NONE:
            return 0;
        case SAHPI_WTU_BIOS_FRB2:
            return SAHPI_WATCHDOG_EXP_BIOS_FRB2;
        case SAHPI_WTU_BIOS_POST:
            return SAHPI_WATCHDOG_EXP_BIOS_POST;
        case SAHPI_WTU_OS_LOAD:
            return SAHPI_WATCHDOG_EXP_OS_LOAD;
        case SAHPI_WTU_SMS_OS:
            return SAHPI_WATCHDOG_EXP_SMS_OS;
        case SAHPI_WTU_OEM:
            return SAHPI_WATCHDOG_EXP_OEM;
        case SAHPI_WTU_UNSPECIFIED:
            return 0;
        default:
            return 0;
    }
}

static SaHpiWatchdogActionEventT ActionEvent( SaHpiWatchdogActionT action )
{
    switch ( action ) {
        case SAHPI_WA_NO_ACTION:
            return SAHPI_WAE_NO_ACTION;
        case SAHPI_WA_RESET:
            return SAHPI_WAE_RESET;
        case SAHPI_WA_POWER_DOWN:
            return SAHPI_WAE_POWER_DOWN;
        case SAHPI_WA_POWER_CYCLE:
            return SAHPI_WAE_POWER_CYCLE;
        default:
            return SAHPI_WAE_NO_ACTION;
    }
}


/**************************************************************
 * class cWatchdog
 *************************************************************/
const std::string cWatchdog::classname( "wdt" );

cWatchdog::cWatchdog( cHandler& handler,
                      cResource& resource,
                      SaHpiWatchdogNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_WATCHDOG_RDR,
                   MakeDefaultWatchdogRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.WatchdogRec ),
      m_wdt( MakeDefaultWatchdog() )
{
    // empty
}

cWatchdog::~cWatchdog()
{
    m_handler.CancelTimer( this );
}


// HPI interface
SaErrorT cWatchdog::Get( SaHpiWatchdogT& wdt ) const
{
    wdt = m_wdt;

    return SA_OK;
}

SaErrorT cWatchdog::Set( const SaHpiWatchdogT& wdt )
{
    if ( wdt.PreTimeoutInterval > wdt.InitialCount ) {
        return SA_ERR_HPI_INVALID_DATA;
    }

    m_wdt.Log                = wdt.Log;
    m_wdt.TimerUse           = wdt.TimerUse;
    m_wdt.TimerAction        = wdt.TimerAction;
    m_wdt.PretimerInterrupt  = wdt.PretimerInterrupt;
    m_wdt.PreTimeoutInterval = wdt.PreTimeoutInterval;
    m_wdt.TimerUseExpFlags  &= ~wdt.TimerUseExpFlags;
    m_wdt.InitialCount       = wdt.InitialCount;

    if ( wdt.Running == SAHPI_FALSE ) {
        m_handler.CancelTimer( this );
        m_wdt.Running = SAHPI_FALSE;
    } else {
        m_wdt.PresentCount = m_wdt.InitialCount;
    }

    return SA_OK;
}

SaErrorT cWatchdog::Reset()
{
    if ( m_wdt.Running != SAHPI_FALSE ) {
        if ( m_wdt.PretimerInterrupt != SAHPI_WPI_NONE ) {
            if ( m_wdt.PresentCount < m_wdt.PreTimeoutInterval ) {
                return SA_ERR_HPI_INVALID_REQUEST;
            }
        }
    }

    m_wdt.Running = SAHPI_TRUE;
    m_wdt.PresentCount = m_wdt.InitialCount;
    if ( m_wdt.InitialCount != 0 ) {
        m_handler.SetTimer( this, 1000000LL ); // 1 ms tick
    } else {
        ProcessTick();
    }

    return SA_OK;
}


// cObject virtual functions
void cWatchdog::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );
    Structs::GetVars( m_wdt, vars );
}


// cTimerCallback virtual functions
void cWatchdog::TimerEvent()
{
    if ( m_wdt.Running == SAHPI_FALSE ) {
        return;
    }
    --m_wdt.PresentCount;
    ProcessTick();
}

void cWatchdog::ProcessTick()
{
    if ( m_wdt.PretimerInterrupt != SAHPI_WPI_NONE ) {
        if ( m_wdt.PresentCount == m_wdt.PreTimeoutInterval ) {
            // Pre-timer interrupt
            PostEvent( SAHPI_WAE_TIMER_INT );
        }
    }
    if ( m_wdt.PresentCount == 0 ) {
        // Expired
        m_wdt.TimerUseExpFlags |= TimerUseToExpFlags( m_wdt.TimerUse );
        m_wdt.Running = SAHPI_FALSE;
        PostEvent( ActionEvent( m_wdt.TimerAction ) );
    }

    // Schedule next tick
    if ( m_wdt.Running != SAHPI_FALSE ) {
        m_handler.SetTimer( this, 1000000LL ); // 1 ms tick
    }
}

void cWatchdog::PostEvent( SaHpiWatchdogActionEventT ae )
{
    if ( m_wdt.Log == SAHPI_FALSE ) {
        return;
    }

    SaHpiEventUnionT data;
    SaHpiWatchdogEventT& we = data.WatchdogEvent;

    we.WatchdogNum            = m_rec.WatchdogNum;
    we.WatchdogAction         = ae;
    we.WatchdogPreTimerAction = m_wdt.PretimerInterrupt;
    we.WatchdogUse            = m_wdt.TimerUse;

    cInstrument::PostEvent( SAHPI_ET_WATCHDOG, data, SAHPI_INFORMATIONAL );
}


}; // namespace TA

