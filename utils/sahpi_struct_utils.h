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
SaErrorT oh_append_textbuffer(SaHpiTextBufferT *buffer, const char *from);
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

#define oh_print_event(event_ptr, offsets) oh_fprint_event(stdout, event_ptr, offsets)
SaErrorT oh_fprint_event(FILE *stream, const SaHpiEventT *event, int offsets);
  
#define oh_print_idrfield(thisfield, offsets) oh_fprint_idrfield(stdout, thisfield, offsets)
SaErrorT oh_fprint_idrfield(FILE *stream, const SaHpiIdrFieldT *thisfield, int offsets);

#define oh_print_idrinfo(idrInfo, offsets) oh_fprint_idrinfo(stdout, idrInfo, offsets)
SaErrorT oh_fprint_idrinfo(FILE *stream, const SaHpiIdrInfoT *idrInfo, int offsets);

#define oh_print_idrareaheader(areaHeader, offsets) oh_fprint_idrareaheader(stdout, areaHeader, offsets)
SaErrorT oh_fprint_idrareaheader(FILE *stream, const SaHpiIdrAreaHeaderT *areaHeader, int offsets);

#define oh_print_rptentry(rptEntry, offsets) oh_fprint_rptentry(stdout, rptEntry, offsets)
SaErrorT oh_fprint_rptentry(FILE *stream, const SaHpiRptEntryT *rptEntry, int offsets);

#define oh_print_sensorrec(sensor_ptr, offsets) oh_fprint_sensorrec(stdout, sensor_ptr, offsets)
SaErrorT oh_fprint_sensorrec(FILE *stream, const SaHpiSensorRecT *sensor, int offsets);

#define oh_print_rdr(rdr, offsets) oh_fprint_rdr(stdout, rdr, offsets)
SaErrorT oh_fprint_rdr(FILE *stream, const SaHpiRdrT *rdrEntry, int offsets);

#define oh_print_textbuffer(buf_ptr, offsets)  oh_fprint_textbuffer(stdout, buf_ptr, offsets)
SaErrorT oh_fprint_textbuffer(FILE *stream, const SaHpiTextBufferT *textbuffer, int offsets);

#define oh_print_ctrlrec(ctrl_ptr, offsets) oh_fprint_ctrlrec(stdout, ctrl_ptr, offsets)
SaErrorT oh_fprint_ctrlrec(FILE *stream, const SaHpiCtrlRecT *control, int offsets);

#define oh_print_watchdogrec(watchdog_ptr, offsets) oh_fprint_watchdogrec(stdout, watchdog_ptr, offsets)
SaErrorT oh_fprint_watchdogrec(FILE *stream, const SaHpiWatchdogRecT *watchdog, int offsets);

#define oh_print_evenloginfo(elinfo_ptr, offsets) oh_fprint_evenloginfo(stdout, elinfo_ptr, offsets)
SaErrorT oh_fprint_evenloginfo(FILE *stream, const SaHpiEventLogInfoT *thiselinfo, int offsets);

#define oh_print_eventlogentry(eventlog_ptr, offsets) oh_fprint_eventlogentry(stdout, eventlog_ptr, offsets)
SaErrorT oh_fprint_eventlogentry(FILE *stream, const SaHpiEventLogEntryT *thiseventlog, int offsets);

#if 0
/* FIXME:: Do we need these ???? */
SaHpiBoolT oh_valid_textbuffer(SaHpiTextBufferT *buffer);
SaHpiBoolT oh_valid_time(SaHpiTimeT time); - move to sahpi_time_utils.c/h
/* For EventAdd and EventLogAdd ??? */
SaHpiBoolT oh_valid_event(SaHpiEventT event); - move to sahpi_event_utils.c/h
#endif

#ifdef __cplusplus
}
#endif
 
#endif
