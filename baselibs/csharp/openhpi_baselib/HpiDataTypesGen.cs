/*      -*- c# -*-
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


namespace org {
namespace openhpi {


/**********************************************************
 * HPI Data Types (auto-generated)
 *********************************************************/
/**********************************************************
 * HPI Constants
 *********************************************************/
public static class HpiConst
{
    public const long SAHPI_TRUE = 1L;
    public const long SAHPI_FALSE = 0L;
    public const long SAHPI_MANUFACTURER_ID_UNSPECIFIED = 0L;
    public const long SAHPI_INTERFACE_VERSION = 131842L;
    public const long SA_OK = 0L;
    public const long SA_HPI_ERR_BASE = -1000L;
    public const long SA_ERR_HPI_ERROR = -1001L;
    public const long SA_ERR_HPI_UNSUPPORTED_API = -1002L;
    public const long SA_ERR_HPI_BUSY = -1003L;
    public const long SA_ERR_HPI_INTERNAL_ERROR = -1004L;
    public const long SA_ERR_HPI_INVALID_CMD = -1005L;
    public const long SA_ERR_HPI_TIMEOUT = -1006L;
    public const long SA_ERR_HPI_OUT_OF_SPACE = -1007L;
    public const long SA_ERR_HPI_OUT_OF_MEMORY = -1008L;
    public const long SA_ERR_HPI_INVALID_PARAMS = -1009L;
    public const long SA_ERR_HPI_INVALID_DATA = -1010L;
    public const long SA_ERR_HPI_NOT_PRESENT = -1011L;
    public const long SA_ERR_HPI_NO_RESPONSE = -1012L;
    public const long SA_ERR_HPI_DUPLICATE = -1013L;
    public const long SA_ERR_HPI_INVALID_SESSION = -1014L;
    public const long SA_ERR_HPI_INVALID_DOMAIN = -1015L;
    public const long SA_ERR_HPI_INVALID_RESOURCE = -1016L;
    public const long SA_ERR_HPI_INVALID_REQUEST = -1017L;
    public const long SA_ERR_HPI_ENTITY_NOT_PRESENT = -1018L;
    public const long SA_ERR_HPI_READ_ONLY = -1019L;
    public const long SA_ERR_HPI_CAPABILITY = -1020L;
    public const long SA_ERR_HPI_UNKNOWN = -1021L;
    public const long SA_ERR_HPI_INVALID_STATE = -1022L;
    public const long SA_ERR_HPI_UNSUPPORTED_PARAMS = -1023L;
    public const long SAHPI_UNSPECIFIED_DOMAIN_ID = 4294967295L;
    public const long SAHPI_UNSPECIFIED_RESOURCE_ID = 4294967295L;
    public const long SAHPI_FIRST_ENTRY = 0L;
    public const long SAHPI_LAST_ENTRY = 4294967295L;
    public const long SAHPI_ENTRY_UNSPECIFIED = 0L;
    public const long SAHPI_TIME_UNSPECIFIED = -9223372036854775808L;
    public const long SAHPI_TIME_MAX_RELATIVE = 864691128455135232L;
    public const long SAHPI_TIMEOUT_IMMEDIATE = 0L;
    public const long SAHPI_TIMEOUT_BLOCK = -1L;
    public const int SAHPI_MAX_TEXT_BUFFER_LENGTH = 255;
    public const long SAHPI_ENT_IPMI_GROUP = 0L;
    public const long SAHPI_ENT_SAFHPI_GROUP = 65536L;
    public const long SAHPI_ENT_ROOT_VALUE = 65535L;
    public const int SAHPI_MAX_ENTITY_PATH = 16;
    public const long SAHPI_EC_UNSPECIFIED = 0L;
    public const long SAHPI_EC_THRESHOLD = 1L;
    public const long SAHPI_EC_USAGE = 2L;
    public const long SAHPI_EC_STATE = 3L;
    public const long SAHPI_EC_PRED_FAIL = 4L;
    public const long SAHPI_EC_LIMIT = 5L;
    public const long SAHPI_EC_PERFORMANCE = 6L;
    public const long SAHPI_EC_SEVERITY = 7L;
    public const long SAHPI_EC_PRESENCE = 8L;
    public const long SAHPI_EC_ENABLE = 9L;
    public const long SAHPI_EC_AVAILABILITY = 10L;
    public const long SAHPI_EC_REDUNDANCY = 11L;
    public const long SAHPI_EC_SENSOR_SPECIFIC = 126L;
    public const long SAHPI_EC_GENERIC = 127L;
    public const long SAHPI_ES_UNSPECIFIED = 0L;
    public const long SAHPI_ES_LOWER_MINOR = 1L;
    public const long SAHPI_ES_LOWER_MAJOR = 2L;
    public const long SAHPI_ES_LOWER_CRIT = 4L;
    public const long SAHPI_ES_UPPER_MINOR = 8L;
    public const long SAHPI_ES_UPPER_MAJOR = 16L;
    public const long SAHPI_ES_UPPER_CRIT = 32L;
    public const long SAHPI_ES_IDLE = 1L;
    public const long SAHPI_ES_ACTIVE = 2L;
    public const long SAHPI_ES_BUSY = 4L;
    public const long SAHPI_ES_STATE_DEASSERTED = 1L;
    public const long SAHPI_ES_STATE_ASSERTED = 2L;
    public const long SAHPI_ES_PRED_FAILURE_DEASSERT = 1L;
    public const long SAHPI_ES_PRED_FAILURE_ASSERT = 2L;
    public const long SAHPI_ES_LIMIT_NOT_EXCEEDED = 1L;
    public const long SAHPI_ES_LIMIT_EXCEEDED = 2L;
    public const long SAHPI_ES_PERFORMANCE_MET = 1L;
    public const long SAHPI_ES_PERFORMANCE_LAGS = 2L;
    public const long SAHPI_ES_OK = 1L;
    public const long SAHPI_ES_MINOR_FROM_OK = 2L;
    public const long SAHPI_ES_MAJOR_FROM_LESS = 4L;
    public const long SAHPI_ES_CRITICAL_FROM_LESS = 8L;
    public const long SAHPI_ES_MINOR_FROM_MORE = 16L;
    public const long SAHPI_ES_MAJOR_FROM_CRITICAL = 32L;
    public const long SAHPI_ES_CRITICAL = 64L;
    public const long SAHPI_ES_MONITOR = 128L;
    public const long SAHPI_ES_INFORMATIONAL = 256L;
    public const long SAHPI_ES_ABSENT = 1L;
    public const long SAHPI_ES_PRESENT = 2L;
    public const long SAHPI_ES_DISABLED = 1L;
    public const long SAHPI_ES_ENABLED = 2L;
    public const long SAHPI_ES_RUNNING = 1L;
    public const long SAHPI_ES_TEST = 2L;
    public const long SAHPI_ES_POWER_OFF = 4L;
    public const long SAHPI_ES_ON_LINE = 8L;
    public const long SAHPI_ES_OFF_LINE = 16L;
    public const long SAHPI_ES_OFF_DUTY = 32L;
    public const long SAHPI_ES_DEGRADED = 64L;
    public const long SAHPI_ES_POWER_SAVE = 128L;
    public const long SAHPI_ES_INSTALL_ERROR = 256L;
    public const long SAHPI_ES_FULLY_REDUNDANT = 1L;
    public const long SAHPI_ES_REDUNDANCY_LOST = 2L;
    public const long SAHPI_ES_REDUNDANCY_DEGRADED = 4L;
    public const long SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES = 8L;
    public const long SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES = 16L;
    public const long SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES = 32L;
    public const long SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL = 64L;
    public const long SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON = 128L;
    public const long SAHPI_ES_STATE_00 = 1L;
    public const long SAHPI_ES_STATE_01 = 2L;
    public const long SAHPI_ES_STATE_02 = 4L;
    public const long SAHPI_ES_STATE_03 = 8L;
    public const long SAHPI_ES_STATE_04 = 16L;
    public const long SAHPI_ES_STATE_05 = 32L;
    public const long SAHPI_ES_STATE_06 = 64L;
    public const long SAHPI_ES_STATE_07 = 128L;
    public const long SAHPI_ES_STATE_08 = 256L;
    public const long SAHPI_ES_STATE_09 = 512L;
    public const long SAHPI_ES_STATE_10 = 1024L;
    public const long SAHPI_ES_STATE_11 = 2048L;
    public const long SAHPI_ES_STATE_12 = 4096L;
    public const long SAHPI_ES_STATE_13 = 8192L;
    public const long SAHPI_ES_STATE_14 = 16384L;
    public const long SAHPI_STANDARD_SENSOR_MIN = 256L;
    public const long SAHPI_STANDARD_SENSOR_MAX = 511L;
    public const long SAHPI_SENSOR_TYPE_SAFHPI_GROUP = 65536L;
    public const int SAHPI_SENSOR_BUFFER_LENGTH = 32;
    public const long SAHPI_ALL_EVENT_STATES = 65535L;
    public const long SAHPI_SRF_MIN = 16L;
    public const long SAHPI_SRF_MAX = 8L;
    public const long SAHPI_SRF_NORMAL_MIN = 4L;
    public const long SAHPI_SRF_NORMAL_MAX = 2L;
    public const long SAHPI_SRF_NOMINAL = 1L;
    public const long SAHPI_STM_LOW_MINOR = 1L;
    public const long SAHPI_STM_LOW_MAJOR = 2L;
    public const long SAHPI_STM_LOW_CRIT = 4L;
    public const long SAHPI_STM_UP_MINOR = 8L;
    public const long SAHPI_STM_UP_MAJOR = 16L;
    public const long SAHPI_STM_UP_CRIT = 32L;
    public const long SAHPI_STM_UP_HYSTERESIS = 64L;
    public const long SAHPI_STM_LOW_HYSTERESIS = 128L;
    public const long SAHPI_DEFAGSENS_OPER = 256L;
    public const long SAHPI_DEFAGSENS_PWR = 257L;
    public const long SAHPI_DEFAGSENS_TEMP = 258L;
    public const long SAHPI_DEFAGSENS_MIN = 256L;
    public const long SAHPI_DEFAGSENS_MAX = 271L;
    public const int SAHPI_CTRL_MAX_STREAM_LENGTH = 4;
    public const long SAHPI_TLN_ALL_LINES = 0L;
    public const int SAHPI_CTRL_MAX_OEM_BODY_LENGTH = 255;
    public const int SAHPI_CTRL_OEM_CONFIG_LENGTH = 10;
    public const long SAHPI_DEFAULT_INVENTORY_ID = 0L;
    public const long SAHPI_DEFAULT_WATCHDOG_NUM = 0L;
    public const long SAHPI_WATCHDOG_EXP_BIOS_FRB2 = 2L;
    public const long SAHPI_WATCHDOG_EXP_BIOS_POST = 4L;
    public const long SAHPI_WATCHDOG_EXP_OS_LOAD = 8L;
    public const long SAHPI_WATCHDOG_EXP_SMS_OS = 16L;
    public const long SAHPI_WATCHDOG_EXP_OEM = 32L;
    public const int SAHPI_DIMITEST_MAX_PARAMETERS = 10;
    public const int SAHPI_DIMITEST_PARAM_NAME_LEN = 20;
    public const long SAHPI_DIMITEST_CAPABILITY_NO_CAPABILITY = 0L;
    public const long SAHPI_DIMITEST_CAPABILITY_RESULTSOUTPUT = 1L;
    public const long SAHPI_DIMITEST_CAPABILITY_SERVICEMODE = 2L;
    public const long SAHPI_DIMITEST_CAPABILITY_LOOPCOUNT = 4L;
    public const long SAHPI_DIMITEST_CAPABILITY_LOOPTIME = 8L;
    public const long SAHPI_DIMITEST_CAPABILITY_LOGGING = 16L;
    public const long SAHPI_DIMITEST_CAPABILITY_TESTCANCEL = 32L;
    public const long SAHPI_DIMITEST_CAPAB_RES_FINALONLY = 0L;
    public const long SAHPI_DIMITEST_CAPAB_RES_ONDEMAND = 1L;
    public const long SAHPI_DIMITEST_CAPAB_RES_ASYNC = 2L;
    public const int SAHPI_DIMITEST_MAX_ENTITIESIMPACTED = 5;
    public const int SAHPI_FUMI_MAX_OEM_BODY_LENGTH = 255;
    public const int SAHPI_FUMI_MAX_ENTITIES_IMPACTED = 5;
    public const long SAHPI_FUMI_NO_MAIN_PERSISTENT_COPY = 1L;
    public const long SAHPI_FUMI_PROT_TFTP = 1L;
    public const long SAHPI_FUMI_PROT_FTP = 2L;
    public const long SAHPI_FUMI_PROT_HTTP = 4L;
    public const long SAHPI_FUMI_PROT_LDAP = 8L;
    public const long SAHPI_FUMI_PROT_LOCAL = 16L;
    public const long SAHPI_FUMI_PROT_NFS = 32L;
    public const long SAHPI_FUMI_PROT_DBACCESS = 64L;
    public const long SAHPI_FUMI_CAP_ROLLBACK = 1L;
    public const long SAHPI_FUMI_CAP_BANKCOPY = 2L;
    public const long SAHPI_FUMI_CAP_BANKREORDER = 4L;
    public const long SAHPI_FUMI_CAP_BACKUP = 8L;
    public const long SAHPI_FUMI_CAP_TARGET_VERIFY = 16L;
    public const long SAHPI_FUMI_CAP_TARGET_VERIFY_MAIN = 32L;
    public const long SAHPI_FUMI_CAP_COMPONENTS = 64L;
    public const long SAHPI_FUMI_CAP_AUTOROLLBACK = 128L;
    public const long SAHPI_FUMI_CAP_AUTOROLLBACK_CAN_BE_DISABLED = 256L;
    public const long SAHPI_FUMI_CAP_MAIN_NOT_PERSISTENT = 512L;
    public const long SAHPI_SOD_TRIGGER_READING = 1L;
    public const long SAHPI_SOD_TRIGGER_THRESHOLD = 2L;
    public const long SAHPI_SOD_OEM = 4L;
    public const long SAHPI_SOD_PREVIOUS_STATE = 8L;
    public const long SAHPI_SOD_CURRENT_STATE = 16L;
    public const long SAHPI_SOD_SENSOR_SPECIFIC = 32L;
    public const long SAHPI_SEOD_CURRENT_STATE = 16L;
    public const long SAHPI_SEOD_ALARM_STATES = 64L;
    public const long SAHPI_EVT_QUEUE_OVERFLOW = 1L;
    public const int SA_HPI_MAX_NAME_LENGTH = 256;
    public const long SAHPI_LOAD_ID_DEFAULT = 0L;
    public const long SAHPI_LOAD_ID_BYNAME = 4294967295L;
    public const int SAHPI_GUID_LENGTH = 16;
    public const long SAHPI_CAPABILITY_RESOURCE = 1073741824L;
    public const long SAHPI_CAPABILITY_FUMI = 65536L;
    public const long SAHPI_CAPABILITY_EVT_DEASSERTS = 32768L;
    public const long SAHPI_CAPABILITY_DIMI = 16384L;
    public const long SAHPI_CAPABILITY_AGGREGATE_STATUS = 8192L;
    public const long SAHPI_CAPABILITY_CONFIGURATION = 4096L;
    public const long SAHPI_CAPABILITY_MANAGED_HOTSWAP = 2048L;
    public const long SAHPI_CAPABILITY_WATCHDOG = 1024L;
    public const long SAHPI_CAPABILITY_CONTROL = 512L;
    public const long SAHPI_CAPABILITY_FRU = 256L;
    public const long SAHPI_CAPABILITY_LOAD_ID = 128L;
    public const long SAHPI_CAPABILITY_ANNUNCIATOR = 64L;
    public const long SAHPI_CAPABILITY_POWER = 32L;
    public const long SAHPI_CAPABILITY_RESET = 16L;
    public const long SAHPI_CAPABILITY_INVENTORY_DATA = 8L;
    public const long SAHPI_CAPABILITY_EVENT_LOG = 4L;
    public const long SAHPI_CAPABILITY_RDR = 2L;
    public const long SAHPI_CAPABILITY_SENSOR = 1L;
    public const long SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY = 2147483648L;
    public const long SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED = 1073741824L;
    public const long SAHPI_HS_CAPABILITY_AUTOINSERT_IMMEDIATE = 536870912L;
    public const long SAHPI_DOMAIN_CAP_AUTOINSERT_READ_ONLY = 1L;
    public const long SAHPI_EVTLOG_CAPABILITY_ENTRY_ADD = 1L;
    public const long SAHPI_EVTLOG_CAPABILITY_CLEAR = 2L;
    public const long SAHPI_EVTLOG_CAPABILITY_TIME_SET = 4L;
    public const long SAHPI_EVTLOG_CAPABILITY_STATE_SET = 8L;
    public const long SAHPI_EVTLOG_CAPABILITY_OVERFLOW_RESET = 16L;
    public const long SAHPI_OLDEST_ENTRY = 0L;
    public const long SAHPI_NEWEST_ENTRY = 4294967295L;
    public const long SAHPI_NO_MORE_ENTRIES = 4294967294L;
    public const long ATCAHPI_PICMG_MID = 12634L;
    public const long ATCAHPI_BLINK_COLOR_LED = 128L;
    public const long ATCAHPI_LED_WHITE = 64L;
    public const long ATCAHPI_LED_ORANGE = 32L;
    public const long ATCAHPI_LED_AMBER = 16L;
    public const long ATCAHPI_LED_GREEN = 8L;
    public const long ATCAHPI_LED_RED = 4L;
    public const long ATCAHPI_LED_BLUE = 2L;
    public const long ATCAHPI_CTRL_NUM_BLUE_LED = 0L;
    public const long ATCAHPI_CTRL_NUM_LED1 = 1L;
    public const long ATCAHPI_CTRL_NUM_LED2 = 2L;
    public const long ATCAHPI_CTRL_NUM_LED3 = 3L;
    public const long ATCAHPI_CTRL_NUM_APP_LED = 4L;
    public const long ATCAHPI_RESOURCE_AGGREGATE_LED = 255L;
    public const long ATCAHPI_ENT_POWER_ENTRY_MODULE_SLOT = 145L;
    public const long ATCAHPI_ENT_SHELF_FRU_DEVICE_SLOT = 146L;
    public const long ATCAHPI_ENT_SHELF_MANAGER_SLOT = 147L;
    public const long ATCAHPI_ENT_FAN_TRAY_SLOT = 148L;
    public const long ATCAHPI_ENT_FAN_FILTER_TRAY_SLOT = 149L;
    public const long ATCAHPI_ENT_ALARM_SLOT = 150L;
    public const long ATCAHPI_ENT_AMC_SLOT = 151L;
    public const long ATCAHPI_ENT_PMC_SLOT = 152L;
    public const long ATCAHPI_ENT_RTM_SLOT = 153L;
    public const long ATCAHPI_ENT_PICMG_FRONT_BLADE = 65558L;
    public const long ATCAHPI_ENT_SHELF_FRU_DEVICE = 65559L;
    public const long ATCAHPI_ENT_FILTRATION_UNIT = 65560L;
    public const long ATCAHPI_ENT_AMC = 65561L;
    public const long ATCAHPI_SENSOR_NUM_SHELF_INFO_VALID = 4096L;
    public const long ATCAHPI_CTRL_NUM_SHELF_ADDRESS = 4096L;
    public const long ATCAHPI_CTRL_NUM_SHELF_IP_ADDRESS = 4097L;
    public const long ATCAHPI_CTRL_NUM_SHELF_STATUS = 4098L;
    public const long ATCAHPI_SENSOR_NUM_PWRONSEQ_COMMIT_STATUS = 4864L;
    public const long ATCAHPI_CTRL_NUM_FRU_POWER_ON_SEQUENCE_COMMIT = 4864L;
    public const long ATCAHPI_CTRL_NUM_FRU_POWER_ON_SEQUENCE = 4865L;
    public const long ATCAHPI_CS_CPS_PWR_ON = 1L;
    public const long ATCAHPI_CS_CPS_PWR_OVERLOAD = 2L;
    public const long ATCAHPI_CS_CPS_INTERLOCK = 4L;
    public const long ATCAHPI_CS_CPS_PWR_FAULT = 8L;
    public const long ATCAHPI_CS_CPS__PWR_CTRL_FAULT = 16L;
    public const long ATCAHPI_CS_CPS_PWR_RESTORE_PWR_UP_PREV = 32L;
    public const long ATCAHPI_CS_CPS_PWR_RESTORE_PWR_UP = 64L;
    public const long ATCAHPI_CS_CPS_PWR_RESTORE_UNKNOWN = 96L;
    public const long ATCAHPI_CS_LPEVT_AC_FAILED = 1L;
    public const long ATCAHPI_CS_LPEVT_PWR_OVERLOAD = 2L;
    public const long ATCAHPI_CS_LPEVT_PWR_INTERLOCK = 4L;
    public const long ATCAHPI_CS_LPEVT_PWR_FAULT = 8L;
    public const long ATCAHPI_CS_LPEVT_PWRON_IPMI = 16L;
    public const long ATCAHPI_CS_MISC_CS_INTRUSION_ACTIVE = 1L;
    public const long ATCAHPI_CS_MISC_CS_FP_LOCKOUT_ACTIVE = 2L;
    public const long ATCAHPI_CS_MISC_CS_DRIVE_FAULT = 4L;
    public const long ATCAHPI_CS_MISC_CS_COOLING_FAULT = 8L;
    public const long ATCAHPI_CS_FP_BUTTON_PWR_OFF = 1L;
    public const long ATCAHPI_CS_FP_BUTTON_RESET_OFF = 2L;
    public const long ATCAHPI_CS_FP_BUTTON_DIAGINTR_OFF = 4L;
    public const long ATCAHPI_CS_FP_BUTTON_STANDBY_OFF = 8L;
    public const long ATCAHPI_CS_FP_BUTTON_ALLOW_PWR_OFF = 16L;
    public const long ATCAHPI_CS_FP_BUTTON_ALLOW_RESET_OFF = 32L;
    public const long ATCAHPI_CS_FP_BUTTON_ALLOW_DIAGINTR_OFF = 64L;
    public const long ATCAHPI_CS_FP_BUTTON_ALLOW_STANDBY_OFF = 128L;
    public const long ATCAHPI_SENSOR_NUM_SHMGR_REDUNDANCY = 4097L;
    public const long ATCAHPI_SENSOR_NUM_SHMGR_ACTIVE = 4098L;
    public const long ATCAHPI_SENSOR_NUM_SHMGR_STANDBY = 4099L;
    public const long ATCAHPI_CTRL_NUM_SHMGR_FAILOVER = 4112L;
    public const long ATCAHPI_CTRL_NUM_FAILED_RESOURCE_EXTRACT = 4126L;
    public const long ATCAHPI_SENSOR_NUM_SLOT_STATE = 4112L;
    public const long ATCAHPI_SENSOR_NUM_ASSIGNED_PWR = 4113L;
    public const long ATCAHPI_SENSOR_NUM_MAX_PWR = 4114L;
    public const long ATCAHPI_CTRL_NUM_FRU_ACTIVATION = 4128L;
    public const long ATCAHPI_SENSOR_NUM_IPMB0 = 4352L;
    public const long ATCAHPI_CTRL_NUM_DESIRED_PWR = 4144L;
    public const long ATCAHPI_CTRL_NUM_IPMB_A_STATE = 4353L;
    public const long ATCAHPI_CTRL_NUM_IPMB_B_STATE = 4354L;
    public const long ATCAHPI_CTRL_NUM_FRU_CONTROL = 4608L;
    public const long ATCAHPI_CTRL_NUM_FRU_IPMC_RESET = 4609L;
    public const long ATCAHPI_SENSOR_NUM_AMC_PWRONSEQ_COMMIT_STATUS = 5376L;
    public const long ATCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE_COMMIT = 5376L;
    public const long ATCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE = 5377L;
    public const long ATCAHPI_CTRL_NUM_FAN_SPEED = 5120L;
    public const long ATCAHPI_PICMG_CT_CHASSIS_STATUS = 16789850L;
    public const long ATCAHPI_PICMG_CT_ATCA_LED = 33567066L;
    public const long XTCAHPI_SPEC_VERSION = 4228711L;
    public const long XTCAHPI_PICMG_MID = 12634L;
    public const long XTCAHPI_ENT_POWER_SLOT = 145L;
    public const long XTCAHPI_ENT_SHELF_FRU_DEVICE_SLOT = 146L;
    public const long XTCAHPI_ENT_SHELF_MANAGER_SLOT = 147L;
    public const long XTCAHPI_ENT_FAN_TRAY_SLOT = 148L;
    public const long XTCAHPI_ENT_FAN_FILTER_TRAY_SLOT = 149L;
    public const long XTCAHPI_ENT_ALARM_SLOT = 150L;
    public const long XTCAHPI_ENT_AMC_SLOT = 151L;
    public const long XTCAHPI_ENT_PMC_SLOT = 152L;
    public const long XTCAHPI_ENT_RTM_SLOT = 153L;
    public const long XTCAHPI_ENT_CARRIER_MANAGER_SLOT = 154L;
    public const long XTCAHPI_ENT_CARRIER_SLOT = 155L;
    public const long XTCAHPI_ENT_COM_E_SLOT = 156L;
    public const long XTCAHPI_CTRL_NUM_IP_ADDRESS_0 = 4097L;
    public const long XTCAHPI_CTRL_NUM_IP_ADDRESS_1 = 4099L;
    public const long XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE = 4865L;
    public const long XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE_COMMIT = 4864L;
    public const long XTCAHPI_SENSOR_NUM_PWRONSEQ_COMMIT_STATUS = 4864L;
    public const long XTCAHPI_ANN_NUM_TELCO_ALARM = 4096L;
    public const long XTCAHPI_SENSOR_NUM_REDUNDANCY = 4097L;
    public const long XTCAHPI_SENSOR_NUM_ACTIVE = 4098L;
    public const long XTCAHPI_SENSOR_NUM_STANDBY = 4099L;
    public const long XTCAHPI_CTRL_NUM_FAILOVER = 4112L;
    public const long XTCAHPI_CTRL_NUM_ACTIVATION = 4128L;
    public const long XTCAHPI_CTRL_NUM_DEACTIVATION = 4129L;
    public const long XTCAHPI_SENSOR_NUM_FRU_INFO_VALID = 4096L;
    public const long XTCAHPI_SENSOR_NUM_ASSIGNED_PWR = 4113L;
    public const long XTCAHPI_SENSOR_NUM_SLOT_ASSIGNED_PWR = 6144L;
    public const long XTCAHPI_IDR_NUM_CONFIG_INFO = 1L;
    public const long XTCAHPI_CTRL_NUM_SHELF_ADDRESS = 4096L;
    public const long XTCAHPI_CTRL_NUM_SHELF_STATUS = 4098L;
    public const long XTCAHPI_CTRL_NUM_SHELF_MANAGER_RMCP_USERNAME = 4177L;
    public const long XTCAHPI_CTRL_NUM_SHELF_MANAGER_RMCP_PASSWORD = 4178L;
    public const long XTCAHPI_CTRL_NUM_IN_SHELF_ACTIVATION = 4192L;
    public const long XTCAHPI_CTRL_NUM_IN_SHELF_DEACTIVATION = 4193L;
    public const long XTCAHPI_CTRL_NUM_DESIRED_PWR = 4144L;
    public const long XTCAHPI_SENSOR_NUM_IPMB0 = 4352L;
    public const long XTCAHPI_CTRL_NUM_IPMB_A_STATE = 4353L;
    public const long XTCAHPI_CTRL_NUM_IPMB_B_STATE = 4354L;
    public const long XTCAHPI_CTRL_NUM_FRU_CONTROL = 4608L;
    public const long XTCAHPI_CTRL_NUM_FRU_IPMC_RESET = 4609L;
    public const long XTCAHPI_CTRL_NUM_BLUE_LED = 0L;
    public const long XTCAHPI_CTRL_NUM_LED1 = 1L;
    public const long XTCAHPI_CTRL_NUM_LED2 = 2L;
    public const long XTCAHPI_CTRL_NUM_LED3 = 3L;
    public const long XTCAHPI_CTRL_NUM_APP_LED = 4L;
    public const long XTCAHPI_RESOURCE_AGGREGATE_LED = 255L;
    public const long XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE = 5377L;
    public const long XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE_COMMIT = 5376L;
    public const long XTCAHPI_SENSOR_NUM_AMC_PWRONSEQ_COMMIT_STATUS = 5376L;
    public const long XTCAHPI_CTRL_NUM_FAN_SPEED = 5120L;
    public const long XTCAHPI_SENSOR_NUM_HPM1_IPMC_GLOBAL_CAPS = 5888L;
    public const long XTCAHPI_SENSOR_NUM_HPM1_IMAGE_CAPS = 5889L;
    public const long XTCAHPI_SENSOR_NUM_HPM1_ROLLBACK_TIMEOUT = 5890L;
    public const long XTCAHPI_CTRL_NUM_CRITICAL_TELCO_ALARM = 5632L;
    public const long XTCAHPI_CTRL_NUM_MAJOR_TELCO_ALARM = 5633L;
    public const long XTCAHPI_CTRL_NUM_MINOR_TELCO_ALARM = 5634L;
    public const long XTCAHPI_CTRL_NUM_POWER_TELCO_ALARM = 5635L;
    public const long XTCAHPI_CTRL_NUM_TELCO_ALARM_CUTOFF = 5636L;
    public const long XTCAHPI_SENSOR_NUM_TELCO_ALARM_INPUT = 5632L;
    public const long XTCAHPI_INDICATOR_LOC_MINOR_ALARM = 0L;
    public const long XTCAHPI_INDICATOR_LOC_MAJOR_ALARM = 1L;
    public const long XTCAHPI_INDICATOR_LOC_CRITICAL_ALARM = 2L;
    public const long XTCAHPI_INDICATOR_LOC_POWER_ALARM = 3L;
    public const long XTCAHPI_CONFIG_DATA_AREA_SPEC_VERSION = 0L;
    public const long XTCAHPI_CONFIG_DATA_FIELD_LABEL = 0L;
    public const long XTCAHPI_CS_CPS_PWR_ON = 1L;
    public const long XTCAHPI_CS_CPS_PWR_OVERLOAD = 2L;
    public const long XTCAHPI_CS_CPS_INTERLOCK = 4L;
    public const long XTCAHPI_CS_CPS_PWR_FAULT = 8L;
    public const long XTCAHPI_CS_CPS_PWR_CTRL_FAULT = 16L;
    public const long XTCAHPI_CS_CPS_PWR_RESTORE_PWR_UP_PREV = 32L;
    public const long XTCAHPI_CS_CPS_PWR_RESTORE_PWR_UP = 64L;
    public const long XTCAHPI_CS_CPS_PWR_RESTORE_UNKNOWN = 96L;
    public const long XTCAHPI_CS_LPEVT_AC_FAILED = 1L;
    public const long XTCAHPI_CS_LPEVT_PWR_OVERLOAD = 2L;
    public const long XTCAHPI_CS_LPEVT_PWR_INTERLOCK = 4L;
    public const long XTCAHPI_CS_LPEVT_PWR_FAULT = 8L;
    public const long XTCAHPI_CS_LPEVT_PWRON_IPMI = 16L;
    public const long XTCAHPI_CS_MISC_CS_INTRUSION_ACTIVE = 1L;
    public const long XTCAHPI_CS_MISC_CS_FP_LOCKOUT_ACTIVE = 2L;
    public const long XTCAHPI_CS_MISC_CS_DRIVE_FAULT = 4L;
    public const long XTCAHPI_CS_MISC_CS_COOLING_FAULT = 8L;
    public const long XTCAHPI_CS_FP_BUTTON_PWR_OFF = 1L;
    public const long XTCAHPI_CS_FP_BUTTON_RESET_OFF = 2L;
    public const long XTCAHPI_CS_FP_BUTTON_DIAGINTR_OFF = 4L;
    public const long XTCAHPI_CS_FP_BUTTON_STANDBY_OFF = 8L;
    public const long XTCAHPI_CS_FP_BUTTON_ALLOW_PWR_OFF = 16L;
    public const long XTCAHPI_CS_FP_BUTTON_ALLOW_RESET_OFF = 32L;
    public const long XTCAHPI_CS_FP_BUTTON_ALLOW_DIAGINTR_OFF = 64L;
    public const long XTCAHPI_CS_FP_BUTTON_ALLOW_STANDBY_OFF = 128L;
    public const long XTCAHPI_BLINK_COLOR_LED = 128L;
    public const long XTCAHPI_LED_WHITE = 64L;
    public const long XTCAHPI_LED_ORANGE = 32L;
    public const long XTCAHPI_LED_AMBER = 16L;
    public const long XTCAHPI_LED_GREEN = 8L;
    public const long XTCAHPI_LED_RED = 4L;
    public const long XTCAHPI_LED_BLUE = 2L;
    public const long XTCAHPI_IF_FABRIC = 1L;
    public const long XTCAHPI_IF_SYNC_CLOCK = 2L;
    public const long XTCAHPI_IF_BASE = 3L;
    public const long XTCAHPI_IF_UPDATE_CHANNEL = 4L;
    public const long XTCAHPI_IF_METALLIC_TEST = 5L;
    public const long XTCAHPI_IF_RINGING_GENERATOR_BUS = 6L;
    public const long XTCAHPI_CONFIG_DATA_LOC_DEFAULT = 0L;
    public const long XTCAHPI_CONFIG_DATA_LOC_SHELF_ADDRESS = 1L;
    public const long XTCAHPI_CONFIG_DATA_LOC_POWER_ON_SEQUENCE = 2L;
    public const long XTCAHPI_CONFIG_DATA_LOC_CHASSIS_STATUS = 3L;
    public const long XTCAHPI_CONFIG_DATA_LOC_ACTIVATION = 4L;
    public const long XTCAHPI_CONFIG_DATA_LOC_DEACTIVATION = 5L;
    public const long XTCAHPI_CONFIG_DATA_LOC_IN_SHELF_ACTIVATION = 6L;
    public const long XTCAHPI_CONFIG_DATA_LOC_IN_SHELF_DEACTIVATION = 7L;
    public const long XTCAHPI_CONFIG_DATA_LOC_USERNAME = 8L;
    public const long XTCAHPI_CONFIG_DATA_LOC_PASSWORD = 9L;
    public const long XTCAHPI_CONFIG_DATA_LOC_FUMI_GLOBAL_UPGRADE_CAP = 10L;
    public const long XTCAHPI_CONFIG_DATA_LOC_FUMI_UPGRADE_IMAGE_CAP = 11L;
    public const long XTCAHPI_CONFIG_DATA_LOC_FUMI_ROLLBACK_TIMEOUT = 12L;
    public const long SAHPI_LANG_UNDEF = 0L;
    public const long SAHPI_LANG_AFAR = 1L;
    public const long SAHPI_LANG_ABKHAZIAN = 2L;
    public const long SAHPI_LANG_AFRIKAANS = 3L;
    public const long SAHPI_LANG_AMHARIC = 4L;
    public const long SAHPI_LANG_ARABIC = 5L;
    public const long SAHPI_LANG_ASSAMESE = 6L;
    public const long SAHPI_LANG_AYMARA = 7L;
    public const long SAHPI_LANG_AZERBAIJANI = 8L;
    public const long SAHPI_LANG_BASHKIR = 9L;
    public const long SAHPI_LANG_BYELORUSSIAN = 10L;
    public const long SAHPI_LANG_BULGARIAN = 11L;
    public const long SAHPI_LANG_BIHARI = 12L;
    public const long SAHPI_LANG_BISLAMA = 13L;
    public const long SAHPI_LANG_BENGALI = 14L;
    public const long SAHPI_LANG_TIBETAN = 15L;
    public const long SAHPI_LANG_BRETON = 16L;
    public const long SAHPI_LANG_CATALAN = 17L;
    public const long SAHPI_LANG_CORSICAN = 18L;
    public const long SAHPI_LANG_CZECH = 19L;
    public const long SAHPI_LANG_WELSH = 20L;
    public const long SAHPI_LANG_DANISH = 21L;
    public const long SAHPI_LANG_GERMAN = 22L;
    public const long SAHPI_LANG_BHUTANI = 23L;
    public const long SAHPI_LANG_GREEK = 24L;
    public const long SAHPI_LANG_ENGLISH = 25L;
    public const long SAHPI_LANG_ESPERANTO = 26L;
    public const long SAHPI_LANG_SPANISH = 27L;
    public const long SAHPI_LANG_ESTONIAN = 28L;
    public const long SAHPI_LANG_BASQUE = 29L;
    public const long SAHPI_LANG_PERSIAN = 30L;
    public const long SAHPI_LANG_FINNISH = 31L;
    public const long SAHPI_LANG_FIJI = 32L;
    public const long SAHPI_LANG_FAEROESE = 33L;
    public const long SAHPI_LANG_FRENCH = 34L;
    public const long SAHPI_LANG_FRISIAN = 35L;
    public const long SAHPI_LANG_IRISH = 36L;
    public const long SAHPI_LANG_SCOTSGAELIC = 37L;
    public const long SAHPI_LANG_GALICIAN = 38L;
    public const long SAHPI_LANG_GUARANI = 39L;
    public const long SAHPI_LANG_GUJARATI = 40L;
    public const long SAHPI_LANG_HAUSA = 41L;
    public const long SAHPI_LANG_HINDI = 42L;
    public const long SAHPI_LANG_CROATIAN = 43L;
    public const long SAHPI_LANG_HUNGARIAN = 44L;
    public const long SAHPI_LANG_ARMENIAN = 45L;
    public const long SAHPI_LANG_INTERLINGUA = 46L;
    public const long SAHPI_LANG_INTERLINGUE = 47L;
    public const long SAHPI_LANG_INUPIAK = 48L;
    public const long SAHPI_LANG_INDONESIAN = 49L;
    public const long SAHPI_LANG_ICELANDIC = 50L;
    public const long SAHPI_LANG_ITALIAN = 51L;
    public const long SAHPI_LANG_HEBREW = 52L;
    public const long SAHPI_LANG_JAPANESE = 53L;
    public const long SAHPI_LANG_YIDDISH = 54L;
    public const long SAHPI_LANG_JAVANESE = 55L;
    public const long SAHPI_LANG_GEORGIAN = 56L;
    public const long SAHPI_LANG_KAZAKH = 57L;
    public const long SAHPI_LANG_GREENLANDIC = 58L;
    public const long SAHPI_LANG_CAMBODIAN = 59L;
    public const long SAHPI_LANG_KANNADA = 60L;
    public const long SAHPI_LANG_KOREAN = 61L;
    public const long SAHPI_LANG_KASHMIRI = 62L;
    public const long SAHPI_LANG_KURDISH = 63L;
    public const long SAHPI_LANG_KIRGHIZ = 64L;
    public const long SAHPI_LANG_LATIN = 65L;
    public const long SAHPI_LANG_LINGALA = 66L;
    public const long SAHPI_LANG_LAOTHIAN = 67L;
    public const long SAHPI_LANG_LITHUANIAN = 68L;
    public const long SAHPI_LANG_LATVIANLETTISH = 69L;
    public const long SAHPI_LANG_MALAGASY = 70L;
    public const long SAHPI_LANG_MAORI = 71L;
    public const long SAHPI_LANG_MACEDONIAN = 72L;
    public const long SAHPI_LANG_MALAYALAM = 73L;
    public const long SAHPI_LANG_MONGOLIAN = 74L;
    public const long SAHPI_LANG_MOLDAVIAN = 75L;
    public const long SAHPI_LANG_MARATHI = 76L;
    public const long SAHPI_LANG_MALAY = 77L;
    public const long SAHPI_LANG_MALTESE = 78L;
    public const long SAHPI_LANG_BURMESE = 79L;
    public const long SAHPI_LANG_NAURU = 80L;
    public const long SAHPI_LANG_NEPALI = 81L;
    public const long SAHPI_LANG_DUTCH = 82L;
    public const long SAHPI_LANG_NORWEGIAN = 83L;
    public const long SAHPI_LANG_OCCITAN = 84L;
    public const long SAHPI_LANG_AFANOROMO = 85L;
    public const long SAHPI_LANG_ORIYA = 86L;
    public const long SAHPI_LANG_PUNJABI = 87L;
    public const long SAHPI_LANG_POLISH = 88L;
    public const long SAHPI_LANG_PASHTOPUSHTO = 89L;
    public const long SAHPI_LANG_PORTUGUESE = 90L;
    public const long SAHPI_LANG_QUECHUA = 91L;
    public const long SAHPI_LANG_RHAETOROMANCE = 92L;
    public const long SAHPI_LANG_KIRUNDI = 93L;
    public const long SAHPI_LANG_ROMANIAN = 94L;
    public const long SAHPI_LANG_RUSSIAN = 95L;
    public const long SAHPI_LANG_KINYARWANDA = 96L;
    public const long SAHPI_LANG_SANSKRIT = 97L;
    public const long SAHPI_LANG_SINDHI = 98L;
    public const long SAHPI_LANG_SANGRO = 99L;
    public const long SAHPI_LANG_SERBOCROATIAN = 100L;
    public const long SAHPI_LANG_SINGHALESE = 101L;
    public const long SAHPI_LANG_SLOVAK = 102L;
    public const long SAHPI_LANG_SLOVENIAN = 103L;
    public const long SAHPI_LANG_SAMOAN = 104L;
    public const long SAHPI_LANG_SHONA = 105L;
    public const long SAHPI_LANG_SOMALI = 106L;
    public const long SAHPI_LANG_ALBANIAN = 107L;
    public const long SAHPI_LANG_SERBIAN = 108L;
    public const long SAHPI_LANG_SISWATI = 109L;
    public const long SAHPI_LANG_SESOTHO = 110L;
    public const long SAHPI_LANG_SUDANESE = 111L;
    public const long SAHPI_LANG_SWEDISH = 112L;
    public const long SAHPI_LANG_SWAHILI = 113L;
    public const long SAHPI_LANG_TAMIL = 114L;
    public const long SAHPI_LANG_TELUGU = 115L;
    public const long SAHPI_LANG_TAJIK = 116L;
    public const long SAHPI_LANG_THAI = 117L;
    public const long SAHPI_LANG_TIGRINYA = 118L;
    public const long SAHPI_LANG_TURKMEN = 119L;
    public const long SAHPI_LANG_TAGALOG = 120L;
    public const long SAHPI_LANG_SETSWANA = 121L;
    public const long SAHPI_LANG_TONGA = 122L;
    public const long SAHPI_LANG_TURKISH = 123L;
    public const long SAHPI_LANG_TSONGA = 124L;
    public const long SAHPI_LANG_TATAR = 125L;
    public const long SAHPI_LANG_TWI = 126L;
    public const long SAHPI_LANG_UKRAINIAN = 127L;
    public const long SAHPI_LANG_URDU = 128L;
    public const long SAHPI_LANG_UZBEK = 129L;
    public const long SAHPI_LANG_VIETNAMESE = 130L;
    public const long SAHPI_LANG_VOLAPUK = 131L;
    public const long SAHPI_LANG_WOLOF = 132L;
    public const long SAHPI_LANG_XHOSA = 133L;
    public const long SAHPI_LANG_YORUBA = 134L;
    public const long SAHPI_LANG_CHINESE = 135L;
    public const long SAHPI_LANG_ZULU = 136L;
    public const long SAHPI_LANG_MAX_VALID = 136L;
    public const long SAHPI_TL_TYPE_UNICODE = 0L;
    public const long SAHPI_TL_TYPE_BCDPLUS = 1L;
    public const long SAHPI_TL_TYPE_ASCII6 = 2L;
    public const long SAHPI_TL_TYPE_TEXT = 3L;
    public const long SAHPI_TL_TYPE_BINARY = 4L;
    public const long SAHPI_TL_TYPE_MAX_VALID = 4L;
    public const long SAHPI_ENT_UNSPECIFIED = 0L;
    public const long SAHPI_ENT_OTHER = 1L;
    public const long SAHPI_ENT_UNKNOWN = 2L;
    public const long SAHPI_ENT_PROCESSOR = 3L;
    public const long SAHPI_ENT_DISK_BAY = 4L;
    public const long SAHPI_ENT_PERIPHERAL_BAY = 5L;
    public const long SAHPI_ENT_SYS_MGMNT_MODULE = 6L;
    public const long SAHPI_ENT_SYSTEM_BOARD = 7L;
    public const long SAHPI_ENT_MEMORY_MODULE = 8L;
    public const long SAHPI_ENT_PROCESSOR_MODULE = 9L;
    public const long SAHPI_ENT_POWER_SUPPLY = 10L;
    public const long SAHPI_ENT_ADD_IN_CARD = 11L;
    public const long SAHPI_ENT_FRONT_PANEL_BOARD = 12L;
    public const long SAHPI_ENT_BACK_PANEL_BOARD = 13L;
    public const long SAHPI_ENT_POWER_SYSTEM_BOARD = 14L;
    public const long SAHPI_ENT_DRIVE_BACKPLANE = 15L;
    public const long SAHPI_ENT_SYS_EXPANSION_BOARD = 16L;
    public const long SAHPI_ENT_OTHER_SYSTEM_BOARD = 17L;
    public const long SAHPI_ENT_PROCESSOR_BOARD = 18L;
    public const long SAHPI_ENT_POWER_UNIT = 19L;
    public const long SAHPI_ENT_POWER_MODULE = 20L;
    public const long SAHPI_ENT_POWER_MGMNT = 21L;
    public const long SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD = 22L;
    public const long SAHPI_ENT_SYSTEM_CHASSIS = 23L;
    public const long SAHPI_ENT_SUB_CHASSIS = 24L;
    public const long SAHPI_ENT_OTHER_CHASSIS_BOARD = 25L;
    public const long SAHPI_ENT_DISK_DRIVE_BAY = 26L;
    public const long SAHPI_ENT_PERIPHERAL_BAY_2 = 27L;
    public const long SAHPI_ENT_DEVICE_BAY = 28L;
    public const long SAHPI_ENT_COOLING_DEVICE = 29L;
    public const long SAHPI_ENT_COOLING_UNIT = 30L;
    public const long SAHPI_ENT_INTERCONNECT = 31L;
    public const long SAHPI_ENT_MEMORY_DEVICE = 32L;
    public const long SAHPI_ENT_SYS_MGMNT_SOFTWARE = 33L;
    public const long SAHPI_ENT_BIOS = 34L;
    public const long SAHPI_ENT_OPERATING_SYSTEM = 35L;
    public const long SAHPI_ENT_SYSTEM_BUS = 36L;
    public const long SAHPI_ENT_GROUP = 37L;
    public const long SAHPI_ENT_REMOTE = 38L;
    public const long SAHPI_ENT_EXTERNAL_ENVIRONMENT = 39L;
    public const long SAHPI_ENT_BATTERY = 40L;
    public const long SAHPI_ENT_PROCESSING_BLADE = 41L;
    public const long SAHPI_ENT_CONNECTIVITY_SWITCH = 42L;
    public const long SAHPI_ENT_PROCESSOR_MEMORY_MODULE = 43L;
    public const long SAHPI_ENT_IO_MODULE = 44L;
    public const long SAHPI_ENT_PROCESSOR_IO_MODULE = 45L;
    public const long SAHPI_ENT_MC_FIRMWARE = 46L;
    public const long SAHPI_ENT_IPMI_CHANNEL = 47L;
    public const long SAHPI_ENT_PCI_BUS = 48L;
    public const long SAHPI_ENT_PCI_EXPRESS_BUS = 49L;
    public const long SAHPI_ENT_SCSI_BUS = 50L;
    public const long SAHPI_ENT_SATA_BUS = 51L;
    public const long SAHPI_ENT_PROC_FSB = 52L;
    public const long SAHPI_ENT_CLOCK = 53L;
    public const long SAHPI_ENT_SYSTEM_FIRMWARE = 54L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC = 144L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC01 = 145L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC02 = 146L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC03 = 147L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC04 = 148L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC05 = 149L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC06 = 150L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC07 = 151L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC08 = 152L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC09 = 153L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC10 = 154L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC11 = 155L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC12 = 156L;
    public const long SAHPI_ENT_CHASSIS_SPECIFIC13 = 157L;
    public const long SAHPI_ENT_BOARD_SET_SPECIFIC = 176L;
    public const long SAHPI_ENT_OEM_SYSINT_SPECIFIC = 208L;
    public const long SAHPI_ENT_ROOT = 65535L;
    public const long SAHPI_ENT_RACK = 65536L;
    public const long SAHPI_ENT_SUBRACK = 65537L;
    public const long SAHPI_ENT_COMPACTPCI_CHASSIS = 65538L;
    public const long SAHPI_ENT_ADVANCEDTCA_CHASSIS = 65539L;
    public const long SAHPI_ENT_RACK_MOUNTED_SERVER = 65540L;
    public const long SAHPI_ENT_SYSTEM_BLADE = 65541L;
    public const long SAHPI_ENT_SWITCH = 65542L;
    public const long SAHPI_ENT_SWITCH_BLADE = 65543L;
    public const long SAHPI_ENT_SBC_BLADE = 65544L;
    public const long SAHPI_ENT_IO_BLADE = 65545L;
    public const long SAHPI_ENT_DISK_BLADE = 65546L;
    public const long SAHPI_ENT_DISK_DRIVE = 65547L;
    public const long SAHPI_ENT_FAN = 65548L;
    public const long SAHPI_ENT_POWER_DISTRIBUTION_UNIT = 65549L;
    public const long SAHPI_ENT_SPEC_PROC_BLADE = 65550L;
    public const long SAHPI_ENT_IO_SUBBOARD = 65551L;
    public const long SAHPI_ENT_SBC_SUBBOARD = 65552L;
    public const long SAHPI_ENT_ALARM_MANAGER = 65553L;
    public const long SAHPI_ENT_SHELF_MANAGER = 65554L;
    public const long SAHPI_ENT_DISPLAY_PANEL = 65555L;
    public const long SAHPI_ENT_SUBBOARD_CARRIER_BLADE = 65556L;
    public const long SAHPI_ENT_PHYSICAL_SLOT = 65557L;
    public const long SAHPI_ENT_PICMG_FRONT_BLADE = 65558L;
    public const long SAHPI_ENT_SYSTEM_INVENTORY_DEVICE = 65559L;
    public const long SAHPI_ENT_FILTRATION_UNIT = 65560L;
    public const long SAHPI_ENT_AMC = 65561L;
    public const long SAHPI_ENT_BMC = 65584L;
    public const long SAHPI_ENT_IPMC = 65585L;
    public const long SAHPI_ENT_MMC = 65586L;
    public const long SAHPI_ENT_SHMC = 65587L;
    public const long SAHPI_ENT_CPLD = 65588L;
    public const long SAHPI_ENT_EPLD = 65589L;
    public const long SAHPI_ENT_FPGA = 65590L;
    public const long SAHPI_ENT_DASD = 65591L;
    public const long SAHPI_ENT_NIC = 65592L;
    public const long SAHPI_ENT_DSP = 65593L;
    public const long SAHPI_ENT_UCODE = 65594L;
    public const long SAHPI_ENT_NPU = 65595L;
    public const long SAHPI_ENT_OEM = 65596L;
    public const long SAHPI_ENT_INTERFACE = 65597L;
    public const long SAHPI_ENT_MICROTCA_CHASSIS = 65598L;
    public const long SAHPI_ENT_CARRIER = 65599L;
    public const long SAHPI_ENT_CARRIER_MANAGER = 65600L;
    public const long SAHPI_ENT_CONFIG_DATA = 65601L;
    public const long SAHPI_ENT_INDICATOR = 65602L;
    public const long SAHPI_ENT_MAX_VALID = 65602L;
    public const long SAHPI_TEMPERATURE = 1L;
    public const long SAHPI_VOLTAGE = 2L;
    public const long SAHPI_CURRENT = 3L;
    public const long SAHPI_FAN = 4L;
    public const long SAHPI_PHYSICAL_SECURITY = 5L;
    public const long SAHPI_PLATFORM_VIOLATION = 6L;
    public const long SAHPI_PROCESSOR = 7L;
    public const long SAHPI_POWER_SUPPLY = 8L;
    public const long SAHPI_POWER_UNIT = 9L;
    public const long SAHPI_COOLING_DEVICE = 10L;
    public const long SAHPI_OTHER_UNITS_BASED_SENSOR = 11L;
    public const long SAHPI_MEMORY = 12L;
    public const long SAHPI_DRIVE_SLOT = 13L;
    public const long SAHPI_POST_MEMORY_RESIZE = 14L;
    public const long SAHPI_SYSTEM_FW_PROGRESS = 15L;
    public const long SAHPI_EVENT_LOGGING_DISABLED = 16L;
    public const long SAHPI_RESERVED1 = 17L;
    public const long SAHPI_SYSTEM_EVENT = 18L;
    public const long SAHPI_CRITICAL_INTERRUPT = 19L;
    public const long SAHPI_BUTTON = 20L;
    public const long SAHPI_MODULE_BOARD = 21L;
    public const long SAHPI_MICROCONTROLLER_COPROCESSOR = 22L;
    public const long SAHPI_ADDIN_CARD = 23L;
    public const long SAHPI_CHASSIS = 24L;
    public const long SAHPI_CHIP_SET = 25L;
    public const long SAHPI_OTHER_FRU = 26L;
    public const long SAHPI_CABLE_INTERCONNECT = 27L;
    public const long SAHPI_TERMINATOR = 28L;
    public const long SAHPI_SYSTEM_BOOT_INITIATED = 29L;
    public const long SAHPI_BOOT_ERROR = 30L;
    public const long SAHPI_OS_BOOT = 31L;
    public const long SAHPI_OS_CRITICAL_STOP = 32L;
    public const long SAHPI_SLOT_CONNECTOR = 33L;
    public const long SAHPI_SYSTEM_ACPI_POWER_STATE = 34L;
    public const long SAHPI_RESERVED2 = 35L;
    public const long SAHPI_PLATFORM_ALERT = 36L;
    public const long SAHPI_ENTITY_PRESENCE = 37L;
    public const long SAHPI_MONITOR_ASIC_IC = 38L;
    public const long SAHPI_LAN = 39L;
    public const long SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH = 40L;
    public const long SAHPI_BATTERY = 41L;
    public const long SAHPI_SESSION_AUDIT = 42L;
    public const long SAHPI_VERSION_CHANGE = 43L;
    public const long SAHPI_OPERATIONAL = 160L;
    public const long SAHPI_OEM_SENSOR = 192L;
    public const long SAHPI_COMM_CHANNEL_LINK_STATE = 65537L;
    public const long SAHPI_MANAGEMENT_BUS_STATE = 65538L;
    public const long SAHPI_COMM_CHANNEL_BUS_STATE = 65539L;
    public const long SAHPI_CONFIG_DATA = 65540L;
    public const long SAHPI_POWER_BUDGET = 65541L;
    public const long SAHPI_SENSOR_TYPE_MAX_VALID = 65541L;
    public const long SAHPI_SENSOR_READING_TYPE_INT64 = 0L;
    public const long SAHPI_SENSOR_READING_TYPE_UINT64 = 1L;
    public const long SAHPI_SENSOR_READING_TYPE_FLOAT64 = 2L;
    public const long SAHPI_SENSOR_READING_TYPE_BUFFER = 3L;
    public const long SAHPI_SENSOR_READING_TYPE_MAX_VALID = 3L;
    public const long SAHPI_SENS_ADD_EVENTS_TO_MASKS = 0L;
    public const long SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS = 1L;
    public const long SAHPI_SENS_EVENT_MASK_ACTION_MAX_VALID = 1L;
    public const long SAHPI_SU_UNSPECIFIED = 0L;
    public const long SAHPI_SU_DEGREES_C = 1L;
    public const long SAHPI_SU_DEGREES_F = 2L;
    public const long SAHPI_SU_DEGREES_K = 3L;
    public const long SAHPI_SU_VOLTS = 4L;
    public const long SAHPI_SU_AMPS = 5L;
    public const long SAHPI_SU_WATTS = 6L;
    public const long SAHPI_SU_JOULES = 7L;
    public const long SAHPI_SU_COULOMBS = 8L;
    public const long SAHPI_SU_VA = 9L;
    public const long SAHPI_SU_NITS = 10L;
    public const long SAHPI_SU_LUMEN = 11L;
    public const long SAHPI_SU_LUX = 12L;
    public const long SAHPI_SU_CANDELA = 13L;
    public const long SAHPI_SU_KPA = 14L;
    public const long SAHPI_SU_PSI = 15L;
    public const long SAHPI_SU_NEWTON = 16L;
    public const long SAHPI_SU_CFM = 17L;
    public const long SAHPI_SU_RPM = 18L;
    public const long SAHPI_SU_HZ = 19L;
    public const long SAHPI_SU_MICROSECOND = 20L;
    public const long SAHPI_SU_MILLISECOND = 21L;
    public const long SAHPI_SU_SECOND = 22L;
    public const long SAHPI_SU_MINUTE = 23L;
    public const long SAHPI_SU_HOUR = 24L;
    public const long SAHPI_SU_DAY = 25L;
    public const long SAHPI_SU_WEEK = 26L;
    public const long SAHPI_SU_MIL = 27L;
    public const long SAHPI_SU_INCHES = 28L;
    public const long SAHPI_SU_FEET = 29L;
    public const long SAHPI_SU_CU_IN = 30L;
    public const long SAHPI_SU_CU_FEET = 31L;
    public const long SAHPI_SU_MM = 32L;
    public const long SAHPI_SU_CM = 33L;
    public const long SAHPI_SU_M = 34L;
    public const long SAHPI_SU_CU_CM = 35L;
    public const long SAHPI_SU_CU_M = 36L;
    public const long SAHPI_SU_LITERS = 37L;
    public const long SAHPI_SU_FLUID_OUNCE = 38L;
    public const long SAHPI_SU_RADIANS = 39L;
    public const long SAHPI_SU_STERADIANS = 40L;
    public const long SAHPI_SU_REVOLUTIONS = 41L;
    public const long SAHPI_SU_CYCLES = 42L;
    public const long SAHPI_SU_GRAVITIES = 43L;
    public const long SAHPI_SU_OUNCE = 44L;
    public const long SAHPI_SU_POUND = 45L;
    public const long SAHPI_SU_FT_LB = 46L;
    public const long SAHPI_SU_OZ_IN = 47L;
    public const long SAHPI_SU_GAUSS = 48L;
    public const long SAHPI_SU_GILBERTS = 49L;
    public const long SAHPI_SU_HENRY = 50L;
    public const long SAHPI_SU_MILLIHENRY = 51L;
    public const long SAHPI_SU_FARAD = 52L;
    public const long SAHPI_SU_MICROFARAD = 53L;
    public const long SAHPI_SU_OHMS = 54L;
    public const long SAHPI_SU_SIEMENS = 55L;
    public const long SAHPI_SU_MOLE = 56L;
    public const long SAHPI_SU_BECQUEREL = 57L;
    public const long SAHPI_SU_PPM = 58L;
    public const long SAHPI_SU_RESERVED = 59L;
    public const long SAHPI_SU_DECIBELS = 60L;
    public const long SAHPI_SU_DBA = 61L;
    public const long SAHPI_SU_DBC = 62L;
    public const long SAHPI_SU_GRAY = 63L;
    public const long SAHPI_SU_SIEVERT = 64L;
    public const long SAHPI_SU_COLOR_TEMP_DEG_K = 65L;
    public const long SAHPI_SU_BIT = 66L;
    public const long SAHPI_SU_KILOBIT = 67L;
    public const long SAHPI_SU_MEGABIT = 68L;
    public const long SAHPI_SU_GIGABIT = 69L;
    public const long SAHPI_SU_BYTE = 70L;
    public const long SAHPI_SU_KILOBYTE = 71L;
    public const long SAHPI_SU_MEGABYTE = 72L;
    public const long SAHPI_SU_GIGABYTE = 73L;
    public const long SAHPI_SU_WORD = 74L;
    public const long SAHPI_SU_DWORD = 75L;
    public const long SAHPI_SU_QWORD = 76L;
    public const long SAHPI_SU_LINE = 77L;
    public const long SAHPI_SU_HIT = 78L;
    public const long SAHPI_SU_MISS = 79L;
    public const long SAHPI_SU_RETRY = 80L;
    public const long SAHPI_SU_RESET = 81L;
    public const long SAHPI_SU_OVERRUN = 82L;
    public const long SAHPI_SU_UNDERRUN = 83L;
    public const long SAHPI_SU_COLLISION = 84L;
    public const long SAHPI_SU_PACKETS = 85L;
    public const long SAHPI_SU_MESSAGES = 86L;
    public const long SAHPI_SU_CHARACTERS = 87L;
    public const long SAHPI_SU_ERRORS = 88L;
    public const long SAHPI_SU_CORRECTABLE_ERRORS = 89L;
    public const long SAHPI_SU_UNCORRECTABLE_ERRORS = 90L;
    public const long SAHPI_SU_MAX_VALID = 90L;
    public const long SAHPI_SMUU_NONE = 0L;
    public const long SAHPI_SMUU_BASIC_OVER_MODIFIER = 1L;
    public const long SAHPI_SMUU_BASIC_TIMES_MODIFIER = 2L;
    public const long SAHPI_SMUU_MAX_VALID = 2L;
    public const long SAHPI_SEC_PER_EVENT = 0L;
    public const long SAHPI_SEC_READ_ONLY_MASKS = 1L;
    public const long SAHPI_SEC_READ_ONLY = 2L;
    public const long SAHPI_SEC_MAX_VALID = 2L;
    public const long SAHPI_CTRL_TYPE_DIGITAL = 0L;
    public const long SAHPI_CTRL_TYPE_DISCRETE = 1L;
    public const long SAHPI_CTRL_TYPE_ANALOG = 2L;
    public const long SAHPI_CTRL_TYPE_STREAM = 3L;
    public const long SAHPI_CTRL_TYPE_TEXT = 4L;
    public const long SAHPI_CTRL_TYPE_OEM = 192L;
    public const long SAHPI_CTRL_TYPE_MAX_VALID = 192L;
    public const long SAHPI_CTRL_STATE_OFF = 0L;
    public const long SAHPI_CTRL_STATE_ON = 1L;
    public const long SAHPI_CTRL_STATE_PULSE_OFF = 2L;
    public const long SAHPI_CTRL_STATE_PULSE_ON = 3L;
    public const long SAHPI_CTRL_STATE_MAX_VALID = 3L;
    public const long SAHPI_CTRL_MODE_AUTO = 0L;
    public const long SAHPI_CTRL_MODE_MANUAL = 1L;
    public const long SAHPI_CTRL_MODE_MAX_VALID = 1L;
    public const long SAHPI_CTRL_GENERIC = 0L;
    public const long SAHPI_CTRL_LED = 1L;
    public const long SAHPI_CTRL_FAN_SPEED = 2L;
    public const long SAHPI_CTRL_DRY_CONTACT_CLOSURE = 3L;
    public const long SAHPI_CTRL_POWER_SUPPLY_INHIBIT = 4L;
    public const long SAHPI_CTRL_AUDIBLE = 5L;
    public const long SAHPI_CTRL_FRONT_PANEL_LOCKOUT = 6L;
    public const long SAHPI_CTRL_POWER_INTERLOCK = 7L;
    public const long SAHPI_CTRL_POWER_STATE = 8L;
    public const long SAHPI_CTRL_LCD_DISPLAY = 9L;
    public const long SAHPI_CTRL_OEM = 10L;
    public const long SAHPI_CTRL_GENERIC_ADDRESS = 11L;
    public const long SAHPI_CTRL_IP_ADDRESS = 12L;
    public const long SAHPI_CTRL_RESOURCE_ID = 13L;
    public const long SAHPI_CTRL_POWER_BUDGET = 14L;
    public const long SAHPI_CTRL_ACTIVATE = 15L;
    public const long SAHPI_CTRL_RESET = 16L;
    public const long SAHPI_CTRL_OUTPUT_TYPE_MAX_VALID = 16L;
    public const long SAHPI_IDR_AREATYPE_INTERNAL_USE = 176L;
    public const long SAHPI_IDR_AREATYPE_CHASSIS_INFO = 177L;
    public const long SAHPI_IDR_AREATYPE_BOARD_INFO = 178L;
    public const long SAHPI_IDR_AREATYPE_PRODUCT_INFO = 179L;
    public const long SAHPI_IDR_AREATYPE_OEM = 192L;
    public const long SAHPI_IDR_AREATYPE_UNSPECIFIED = 255L;
    public const long SAHPI_IDR_AREATYPE_MAX_VALID = 255L;
    public const long SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE = 0L;
    public const long SAHPI_IDR_FIELDTYPE_MFG_DATETIME = 1L;
    public const long SAHPI_IDR_FIELDTYPE_MANUFACTURER = 2L;
    public const long SAHPI_IDR_FIELDTYPE_PRODUCT_NAME = 3L;
    public const long SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION = 4L;
    public const long SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER = 5L;
    public const long SAHPI_IDR_FIELDTYPE_PART_NUMBER = 6L;
    public const long SAHPI_IDR_FIELDTYPE_FILE_ID = 7L;
    public const long SAHPI_IDR_FIELDTYPE_ASSET_TAG = 8L;
    public const long SAHPI_IDR_FIELDTYPE_CUSTOM = 9L;
    public const long SAHPI_IDR_FIELDTYPE_UNSPECIFIED = 255L;
    public const long SAHPI_IDR_FIELDTYPE_MAX_VALID = 255L;
    public const long SAHPI_WA_NO_ACTION = 0L;
    public const long SAHPI_WA_RESET = 1L;
    public const long SAHPI_WA_POWER_DOWN = 2L;
    public const long SAHPI_WA_POWER_CYCLE = 3L;
    public const long SAHPI_WA_MAX_VALID = 3L;
    public const long SAHPI_WAE_NO_ACTION = 0L;
    public const long SAHPI_WAE_RESET = 1L;
    public const long SAHPI_WAE_POWER_DOWN = 2L;
    public const long SAHPI_WAE_POWER_CYCLE = 3L;
    public const long SAHPI_WAE_TIMER_INT = 8L;
    public const long SAHPI_WAE_MAX_VALID = 8L;
    public const long SAHPI_WPI_NONE = 0L;
    public const long SAHPI_WPI_SMI = 1L;
    public const long SAHPI_WPI_NMI = 2L;
    public const long SAHPI_WPI_MESSAGE_INTERRUPT = 3L;
    public const long SAHPI_WPI_OEM = 15L;
    public const long SAHPI_WPI_MAX_VALID = 15L;
    public const long SAHPI_WTU_NONE = 0L;
    public const long SAHPI_WTU_BIOS_FRB2 = 1L;
    public const long SAHPI_WTU_BIOS_POST = 2L;
    public const long SAHPI_WTU_OS_LOAD = 3L;
    public const long SAHPI_WTU_SMS_OS = 4L;
    public const long SAHPI_WTU_OEM = 5L;
    public const long SAHPI_WTU_UNSPECIFIED = 15L;
    public const long SAHPI_WTU_MAX_VALID = 15L;
    public const long SAHPI_DIMITEST_NONDEGRADING = 0L;
    public const long SAHPI_DIMITEST_DEGRADING = 1L;
    public const long SAHPI_DIMITEST_VENDOR_DEFINED_LEVEL = 2L;
    public const long SAHPI_DIMITEST_SERVICE_IMPACT_MAX_VALID = 2L;
    public const long SAHPI_DIMITEST_STATUS_NOT_RUN = 0L;
    public const long SAHPI_DIMITEST_STATUS_FINISHED_NO_ERRORS = 1L;
    public const long SAHPI_DIMITEST_STATUS_FINISHED_ERRORS = 2L;
    public const long SAHPI_DIMITEST_STATUS_CANCELED = 3L;
    public const long SAHPI_DIMITEST_STATUS_RUNNING = 4L;
    public const long SAHPI_DIMITEST_STATUS_MAX_VALID = 4L;
    public const long SAHPI_DIMITEST_STATUSERR_NOERR = 0L;
    public const long SAHPI_DIMITEST_STATUSERR_RUNERR = 1L;
    public const long SAHPI_DIMITEST_STATUSERR_UNDEF = 2L;
    public const long SAHPI_DIMITEST_STATUSERR_MAX_VALID = 2L;
    public const long SAHPI_DIMITEST_PARAM_TYPE_BOOLEAN = 0L;
    public const long SAHPI_DIMITEST_PARAM_TYPE_INT32 = 1L;
    public const long SAHPI_DIMITEST_PARAM_TYPE_FLOAT64 = 2L;
    public const long SAHPI_DIMITEST_PARAM_TYPE_TEXT = 3L;
    public const long SAHPI_DIMITEST_PARAM_TYPE_MAX_VALID = 3L;
    public const long SAHPI_DIMI_READY = 0L;
    public const long SAHPI_DIMI_WRONG_STATE = 1L;
    public const long SAHPI_DIMI_BUSY = 2L;
    public const long SAHPI_DIMI_READY_MAX_VALID = 2L;
    public const long SAHPI_FUMI_SPEC_INFO_NONE = 0L;
    public const long SAHPI_FUMI_SPEC_INFO_SAF_DEFINED = 1L;
    public const long SAHPI_FUMI_SPEC_INFO_OEM_DEFINED = 2L;
    public const long SAHPI_FUMI_SPEC_INFO_MAX_VALID = 2L;
    public const long SAHPI_FUMI_SPEC_HPM1 = 0L;
    public const long SAHPI_FUMI_SPEC_MAX_VALID = 0L;
    public const long SAHPI_FUMI_PROCESS_NONDEGRADING = 0L;
    public const long SAHPI_FUMI_PROCESS_DEGRADING = 1L;
    public const long SAHPI_FUMI_PROCESS_VENDOR_DEFINED_IMPACT_LEVEL = 2L;
    public const long SAHPI_FUMI_PROCESS_IMPACT_MAX_VALID = 2L;
    public const long SAHPI_FUMI_SRC_VALID = 0L;
    public const long SAHPI_FUMI_SRC_PROTOCOL_NOT_SUPPORTED = 1L;
    public const long SAHPI_FUMI_SRC_UNREACHABLE = 2L;
    public const long SAHPI_FUMI_SRC_VALIDATION_NOT_STARTED = 3L;
    public const long SAHPI_FUMI_SRC_VALIDATION_INITIATED = 4L;
    public const long SAHPI_FUMI_SRC_VALIDATION_FAIL = 5L;
    public const long SAHPI_FUMI_SRC_TYPE_MISMATCH = 6L;
    public const long SAHPI_FUMI_SRC_INVALID = 7L;
    public const long SAHPI_FUMI_SRC_VALIDITY_UNKNOWN = 8L;
    public const long SAHPI_FUMI_SRC_STATUS_MAX_VALID = 8L;
    public const long SAHPI_FUMI_BANK_VALID = 0L;
    public const long SAHPI_FUMI_BANK_UPGRADE_IN_PROGRESS = 1L;
    public const long SAHPI_FUMI_BANK_CORRUPTED = 2L;
    public const long SAHPI_FUMI_BANK_ACTIVE = 3L;
    public const long SAHPI_FUMI_BANK_BUSY = 4L;
    public const long SAHPI_FUMI_BANK_UNKNOWN = 5L;
    public const long SAHPI_FUMI_BANK_STATE_MAX_VALID = 5L;
    public const long SAHPI_FUMI_OPERATION_NOTSTARTED = 0L;
    public const long SAHPI_FUMI_SOURCE_VALIDATION_INITIATED = 1L;
    public const long SAHPI_FUMI_SOURCE_VALIDATION_FAILED = 2L;
    public const long SAHPI_FUMI_SOURCE_VALIDATION_DONE = 3L;
    public const long SAHPI_FUMI_SOURCE_VALIDATION_CANCELLED = 4L;
    public const long SAHPI_FUMI_INSTALL_INITIATED = 5L;
    public const long SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NEEDED = 6L;
    public const long SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_INITIATED = 7L;
    public const long SAHPI_FUMI_INSTALL_FAILED_ROLLBACK_NOT_POSSIBLE = 8L;
    public const long SAHPI_FUMI_INSTALL_DONE = 9L;
    public const long SAHPI_FUMI_INSTALL_CANCELLED = 10L;
    public const long SAHPI_FUMI_ROLLBACK_INITIATED = 11L;
    public const long SAHPI_FUMI_ROLLBACK_FAILED = 12L;
    public const long SAHPI_FUMI_ROLLBACK_DONE = 13L;
    public const long SAHPI_FUMI_ROLLBACK_CANCELLED = 14L;
    public const long SAHPI_FUMI_BACKUP_INITIATED = 15L;
    public const long SAHPI_FUMI_BACKUP_FAILED = 16L;
    public const long SAHPI_FUMI_BACKUP_DONE = 17L;
    public const long SAHPI_FUMI_BACKUP_CANCELLED = 18L;
    public const long SAHPI_FUMI_BANK_COPY_INITIATED = 19L;
    public const long SAHPI_FUMI_BANK_COPY_FAILED = 20L;
    public const long SAHPI_FUMI_BANK_COPY_DONE = 21L;
    public const long SAHPI_FUMI_BANK_COPY_CANCELLED = 22L;
    public const long SAHPI_FUMI_TARGET_VERIFY_INITIATED = 23L;
    public const long SAHPI_FUMI_TARGET_VERIFY_FAILED = 24L;
    public const long SAHPI_FUMI_TARGET_VERIFY_DONE = 25L;
    public const long SAHPI_FUMI_TARGET_VERIFY_CANCELLED = 26L;
    public const long SAHPI_FUMI_ACTIVATE_INITIATED = 27L;
    public const long SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NEEDED = 28L;
    public const long SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_INITIATED = 29L;
    public const long SAHPI_FUMI_ACTIVATE_FAILED_ROLLBACK_NOT_POSSIBLE = 30L;
    public const long SAHPI_FUMI_ACTIVATE_DONE = 31L;
    public const long SAHPI_FUMI_ACTIVATE_CANCELLED = 32L;
    public const long SAHPI_FUMI_UPGRADE_STATUS_MAX_VALID = 32L;
    public const long SAHPI_HS_INDICATOR_OFF = 0L;
    public const long SAHPI_HS_INDICATOR_ON = 1L;
    public const long SAHPI_HS_INDICATOR_STATE_MAX_VALID = 1L;
    public const long SAHPI_HS_ACTION_INSERTION = 0L;
    public const long SAHPI_HS_ACTION_EXTRACTION = 1L;
    public const long SAHPI_HS_ACTION_MAX_VALID = 1L;
    public const long SAHPI_HS_STATE_INACTIVE = 0L;
    public const long SAHPI_HS_STATE_INSERTION_PENDING = 1L;
    public const long SAHPI_HS_STATE_ACTIVE = 2L;
    public const long SAHPI_HS_STATE_EXTRACTION_PENDING = 3L;
    public const long SAHPI_HS_STATE_NOT_PRESENT = 4L;
    public const long SAHPI_HS_STATE_MAX_VALID = 4L;
    public const long SAHPI_HS_CAUSE_AUTO_POLICY = 0L;
    public const long SAHPI_HS_CAUSE_EXT_SOFTWARE = 1L;
    public const long SAHPI_HS_CAUSE_OPERATOR_INIT = 2L;
    public const long SAHPI_HS_CAUSE_USER_UPDATE = 3L;
    public const long SAHPI_HS_CAUSE_UNEXPECTED_DEACTIVATION = 4L;
    public const long SAHPI_HS_CAUSE_SURPRISE_EXTRACTION = 5L;
    public const long SAHPI_HS_CAUSE_EXTRACTION_UPDATE = 6L;
    public const long SAHPI_HS_CAUSE_HARDWARE_FAULT = 7L;
    public const long SAHPI_HS_CAUSE_CONTAINING_FRU = 8L;
    public const long SAHPI_HS_CAUSE_UNKNOWN = 65535L;
    public const long SAHPI_HS_CAUSE_MAX_VALID = 65535L;
    public const long SAHPI_CRITICAL = 0L;
    public const long SAHPI_MAJOR = 1L;
    public const long SAHPI_MINOR = 2L;
    public const long SAHPI_INFORMATIONAL = 3L;
    public const long SAHPI_OK = 4L;
    public const long SAHPI_DEBUG = 240L;
    public const long SAHPI_ALL_SEVERITIES = 255L;
    public const long SAHPI_SEVERITY_MAX_VALID = 240L;
    public const long SAHPI_RESE_RESOURCE_FAILURE = 0L;
    public const long SAHPI_RESE_RESOURCE_RESTORED = 1L;
    public const long SAHPI_RESE_RESOURCE_ADDED = 2L;
    public const long SAHPI_RESE_RESOURCE_REMOVED = 3L;
    public const long SAHPI_RESE_RESOURCE_INACCESSIBLE = 4L;
    public const long SAHPI_RESE_RESOURCE_UPDATED = 5L;
    public const long SAHPI_RESE_TYPE_MAX_VALID = 5L;
    public const long SAHPI_DOMAIN_REF_ADDED = 0L;
    public const long SAHPI_DOMAIN_REF_REMOVED = 1L;
    public const long SAHPI_DOMAIN_EVENT_TYPE_MAX_VALID = 1L;
    public const long SAHPI_HPIE_AUDIT = 0L;
    public const long SAHPI_HPIE_STARTUP = 1L;
    public const long SAHPI_HPIE_OTHER = 2L;
    public const long SAHPI_HPIE_TYPE_MAX_VALID = 2L;
    public const long SAHPI_ET_RESOURCE = 0L;
    public const long SAHPI_ET_DOMAIN = 1L;
    public const long SAHPI_ET_SENSOR = 2L;
    public const long SAHPI_ET_SENSOR_ENABLE_CHANGE = 3L;
    public const long SAHPI_ET_HOTSWAP = 4L;
    public const long SAHPI_ET_WATCHDOG = 5L;
    public const long SAHPI_ET_HPI_SW = 6L;
    public const long SAHPI_ET_OEM = 7L;
    public const long SAHPI_ET_USER = 8L;
    public const long SAHPI_ET_DIMI = 9L;
    public const long SAHPI_ET_DIMI_UPDATE = 10L;
    public const long SAHPI_ET_FUMI = 11L;
    public const long SAHPI_ET_MAX_VALID = 11L;
    public const long SAHPI_STATUS_COND_TYPE_SENSOR = 0L;
    public const long SAHPI_STATUS_COND_TYPE_RESOURCE = 1L;
    public const long SAHPI_STATUS_COND_TYPE_OEM = 2L;
    public const long SAHPI_STATUS_COND_TYPE_USER = 3L;
    public const long SAHPI_STATUS_COND_TYPE_MAX_VALID = 3L;
    public const long SAHPI_ANNUNCIATOR_MODE_AUTO = 0L;
    public const long SAHPI_ANNUNCIATOR_MODE_USER = 1L;
    public const long SAHPI_ANNUNCIATOR_MODE_SHARED = 2L;
    public const long SAHPI_ANNUNCIATOR_MODE_MAX_VALID = 2L;
    public const long SAHPI_ANNUNCIATOR_TYPE_LED = 0L;
    public const long SAHPI_ANNUNCIATOR_TYPE_DRY_CONTACT_CLOSURE = 1L;
    public const long SAHPI_ANNUNCIATOR_TYPE_AUDIBLE = 2L;
    public const long SAHPI_ANNUNCIATOR_TYPE_LCD_DISPLAY = 3L;
    public const long SAHPI_ANNUNCIATOR_TYPE_MESSAGE = 4L;
    public const long SAHPI_ANNUNCIATOR_TYPE_COMPOSITE = 5L;
    public const long SAHPI_ANNUNCIATOR_TYPE_OEM = 6L;
    public const long SAHPI_ANNUNCIATOR_TYPE_MAX_VALID = 6L;
    public const long SAHPI_NO_RECORD = 0L;
    public const long SAHPI_CTRL_RDR = 1L;
    public const long SAHPI_SENSOR_RDR = 2L;
    public const long SAHPI_INVENTORY_RDR = 3L;
    public const long SAHPI_WATCHDOG_RDR = 4L;
    public const long SAHPI_ANNUNCIATOR_RDR = 5L;
    public const long SAHPI_DIMI_RDR = 6L;
    public const long SAHPI_FUMI_RDR = 7L;
    public const long SAHPI_RDR_TYPE_MAX_VALID = 7L;
    public const long SAHPI_DEFAULT_PARM = 0L;
    public const long SAHPI_SAVE_PARM = 1L;
    public const long SAHPI_RESTORE_PARM = 2L;
    public const long SAHPI_PARM_ACTION_MAX_VALID = 2L;
    public const long SAHPI_COLD_RESET = 0L;
    public const long SAHPI_WARM_RESET = 1L;
    public const long SAHPI_RESET_ASSERT = 2L;
    public const long SAHPI_RESET_DEASSERT = 3L;
    public const long SAHPI_RESET_MAX_VALID = 3L;
    public const long SAHPI_POWER_OFF = 0L;
    public const long SAHPI_POWER_ON = 1L;
    public const long SAHPI_POWER_CYCLE = 2L;
    public const long SAHPI_POWER_STATE_MAX_VALID = 2L;
    public const long SAHPI_EL_OVERFLOW_DROP = 0L;
    public const long SAHPI_EL_OVERFLOW_OVERWRITE = 1L;
    public const long SAHPI_EL_OVERFLOW_ACTION_MAX_TYPE = 1L;
    public const long ATCAHPI_LED_COLOR_RESERVED = 0L;
    public const long ATCAHPI_LED_COLOR_BLUE = 1L;
    public const long ATCAHPI_LED_COLOR_RED = 2L;
    public const long ATCAHPI_LED_COLOR_GREEN = 3L;
    public const long ATCAHPI_LED_COLOR_AMBER = 4L;
    public const long ATCAHPI_LED_COLOR_ORANGE = 5L;
    public const long ATCAHPI_LED_COLOR_WHITE = 6L;
    public const long ATCAHPI_LED_COLOR_NO_CHANGE = 14L;
    public const long ATCAHPI_LED_COLOR_USE_DEFAULT = 15L;
    public const long ATCAHPI_LED_AUTO = 0L;
    public const long ATCAHPI_LED_MANUAL = 1L;
    public const long ATCAHPI_LED_LAMP_TEST = 2L;
    public const long ATCAHPI_LED_BR_SUPPORTED = 0L;
    public const long ATCAHPI_LED_BR_NOT_SUPPORTED = 1L;
    public const long ATCAHPI_LED_BR_UNKNOWN = 2L;
    public const long XTCAHPI_LED_COLOR_RESERVED = 0L;
    public const long XTCAHPI_LED_COLOR_BLUE = 1L;
    public const long XTCAHPI_LED_COLOR_RED = 2L;
    public const long XTCAHPI_LED_COLOR_GREEN = 3L;
    public const long XTCAHPI_LED_COLOR_AMBER = 4L;
    public const long XTCAHPI_LED_COLOR_ORANGE = 5L;
    public const long XTCAHPI_LED_COLOR_WHITE = 6L;
    public const long XTCAHPI_LED_COLOR_NO_CHANGE = 14L;
    public const long XTCAHPI_LED_COLOR_USE_DEFAULT = 15L;
    public const long XTCAHPI_LED_AUTO = 0L;
    public const long XTCAHPI_LED_MANUAL = 1L;
    public const long XTCAHPI_LED_LAMP_TEST = 2L;
    public const long XTCAHPI_LED_BR_SUPPORTED = 0L;
    public const long XTCAHPI_LED_BR_NOT_SUPPORTED = 1L;
    public const long XTCAHPI_LED_BR_UNKNOWN = 2L;
    public const long SA_ERR_HPI_OK = 0L;
};

