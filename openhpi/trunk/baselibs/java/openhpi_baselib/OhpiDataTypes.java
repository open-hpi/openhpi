/*      -*- java -*-
 *
 * Copyright (C) 2012, Pigeon Point Systems
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 */

package org.openhpi;

import java.util.List;
import java.util.Map.Entry;

import static org.openhpi.HpiDataTypes.*;


public class OhpiDataTypes
{
    // Just to ensure nobody creates it
    protected OhpiDataTypes()
    {
        // empty
    }


    /**********************************************************
     * OHPI RPC IDs
     *********************************************************/

    public static final int RPC_SAHPI_NULL                                   = 0;
    public static final int RPC_SAHPI_SESSION_OPEN                           = 1;
    public static final int RPC_SAHPI_SESSION_CLOSE                          = 2;
    public static final int RPC_SAHPI_DISCOVER                               = 3;
    public static final int RPC_SAHPI_DOMAIN_INFO_GET                        = 4;
    public static final int RPC_SAHPI_DRT_ENTRY_GET                          = 5;
    public static final int RPC_SAHPI_DOMAIN_TAG_SET                         = 6;
    public static final int RPC_SAHPI_RPT_ENTRY_GET                          = 7;
    public static final int RPC_SAHPI_RPT_ENTRY_GET_BY_RESOURCE_ID           = 8;
    public static final int RPC_SAHPI_RESOURCE_SEVERITY_SET                  = 9;
    public static final int RPC_SAHPI_RESOURCE_TAG_SET                       = 10;
    public static final int RPC_SAHPI_RESOURCE_ID_GET                        = 11;
    public static final int RPC_SAHPI_GET_ID_BY_ENTITY_PATH                  = 12;
    public static final int RPC_SAHPI_GET_CHILD_ENTITY_PATH                  = 13;
    public static final int RPC_SAHPI_RESOURCE_FAILED_REMOVE                 = 14;
    public static final int RPC_SAHPI_EVENT_LOG_INFO_GET                     = 15;
    public static final int RPC_SAHPI_EVENT_LOG_CAPABILITIES_GET             = 16;
    public static final int RPC_SAHPI_EVENT_LOG_ENTRY_GET                    = 17;
    public static final int RPC_SAHPI_EVENT_LOG_ENTRY_ADD                    = 18;
    public static final int RPC_SAHPI_EVENT_LOG_CLEAR                        = 19;
    public static final int RPC_SAHPI_EVENT_LOG_TIME_GET                     = 20;
    public static final int RPC_SAHPI_EVENT_LOG_TIME_SET                     = 21;
    public static final int RPC_SAHPI_EVENT_LOG_STATE_GET                    = 22;
    public static final int RPC_SAHPI_EVENT_LOG_STATE_SET                    = 23;
    public static final int RPC_SAHPI_EVENT_LOG_OVERFLOW_RESET               = 24;
    public static final int RPC_SAHPI_SUBSCRIBE                              = 25;
    public static final int RPC_SAHPI_UNSUBSCRIBE                            = 26;
    public static final int RPC_SAHPI_EVENT_GET                              = 27;
    public static final int RPC_SAHPI_EVENT_ADD                              = 28;
    public static final int RPC_SAHPI_ALARM_GET_NEXT                         = 29;
    public static final int RPC_SAHPI_ALARM_GET                              = 30;
    public static final int RPC_SAHPI_ALARM_ACKNOWLEDGE                      = 31;
    public static final int RPC_SAHPI_ALARM_ADD                              = 32;
    public static final int RPC_SAHPI_ALARM_DELETE                           = 33;
    public static final int RPC_SAHPI_RDR_GET                                = 34;
    public static final int RPC_SAHPI_RDR_GET_BY_INSTRUMENT_ID               = 35;
    public static final int RPC_SAHPI_SENSOR_READING_GET                     = 36;
    public static final int RPC_SAHPI_SENSOR_THRESHOLDS_GET                  = 37;
    public static final int RPC_SAHPI_SENSOR_THRESHOLDS_SET                  = 38;
    public static final int RPC_SAHPI_SENSOR_TYPE_GET                        = 39;
    public static final int RPC_SAHPI_SENSOR_ENABLE_GET                      = 40;
    public static final int RPC_SAHPI_SENSOR_ENABLE_SET                      = 41;
    public static final int RPC_SAHPI_SENSOR_EVENT_ENABLE_GET                = 42;
    public static final int RPC_SAHPI_SENSOR_EVENT_ENABLE_SET                = 43;
    public static final int RPC_SAHPI_SENSOR_EVENT_MASKS_GET                 = 44;
    public static final int RPC_SAHPI_SENSOR_EVENT_MASKS_SET                 = 45;
    public static final int RPC_SAHPI_CONTROL_TYPE_GET                       = 46;
    public static final int RPC_SAHPI_CONTROL_GET                            = 47;
    public static final int RPC_SAHPI_CONTROL_SET                            = 48;
    public static final int RPC_SAHPI_IDR_INFO_GET                           = 49;
    public static final int RPC_SAHPI_IDR_AREA_HEADER_GET                    = 50;
    public static final int RPC_SAHPI_IDR_AREA_ADD                           = 51;
    public static final int RPC_SAHPI_IDR_AREA_ADD_BY_ID                     = 52;
    public static final int RPC_SAHPI_IDR_AREA_DELETE                        = 53;
    public static final int RPC_SAHPI_IDR_FIELD_GET                          = 54;
    public static final int RPC_SAHPI_IDR_FIELD_ADD                          = 55;
    public static final int RPC_SAHPI_IDR_FIELD_ADD_BY_ID                    = 56;
    public static final int RPC_SAHPI_IDR_FIELD_SET                          = 57;
    public static final int RPC_SAHPI_IDR_FIELD_DELETE                       = 58;
    public static final int RPC_SAHPI_WATCHDOG_TIMER_GET                     = 59;
    public static final int RPC_SAHPI_WATCHDOG_TIMER_SET                     = 60;
    public static final int RPC_SAHPI_WATCHDOG_TIMER_RESET                   = 61;
    public static final int RPC_SAHPI_ANNUNCIATOR_GET_NEXT                   = 62;
    public static final int RPC_SAHPI_ANNUNCIATOR_GET                        = 63;
    public static final int RPC_SAHPI_ANNUNCIATOR_ACKNOWLEDGE                = 64;
    public static final int RPC_SAHPI_ANNUNCIATOR_ADD                        = 65;
    public static final int RPC_SAHPI_ANNUNCIATOR_DELETE                     = 66;
    public static final int RPC_SAHPI_ANNUNCIATOR_MODE_GET                   = 67;
    public static final int RPC_SAHPI_ANNUNCIATOR_MODE_SET                   = 68;
    public static final int RPC_SAHPI_DIMI_INFO_GET                          = 69;
    public static final int RPC_SAHPI_DIMI_TEST_INFO_GET                     = 70;
    public static final int RPC_SAHPI_DIMI_TEST_READINESS_GET                = 71;
    public static final int RPC_SAHPI_DIMI_TEST_START                        = 72;
    public static final int RPC_SAHPI_DIMI_TEST_CANCEL                       = 73;
    public static final int RPC_SAHPI_DIMI_TEST_STATUS_GET                   = 74;
    public static final int RPC_SAHPI_DIMI_TEST_RESULTS_GET                  = 75;
    public static final int RPC_SAHPI_FUMI_SOURCE_SET                        = 76;
    public static final int RPC_SAHPI_FUMI_SOURCE_INFO_VALIDATE_START        = 77;
    public static final int RPC_SAHPI_FUMI_SOURCE_INFO_GET                   = 78;
    public static final int RPC_SAHPI_FUMI_TARGET_INFO_GET                   = 79;
    public static final int RPC_SAHPI_FUMI_BACKUP_START                      = 80;
    public static final int RPC_SAHPI_FUMI_BANK_BOOT_ORDER_SET               = 81;
    public static final int RPC_SAHPI_FUMI_BANK_COPY_START                   = 82;
    public static final int RPC_SAHPI_FUMI_INSTALL_START                     = 83;
    public static final int RPC_SAHPI_FUMI_UPGRADE_STATUS_GET                = 84;
    public static final int RPC_SAHPI_FUMI_TARGET_VERIFY_START               = 85;
    public static final int RPC_SAHPI_FUMI_UPGRADE_CANCEL                    = 86;
    public static final int RPC_SAHPI_FUMI_ROLLBACK_START                    = 87;
    public static final int RPC_SAHPI_FUMI_ACTIVATE                          = 88;
    public static final int RPC_SAHPI_HOTSWAP_POLICY_CANCEL                  = 89;
    public static final int RPC_SAHPI_RESOURCE_ACTIVE_SET                    = 90;
    public static final int RPC_SAHPI_RESOURCE_INACTIVE_SET                  = 91;
    public static final int RPC_SAHPI_AUTO_INSERT_TIMEOUT_GET                = 92;
    public static final int RPC_SAHPI_AUTO_INSERT_TIMEOUT_SET                = 93;
    public static final int RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_GET               = 94;
    public static final int RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_SET               = 95;
    public static final int RPC_SAHPI_HOTSWAP_STATE_GET                      = 96;
    public static final int RPC_SAHPI_HOTSWAP_ACTION_REQUEST                 = 97;
    public static final int RPC_SAHPI_HOTSWAP_INDICATOR_STATE_GET            = 98;
    public static final int RPC_SAHPI_HOTSWAP_INDICATOR_STATE_SET            = 99;
    public static final int RPC_SAHPI_PARM_CONTROL                           = 100;
    public static final int RPC_SAHPI_RESOURCE_LOADID_GET                    = 101;
    public static final int RPC_SAHPI_RESOURCE_LOADID_SET                    = 102;
    public static final int RPC_SAHPI_RESOURCE_RESET_STATE_GET               = 103;
    public static final int RPC_SAHPI_RESOURCE_RESET_STATE_SET               = 104;
    public static final int RPC_SAHPI_RESOURCE_POWER_STATE_GET               = 105;
    public static final int RPC_SAHPI_RESOURCE_POWER_STATE_SET               = 106;
    public static final int RPC_OHPI_HANDLER_CREATE                          = 107;
    public static final int RPC_OHPI_HANDLER_DESTROY                         = 108;
    public static final int RPC_OHPI_HANDLER_INFO                            = 109;
    public static final int RPC_OHPI_HANDLER_GET_NEXT                        = 110;
    public static final int RPC_OHPI_HANDLER_FIND                            = 111;
    public static final int RPC_OHPI_HANDLER_RETRY                           = 112;
    public static final int RPC_OHPI_GLOBAL_PARAM_GET                        = 113;
    public static final int RPC_OHPI_GLOBAL_PARAM_SET                        = 114;
    public static final int RPC_OHPI_INJECT_EVENT                            = 115;
    public static final int RPC_SAHPI_MY_ENTITY_PATH_GET                     = 116;
    public static final int RPC_SAHPI_RDR_UPDATE_COUNT_GET                   = 117;
    public static final int RPC_SAHPI_FUMI_SPEC_INFO_GET                     = 118;
    public static final int RPC_SAHPI_FUMI_SERVICE_IMPACT_GET                = 119;
    public static final int RPC_SAHPI_FUMI_SOURCE_COMPONENT_INFO_GET         = 120;
    public static final int RPC_SAHPI_FUMI_TARGET_COMPONENT_INFO_GET         = 121;
    public static final int RPC_SAHPI_FUMI_LOGICAL_TARGET_INFO_GET           = 122;
    public static final int RPC_SAHPI_FUMI_LOGICAL_TARGET_COMPONENT_INFO_GET = 123;
    public static final int RPC_SAHPI_FUMI_TARGET_VERIFY_MAIN_START          = 124;
    public static final int RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_GET         = 125;
    public static final int RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_SET         = 126;
    public static final int RPC_SAHPI_FUMI_ACTIVATE_START                    = 127;
    public static final int RPC_SAHPI_FUMI_CLEANUP                           = 128;


