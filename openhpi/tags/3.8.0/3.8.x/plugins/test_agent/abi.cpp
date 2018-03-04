/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#include <stdint.h>

#include <glib.h>

#include <SaHpi.h>

#include <oHpi.h>
#include <oh_error.h>
#include <oh_utils.h>

#include "abi.h"
#include "annunciator.h"
#include "area.h"
#include "bank.h"
#include "control.h"
#include "dimi.h"
#include "fumi.h"
#include "handler.h"
#include "inventory.h"
#include "log.h"
#include "resource.h"
#include "sensor.h"
#include "test.h"
#include "utils.h"
#include "watchdog.h"


namespace TA {


/**************************************************************
 *  Helpers
 *************************************************************/

static bool ParseConfig(
    GHashTable * config,
    uint16_t& port )
{
    const char * param;

    param = (const char*)g_hash_table_lookup( config, "port" );
    if ( !param ) {
        CRIT( "no port is specified!" );
        return false;
    }

    port = atoi( param );

    return true;
}

static cHandler * GetHandler( void * hnd )
{
    return Ptr<cHandler>(hnd);
}

static cResource * GetResource( cHandler * h, SaHpiResourceIdT rid )
{
    cResource * r = h->GetResource( rid );
    if ( r && r->IsVisible() ) {
        return r;
    }

    return 0;
}

static cLog * GetLog( cHandler * h, SaHpiResourceIdT rid )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cLog * log = r->GetLog();
        if ( log && log->IsVisible() ) {
            return log;
        }
    }

    return 0;
}

static cControl * GetControl( cHandler * h,
                              SaHpiResourceIdT rid,
                              SaHpiCtrlNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cControl * ctrl = r->GetControl( num );
        if ( ctrl && ctrl->IsVisible() ) {
            return ctrl;
        }
    }

    return 0;
}

static cSensor * GetSensor( cHandler * h,
                            SaHpiResourceIdT rid,
                            SaHpiSensorNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cSensor * sen = r->GetSensor( num );
        if ( sen && sen->IsVisible() ) {
            return sen;
        }
    }

    return 0;
}

static cInventory * GetInventory( cHandler * h,
                                  SaHpiResourceIdT rid,
                                  SaHpiIdrIdT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cInventory * inv = r->GetInventory( num );
        if ( inv && inv->IsVisible() ) {
            return inv;
        }
    }

    return 0;
}

static cArea * GetArea( cHandler * h,
                        SaHpiResourceIdT rid,
                        SaHpiIdrIdT num,
                        SaHpiEntryIdT id )
{
    cInventory * inv = GetInventory( h, rid, num );
    if ( inv ) {
        cArea * area = inv->GetArea( id );
        if ( area && area->IsVisible() ) {
            return area;
        }
    }

    return 0;
}

static cWatchdog * GetWatchdog( cHandler * h,
                                SaHpiResourceIdT rid,
                                SaHpiWatchdogNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cWatchdog * wdt = r->GetWatchdog( num );
        if ( wdt && wdt->IsVisible() ) {
            return wdt;
        }
    }

    return 0;
}

static cAnnunciator * GetAnnunciator( cHandler * h,
                                      SaHpiResourceIdT rid,
                                      SaHpiAnnunciatorNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cAnnunciator * ann = r->GetAnnunciator( num );
        if ( ann && ann->IsVisible() ) {
            return ann;
        }
    }

    return 0;
}

static cDimi * GetDimi( cHandler * h,
                        SaHpiResourceIdT rid,
                        SaHpiDimiNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cDimi * dimi = r->GetDimi( num );
        if ( dimi && dimi->IsVisible() ) {
            return dimi;
        }
    }

    return 0;
}

static cTest * GetTest( cHandler * h,
                        SaHpiResourceIdT rid,
                        SaHpiDimiNumT num,
                        SaHpiDimiTestNumT tnum )
{
    cDimi * dimi = GetDimi( h, rid, num );
    if ( dimi ) {
        cTest * test = dimi->GetTest( tnum );
        if ( test && test->IsVisible() ) {
            return test;
        }
    }

    return 0;
}

static cFumi * GetFumi( cHandler * h,
                        SaHpiResourceIdT rid,
                        SaHpiFumiNumT num )
{
    cResource * r = GetResource( h, rid );
    if ( r ) {
        cFumi * fumi = r->GetFumi( num );
        if ( fumi && fumi->IsVisible() ) {
            return fumi;
        }
    }

    return 0;
}