/**********************************************************
 * HPI Simple Data Types Map
 *********************************************************/

/**********************************************************
 *  SaHpiUint8T : long
 *  SaHpiUint16T : long
 *  SaHpiUint32T : long
 *  SaHpiUint64T : long
 *  SaHpiInt8T : long
 *  SaHpiInt16T : long
 *  SaHpiInt32T : long
 *  SaHpiInt64T : long
 *  Enum : long
 *  SaHpiFloat64T : double
 *  SaHpiBoolT : long
 *  SaHpiManufacturerIdT : long
 *  SaHpiVersionT : long
 *  SaErrorT : long
 *  SaHpiDomainIdT : long
 *  SaHpiSessionIdT : long
 *  SaHpiResourceIdT : long
 *  SaHpiEntryIdT : long
 *  SaHpiTimeT : long
 *  SaHpiTimeoutT : long
 *  SaHpiInstrumentIdT : long
 *  SaHpiEntityLocationT : long
 *  SaHpiEventCategoryT : long
 *  SaHpiEventStateT : long
 *  SaHpiSensorNumT : long
 *  SaHpiSensorRangeFlagsT : long
 *  SaHpiSensorThdMaskT : long
 *  SaHpiCtrlNumT : long
 *  SaHpiCtrlStateDiscreteT : long
 *  SaHpiCtrlStateAnalogT : long
 *  SaHpiTxtLineNumT : long
 *  SaHpiIdrIdT : long
 *  SaHpiWatchdogNumT : long
 *  SaHpiWatchdogExpFlagsT : long
 *  SaHpiDimiNumT : long
 *  SaHpiDimiTestCapabilityT : long
 *  SaHpiDimiTestNumT : long
 *  SaHpiDimiTestPercentCompletedT : long
 *  SaHpiFumiNumT : long
 *  SaHpiBankNumT : long
 *  SaHpiFumiLogicalBankStateFlagsT : long
 *  SaHpiFumiProtocolT : long
 *  SaHpiFumiCapabilityT : long
 *  SaHpiSensorOptionalDataT : long
 *  SaHpiSensorEnableOptDataT : long
 *  SaHpiEvtQueueStatusT : long
 *  SaHpiAnnunciatorNumT : long
 *  SaHpiLoadNumberT : long
 *  SaHpiCapabilitiesT : long
 *  SaHpiHsCapabilitiesT : long
 *  SaHpiDomainCapabilitiesT : long
 *  SaHpiAlarmIdT : long
 *  SaHpiEventLogCapabilitiesT : long
 *  SaHpiEventLogEntryIdT : long
 *  AtcaHpiColorCapabilitiesT : long
 *  AtcaHpiEntityTypeT : long
 *  AtcaHpiCsCurrentPwrState : long
 *  AtcaHpiCsLastPwrEvent : long
 *  AtcaHpiCsMiscChassisState : long
 *  AtcaHpiCsFpButtonCap : long
 *  XtcaHpiCsCurrentPwrState : long
 *  XtcaHpiCsLastPwrEvent : long
 *  XtcaHpiCsMiscChassisState : long
 *  XtcaHpiCsFpButtonCap : long
 *  XtcaHpiColorCapabilitiesT : long
 *  SaHpiLanguageT : long
 *  SaHpiTextTypeT : long
 *  SaHpiEntityTypeT : long
 *  SaHpiSensorTypeT : long
 *  SaHpiSensorReadingTypeT : long
 *  SaHpiSensorEventMaskActionT : long
 *  SaHpiSensorUnitsT : long
 *  SaHpiSensorModUnitUseT : long
 *  SaHpiSensorEventCtrlT : long
 *  SaHpiCtrlTypeT : long
 *  SaHpiCtrlStateDigitalT : long
 *  SaHpiCtrlModeT : long
 *  SaHpiCtrlOutputTypeT : long
 *  SaHpiIdrAreaTypeT : long
 *  SaHpiIdrFieldTypeT : long
 *  SaHpiWatchdogActionT : long
 *  SaHpiWatchdogActionEventT : long
 *  SaHpiWatchdogPretimerInterruptT : long
 *  SaHpiWatchdogTimerUseT : long
 *  SaHpiDimiTestServiceImpactT : long
 *  SaHpiDimiTestRunStatusT : long
 *  SaHpiDimiTestErrCodeT : long
 *  SaHpiDimiTestParamTypeT : long
 *  SaHpiDimiReadyT : long
 *  SaHpiFumiSpecInfoTypeT : long
 *  SaHpiFumiSafDefinedSpecIdT : long
 *  SaHpiFumiServiceImpactT : long
 *  SaHpiFumiSourceStatusT : long
 *  SaHpiFumiBankStateT : long
 *  SaHpiFumiUpgradeStatusT : long
 *  SaHpiHsIndicatorStateT : long
 *  SaHpiHsActionT : long
 *  SaHpiHsStateT : long
 *  SaHpiHsCauseOfStateChangeT : long
 *  SaHpiSeverityT : long
 *  SaHpiResourceEventTypeT : long
 *  SaHpiDomainEventTypeT : long
 *  SaHpiSwEventTypeT : long
 *  SaHpiEventTypeT : long
 *  SaHpiStatusCondTypeT : long
 *  SaHpiAnnunciatorModeT : long
 *  SaHpiAnnunciatorTypeT : long
 *  SaHpiRdrTypeT : long
 *  SaHpiParmActionT : long
 *  SaHpiResetActionT : long
 *  SaHpiPowerStateT : long
 *  SaHpiEventLogOverflowActionT : long
 *  AtcaHpiLedColorT : long
 *  AtcaHpiResourceLedModeT : long
 *  AtcaHpiLedBrSupportT : long
 *  XtcaHpiLedColorT : long
 *  XtcaHpiResourceLedModeT : long
 *  XtcaHpiLedBrSupportT : long
 *  SaHpiUint8T[] : byte[]
 *********************************************************/

