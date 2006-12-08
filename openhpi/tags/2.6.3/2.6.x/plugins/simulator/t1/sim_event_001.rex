/*      -*- linux-c -*-
*
*(C) Copyright IBM Corp. 2005
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
* file and program are licensed under a BSD style license.  See
* the Copying file included with the OpenHPI distribution for
* full licensing terms.
*
* Authors:
*     W. david Ashley <dashley@us.ibm.com>
*/


/* this constant wiil need to change if the simulator handler name changes */
handler = 'simulator'

call value 'LD_LIBRARY_PATH', '.libs', 'ENVIRONMENT'

if rxfuncquery('LnxOpenQueue') = 1 then do
   call rxfuncadd 'LnxOpenQueue', 'rxlinuxqueue', 'LnxOpenQueue'
   call rxfuncadd 'LnxWriteQueue', 'rxlinuxqueue', 'LnxWriteQueue'
   call rxfuncadd 'LnxGetHpiConst', 'rxlinuxqueue', 'LnxGetHpiConst'
   end

/* these are the constants from the sim_injector_ext.h file */
SIM_MSG_SENSOR_EVENT = 1
SIM_MSG_SENSOR_ENABLE_CHANGE_EVENT = 2
SIM_MSG_HOT_SWAP_EVENT = 3
SIM_MSG_WATCHDOG_EVENT = 4
SIM_MSG_SW_EVENT = 5
SIM_MSG_OEM_EVENT = 6
SIM_MSG_USER_EVENT = 7
SIM_MSG_RESOURCE_EVENT = 8
SIM_MSG_DOMAIN_EVENT = 9
SIM_MSG_RESOURCE_ADD_EVENT = 10
SIM_MSG_RDR_ADD_EVENT = 11
SIM_MSG_HANDLER_NAME = "handler_name"
SIM_MSG_EVENT_SEVERITY = "event_severity"
SIM_MSG_RESOURCE_ID = "resource_id"
SIM_MSG_SENSOR_NUM = "sensor_num"
SIM_MSG_SENSOR_TYPE = "sensor_type"
SIM_MSG_SENSOR_EVENT_CATEGORY = "sensor_category"
SIM_MSG_SENSOR_ASSERTION = "sensor_assertion"
SIM_MSG_SENSOR_EVENT_STATE = "sensor_state"
SIM_MSG_SENSOR_OPTIONAL_DATA = "sensor_opt_data"
SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED = "sensor_trigger_reading_supported"
SIM_MSG_SENSOR_TRIGGER_READING_TYPE = "sensor_trigger_reading_type"
SIM_MSG_SENSOR_TRIGGER_READING = "sensor_trigger_reading"
SIM_MSG_SENSOR_TRIGGER_THRESHOLD_SUPPORTED = "sensor_trigger_threshold_supported"
SIM_MSG_SENSOR_TRIGGER_THRESHOLD_TYPE = "sensor_trigger_threshold_type"
SIM_MSG_SENSOR_TRIGGER_THRESHOLD = "sensor_trigger_threshold"
SIM_MSG_SENSOR_PREVIOUS_STATE = "sensor_previous_state"
SIM_MSG_SENSOR_CURRENT_STATE = "sensor_current_state"
SIM_MSG_SENSOR_OEM = "sensor_oem"
SIM_MSG_SENSOR_SPECIFIC = "sensor_specific"
SIM_MSG_SENSOR_ENABLE = "sensor_enable"
SIM_MSG_SENSOR_EVENT_ENABLE = "sensor_event_enable"
SIM_MSG_SENSOR_ASSERT_EVENT_MASK = "sensor_assert_event_mask"
SIM_MSG_SENSOR_DEASSERT_EVENT_MASK = "sensor_deassert_event_mask"
SIM_MSG_HS_STATE = "hs_state"
SIM_MSG_HS_PREVIOUS_STATE = "hs_previous_state"
SIM_MSG_WATCHDOG_NUM = "watchdog_num"
SIM_MSG_WATCHDOG_ACTION = "watchdog_action"
SIM_MSG_WATCHDOG_PRETIMER_ACTION = "watchdog_pretimer_action"
SIM_MSG_WATCHDOG_TIMER_USE = "watchdog_timer_use"
SIM_MSG_SW_MID = "sw_mid"
SIM_MSG_SW_EVENT_TYPE = "sw_event_type"
SIM_MSG_SW_EVENT_DATA = "sw_event_data"
SIM_MSG_OEM_MID = "oem_mid"
SIM_MSG_OEM_EVENT_DATA = "oem_event_data"
SIM_MSG_USER_EVENT_DATA = "user_event_data"
SIM_MSG_RESOURCE_EVENT_TYPE = "resource_event_type"
SIM_MSG_DOMAIN_TYPE = "domain_type"
SIM_MSG_DOMAIN_ID = "domain_id"
SIM_MSG_RPT_ENTITYPATH = "rpt_entitypath"
SIM_MSG_RPT_CAPABILITIES = "rpt_capabilities"
SIM_MSG_RPT_HSCAPABILITIES = "rpt_hscapabilities"
SIM_MSG_RPT_FAILED = "rpt_failed"
SIM_MSG_RPT_RESTAG = "rpt_restag"
SIM_MSG_RPT_COMMENT = "rpt_comment"
SIM_MSG_RPT_RESINFO_RESREV = "rpt_resinfo_resrev"
SIM_MSG_RPT_RESINFO_SPECVER = "rpt_resinfo_specver"
SIM_MSG_RPT_RESINFO_DEVSUPPORT = "rpt_resinfo_devsupport"
SIM_MSG_RPT_RESINFO_MFGID = "rpt_resinfo_mfgid"
SIM_MSG_RPT_RESINFO_PRODID = "rpt_resinfo_prodid"
SIM_MSG_RPT_RESINFO_FIRMMAJREV = "rpt_resinfo_firmmajrev"
SIM_MSG_RPT_RESINFO_FIRMMINREV = "rpt_resinfo_firmminrev"
SIM_MSG_RPT_RESINFO_AUXFIRMREV = "rpt_resinfo_auxfirmrev"
SIM_MSG_RDR_TYPE = "rdr_type"
SIM_MSG_RDR_ENTITYPATH = "rdr_entitypath"
SIM_MSG_RDR_FRU = "rdr_fru"
SIM_MSG_RDR_IDSTRING = "rdr_idstring"
SIM_MSG_RDR_ANNUN_NUM = "rdr_annum_num"
SIM_MSG_RDR_ANNUN_TYPE = "rdr_annum_type"
SIM_MSG_RDR_ANNUN_MODE = "rdr_annum_mode"
SIM_MSG_RDR_ANNUN_MAXCOND = "rdr_annum_maxcond"
SIM_MSG_RDR_ANNUN_OEM = "rdr_annum_oem"
SIM_MSG_RDR_WATCHDOG_NUM = "rdr_watchdog_num"
SIM_MSG_RDR_WATCHDOG_OEM = "rdr_watchdog_oem"
SIM_MSG_RDR_IDR_ID = "rdr_idr_id"
SIM_MSG_RDR_IDR_PERSISTENT = "rdr_idr_persistent"
SIM_MSG_RDR_IDR_OEM = "rdr_idr_oem"
SIM_MSG_RDR_CTRL_NUM = "rdr_ctrl_num"
SIM_MSG_RDR_CTRL_OUTTYPE = "rdr_ctrl_outtype"
SIM_MSG_RDR_CTRL_TYPE = "rdr_ctrl_type"
SIM_MSG_RDR_CTRL_MODE = "rdr_ctrl_mode"
SIM_MSG_RDR_CTRL_MODE_READONLY = "rdr_ctrl_mode_readonly"
SIM_MSG_RDR_CTRL_WRITEONLY = "rdr_ctrl_writeonly"
SIM_MSG_RDR_CTRL_OEM = "rdr_ctrl_oem"
SIM_MSG_RDR_CTRL_DIGITAL_DEFSTATE = "rdr_ctrl_digital_defstate"
SIM_MSG_RDR_CTRL_DISCRETE_DEFSTATE = "rdr_ctrl_discrete_defstate"
SIM_MSG_RDR_CTRL_ANALOG_MIN = "rdr_ctrl_analog_min"
SIM_MSG_RDR_CTRL_ANALOG_MAX = "rdr_ctrl_analog_max"
SIM_MSG_RDR_CTRL_ANALOG_DEFAULT = "rdr_ctrl_analog_default"
SIM_MSG_RDR_CTRL_STREAM = "rdr_ctrl_stream"
SIM_MSG_RDR_CTRL_TEXT = "rdr_ctrl_text"
SIM_MSG_RDR_CTRL_OEM_MID = "rdr_ctrl_oem_mid"
SIM_MSG_RDR_CTRL_OEM_CONFIG = "rdr_ctrl_oem_config"
SIM_MSG_RDR_CTRL_OEM_BODY = "rdr_ctrl_oem_body"
SIM_MSG_RDR_SENSOR_NUM = "rdr_sensor_num"
SIM_MSG_RDR_SENSOR_TYPE = "rdr_sensor_type"
SIM_MSG_RDR_SENSOR_CATEGORY = "rdr_sensor_category"
SIM_MSG_RDR_SENSOR_ENABLECTRL = "rdr_sensor_enablectrl"
SIM_MSG_RDR_SENSOR_EVENTCTRL = "rdr_sensor_eventctrl"
SIM_MSG_RDR_SENSOR_EVENTSTATE = "rdr_sensor_eventstate"
SIM_MSG_RDR_SENSOR_DATA_SUPPORTED = "rdr_sensor_data_supported"
SIM_MSG_RDR_SENSOR_DATA_READINGTYPE = "rdr_sensor_data_readingtype"
SIM_MSG_RDR_SENSOR_DATA_BASEUNITS = "rdr_sensor_data_baseunits"
SIM_MSG_RDR_SENSOR_DATA_MODUNITS = "rdr_sensor_data_modunits"
SIM_MSG_RDR_SENSOR_DATA_MODUSE = "rdr_sensor_data_moduse"
SIM_MSG_RDR_SENSOR_DATA_PERCENT = "rdr_sensor_data_percent"
SIM_MSG_RDR_SENSOR_DATA_RANGE_FLAGS = "rdr_sensor_data_range_flags"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_SUPPORTED = "rdr_sensor_data_range_max_supported"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_TYPE = "rdr_sensor_data_range_max_type"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MAX_VALUE = "rdr_sensor_data_range_max_value"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MIN_SUPPORTED = "rdr_sensor_data_range_min_supported"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MIN_TYPE = "rdr_sensor_data_range_min_type"
SIM_MSG_RDR_SENSOR_DATA_RANGE_MIN_VALUE = "rdr_sensor_data_range_min_value"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NOMINAL_SUPPORTED = "rdr_sensor_data_range_nominal_supported"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NOMINAL_TYPE = "rdr_sensor_data_range_nominal_type"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NOMINAL_VALUE = "rdr_sensor_data_range_nominal_value"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMAX_SUPPORTED = "rdr_sensor_data_range_normalmax_supported"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMAX_TYPE = "rdr_sensor_data_range_normalmax_type"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMAX_VALUE = "rdr_sensor_data_range_normalmax_value"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMIN_SUPPORTED = "rdr_sensor_data_range_normalmin_supported"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMIN_TYPE = "rdr_sensor_data_range_normalmin_type"
SIM_MSG_RDR_SENSOR_DATA_RANGE_NORMALMIN_VALUE = "rdr_sensor_data_range_normalmin_value"
SIM_MSG_RDR_SENSOR_DATA_ACCURACY = "rdr_sensor_data_accuracy"
SIM_MSG_RDR_SENSOR_THRESHOLD_ACCESSIBLE = "rdr_sensor_threshold_acessible"
SIM_MSG_RDR_SENSOR_THRESHOLD_READ = "rdr_sensor_threshold_read"
SIM_MSG_RDR_SENSOR_THRESHOLD_WRITE = "rdr_sensor_threshold_write"
SIM_MSG_RDR_SENSOR_THRESHOLD_NONLINEAR = "rdr_sensor_threshold_nonlinear"
SIM_MSG_RDR_SENSOR_OEM = "rdr_sensor_oem"



