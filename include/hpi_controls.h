/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: license
 */


/*******************************************************************************
********************************************************************************
**********                                                            **********
********** Controls                                                   **********
**********                                                            **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_CONTROLS_H
#define HPI_CONTROLS_H

/* control Number */
typedef SaHpiUint8T SaHpiCtrlNumT;

/*
** Type of Control
**
** This enumerated type defines the different types of generic controls.
*/

typedef enum {
	SAHPI_CTRL_TYPE_DIGITAL = 0x00,
  	SAHPI_CTRL_TYPE_DISCRETE,
  	SAHPI_CTRL_TYPE_ANALOG,
  	SAHPI_CTRL_TYPE_STREAM,
  	SAHPI_CTRL_TYPE_TEXT,
  	SAHPI_CTRL_TYPE_OEM = 0xC0
} SaHpiCtrlTypeT;

/*
** Control State Type Definitions
**
** Defines the types of control states.
*/

typedef enum {
  	SAHPI_CTRL_STATE_OFF = 0,
  	SAHPI_CTRL_STATE_ON,
  	SAHPI_CTRL_STATE_PULSE_OFF,
  	SAHPI_CTRL_STATE_PULSE_ON,
  	SAHPI_CTRL_STATE_AUTO = 0xFF
} SaHpiCtrlStateDigitalT;

typedef SaHpiUint32T SaHpiCtrlStateDiscreteT;
typedef SaHpiInt32T SaHpiCtrlStateAnalogT;

#define SAHPI_CTRL_MAX_STREAM_LENGTH 4

typedef struct {
  	SaHpiBoolT Repeat; /* Repeat flag */
  	SaHpiUint32T StreamLength; /* Length of the data, in bytes,
				     ** stored in the stream. */
  	SaHpiUint8T Stream[SAHPI_CTRL_MAX_STREAM_LENGTH];
} SaHpiCtrlStateStreamT;

typedef SaHpiUint8T SaHpiTxtLineNumT;

/* Reserved number for sending output to all lines */
#define SAHPI_TLN_ALL_LINES (SaHpiTxtLineNumT)0xFF

typedef struct {
  	SaHpiTxtLineNumT Line; /* Operate on line # */
  	SaHpiTextBufferT Text; /* Text to display */
} SaHpiCtrlStateTextT;

#define SAHPI_CTRL_MAX_OEM_BODY_LENGTH 255

typedef struct {
  	SaHpiManufacturerIdT 	MId;
  	SaHpiUint8T BodyLength;
  	SaHpiUint8T Body[SAHPI_CTRL_MAX_OEM_BODY_LENGTH]; /* OEM Specific */
} SaHpiCtrlStateOemT;

typedef union {
  	SaHpiCtrlStateDigitalT 	Digital;
  	SaHpiCtrlStateDiscreteT Discrete;
  	SaHpiCtrlStateAnalogT 	Analog;
  	SaHpiCtrlStateStreamT 	Stream;
  	SaHpiCtrlStateTextT 	Text;
  	SaHpiCtrlStateOemT 	Oem;
} SaHpiCtrlStateUnionT;

typedef struct {
  	SaHpiCtrlTypeT 		Type; /* Type of control */
  	SaHpiCtrlStateUnionT 	StateUnion; /* Data for control type */
} SaHpiCtrlStateT;

#endif /* HPI_CONTROLS_H */