/**********************************************************
 * HPI Complex Data Types
 *********************************************************/

/**
 * HPI struct SaHpiTextBufferT
 */
public class SaHpiTextBufferT
{
    public long DataType;
    public long Language;
    public long DataLength;
    // Array of SAHPI_MAX_TEXT_BUFFER_LENGTH elements
    public byte[] Data;
};

/**
 * HPI struct SaHpiEntityT
 */
public class SaHpiEntityT
{
    public long EntityType;
    public long EntityLocation;
};

/**
 * HPI struct SaHpiEntityPathT
 */
public class SaHpiEntityPathT
{
    // Array of SAHPI_MAX_ENTITY_PATH elements
    public SaHpiEntityT[] Entry;
};

/**
 * HPI union SaHpiSensorReadingUnionT
 */
public class SaHpiSensorReadingUnionT
{
    public long SensorInt64;
    public long SensorUint64;
    public double SensorFloat64;
    // Array of SAHPI_SENSOR_BUFFER_LENGTH elements
    public byte[] SensorBuffer;
};

/**
 * HPI struct SaHpiSensorReadingT
 */
public class SaHpiSensorReadingT
{
    public long IsSupported;
    public long Type;
    public SaHpiSensorReadingUnionT Value;
};

/**
 * HPI struct SaHpiSensorThresholdsT
 */
