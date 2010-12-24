/*      -*- linux-c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
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

#include <stdlib.h>
#include <string.h>

#include <string>

#include <glib.h>

#include <SaHpi.h>

#include <oHpi.h>
#include <oh_error.h>
#include <oh_utils.h>

#include "handler.h"
#include "slave.h"
#include "util.h"


using Slave::cHandler;


/**************************************************************
 *  Helpers
 *************************************************************/
static bool slave_parse_config(
    GHashTable * config,
    SaHpiEntityPathT& root,
    std::string& host,
    unsigned short& port )
{
    const char * param;

    root.Entry[0].EntityType = SAHPI_ENT_ROOT;
    root.Entry[0].EntityLocation = 0;
    param = (const char*)g_hash_table_lookup( config, "entity_root" );
    if ( param && ( strlen( param ) > 0 ) ) {
        if ( oh_encode_entitypath( param, &root ) != SA_OK ) {
            CRIT( "Cannot decode entity_root." );
            return false;
        }
    }

    param = (const char*)g_hash_table_lookup( config, "host" );
    if ( !param ) {
        CRIT( "No host specified." );
        return false;
    }
    host = param;

    port = OPENHPI_DEFAULT_DAEMON_PORT;
    param = (const char*)g_hash_table_lookup( config, "port" );
    if ( param ) {
        port = atoi( param );
    }

    return true;
}

#define GET_SLAVE( h, master_rid, slave_rid ) \
    slave_rid = h->GetSlave( master_rid ); \
    if ( slave_rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) { \
        return SA_ERR_HPI_NOT_PRESENT; \
    }

#define CALL_ABI( h, FFF, rv,  ... ) \
    rv = h->Abi()->FFF( handler->GetSessionId(), ##__VA_ARGS__ );


/**************************************************************
 * slave_open
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

    SaHpiEntityPathT root;
    std::string host;
    unsigned short port;
    rc = slave_parse_config( handler_config, root, host, port );
    if ( !rc ) {
        CRIT( "Error while parsing config." );
        return 0;
    }

    cHandler * handler = new cHandler( hid, root, host, port, *eventq );

    rc = handler->Init();
    if ( !rc ) {
        CRIT( "Handler::Init failed.");
        return 0;
    }

    return handler;
}


/**************************************************************
 * slave_close
 *************************************************************/
void
oh_close(
    void * hnd )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);

    delete handler;
}


/**************************************************************
 * slave_discover_resources
 *************************************************************/
SaErrorT
oh_discover_resources(
    void * hnd )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);

    bool rc = handler->WaitForDiscovery();

    return rc ? SA_OK : SA_ERR_HPI_ERROR;
}


/**************************************************************
 * slave_set_resource_tag
 *************************************************************/
SaErrorT
oh_set_resource_tag(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTextBufferT * tag )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceTagSet,
              rv,
              slave_id,
              tag );

    return rv;
}


/**************************************************************
 * slave_set_resource_severity
 *************************************************************/
SaErrorT
oh_set_resource_severity(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSeverityT sev )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceSeveritySet,
              rv,
              slave_id,
              sev );

    return rv;
}


/**************************************************************
 * slave_resource_failed_remove
 *************************************************************/
SaErrorT
oh_resource_failed_remove(
    void * hnd,
    SaHpiResourceIdT id )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceFailedRemove,
              rv,
              slave_id );

    return rv;
}


/**************************************************************
 * slave_get_el_info
 *************************************************************/
SaErrorT
oh_get_el_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogInfoT * info )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogInfoGet,
              rv,
              slave_id,
              info );

    return rv;
}


/**************************************************************
 * slave_get_el_caps
 *************************************************************/
SaErrorT
oh_get_el_caps(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogCapabilitiesT * caps )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogCapabilitiesGet,
              rv,
              slave_id,
              caps );

    return rv;
}


/**************************************************************
 * slave_set_el_time
 *************************************************************/
SaErrorT
oh_set_el_time(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeT time )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogTimeSet,
              rv,
              slave_id,
              time );

    return rv;
}


/**************************************************************
 * slave_add_el_entry
 *************************************************************/
