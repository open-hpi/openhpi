/** 
 * TODO: license
 *
 * Copyright (c) 2003 Intel Corporation
 *
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Control Resource Data Records 			      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_CTRLRDR_H
#define HPI_CTRLRDR_H

/*
** Output Type
**
** This enumeration defines the what the control's output will be.
*/
typedef enum {
	SAHPI_CTRL_GENERIC = 0,
	SAHPI_CTRL_LED,
	SAHPI_CTRL_FAN_SPEED,
	SAHPI_CTRL_DRY_CONTACT_CLOSURE,
	SAHPI_CTRL_POWER_SUPPLY_INHIBIT,
	SAHPI_CTRL_AUDIBLE,
	SAHPI_CTRL_FRONT_PANEL_LOCKOUT,
	SAHPI_CTRL_POWER_INTERLOCK,
	SAHPI_CTRL_POWER_STATE,
	SAHPI_CTRL_LCD_DISPLAY,
	SAHPI_CTRL_OEM
} SaHpiCtrlOutputTypeT;

/*
** Specific Record Types
** These types represent the specific types of control resource data records.
*/
typedef struct {
	SaHpiCtrlStateDigitalT Default;
} SaHpiCtrlRecDigitalT;

typedef struct {
	SaHpiCtrlStateDiscreteT Default;
} SaHpiCtrlRecDiscreteT;

typedef struct {
	SaHpiCtrlStateAnalogT Min; /* Minimum Value */
	SaHpiCtrlStateAnalogT Max; /* Maximum Value */
	SaHpiCtrlStateAnalogT Default;
} SaHpiCtrlRecAnalogT;

typedef struct {
	SaHpiCtrlStateStreamT Default;
} SaHpiCtrlRecStreamT;

typedef struct {
	SaHpiUint8T MaxChars; /* Maximum chars per line */
	SaHpiUint8T MaxLines; /* Maximum # of lines */
	SaHpiLanguageT Language; /* Language Code */
	SaHpiTextTypeT DataType; /* Permitted Data */
	SaHpiCtrlStateTextT Default;
} SaHpiCtrlRecTextT;

#define SAHPI_CTRL_OEM_CONFIG_LENGTH 10

typedef struct {
	SaHpiManufacturerIdT MId;
	SaHpiUint8T ConfigData[SAHPI_CTRL_OEM_CONFIG_LENGTH];
	SaHpiCtrlStateOemT Default;
} SaHpiCtrlRecOemT;

typedef union {
	SaHpiCtrlRecDigitalT Digital;
	SaHpiCtrlRecDiscreteT Discrete;
	SaHpiCtrlRecAnalogT Analog;
	SaHpiCtrlRecStreamT Stream;
	SaHpiCtrlRecTextT Text;
	SaHpiCtrlRecOemT Oem;
} SaHpiCtrlRecUnionT;

/*
** Record Definition
** Definition of the control resource data record.
*/
typedef struct {
	SaHpiCtrlNumT Num; /* Control Number/Index */
	SaHpiBoolT Ignore; /* Ignore control (entity
			    * not present, disabled, etc.) */
	SaHpiCtrlOutputTypeT OutputType;
	SaHpiCtrlTypeT Type; /* Type of control */
	SaHpiCtrlRecUnionT TypeUnion; /* Specific control record */
	SaHpiUint32T Oem; /* Reserved for OEM use */
} SaHpiCtrlRecT;

#endif /* HPI_CTRLRDR_H */