static cBank * GetBank( cHandler * h,
                        SaHpiResourceIdT rid,
                        SaHpiFumiNumT num,
                        SaHpiBankNumT bnum )
{
    cFumi * fumi = GetFumi( h, rid, num );
    if ( fumi ) {
        cBank * bank = fumi->GetBank( bnum );
        if ( bank && bank->IsVisible() ) {
            return bank;
        }
    }

    return 0;
}


}; // namespace TA


/**************************************************************
 * oh_open
 *************************************************************/
void *
oh_open(
    GHashTable * handler_config,
    unsigned int hid,
    oh_evt_queue * eventq )
{
    if ( !handler_config ) {
        CRIT( "handler_config is NULL!" );
        return 0;
    }
    if ( hid == 0 ) {
        CRIT( "Bad handler id passed." );
        return 0;
    }
    if ( !eventq ) {
        CRIT( "No event queue was passed." );
        return 0;
    }

    bool rc;

    uint16_t port;
    rc = TA::ParseConfig( handler_config, port );
    if ( !rc ) {
        CRIT( "Error while parsing config." );
        return 0;
    }

    TA::cHandler * handler = new TA::cHandler( hid, port, *eventq );

    rc = handler->Init();
    if ( !rc ) {
        CRIT( "Handler::Init failed.");
        return 0;
    }

    return handler;
}


/**************************************************************
 * oh_close
 *************************************************************/
void
oh_close(
    void * hnd )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    // TODO it is not safe to get handler lock here

    delete handler;
}


/**************************************************************
 * oh_discover_resources
 *************************************************************/
SaErrorT
oh_discover_resources(
    void * hnd )
{
    return SA_OK;
}


/**************************************************************
 * oh_set_resource_tag
 *************************************************************/
SaErrorT
oh_set_resource_tag(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTextBufferT * tag )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetTag( *tag );
}


/**************************************************************
 * oh_set_resource_severity
 *************************************************************/
SaErrorT
oh_set_resource_severity(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSeverityT sev )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetSeverity( sev );
}


/**************************************************************
 * oh_resource_failed_remove
 *************************************************************/
SaErrorT
oh_resource_failed_remove(
    void * hnd,
    SaHpiResourceIdT rid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );

    return handler->RemoveFailedResource( rid );
}


/**************************************************************
 * oh_get_el_info
 *************************************************************/
SaErrorT
oh_get_el_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogInfoT * info )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->GetInfo( *info );
}


/**************************************************************
 * oh_get_el_caps
 *************************************************************/
SaErrorT
oh_get_el_caps(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogCapabilitiesT * caps )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->GetCapabilities( *caps );
}


/**************************************************************
 * oh_set_el_time
 *************************************************************/
SaErrorT
oh_set_el_time(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeT time )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->SetTime( time );
}


/**************************************************************
 * oh_add_el_entry
 *************************************************************/
SaErrorT
oh_add_el_entry(
    void * hnd,
    SaHpiResourceIdT rid,
    const SaHpiEventT * event )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->AddEntry( *event );
}


/**************************************************************
 * oh_get_el_entry
 *************************************************************/
SaErrorT
oh_get_el_entry(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiEventLogEntryIdT current,
    SaHpiEventLogEntryIdT * prev,
    SaHpiEventLogEntryIdT * next,
    SaHpiEventLogEntryT * entry,
    SaHpiRdrT * rdr,
    SaHpiRptEntryT * rpte )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->GetEntry( current, *prev, *next, *entry, *rdr, *rpte );
}


/**************************************************************
 * oh_clear_el
 *************************************************************/
SaErrorT
oh_clear_el(
    void * hnd,
    SaHpiResourceIdT rid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->Clear();
}


/**************************************************************
 * oh_set_el_state
 *************************************************************/
SaErrorT
oh_set_el_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiBoolT e )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->SetState( e );
}


/**************************************************************
 * oh_reset_el_overflow
 *************************************************************/
SaErrorT
oh_reset_el_overflow(
    void * hnd,
    SaHpiResourceIdT rid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cLog * log = TA::GetLog( handler, rid );
    if ( !log ) {
        return SA_ERR_HPI_CAPABILITY;
    }

    return log->ResetOverflow();
}


/**************************************************************
 * oh_get_sensor_reading
 *************************************************************/
SaErrorT
oh_get_sensor_reading(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorReadingT * reading,
    SaHpiEventStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->GetReading( *reading, *state );
}


