/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTINSTRUMENTLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef INSTRUMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define INSTRUMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "object.h"


namespace TA {


/**************************************************************
 * class cInstrument
 *************************************************************/
class cHandler;
class cResource;
class cTimers;

class cInstrument : public cObject
{
public:

    explicit cInstrument( cHandler& handler,
                          cResource& resource,
                          const std::string& name,
                          SaHpiRdrTypeT type,
                          const SaHpiRdrTypeUnionT& data );
    virtual ~cInstrument();

public:

    const SaHpiRdrT& GetRdr() const;

protected: // cObject virtual functions

    virtual void BeforeVisibilityChange();
    virtual void AfterVisibilityChange();
    virtual void GetVars( cVars& vars );
    virtual void AfterVarSet( const std::string& var_name );

protected: // Event generation

    void PostEvent( SaHpiEventTypeT type,
                    const SaHpiEventUnionT& data,
                    SaHpiSeverityT severity,
                    bool remove = false ) const;

protected: // Handling RDR changes

    void HandleRdrChange( const std::string& field_name );
    virtual void UpdateRdr( const std::string& field_name,
                            SaHpiRdrTypeUnionT& data );

private:

    cInstrument( const cInstrument& );
    cInstrument& operator =( const cInstrument& );

private: // Event generation

    void PostUpdateEvent( bool remove = false ) const;

    virtual SaHpiCapabilitiesT RequiredResourceCap() const = 0;

protected: // data

    cHandler&  m_handler;
    cResource& m_resource;

private: // data

    SaHpiRdrT  m_rdr;
};


/**************************************************************
 * List of instruments for event generation
 *************************************************************/
typedef std::list<const cInstrument *> InstrumentList;


}; // namespace TA


#endif // INSTRUMENT_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

