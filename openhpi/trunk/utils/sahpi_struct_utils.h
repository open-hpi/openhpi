/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef __SAHPI_STRUCT_UTILS_H
#define __SAHPI_STRUCT_UTILS_H

#ifndef OH_UTILS_H
#warning *** Include oh_utils.h instead of individual utility header files ***
#endif

#ifdef __cplusplus
extern "C" {
#endif 

/*********************** 
 * Text buffer utilities
 ***********************/
#define OH_MAX_TEXT_BUFFER_LENGTH 1024

/* Same as SaHpiTextBufferT, only more Data */
typedef struct {   
	SaHpiTextTypeT  DataType;
	SaHpiLanguageT  Language;
	SaHpiUint16T    DataLength;
	SaHpiUint8T     Data[OH_MAX_TEXT_BUFFER_LENGTH];
} oh_big_textbuffer;

SaErrorT oh_init_textbuffer(SaHpiTextBufferT *buffer);
SaErrorT oh_append_textbuffer(SaHpiTextBufferT *buffer, const char *from, size_t size);
SaErrorT oh_copy_textbuffer(SaHpiTextBufferT *dest, const SaHpiTextBufferT *from);

/* Print just the Data portions of the text structures */
#define oh_print_text(buf_ptr)  oh_fprint_text(stdout, buf_ptr)
SaErrorT oh_fprint_text(FILE *stream, const SaHpiTextBufferT *buffer);
#define oh_print_bigtext(bigbuf_ptr) oh_fprint_bigtext(stdout, bigbuf_ptr)
SaErrorT oh_fprint_bigtext(FILE *stream, const oh_big_textbuffer *big_buffer);

/************************************ 
 * HPI structure to string conversion
 ************************************/
SaErrorT oh_decode_manufacturerid(SaHpiManufacturerIdT value,
				  SaHpiTextBufferT *buffer);

SaErrorT oh_decode_sensorreading(SaHpiSensorReadingT reading,
                                 SaHpiSensorDataFormatT format,
				 SaHpiTextBufferT *buffer);

SaErrorT oh_encode_sensorreading(SaHpiTextBufferT *buffer,
				 SaHpiSensorReadingTypeT type,
				 SaHpiSensorReadingT *reading);
SaErrorT oh_decode_capability (SaHpiCapabilitiesT ResourceCapabilities,
		               SaHpiTextBufferT *buffer);
#if 0
SaErrorT oh_decode_ctrlstate(SaHpiCtrlStateT control_state,
			     SaHpiTextBufferT *buffer);
#endif

/************************* 
 * Validate HPI structures
 *************************/

/***************************
 * Print HPI data structures
 ***************************/
#define OH_PRINT_OFFSET "  "  /* Offset string */

#define oh_print_textbuffer(buf_ptr)  oh_fprint_textbuffer(stdout, buf_ptr)
SaErrorT oh_fprint_textbuffer(FILE *stream, const SaHpiTextBufferT *textbuffer);

#define oh_print_sensorrec(sensor_ptr) oh_fprint_sensorrec(stdout, sensor_ptr)
SaErrorT oh_fprint_sensorrec(FILE *stream, const SaHpiSensorRecT *sensor);

#define oh_print_idrfield(thisfield, space) oh_fprint_idrfield(stdout, thisfield, space)
SaErrorT oh_fprint_idrfield(FILE *stream, const SaHpiIdrFieldT *thisfield, int space);

#define oh_print_idrinfo(idrInfo, space) oh_fprint_idrinfo(stdout, idrInfo, space)
SaErrorT oh_fprint_idrinfo(FILE *stream, const SaHpiIdrInfoT *idrInfo, int space);

#define oh_print_idrareaheader(areaHeader, space) oh_fprint_idrareaheader(stdout, areaHeader, space)
SaErrorT oh_fprint_idrareaheader(FILE *stream, const SaHpiIdrAreaHeaderT *areaHeader, int space);

#define oh_print_rptentry(rptEntry, space) oh_fprint_rptentry(stdout, rptEntry, space)
SaErrorT oh_fprint_rptentry(FILE *stream, const SaHpiRptEntryT *rptEntry, int space);
  
/* FIXME:: */
#if 0
#define oh_print_ctrlrec(ctrl_ptr) oh_fprint_ctrlrec(stdout, ctrl_ptr)
SaErrorT oh_fprint_ctrlrec(FILE *stream, const SaHpiCtrlRecT *control);

#define oh_print_watchdogrec(watchdog_ptr) oh_fprint_watchdogrec(stdout, watchdog_ptr)
SaErrorT oh_fprint_watchdogrec(FILE *stream, const SaHpiWatchdogRecT *watchdog);

Need ?????
SaHpiBoolT oh_valid_textbuffer(SaHpiTextBufferT *buffer);
SaHpiBoolT oh_valid_time(SaHpiTimeT time); - move to sahpi_time_utils.c/h
/* EventAdd and EventLogAdd ??? */
SaHpiBoolT oh_valid_event(SaHpiEventT event); - move to sahpi_event_utils.c/h
#endif

#ifdef __cplusplus
}
#endif
 
#endif
