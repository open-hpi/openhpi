/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *     Bryan Sutula <Bryan.Sutula@hp.com>
 *     Raghavendra PG <raghavendra.pg@hp.com>
 *     Raghavendra MS <raghavendra.ms@hp.com>
 *     Anand S <S.Anand@hp.com>
 */

#ifndef _OA_SOAP_CALLS_H_
#define _OA_SOAP_CALLS_H_

/* Include files */
#include "oa_soap_callsupport.h"

/* Maximum length of the serial number, model number and part number */
#define MAX_SERIAL_NUM_LENGTH 32
#define MAX_PART_NUM_LENGTH 32
#define MAX_MODEL_NUM_LENGTH 32
#define MAX_PRODUCT_NAME_LENGTH 32

/* Moved these #defines from oa_soap.h - which includes this header file. */
/* Max Blade in HP BladeSystem c7000 c-Class enclosure*/
#define OA_SOAP_C7000_MAX_BLADE 16
/* Max Blade in HP BladeSystem c3000 c-Class enclosure*/
#define OA_SOAP_C3000_MAX_BLADE 8

/* Data types used to help us be more consistent with the WSDL description */
typedef unsigned char byte;


/* These define the SOAP commands used to talk to the OA */
#define GET_BLADE_INFO \
                        "<hpoa:getBladeInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getBladeInfo>\n"

#define GET_BLADE_PORTMAP \
                        "<hpoa:getBladePortMap>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getBladePortMap>\n"

#define SUBSCRIBE_FOR_EVENTS \
                        "<hpoa:subscribeForEvents>" \
                        "</hpoa:subscribeForEvents>\n"

#define UN_SUBSCRIBE_FOR_EVENTS \
                        "<hpoa:unSubscribeForEvents>" \
                        "<hpoa:pid>%d</hpoa:pid>" \
                        "</hpoa:unSubscribeForEvents>\n"

#define GET_EVENT \
                        "<hpoa:getEvent>" \
                        "<hpoa:pid>%d</hpoa:pid>" \
                        "<hpoa:waitTilEventHappens>" \
                                "%d</hpoa:waitTilEventHappens>" \
                        "<hpoa:lcdEvents>%d</hpoa:lcdEvents>" \
                        "</hpoa:getEvent>\n"

#define GET_ALL_EVENTSEX \
                        "<hpoa:getAllEventsEx>" \
                        "<hpoa:pid>%d</hpoa:pid>" \
                        "<hpoa:waitTilEventHappens>" \
                                "%d</hpoa:waitTilEventHappens>" \
                        "<hpoa:lcdEvents>%d</hpoa:lcdEvents>" \
                        "<hpoa:oaFwVersion>%s</hpoa:oaFwVersion>" \
                        "</hpoa:getAllEventsEx>\n"

#define SET_BLADE_POWER \
                        "<hpoa:setBladePower>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "<hpoa:power>%s</hpoa:power>" \
                        "</hpoa:setBladePower>\n"

#define SET_INTERCONNECT_TRAY_POWER \
                        "<hpoa:setInterconnectTrayPower>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "<hpoa:on>%d</hpoa:on>" \
                        "</hpoa:setInterconnectTrayPower>\n"

#define RESET_INTERCONNECT_TRAY \
                        "<hpoa:resetInterconnectTray>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:resetInterconnectTray>\n"

#define GET_ENCLOSURE_INFO \
                        "<hpoa:getEnclosureInfo>" \
                        "</hpoa:getEnclosureInfo>\n"

#define GET_OA_STATUS \
                        "<hpoa:getOaStatus>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getOaStatus>\n"

#define GET_OA_INFO \
                        "<hpoa:getOaInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getOaInfo>\n"

#define GET_OA_ID \
                        "<hpoa:getOaId>" \
                        "</hpoa:getOaId>\n"

#define GET_POWER_CONFIG_INFO \
                        "<hpoa:getPowerConfigInfo>" \
                        "</hpoa:getPowerConfigInfo>\n"

#define SET_POWER_CONFIG_INFO \
        "<hpoa:setPowerConfigInfo>" \
        "<hpoa:redundancyMode>%d</hpoa:redundancyMode>" \
        "<hpoa:powerCeiling>%d</hpoa:powerCeiling>" \
        "<hpoa:dynamicPowerSaverEnabled>%d</hpoa:dynamicPowerSaverEnabled>" \
        "</hpoa:setPowerConfigInfo>\n"

#define GET_POWER_CAP_CONFIG \
                        "<hpoa:getPowerCapConfig>" \
                        "</hpoa:getPowerCapConfig>\n"

#define SET_POWER_CAP_CONFIG \
        "<hpoa:setPowerCapConfig>" \
        "<hpoa:config>" \
        "<hpoa:powerCap>%d</hpoa:powerCap>" \
        "<hpoa:optOutBayArray>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "<hpoa:bay>%s</hpoa:bay>" \
        "</hpoa:optOutBayArray>" \
        "<hpoa:extraData hpoa:name=\"deratedCircuit\">%d</hpoa:extraData>" \
        "<hpoa:extraData hpoa:name=\"ratedCircuit\">%d</hpoa:extraData>" \
        "</hpoa:config>" \
        "</hpoa:setPowerCapConfig>\n"

#define GET_INTERCONNECT_TRAY_STATUS \
                        "<hpoa:getInterconnectTrayStatus>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getInterconnectTrayStatus>\n"

#define GET_INTERCONNECT_TRAY_INFO \
                        "<hpoa:getInterconnectTrayInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getInterconnectTrayInfo>\n"

#define GET_INTERCONNECT_TRAY_PORT_MAP \
                        "<hpoa:getInterconnectTrayPortMap>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getInterconnectTrayPortMap>\n"

#define GET_FAN_INFO \
                        "<hpoa:getFanInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getFanInfo>\n"

#define GET_POWER_SUBSYSTEM_INFO \
                        "<hpoa:getPowerSubsystemInfo>" \
                        "</hpoa:getPowerSubsystemInfo>\n"

#define GET_POWER_SUPPLY_INFO \
                        "<hpoa:getPowerSupplyInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getPowerSupplyInfo>\n"

#define GET_OA_NETWORK_INFO \
                        "<hpoa:getOaNetworkInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getOaNetworkInfo>\n"

#define GET_BLADE_STATUS \
                        "<hpoa:getBladeStatus>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getBladeStatus>\n"

#define GET_THERMAL_INFO \
                        "<hpoa:getThermalInfo>" \
                        "<hpoa:sensorType>%s</hpoa:sensorType>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getThermalInfo>\n"

#define GET_USER_INFO \
                        "<hpoa:getUserInfo>" \
                        "<hpoa:username>%s</hpoa:username>" \
                        "</hpoa:getUserInfo>\n"

#define IS_VALID_SESSION \
                        "<hpoa:isValidSession>" \
                        "</hpoa:isValidSession>\n"

#define GET_RACK_TOPOLOGY2 \
                        "<hpoa:getRackTopology2>" \
                        "</hpoa:getRackTopology2>\n"

#define GET_BLADE_MP_INFO \
                        "<hpoa:getBladeMpInfo>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getBladeMpInfo>\n"

#define GET_THERMAL_SUBSYSTEM_INFO \
                        "<hpoa:getThermalSubsystemInfo>" \
                        "</hpoa:getThermalSubsystemInfo>\n"

#define GET_FAN_ZONE_ARRAY \
                        "<hpoa:getFanZoneArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getFanZoneArray>\n"

#define BAY             "<hpoa:bay>%d</hpoa:bay>"

#define GET_ENCLOSURE_STATUS \
                        "<hpoa:getEnclosureStatus>" \
                        "</hpoa:getEnclosureStatus>\n"

#define GET_POWER_SUPPLY_STATUS \
                        "<hpoa:getPowerSupplyStatus>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getPowerSupplyStatus>\n"

#define GET_LCD_INFO \
                        "<hpoa:getLcdInfo>" \
                        "</hpoa:getLcdInfo>\n"

#define GET_LCD_STATUS \
                        "<hpoa:getLcdStatus>" \
                        "</hpoa:getLcdStatus>\n"

#define GET_BLADE_THERMAL_INFO_ARRAY \
                        "<hpoa:getBladeThermalInfoArray>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "</hpoa:getBladeThermalInfoArray>\n"

#define SET_ENCLOSURE_UID \
                        "<hpoa:setEnclosureUid>" \
                        "<hpoa:uid>%s</hpoa:uid>" \
                        "</hpoa:setEnclosureUid>\n"

#define SET_OA_UID \
                        "<hpoa:setOaUid>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "<hpoa:uid>%s</hpoa:uid>" \
                        "</hpoa:setOaUid>\n"

#define SET_BLADE_UID \
                        "<hpoa:setBladeUid>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "<hpoa:uid>%s</hpoa:uid>" \
                        "</hpoa:setBladeUid>\n"

#define SET_INTERCONNECT_TRAY_UID \
                        "<hpoa:setInterconnectTrayUid>" \
                        "<hpoa:bayNumber>%d</hpoa:bayNumber>" \
                        "<hpoa:uid>%s</hpoa:uid>" \
                        "</hpoa:setInterconnectTrayUid>\n"

#define SET_LCD_BUTTON_LOCK \
			"<hpoa:setLcdButtonLock>" \
			"<hpoa:buttonLock>%d</hpoa:buttonLock>" \
			"</hpoa:setLcdButtonLock>\n"

#define GET_BLADE_INFO_ARRAY \
                        "<hpoa:getBladeInfoArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getBladeInfoArray>\n"

#define GET_INTERCONNECT_TRAY_INFO_ARRAY \
                        "<hpoa:getInterconnectTrayInfoArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getInterconnectTrayInfoArray>\n"