public class SaHpiSensorThresholdsT
{
    public SaHpiSensorReadingT LowCritical;
    public SaHpiSensorReadingT LowMajor;
    public SaHpiSensorReadingT LowMinor;
    public SaHpiSensorReadingT UpCritical;
    public SaHpiSensorReadingT UpMajor;
    public SaHpiSensorReadingT UpMinor;
    public SaHpiSensorReadingT PosThdHysteresis;
    public SaHpiSensorReadingT NegThdHysteresis;
};

/**
 * HPI struct SaHpiSensorRangeT
 */
public class SaHpiSensorRangeT
{
    public long Flags;
    public SaHpiSensorReadingT Max;
    public SaHpiSensorReadingT Min;
    public SaHpiSensorReadingT Nominal;
    public SaHpiSensorReadingT NormalMax;
    public SaHpiSensorReadingT NormalMin;
};

/**
 * HPI struct SaHpiSensorDataFormatT
 */
public class SaHpiSensorDataFormatT
{
    public long IsSupported;
    public long ReadingType;
    public long BaseUnits;
    public long ModifierUnits;
    public long ModifierUse;
    public long Percentage;
    public SaHpiSensorRangeT Range;
    public double AccuracyFactor;
};

/**
 * HPI struct SaHpiSensorThdDefnT
 */
