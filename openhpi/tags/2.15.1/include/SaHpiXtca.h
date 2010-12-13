/*******************************************************************************
**
** FILE:
**   SaHpiXtca.h
**
** DESCRIPTION:
**   This file provides the C language binding for the Service
**   Availability(TM) Forum HPI to AdvancedTCA(R) and MicroTCA(R)
**   Mapping.
**   It contains all of the required prototypes and type definitions.
**   This file was generated from the Mapping Specification document.
**
** SPECIFICATION VERSION:
**   SAIM-HPI-B.03.02-xTCA
**
** DATE:
**   Sat Oct 31 2009
**
** LEGAL:
**   OWNERSHIP OF SPECIFICATION AND COPYRIGHTS.
**
** Copyright(c) 2009, Service Availability(TM) Forum. All rights reserved.
**
** Permission to use, copy, modify, and distribute this software for any
** purpose without fee is hereby granted, provided that this entire notice
** is included in all copies of any software which is or includes a copy
** or modification of this software and in all copies of the supporting
** documentation for such software.
**
** THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTY.  IN PARTICULAR, THE SERVICE AVAILABILITY FORUM DOES NOT MAKE ANY
** REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
** OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
**
*******************************************************************************/

#ifndef __SAHPI_xTCA_H
#define __SAHPI_xTCA_H

/*
 * SaHpi.h contains the basic data types that are used in this section,
 *         e.g., SaHpiCtrlNumT.
 */
#include <SaHpi.h>

/*******************************************************************************
*  Basic Data Types and Values
*******************************************************************************/

