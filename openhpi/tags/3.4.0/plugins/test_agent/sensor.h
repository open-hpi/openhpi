/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTSENSORLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef SENSOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define SENSOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <string>

#include <SaHpi.h>

#include "instrument.h"


namespace TA {


/**************************************************************
 * class cSensor
 *************************************************************/
class cSensor : public cInstrument
{
public:

    static const std::string classname;

    explicit cSensor( cHandler& handler, cResource& resource, SaHpiSensorNumT num );
    virtual ~cSensor();

public:  // HPI interface

    SaErrorT GetReading( SaHpiSensorReadingT& r, SaHpiEventStateT& s ) const;
    SaErrorT GetThresholds( SaHpiSensorThresholdsT& ths ) const;
    SaErrorT SetThresholds( const SaHpiSensorThresholdsT& ths );
    SaErrorT GetEnable( SaHpiBoolT& e ) const;
    SaErrorT SetEnable( SaHpiBoolT e );
    SaErrorT GetEventEnable( SaHpiBoolT& e ) const;
    SaErrorT SetEventEnable( SaHpiBoolT e );
    SaErrorT GetMasks( SaHpiEventStateT& amask, SaHpiEventStateT& dmask ) const;
    SaErrorT SetMasks( SaHpiSensorEventMaskActionT act,
                       SaHpiEventStateT amask,
                       SaHpiEventStateT dmask );

protected: // cObject virtual functions

    virtual void GetVars( cVars& vars );
    virtual void BeforeVarSet( const std::string& var_name );
    virtual void AfterVarSet( const std::string& var_name );

private:

    cSensor( const cSensor& );
    cSensor& operator =( const cSensor& );

private: // Handling RDR changes

    virtual void UpdateRdr( const std::string& field_name,
                            SaHpiRdrTypeUnionT& data );

private:

    virtual SaHpiCapabilitiesT RequiredResourceCap() const
    {
        return SAHPI_CAPABILITY_SENSOR;
    }

    void PostEnableChangeEvent() const;
    void PostEvent( bool assertion, SaHpiEventStateT state ) const;
    SaHpiEventStateT CalculateThresholdEventStates() const;
    void CommitChanges();

private: // data

    const SaHpiSensorRecT& m_rec;
    SaHpiBoolT             m_enabled;
    SaHpiBoolT             m_new_enabled;
    SaHpiBoolT             m_event_enabled;
    SaHpiBoolT             m_new_event_enabled;
    SaHpiSensorReadingT    m_reading;
    SaHpiEventStateT       m_prev_states;
    SaHpiEventStateT       m_states;
    SaHpiEventStateT       m_new_states;
    SaHpiEventStateT       m_amask;
    SaHpiEventStateT       m_new_amask;
    SaHpiEventStateT       m_dmask;
    SaHpiEventStateT       m_new_dmask;
    SaHpiSensorThresholdsT m_ths;
};


}; // namespace TA


#endif // SENSOR_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