public class SaHpiSensorThdDefnT
{
    public long IsAccessible;
    public long ReadThold;
    public long WriteThold;
    public long Nonlinear;
};

/**
 * HPI struct SaHpiSensorRecT
 */
public class SaHpiSensorRecT
{
    public long Num;
    public long Type;
    public long Category;
    public long EnableCtrl;
    public long EventCtrl;
    public long Events;
    public SaHpiSensorDataFormatT DataFormat;
    public SaHpiSensorThdDefnT ThresholdDefn;
    public long Oem;
};

/**
 * HPI struct SaHpiCtrlStateStreamT
 */
public class SaHpiCtrlStateStreamT
{
    public long Repeat;
    public long StreamLength;
    // Array of SAHPI_CTRL_MAX_STREAM_LENGTH elements
    public byte[] Stream;
};

/**
 * HPI struct SaHpiCtrlStateTextT
 */
public class SaHpiCtrlStateTextT
{
    public long Line;
    public SaHpiTextBufferT Text;
};

/**
 * HPI struct SaHpiCtrlStateOemT
 */
public class SaHpiCtrlStateOemT
{
    public long MId;
    public long BodyLength;
    // Array of SAHPI_CTRL_MAX_OEM_BODY_LENGTH elements
    public byte[] Body;
};

