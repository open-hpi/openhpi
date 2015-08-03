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

#include <stddef.h>
#include <string.h>

#include <string>

#include "codec.h"
#include "sensor.h"
#include "structs.h"


namespace TA {

/**************************************************************
 * Helper functions
 *************************************************************/
static const SaHpiEventStateT all_th_states = SAHPI_ES_LOWER_MINOR |
                                              SAHPI_ES_LOWER_MAJOR |
                                              SAHPI_ES_LOWER_CRIT  |
                                              SAHPI_ES_UPPER_MINOR |
                                              SAHPI_ES_UPPER_MAJOR |
                                              SAHPI_ES_UPPER_CRIT;

static const SaHpiSensorThdMaskT all_thd_mask = SAHPI_STM_LOW_MINOR     |
                                                SAHPI_STM_LOW_MAJOR     |
                                                SAHPI_STM_LOW_CRIT      |
                                                SAHPI_STM_UP_MINOR      |
                                                SAHPI_STM_UP_MAJOR      |
                                                SAHPI_STM_UP_CRIT       |
                                                SAHPI_STM_UP_HYSTERESIS |
                                                SAHPI_STM_LOW_HYSTERESIS;

static void MakeFloatNoReading( SaHpiSensorReadingT& r )
{
    r.IsSupported         = SAHPI_FALSE;
    r.Type                = SAHPI_SENSOR_READING_TYPE_FLOAT64;
    r.Value.SensorFloat64 = 0.0;
}

static void MakeFloatReading( SaHpiSensorReadingT& r, const SaHpiFloat64T& x )
{
    r.IsSupported         = SAHPI_TRUE;
    r.Type                = SAHPI_SENSOR_READING_TYPE_FLOAT64;
    r.Value.SensorFloat64 = x;
}


static SaHpiRdrTypeUnionT MakeDefaultSensorRec( SaHpiSensorNumT num )
{
    SaHpiRdrTypeUnionT data;
    SaHpiSensorRecT& rec       = data.SensorRec;
    SaHpiSensorDataFormatT& df = rec.DataFormat;
    SaHpiSensorRangeT& range   = df.Range;
    SaHpiSensorThdDefnT& thd   = rec.ThresholdDefn;

    rec.Num                    = num;
    rec.Type                   = SAHPI_TEMPERATURE;
    rec.Category               = SAHPI_EC_THRESHOLD;
    rec.EnableCtrl             = SAHPI_TRUE;
    rec.EventCtrl              = SAHPI_SEC_PER_EVENT;
    rec.Events                 = all_th_states;
    df.IsSupported             = SAHPI_TRUE;
    df.ReadingType             = SAHPI_SENSOR_READING_TYPE_FLOAT64;
    df.BaseUnits               = SAHPI_SU_DEGREES_C;
    df.ModifierUnits           = SAHPI_SU_UNSPECIFIED;
    df.ModifierUse             = SAHPI_SMUU_NONE;
    df.Percentage              = SAHPI_FALSE;
    range.Flags                = SAHPI_SRF_MIN | SAHPI_SRF_MAX | SAHPI_SRF_NOMINAL;
    MakeFloatReading( range.Max, 1000. );
    MakeFloatReading( range.Min, -273.15 );
    MakeFloatReading( range.Nominal, 36.6 );
    MakeFloatNoReading( range.NormalMax );
    MakeFloatNoReading( range.NormalMin );
    df.AccuracyFactor          = 0.1;
    thd.IsAccessible           = SAHPI_TRUE;
    thd.ReadThold              = all_thd_mask;
    thd.WriteThold             = all_thd_mask;
    thd.Nonlinear              = SAHPI_FALSE;
    rec.Oem                    = 0;

    return data;
}

static void MakeDefaultThresholds( SaHpiSensorThresholdsT& ths )
{
    MakeFloatReading( ths.LowCritical, 0.0 );
    MakeFloatReading( ths.LowMajor, 30.0 );
    MakeFloatReading( ths.LowMinor, 36.0 );
    MakeFloatReading( ths.UpCritical, 100.0 );
    MakeFloatReading( ths.UpMajor, 40.0 );
    MakeFloatReading( ths.UpMinor, 37.0 );
    MakeFloatReading( ths.PosThdHysteresis, 0.0 );
    MakeFloatReading( ths.NegThdHysteresis, 0.0 );
}

static void MakeDefaultReading( SaHpiSensorReadingT& reading )
{
    MakeFloatReading( reading, 36.7 );
}

static void MergeThreshold( SaHpiSensorReadingT& th,
                            const SaHpiSensorReadingT& th1,
                            const SaHpiSensorReadingT& th2 )
{
    if ( th2.IsSupported != SAHPI_FALSE ) {
        th = th2;
    } else {
        th = th1;
    }
}

static void MergeThresholds( SaHpiSensorThresholdsT& ths,
                            const SaHpiSensorThresholdsT& ths1,
                            const SaHpiSensorThresholdsT& ths2 )
{
    MergeThreshold( ths.LowCritical,
                    ths1.LowCritical,
                    ths2.LowCritical );
    MergeThreshold( ths.LowMajor,
                    ths1.LowMajor,
                    ths2.LowMajor );
    MergeThreshold( ths.LowMinor,
                    ths1.LowMinor,
                    ths2.LowMinor );
    MergeThreshold( ths.UpCritical,
                    ths1.UpCritical,
                    ths2.UpCritical );
    MergeThreshold( ths.UpMajor,
                    ths1.UpMajor,
                    ths2.UpMajor );
    MergeThreshold( ths.UpMinor,
                    ths1.UpMinor,
                    ths2.UpMinor );
    MergeThreshold( ths.PosThdHysteresis,
                    ths1.PosThdHysteresis,
                    ths2.PosThdHysteresis );
    MergeThreshold( ths.NegThdHysteresis,
                    ths1.NegThdHysteresis,
                    ths2.NegThdHysteresis );
}

static bool IsThresholdCrossed( const SaHpiSensorReadingT& r,
                                const SaHpiSensorReadingT& th,
                                bool up )
{
    if ( r.IsSupported == SAHPI_FALSE ) {
        return false;
    }
    if ( th.IsSupported == SAHPI_FALSE ) {
        return false;
    }
    if ( r.Type != th.Type ) {
        return false;
    }
    if ( r.Type == SAHPI_SENSOR_READING_TYPE_INT64 ) {
        if ( up ) {
            return r.Value.SensorInt64 > th.Value.SensorInt64;
        } else {
            return r.Value.SensorInt64 < th.Value.SensorInt64;
        }
    } else if ( r.Type == SAHPI_SENSOR_READING_TYPE_UINT64 ) {
        if ( up ) {
            return r.Value.SensorUint64 > th.Value.SensorUint64;
        } else {
            return r.Value.SensorUint64 < th.Value.SensorUint64;
        }
    } else if ( r.Type == SAHPI_SENSOR_READING_TYPE_FLOAT64 ) {
        if ( up ) {
            return r.Value.SensorFloat64 > th.Value.SensorFloat64;
        } else {
            return r.Value.SensorFloat64 < th.Value.SensorFloat64;
        }
    }

    return false;
}

SaHpiSeverityT GetEventSeverity( SaHpiEventCategoryT cat,
                                 bool assertion,
                                 SaHpiEventStateT state )
{
    if ( cat == SAHPI_EC_THRESHOLD ) {
        switch ( state ) {
            case SAHPI_ES_LOWER_CRIT:
            case SAHPI_ES_UPPER_CRIT:
                return SAHPI_CRITICAL;
            case SAHPI_ES_LOWER_MAJOR:
            case SAHPI_ES_UPPER_MAJOR:
                return SAHPI_MAJOR;
            case SAHPI_ES_LOWER_MINOR:
            case SAHPI_ES_UPPER_MINOR:
                return SAHPI_MINOR;
        }
    } else if ( cat == SAHPI_EC_SEVERITY ) {
        switch ( state ) {
            case SAHPI_ES_INFORMATIONAL:
                return SAHPI_INFORMATIONAL;
            case SAHPI_ES_OK:
                return SAHPI_OK;
            case SAHPI_ES_MINOR_FROM_OK:
            case SAHPI_ES_MINOR_FROM_MORE:
                return SAHPI_MINOR;
            case SAHPI_ES_MAJOR_FROM_LESS:
            case SAHPI_ES_MAJOR_FROM_CRITICAL:
                return SAHPI_MAJOR;
            case SAHPI_ES_CRITICAL_FROM_LESS:
            case SAHPI_ES_CRITICAL:
                return SAHPI_CRITICAL;
        }
    }

    return SAHPI_INFORMATIONAL;
}


/**************************************************************
 * class cSensor
 *************************************************************/
const std::string cSensor::classname( "sen" );

cSensor::cSensor( cHandler& handler, cResource& resource, SaHpiSensorNumT num )
    : cInstrument( handler,
                   resource,
                   AssembleNumberedObjectName( classname, num ),
                   SAHPI_SENSOR_RDR,
                   MakeDefaultSensorRec( num ) ),
      m_rec( GetRdr().RdrTypeUnion.SensorRec )
{
    m_enabled       = SAHPI_TRUE;
    m_event_enabled = SAHPI_TRUE;
    m_states        = SAHPI_ES_UNSPECIFIED;
    m_amask         = m_rec.Events;
    m_dmask         = SAHPI_ES_UNSPECIFIED;

    MakeDefaultThresholds( m_ths );
    MakeDefaultReading( m_reading );

    m_prev_states = m_states;

    m_new_enabled       = m_enabled;
    m_new_event_enabled = m_event_enabled;
    m_new_states        = m_states;
    m_new_amask         = m_amask;
    m_new_dmask         = m_dmask;
}

cSensor::~cSensor()
{
    // empty
}


// HPI interface
SaErrorT cSensor::GetReading( SaHpiSensorReadingT& r,
                              SaHpiEventStateT& s ) const
{
    if ( m_enabled == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_REQUEST;
    }

    r = m_reading;
    s = m_states;

    return SA_OK;
}

SaErrorT cSensor::GetThresholds( SaHpiSensorThresholdsT& ths ) const
{
    if ( m_rec.Category != SAHPI_EC_THRESHOLD ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( m_rec.ThresholdDefn.IsAccessible == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( m_rec.ThresholdDefn.ReadThold == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }

    ths = m_ths;

    return SA_OK;
}

SaErrorT cSensor::SetThresholds( const SaHpiSensorThresholdsT& ths )
{
    if ( m_rec.Category != SAHPI_EC_THRESHOLD ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( m_rec.ThresholdDefn.IsAccessible == SAHPI_FALSE ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    if ( m_rec.ThresholdDefn.WriteThold == 0 ) {
        return SA_ERR_HPI_INVALID_CMD;
    }
    // TODO other checks
    //  Writing to a threshold that is not writable.
    //  Setting a threshold outside of the Min-Max range.
    //  Setting a hysteresis value to a positive value greater
    //   than an implementation-specific limit.
    //  Thresholds are set out-of-order.
    //  A negative number is provided for either
    //   the postive hysteresis value or for the negative hysteresis.

    SaHpiSensorThresholdsT new_ths;
    MergeThresholds( new_ths, m_ths, ths );
    m_ths = new_ths;

    CommitChanges();

    return SA_OK;
}

SaErrorT cSensor::GetEnable( SaHpiBoolT& e ) const
{
    e = m_enabled;

    return SA_OK;
}

SaErrorT cSensor::SetEnable( SaHpiBoolT e )
{
    if ( m_rec.EnableCtrl == SAHPI_FALSE ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    m_new_enabled = e;
    CommitChanges();

    return SA_OK;
}

SaErrorT cSensor::GetEventEnable( SaHpiBoolT& e ) const
{
    e = m_event_enabled;

    return SA_OK;
}

SaErrorT cSensor::SetEventEnable( SaHpiBoolT e )
{
    if ( m_rec.EventCtrl == SAHPI_SEC_READ_ONLY ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    m_new_event_enabled = e;
    CommitChanges();

    return SA_OK;
}

SaErrorT cSensor::GetMasks( SaHpiEventStateT& amask,
                            SaHpiEventStateT& dmask ) const
{
    amask = m_amask;
    dmask = m_dmask;

    return SA_OK;
}

SaErrorT cSensor::SetMasks( SaHpiSensorEventMaskActionT act,
                            SaHpiEventStateT amask,
                            SaHpiEventStateT dmask )
{
    if ( m_rec.EventCtrl != SAHPI_SEC_PER_EVENT ) {
        return SA_ERR_HPI_READ_ONLY;
    }

    SaHpiEventStateT a = amask;
    SaHpiEventStateT d = dmask;
    if ( a == SAHPI_ALL_EVENT_STATES ) {
        a = m_rec.Events;
    }
    if ( d == SAHPI_ALL_EVENT_STATES ) {
        d = m_rec.Events;
    }

    if ( act == SAHPI_SENS_ADD_EVENTS_TO_MASKS ) {
        if ( ( a & m_rec.Events ) != a ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
        if ( ( d & m_rec.Events ) != d ) {
            return SA_ERR_HPI_INVALID_DATA;
        }
        m_new_amask = m_amask | a;
        m_new_dmask = m_dmask | d;
    } else if ( act == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS ) {
        m_new_amask = m_amask & ( ~a );
        m_new_dmask = m_dmask & ( ~d );
    } else {
        return SA_ERR_HPI_INVALID_PARAMS;
    }

    CommitChanges();

    return SA_OK;
}


void cSensor::GetVars( cVars& vars )
{
    cInstrument::GetVars( vars );

    vars << "Enabled"
         << dtSaHpiBoolT
         << DATA( m_enabled, m_new_enabled )
         << VAR_END();
    vars << "EventsEnabled"
         << dtSaHpiBoolT
         << DATA( m_event_enabled, m_new_event_enabled )
         << VAR_END();
    Structs::GetVars( "Reading", m_reading, vars );
    vars << IF( m_rec.Category != SAHPI_EC_THRESHOLD )
         << "PreviousEventState"
         << dtSaHpiEventStateT
         << DATA( m_prev_states )
         << READONLY()
         << VAR_END();
    vars << IF( m_rec.Category == SAHPI_EC_THRESHOLD )
         << "PreviousEventState"
         << dtSaHpiEventStateTThreshold
         << DATA( m_prev_states )
         << READONLY()
         << VAR_END();
    vars << IF( m_rec.Category != SAHPI_EC_THRESHOLD )
         << "EventState"
         << dtSaHpiEventStateT
         << DATA( m_states, m_new_states )
         << VAR_END();
    vars << IF( m_rec.Category == SAHPI_EC_THRESHOLD )
         << "EventState"
         << dtSaHpiEventStateTThreshold
         << DATA( m_states )
         << READONLY()
         << VAR_END();
    vars << "AssertEventMask"
         << dtSaHpiEventStateT
         << DATA( m_amask, m_new_amask )
         << VAR_END();
    vars << "DeassertEventMask"
         << dtSaHpiEventStateT
         << DATA( m_dmask, m_new_dmask )
         << VAR_END();
    if ( m_rec.Category == SAHPI_EC_THRESHOLD ) {
        Structs::GetVars( m_ths, vars );
    }
}

void cSensor::BeforeVarSet( const std::string& var_name )
{
    cInstrument::BeforeVarSet( var_name );

    m_new_enabled       = m_enabled;
    m_new_event_enabled = m_event_enabled;
    m_new_states        = m_states;
    m_new_amask         = m_amask;
    m_new_dmask         = m_dmask;
}
void cSensor::AfterVarSet( const std::string& var_name )
{
    cInstrument::AfterVarSet( var_name );

    CommitChanges();
}


// Handling RDR changes
void cSensor::UpdateRdr( const std::string& field_name,
                         SaHpiRdrTypeUnionT& data )
{
    cInstrument::UpdateRdr( field_name, data );

    SaHpiSensorRecT& rec       = data.SensorRec;
    SaHpiSensorDataFormatT& df = rec.DataFormat;
    SaHpiSensorRangeT& range   = df.Range;
    SaHpiSensorThdDefnT& thd   = rec.ThresholdDefn;

    if ( field_name == "Rdr.SensorRec.Category" ) {
        if ( rec.Category == SAHPI_EC_THRESHOLD ) {
            thd.IsAccessible = SAHPI_TRUE;
        } else {
            thd.IsAccessible = SAHPI_FALSE;
            thd.ReadThold    = 0;
            thd.WriteThold   = 0;
        }
    }
    if ( field_name == "Rdr.SensorRec.DataFormat.IsSupported" ) {
        m_reading.IsSupported = df.IsSupported;
    }
    if ( field_name == "Rdr.SensorRec.DataFormat.ReadingType" ) {
        range.Max.Type       = df.ReadingType;
        range.Min.Type       = df.ReadingType;
        range.Nominal.Type   = df.ReadingType;
        range.NormalMax.Type = df.ReadingType;
        range.NormalMin.Type = df.ReadingType;

        m_reading.Type = df.ReadingType;

        m_ths.LowCritical.Type      = df.ReadingType;
        m_ths.LowMajor.Type         = df.ReadingType;
        m_ths.LowMinor.Type         = df.ReadingType;
        m_ths.UpCritical.Type       = df.ReadingType;
        m_ths.UpMajor.Type          = df.ReadingType;
        m_ths.UpMinor.Type          = df.ReadingType;
        m_ths.PosThdHysteresis.Type = df.ReadingType;
        m_ths.NegThdHysteresis.Type = df.ReadingType;
    }
}

SaHpiEventStateT cSensor::CalculateThresholdEventStates() const
{
    // TODO hysteresis

    SaHpiEventStateT states = SAHPI_ES_UNSPECIFIED;

    bool rc;

    rc = IsThresholdCrossed( m_reading, m_ths.LowCritical, false );
    if ( rc ) {
        states |= SAHPI_ES_LOWER_CRIT;
    }
    rc = IsThresholdCrossed( m_reading, m_ths.LowMajor, false );
    if ( rc ) {
        states |= SAHPI_ES_LOWER_MAJOR;
    }
    rc = IsThresholdCrossed( m_reading, m_ths.LowMinor, false );
    if ( rc ) {
        states |= SAHPI_ES_LOWER_MINOR;
    }
    rc = IsThresholdCrossed( m_reading, m_ths.UpMinor, true );
    if ( rc ) {
        states |= SAHPI_ES_UPPER_MINOR;
    }
    rc = IsThresholdCrossed( m_reading, m_ths.UpMajor, true );
    if ( rc ) {
        states |= SAHPI_ES_UPPER_MAJOR;
    }
    rc = IsThresholdCrossed( m_reading, m_ths.UpCritical, true );
    if ( rc ) {
        states |= SAHPI_ES_UPPER_CRIT;
    }

    return states;
}

void cSensor::CommitChanges()
{
    bool send_enable_event = false;
    bool states_changed    = false;

    if ( m_enabled != m_new_enabled ) {
        m_enabled = m_new_enabled;
        send_enable_event = true;
    }
    if ( m_event_enabled != m_new_event_enabled ) {
        m_event_enabled = m_new_event_enabled;
        send_enable_event = true;
    }
    if ( m_rec.Category == SAHPI_EC_THRESHOLD ) {
        m_new_states = CalculateThresholdEventStates();
    }
    if ( m_states != m_new_states ) {
        m_prev_states  = m_states;
        m_states       = m_new_states;
        states_changed = true;
    }
    if ( m_amask != m_new_amask ) {
        m_amask = m_new_amask;
        send_enable_event = true;
    }
    if ( m_dmask != m_new_dmask ) {
        m_dmask = m_new_dmask;
        send_enable_event = true;
    }

    if ( send_enable_event ) {
        PostEnableChangeEvent();
    }

    // Check if event shall be generated
    if ( m_enabled == SAHPI_FALSE ) {
        return;
    }
    if ( m_event_enabled == SAHPI_FALSE ) {
        return;
    }
    if ( !states_changed ) {
        return;
    }

    SaHpiEventStateT a = m_states & ( ~m_prev_states ) & m_amask;
    SaHpiEventStateT d = m_prev_states & ( ~m_states ) & m_dmask;
    for ( size_t i = 0; i < 15; ++i ) {
        SaHpiEventStateT s = ( 1 << i );
        if ( s & a ) {
            PostEvent( true, s );
        }
        if ( s & d ) {
            PostEvent( false, s );
        }
    }
}

void cSensor::PostEnableChangeEvent() const
{
    SaHpiEventUnionT data;
    SaHpiSensorEnableChangeEventT& sece = data.SensorEnableChangeEvent;

    sece.SensorNum           = m_rec.Num;
    sece.SensorType          = m_rec.Type;
    sece.EventCategory       = m_rec.Category;
    sece.SensorEnable        = m_enabled;
    sece.SensorEventEnable   = m_event_enabled;
    sece.AssertEventMask     = m_amask;
    sece.DeassertEventMask   = m_dmask;
    sece.OptionalDataPresent = SAHPI_SEOD_CURRENT_STATE;
    sece.CurrentState        = m_states;

    cInstrument::PostEvent( SAHPI_ET_SENSOR_ENABLE_CHANGE, data, SAHPI_INFORMATIONAL );
}

void cSensor::PostEvent( bool assertion, SaHpiEventStateT state ) const
{
    SaHpiEventUnionT data;
    SaHpiSensorEventT& se = data.SensorEvent;

    se.SensorNum           = m_rec.Num;
    se.SensorType          = m_rec.Type;
    se.EventCategory       = m_rec.Category;
    se.Assertion           = assertion ? SAHPI_TRUE : SAHPI_FALSE;
    se.EventState          = state;
    se.OptionalDataPresent = SAHPI_SOD_PREVIOUS_STATE |
                             SAHPI_SOD_CURRENT_STATE;
    MakeFloatNoReading( se.TriggerReading );
    MakeFloatNoReading( se.TriggerThreshold );
    se.PreviousState       = m_prev_states;
    se.CurrentState        = m_states;
    //se.Oem SaHpiUint32T SAHPI_SOD_OEM
    //se.SensorSpecific SaHpiUint32T SAHPI_SOD_SENSOR_SPECIFIC

    if ( m_rec.Category == SAHPI_EC_THRESHOLD ) {
        se.OptionalDataPresent |= SAHPI_SOD_TRIGGER_READING;
        se.TriggerReading = m_reading;
// TODO SAHPI_SOD_TRIGGER_THRESHOLD
#if 0
#define SAHPI_SOD_TRIGGER_THRESHOLD (SaHpiSensorOptionalDataT)0x02
#endif
    //se.TriggerThreshold SaHpiSensorReadingT
    }

    SaHpiSeverityT sev = GetEventSeverity( m_rec.Category, assertion, state );

    cInstrument::PostEvent( SAHPI_ET_SENSOR, data, sev );
}


}; // namespace TA

