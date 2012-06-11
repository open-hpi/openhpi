
#      -*- python -*-
#
# Copyright (C) 2012, Pigeon Point Systems
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#      Anton Pak <anton.pak@pigeonpoint.com>
#



#**********************************************************
# OHPI Constants
#**********************************************************

#**********************************************************
# OHPI RPC IDs
#**********************************************************
RPC_SAHPI_NULL                                   = 0
RPC_SAHPI_SESSION_OPEN                           = 1
RPC_SAHPI_SESSION_CLOSE                          = 2
RPC_SAHPI_DISCOVER                               = 3
RPC_SAHPI_DOMAIN_INFO_GET                        = 4
RPC_SAHPI_DRT_ENTRY_GET                          = 5
RPC_SAHPI_DOMAIN_TAG_SET                         = 6
RPC_SAHPI_RPT_ENTRY_GET                          = 7
RPC_SAHPI_RPT_ENTRY_GET_BY_RESOURCE_ID           = 8
RPC_SAHPI_RESOURCE_SEVERITY_SET                  = 9
RPC_SAHPI_RESOURCE_TAG_SET                       = 10
RPC_SAHPI_RESOURCE_ID_GET                        = 11
RPC_SAHPI_GET_ID_BY_ENTITY_PATH                  = 12
RPC_SAHPI_GET_CHILD_ENTITY_PATH                  = 13
RPC_SAHPI_RESOURCE_FAILED_REMOVE                 = 14
RPC_SAHPI_EVENT_LOG_INFO_GET                     = 15
RPC_SAHPI_EVENT_LOG_CAPABILITIES_GET             = 16
RPC_SAHPI_EVENT_LOG_ENTRY_GET                    = 17
RPC_SAHPI_EVENT_LOG_ENTRY_ADD                    = 18
RPC_SAHPI_EVENT_LOG_CLEAR                        = 19
RPC_SAHPI_EVENT_LOG_TIME_GET                     = 20
RPC_SAHPI_EVENT_LOG_TIME_SET                     = 21
RPC_SAHPI_EVENT_LOG_STATE_GET                    = 22
RPC_SAHPI_EVENT_LOG_STATE_SET                    = 23
RPC_SAHPI_EVENT_LOG_OVERFLOW_RESET               = 24
RPC_SAHPI_SUBSCRIBE                              = 25
RPC_SAHPI_UNSUBSCRIBE                            = 26
RPC_SAHPI_EVENT_GET                              = 27
RPC_SAHPI_EVENT_ADD                              = 28
RPC_SAHPI_ALARM_GET_NEXT                         = 29
RPC_SAHPI_ALARM_GET                              = 30
RPC_SAHPI_ALARM_ACKNOWLEDGE                      = 31
RPC_SAHPI_ALARM_ADD                              = 32
RPC_SAHPI_ALARM_DELETE                           = 33
RPC_SAHPI_RDR_GET                                = 34
RPC_SAHPI_RDR_GET_BY_INSTRUMENT_ID               = 35
RPC_SAHPI_SENSOR_READING_GET                     = 36
RPC_SAHPI_SENSOR_THRESHOLDS_GET                  = 37
RPC_SAHPI_SENSOR_THRESHOLDS_SET                  = 38
RPC_SAHPI_SENSOR_TYPE_GET                        = 39
RPC_SAHPI_SENSOR_ENABLE_GET                      = 40
RPC_SAHPI_SENSOR_ENABLE_SET                      = 41
RPC_SAHPI_SENSOR_EVENT_ENABLE_GET                = 42
RPC_SAHPI_SENSOR_EVENT_ENABLE_SET                = 43
RPC_SAHPI_SENSOR_EVENT_MASKS_GET                 = 44
RPC_SAHPI_SENSOR_EVENT_MASKS_SET                 = 45
RPC_SAHPI_CONTROL_TYPE_GET                       = 46
RPC_SAHPI_CONTROL_GET                            = 47
RPC_SAHPI_CONTROL_SET                            = 48
RPC_SAHPI_IDR_INFO_GET                           = 49
RPC_SAHPI_IDR_AREA_HEADER_GET                    = 50
RPC_SAHPI_IDR_AREA_ADD                           = 51
RPC_SAHPI_IDR_AREA_ADD_BY_ID                     = 52
RPC_SAHPI_IDR_AREA_DELETE                        = 53
RPC_SAHPI_IDR_FIELD_GET                          = 54
RPC_SAHPI_IDR_FIELD_ADD                          = 55
RPC_SAHPI_IDR_FIELD_ADD_BY_ID                    = 56
RPC_SAHPI_IDR_FIELD_SET                          = 57
RPC_SAHPI_IDR_FIELD_DELETE                       = 58
RPC_SAHPI_WATCHDOG_TIMER_GET                     = 59
RPC_SAHPI_WATCHDOG_TIMER_SET                     = 60
RPC_SAHPI_WATCHDOG_TIMER_RESET                   = 61
RPC_SAHPI_ANNUNCIATOR_GET_NEXT                   = 62
RPC_SAHPI_ANNUNCIATOR_GET                        = 63
RPC_SAHPI_ANNUNCIATOR_ACKNOWLEDGE                = 64
RPC_SAHPI_ANNUNCIATOR_ADD                        = 65
RPC_SAHPI_ANNUNCIATOR_DELETE                     = 66
RPC_SAHPI_ANNUNCIATOR_MODE_GET                   = 67
RPC_SAHPI_ANNUNCIATOR_MODE_SET                   = 68
RPC_SAHPI_DIMI_INFO_GET                          = 69
RPC_SAHPI_DIMI_TEST_INFO_GET                     = 70
RPC_SAHPI_DIMI_TEST_READINESS_GET                = 71
RPC_SAHPI_DIMI_TEST_START                        = 72
RPC_SAHPI_DIMI_TEST_CANCEL                       = 73
RPC_SAHPI_DIMI_TEST_STATUS_GET                   = 74
RPC_SAHPI_DIMI_TEST_RESULTS_GET                  = 75
RPC_SAHPI_FUMI_SOURCE_SET                        = 76
RPC_SAHPI_FUMI_SOURCE_INFO_VALIDATE_START        = 77
RPC_SAHPI_FUMI_SOURCE_INFO_GET                   = 78
RPC_SAHPI_FUMI_TARGET_INFO_GET                   = 79
RPC_SAHPI_FUMI_BACKUP_START                      = 80
RPC_SAHPI_FUMI_BANK_BOOT_ORDER_SET               = 81
RPC_SAHPI_FUMI_BANK_COPY_START                   = 82
RPC_SAHPI_FUMI_INSTALL_START                     = 83
RPC_SAHPI_FUMI_UPGRADE_STATUS_GET                = 84
RPC_SAHPI_FUMI_TARGET_VERIFY_START               = 85
RPC_SAHPI_FUMI_UPGRADE_CANCEL                    = 86
RPC_SAHPI_FUMI_ROLLBACK_START                    = 87
RPC_SAHPI_FUMI_ACTIVATE                          = 88
RPC_SAHPI_HOTSWAP_POLICY_CANCEL                  = 89
RPC_SAHPI_RESOURCE_ACTIVE_SET                    = 90
RPC_SAHPI_RESOURCE_INACTIVE_SET                  = 91
RPC_SAHPI_AUTO_INSERT_TIMEOUT_GET                = 92
RPC_SAHPI_AUTO_INSERT_TIMEOUT_SET                = 93
RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_GET               = 94
RPC_SAHPI_AUTO_EXTRACT_TIMEOUT_SET               = 95
RPC_SAHPI_HOTSWAP_STATE_GET                      = 96
RPC_SAHPI_HOTSWAP_ACTION_REQUEST                 = 97
RPC_SAHPI_HOTSWAP_INDICATOR_STATE_GET            = 98
RPC_SAHPI_HOTSWAP_INDICATOR_STATE_SET            = 99
RPC_SAHPI_PARM_CONTROL                           = 100
RPC_SAHPI_RESOURCE_LOADID_GET                    = 101
RPC_SAHPI_RESOURCE_LOADID_SET                    = 102
RPC_SAHPI_RESOURCE_RESET_STATE_GET               = 103
RPC_SAHPI_RESOURCE_RESET_STATE_SET               = 104
RPC_SAHPI_RESOURCE_POWER_STATE_GET               = 105
RPC_SAHPI_RESOURCE_POWER_STATE_SET               = 106
RPC_OHPI_HANDLER_CREATE                          = 107
RPC_OHPI_HANDLER_DESTROY                         = 108
RPC_OHPI_HANDLER_INFO                            = 109
RPC_OHPI_HANDLER_GET_NEXT                        = 110
RPC_OHPI_HANDLER_FIND                            = 111
RPC_OHPI_HANDLER_RETRY                           = 112
RPC_OHPI_GLOBAL_PARAM_GET                        = 113
RPC_OHPI_GLOBAL_PARAM_SET                        = 114
RPC_OHPI_INJECT_EVENT                            = 115
RPC_SAHPI_MY_ENTITY_PATH_GET                     = 116
RPC_SAHPI_RDR_UPDATE_COUNT_GET                   = 117
RPC_SAHPI_FUMI_SPEC_INFO_GET                     = 118
RPC_SAHPI_FUMI_SERVICE_IMPACT_GET                = 119
RPC_SAHPI_FUMI_SOURCE_COMPONENT_INFO_GET         = 120
RPC_SAHPI_FUMI_TARGET_COMPONENT_INFO_GET         = 121
RPC_SAHPI_FUMI_LOGICAL_TARGET_INFO_GET           = 122
RPC_SAHPI_FUMI_LOGICAL_TARGET_COMPONENT_INFO_GET = 123
RPC_SAHPI_FUMI_TARGET_VERIFY_MAIN_START          = 124
RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_GET         = 125
RPC_SAHPI_FUMI_AUTO_ROLLBACK_DISABLE_SET         = 126
RPC_SAHPI_FUMI_ACTIVATE_START                    = 127
RPC_SAHPI_FUMI_CLEANUP                           = 128


DEFAULT_PORT = 4743
DEFAULT_DOMAIN_ID = 0

MAX_PLUGIN_NAME_LENGTH = 32


#**********************************************************
#* OHPI Complex Data Types
#**********************************************************
#**
# OHPI struct oHpiHandlerInfoT
#**
class oHpiHandlerInfoT:
    def __init__( self ):
        # oHpiHandlerIdT
        self.id = None
        # SaHpiUint8T[MAX_PLUGIN_NAME_LENGTH]
        self.plugin_name = None
        # SaHpiEntityPathT
        self.entity_root = None
        # SaHpiInt32T
        self.load_failed = None

#**
# OHPI struct oHpiHandlerConfigT
#**
class oHpiHandlerConfigT:
    def __init__( self ):
        # list of ( name, value ) tuples
        # Both name and value are SaHpiUint8T[SAHPI_MAX_TEXT_BUFFER_LENGTH]
        self.items = None

