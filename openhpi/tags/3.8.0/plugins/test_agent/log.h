/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2012
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTLOGLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef LOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define LOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>

#include <SaHpi.h>

#include "object.h"


namespace TA {


/**************************************************************
 * class cLog
 *************************************************************/
class cLog : public cObject
{
public:

    static const std::string classname;

    struct Entry
    {
        SaHpiEventLogEntryT entry;
        SaHpiRdrT           rdr;
        SaHpiRptEntryT      rpte;
    };

    explicit cLog();
    virtual ~cLog();

    static SaHpiCapabilitiesT RequiredResourceCap()
    {
        return SAHPI_CAPABILITY_EVENT_LOG;
    }

    void AddEntry( SaHpiEventTypeT type,
                   const SaHpiEventUnionT& data,
                   SaHpiSeverityT severity,
                   const SaHpiRdrT * rdr,
                   const SaHpiRptEntryT * rpte );

public:  // HPI interface

    SaErrorT GetInfo( SaHpiEventLogInfoT& info );
    SaErrorT GetCapabilities( SaHpiEventLogCapabilitiesT& caps ) const;
    SaErrorT SetTime( SaHpiTimeT t );
    SaErrorT AddEntry( const SaHpiEventT& event );
    SaErrorT GetEntry( SaHpiEventLogEntryIdT eid,
                       SaHpiEventLogEntryIdT& prev_eid,
                       SaHpiEventLogEntryIdT& next_eid,
                       SaHpiEventLogEntryT& entry,
                       SaHpiRdrT& rdr,
                       SaHpiRptEntryT& rpte );
    SaErrorT Clear();
    SaErrorT SetState( SaHpiBoolT enable );
    SaErrorT ResetOverflow();

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cLog( const cLog& );
    cLog& operator =( const cLog& );

private:

    void Update();
    void SyncInfo();
    bool AddEntry( const SaHpiEventT& event,
                   const SaHpiRdrT * rdr,
                   const SaHpiRptEntryT * rpte );


private: // data

    SaHpiEventLogInfoT         m_info;
    SaHpiEventLogCapabilitiesT m_caps;
    SaHpiTimeT                 m_delta;
    SaHpiEventLogEntryIdT      m_next_eid;

    typedef std::list<Entry> Entries;
    Entries m_entries;
};


}; // namespace TA


#endif // LOG_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