buf = .mutablebuffer~new('' , 1024)

queid = LnxOpenQueue('.')
if queid = -1 then do
   say 'Error: queue open.'
   return -1
   end

buf~append(SIM_MSG_HANDLER_NAME'='handler || d2c(0))
buf~append(SIM_MSG_RESOURCE_ID'=1' || d2c(0))
buf~append(SIM_MSG_EVENT_SEVERITY'='LnxGetHpiConst('SAHPI_CRITICAL') || d2c(0))
buf~append(SIM_MSG_SENSOR_NUM'=1' || d2c(0))
buf~append(SIM_MSG_SENSOR_TYPE'='LnxGetHpiConst('SAHPI_TEMPERATURE') || d2c(0))
buf~append(SIM_MSG_SENSOR_EVENT_CATEGORY'='LnxGetHpiConst('SAHPI_EC_THRESHOLD') || d2c(0))
buf~append(SIM_MSG_SENSOR_ASSERTION'='LnxGetHpiConst('SAHPI_TRUE') || d2c(0))
buf~append(SIM_MSG_SENSOR_EVENT_STATE'='LnxGetHpiConst('SAHPI_ES_UPPER_CRIT') || d2c(0))
buf~append(SIM_MSG_SENSOR_OPTIONAL_DATA'='LnxGetHpiConst('SAHPI_SOD_TRIGGER_READING') || d2c(0))
buf~append(SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED'='LnxGetHpiConst('SAHPI_TRUE') || d2c(0))
buf~append(SIM_MSG_SENSOR_TRIGGER_READING_TYPE'='LnxGetHpiConst('SAHPI_SENSOR_READING_TYPE_INT64') || d2c(0))
buf~append(SIM_MSG_SENSOR_TRIGGER_READING'=21' || d2c(0))

/* send the msg */
rc = LnxWriteQueue(queid, SIM_MSG_SENSOR_EVENT, buf~string)
if (rc <> 0) then do
    say 'Error: message send.'
    return -1
    end
say 'Info: message sent.'

return 0