/**************************************************************
 * oh_get_sensor_thresholds
 *************************************************************/
SaErrorT
oh_get_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorThresholdsT * thres )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->GetThresholds( *thres );
}


/**************************************************************
 * oh_set_sensor_thresholds
 *************************************************************/
SaErrorT
oh_set_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    const SaHpiSensorThresholdsT * thres )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->SetThresholds( *thres );
}


/**************************************************************
 * oh_get_sensor_enable
 *************************************************************/
SaErrorT
oh_get_sensor_enable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT * enable )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->GetEnable( *enable );
}


/**************************************************************
 * oh_set_sensor_enable
 *************************************************************/
SaErrorT
oh_set_sensor_enable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT enable )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->SetEnable( enable );
}


/**************************************************************
 * oh_get_sensor_event_enables
 *************************************************************/
SaErrorT
oh_get_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiBoolT * enables )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->GetEventEnable( *enables );
}


/**************************************************************
 * oh_set_sensor_event_enables
 *************************************************************/
SaErrorT
oh_set_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    const SaHpiBoolT enables )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->SetEventEnable( enables );
}


/**************************************************************
 * oh_get_sensor_event_masks
 *************************************************************/
SaErrorT
oh_get_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiEventStateT * AssertEventMask,
    SaHpiEventStateT * DeassertEventMask )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->GetMasks( *AssertEventMask, *DeassertEventMask );
}


/**************************************************************
 * oh_set_sensor_event_masks
 *************************************************************/
SaErrorT
oh_set_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiSensorNumT num,
    SaHpiSensorEventMaskActionT act,
    SaHpiEventStateT AssertEventMask,
    SaHpiEventStateT DeassertEventMask )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cSensor * sen = TA::GetSensor( handler, rid, num );
    if ( !sen ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return sen->SetMasks( act, AssertEventMask, DeassertEventMask );
}


/**************************************************************
 * oh_get_control_state
 *************************************************************/
SaErrorT
oh_get_control_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT * mode,
    SaHpiCtrlStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cControl * ctrl = TA::GetControl( handler, rid, num );
    if ( !ctrl ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ctrl->Get( *mode, *state );
}


/**************************************************************
 * oh_set_control_state
 *************************************************************/
SaErrorT
oh_set_control_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT mode,
    SaHpiCtrlStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cControl * ctrl = TA::GetControl( handler, rid, num );
    if ( !ctrl ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ctrl->Set( mode, *state );
}


/**************************************************************
 * oh_get_idr_info
 *************************************************************/
SaErrorT
oh_get_idr_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrInfoT * idrinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cInventory * inv = TA::GetInventory( handler, rid, idrid );
    if ( !inv ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return inv->GetInfo( *idrinfo );
}


/**************************************************************
 * oh_get_idr_area_header
 *************************************************************/
SaErrorT
oh_get_idr_area_header(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT * nextareaid,
    SaHpiIdrAreaHeaderT * header )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cInventory * inv = TA::GetInventory( handler, rid, idrid );
    if ( !inv ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return inv->GetArea( areatype, areaid, *nextareaid, *header );
}


/**************************************************************
 * oh_add_idr_area
 *************************************************************/
SaErrorT
oh_add_idr_area(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT * areaid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cInventory * inv = TA::GetInventory( handler, rid, idrid );
    if ( !inv ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return inv->AddArea( areatype, *areaid );
}


/**************************************************************
 * oh_add_idr_area_id
 *************************************************************/
SaErrorT
oh_add_idr_area_id(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cInventory * inv = TA::GetInventory( handler, rid, idrid );
    if ( !inv ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return inv->AddAreaById( areaid, areatype );
}


/**************************************************************
 * oh_del_idr_area
 *************************************************************/
SaErrorT
oh_del_idr_area(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cInventory * inv = TA::GetInventory( handler, rid, idrid );
    if ( !inv ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return inv->DeleteAreaById( areaid );
}


/**************************************************************
 * oh_get_idr_field
 *************************************************************/
SaErrorT
oh_get_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiIdrFieldTypeT fieldtype,
    SaHpiEntryIdT fieldid,
    SaHpiEntryIdT * nextfieldid,
    SaHpiIdrFieldT * field )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cArea * area = TA::GetArea( handler, rid, idrid, areaid );
    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return area->GetField( fieldtype, fieldid, *nextfieldid, *field );
}


/**************************************************************
 * oh_add_idr_field
 *************************************************************/
SaErrorT
oh_add_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cArea * area = TA::GetArea( handler, rid, idrid, field->AreaId );
    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    field->ReadOnly = SAHPI_FALSE;

    return area->AddField( field->Type, field->Field, field->FieldId );
}