SaErrorT
oh_add_el_entry(
    void * hnd,
    SaHpiResourceIdT id,
    const SaHpiEventT * event )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    // Workaround for const event pointer
    SaHpiEventT event2 = *event;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogEntryAdd,
              rv,
              slave_id,
              &event2 );

    return rv;
}


/**************************************************************
 * slave_get_el_entry
 *************************************************************/
SaErrorT
oh_get_el_entry(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiEventLogEntryIdT current,
    SaHpiEventLogEntryIdT * prev,
    SaHpiEventLogEntryIdT * next,
    SaHpiEventLogEntryT * entry,
    SaHpiRdrT * rdr,
    SaHpiRptEntryT * rpte )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogEntryGet,
              rv,
              slave_id,
              current,
              prev,
              next,
              entry,
              rdr,
              rpte );

    if ( rv == SA_OK ) {
        const SaHpiEntityPathT& root = handler->GetRoot();
        SaHpiResourceIdT master_src = handler->GetMaster( entry->Event.Source );
        Slave::TranslateEvent( entry->Event, master_src );
        Slave::TranslateRdr( *rdr, root );
        if ( Slave::IsRptEntryValid( *rpte ) ) {
            SaHpiResourceIdT master_rpte = handler->GetMaster( rpte->ResourceId );
            Slave::TranslateRptEntry( *rpte, master_rpte, root );
        }
    }

    return rv;
}


/**************************************************************
 * slave_clear_el
 *************************************************************/
SaErrorT
oh_clear_el(
    void * hnd,
    SaHpiResourceIdT id )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogClear,
              rv,
              slave_id );

    return rv;
}


/**************************************************************
 * slave_set_el_state
 *************************************************************/
SaErrorT
oh_set_el_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiBoolT e )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogStateSet,
              rv,
              slave_id,
              e );

    return rv;
}


/**************************************************************
 * slave_reset_el_overflow
 *************************************************************/
SaErrorT
oh_reset_el_overflow(
    void * hnd,
    SaHpiResourceIdT id )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiEventLogOverflowReset,
              rv,
              slave_id );

    return rv;
}


/**************************************************************
 * slave_get_sensor_reading
 *************************************************************/
SaErrorT
oh_get_sensor_reading(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorReadingT * reading,
    SaHpiEventStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorReadingGet,
              rv,
              slave_id,
              num,
              reading,
              state );

    return rv;
}


/**************************************************************
 * slave_get_sensor_thresholds
 *************************************************************/
SaErrorT
oh_get_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorThresholdsT * thres )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorThresholdsGet,
              rv,
              slave_id,
              num,
              thres );

    return rv;
}


/**************************************************************
 * slave_set_sensor_thresholds
 *************************************************************/
SaErrorT
oh_set_sensor_thresholds(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    const SaHpiSensorThresholdsT * thres )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    // Workaround for const thres pointer
    SaHpiSensorThresholdsT thres2 = *thres;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorThresholdsSet,
              rv,
              slave_id,
              num,
              &thres2 );

    return rv;
}


/**************************************************************
 * slave_get_sensor_enable
 *************************************************************/
SaErrorT
oh_get_sensor_enable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT * enable )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEnableGet,
              rv,
              slave_id,
              num,
              enable );

    return rv;
}


/**************************************************************
 * slave_set_sensor_enable
 *************************************************************/
SaErrorT
oh_set_sensor_enable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT enable )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEnableSet,
              rv,
              slave_id,
              num,
              enable );

    return rv;
}


/**************************************************************
 * slave_get_sensor_event_enables
 *************************************************************/
SaErrorT
oh_get_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiBoolT * enables )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEventEnableGet,
              rv,
              slave_id,
              num,
              enables );

    return rv;
}


/**************************************************************
 * slave_set_sensor_event_enables
 *************************************************************/
SaErrorT
oh_set_sensor_event_enables(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    const SaHpiBoolT enables )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEventEnableSet,
              rv,
              slave_id,
              num,
              enables );

    return rv;
}


/**************************************************************
 * slave_get_sensor_event_masks
 *************************************************************/