/**
 * HPI union SaHpiCtrlStateUnionT
 */
public class SaHpiCtrlStateUnionT
{
    public long Digital;
    public long Discrete;
    public long Analog;
    public SaHpiCtrlStateStreamT Stream;
    public SaHpiCtrlStateTextT Text;
    public SaHpiCtrlStateOemT Oem;
};

/**
 * HPI struct SaHpiCtrlStateT
 */
public class SaHpiCtrlStateT
{
    public long Type;
    public SaHpiCtrlStateUnionT StateUnion;
};

/**
 * HPI struct SaHpiCtrlRecDigitalT
 */
public class SaHpiCtrlRecDigitalT
{
    public long Default;
};

/**
 * HPI struct SaHpiCtrlRecDiscreteT
 */
public class SaHpiCtrlRecDiscreteT
{
    public long Default;
};

/**
 * HPI struct SaHpiCtrlRecAnalogT
 */
public class SaHpiCtrlRecAnalogT
{
    public long Min;
    public long Max;
    public long Default;
};

/**
 * HPI struct SaHpiCtrlRecStreamT
 */
public class SaHpiCtrlRecStreamT
{
    public SaHpiCtrlStateStreamT Default;
};

/**
 * HPI struct SaHpiCtrlRecTextT
 */
public class SaHpiCtrlRecTextT
{
    public long MaxChars;
    public long MaxLines;
    public long Language;
    public long DataType;
    public SaHpiCtrlStateTextT Default;
};