/**************************************************************
 * oh_add_idr_field_id
 *************************************************************/
SaErrorT
oh_add_idr_field_id(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cArea * area = TA::GetArea( handler, rid, idrid, field->AreaId );
    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return area->AddFieldById( field->FieldId, field->Type, field->Field );
}


/**************************************************************
 * oh_set_idr_field
 *************************************************************/
SaErrorT
oh_set_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cArea * area = TA::GetArea( handler, rid, idrid, field->AreaId );
    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return area->SetField( field->FieldId, field->Type, field->Field );
}


/**************************************************************
 * oh_del_idr_field
 *************************************************************/
SaErrorT
oh_del_idr_field(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT fieldid )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cArea * area = TA::GetArea( handler, rid, idrid, areaid );
    if ( !area ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return area->DeleteFieldById( fieldid );
}


/**************************************************************
 * oh_get_watchdog_info
 *************************************************************/
SaErrorT
oh_get_watchdog_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cWatchdog * w = TA::GetWatchdog( handler, rid, num );
    if ( !w ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return w->Get( *wdt );
}


/**************************************************************
 * oh_set_watchdog_info
 *************************************************************/
SaErrorT
oh_set_watchdog_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cWatchdog * w = TA::GetWatchdog( handler, rid, num );
    if ( !w ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return w->Set( *wdt );
}


/**************************************************************
 * oh_reset_watchdog
 *************************************************************/
SaErrorT
oh_reset_watchdog(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiWatchdogNumT num )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cWatchdog * w = TA::GetWatchdog( handler, rid, num );
    if ( !w ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return w->Reset();
}


/**************************************************************
 * oh_get_next_announce
 *************************************************************/
SaErrorT
oh_get_next_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiSeverityT sev,
    SaHpiBoolT ack,
    SaHpiAnnouncementT * a )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->GetNextAnnouncement( sev, ack, *a );
}


/**************************************************************
 * oh_get_announce
 *************************************************************/
SaErrorT
oh_get_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiAnnouncementT * a )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->GetAnnouncement( aid, *a );
}


/**************************************************************
 * oh_ack_announce
 *************************************************************/
SaErrorT
oh_ack_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiSeverityT sev )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->AckAnnouncement( aid, sev );
}


/**************************************************************
 * oh_add_announce
 *************************************************************/
SaErrorT
oh_add_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnouncementT * a )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->AddAnnouncement( *a );
}


/**************************************************************
 * oh_del_announce
 *************************************************************/
SaErrorT
oh_del_announce(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT aid,
    SaHpiSeverityT sev )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->DeleteAnnouncement( aid, sev );
}


/**************************************************************
 * oh_get_annunc_mode
 *************************************************************/
SaErrorT
oh_get_annunc_mode(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT * mode )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->GetMode( *mode );
}


/**************************************************************
 * oh_set_annunc_mode
 *************************************************************/
SaErrorT
oh_set_annunc_mode(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT mode )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cAnnunciator * ann = TA::GetAnnunciator( handler, rid, num );
    if ( !ann ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return ann->SetMode( mode );
}


/**************************************************************
 * oh_get_dimi_info
 *************************************************************/
SaErrorT
oh_get_dimi_info(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiInfoT * info )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cDimi * dimi = TA::GetDimi( handler, rid, num );
    if ( !dimi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return dimi->GetInfo( *info );
}


/**************************************************************
 * oh_get_dimi_test
 *************************************************************/
SaErrorT
oh_get_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestT * test )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->GetInfo( *test );
}


/**************************************************************
 * oh_get_dimi_test_ready
 *************************************************************/
SaErrorT
oh_get_dimi_test_ready(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiReadyT * ready )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->GetReadiness( *ready );
}


/**************************************************************
 * oh_start_dimi_test
 *************************************************************/
SaErrorT
oh_start_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiUint8T numparams,
    SaHpiDimiTestVariableParamsT * paramslist )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->Start( numparams, paramslist );
}


/**************************************************************
 * oh_cancel_dimi_test
 *************************************************************/
SaErrorT
oh_cancel_dimi_test(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->Cancel();
}