SaErrorT
oh_get_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiEventStateT * AssertEventMask,
    SaHpiEventStateT * DeassertEventMask )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEventMasksGet,
              rv,
              slave_id,
              num,
              AssertEventMask,
              DeassertEventMask );

    return rv;
}


/**************************************************************
 * slave_set_sensor_event_masks
 *************************************************************/
SaErrorT
oh_set_sensor_event_masks(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiSensorNumT num,
    SaHpiSensorEventMaskActionT act,
    SaHpiEventStateT AssertEventMask,
    SaHpiEventStateT DeassertEventMask )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiSensorEventMasksSet,
              rv,
              slave_id,
              num,
              act,
              AssertEventMask,
              DeassertEventMask );

    return rv;
}


/**************************************************************
 * slave_get_control_state
 *************************************************************/
SaErrorT
oh_get_control_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT * mode,
    SaHpiCtrlStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiControlGet,
              rv,
              slave_id,
              num,
              mode,
              state );

    return rv;
}


/**************************************************************
 * slave_set_control_state
 *************************************************************/
SaErrorT
oh_set_control_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiCtrlNumT num,
    SaHpiCtrlModeT mode,
    SaHpiCtrlStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiControlSet,
              rv,
              slave_id,
              num,
              mode,
              state );

    return rv;
}


/**************************************************************
 * slave_get_idr_info
 *************************************************************/
SaErrorT
oh_get_idr_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrInfoT * idrinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrInfoGet,
              rv,
              slave_id,
              idrid,
              idrinfo );

    return rv;
}


/**************************************************************
 * slave_get_idr_area_header
 *************************************************************/
SaErrorT
oh_get_idr_area_header(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT * nextareaid,
    SaHpiIdrAreaHeaderT * header )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrAreaHeaderGet,
              rv,
              slave_id,
              idrid,
              areatype,
              areaid,
              nextareaid,
              header );

    return rv;
}


/**************************************************************
 * slave_add_idr_area
 *************************************************************/
SaErrorT
oh_add_idr_area(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT * areaid )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrAreaAdd,
              rv,
              slave_id,
              idrid,
              areatype,
              areaid );

    return rv;
}


/**************************************************************
 * slave_add_idr_area_id
 *************************************************************/
SaErrorT
oh_add_idr_area_id(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrAreaTypeT areatype,
    SaHpiEntryIdT areaid )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrAreaAddById,
              rv,
              slave_id,
              idrid,
              areatype,
              areaid );

    return rv;
}


/**************************************************************
 * slave_del_idr_area
 *************************************************************/
SaErrorT
oh_del_idr_area(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrAreaDelete,
              rv,
              slave_id,
              idrid,
              areaid );

    return rv;
}


/**************************************************************
 * slave_get_idr_field
 *************************************************************/
SaErrorT
oh_get_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiIdrFieldTypeT fieldtype,
    SaHpiEntryIdT fieldid,
    SaHpiEntryIdT * nextfieldid,
    SaHpiIdrFieldT * field )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrFieldGet,
              rv,
              slave_id,
              idrid,
              areaid,
              fieldtype,
              fieldid,
              nextfieldid,
              field );

    return rv;
}


/**************************************************************
 * slave_add_idr_field
 *************************************************************/
SaErrorT
oh_add_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrFieldAdd,
              rv,
              slave_id,
              idrid,
              field );

    return rv;
}


/**************************************************************
 * slave_add_idr_field_id
 *************************************************************/
SaErrorT
oh_add_idr_field_id(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrFieldAddById,
              rv,
              slave_id,
              idrid,
              field );

    return rv;
}


/**************************************************************
 * slave_set_idr_field
 *************************************************************/
SaErrorT
oh_set_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiIdrFieldT * field )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrFieldSet,
              rv,
              slave_id,
              idrid,
              field );

    return rv;
}


/**************************************************************
 * slave_del_idr_field
 *************************************************************/
SaErrorT
oh_del_idr_field(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiIdrIdT idrid,
    SaHpiEntryIdT areaid,
    SaHpiEntryIdT fieldid )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiIdrFieldDelete,
              rv,
              slave_id,
              idrid,
              areaid,
              fieldid );

    return rv;
}


