/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTRESOURCELITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef RESOURCE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define RESOURCE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <map>

#include <SaHpi.h>

#include "instruments.h"
#include "object.h"
#include "timers.h"


namespace TA {


/**************************************************************
 * class cResource
 *************************************************************/
class cHandler;
class cLog;

class cResource : public cObject, public cInstruments, private cTimerCallback
{
public:

    explicit cResource( cHandler& handler,
                        const SaHpiEntityPathT& ep );
    virtual ~cResource();

public:

    const SaHpiRptEntryT& GetRptEntry() const;
    SaHpiResourceIdT GetResourceId() const;
    const SaHpiEntityPathT& GetEntityPath() const;
    bool IsFailed() const;
    void UpdateCaps( SaHpiCapabilitiesT caps );

    cLog * GetLog() const;

public:  // HPI interface

    SaErrorT SetTag( const SaHpiTextBufferT& tag );
    SaErrorT SetSeverity( SaHpiSeverityT sev );
    SaErrorT CancelHsPolicy( const SaHpiTimeoutT& timeout );
    SaErrorT GetAutoExtractTimeout( SaHpiTimeoutT& timeout ) const;
    SaErrorT SetAutoExtractTimeout( const SaHpiTimeoutT& timeout );
    SaErrorT GetHsState( SaHpiHsStateT& state ) const;
    SaErrorT SetHsState( SaHpiHsStateT state );
    SaErrorT RequestHsAction( SaHpiHsActionT action );
    SaErrorT GetHsIndicatorState( SaHpiHsIndicatorStateT& state ) const;
    SaErrorT SetHsIndicatorState( const SaHpiHsIndicatorStateT& state );
    SaErrorT GetPowerState( SaHpiPowerStateT& state );
    SaErrorT SetPowerState( const SaHpiPowerStateT& state );
    SaErrorT ControlParm( SaHpiParmActionT action );
    SaErrorT GetLoadId( SaHpiLoadIdT& load_id ) const;
    SaErrorT SetLoadId( const SaHpiLoadIdT& load_id );
    SaErrorT GetResetState( SaHpiResetActionT& action ) const;
    SaErrorT SetResetState( const SaHpiResetActionT& action );

public: // Event generation

    void PostEvent( SaHpiEventTypeT type,
                    const SaHpiEventUnionT& data,
                    SaHpiSeverityT severity,
                    const InstrumentList& updates  = InstrumentList(),
                    const InstrumentList& removals = InstrumentList() ) const;

private:

    cResource( const cResource& );
    cResource& operator =( const cResource& );

private: // Event Generation

    void PostResourceEvent( SaHpiResourceEventTypeT type ) const;
    void PostHsEvent( SaHpiHsStateT current, SaHpiHsStateT prev ) const;

private: // cObject virtual functions

    virtual void BeforeVisibilityChange();
    virtual void AfterVisibilityChange();
    virtual void GetNewNames( cObject::NewNames& names ) const;
    virtual bool CreateChild( const std::string& name );
    virtual bool RemoveChild( const std::string& name );
    virtual void GetChildren( cObject::Children& children ) const;
    virtual void GetVars( cVars& vars );
    virtual void BeforeVarSet( const std::string& var_name );
    virtual void AfterVarSet( const std::string& var_name );

private: // cTimerCallback virtual functions

    void CreateLog();
    void RemoveLog();
    virtual void TimerEvent();

private:

    void GetTimeouts( SaHpiTimeoutT& ai_timeout,
                      SaHpiTimeoutT& ae_timeout ) const;
    void CommitChanges();

private: // data

    cHandler&              m_handler;
    cLog *                 m_log;
    SaHpiRptEntryT         m_rpte;
    SaHpiBoolT             m_failed;
    SaHpiBoolT             m_new_failed;
    SaHpiTimeoutT          m_ae_timeout;
    SaHpiHsStateT          m_prev_hs_state;
    SaHpiHsStateT          m_hs_state;
    SaHpiHsStateT          m_new_hs_state;
    SaHpiHsIndicatorStateT m_hs_ind_state;
    SaHpiLoadIdT           m_load_id;
    SaHpiResetActionT      m_rst_state;
    SaHpiPowerStateT       m_pwr_state;

    // If Power State is set to POWER_CYCLE then for PwrCycleDuration times
    // saHpiResourcePowerStateGet() call will report ON (or OFF)
    // and after that it will report OFF (or ON).
    static const int       PwrCycleDuration = 3;

    int                    m_pwr_cycle_cnt;
};


}; // namespace TA


#endif // RESOURCE_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

