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

/************************************************* 
 * Structures to string/buffer conversion routines
 *************************************************/
const char * oh_lookup_manufacturerid(SaHpiManufacturerIdT value);

SaErrorT oh_decode_sensorreading(SaHpiSensorReadingT reading,
                                 SaHpiSensorDataFormatT format,
				 SaHpiTextBufferT *buffer);

/***************************** 
 * Validate structure routines
 *****************************/

/************************** 
 * Print structure routines
 **************************/
#define oh_print_textbuffer(buffer)  oh_fprint_textbuffer(stdout, buffer)
SaErrorT oh_fprint_textbuffer(FILE *stream, const SaHpiTextBufferT *buffer);
#define oh_print_big_textbuffer(big_buffer)  oh_fprint_big_textbuffer(stdout, big_buffer)
SaErrorT oh_fprint_big_textbuffer(FILE *stream, const oh_big_textbuffer *big_buffer);

#if 0
SaHpiBoolT valid_SaHpiTextBufferT(SaHpiTextBufferT *buffer);
SaHpiBoolT valid_SaHpiTimeT(SaHpiTimeT time);
/* EventAdd and EventLogAdd ??? */
SaHpiBoolT valid_SaHpiEventT(SaHpiEventT event);
#endif

#ifdef __cplusplus
}
#endif
 
#endif