/**
 * HPI struct SaHpiCtrlRecOemT
 */
public class SaHpiCtrlRecOemT
{
    public long MId;
    // Array of SAHPI_CTRL_OEM_CONFIG_LENGTH elements
    public byte[] ConfigData;
    public SaHpiCtrlStateOemT Default;
};

/**
 * HPI union SaHpiCtrlRecUnionT
 */
public class SaHpiCtrlRecUnionT
{
    public SaHpiCtrlRecDigitalT Digital;
    public SaHpiCtrlRecDiscreteT Discrete;
    public SaHpiCtrlRecAnalogT Analog;
    public SaHpiCtrlRecStreamT Stream;
    public SaHpiCtrlRecTextT Text;
    public SaHpiCtrlRecOemT Oem;
};

/**
 * HPI struct SaHpiCtrlDefaultModeT
 */
public class SaHpiCtrlDefaultModeT
{
    public long Mode;
    public long ReadOnly;
};

/**
 * HPI struct SaHpiCtrlRecT
 */
public class SaHpiCtrlRecT
{
    public long Num;
    public long OutputType;
    public long Type;
    public SaHpiCtrlRecUnionT TypeUnion;
    public SaHpiCtrlDefaultModeT DefaultMode;
    public long WriteOnly;
    public long Oem;
};

/**
 * HPI struct SaHpiIdrFieldT
 */
public class SaHpiIdrFieldT
{
    public long AreaId;
    public long FieldId;
    public long Type;
    public long ReadOnly;
    public SaHpiTextBufferT Field;
};

/**
 * HPI struct SaHpiIdrAreaHeaderT
 */
public class SaHpiIdrAreaHeaderT
{
    public long AreaId;
    public long Type;
    public long ReadOnly;
    public long NumFields;
};

/**
 * HPI struct SaHpiIdrInfoT
 */
public class SaHpiIdrInfoT
{
    public long IdrId;
    public long UpdateCount;
    public long ReadOnly;
    public long NumAreas;
};

/**
 * HPI struct SaHpiInventoryRecT
 */
public class SaHpiInventoryRecT
{
    public long IdrId;
    public long Persistent;
    public long Oem;
};

/**
 * HPI struct SaHpiWatchdogT
 */
public class SaHpiWatchdogT
{
    public long Log;
    public long Running;
    public long TimerUse;
    public long TimerAction;
    public long PretimerInterrupt;
    public long PreTimeoutInterval;
    public long TimerUseExpFlags;
    public long InitialCount;
    public long PresentCount;
};

/**
 * HPI struct SaHpiWatchdogRecT
 */
public class SaHpiWatchdogRecT
{
    public long WatchdogNum;
    public long Oem;
};

/**
 * HPI struct SaHpiDimiTestAffectedEntityT
 */
public class SaHpiDimiTestAffectedEntityT
{
    public SaHpiEntityPathT EntityImpacted;
    public long ServiceImpact;
};

/**
 * HPI struct SaHpiDimiTestResultsT
 */
public class SaHpiDimiTestResultsT
{
    public long ResultTimeStamp;
    public long RunDuration;
    public long LastRunStatus;
    public long TestErrorCode;
    public SaHpiTextBufferT TestResultString;
    public long TestResultStringIsURI;
};

/**
 * HPI union SaHpiDimiTestParamValueT
 */
public class SaHpiDimiTestParamValueT
{
    public long paramint;
    public long parambool;
    public double paramfloat;
    public SaHpiTextBufferT paramtext;
};

/**
 * HPI union SaHpiDimiTestParameterValueUnionT
 */
public class SaHpiDimiTestParameterValueUnionT
{
    public long IntValue;
    public double FloatValue;
};

/**
 * HPI struct SaHpiDimiTestParamsDefinitionT
 */