#define GET_INTERCONNECT_TRAY_STATUS_ARRAY \
                        "<hpoa:getInterconnectTrayStatusArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getInterconnectTrayStatusArray>\n"
						
#define GET_INTERCONNECT_TRAY_PORTMAP_ARRAY \
                        "<hpoa:getInterconnectTrayPortMapArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getInterconnectTrayPortMapArray>\n"

#define GET_POWER_SUPPLY_INFO_ARRAY \
                        "<hpoa:getPowerSupplyInfoArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getPowerSupplyInfoArray>\n"
						
#define GET_POWER_SUPPLY_STATUS_ARRAY \
                        "<hpoa:getPowerSupplyStatusArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getPowerSupplyStatusArray>\n"

#define GET_FAN_INFO_ARRAY \
                        "<hpoa:getFanInfoArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getFanInfoArray>\n"
						
#define GET_OA_INFO_ARRAY \
                        "<hpoa:getOaInfoArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getOaInfoArray>\n"

#define GET_OA_STATUS_ARRAY \
                        "<hpoa:getOaStatusArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getOaStatusArray>\n"

#define GET_BLADE_STATUS_ARRAY \
                        "<hpoa:getBladeStatusArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getBladeStatusArray>\n"

#define GET_BLADE_PORTMAP_ARRAY \
                        "<hpoa:getBladePortMapArray>" \
                        "<hpoa:bayArray>%s</hpoa:bayArray>" \
                        "</hpoa:getBladePortMapArray>\n"


              
/* Enumerated types used for specific SOAP commands */
#define HPOA_BOOLEAN_LENGTH    11      /* Max length of these enums + 1 */
OA_SOAP_ENUM(hpoa_boolean,
        HPOA_FALSE,
        HPOA_TRUE)

OA_SOAP_ENUM(presence,
        PRESENCE_NO_OP,
        PRESENCE_UNKNOWN,
        ABSENT,
        PRESENT,
        SUBSUMED)

OA_SOAP_ENUM(bladeType,
        BLADE_TYPE_NO_OP,
        BLADE_TYPE_UNKNOWN,
        BLADE_TYPE_SERVER,
        BLADE_TYPE_STORAGE,
        BLADE_TYPE_WORKSTATION,
        BLADE_TYPE_IO)

OA_SOAP_ENUM(power,
        POWER_NO_OP,
        POWER_UNKNOWN,
        POWER_ON,
        POWER_OFF,
        POWER_STAGED_OFF,
        POWER_REBOOT)

OA_SOAP_ENUM(powerState,
        PS_NO_OP,
        PS_UNKNOWN,
        PS_OFF,
        PS_LOW,
        PS_AUTOMATIC,
        PS_MAXIMUM)

OA_SOAP_ENUM(shutdown,
        SHUTDOWN_NO_OP,
        SHUTDOWN_UNKNOWN,
        SHUTDOWN_OK,
        SHUTDOWN_SHUTDOWN,
        SHUTDOWN_THERMAL,
        SHUTDOWN_FAN,
        SHUTDOWN_RESTART)

OA_SOAP_ENUM(uidStatus,
        UID_NO_OP,
        UID_UNKNOWN,
        UID_ON,
        UID_OFF,
        UID_BLINK,
        UID_DEMONSTRATION)

/* Thank-you, OA team!  One of the enum names here includes a '-' character,
 * which is illegal in an enum name.  We need to keep the enum name legal in
 * C, yet parse a '-' character in the ASCII representations of opStatus.
 * If any changes are made to these values, make sure to keep both version
 * the same!
 */
enum opStatus {
        OP_STATUS_UNKNOWN,
        OP_STATUS_OTHER,
        OP_STATUS_OK,
        OP_STATUS_DEGRADED,
        OP_STATUS_STRESSED,
        OP_STATUS_PREDICTIVE_FAILURE,
        OP_STATUS_ERROR,
        OP_STATUS_NON_RECOVERABLE_ERROR,
        OP_STATUS_STARTING,
        OP_STATUS_STOPPING,
        OP_STATUS_STOPPED,
        OP_STATUS_IN_SERVICE,
        OP_STATUS_NO_CONTACT,
        OP_STATUS_LOST_COMMUNICATION,
        OP_STATUS_ABORTED,
        OP_STATUS_DORMANT,
        OP_STATUS_SUPPORTING_ENTITY_IN_ERROR,
        OP_STATUS_COMPLETED,
        OP_STATUS_POWER_MODE,
        OP_STATUS_DMTF_RESERVED,
        OP_STATUS_VENDER_RESERVED};
OA_SOAP_ENUM_STRING(opStatus,
        OP_STATUS_UNKNOWN,
        OP_STATUS_OTHER,
        OP_STATUS_OK,
        OP_STATUS_DEGRADED,
        OP_STATUS_STRESSED,
        OP_STATUS_PREDICTIVE_FAILURE,
        OP_STATUS_ERROR,
        OP_STATUS_NON-RECOVERABLE_ERROR,
        OP_STATUS_STARTING,
        OP_STATUS_STOPPING,
        OP_STATUS_STOPPED,
        OP_STATUS_IN_SERVICE,
        OP_STATUS_NO_CONTACT,
        OP_STATUS_LOST_COMMUNICATION,
        OP_STATUS_ABORTED,
        OP_STATUS_DORMANT,
        OP_STATUS_SUPPORTING_ENTITY_IN_ERROR,
        OP_STATUS_COMPLETED,
        OP_STATUS_POWER_MODE,
        OP_STATUS_DMTF_RESERVED,
        OP_STATUS_VENDER_RESERVED)

OA_SOAP_ENUM(sensorStatus,
        SENSOR_STATUS_NO_OP,
        SENSOR_STATUS_UNKNOWN,
        SENSOR_STATUS_OK,
        SENSOR_STATUS_WARM,
        SENSOR_STATUS_CAUTION,
        SENSOR_STATUS_CRITICAL)

OA_SOAP_ENUM(diagnosticStatus,
        NOT_RELEVANT,
        DIAGNOSTIC_CHECK_NOT_PERFORMED,
        NO_ERROR,
        ERROR)

OA_SOAP_ENUM(oaRole,
        OA_ABSENT,
        STANDBY,
        TRANSITION,
        ACTIVE)

OA_SOAP_ENUM(wizardStatus,
        WIZARD_NOT_COMPLETED,
        LCD_WIZARD_COMPLETE,
        WIZARD_SETUP_COMPLETE)

OA_SOAP_ENUM(portMapStatus,
        UNKNOWN,
        OK,
        MISMATCH)

OA_SOAP_ENUM(bladeSizeType,
        BLADE_SIZE_TYPE_MT,
        BLADE_SIZE_TYPE_1X1,
        BLADE_SIZE_TYPE_1X2)

OA_SOAP_ENUM(bladeMezzSlotType,
        MEZZ_SLOT_TYPE_MT,
        MEZZ_SLOT_TYPE_ONE,
        MEZZ_SLOT_TYPE_TWO,
        MEZZ_SLOT_TYPE_FIXED)

OA_SOAP_ENUM(bladeMezzDevType,
        MEZZ_DEV_TYPE_MT,
        MEZZ_DEV_TYPE_ONE,
        MEZZ_DEV_TYPE_TWO,
        MEZZ_DEV_TYPE_FIXED)

OA_SOAP_ENUM(bladeMezzDevStatus,
        MEZZ_DEV_STATUS_UNKNOWN,
        MEZZ_DEV_STATUS_OK,
        MEZZ_DEV_STATUS_MISMATCH)

OA_SOAP_ENUM(fabricType,
        FABRIC_TYPE_MT,
        FABRIC_TYPE_ETH,
        FABRIC_TYPE_FIB,
        FABRIC_TYPE_10GETH,
        FABRIC_TYPE_IFB,
        FABRIC_TYPE_PCI,
        FABRIC_TYPE_SAS,
        FABRIC_TYPE_MAX)

OA_SOAP_ENUM(fabricStatus,
        FABRIC_STATUS_UNKNOWN,
        FABRIC_STATUS_OK,
        FABRIC_STATUS_MISMATCH)

OA_SOAP_ENUM(interconnectTrayType,
        INTERCONNECT_TRAY_TYPE_NO_CONNECTION,
        INTERCONNECT_TRAY_TYPE_NIC,
        INTERCONNECT_TRAY_TYPE_FC,
        INTERCONNECT_TRAY_TYPE_10GETH,
        INTERCONNECT_TRAY_TYPE_IB,
        INTERCONNECT_TRAY_TYPE_PCIE,
        INTERCONNECT_TRAY_TYPE_SAS,
        INTERCONNECT_TRAY_TYPE_MAX)

enum interconnectTraySizeType {
        INTERCONNECT_TRAY_SIZE_TYPE_MT,
        INTERCONNECT_TRAY_SIZE_TYPE_0X1,
        INTERCONNECT_TRAY_SIZE_TYPE_1X1,
        INTERCONNECT_TRAY_SIZE_TYPE_2x1,
        INTERCONNECT_TRAY_SIZE_TYPE_3x1};

OA_SOAP_ENUM_STRING(interconnectTraySizeType,
        INTERCONNECT_TRAY_SIZE_TYPE_MT,
	INTERCONNECT_TRAY_SIZE_TYPE-1X1,
        INTERCONNECT_TRAY_SIZE_TYPE_1X1,
        INTERCONNECT_TRAY_SIZE_TYPE_2X1,
        INTERCONNECT_TRAY_SIZE_TYPE-2x1)

OA_SOAP_ENUM(interconnectTrayPassThroughEnabled,
        INTERCONNECT_TRAY_PASSTHROUGH_UNKNOWN,
        INTERCONNECT_TRAY_PASSTHROUGH_DISABLED,
        INTERCONNECT_TRAY_PASSTHROUGH_ENABLED)