/**************************************************************
 * oh_get_dimi_test_status
 *************************************************************/
SaErrorT
oh_get_dimi_test_status(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestPercentCompletedT * percentcompleted,
    SaHpiDimiTestRunStatusT * runstatus )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->GetStatus( *percentcompleted, *runstatus );
}


/**************************************************************
 * oh_get_dimi_test_results
 *************************************************************/
SaErrorT
oh_get_dimi_test_results(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestResultsT * testresults )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cTest * t = TA::GetTest( handler, rid, num, testnum );
    if ( !t ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return t->GetResults( *testresults );
}


/**************************************************************
 * oh_get_fumi_spec
 *************************************************************/
SaErrorT
oh_get_fumi_spec(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiSpecInfoT * specinfo  )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->GetSpecInfo( *specinfo );
}


/**************************************************************
 * oh_get_fumi_service_impact
 *************************************************************/
SaErrorT
oh_get_fumi_service_impact(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiServiceImpactDataT * serviceimpact  )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->GetServiceImpact( *serviceimpact );
}


/**************************************************************
 * oh_set_fumi_source
 *************************************************************/
SaErrorT
oh_set_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiTextBufferT * sourceuri )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->SetSource( *sourceuri );
}


/**************************************************************
 * oh_validate_fumi_source
 *************************************************************/
SaErrorT
oh_validate_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartSourceValidation();
}


/**************************************************************
 * oh_get_fumi_source
 *************************************************************/
SaErrorT
oh_get_fumi_source(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiSourceInfoT * sourceinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetSourceInfo( *sourceinfo );
}


/**************************************************************
 * oh_get_fumi_source_component
 *************************************************************/
SaErrorT
oh_get_fumi_source_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetSourceComponentInfo( compid, *nextcompid, *compinfo );
}


/**************************************************************
 * oh_get_fumi_target
 *************************************************************/
SaErrorT
oh_get_fumi_target(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiBankInfoT * bankinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetTargetInfo( *bankinfo );
}


/**************************************************************
 * oh_get_fumi_target_component
 *************************************************************/
SaErrorT
oh_get_fumi_target_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetTargetComponentInfo( compid, *nextcompid, *compinfo );
}


/**************************************************************
 * oh_get_fumi_logical_target
 *************************************************************/
SaErrorT
oh_get_fumi_logical_target(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiFumiLogicalBankInfoT * bankinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, 0 );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetLogicalTargetInfo( *bankinfo );
}


/**************************************************************
 * oh_get_fumi_logical_target_component
 *************************************************************/
SaErrorT
oh_get_fumi_logical_target_component(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiLogicalComponentInfoT * compinfo )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, 0 );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetLogicalTargetComponentInfo( compid, *nextcompid, *compinfo );
}


/**************************************************************
 * oh_start_fumi_backup
 *************************************************************/
SaErrorT
oh_start_fumi_backup(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, 0 );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartBackup();
}


/**************************************************************
 * oh_set_fumi_bank_order
 *************************************************************/
SaErrorT
oh_set_fumi_bank_order(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiUint32T position )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->SetBootOrder( banknum, position );
}


/**************************************************************
 * oh_start_fumi_bank_copy
 *************************************************************/
SaErrorT
oh_start_fumi_bank_copy(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT sourcebanknum,
    SaHpiBankNumT targetbanknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, sourcebanknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartCopy( targetbanknum );
}


/**************************************************************
 * oh_start_fumi_install
 *************************************************************/
SaErrorT
oh_start_fumi_install(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartInstallation();
}


/**************************************************************
 * oh_get_fumi_status
 *************************************************************/
SaErrorT
oh_get_fumi_status(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiUpgradeStatusT * status )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->GetUpgradeStatus( *status );
}


/**************************************************************
 * oh_start_fumi_verify
 *************************************************************/
SaErrorT
oh_start_fumi_verify(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartTargetVerification();
}


/**************************************************************
 * oh_start_fumi_verify_main
 *************************************************************/
SaErrorT
oh_start_fumi_verify_main(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num  )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, 0 );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartTargetMainVerification();
}


/**************************************************************
 * oh_cancel_fumi_upgrade
 *************************************************************/
SaErrorT
oh_cancel_fumi_upgrade(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->CancelUpgrade();
}


/**************************************************************
 * oh_get_fumi_autorollback_disable
 *************************************************************/
SaErrorT
oh_get_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT * disable )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->GetAutoRollbackDisabled( *disable );
}


