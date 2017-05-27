/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2012
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTBANKLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef BANK_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define BANK_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <stddef.h>

#include <list>
#include <string>

#include <SaHpi.h>

#include "object.h"
#include "timers.h"


namespace TA {


/**************************************************************
 * class cBank
 *************************************************************/
class cHandler;
class cFumi;

class cBank : public cObject, private cTimerCallback
{
public:

    static const std::string classname;

    explicit cBank( cHandler& handler, cFumi& fumi, SaHpiBankNumT num );
    virtual ~cBank();

public:

    SaHpiUint32T Position() const;
    void SetPosition( SaHpiUint32T pos );
    SaHpiFumiBankStateT State() const;

public: // HPI interface

    SaErrorT SetSource( const SaHpiTextBufferT& uri );
    SaErrorT StartSourceValidation();
    SaErrorT GetSourceInfo( SaHpiFumiSourceInfoT& info ) const;
    SaErrorT GetSourceComponentInfo( SaHpiEntryIdT eid,
                                     SaHpiEntryIdT& next_eid,
                                     SaHpiFumiComponentInfoT& info ) const;
    SaErrorT GetTargetInfo( SaHpiFumiBankInfoT& info ) const;
    SaErrorT GetTargetComponentInfo( SaHpiEntryIdT eid,
                                     SaHpiEntryIdT& next_eid,
                                     SaHpiFumiComponentInfoT& info ) const;
    SaErrorT GetLogicalTargetInfo( SaHpiFumiLogicalBankInfoT& info ) const;
    SaErrorT GetLogicalTargetComponentInfo( SaHpiEntryIdT eid,
                                            SaHpiEntryIdT& next_eid,
                                            SaHpiFumiLogicalComponentInfoT& info ) const;
    SaErrorT StartBackup();
    SaErrorT StartCopy( SaHpiBankNumT dest_num );
    SaErrorT StartInstallation();
    SaErrorT GetUpgradeStatus( SaHpiFumiUpgradeStatusT& status ) const;
    SaErrorT StartTargetVerification();
    SaErrorT StartTargetMainVerification();
    SaErrorT CancelUpgrade();
    SaErrorT StartRollback();
    SaErrorT StartActivation( SaHpiBoolT pass );
    SaErrorT Cleanup();

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cBank( const cBank& );
    cBank& operator =( const cBank& );

private: // cTimerCallback virtual functions

    virtual void TimerEvent();

private:

    void ChangeStatus( SaHpiFumiUpgradeStatusT status );

    void DoValidation();
    void DoInstall();
    void DoBackup();
    void DoCopy();
    void DoVerification();
    void DoActivation();
    void DoRollback();

private: // data

    static const size_t MAX_COMPONENTS = 8;

    cHandler&                      m_handler;
    cFumi&                         m_fumi;
    SaHpiFumiBankInfoT             m_info;
    SaHpiFumiLogicalBankInfoT      m_logical_info;
    SaHpiBoolT                     m_enabled_components[MAX_COMPONENTS];
    SaHpiFumiComponentInfoT        m_components[MAX_COMPONENTS];
    SaHpiFumiLogicalComponentInfoT m_logical_components[MAX_COMPONENTS];
    SaHpiBoolT                     m_src_set;
    SaHpiFumiSourceInfoT           m_src_info;
    SaHpiBoolT                     m_enabled_src_components[MAX_COMPONENTS];
    SaHpiFumiComponentInfoT        m_src_components[MAX_COMPONENTS];
    SaHpiFumiUpgradeStatusT        m_status;

    struct
    {
        SaHpiTimeoutT action_duration;
        struct
        {
            SaHpiBoolT validate;
            SaHpiBoolT install;
            SaHpiBoolT rollback;
            SaHpiBoolT backup;
            SaHpiBoolT copy;
            SaHpiBoolT verify;
            SaHpiBoolT verifymain;
            SaHpiBoolT activate;
        } pass;
        SaHpiFumiSourceStatusT src_fail_status;
        SaHpiFumiSourceInfoT   src_info;
    } m_next;

    bool          m_verify_main;
    SaHpiBankNumT m_copy_dest_num;
};


}; // namespace TA


#endif // BANK_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