/**************************************************************
 * slave_get_watchdog_info
 *************************************************************/
SaErrorT
oh_get_watchdog_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiWatchdogTimerGet,
              rv,
              slave_id,
              num,
              wdt );

    return rv;
}


/**************************************************************
 * slave_set_watchdog_info
 *************************************************************/
SaErrorT
oh_set_watchdog_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num,
    SaHpiWatchdogT * wdt )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiWatchdogTimerSet,
              rv,
              slave_id,
              num,
              wdt );

    return rv;
}


/**************************************************************
 * slave_reset_watchdog
 *************************************************************/
SaErrorT
oh_reset_watchdog(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiWatchdogNumT num )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiWatchdogTimerReset,
              rv,
              slave_id,
              num );

    return rv;
}


/**************************************************************
 * slave_get_next_announce
 *************************************************************/
SaErrorT
oh_get_next_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiSeverityT sev,
    SaHpiBoolT ack,
    SaHpiAnnouncementT * ann )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorGetNext,
              rv,
              slave_id,
              num,
              sev,
              ack,
              ann );

    if ( rv == SA_OK ) {
        SaHpiResourceIdT master_rid;
        master_rid = handler->GetMaster( ann->StatusCond.ResourceId );
        Slave::TranslateAnnouncement( *ann, master_rid, handler->GetRoot() );
    }

    return rv;
}


/**************************************************************
 * slave_get_announce
 *************************************************************/
SaErrorT
oh_get_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiAnnouncementT * ann )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorGet,
              rv,
              slave_id,
              num,
              annid,
              ann );

    if ( rv == SA_OK ) {
        SaHpiResourceIdT master_rid;
        master_rid = handler->GetMaster( ann->StatusCond.ResourceId );
        Slave::TranslateAnnouncement( *ann, master_rid, handler->GetRoot() );
    }

    return rv;
}


/**************************************************************
 * slave_ack_announce
 *************************************************************/
SaErrorT
oh_ack_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiSeverityT sev )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorAcknowledge,
              rv,
              slave_id,
              num,
              annid,
              sev );

    return rv;
}


/**************************************************************
 * slave_add_announce
 *************************************************************/
SaErrorT
oh_add_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnouncementT * ann )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );

    CALL_ABI( handler,
              saHpiAnnunciatorAdd,
              rv,
              slave_id,
              num,
              ann );

    return rv;
}


/**************************************************************
 * slave_del_announce
 *************************************************************/
SaErrorT
oh_del_announce(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiEntryIdT annid,
    SaHpiSeverityT sev )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorDelete,
              rv,
              slave_id,
              num,
              annid,
              sev );

    return rv;
}


/**************************************************************
 * slave_get_annunc_mode
 *************************************************************/
SaErrorT
oh_get_annunc_mode(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT * mode )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorModeGet,
              rv,
              slave_id,
              num,
              mode );

    return rv;
}


/**************************************************************
 * slave_set_annunc_mode
 *************************************************************/
SaErrorT
oh_set_annunc_mode(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiAnnunciatorNumT num,
    SaHpiAnnunciatorModeT mode )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAnnunciatorModeSet,
              rv,
              slave_id,
              num,
              mode );

    return rv;
}


/**************************************************************
 * slave_get_dimi_info
 *************************************************************/
SaErrorT
oh_get_dimi_info(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiInfoT * info )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiInfoGet,
              rv,
              slave_id,
              num,
              info );

    return rv;
}


/**************************************************************
 * slave_get_dimi_test
 *************************************************************/
SaErrorT
oh_get_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestT * test )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestInfoGet,
              rv,
              slave_id,
              num,
              testnum,
              test );

    if ( rv == SA_OK ) {
        Slave::TranslateDimiTest( *test, handler->GetRoot() );
    }

    return rv;
}


/**************************************************************
 * slave_get_dimi_test_ready
 *************************************************************/
SaErrorT
oh_get_dimi_test_ready(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiReadyT * ready )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestReadinessGet,
              rv,
              slave_id,
              num,
              testnum,
              ready );

    return rv;
}