    /**********************************************************
     * OHPI Constants
     *********************************************************/

    public static final int  OPENHPI_DEFAULT_DAEMON_PORT = 4743;
    public static final long OPENHPI_DEFAULT_DOMAIN_ID = 0L;

    public static final int OPENHPI_MAX_PLUGIN_NAME_LENGTH = 32;


    /**********************************************************
     * OHPI Simple Data Types Map
     *********************************************************/

    /**********************************************************
     *  oHpiHandlerIdT : long
     *********************************************************/


    /**********************************************************
     * OHPI Complex Data Types
     *********************************************************/

    /**
     * OHPI struct oHpiHandlerInfoT
     */
    public static class oHpiHandlerInfoT
    {
        public long id;
        // Byte[OPENHPI_MAX_PLUGIN_NAME_LENGTH]
        public byte[] plugin_name;
        public SaHpiEntityPathT entity_root;
        public long load_failed;
    };

    /**
     * OHPI struct oHpiHandlerConfigT
     */
    public static class oHpiHandlerConfigT
    {
        // List of ( name, value ) pairs
        // Both name and value are arrays if SAHPI_MAX_TEXT_BUFFER_LENGTH elements
        public List< Entry<byte[], byte[]> > items;
    };


    /**********************************************************
     * OHPI API Returns Types (NB: Partly implemented)
     *********************************************************/

    /**
     * Represents output parameters
     * for oHpiDomainAdd().
     */
    public static class oHpiDomainAddOutputParamsT
    {
        public long DomainId;
    };

    /**
     * Represents output parameters
     * for oHpiHandlerCreate().
     */
    public static class oHpiHandlerCreateOutputParamsT
    {
        public long HandlerId;
    };

    /**
     * Represents output parameters
     * for oHpiHandlerInfo().
     */
    public static class oHpiHandlerInfoOutputParamsT
    {
        public oHpiHandlerInfoT HandlerInfo;
        public oHpiHandlerConfigT HandlerConfig;
    };

    /**
     * Represents output parameters
     * for oHpiHandlerGetNext().
     */
    public static class oHpiHandlerGetNextOutputParamsT
    {
        public long NextHandlerId;
    };

    /**
     * Represents output parameters
     * for oHpiHandlerFind().
     */
    public static class oHpiHandlerFindOutputParamsT
    {
        public long HandlerId;
    };


};

