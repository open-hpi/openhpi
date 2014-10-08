/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTCONTROLLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef CONTROL_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define CONTROL_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>
#include <vector>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cControl
 *************************************************************/
class cControl : public cInstrument
{
public:

    static const std::string classname;

    explicit cControl( cHandler& handler, cResource& resource, SaHpiCtrlNumT num );
    virtual ~cControl();

public:  // HPI interface

    SaErrorT Get( SaHpiCtrlModeT& mode, SaHpiCtrlStateT& state ) const;
    SaErrorT Set( SaHpiCtrlModeT mode, const SaHpiCtrlStateT& state );

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cControl( const cControl& );
    cControl& operator =( const cControl& );

private: // Handling RDR changes

    virtual void UpdateRdr( const std::string& field_name,
                            SaHpiRdrTypeUnionT& data );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_CONTROL;
    }

    SaErrorT CheckStateDigital( const SaHpiCtrlStateDigitalT& ds ) const;
    SaErrorT CheckStateAnalog( const SaHpiCtrlStateAnalogT& as ) const;
    SaErrorT CheckStateStream( const SaHpiCtrlStateStreamT& ss ) const;
    SaErrorT CheckStateText( const SaHpiCtrlStateTextT& ts ) const;
    void NormalizeLines();

private: // data

    const SaHpiCtrlRecT&          m_rec;
    SaHpiCtrlModeT                m_mode;
    SaHpiCtrlStateT               m_state;
    std::vector<SaHpiTextBufferT> m_lines;
};


}; // namespace TA


#endif // CONTROL_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