/**************************************************************
 * slave_start_dimi_test
 *************************************************************/
SaErrorT
oh_start_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiUint8T numparams,
    SaHpiDimiTestVariableParamsT * paramslist )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestStart,
              rv,
              slave_id,
              num,
              testnum,
              numparams,
              paramslist );

    return rv;
}


/**************************************************************
 * slave_cancel_dimi_test
 *************************************************************/
SaErrorT
oh_cancel_dimi_test(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestCancel,
              rv,
              slave_id,
              num,
              testnum );

    return rv;
}


/**************************************************************
 * slave_get_dimi_test_status
 *************************************************************/
SaErrorT
oh_get_dimi_test_status(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestPercentCompletedT * percentcompleted,
    SaHpiDimiTestRunStatusT * runstatus )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestStatusGet,
              rv,
              slave_id,
              num,
              testnum,
              percentcompleted,
              runstatus );

    return rv;
}


/**************************************************************
 * slave_get_dimi_test_results
 *************************************************************/
SaErrorT
oh_get_dimi_test_results(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiDimiNumT num,
    SaHpiDimiTestNumT testnum,
    SaHpiDimiTestResultsT * testresults )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiDimiTestResultsGet,
              rv,
              slave_id,
              num,
              testnum,
              testresults );

    return rv;
}


/**************************************************************
 * slave_get_fumi_spec
 *************************************************************/
SaErrorT
oh_get_fumi_spec(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiSpecInfoT * specinfo  )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiSpecInfoGet,
              rv,
              slave_id,
              num,
              specinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_service_impact
 *************************************************************/
SaErrorT
oh_get_fumi_service_impact(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiServiceImpactDataT * serviceimpact  )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiServiceImpactGet,
              rv,
              slave_id,
              num,
              serviceimpact );

    if ( rv == SA_OK ) {
        for ( unsigned int i = 0; i < serviceimpact->NumEntities; ++i ) {
            Slave::TranslateFumiServiceImpact( *serviceimpact,
                                               handler->GetRoot() );
        }
    }

    return rv;
}


/**************************************************************
 * slave_set_fumi_source
 *************************************************************/
SaErrorT
oh_set_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiTextBufferT * sourceuri )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiSourceSet,
              rv,
              slave_id,
              num,
              banknum,
              sourceuri );

    return rv;
}


/**************************************************************
 * slave_validate_fumi_source
 *************************************************************/
SaErrorT
oh_validate_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiSourceInfoValidateStart,
              rv,
              slave_id,
              num,
              banknum );

    return rv;
}


/**************************************************************
 * slave_get_fumi_source
 *************************************************************/
SaErrorT
oh_get_fumi_source(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiSourceInfoT * sourceinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiSourceInfoGet,
              rv,
              slave_id,
              num,
              banknum,
              sourceinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_source_component
 *************************************************************/
SaErrorT
oh_get_fumi_source_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiSourceComponentInfoGet,
              rv,
              slave_id,
              num,
              banknum,
              compid,
              nextcompid,
              compinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_target
 *************************************************************/
SaErrorT
oh_get_fumi_target(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiBankInfoT * bankinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiTargetInfoGet,
              rv,
              slave_id,
              num,
              banknum,
              bankinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_target_component
 *************************************************************/
SaErrorT
oh_get_fumi_target_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiComponentInfoT * compinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiTargetComponentInfoGet,
              rv,
              slave_id,
              num,
              banknum,
              compid,
              nextcompid,
              compinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_logical_target
 *************************************************************/
SaErrorT
oh_get_fumi_logical_target(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiFumiLogicalBankInfoT * bankinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiLogicalTargetInfoGet,
              rv,
              slave_id,
              num,
              bankinfo );

    return rv;
}


/**************************************************************
 * slave_get_fumi_logical_target_component
 *************************************************************/
SaErrorT
oh_get_fumi_logical_target_component(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiEntryIdT compid,
    SaHpiEntryIdT * nextcompid,
    SaHpiFumiLogicalComponentInfoT * compinfo )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiLogicalTargetComponentInfoGet,
              rv,
              slave_id,
              num,
              compid,
              nextcompid,
              compinfo );

    return rv;
}