OA_SOAP_ENUM(interconnectTrayPortStatus,
        INTERCONNECT_TRAY_PORT_STATUS_UNKNOWN,
        INTERCONNECT_TRAY_PORT_STATUS_OK,
        INTERCONNECT_TRAY_PORT_STATUS_MISMATCH)

OA_SOAP_ENUM(interconnectTrayPortEnabled,
        INTERCONNECT_TRAY_PORT_ENABLED_UNKNOWN,
        INTERCONNECT_TRAY_PORT_DISABLED,
        INTERCONNECT_TRAY_PORT_ENABLED)

OA_SOAP_ENUM(interconnectTrayPortUidStatus,
        INTERCONNECT_TRAY_UID_UNKNOWN,
        INTERCONNECT_TRAY_UID_OFF,
        INTERCONNECT_TRAY_UID_ON)

OA_SOAP_ENUM(interconnectTrayPortLinkLedStatus,
        INTERCONNECT_TRAY_LINK_LED_UNKNOWN,
        INTERCONNECT_TRAY_LINK_LED_OFF,
        INTERCONNECT_TRAY_LINK_LED_ON)

OA_SOAP_ENUM(powerSystemType,
        SUBSYSTEM_NO_OP,
        SUBSYSTEM_UNKNOWN,
        INTERNAL_AC,
        INTERNAL_DC,
        EXTERNAL_DC)

OA_SOAP_ENUM(redundancy,
        REDUNDANCY_NO_OP,
        REDUNDANCY_UNKNOWN,
        NOT_REDUNDANT,
        REDUNDANT)

#define POWER_REDUNDANCY_LENGTH 42      /* Max length of these enums + 1 */
OA_SOAP_ENUM(powerRedundancy,
        REDUNDANT_UNKNOWN,
        NON_REDUNDANT,
        AC_REDUNDANT,
        POWER_SUPPLY_REDUNDANT,
        AC_REDUNDANT_WITH_POWER_CEILING,
        POWER_SUPPLY_REDUNDANT_WITH_POWER_CEILING,
        NON_REDUNDANT_WITH_POWER_CEILING)

#define POWER_LIMIT_MODE_LENGTH 19      /* Max length of these enums + 1 */
OA_SOAP_ENUM(powerLimitMode,
        POWER_LIMIT_NONE,
        STATIC_POWER_LIMIT,
        DYNAMIC_POWER_CAP)

#define SENSOR_TYPE_LENGTH      25      /* Max length of these enums + 1 */
OA_SOAP_ENUM(sensorType,
        SENSOR_TYPE_BLADE,
        SENSOR_TYPE_INTERCONNECT,
        SENSOR_TYPE_OA,
        SENSOR_TYPE_ENC)

OA_SOAP_ENUM(userAcl,
        ADMINISTRATOR,
        OPERATOR,
        USER,
        ANONYMOUS)

OA_SOAP_ENUM(lcdButton,
        LCD_OK,
        LCD_UP,
        LCD_DOWN,
        LCD_RIGHT,
        LCD_LEFT,
        LCD_USERNOTES)

OA_SOAP_ENUM(lcdButtonState,
        CLICKED,
        PRESSED,
        RELEASED)

OA_SOAP_ENUM(lcdSetupHealth,
        LCD_SETUP_HEALTH_UNKNOWN,
        LCD_SETUP_HEALTH_OK,
        LCD_SETUP_HEALTH_INFORMATIONAL,
        LCD_SETUP_HEALTH_DEGRADED,
        LCD_SETUP_HEALTH_FAILED)

OA_SOAP_ENUM(lcdChatMessageType,
        STATEMENT,
        QUESTION,
        ANSWER,
        QUESTION_DISMISSED)

OA_SOAP_ENUM(hpSimTrustMode,
        HPSIM_DISABLED,
        TRUST_BY_NAME,
        TRUST_BY_CERTIFICATE,
        TRUST_ALL)

OA_SOAP_ENUM(iplDevice,
        IPL_NO_OP,
        CD,
        FLOPPY,
        HDD,
        USB,
        PXE_NIC1,
        PXE_NIC2,
        PXE_NIC3,
        PXE_NIC4)

OA_SOAP_ENUM(oneTimeBootDevice,
        ONE_TIME_BOOT_NO_CHANGE,
        ONE_TIME_BOOT_FLOPPY,
        ONE_TIME_BOOT_CD,
        ONE_TIME_BOOT_HARD_DRIVE,
        ONE_TIME_BOOT_TAPE)

OA_SOAP_ENUM(oneTimeBootAgent,
        NORMAL_BOOT_OS,
        SYS_PART,
        QUICK_DIAGS,
        RBSU,
        PXE)

OA_SOAP_ENUM(powerReductionState,
        SPRS_NO_OP,
        SPRS_UNKNOWN,
        SPRS_FIRED,
        SPRS_RESTORED)

OA_SOAP_ENUM(powerReductionArmedState,
        SPRAS_NO_OP,
        SPRAS_UNKNOWN,
        SPRAS_DISARMED,
        SPRAS_ARMED)

OA_SOAP_ENUM(virtualMediaSupport,
        VM_SUPPORT_UNKNOWN,
        VM_DEV_ABSENT,
        VM_BAY_SUBSUMED,
        VM_SUPPORTED,
        VM_NOT_SUPPORTED,
        VM_FIRMWARE_UPDATE_NEEDED)

OA_SOAP_ENUM(virtualMediaDeviceStatus,
        VM_DEV_STATUS_UNKNOWN,
        VM_DEV_STATUS_DISCONNECTED,
        VM_DEV_STATUS_CONNECTED,
        VM_DEV_STATUS_DISCONNECTING,
        VM_DEV_STATUS_CONNECTING)

#define POWER_CONTROL_LENGTH    16      /* Max length of these enums + 1 */
OA_SOAP_ENUM(powerControl,
        MOMENTARY_PRESS,
        PRESS_AND_HOLD,
        COLD_BOOT,
        RESET)

#define UID_CONTROL_LENGTH    15      /* Max length of these enums + 1 */
OA_SOAP_ENUM(uidControl,
        UID_CMD_TOGGLE,
        UID_CMD_ON,
        UID_CMD_OFF,
        UID_CMD_BLINK)