#ifdef  __cplusplus
extern "C" {
#endif

/* Specification version */
#define XTCAHPI_SPEC_VERSION     "SAIM-HPI-B.03.02.XTCA"

/* PICMG manufacturer identifier */
#define XTCAHPI_PICMG_MID        0x00315A

/*******************************************************************************
*  Entities
*******************************************************************************/

/*
 * Common Entity Definitions
 * These definitions describe the ATCA and MicroTCA Entity Types defined
 * in this specification.
 *
 * XTCAHPI_ENT_POWER_SLOT              - ATCA Power Entry Module Slot/
 *                                       MicroTCA Power Module Slot
 * XTCAHPI_ENT_SHELF_FRU_DEVICE_SLOT   - ATCA Shelf FRU information device Slot
 * XTCAHPI_ENT_SHELF_MANAGER_SLOT      - Dedicated Shelf Manager Slot
 * XTCAHPI_ENT_FAN_TRAY_SLOT           - Fan Tray Slot/Cooling Unit Slot
 * XTCAHPI_ENT_FAN_FILTER_TRAY_SLOT    - Fan Tray Filter Slot
 * XTCAHPI_ENT_ALARM_SLOT              - Alarm Module Slot
 * XTCAHPI_ENT_AMC_SLOT                - ATCA AMC Slot
 * XTCAHPI_ENT_PMC_SLOT                - PMC Module Slot
 * XTCAHPI_ENT_RTM_SLOT                - Rear Transition Module Slot
 * XTCAHPI_ENT_CARRIER_MANAGER_SLOT    - MicroTCA MCH Slot
 * XTCAHPI_ENT_CARRIER_ SLOT           - MicroTCA Carrier Slot
 * XTCAHPI_ENT_COM_E_SLOT              - COM-E Module Slot
 *
 */

#define XTCAHPI_ENT_POWER_SLOT              \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 1))
#define XTCAHPI_ENT_SHELF_FRU_DEVICE_SLOT   \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 2))
#define XTCAHPI_ENT_SHELF_MANAGER_SLOT      \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 3))
#define XTCAHPI_ENT_FAN_TRAY_SLOT           \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 4))
#define XTCAHPI_ENT_FAN_FILTER_TRAY_SLOT    \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 5))
#define XTCAHPI_ENT_ALARM_SLOT              \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 6))
#define XTCAHPI_ENT_AMC_SLOT                \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 7))
#define XTCAHPI_ENT_PMC_SLOT                \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 8))
#define XTCAHPI_ENT_RTM_SLOT                \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 9))
#define XTCAHPI_ENT_CARRIER_MANAGER_SLOT    \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 0xA))
#define XTCAHPI_ENT_CARRIER_SLOT            \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 0xB))
#define XTCAHPI_ENT_COM_E_SLOT              \
                         ((SaHpiEntityTypeT)(SAHPI_ENT_CHASSIS_SPECIFIC + 0xC))

/*******************************************************************************
*  Common Management Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * IP Address 0 Control
 * IP Address 1 Control
 * Power On Sequence Controls
 * - Note that the Control numbers from
 *      XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE to
 *      XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE + FEh
 *   are reserved for Power On Sequence Controls.
 *
 * Power On Sequence Commit Control
 * Power On Sequence Commit Status Sensor
 * Telco Alarm Annunciator
 * Redundancy Sensor
 * Active Instance Sensor
 * Standby Instance Sensor
 * Failover Control
 * Slot State Sensor
 * Activation Control
 * Deactivation Control
 * FRU Info Valid Sensor
 * Assigned Power Sensors
 * - Note that the Sensor numbers from
 *      XTCAHPI_SENSOR_NUM_SLOT_ASSIGNED_PWR to
 *      XTCAHPI_SENSOR_NUM_SLOT_ASSIGNED_PWR + FEh
 *   are reserved for Assigned Power Sensors.
 *
 */

#define XTCAHPI_CTRL_NUM_IP_ADDRESS_0             ((SaHpiCtrlNumT)0x1001)
#define XTCAHPI_CTRL_NUM_IP_ADDRESS_1             ((SaHpiCtrlNumT)0x1003)
#define XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE        ((SaHpiCtrlNumT)0x1301)
#define XTCAHPI_CTRL_NUM_POWER_ON_SEQUENCE_COMMIT ((SaHpiCtrlNumT)0x1300)
#define XTCAHPI_SENSOR_NUM_PWRONSEQ_COMMIT_STATUS ((SaHpiSensorNumT)0x1300)
#define XTCAHPI_ANN_NUM_TELCO_ALARM               ((SaHpiAnnunciatorNumT)0x1000)
#define XTCAHPI_SENSOR_NUM_REDUNDANCY             ((SaHpiSensorNumT)0x1001)
#define XTCAHPI_SENSOR_NUM_ACTIVE                 ((SaHpiSensorNumT)0x1002)
#define XTCAHPI_SENSOR_NUM_STANDBY                ((SaHpiSensorNumT)0x1003)
#define XTCAHPI_CTRL_NUM_FAILOVER                 ((SaHpiCtrlNumT)0x1010)
#define XTCAHPI_CTRL_NUM_ACTIVATION               ((SaHpiCtrlNumT)0x1020)
#define XTCAHPI_CTRL_NUM_DEACTIVATION             ((SaHpiCtrlNumT)0x1021)
#define XTCAHPI_SENSOR_NUM_FRU_INFO_VALID         ((SaHpiSensorNumT)0x1000)

/*
 * Instrument numbers for:
 * ATCA Assigned Power Sensor in Slot Resource
 */

#define XTCAHPI_SENSOR_NUM_ASSIGNED_PWR        ((SaHpiSensorNumT)0x1011)

/*
 * Instrument numbers for:
 * ATCA Assigned Power Sensors in Containing entity Resource
 */

#define XTCAHPI_SENSOR_NUM_SLOT_ASSIGNED_PWR   ((SaHpiSensorNumT)0x1800)

/*******************************************************************************
*  Shelf Resource Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * Shelf Configuration Information IDR
 * Shelf Address Control
 * Chassis Status Control
 */

#define XTCAHPI_IDR_NUM_CONFIG_INFO              ((SaHpiIdrIdT)0x0001)
#define XTCAHPI_CTRL_NUM_SHELF_ADDRESS           ((SaHpiCtrlNumT)0x1000)
#define XTCAHPI_CTRL_NUM_SHELF_STATUS            ((SaHpiCtrlNumT)0x1002)

/*******************************************************************************
*  MicroTCA Carrier Resource Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * Shelf Manager RMCP Username Control
 * Shelf Manager RMCP Password Control
 */

#define XTCAHPI_CTRL_NUM_SHELF_MANAGER_RMCP_USERNAME   ((SaHpiCtrlNumT)0x1051)
#define XTCAHPI_CTRL_NUM_SHELF_MANAGER_RMCP_PASSWORD   ((SaHpiCtrlNumT)0x1052)

/*******************************************************************************
*  MicroTCA Module Slot Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * In-Shelf Module Activation Control
 * In-Shelf Module Dectivation Control
 */

#define XTCAHPI_CTRL_NUM_IN_SHELF_ACTIVATION   ((SaHpiCtrlNumT)0x1060)
#define XTCAHPI_CTRL_NUM_IN_SHELF_DEACTIVATION ((SaHpiCtrlNumT)0x1061)

/*******************************************************************************
*  FRU/Module Resource Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * Desired Power Control
 * IPMB0 Sensor
 * - Note that the Sensor numbers from XTCAHPI_SENSOR_NUM_IPMB0 to
 *   XTCAHPI_SENSOR_NUM_IPMB0 + 0x5F (0x1100-0x115F) are reserved for
 *   systems supporting a radial IPMB requiring multiple IPMB0 Sensors.
 *
 * IPMI A/B State Control
 * - Note that the Control numbers from XTCAHPI_CTRL_NUM_IPMB_A_STATE to
 *   XTCAHPI_CTRL_NUM_IPMB_B_STATE + 2*0x5F (0x1101-0x11C0) are reserved
 *   for systems supporting a radial IPMB requiring multiple IPMB0 controls
 *
 * FRU Reset and Diagnostics Control
 * FRU IPM Controller Reset Control
 * LED Controls
 * - Note that Control numbers from XTCAHPI_CTRL_NUM_APP_LED
 *   to XTCAHPI_CTRL_NUM_APP_LED + 0xFA
 *   are reserved for Application Specific LED Controls.
 *
 * AMC Power On Sequence Control
 * - Note that the Control numbers from XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE
 *   to XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE + Fh
 *   are reserved for AMC Power On Sequence Controls.
 *
 * AMC Power On Sequence Commit Control
 * AMC Power On Sequence Commit Status Sensor
 * Fan Speed Control
 * HPM.1 Global IPMC Upgrade Capabilities Sensor
 * HPM.1 Upgrade Image Capabilities Sensor
 * HPM.1 Rollback Timeout Period Sensor
 */

#define XTCAHPI_CTRL_NUM_DESIRED_PWR                   ((SaHpiCtrlNumT)0x1030)
#define XTCAHPI_SENSOR_NUM_IPMB0                       ((SaHpiSensorNumT)0x1100)
#define XTCAHPI_CTRL_NUM_IPMB_A_STATE                  ((SaHpiCtrlNumT)0x1101)
#define XTCAHPI_CTRL_NUM_IPMB_B_STATE                  ((SaHpiCtrlNumT)0x1102)
#define XTCAHPI_CTRL_NUM_FRU_CONTROL                   ((SaHpiCtrlNumT)0x1200)
#define XTCAHPI_CTRL_NUM_FRU_IPMC_RESET                ((SaHpiCtrlNumT)0x1201)
#define XTCAHPI_CTRL_NUM_BLUE_LED                      ((SaHpiCtrlNumT)0x00)
#define XTCAHPI_CTRL_NUM_LED1                          ((SaHpiCtrlNumT)0x01)
#define XTCAHPI_CTRL_NUM_LED2                          ((SaHpiCtrlNumT)0x02)
#define XTCAHPI_CTRL_NUM_LED3                          ((SaHpiCtrlNumT)0x03)
#define XTCAHPI_CTRL_NUM_APP_LED                       ((SaHpiCtrlNumT)0x04)
#define XTCAHPI_RESOURCE_AGGREGATE_LED                 ((SaHpiCtrlNumT)0xFF)
#define XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE         ((SaHpiCtrlNumT)0x1501)
#define XTCAHPI_CTRL_NUM_AMC_POWER_ON_SEQUENCE_COMMIT  ((SaHpiCtrlNumT)0x1500)
#define XTCAHPI_SENSOR_NUM_AMC_PWRONSEQ_COMMIT_STATUS  ((SaHpiSensorNumT)0x1500)
#define XTCAHPI_CTRL_NUM_FAN_SPEED                     ((SaHpiCtrlNumT)0x1400)
#define XTCAHPI_SENSOR_NUM_HPM1_IPMC_GLOBAL_CAPS       ((SaHpiSensorNumT)0x1700)
#define XTCAHPI_SENSOR_NUM_HPM1_IMAGE_CAPS             ((SaHpiSensorNumT)0x1701)
#define XTCAHPI_SENSOR_NUM_HPM1_ROLLBACK_TIMEOUT       ((SaHpiSensorNumT)0x1702)

/*******************************************************************************
*  Virtual Telco Alarm Resource Instruments
*******************************************************************************/

/*
 * Instrument numbers for:
 * Minor Telco Alarm Control
 * Major Telco Alarm Control
 * Critical Telco Alarm Control
 * Power Telco Alarm Control
 * Telco Alarm Cutoff Control
 * Telco Alarm Input Sensor
*/

#define XTCAHPI_CTRL_NUM_CRITICAL_TELCO_ALARM    ((SaHpiCtrlNumT)0x1600)
#define XTCAHPI_CTRL_NUM_MAJOR_TELCO_ALARM       ((SaHpiCtrlNumT)0x1601)
#define XTCAHPI_CTRL_NUM_MINOR_TELCO_ALARM       ((SaHpiCtrlNumT)0x1602)
#define XTCAHPI_CTRL_NUM_POWER_TELCO_ALARM       ((SaHpiCtrlNumT)0x1603)
#define XTCAHPI_CTRL_NUM_TELCO_ALARM_CUTOFF      ((SaHpiCtrlNumT)0x1604)
#define XTCAHPI_SENSOR_NUM_TELCO_ALARM_INPUT     ((SaHpiSensorNumT)0x1600)

/*
 * Telco Alarm Control Entity Locations.
 * These entity location values are used for Telco Alarm Controls.
 */
#define XTCAHPI_INDICATOR_LOC_MINOR_ALARM     ((SaHpiEntityLocationT)0x0000)
#define XTCAHPI_INDICATOR_LOC_MAJOR_ALARM     ((SaHpiEntityLocationT)0x0001)
#define XTCAHPI_INDICATOR_LOC_CRITICAL_ALARM  ((SaHpiEntityLocationT)0x0002)
#define XTCAHPI_INDICATOR_LOC_POWER_ALARM     ((SaHpiEntityLocationT)0x0003)

/*******************************************************************************
*  Shelf Configuration Information IDR Data Types
*******************************************************************************/

/*
 * Shelf Configuration Information IDR Area and Field IDs.
 */

#define XTCAHPI_CONFIG_DATA_AREA_SPEC_VERSION  ((SaHpiEntryIdT)0x0000)
#define XTCAHPI_CONFIG_DATA_FIELD_LABEL        ((SaHpiEntryIdT)0x0000)

/*******************************************************************************
*  Chassis Status Control Data Types
*******************************************************************************/

/* Bitmask for Current Power State */
/* Maps to OEM Control state byte [2] for the Chassis Status Control */
typedef SaHpiUint8T XtcaHpiCsCurrentPwrState;

/* If bit is set, powered on, powered off otherwise */
#define XTCAHPI_CS_CPS_PWR_ON                   ((XtcaHpiCsCurrentPwrState) 0x01)
/* If bit is set, chassis shut down due to power overload */
#define XTCAHPI_CS_CPS_PWR_OVERLOAD             ((XtcaHpiCsCurrentPwrState) 0x02)
/* If bit is set, chassis shut down due to interlock switch being active */
#define XTCAHPI_CS_CPS_INTERLOCK                ((XtcaHpiCsCurrentPwrState) 0x04)
/* If bit is set, fault detected in main power subsystem */
#define XTCAHPI_CS_CPS_PWR_FAULT                ((XtcaHpiCsCurrentPwrState) 0x08)
/* If bit is set, fault in power Control to power up/down */
#define XTCAHPI_CS_CPS_PWR_CTRL_FAULT           ((XtcaHpiCsCurrentPwrState) 0x10)
/* If bit is set, after power restoration, chassis is restored
   to the state it was in, else chassis stays powered off after
   power restoration */
#define XTCAHPI_CS_CPS_PWR_RESTORE_PWR_UP_PREV  ((XtcaHpiCsCurrentPwrState) 0x20)
/* If bit is set, chassis powers up after power restoration */
#define XTCAHPI_CS_CPS_PWR_RESTORE_PWR_UP       ((XtcaHpiCsCurrentPwrState) 0x40)
/* If bits are set, unknown */
#define XTCAHPI_CS_CPS_PWR_RESTORE_UNKNOWN      ((XtcaHpiCsCurrentPwrState) 0x60)

/* Bitmask for Last Power Event */
/* maps to OEM Control state byte [3] for the Chassis Status Control */
typedef SaHpiUint8T XtcaHpiCsLastPwrEvent;

/* If bit is set, AC failed */
#define XTCAHPI_CS_LPEVT_AC_FAILED       ((XtcaHpiCsLastPwrEvent) 0x01)
/* If bit is set, last power down caused by a Power overload */
#define XTCAHPI_CS_LPEVT_PWR_OVERLOAD    ((XtcaHpiCsLastPwrEvent) 0x02)
/* If bit is set, last power down caused by a power interlock being
   activated */
#define XTCAHPI_CS_LPEVT_PWR_INTERLOCK   ((XtcaHpiCsLastPwrEvent) 0x04)
/* If bit is set, last power down caused by power fault */
#define XTCAHPI_CS_LPEVT_PWR_FAULT       ((XtcaHpiCsLastPwrEvent) 0x08)
/* If bit is set, last 'Power is on' state was entered via IPMI command */
#define XTCAHPI_CS_LPEVT_PWRON_IPMI      ((XtcaHpiCsLastPwrEvent) 0x10)

/* Bitmask for Miscellaneous Chassis State */
/* maps to OEM Control state byte [4] for the Chassis Status Control */
typedef SaHpiUint8T XtcaHpiCsMiscChassisState;

/* If bit is set, Chassis intrusion is active */
#define XTCAHPI_CS_MISC_CS_INTRUSION_ACTIVE    ((XtcaHpiCsMiscChassisState) 0x01)
/* If bit is set, Front Panel Lockout is active (power off and
   reset via chassis push-buttons disabled) */
#define XTCAHPI_CS_MISC_CS_FP_LOCKOUT_ACTIVE   ((XtcaHpiCsMiscChassisState) 0x02)
/* If bit is set, indicates Drive Fault */
#define XTCAHPI_CS_MISC_CS_DRIVE_FAULT         ((XtcaHpiCsMiscChassisState) 0x04)
/* If bit is set, Cooling/fan fault detected */
#define XTCAHPI_CS_MISC_CS_COOLING_FAULT       ((XtcaHpiCsMiscChassisState) 0x08)

/* Bitmask for Front Panel Button Capabilities and disable/enable status */
/* maps to OEM Control state byte [5] for the Chassis Status Control */
typedef SaHpiUint8T XtcaHpiCsFpButtonCap;

/* If bit is set, power off button is disabled */
#define XTCAHPI_CS_FP_BUTTON_PWR_OFF             ((XtcaHpiCsFpButtonCap) 0x01)
/* If bit is set, reset button is disabled */
#define XTCAHPI_CS_FP_BUTTON_RESET_OFF           ((XtcaHpiCsFpButtonCap) 0x02)
/* If bit is set, diagnostic interrupt button is disabled */
#define XTCAHPI_CS_FP_BUTTON_DIAGINTR_OFF        ((XtcaHpiCsFpButtonCap) 0x04)
/* If bit is set, standby button is disabled */
#define XTCAHPI_CS_FP_BUTTON_STANDBY_OFF         ((XtcaHpiCsFpButtonCap) 0x08)
/* If bit is set, power off button disable is allowed */
#define XTCAHPI_CS_FP_BUTTON_ALLOW_PWR_OFF       ((XtcaHpiCsFpButtonCap) 0x10)
/* If bit is set, reset button disable is allowed */
#define XTCAHPI_CS_FP_BUTTON_ALLOW_RESET_OFF     ((XtcaHpiCsFpButtonCap) 0x20)
/* If bit is set, diagnostic interrupt button disable is allowed */
#define XTCAHPI_CS_FP_BUTTON_ALLOW_DIAGINTR_OFF  ((XtcaHpiCsFpButtonCap) 0x40)
/* If bit is set, standby button disable is allowed */
#define XTCAHPI_CS_FP_BUTTON_ALLOW_STANDBY_OFF   ((XtcaHpiCsFpButtonCap) 0x80)

/*******************************************************************************
*  xTCA LED Common Data Types
*******************************************************************************/

/*
 * Color Capabilities
 *
 * This defines the possible color capabilities for an individual LED.
 * One LED may support any number of colors using the bit mask.
 *
 * XTCAHPI_LED_WHITE
 * - Indicates that the LED supports illumination in WHITE.
 * XTCAHPI_LED_ORANGE
 * - Indicates that the LED supports illumination in ORANGE.
 * XTCAHPI_LED_AMBER
 * - Indicates that the LED supports illumination in AMBER.
 * XTCAHPI_LED_GREEN
 * - Indicates that the LED supports illumination in GREEN.
 * XTCAHPI_LED_RED
 * - Indicates that the LED supports illumination in RED.
 * XTCAHPI_LED_BLUE
 * - Indicates that the LED supports illumination in BLUE.
 *
 */
typedef SaHpiUint8T XtcaHpiColorCapabilitiesT;

#define XTCAHPI_BLINK_COLOR_LED   ((XtcaHpiColorCapabilitiesT)0x80)
#define XTCAHPI_LED_WHITE         ((XtcaHpiColorCapabilitiesT)0x40)
#define XTCAHPI_LED_ORANGE        ((XtcaHpiColorCapabilitiesT)0x20)
#define XTCAHPI_LED_AMBER         ((XtcaHpiColorCapabilitiesT)0x10)
#define XTCAHPI_LED_GREEN         ((XtcaHpiColorCapabilitiesT)0x08)
#define XTCAHPI_LED_RED           ((XtcaHpiColorCapabilitiesT)0x04)
#define XTCAHPI_LED_BLUE          ((XtcaHpiColorCapabilitiesT)0x02)

typedef enum {
    XTCAHPI_LED_COLOR_RESERVED = 0,
    XTCAHPI_LED_COLOR_BLUE,
    XTCAHPI_LED_COLOR_RED,
    XTCAHPI_LED_COLOR_GREEN,
    XTCAHPI_LED_COLOR_AMBER,
    XTCAHPI_LED_COLOR_ORANGE,
    XTCAHPI_LED_COLOR_WHITE,
    XTCAHPI_LED_COLOR_NO_CHANGE = 0x0E,
    XTCAHPI_LED_COLOR_USE_DEFAULT = 0x0F
} XtcaHpiLedColorT;

typedef enum {
    XTCAHPI_LED_AUTO,       /* Set all LEDs under this Resource to Auto mode */
    XTCAHPI_LED_MANUAL,     /* Set all LEDs under this Resource to Manual mode */
    XTCAHPI_LED_LAMP_TEST   /* Put all LEDs under this Resource in Lamp Test */
} XtcaHpiResourceLedModeT;

typedef enum {
    XTCAHPI_LED_BR_SUPPORTED,     /* Blinking Rates are Supported */
    XTCAHPI_LED_BR_NOT_SUPPORTED, /* Blinking Rates are Not Supported */
    XTCAHPI_LED_BR_UNKNOWN,       /* The HPI implementation cannot determine
                                    whether or not Blinking Rates are
                                    supported. */
} XtcaHpiLedBrSupportT;

/*******************************************************************************
*  Electronic Keying Data Types
*******************************************************************************/

/*
 * Interface types for:
 * P2P Fabric links
 * Bused Clock links, Clock links
 * P2P Base links
 * P2P Update Channel links
 * Metallic Test Bus links
 * Ringing Generator Bus links
 */
#define XTCAHPI_IF_FABRIC                 ((SaHpiEntityLocationT)0x00001)
#define XTCAHPI_IF_SYNC_CLOCK             ((SaHpiEntityLocationT)0x00002)
#define XTCAHPI_IF_BASE                   ((SaHpiEntityLocationT)0x00003)
#define XTCAHPI_IF_UPDATE_CHANNEL         ((SaHpiEntityLocationT)0x00004)
#define XTCAHPI_IF_METALLIC_TEST          ((SaHpiEntityLocationT)0x00005)
#define XTCAHPI_IF_RINGING_GENERATOR_BUS  ((SaHpiEntityLocationT)0x00006)

/*******************************************************************************
*  ConfigData Entity Locations
*******************************************************************************/

/*
 * ConfigData Entity Locations.
 * These entity location values are used for management instruments that are
 * defined with the SAHPI_ENT_CONFIG_DATA entity type to hold various items
 * of configuration information.
 */
#define XTCAHPI_CONFIG_DATA_LOC_DEFAULT             ((SaHpiEntityLocationT)0x00)
#define XTCAHPI_CONFIG_DATA_LOC_SHELF_ADDRESS       ((SaHpiEntityLocationT)0x01)
#define XTCAHPI_CONFIG_DATA_LOC_POWER_ON_SEQUENCE   ((SaHpiEntityLocationT)0x02)
#define XTCAHPI_CONFIG_DATA_LOC_CHASSIS_STATUS      ((SaHpiEntityLocationT)0x03)
#define XTCAHPI_CONFIG_DATA_LOC_ACTIVATION          ((SaHpiEntityLocationT)0x04)
#define XTCAHPI_CONFIG_DATA_LOC_DEACTIVATION        ((SaHpiEntityLocationT)0x05)
#define XTCAHPI_CONFIG_DATA_LOC_IN_SHELF_ACTIVATION ((SaHpiEntityLocationT)0x06)
#define XTCAHPI_CONFIG_DATA_LOC_IN_SHELF_DEACTIVATION   \
                                                    ((SaHpiEntityLocationT)0x07)
#define XTCAHPI_CONFIG_DATA_LOC_USERNAME            ((SaHpiEntityLocationT)0x08)
#define XTCAHPI_CONFIG_DATA_LOC_PASSWORD            ((SaHpiEntityLocationT)0x09)
#define XTCAHPI_CONFIG_DATA_LOC_FUMI_GLOBAL_UPGRADE_CAP \
                                                    ((SaHpiEntityLocationT)0x0A)
#define XTCAHPI_CONFIG_DATA_LOC_FUMI_UPGRADE_IMAGE_CAP  \
                                                    ((SaHpiEntityLocationT)0x0B)
#define XTCAHPI_CONFIG_DATA_LOC_FUMI_ROLLBACK_TIMEOUT   \
                                                    ((SaHpiEntityLocationT)0x0C)

#ifdef __cplusplus
}
#endif

#endif  /* __SAHPI_xTCA_H */