/**************************************************************
 * slave_start_fumi_backup
 *************************************************************/
SaErrorT
oh_start_fumi_backup(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiBackupStart,
              rv,
              slave_id,
              num );

    return rv;
}


/**************************************************************
 * slave_set_fumi_bank_order
 *************************************************************/
SaErrorT
oh_set_fumi_bank_order(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiUint32T position )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiBankBootOrderSet,
              rv,
              slave_id,
              num,
              banknum,
              position );

    return rv;
}


/**************************************************************
 * slave_start_fumi_bank_copy
 *************************************************************/
SaErrorT
oh_start_fumi_bank_copy(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT sourcebanknum,
    SaHpiBankNumT targetbanknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiBankCopyStart,
              rv,
              slave_id,
              num,
              sourcebanknum,
              targetbanknum );

    return rv;
}


/**************************************************************
 * slave_start_fumi_install
 *************************************************************/
SaErrorT
oh_start_fumi_install(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiInstallStart,
              rv,
              slave_id,
              num,
              banknum );

    return rv;
}


/**************************************************************
 * slave_get_fumi_status
 *************************************************************/
SaErrorT
oh_get_fumi_status(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum,
    SaHpiFumiUpgradeStatusT * status )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiUpgradeStatusGet,
              rv,
              slave_id,
              num,
              banknum,
              status );

    return rv;
}


/**************************************************************
 * slave_start_fumi_verify
 *************************************************************/
SaErrorT
oh_start_fumi_verify(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiTargetVerifyStart,
              rv,
              slave_id,
              num,
              banknum );

    return rv;
}


/**************************************************************
 * slave_start_fumi_verify_main
 *************************************************************/
SaErrorT
oh_start_fumi_verify_main(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num  )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiTargetVerifyMainStart,
              rv,
              slave_id,
              num );

    return rv;
}


/**************************************************************
 * slave_cancel_fumi_upgrade
 *************************************************************/
SaErrorT
oh_cancel_fumi_upgrade(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiUpgradeCancel,
              rv,
              slave_id,
              num,
              banknum );

    return rv;
}


/**************************************************************
 * slave_get_fumi_autorollback_disable
 *************************************************************/
SaErrorT
oh_get_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT * disable )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiAutoRollbackDisableGet,
              rv,
              slave_id,
              num,
              disable );

    return rv;
}


/**************************************************************
 * slave_set_fumi_autorollback_disable
 *************************************************************/
SaErrorT
oh_set_fumi_autorollback_disable(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT disable )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiAutoRollbackDisableSet,
              rv,
              slave_id,
              num,
              disable );

    return rv;
}


/**************************************************************
 * slave_start_fumi_rollback
 *************************************************************/
SaErrorT
oh_start_fumi_rollback(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiRollbackStart,
              rv,
              slave_id,
              num );

    return rv;
}


/**************************************************************
 * slave_activate_fumi
 *************************************************************/
SaErrorT
oh_activate_fumi(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiActivate,
              rv,
              slave_id,
              num );

    return rv;
}


/**************************************************************
 * slave_start_fumi_activate
 *************************************************************/
SaErrorT
oh_start_fumi_activate(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBoolT logical )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiActivateStart,
              rv,
              slave_id,
              num,
              logical );

    return rv;
}


/**************************************************************
 * slave_cleanup_fumi
 *************************************************************/
SaErrorT
oh_cleanup_fumi(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiFumiNumT num,
    SaHpiBankNumT banknum )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiFumiCleanup,
              rv,
              slave_id,
              num,
              banknum );

    return rv;
}


/**************************************************************
 * slave_hotswap_policy_cancel
 *************************************************************/
SaErrorT
oh_hotswap_policy_cancel(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT timeout )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    // TODO timeout
    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiHotSwapPolicyCancel,
              rv,
              slave_id );

    return rv;
}


/**************************************************************
 * slave_set_autoinsert_timeout
 *************************************************************/
SaErrorT
oh_set_autoinsert_timeout(
    void * hnd,
    SaHpiTimeoutT timeout )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;

    CALL_ABI( handler,
              saHpiAutoInsertTimeoutSet,
              rv,
              timeout );

    return rv;
}


