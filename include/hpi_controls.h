/*
 * Copyright (c) 2003, Service Availability Forum
 * All rights reserved.
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
 * Neither the name of the Service Availalability Forum nor the names 
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