OA_SOAP_ENUM(eventType,
        EVENT_HEARTBEAT,
        EVENT_ENC_STATUS,
        EVENT_ENC_UID,
        EVENT_ENC_SHUTDOWN,
        EVENT_ENC_INFO,
        EVENT_ENC_NAMES,
        EVENT_USER_PERMISSION,
        EVENT_ADMIN_RIGHTS_CHANGED,
        EVENT_ENC_SHUTDOWN_PENDING,
        EVENT_ENC_TOPOLOGY,
        EVENT_FAN_STATUS,
        EVENT_FAN_INSERTED,
        EVENT_FAN_REMOVED,
        EVENT_FAN_GROUP_STATUS,
        EVENT_THERMAL_STATUS,
        EVENT_COOLING_STATUS,
        EVENT_FAN_ZONE_STATUS,
        EVENT_PS_STATUS,
        EVENT_PS_INSERTED,
        EVENT_PS_REMOVED,
        EVENT_PS_REDUNDANT,
        EVENT_PS_OVERLOAD,
        EVENT_AC_FAILURE,
        EVENT_PS_INFO,
        EVENT_PS_SUBSYSTEM_STATUS,
        EVENT_SERVER_POWER_REDUCTION_STATUS,
        EVENT_INTERCONNECT_STATUS,
        EVENT_INTERCONNECT_RESET,
        EVENT_INTERCONNECT_UID,
        EVENT_INTERCONNECT_INSERTED,
        EVENT_INTERCONNECT_REMOVED,
        EVENT_INTERCONNECT_INFO,
        EVENT_INTERCONNECT_HEALTH_LED,
        EVENT_INTERCONNECT_THERMAL,
        EVENT_INTERCONNECT_CPUFAULT,
        EVENT_INTERCONNECT_POWER,
        EVENT_INTERCONNECT_PORTMAP,
        EVENT_BLADE_PORTMAP,
        EVENT_INTERCONNECT_VENDOR_BLOCK,
        EVENT_INTERCONNECT_HEALTH_STATE,
        EVENT_DEMO_MODE,
        EVENT_BLADE_STATUS,
        EVENT_BLADE_INSERTED,
        EVENT_BLADE_REMOVED,
        EVENT_BLADE_POWER_STATE,
        EVENT_BLADE_POWER_MGMT,
        EVENT_BLADE_UID,
        EVENT_BLADE_SHUTDOWN,
        EVENT_BLADE_FAULT,
        EVENT_BLADE_THERMAL,
        EVENT_BLADE_INFO,
        EVENT_BLADE_MP_INFO,
        EVENT_ILO_READY,
        EVENT_LCD_BUTTON,
        EVENT_KEYING_ERROR,
        EVENT_ILO_HAS_IPADDRESS,
        EVENT_POWER_INFO,
        EVENT_LCD_STATUS,
        EVENT_LCD_INFO,
        EVENT_REDUNDANCY,
        EVENT_ILO_DEAD,
        EVENT_RACK_SERVICE_STARTED,
        EVENT_LCD_SCREEN_REFRESH,
        EVENT_ILO_ALIVE,
        EVENT_PERSONALITY_CHECK,
        EVENT_BLADE_POST_COMPLETE,
        EVENT_BLADE_SIGNATURE_CHANGED,
        EVENT_BLADE_PERSONALITY_CHANGED,
        EVENT_BLADE_TOO_LOW_POWER,
        EVENT_VIRTUAL_MEDIA_STATUS,
        EVENT_MEDIA_DRIVE_INSERTED,
        EVENT_MEDIA_DRIVE_REMOVED,
        EVENT_MEDIA_INSERTED,
        EVENT_MEDIA_REMOVED,
        EVENT_OA_NAMES,
        EVENT_OA_STATUS,
        EVENT_OA_UID,
        EVENT_OA_INSERTED,
        EVENT_OA_REMOVED,
        EVENT_OA_INFO,
        EVENT_OA_FAILOVER,
        EVENT_OA_TRANSITION_COMPLETE,
        EVENT_OA_VCM,
        EVENT_NETWORK_INFO_CHANGED,
        EVENT_SNMP_INFO_CHANGED,
        EVENT_SYSLOG_CLEARED,
        EVENT_SESSION_CLEARED,
        EVENT_TIME_CHANGE,
        EVENT_SESSION_STARTED,
        EVENT_BLADE_CONNECT,
        EVENT_BLADE_DISCONNECT,
        EVENT_SWITCH_CONNECT,
        EVENT_SWITCH_DISCONNECT,
        EVENT_BLADE_CLEARED,
        EVENT_SWITCH_CLEARED,
        EVENT_ALERTMAIL_INFO_CHANGED,
        EVENT_LDAP_INFO_CHANGED,
        EVENT_EBIPA_INFO_CHANGED,
        EVENT_HPSIM_TRUST_MODE_CHANGED,
        EVENT_HPSIM_CERTIFICATE_ADDED,
        EVENT_HPSIM_CERTIFICATE_REMOVED,
        EVENT_USER_INFO_CHANGED,
        EVENT_BAY_CHANGED,
        EVENT_GROUP_CHANGED,
        EVENT_OA_REBOOT,
        EVENT_OA_LOGOFF_REQUEST,
        EVENT_USER_ADDED,
        EVENT_USER_DELETED,
        EVENT_USER_ENABLED,
        EVENT_USER_DISABLED,
        EVENT_GROUP_ADDED,
        EVENT_GROUP_DELETED,
        EVENT_LDAPGROUP_ADDED,
        EVENT_LDAPGROUP_DELETED,
        EVENT_LDAPGROUP_ADMIN_RIGHTS_CHANGED,
        EVENT_LDAPGROUP_INFO_CHANGED,
        EVENT_LDAPGROUP_PERMISSION,
        EVENT_LCDPIN,
        EVENT_LCD_USER_NOTES_CHANGED,
        EVENT_LCD_BUTTONS_LOCKED,
        EVENT_LCD_SCREEN_CHAT_REQUESTED,
        EVENT_LCD_SCREEN_CHAT_WITHDRAWN,
        EVENT_LCD_SCREEN_CHAT_ANSWERED,
        EVENT_LCD_USER_NOTES_IMAGE_CHANGED,
        EVENT_ENC_WIZARD_STATUS,
        EVENT_SSHKEYS_INSTALLED,
        EVENT_SSHKEYS_CLEARED,
        EVENT_LDAP_DIRECTORY_SERVER_CERTIFICATE_ADDED,
        EVENT_LDAP_DIRECTORY_SERVER_CERTIFICATE_REMOVED,
        EVENT_BLADE_BOOT_CONFIG,
        EVENT_OA_NETWORK_CONFIG_CHANGED,
        EVENT_HPSIM_XENAME_ADDED,
        EVENT_HPSIM_XENAME_REMOVED,
        EVENT_FLASH_PENDING,
        EVENT_FLASH_STARTED,
        EVENT_FLASH_PROGRESS,
        EVENT_FLASH_COMPLETE,
        EVENT_STANDBY_FLASH_STARTED,
        EVENT_STANDBY_FLASH_PROGRESS,
        EVENT_STANDBY_FLASH_COMPLETE,
        EVENT_STANDBY_FLASH_BOOTING,
        EVENT_STANDBY_FLASH_BOOTED,
        EVENT_STANDBY_FLASH_FAILED,
        EVENT_FLASHSYNC_BUILD,
        EVENT_FLASHSYNC_BUILDDONE,
        EVENT_FLASHSYNC_FAILED,
        EVENT_FLASHSYNC_STANDBY_BUILD,
        EVENT_FLASHSYNC_STANDBY_BUILDDONE,
        EVENT_FLASHSYNC_STANDBY_FAILED,
        EVENT_NONILO_EBIPA,
        EVENT_FACTORY_RESET,
        EVENT_BLADE_INSERT_COMPLETED,
        EVENT_EBIPA_INFO_CHANGED_EX,
        EVENT_BLADE_ESI_CHANGED,
        EVENT_ENC_TOPOLOGY_2,
        EVENT_TFA_CA_CERT_ADDED,
        EVENT_TFA_CA_CERT_REMOVED,
        EVENT_USER_CERT_ADDED,
        EVENT_USER_CERT_REMOVED,
        EVENT_PW_SETTINGS_CHANGED,
        EVENT_SYSLOG_SETTINGS_CHANGED,
        EVENT_POWERDELAY_SETTINGS_CHANGED,
        EVENT_USB_OA_FW_FILES,
        EVENT_USB_OA_CONFIG_SCRIPTS,
        EVENT_MEDIA_DRIVE_INSERTED2,
        EVENT_MEDIA_DRIVE_REMOVED2,
        EVENT_MEDIA_INSERTED2,
        EVENT_MEDIA_REMOVED2,
        EVENT_ENC_GRP_CAP,
        EVENT_BLADE_GRP_CAP_TIMEOUT,
        EVENT_VLAN_INFO_CHANGED,
        EVENT_SESSION_TIMEOUT_CHANGED,
        EVENT_SBL_DOMAIN_INFO_CHANGED,
        EVENT_FW_MGMT_SETTINGS_CHANGED,
        EVENT_FW_DISCOVERY,
        EVENT_FW_UPDATE,
        EVENT_NET_SERVICE_RESTART,
        EVENT_PS_OVERLOAD_REPAIRED,
        EVENT_VCM_IPV6_URL_CHANGED,
        EVENT_VCM_MIN_OA_FW_VER_CHANGED,
        EVENT_SNMPV3_INFO_CHANGED,
        EVENT_OA_ERS_CONFIG_CHANGED,
        EVENT_OA_ERS_TEST_MANUAL,
        EVENT_OA_ERS_DATACOLLECTION_MANUAL,
        EVENT_OA_ERS_DATACOLLECTION_SUCCESS,
        EVENT_OA_ERS_DATACOLLECTION_FAILURE,
        EVENT_OA_ERS_MAINTENANCE_SET,
        EVENT_OA_ERS_MAINTENANCE_CLEARED,
        EVENT_OA_ERS_STATUS,
        EVENT_OA_LOGIN_BANNER_SETTINGS_CHANGED,
        EVENT_OA_ERS_EVENTS_CLEARED,
        EVENT_ERS_CA_CERT_ADDED,
        EVENT_ERS_CA_CERT_REMOVED,
        EVENT_FW_MGMT_LOG_CLEARED,
        EVENT_FW_MGMT_ISO_STATUS,
        EVENT_LANG_PACK_ADDED,
        EVENT_LANG_PACK_REMOVED,
        EVENT_TCP_TIMEOUT_CHANGED,
        EVENT_BLADE_FQDN_INFO_REFRESH,
        EVENT_TRAY_FQDN_INFO_REFRESH,
        EVENT_VCM_FQDN_INFO_REFRESH,
        EVENT_EBIPAV6_INFO_CHANGED_EX)

/* This is not part of the SOAP response data from the OA, but is useful
 * for identifying the type of data that comes back from getAllEvents().
 */
OA_SOAP_ENUM(enum_eventInfo,
        SYSLOG,
        RACKTOPOLOGY,
        ENCLOSURESTATUS,
        ENCLOSUREINFO,
        OASTATUS,
        OAINFO,
        BLADEINFO,
        BLADEMPINFO,
        BLADESTATUS,
        BLADEPORTMAP,
        FANINFO,
        INTERCONNECTTRAYSTATUS,
        INTERCONNECTTRAYINFO,
        INTERCONNECTTRAYPORTMAP,
        POWERSUPPLYINFO,
        POWERSUPPLYSTATUS,
        POWERSUBSYSTEMINFO,
        POWERCONFIGINFO,
        THERMALINFO,
        USERINFOARRAY,
        USERINFO,
        LDAPINFO,
        LDAPGROUPINFO,
        SNMPINFO,
        ENCLOSURENETWORKINFO,
        OANETWORKINFO,
        ENCLOSURETIME,
        ALERTMAILINFO,
        PASSWORDSETTINGS,
        EBIPAINFO,
        LCDCHATMESSAGE,
        LCDUSERNOTES,
        LCDBUTTONEVENT,
        LCDSTATUS,
        LCDINFO,
        HPSIMINFO,
        THERMALSUBSYSTEMINFO,
        BLADEBOOTINFO,
        OAVCMMODE,
        POWERREDUCTIONSTATUS,
        VIRTUALMEDIASTATUS,
        OAMEDIADEVICE,
        FANZONE,
        EBIPAINFOEX,
        CACERTSINFO,
        RACKTOPOLOGY2,
        USERCERTIFICATEINFO,
        SYSLOGSETTINGS,
        POWERDELAYSETTINGS,
        USBMEDIAFIRMWAREIMAGES,
        CONFIGSCRIPTS,
        NUMVALUE,
        STRING,
        MESSAGE,
        NOPAYLOAD,
        POWERCAPCONFIG)

OA_SOAP_ENUM(enum_usbMode,
        USB_KVM_ENABLED,
        USB_DVD_ENABLED)

OA_SOAP_ENUM(enum_networkProtocol,
        NET_PROTO_SNMP,
        NET_PROTO_SSH,
        NET_PROTO_TELNET,
        NET_PROTO_HTTP,
        NET_PROTO_NTP,
        NET_PROTO_IPSECURITY,
        NET_PROTO_ALERTMAIL,
        NET_PROTO_EBIPA_SVB,
        NET_PROTO_EBIPA_SWM,
        NET_PROTO_XMLREPLY,
        NET_PROTO_DYNDNS,
        NET_PROTO_LLF,
        NET_PROTO_IPSWAP)