/**************************************************************
 * slave_get_autoextract_timeout
 *************************************************************/
SaErrorT
oh_get_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT * timeout )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAutoExtractTimeoutGet,
              rv,
              slave_id,
              timeout );

    return rv;
}


/**************************************************************
 * slave_set_autoextract_timeout
 *************************************************************/
SaErrorT
oh_set_autoextract_timeout(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiTimeoutT timeout )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiAutoExtractTimeoutSet,
              rv,
              slave_id,
              timeout );

    return rv;
}


/**************************************************************
 * slave_get_hotswap_state
 *************************************************************/
SaErrorT
oh_get_hotswap_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiHotSwapStateGet,
              rv,
              slave_id,
              state );

    return rv;
}


/**************************************************************
 * slave_set_hotswap_state
 *************************************************************/
SaErrorT
oh_set_hotswap_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsStateT state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    if ( state == SAHPI_HS_STATE_ACTIVE ) {
        CALL_ABI( handler,
                  saHpiResourceActiveSet,
                  rv,
                  slave_id );
    } else if ( state == SAHPI_HS_STATE_INACTIVE ) {
        CALL_ABI( handler,
                  saHpiResourceInactiveSet,
                  rv,
                  slave_id );
    } else {
        rv = SA_ERR_HPI_INVALID_PARAMS;
    }

    return rv;
}


/**************************************************************
 * slave_request_hotswap_action
 *************************************************************/
SaErrorT
oh_request_hotswap_action(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsActionT act )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiHotSwapActionRequest,
              rv,
              slave_id,
              act );

    return rv;
}


/**************************************************************
 * slave_get_indicator_state
 *************************************************************/
SaErrorT
oh_get_indicator_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsIndicatorStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiHotSwapIndicatorStateGet,
              rv,
              slave_id,
              state );

    return rv;
}


/**************************************************************
 * slave_set_indicator_state
 *************************************************************/
SaErrorT
oh_set_indicator_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiHsIndicatorStateT state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiHotSwapIndicatorStateSet,
              rv,
              slave_id,
              state );

    return rv;
}


/**************************************************************
 * slave_get_power_state
 *************************************************************/
SaErrorT
oh_get_power_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiPowerStateT * state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourcePowerStateGet,
              rv,
              slave_id,
              state );

    return rv;
}


/**************************************************************
 * slave_set_power_state
 *************************************************************/
SaErrorT
oh_set_power_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiPowerStateT state )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourcePowerStateSet,
              rv,
              slave_id,
              state );

    return rv;
}


/**************************************************************
 * slave_control_parm
 *************************************************************/
SaErrorT
oh_control_parm(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiParmActionT act )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiParmControl,
              rv,
              slave_id,
              act );

    return rv;
}


/**************************************************************
 * slave_load_id_get
 *************************************************************/
SaErrorT
oh_load_id_get(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiLoadIdT * load_id )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceLoadIdGet,
              rv,
              slave_id,
              load_id );

    return rv;
}


/**************************************************************
 * slave_load_id_set
 *************************************************************/
SaErrorT
oh_load_id_set(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiLoadIdT * load_id )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceLoadIdSet,
              rv,
              slave_id,
              load_id );

    return rv;
}


/**************************************************************
 * slave_get_reset_state
 *************************************************************/
SaErrorT
oh_get_reset_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiResetActionT * act )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceResetStateGet,
              rv,
              slave_id,
              act );

    return rv;
}


/**************************************************************
 * slave_set_reset_state
 *************************************************************/
SaErrorT
oh_set_reset_state(
    void * hnd,
    SaHpiResourceIdT id,
    SaHpiResetActionT act )
{
    cHandler * handler = reinterpret_cast<cHandler *>(hnd);
    SaErrorT rv;
    SaHpiResourceIdT slave_id;

    GET_SLAVE( handler, id, slave_id );
    CALL_ABI( handler,
              saHpiResourceResetStateSet,
              rv,
              slave_id,
              act );

    return rv;
}