public class SaHpiDimiTestParamsDefinitionT
{
    // Array of SAHPI_DIMITEST_PARAM_NAME_LEN elements
    public byte[] ParamName;
    public SaHpiTextBufferT ParamInfo;
    public long ParamType;
    public SaHpiDimiTestParameterValueUnionT MinValue;
    public SaHpiDimiTestParameterValueUnionT MaxValue;
    public SaHpiDimiTestParamValueT DefaultParam;
};

/**
 * HPI struct SaHpiDimiTestT
 */
public class SaHpiDimiTestT
{
    public SaHpiTextBufferT TestName;
    public long ServiceImpact;
    // Array of SAHPI_DIMITEST_MAX_ENTITIESIMPACTED elements
    public SaHpiDimiTestAffectedEntityT[] EntitiesImpacted;
    public long NeedServiceOS;
    public SaHpiTextBufferT ServiceOS;
    public long ExpectedRunDuration;
    public long TestCapabilities;
    // Array of SAHPI_DIMITEST_MAX_PARAMETERS elements
    public SaHpiDimiTestParamsDefinitionT[] TestParameters;
};

/**
 * HPI struct SaHpiDimiTestVariableParamsT
 */
public class SaHpiDimiTestVariableParamsT
{
    // Array of SAHPI_DIMITEST_PARAM_NAME_LEN elements
    public byte[] ParamName;
    public long ParamType;
    public SaHpiDimiTestParamValueT Value;
};

/**
 * HPI struct SaHpiDimiInfoT
 */
public class SaHpiDimiInfoT
{
    public long NumberOfTests;
    public long TestNumUpdateCounter;
};

/**
 * HPI struct SaHpiDimiRecT
 */
public class SaHpiDimiRecT
{
    public long DimiNum;
    public long Oem;
};

/**
 * HPI struct SaHpiFumiSafDefinedSpecInfoT
 */
public class SaHpiFumiSafDefinedSpecInfoT
{
    public long SpecID;
    public long RevisionID;
};

/**
 * HPI struct SaHpiFumiOemDefinedSpecInfoT
 */
public class SaHpiFumiOemDefinedSpecInfoT
{
    public long Mid;
    public long BodyLength;
    // Array of SAHPI_FUMI_MAX_OEM_BODY_LENGTH elements
    public byte[] Body;
};

/**
 * HPI union SaHpiFumiSpecInfoTypeUnionT
 */
public class SaHpiFumiSpecInfoTypeUnionT
{
    public SaHpiFumiSafDefinedSpecInfoT SafDefined;
    public SaHpiFumiOemDefinedSpecInfoT OemDefined;
};

/**
 * HPI struct SaHpiFumiSpecInfoT
 */
public class SaHpiFumiSpecInfoT
{
    public long SpecInfoType;
    public SaHpiFumiSpecInfoTypeUnionT SpecInfoTypeUnion;
};

/**
 * HPI struct SaHpiFumiFirmwareInstanceInfoT
 */
public class SaHpiFumiFirmwareInstanceInfoT
{
    public long InstancePresent;
    public SaHpiTextBufferT Identifier;
    public SaHpiTextBufferT Description;
    public SaHpiTextBufferT DateTime;
    public long MajorVersion;
    public long MinorVersion;
    public long AuxVersion;
};

/**
 * HPI struct SaHpiFumiImpactedEntityT
 */
public class SaHpiFumiImpactedEntityT
{
    public SaHpiEntityPathT ImpactedEntity;
    public long ServiceImpact;
};

/**
 * HPI struct SaHpiFumiServiceImpactDataT
 */
public class SaHpiFumiServiceImpactDataT
{
    public long NumEntities;
    // Array of SAHPI_FUMI_MAX_ENTITIES_IMPACTED elements
    public SaHpiFumiImpactedEntityT[] ImpactedEntities;
};

/**
 * HPI struct SaHpiFumiSourceInfoT
 */
public class SaHpiFumiSourceInfoT
{
    public SaHpiTextBufferT SourceUri;
    public long SourceStatus;
    public SaHpiTextBufferT Identifier;
    public SaHpiTextBufferT Description;
    public SaHpiTextBufferT DateTime;
    public long MajorVersion;
    public long MinorVersion;
    public long AuxVersion;
};

/**
 * HPI struct SaHpiFumiComponentInfoT
 */
public class SaHpiFumiComponentInfoT
{
    public long EntryId;
    public long ComponentId;
    public SaHpiFumiFirmwareInstanceInfoT MainFwInstance;
    public long ComponentFlags;
};

/**
 * HPI struct SaHpiFumiBankInfoT
 */
public class SaHpiFumiBankInfoT
{
    public long BankId;
    public long BankSize;
    public long Position;
    public long BankState;
    public SaHpiTextBufferT Identifier;
    public SaHpiTextBufferT Description;
    public SaHpiTextBufferT DateTime;
    public long MajorVersion;
    public long MinorVersion;
    public long AuxVersion;
};

/**
 * HPI struct SaHpiFumiLogicalBankInfoT
 */
public class SaHpiFumiLogicalBankInfoT
{
    public long FirmwarePersistentLocationCount;
    public long BankStateFlags;
    public SaHpiFumiFirmwareInstanceInfoT PendingFwInstance;
    public SaHpiFumiFirmwareInstanceInfoT RollbackFwInstance;
};

/**
 * HPI struct SaHpiFumiLogicalComponentInfoT
 */
public class SaHpiFumiLogicalComponentInfoT
{
    public long EntryId;
    public long ComponentId;
    public SaHpiFumiFirmwareInstanceInfoT PendingFwInstance;
    public SaHpiFumiFirmwareInstanceInfoT RollbackFwInstance;
    public long ComponentFlags;
};

/**
 * HPI struct SaHpiFumiRecT
 */
public class SaHpiFumiRecT
{
    public long Num;
    public long AccessProt;
    public long Capability;
    public long NumBanks;
    public long Oem;
};

/**
 * HPI struct SaHpiResourceEventT
 */
public class SaHpiResourceEventT
{
    public long ResourceEventType;
};

/**
 * HPI struct SaHpiDomainEventT
 */
public class SaHpiDomainEventT
{
    public long Type;
    public long DomainId;
};

/**
 * HPI struct SaHpiSensorEventT
 */
public class SaHpiSensorEventT
{
    public long SensorNum;
    public long SensorType;
    public long EventCategory;
    public long Assertion;
    public long EventState;
    public long OptionalDataPresent;
    public SaHpiSensorReadingT TriggerReading;
    public SaHpiSensorReadingT TriggerThreshold;
    public long PreviousState;
    public long CurrentState;
    public long Oem;
    public long SensorSpecific;
};

/**
 * HPI struct SaHpiSensorEnableChangeEventT
 */
public class SaHpiSensorEnableChangeEventT
{
    public long SensorNum;
    public long SensorType;
    public long EventCategory;
    public long SensorEnable;
    public long SensorEventEnable;
    public long AssertEventMask;
    public long DeassertEventMask;
    public long OptionalDataPresent;
    public long CurrentState;
    public long CriticalAlarms;
    public long MajorAlarms;
    public long MinorAlarms;
};

/**
 * HPI struct SaHpiHotSwapEventT
 */
public class SaHpiHotSwapEventT
{
    public long HotSwapState;
    public long PreviousHotSwapState;
    public long CauseOfStateChange;
};

/**
 * HPI struct SaHpiWatchdogEventT
 */
public class SaHpiWatchdogEventT
{
    public long WatchdogNum;
    public long WatchdogAction;
    public long WatchdogPreTimerAction;
    public long WatchdogUse;
};

/**
 * HPI struct SaHpiHpiSwEventT
 */
public class SaHpiHpiSwEventT
{
    public long MId;
    public long Type;
    public SaHpiTextBufferT EventData;
};

/**
 * HPI struct SaHpiOemEventT
 */
public class SaHpiOemEventT
{
    public long MId;
    public SaHpiTextBufferT OemEventData;
};

/**
 * HPI struct SaHpiUserEventT
 */
public class SaHpiUserEventT
{
    public SaHpiTextBufferT UserEventData;
};

/**
 * HPI struct SaHpiDimiEventT
 */
public class SaHpiDimiEventT
{
    public long DimiNum;
    public long TestNum;
    public long DimiTestRunStatus;
    public long DimiTestPercentCompleted;
};

/**
 * HPI struct SaHpiDimiUpdateEventT
 */
public class SaHpiDimiUpdateEventT
{
    public long DimiNum;
};

/**
 * HPI struct SaHpiFumiEventT
 */
public class SaHpiFumiEventT
{
    public long FumiNum;
    public long BankNum;
    public long UpgradeStatus;
};

/**
 * HPI union SaHpiEventUnionT
 */
public class SaHpiEventUnionT
{
    public SaHpiResourceEventT ResourceEvent;
    public SaHpiDomainEventT DomainEvent;
    public SaHpiSensorEventT SensorEvent;
    public SaHpiSensorEnableChangeEventT SensorEnableChangeEvent;
    public SaHpiHotSwapEventT HotSwapEvent;
    public SaHpiWatchdogEventT WatchdogEvent;
    public SaHpiHpiSwEventT HpiSwEvent;
    public SaHpiOemEventT OemEvent;
    public SaHpiUserEventT UserEvent;
    public SaHpiDimiEventT DimiEvent;
    public SaHpiDimiUpdateEventT DimiUpdateEvent;
    public SaHpiFumiEventT FumiEvent;
};

/**
 * HPI struct SaHpiEventT
 */
public class SaHpiEventT
{
    public long Source;
    public long EventType;
    public long Timestamp;
    public long Severity;
    public SaHpiEventUnionT EventDataUnion;
};

/**
 * HPI struct SaHpiNameT
 */
public class SaHpiNameT
{
    public long Length;
    // Array of SA_HPI_MAX_NAME_LENGTH elements
    public byte[] Value;
};

/**
 * HPI struct SaHpiConditionT
 */
public class SaHpiConditionT
{
    public long Type;
    public SaHpiEntityPathT Entity;
    public long DomainId;
    public long ResourceId;
    public long SensorNum;
    public long EventState;
    public SaHpiNameT Name;
    public long Mid;
    public SaHpiTextBufferT Data;
};

/**
 * HPI struct SaHpiAnnouncementT
 */
public class SaHpiAnnouncementT
{
    public long EntryId;
    public long Timestamp;
    public long AddedByUser;
    public long Severity;
    public long Acknowledged;
    public SaHpiConditionT StatusCond;
};

/**
 * HPI struct SaHpiAnnunciatorRecT
 */
public class SaHpiAnnunciatorRecT
{
    public long AnnunciatorNum;
    public long AnnunciatorType;
    public long ModeReadOnly;
    public long MaxConditions;
    public long Oem;
};

/**
 * HPI union SaHpiRdrTypeUnionT
 */
public class SaHpiRdrTypeUnionT
{
    public SaHpiCtrlRecT CtrlRec;
    public SaHpiSensorRecT SensorRec;
    public SaHpiInventoryRecT InventoryRec;
    public SaHpiWatchdogRecT WatchdogRec;
    public SaHpiAnnunciatorRecT AnnunciatorRec;
    public SaHpiDimiRecT DimiRec;
    public SaHpiFumiRecT FumiRec;
};

/**
 * HPI struct SaHpiRdrT
 */
public class SaHpiRdrT
{
    public long RecordId;
    public long RdrType;
    public SaHpiEntityPathT Entity;
    public long IsFru;
    public SaHpiRdrTypeUnionT RdrTypeUnion;
    public SaHpiTextBufferT IdString;
};

/**
 * HPI struct SaHpiLoadIdT
 */
public class SaHpiLoadIdT
{
    public long LoadNumber;
    public SaHpiTextBufferT LoadName;
};

/**
 * HPI struct SaHpiResourceInfoT
 */
public class SaHpiResourceInfoT
{
    public long ResourceRev;
    public long SpecificVer;
    public long DeviceSupport;
    public long ManufacturerId;
    public long ProductId;
    public long FirmwareMajorRev;
    public long FirmwareMinorRev;
    public long AuxFirmwareRev;
    // Array of SAHPI_GUID_LENGTH elements
    public byte[] Guid;
};

/**
 * HPI struct SaHpiRptEntryT
 */
public class SaHpiRptEntryT
{
    public long EntryId;
    public long ResourceId;
    public SaHpiResourceInfoT ResourceInfo;
    public SaHpiEntityPathT ResourceEntity;
    public long ResourceCapabilities;
    public long HotSwapCapabilities;
    public long ResourceSeverity;
    public long ResourceFailed;
    public SaHpiTextBufferT ResourceTag;
};

/**
 * HPI struct SaHpiDomainInfoT
 */
public class SaHpiDomainInfoT
{
    public long DomainId;
    public long DomainCapabilities;
    public long IsPeer;
    public SaHpiTextBufferT DomainTag;
    public long DrtUpdateCount;
    public long DrtUpdateTimestamp;
    public long RptUpdateCount;
    public long RptUpdateTimestamp;
    public long DatUpdateCount;
    public long DatUpdateTimestamp;
    public long ActiveAlarms;
    public long CriticalAlarms;
    public long MajorAlarms;
    public long MinorAlarms;
    public long DatUserAlarmLimit;
    public long DatOverflow;
    // Array of SAHPI_GUID_LENGTH elements
    public byte[] Guid;
};

/**
 * HPI struct SaHpiDrtEntryT
 */
public class SaHpiDrtEntryT
{
    public long EntryId;
    public long DomainId;
    public long IsPeer;
};

/**
 * HPI struct SaHpiAlarmT
 */
public class SaHpiAlarmT
{
    public long AlarmId;
    public long Timestamp;
    public long Severity;
    public long Acknowledged;
    public SaHpiConditionT AlarmCond;
};

/**
 * HPI struct SaHpiEventLogInfoT
 */
public class SaHpiEventLogInfoT
{
    public long Entries;
    public long Size;
    public long UserEventMaxSize;
    public long UpdateTimestamp;
    public long CurrentTime;
    public long Enabled;
    public long OverflowFlag;
    public long OverflowResetable;
    public long OverflowAction;
};

/**
 * HPI struct SaHpiEventLogEntryT
 */
public class SaHpiEventLogEntryT
{
    public long EntryId;
    public long Timestamp;
    public SaHpiEventT Event;
};


}; // namespace openhpi
}; // namespace org

