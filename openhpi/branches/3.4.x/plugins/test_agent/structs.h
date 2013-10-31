/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTSTRUCTSLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef STRUCTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define STRUCTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "vars.h"


namespace TA {

namespace Structs {


/**************************************************************
 * Helpers to get vars from common HPI structures
 *************************************************************/
void GetVars( SaHpiRptEntryT& rpte, cVars& vars );
void GetVars( SaHpiEventLogInfoT& info, cVars& vars );
void GetVars( SaHpiLoadIdT& load_id, cVars& vars );
void GetVars( SaHpiRdrT& rdr, cVars& vars );
void GetVars( SaHpiCtrlStateT& state, cVars& vars );
void GetVars( const std::string& name, SaHpiSensorReadingT& r, cVars& vars );
void GetVars( SaHpiSensorThresholdsT& ths, cVars& vars );
void GetVars( SaHpiWatchdogT& wd, cVars& vars );
void GetVars( SaHpiAnnouncementT& a, cVars& vars );
void GetVars( SaHpiDimiTestT& info, cVars& vars );
void GetVars( SaHpiFumiSpecInfoT& info, cVars& vars );
void GetVars( SaHpiFumiServiceImpactDataT& data, cVars& vars );
void GetVars( SaHpiFumiBankInfoT& info, cVars& vars );
void GetVars( const std::string& name, SaHpiFumiSourceInfoT& info, bool uri_and_status, cVars& vars );
void GetVars( const std::string& name, SaHpiFumiComponentInfoT& info, cVars& vars );
void GetVars( SaHpiFumiLogicalBankInfoT& info, cVars& vars );
void GetVars( const std::string& name, SaHpiFumiLogicalComponentInfoT& info, cVars& vars );


}; // namespace Structs

}; // namespace TA


#endif // STRUCTS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