OA_SOAP_ENUM(enum_nicSpeed,
        NIC_SPEED_10,
        NIC_SPEED_100,
        NIC_SPEED_1000,
        NIC_SPEED_10000)

OA_SOAP_ENUM(enum_nicDuplex,
        NIC_DUPLEX_HALF,
        NIC_DUPLEX_FULL)

OA_SOAP_ENUM(enum_fileType,
        FIRMWARE_IMAGE,
        LCD_IMAGE,
        CONFIG_SCRIPT,
        SSH_KEYS_FILE,
        SSL_CERTIFICATE,
        LDAP_DIRECTORY_SERVER_CERTIFICATE,
        HPSIM_CERTIFICATE,
        FIRMWARE_INTERNAL_IMAGE,
        PROLIANT_MP_IMAGE)

/* Structures that return information from OA SOAP calls */
struct bladeCpuInfo
{
        char *cpuType;
        int cpuSpeed;
};

struct bladeNicInfo
{
        char *port;
        char *macAddress;
};

struct extraDataInfo
{
        char *name;
        char *value;
};

struct bladeInfo
{
        byte bayNumber;
        enum presence presence;
        enum bladeType bladeType;
        byte width;
        byte height;
        char *name;
        char *manufacturer;
        char *partNumber;
        char *sparePartNumber;
        char *serialNumber;
        char *serverName;
        char *uuid;
        char *rbsuOsName;
        char *assetTag;
        char *romVersion;
        byte numberOfCpus;
        xmlNode *cpus;
        int memory;
        byte numberOfNics;
        xmlNode *nics;
        short mmHeight;
        short mmWidth;
        short mmDepth;
        int deviceId;
        int productId;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

/* Structures that supply information to OA SOAP calls */
struct getBladeInfo
{
        int bayNumber;
};
/* Structures that supply information to OA SOAP calls */
struct getBladePortMap
{
        int bayNumber;
};

struct diagnosticChecks
{
        enum diagnosticStatus internalDataError;
        enum diagnosticStatus managementProcessorError;
        enum diagnosticStatus thermalWarning;
        enum diagnosticStatus thermalDanger;
        enum diagnosticStatus ioConfigurationError;
        enum diagnosticStatus devicePowerRequestError;
        enum diagnosticStatus insufficientCooling;
        enum diagnosticStatus deviceLocationError;
        enum diagnosticStatus deviceFailure;
        enum diagnosticStatus deviceDegraded;
        enum diagnosticStatus acFailure;
        enum diagnosticStatus i2cBuses;
        enum diagnosticStatus redundancy;
};

struct diagnosticData
{
        enum diagnosticStatus value;
        char *name;
};

struct bladeStatus
{
        byte bayNumber;
        enum presence presence;
        enum opStatus operationalStatus;
        enum sensorStatus thermal;
        enum power powered;
        enum powerState powerState;
        enum shutdown shutdown;
        enum uidStatus uid;
        int powerConsumed;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getBladeStatus
{
        int bayNumber;
};

struct syslog
{
        byte bayNumber;
        int syslogStrlen;
        char *logContents;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct encLink
{
        byte enclosureNumber;
        char *oaName;
        char *uuid;
        char *rackName;
        char *enclosureName;
        char *url;
        enum hpoa_boolean local;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct rackTopology
{
        char *ruid;
        xmlNode *enclosures;            /* Items are struct encLink */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct enclosureStatus
{
        enum opStatus operationalStatus;
        enum uidStatus uid;
        enum wizardStatus wizardStatus;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct enclosureInfo
{
        char *rackName;
        char *enclosureName;
        char *hwVersion;
        byte bladeBays;
        byte fanBays;
        byte powerSupplyBays;
        byte thermalSensors;
        byte interconnectTrayBays;
        byte oaBays;
        char *name;
        char *partNumber;
        char *serialNumber;
        char *uuid;
        char *assetTag;
        char *manufacturer;
        char *chassisSparePartNumber;
        char *interposerManufacturer;
        char *interposerName;
        char *interposerPartNumber;
        char *interposerSerialNumber;
        char *pduType;
        short mmHeight;
        short mmWidth;
        short mmDepth;
        char *pduPartNumber;
        char *pduSparePartNumber;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
        int powerType; 
        int enclosureStatus; 
};

struct oaStatus
{
        byte bayNumber;
        char *oaName;
        enum oaRole oaRole;
        enum opStatus operationalStatus;
        enum uidStatus uid;
        byte restartCause;
        enum hpoa_boolean oaRedundancy;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getOaStatus
{
        int bayNumber;
};

struct oaInfo
{
        byte bayNumber;
        enum hpoa_boolean youAreHere;
        char *name;
        char *partNumber;
        char *sparePartNumber;
        char *serialNumber;
        char *uuid;
        char *assetTag;
        char *manufacturer;
        char *hwVersion;
        char *fwVersion;
        short mmHeight;
        short mmWidth;
        short mmDepth;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct OaId
{
        byte bayNumber;	
};

struct getOaInfo
{
        int bayNumber;
};

struct getBladeMpInfo
{
        int bayNumber;
};

struct bladeMpInfo
{
        byte bayNumber;
        char *ipAddress;
        char *macAddress;
        char *dnsName;
        char *modelName;
        char *fwVersion;
        char *remoteConsoleUrl;
        char *webUrl;
        char *ircUrl;
        char *loginUrl;
        char *ircFullUrl;
        char *remoteSerialUrl;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeMezzSlotPort
{
        char *slotNumber;
        char *interconnectTrayBayNumber;
        char *interconnectTrayPortNumber;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeMezzSlotInfo
{
        enum bladeMezzSlotType type;
        int sizeslot;
        xmlNode *slot;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeMezzDevPort
{
        char *portNumber;
        char *wwpn;
        enum fabricType fabric;
        enum fabricStatus status;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeMezzDevInfo
{
        char *name;
        enum bladeMezzDevType type;
        enum bladeMezzDevStatus status;
        int sizeport;
        xmlNode *port;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeMezzInfo
{
        char *mezzNumber;
        xmlNode *mezzSlots;
        xmlNode *mezzDevices;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladePortMap
{
        char *bladeBayNumber;
        enum portMapStatus status;
        enum bladeSizeType bladeSizeType;
        char *numberOfMezzes;
        xmlNode *mezz;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct fanInfo
{
        byte bayNumber;
        enum presence presence;
        char *name;
        char *partNumber;
        char *sparePartNumber;
        char *serialNumber;
        int powerConsumed;
        int fanSpeed;
        int maxFanSpeed;
        int lowLimitFanSpeed;
        enum opStatus operationalStatus;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getFanInfo
{
        int bayNumber;
};

struct portEnabled
{
        byte portNumber;
        enum hpoa_boolean enabled;
};

struct interconnectTrayStatus
{
        byte bayNumber;
        enum opStatus operationalStatus;
        enum presence presence;
        enum sensorStatus thermal;
        enum hpoa_boolean cpuFault;
        enum hpoa_boolean healthLed;
        enum uidStatus uid;
        enum power powered;
        xmlNode *ports;                 /* Items are struct portEnabled */
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getInterconnectTrayStatus
{
        int bayNumber;
};

struct interconnectTrayInfo
{
        byte bayNumber;
        enum interconnectTrayType interconnectTrayType;
        enum hpoa_boolean passThroughSupport;
        enum hpoa_boolean portDisableSupport;
        enum hpoa_boolean temperatureSensorSupport;
        byte width;
        char *manufacturer;
        char *name;
        char *partNumber;
        char *serialNumber;
        char *sparePartNumber;
        enum hpoa_boolean rs232PortRoute;
        enum hpoa_boolean ethernetPortRoute;
        char *userAssignedName;
        char *inBandIpAddress;
        char *urlToMgmt;
        int powerOnWatts;
        int powerOffWatts;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getInterconnectTrayInfo
{
        int bayNumber;
};

struct interconnectTrayPortInfo
{
        byte interconnectTraySlotPortNumber;
        byte bladeBayNumber;
        byte bladeMezzNumber;
        byte bladeMezzPortNumber;
        enum interconnectTrayPortStatus portStatus;
        enum interconnectTrayPortEnabled portEnabled;
        enum interconnectTrayPortUidStatus portUidStatus;
        enum interconnectTrayPortLinkLedStatus portLinkLedStatus;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct interconnectTraySlotInfo
{
        byte interconnectTraySlotNumber;
        enum interconnectTrayType type;
        int sizeport;
        xmlNode *port;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct interconnectTrayPortMap
{
        byte interconnectTrayBayNumber;
        enum portMapStatus status;
        enum interconnectTraySizeType sizeType;
        enum interconnectTrayPassThroughEnabled passThroughModeEnabled;
        byte numberOfSlots;
        int sizeslot;
        xmlNode *slot;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct powerSupplyInfo
{
        byte bayNumber;
        enum presence presence;
        char modelNumber[MAX_MODEL_NUM_LENGTH];
        char sparePartNumber[MAX_PART_NUM_LENGTH];
        char productName[MAX_PRODUCT_NAME_LENGTH];
        char serialNumber[MAX_SERIAL_NUM_LENGTH];
        int capacity;
        int actualOutput;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getPowerSupplyInfo
{
        int bayNumber;
};

struct getPowerSupplyStatus
{
        int bayNumber;
};

struct powerSupplyStatus
{
        byte bayNumber;
        enum presence presence;
        enum opStatus operationalStatus;
        enum opStatus inputStatus;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct powerSubsystemInfo
{
        enum powerSystemType subsystemType;
        enum opStatus operationalStatus;
        enum redundancy redundancy;
        enum powerRedundancy redundancyMode;
        int capacity;
        int redundantCapacity;
        int outputPower;
        int powerConsumed;
        float inputPowerVa;
        float inputPowerCapacityVa;
        float inputPower;
        float inputPowerCapacity;
        byte goodPowerSupplies;
        byte wantedPowerSupplies;
        byte neededPowerSupplies;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct powerConfigInfo
{
        int powerCeiling;
        enum powerRedundancy redundancyMode;
        enum hpoa_boolean dynamicPowerSaverEnabled;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
        /* These are needed as high/low limits of the analog control RDRs */
        /* static power limit.                                            */
        int ACLimitLow;
        int ACLimitHigh;
};

struct powerCapConfig
{
        int enclosureMinWattageMeasured;
        int enclosureMaxWattageMeasured;
        int enclosurePowerCapLowerBound;
        int enclosurePowerCapUpperBound;
        enum hpoa_boolean enclosureHighLine;
        int enclosureAcPhaseType;
        int enclosureEstimatedVoltage;
        int powerCap;
        /* optOutBayArray holds true or false for each bay */
        char optOutBayArray[OA_SOAP_C7000_MAX_BLADE][8];
        /* Items are struct extraDataInfo */
        xmlNode *extraData;
        /* The following are needed to hold values associated with */
        /* the derated and rated circuit caps.                     */
        int deratedCircuitCap;
        int ratedCircuitCap;
        int deratedCircuitCapLowerBound;
        int deratedCircuitCapUpperBound;
        int ratedCircuitCapLowerBound;
        int ratedCircuitCapUpperBound;
};

struct thermalInfo
{
        enum sensorType sensorType;
        byte bayNumber;
        enum sensorStatus sensorStatus;
        enum opStatus operationalStatus;
        byte temperatureC;
        byte cautionThreshold;
        byte criticalThreshold;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getThermalInfo
{
        enum sensorType sensorType;
        int bayNumber;
};

struct bayAccess
{
        byte bayNumber;
        enum hpoa_boolean access;
};

struct enclosureBaysSelection
{
        enum hpoa_boolean oaAccess;
        xmlNode *bladeBays;             /* Items are struct bayAccess */
        xmlNode *interconnectTrayBays;  /* Items are struct bayAccess */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct userInfo
{
        char *username;
        char *fullname;
        char *contactInfo;
        enum hpoa_boolean isEnabled;
        enum userAcl acl;
        struct enclosureBaysSelection bayPermissions;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getUserInfo
{
        char *username;
};

struct x509CertificateInfo
{
        byte certificateVersion;
        char *issuerOrganization;
        char *issuerOrganizationalUnit;
        char *issuerCommonName;
        char *subjectOrganization;
        char *subjectOrganizationalUnit;
        char *subjectCommonName;
        time_t validFrom;
        time_t validTo;
        char *serialNumber;
        int extensionCount;
        char *md5Fingerprint;
        char *sha1Fingerprint;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct certificates
{
        int numberOfCertificates;
        int sizecertificate;
        xmlNode *certificate;   /* Items are struct x509CertificateInfo */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct ldapInfo
{
        enum hpoa_boolean ldapEnabled;
        enum hpoa_boolean localUsersEnabled;
        char *directoryServerAddress;
        short directoryServerSslPort;
        char *searchContext1;
        char *searchContext2;
        char *searchContext3;
        enum hpoa_boolean userNtAccountNameMapping;
        struct certificates certificates;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct ldapGroupInfo
{
        char *ldapGroupName;
        char *description;
        enum userAcl acl;
        struct enclosureBaysSelection bayPermissions;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct trapInfo
{
        char *ipAddress;
        char *community;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct snmpInfo
{
        char *sysName;
        char *sysLocation;
        char *sysContact;
        char *roCommunity;
        char *rwCommunity;
        int numTraps;
        xmlNode *traps;                 /* Items are struct trapInfo */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct enclosureNetworkInfo
{
        xmlNode *ntpServers;            /* Items are char *ipAddress */
        int ntpPoll;
        xmlNode *ipAllow;               /* Items are char *ipAddress */
        enum hpoa_boolean httpsEnabled;
        enum hpoa_boolean snmpEnabled;
        enum hpoa_boolean sshEnabled;
        enum hpoa_boolean telnetEnabled;
        enum hpoa_boolean ntpEnabled;
        enum hpoa_boolean ipSecurityEnabled;
        enum hpoa_boolean alertmailEnabled;
        enum hpoa_boolean ebipaSvbEnabled;
        enum hpoa_boolean ebipaSwmEnabled;
        enum hpoa_boolean xmlReplyEnabled;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct oaNetworkInfo
{
        byte bayNumber;
        enum hpoa_boolean dhcpEnabled;
        enum hpoa_boolean dynDnsEnabled;
        char *macAddress;
        char *ipAddress;
        char ipv6Address0[255];
        char ipv6Address1[255];
        char ipv6Address2[255];
        char ipv6Address3[255];
        char *ipv6AddressType0;
        char *ipv6AddressType1;
        char *ipv6AddressType2;
        char *ipv6AddressType3;
        char *netmask;
        char *gateway;
        xmlNode *dns;                   /* Items are char *ipAddress */
        char *elinkIpAddress;
        enum hpoa_boolean linkActive;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getOaNetworkInfo
{
        int bayNumber;
};

struct enclosureTime
{
        time_t dateTime;
        char *timeZone;
        time_t universalDateTime;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct alertmailInfo
{
        char *server;
        char *receiver;
        char *domain;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct ebipaInfo
{
        char *svbIpAddress;
        char *svbNetmask;
        char *svbGateway;
        char *svbDomain;
        xmlNode *svbDns;                /* Items are char *ipAddress */
        xmlNode *svbNtpServer;          /* Items are char *ipAddress */
        char *swmIpAddress;
        char *swmNetmask;
        char *swmGateway;
        char *swmDomain;
        xmlNode *swmDns;                /* Items are char *ipAddress */
        xmlNode *swmNtpServer;          /* Items are char *ipAddress */
        enum hpoa_boolean isConfiguredSvb;
        enum hpoa_boolean isConfiguredSwm;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct lcdChatMessage
{
        enum lcdChatMessageType lcdChatMessageType;
        char *screenName;
        char *questionText;
        char *answerChoiceList;
        char *selectedAnswerText;
        char *customAnswerText;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct lcdUserNotes
{
        char *lcdUserNotesLine1;
        char *lcdUserNotesLine2;
        char *lcdUserNotesLine3;
        char *lcdUserNotesLine4;
        char *lcdUserNotesLine5;
        char *lcdUserNotesLine6;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct lcdButtonEvent
{
        enum lcdButton button;
        enum lcdButtonState buttonState;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct lcdStatus
{
        enum opStatus status;
        enum uidStatus display;
        enum hpoa_boolean lcdPin;
        enum hpoa_boolean buttonLock;
        enum lcdSetupHealth lcdSetupHealth;
        struct diagnosticChecks diagnosticChecks;
        xmlNode *diagnosticChecksEx;    /* Items are struct diagnosticData */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct lcdInfo
{
        char *name;
        char *partNumber;
        char *manufacturer;
        char *fwVersion;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct hpSimInfo
{
        enum hpSimTrustMode trustMode;
        struct certificates certificates;
        xmlNode *xeNameList;            /* Items are char *xeName */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct thermalSubsystemInfo
{
        enum opStatus operationalStatus;
        enum redundancy redundancy;
        byte goodFans;
        byte wantedFans;
        byte neededFans;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct bladeIpl
{
        enum iplDevice iplDevice;
        byte bootPriority;
};

struct bladeBootInfo
{
        byte numberOfIpls;
        xmlNode *ipls;                  /* Items are struct bladeIpl */
        byte lastIplDeviceBooted;
        enum oneTimeBootDevice oneTimeBootDevice;
        enum oneTimeBootAgent oneTimeBootAgent;
        enum hpoa_boolean oneTimeBypassF1F2Messages;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct oaVcmMode
{
        enum hpoa_boolean isVcmMode;
        char *vcmUrl;
        char *vcmDomainId;
        char *vcmDomainName;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct powerReductionStatus
{
        enum powerReductionState powerReductionState;
        enum powerReductionArmedState powerReductionArmedState;
        enum powerReductionState powerReductionFiredState;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct virtualMediaStatus
{
        byte bayNumber;
        enum virtualMediaSupport support;
        enum virtualMediaDeviceStatus cdromStatus;
        char *cdromUrl;
        enum virtualMediaDeviceStatus floppyStatus;
        char *floppyUrl;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct fanZone
{
        byte zoneNumber;
        enum redundancy redundant;
        enum opStatus operationalStatus;
        int targetRpm;
        int targetPwm;
        xmlNode *deviceBayArray;        /* Items are byte bay */
        xmlNode *fanInfoArray;          /* Items are struct fanInfo */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct deviceBayArray {
        xmlNode *bay;                   /* Items are byte (bay) */
};

struct fanInfoArray{
        xmlNode *fanInfo;               /* Items are struct fanInfo */
};

struct ebipaBay
{
        int bayNumber;
        enum hpoa_boolean enabled;
        char *ipAddress;
};

struct ebipaInfoEx
{
        struct ebipaInfo info;
        xmlNode *deviceBays;                    /* Items are struct ebipaBay */
        xmlNode *interconnectBays;              /* Items are struct ebipaBay */
};

struct eventPid
{
        int pid;
};

struct unSubscribeForEvents
{
        int pid;
};

struct syslogSettings
{
        char *syslogServer;
        int syslogPort;
        enum hpoa_boolean remoteEnabled;
};

struct encLinkOa
{
        enum hpoa_boolean activeOa;
        int bayNumber;
        char *oaName;
        char *ipAddress;
        char *macAddress;
        char *fwVersion;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct encLink2
{
        int enclosureNumber;
        int productId;
        int mfgId;
        char *enclosureUuid;
        char *enclosureSerialNumber;
        char *enclosureName;
        char *enclosureProductName;
        enum opStatus enclosureStatus;
        char *enclosureRackIpAddress;
        char *enclosureUrl;
        char *rackName;
        enum hpoa_boolean primaryEnclosure;
        xmlNode *encLinkOa;             /* Items are struct encLinkOa */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct rackTopology2
{
        char *ruid;
        xmlNode *enclosures;            /* Items are struct encLink2 */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct passwordSettings
{
        enum hpoa_boolean strictPasswordsEnabled;
        int minPasswordLength;
};

struct oaMediaDevice
{
        int bayNumber;
        int deviceIndex;
        int deviceType;
        enum presence devicePresence;
        enum presence mediaPresence;
        char *volumeLabel;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct firmwareImage
{
        char *fileName;
        char *fwVersion;
};

struct usbMediaFirmwareImages
{
        xmlNode *image;                 /* Items are struct firmwareImage */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct usbMediaConfigScript
{
        char *fileName;
};

struct usbMediaConfigScripts
{
        xmlNode *usbMediaConfigScript;  /* Items are
                                         * struct usbMediaConfigScript
                                         */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct tfaSettings
{
        enum hpoa_boolean enableTwoFactor;
        enum hpoa_boolean enableCrl;
        enum hpoa_boolean subjectAltName;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct userCertificateInfo
{
        char *fingerprint;
};

struct caCertsInfo
{
        xmlNode *certificates;
};

struct powerdelayBay
{
        int bayNumber;
        int delay;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct powerdelaySettings
{
        enum hpoa_boolean isPowerdelayInProgress;
        xmlNode *interconnects;         /* Items are struct powerdelayBay */
        xmlNode *servers;               /* Items are struct powerdelayBay */
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getFanZoneArrayResponse
{
        xmlNode *fanZoneArray;          /* Items are struct fanZone */
};

struct getBladeInfoArrayResponse
{
        xmlNode *bladeInfoArray;       /* Items are struct bladeInfo */
};
struct interconnectTrayInfoArrayResponse
{
        xmlNode *interconnectTrayInfoArray;
};
struct interconnectTrayStsArrayResponse
{
        xmlNode *interconnectTrayStsArray;
};
struct interconnectTrayPmArrayResponse
{
        xmlNode *interconnectTrayPmArray;
};

struct getPowerSupplyInfoArrayResponse
{
        xmlNode *powerSupplyInfoArray;
};

struct getPowerSupplyStsArrayResponse
{
        xmlNode *powerSupplyStsArray;
};

struct getFanInfoArrayResponse
{
        xmlNode *fanInfoArray;
};

struct getOaInfoArrayResponse
{
        xmlNode *oaInfoArray;
};

struct getOaStatusArrayResponse
{
        xmlNode *oaStatusArray;
};

struct getBladePortMapArrayResponse
{
        xmlNode *portMapArray;
};

struct getBladeStsArrayResponse
{
        xmlNode *bladeStsArray;
};

struct bayArray
{
        int     size;                   /* Size of array */
        byte    *array;                 /* Actual array */
};

struct getFanZoneArray
{
        struct bayArray bayArray;
};

struct getBladeInfoArray
{
        struct bayArray bayArray;
};

struct getInterconnectTrayInfoArray
{
        struct bayArray bayArray;
};
struct interconnectTrayStsArray
{
        struct bayArray bayArray;
};
struct interconnectTrayPmArray
{
        struct bayArray bayArray;
};

struct getPowerSupplyInfoArray
{
        struct bayArray bayArray;
};

struct getPowerSupplyStsArray
{
        struct bayArray bayArray;
};

struct getFanInfoArray
{
        struct bayArray bayArray;
};
struct getOaInfoArray
{
        struct bayArray bayArray;
};
struct getOaStatusArray
{
        struct bayArray bayArray;
};
struct getBladePortMapArray
{
        struct bayArray bayArray;
};
struct getBladeStsArray
{
        struct bayArray bayArray;
};

struct getEvent
{
        int pid;
        enum hpoa_boolean waitTilEventHappens;
        enum hpoa_boolean lcdEvents;
};

struct getAllEventsEx
{
        int pid;
        enum hpoa_boolean waitTilEventHappens;
        enum hpoa_boolean lcdEvents;
        char *oaFwVersion;
};

union _hpoa__data
{
        struct syslog syslog;
        struct rackTopology rackTopology;
        struct enclosureStatus enclosureStatus;
        struct enclosureInfo enclosureInfo;
        struct oaStatus oaStatus;
        struct oaInfo oaInfo;
        struct bladeInfo bladeInfo;
        struct bladeMpInfo bladeMpInfo;
        struct bladeStatus bladeStatus;
        struct bladePortMap bladePortMap;
        struct fanInfo fanInfo;
        struct interconnectTrayStatus interconnectTrayStatus;
        struct interconnectTrayInfo interconnectTrayInfo;
        struct interconnectTrayPortMap interconnectTrayPortMap;
        struct powerSupplyInfo powerSupplyInfo;
        struct powerSupplyStatus powerSupplyStatus;
        struct powerSubsystemInfo powerSubsystemInfo;
        struct powerConfigInfo powerConfigInfo;
        struct powerCapConfig powerCapConfig;
        struct thermalInfo thermalInfo;
        xmlNode *userInfoArray;         /* Items are struct userInfo */
        struct userInfo userInfo;
        struct ldapInfo ldapInfo;
        struct ldapGroupInfo ldapGroupInfo;
        struct snmpInfo snmpInfo;
        struct enclosureNetworkInfo enclosureNetworkInfo;
        struct oaNetworkInfo oaNetworkInfo;
        struct enclosureTime enclosureTime;
        struct alertmailInfo alertmailInfo;
        struct passwordSettings passwordSettings;
        struct ebipaInfo ebipaInfo;
        struct lcdChatMessage lcdChatMessage;
        struct lcdUserNotes lcdUserNotes;
        struct lcdButtonEvent lcdButtonEvent;
        struct lcdStatus lcdStatus;
        struct lcdInfo lcdInfo;
        struct hpSimInfo hpSimInfo;
        struct thermalSubsystemInfo thermalSubsystemInfo;
        struct bladeBootInfo bladeBootInfo;
        struct oaVcmMode oaVcmMode;
        struct powerReductionStatus powerReductionStatus;
        struct virtualMediaStatus virtualMediaStatus;
        struct oaMediaDevice oaMediaDevice;
        struct fanZone fanZone;
        struct ebipaInfoEx ebipaInfoEx;
        struct caCertsInfo caCertsInfo;
        struct rackTopology2 rackTopology2;
        struct userCertificateInfo userCertificateInfo;
        struct syslogSettings syslogSettings;
        struct powerdelaySettings powerdelaySettings;
        struct usbMediaFirmwareImages usbMediaFirmwareImages;
        struct usbMediaConfigScripts configScripts;
        int numValue;
        char *string;
        char *message;
        char *noPayload;
};

struct eventInfo
{
        enum eventType event;
        time_t eventTimeStamp;
        int queueSize;
        int numValue;
        union _hpoa__data eventData;
        enum enum_eventInfo enum_eventInfo;
        xmlNode *extraData;             /* Items are struct extraDataInfo */
};

struct getAllEventsResponse
{
        xmlNode *eventInfoArray;
};

struct getBladeThermalInfoArray
{
	int bayNumber;
};

struct bladeThermalInfoArrayResponse
{
	xmlNode *bladeThermalInfoArray; /* Items are struct bladeThermalInfo */
};

struct bladeThermalInfo
{
	byte sensorNumber;
	byte sensorType;
	byte entityId;
	byte entityInstance;
	byte criticalThreshold;
	byte cautionThreshold;
	byte temperatureC;
	byte oem;
	char *description;
	xmlNode *extraData;             /* Items are struct extraDataInfo */
};	

struct setBladePower
{
        int bayNumber;
        enum powerControl power;
};

struct setInterconnectTrayPower
{
        int bayNumber;
        enum hpoa_boolean on;
};

struct resetInterconnectTray
{
        int bayNumber;
};

struct setEnclosureUid
{
        enum uidControl uid;
};

struct setOaUid
{
        int bayNumber;
        enum uidControl uid;
};

struct setBladeUid
{
        int bayNumber;
        enum uidControl uid;
};

struct setInterconnectTrayUid
{
        int bayNumber;
        enum uidControl uid;
};

/* Main OA SOAP Function prototypes */
int soap_subscribeForEvents(SOAP_CON *connection,
                            struct eventPid *response);

int soap_unSubscribeForEvents(SOAP_CON *connection,
                              const struct unSubscribeForEvents *request);

int soap_getEvent(SOAP_CON *connection,
                  const struct getEvent *request,
                  struct eventInfo *response);

int soap_getAllEventsEx(SOAP_CON *connection,
                      const struct getAllEventsEx *request,
                      struct getAllEventsResponse *response);

int soap_getBladeInfo(SOAP_CON *connection,
                      const struct getBladeInfo *request,
                      struct bladeInfo *response);

int soap_getBladePortMap(SOAP_CON *connection,
                      const struct getBladeInfo *request,
                      struct bladePortMap *response);

int soap_getBladeMpInfo(SOAP_CON *connection,
                        const struct getBladeMpInfo *request,
                        struct bladeMpInfo *response);

int soap_getEnclosureInfo(SOAP_CON *connection,
                          struct enclosureInfo *response);

int soap_getPowerConfigInfo(SOAP_CON *connection,
                            struct powerConfigInfo *response,
                            uint *desired_static_pwr_limit);

int soap_setPowerConfigInfo(SOAP_CON *connection,
                            const struct powerConfigInfo *request);

int soap_getPowerCapConfig(SOAP_CON *connection,
                           struct powerCapConfig *response,
                           uint *desired_dynamic_pwr_cap_limit,
                           uint *desired_derated_circuit_cap,
                           uint *desired_rated_circuit_cap);

int soap_setPowerCapConfig(SOAP_CON *connection,
                           const struct powerCapConfig *request);

int soap_getOaStatus(SOAP_CON *connection,
                     const struct getOaStatus *request,
                     struct oaStatus *response);

int soap_getOaInfo(SOAP_CON *connection,
                   const struct getOaInfo *request,
                   struct oaInfo *response);

int soap_getOaId(SOAP_CON *connection,
                 struct OaId *oaId);

int soap_getInterconnectTrayStatus(SOAP_CON *connection,
                const struct getInterconnectTrayStatus *request,
                struct interconnectTrayStatus *response);

int soap_getInterconnectTrayInfo(SOAP_CON *connection,
                                 const struct getInterconnectTrayInfo *request,
                                 struct interconnectTrayInfo *response);

int soap_getFanInfo(SOAP_CON *connection,
                    const struct getFanInfo *request,
                    struct fanInfo *response);

int soap_getPowerSubsystemInfo(SOAP_CON *connection,
                               struct powerSubsystemInfo *response);

int soap_getPowerSupplyInfo(SOAP_CON *connection,
                            const struct getPowerSupplyInfo *request,
                            struct powerSupplyInfo *response);

int soap_getOaNetworkInfo(SOAP_CON *connection,
                          const struct getOaNetworkInfo *request,
                          struct oaNetworkInfo *response);

int soap_getBladeStatus(SOAP_CON *connection,
                        const struct getBladeStatus *request,
                        struct bladeStatus *response);

int soap_setBladePower(SOAP_CON *connection,
                       const struct setBladePower *request);

int soap_setInterconnectTrayPower(SOAP_CON *connection,
                const struct setInterconnectTrayPower *request);

int soap_resetInterconnectTray(SOAP_CON *connection,
                               const struct resetInterconnectTray *request);

int soap_getThermalInfo(SOAP_CON *connection,
                        const struct getThermalInfo *request,
                        struct thermalInfo *response);

int soap_getUserInfo(SOAP_CON *connection,
                     const struct getUserInfo *request,
                     struct userInfo *response);

int soap_isValidSession(SOAP_CON *connection);

int soap_getRackTopology2(SOAP_CON *con,
                          struct rackTopology2 *response);

int soap_getThermalSubsystemInfo(SOAP_CON *con,
                                 struct thermalSubsystemInfo *response);

int soap_getFanZoneArray(SOAP_CON *con,
                         const struct getFanZoneArray *request,
                         struct getFanZoneArrayResponse *response);

int soap_getEnclosureStatus(SOAP_CON *con,
                            struct enclosureStatus *response);

int soap_getLcdInfo(SOAP_CON *con,
                    struct lcdInfo *response);

int soap_getLcdStatus(SOAP_CON *con,
                      struct lcdStatus *response);

int soap_getPowerSupplyStatus(SOAP_CON *con,
                              const struct getPowerSupplyStatus *request,
                              struct powerSupplyStatus *response);

int soap_setEnclosureUid(SOAP_CON *con,
                         const struct setEnclosureUid *request);

int soap_setOaUid(SOAP_CON *con,
                  const struct setOaUid *request);

int soap_setBladeUid(SOAP_CON *connection,
                     const struct setBladeUid *request);

int soap_setInterconnectTrayUid(SOAP_CON *con,
                                const struct setInterconnectTrayUid *request);

int soap_setLcdButtonLock(SOAP_CON *con,
                          enum hpoa_boolean buttonLock);

int soap_getBladeThermalInfoArray(SOAP_CON *con, 
				  struct getBladeThermalInfoArray *request,
				  struct bladeThermalInfoArrayResponse 
								*response);

int soap_getInterconnectTrayPortMap(SOAP_CON *con,
                                 const struct getInterconnectTrayInfo *request,
                                 struct interconnectTrayPortMap *response);

int soap_getPowerSupplyInfoArray(SOAP_CON *con,
                         const struct getPowerSupplyInfoArray *request,
                         struct getPowerSupplyInfoArrayResponse *response,
                         xmlDocPtr *ps_info_doc);

int soap_getPowerSupplyStatusArray(SOAP_CON *con,
                         const struct getPowerSupplyStsArray *request,
                         struct getPowerSupplyStsArrayResponse *response,
                         xmlDocPtr *ps_sts_doc);

int soap_getFanInfoArray(SOAP_CON *con,
                         const struct getFanInfoArray *request,
                         struct getFanInfoArrayResponse *response,
                         xmlDocPtr *fan_info_doc);

int soap_getBladeInfoArray(SOAP_CON *con,
                         const struct getBladeInfoArray *request,
                         struct getBladeInfoArrayResponse *response,
                         xmlDocPtr *bl_info_doc);

int soap_getBladeStatusArray(SOAP_CON *con,
                         const struct getBladeStsArray *request,
                         struct getBladeStsArrayResponse *response,
                         xmlDocPtr *bl_sts_doc);

int soap_getBladePortMapArray(SOAP_CON *con,
                         const struct getBladePortMapArray *request,
                         struct getBladePortMapArrayResponse *response,
                         xmlDocPtr *bl_pm_doc);

int soap_getInterconnectTrayInfoArray(SOAP_CON *con,
                            const struct getInterconnectTrayInfoArray *request,
                            struct interconnectTrayInfoArrayResponse *response,
                            xmlDocPtr *intr_info_doc);

int soap_getInterconnectTrayStatusArray(SOAP_CON *con,
                            const struct interconnectTrayStsArray *request,
                            struct interconnectTrayStsArrayResponse *response,
                            xmlDocPtr *intr_sts_doc);

int soap_getInterconnectTrayPortMapArray(SOAP_CON *con,
                            const struct interconnectTrayPmArray *request,
                            struct interconnectTrayPmArrayResponse *response,
                            xmlDocPtr *intr_pm_doc);

int soap_getOaInfoArray(SOAP_CON *con,
                         const struct getOaInfoArray *request,
                         struct getOaInfoArrayResponse *response,
                         xmlDocPtr *oa_info_doc);

int soap_getOaStatusArray(SOAP_CON *con,
                         const struct getOaStatusArray *request,
                         struct getOaStatusArrayResponse *response,
                         xmlDocPtr *oa_sts_doc);

/* Function prototypes for OA SOAP helper functions */
void    soap_getExtraData(xmlNode *extraData, struct extraDataInfo *result);
void    soap_getDiagnosticChecksEx(xmlNode *diag,
                                   struct diagnosticData *result);
void    soap_getBladeCpuInfo(xmlNode *cpus, struct bladeCpuInfo *result);
void    soap_getBladeNicInfo(xmlNode *nics, struct bladeNicInfo *result);
void    soap_getBladeMezzInfo(xmlNode *mezz, struct bladeMezzInfo *result);
void    soap_getBladeMezzDevInfo(xmlNode *mezzDevices,
				 struct bladeMezzDevInfo *result);
void    soap_getBladeMezzSlotInfo(xmlNode *mezzSlots,
				  struct bladeMezzSlotInfo *result);
void    soap_getBladeMezzSlotPort(xmlNode *slot,
				  struct bladeMezzSlotPort *result);
void    soap_getBladeMezzDevPort(xmlNode *port,
				 struct bladeMezzDevPort *result);
void    soap_getInterconnectTraySlotInfo(xmlNode *slot,
				struct interconnectTraySlotInfo *result);
void    soap_getInterconnectTrayPortInfo(xmlNode *port,
				 struct interconnectTrayPortInfo *result);
void    soap_getDiagnosticData(xmlNode *data, struct diagnosticData *result);
void    soap_getBayAccess(xmlNode *bay, struct bayAccess *result);
void    soap_getEncLink(xmlNode *data, struct encLink *result);
void    soap_getPortEnabled(xmlNode *data, struct portEnabled *result);
void    soap_getIpAddress(xmlNode *ips, char **result);
void    soap_getEventInfo(xmlNode *events, struct eventInfo *result);
void    soap_getEncLinkOa(xmlNode *data, struct encLinkOa *result);
void    soap_getEncLink2(xmlNode *data, struct encLink2 *result);
void    soap_fanZone(xmlNode *fanZone, struct fanZone *result);
void    soap_fanInfo(xmlNode *fanZone, struct fanInfo *result);
void    soap_deviceBayArray(xmlNode *node, byte *bay);
void	soap_bladeThermalInfo(xmlNode *node, struct bladeThermalInfo *result);

void    parse_powerSupplyInfo(xmlNode *node,struct powerSupplyInfo *response);
void    parse_powerSupplyStatus(xmlNode *node,
                                struct powerSupplyStatus *response);
void    parse_bladeInfo(xmlNode *node, struct bladeInfo *response);
void    parse_interconnectTrayInfo(xmlNode *node,struct interconnectTrayInfo *);
void    parse_interconnectTrayStatus(xmlNode *node,
                                     struct interconnectTrayStatus *);
void    parse_oaInfo(xmlNode *node, struct oaInfo *response);
void    parse_oaStatus(xmlNode *node, struct oaStatus *response);
void    parse_bladeStatus(xmlNode *node, struct bladeStatus *response);
void    parse_bladePortMap(xmlNode *node, struct bladePortMap *response);
void    parse_interconnectTrayPortMap(xmlNode *portmap,
					struct interconnectTrayPortMap *result);
#endif  /* _INC_OASOAP_CALLS_H_ */