/**************************************************************
 * oh_set_fumi_autorollback_disable
 *************************************************************/
SaErrorT
oh_set_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT disable )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->SetAutoRollbackDisabled( disable );
}


/**************************************************************
 * oh_start_fumi_rollback
 *************************************************************/
SaErrorT
oh_start_fumi_rollback(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, 0 );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->StartRollback();
}


/**************************************************************
 * oh_activate_fumi
 *************************************************************/
SaErrorT
oh_activate_fumi(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num )
{
    return SA_ERR_HPI_UNSUPPORTED_API;
}


/**************************************************************
 * oh_start_fumi_activate
 *************************************************************/
SaErrorT
oh_start_fumi_activate(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBoolT logical )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cFumi * fumi = TA::GetFumi( handler, rid, num );
    if ( !fumi ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return fumi->StartActivation( logical );
}


/**************************************************************
 * oh_cleanup_fumi
 *************************************************************/
SaErrorT
oh_cleanup_fumi(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cBank * b = TA::GetBank( handler, rid, num, banknum );
    if ( !b ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return b->Cleanup();
}


/**************************************************************
 * oh_hotswap_policy_cancel
 *************************************************************/
SaErrorT
oh_hotswap_policy_cancel(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT timeout )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->CancelHsPolicy( timeout );
}


/**************************************************************
 * oh_set_autoinsert_timeout
 *************************************************************/
SaErrorT
oh_set_autoinsert_timeout(
    void * hnd,
    SaHpiTimeoutT timeout )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );

    return handler->SetAutoInsertTimeout( timeout );
}


/**************************************************************
 * oh_get_autoextract_timeout
 *************************************************************/
SaErrorT
oh_get_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT * timeout )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetAutoExtractTimeout( *timeout );
}


/**************************************************************
 * oh_set_autoextract_timeout
 *************************************************************/
SaErrorT
oh_set_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiTimeoutT timeout )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetAutoExtractTimeout( timeout );
}


/**************************************************************
 * oh_get_hotswap_state
 *************************************************************/
SaErrorT
oh_get_hotswap_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetHsState( *state );
}


/**************************************************************
 * oh_set_hotswap_state
 *************************************************************/
SaErrorT
oh_set_hotswap_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsStateT state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetHsState( state );
}


/**************************************************************
 * oh_request_hotswap_action
 *************************************************************/
SaErrorT
oh_request_hotswap_action(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsActionT act )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->RequestHsAction( act );
}


/**************************************************************
 * oh_get_indicator_state
 *************************************************************/
SaErrorT
oh_get_indicator_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsIndicatorStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetHsIndicatorState( *state );
}


/**************************************************************
 * oh_set_indicator_state
 *************************************************************/
SaErrorT
oh_set_indicator_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiHsIndicatorStateT state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetHsIndicatorState( state );
}


/**************************************************************
 * oh_get_power_state
 *************************************************************/
SaErrorT
oh_get_power_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiPowerStateT * state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetPowerState( *state );
}


/**************************************************************
 * oh_set_power_state
 *************************************************************/
SaErrorT
oh_set_power_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiPowerStateT state )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetPowerState( state );
}


/**************************************************************
 * oh_control_parm
 *************************************************************/
SaErrorT
oh_control_parm(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiParmActionT act )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->ControlParm( act );
}


/**************************************************************
 * oh_load_id_get
 *************************************************************/
SaErrorT
oh_load_id_get(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiLoadIdT * load_id )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetLoadId( *load_id );
}


/**************************************************************
 * oh_load_id_set
 *************************************************************/
SaErrorT
oh_load_id_set(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiLoadIdT * load_id )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetLoadId( *load_id );
}


/**************************************************************
 * oh_get_reset_state
 *************************************************************/
SaErrorT
oh_get_reset_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiResetActionT * act )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->GetResetState( *act );
}


/**************************************************************
 * oh_set_reset_state
 *************************************************************/
SaErrorT
oh_set_reset_state(
    void * hnd,
    SaHpiResourceIdT rid,
    SaHpiResetActionT act )
{
    TA::cHandler * handler = TA::GetHandler( hnd );
    TA::cLocker<TA::cHandler> al( handler );
    TA::cResource * r = TA::GetResource( handler, rid );
    if ( !r ) {
        return SA_ERR_HPI_NOT_PRESENT;
    }

    return r->SetResetState( act );
}

