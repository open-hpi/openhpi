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
 *      Sean Dague <sdague@users.sf.net>
 *      Steve Sherman <stevees@us.ibm.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SaHpi.h>
#include <oh_utils.h>

static inline SaErrorT oh_init_big_textbuffer(oh_big_textbuffer *big_buffer);
static inline SaErrorT oh_append_big_textbuffer(oh_big_textbuffer *big_buffer, const char *from, size_t size);
static inline SaErrorT oh_copy_big_textbuffer(oh_big_textbuffer *dest, const oh_big_textbuffer *from);

/************************************************************************
 * NOTES!
 * - SA_ERR_HPI_INTERNAL_ERROR is used instead of SA_ERR_HPI_OUT_OF_SPACE 
 *   when oh_textbuffer cannot hold the generated string, since OpenHPI 
 *   guarentees this buffer is large enough.
 *
 * - Several error checks can be removed if valid_xxx routines are defined
 *   for input structures. If this happens, several of the default switch
 *   statements should also return SA_ERR_HPI_INTERNAL_ERROR instead
 *   of SA_ERR_HPI_INVALID_PARMS.
 ************************************************************************/

/**
 * oh_lookup_manufacturerid:
 * @value: enum value of type SaHpiManufacturerIdT.
 *
 * Converts @value into a string based on @value's enum definition
 * in http://www.iana.org/assignments/enterprise-numbers.
 * Only a few of the manufacturers in that list have been defined.
 * For all others, this routine returns "Unknown Manufacturer".
 * Feel free to add your own favorite manufacturer to this routine.
 * 
 * Returns:
 * string - normal operation.
 **/
const char *oh_lookup_manufacturerid(SaHpiManufacturerIdT value) 
{
	switch(value) {
	case SAHPI_MANUFACTURER_ID_UNSPECIFIED:
		return("UNSPECIFIED Manufacturer");
	case 2:
		return("IBM");
	default:  
		return("Unknown Manufacturer");
	}
}

/**
 * oh_decode_sensorreading: 
 * @reading: Pointer to SaHpiSensorReadingT to convert.
 * @format: Pointer to SaHpiDataFormatT for the sensor reading.
 * @buffer: Location to store the converted string.
 *
 * Converts a sensor reading and format into a string. 
 * String is stored in an SaHpiTextBufferT data structure.
 * 
 * Returns: 
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_CMD - @format or @reading have IsSupported == FALSE.
 * SA_ERR_HPI_INVALID_DATA - @format and @reading types don't match.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer, @reading, @format is NULL; Invalid @reading type
 * SA_ERR_HPI_OUT_OF_SPACE - @buffer not big enough to accomodate appended string
 **/
SaErrorT oh_decode_sensorreading(SaHpiSensorReadingT *reading,
				 SaHpiSensorDataFormatT *format,
				 SaHpiTextBufferT *buffer)
{
	SaErrorT err;
	SaHpiTextBufferT working;
        char text[SAHPI_MAX_TEXT_BUFFER_LENGTH];
        size_t text_size = 0;

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        if (!reading->IsSupported || !format->IsSupported) {
                return(SA_ERR_HPI_INVALID_CMD);
        }
        if (reading->Type != format->ReadingType) {
                return(SA_ERR_HPI_INVALID_DATA);
        }
	
	oh_init_textbuffer(&working);
	memset(text, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);

        switch(reading->Type) {
        case SAHPI_SENSOR_READING_TYPE_INT64:
                text_size = snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH,
				     "%lld", reading->Value.SensorInt64);
		err = oh_append_textbuffer(&working, text, text_size);
		if (err != SA_OK) { return(err); }
                break;
        case SAHPI_SENSOR_READING_TYPE_UINT64:
		text_size = snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH,
				     "%llu", reading->Value.SensorUint64);
		err = oh_append_textbuffer(&working, text, text_size);
		if (err != SA_OK) { return(err); }
                break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                text_size = snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH, 
				     "%le", reading->Value.SensorFloat64);
		err = oh_append_textbuffer(&working, text, text_size);
		if (err != SA_OK) { return(err); }
 		break;
        case SAHPI_SENSOR_READING_TYPE_BUFFER:
		err = oh_append_textbuffer(&working, reading->Value.SensorBuffer, 
					   SAHPI_SENSOR_BUFFER_LENGTH);
		if (err != SA_OK) { return(err); }
                break;
	default:
		return(SA_ERR_HPI_INVALID_PARAMS);
        }
        
        if (format->Percentage) {
		err = oh_append_textbuffer(&working, "%", 1); 
		if (err != SA_OK) { return(err); }
        }
	else {
		const char *str;

		/* Add units */
		err = oh_append_textbuffer(&working, " ", 1); 		
		if (err != SA_OK) { return(err); }
		str = oh_lookup_sensorunits(format->BaseUnits);
 		if (str == NULL) { return(SA_ERR_HPI_INVALID_PARAMS); }
		err = oh_append_textbuffer(&working, str, strlen(str)); 		
		if (err != SA_OK) { return(err); }
		
		/* Add modifier units, if appropriate */
		if (format->BaseUnits != SAHPI_SU_UNSPECIFIED && 
		    format->ModifierUse != SAHPI_SMUU_NONE) {

			switch(format->ModifierUse) {
			case SAHPI_SMUU_BASIC_OVER_MODIFIER:
				err = oh_append_textbuffer(&working, " / ", 3); 
				if (err != SA_OK) { return(err); }
				break;
			case SAHPI_SMUU_BASIC_TIMES_MODIFIER:
				err = oh_append_textbuffer(&working, " * ", 3); 
				if (err != SA_OK) { return(err); }
				break;
			default:
				return(SA_ERR_HPI_INVALID_PARAMS);
			}
			str = oh_lookup_sensorunits(format->ModifierUnits);
			if (str == NULL) { return(SA_ERR_HPI_INVALID_PARAMS); }
			err = oh_append_textbuffer(&working, str, strlen(str)); 		
			if (err != SA_OK) { return(err); }
		}
	}

	oh_copy_textbuffer(buffer, &working);

        return(SA_OK);
}

/**
 * oh_fprint_textbuffer:
 * @stream: File handle.
 * @buffer: Pointer to SaHpiTextBuffer to be printed.
 * 
 * Prints the text data contained in SaHpiTextBuffer to a file. Data must
 * be of type SAHPI_TL_TYPE_TEXT. @buffer->DataLength is ignored.
 * The MACRO oh_print_textbuffer(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_DATA - @buffer->DataType not SAHPI_TL_TYPE_TEXT.
 **/
SaErrorT oh_fprint_textbuffer(FILE *stream, const SaHpiTextBufferT *buffer)
{
	int err;

	/* FIXME:: Why not support the other types - most represented as 8-bit 
	   ASCII; so can print without a problem */

        if (buffer->DataType == SAHPI_TL_TYPE_TEXT) {
                err = fprintf(stream, "%s\n", buffer->Data);
		if (err < 0) {
			return(SA_ERR_HPI_INVALID_PARAMS);
		}
        }
	else {
		return(SA_ERR_HPI_INVALID_DATA);
	}
	
	return(SA_OK);
}

/**
 * oh_fprint_big_textbuffer:
 * @stream: File handle.
 * @big_buffer: Pointer to oh_big_textbuffer to be printed.
 * 
 * Prints the text data contained in oh_big_textbuffer to a file. Data must
 * be of type SAHPI_TL_TYPE_TEXT. @big_buffer->DataLength is ignored.
 * The MACRO oh_print_big_textbuffer(), uses this function to print to STDOUT.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_DATA - @big_buffer->DataType not SAHPI_TL_TYPE_TEXT.
 **/
SaErrorT oh_fprint_big_textbuffer(FILE *stream, const oh_big_textbuffer *big_buffer)
{
	int err;
	
	/* FIXME:: Why not support the other types - most represented as 8-bit 
	   ASCII; so can print without a problem */

        if (big_buffer->DataType == SAHPI_TL_TYPE_TEXT) {
                err = fprintf(stream, "%s\n", big_buffer->Data);
		if (err < 0) {
			return(SA_ERR_HPI_INVALID_PARAMS);
		}
        }
	else {
		return(SA_ERR_HPI_INVALID_DATA);
	}
	
	return(SA_OK);
}

/**
 * oh_init_textbuffer:
 * @buffer: Pointer to an SaHpiTextBufferT.
 * 
 * Initializes an SaHpiTextBufferT. Assumes an english language set.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is NULL.
 **/
SaErrorT oh_init_textbuffer(SaHpiTextBufferT *buffer)
{
	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	memset(buffer, 0, sizeof(*buffer));
        buffer->DataType = SAHPI_TL_TYPE_TEXT;
        buffer->Language = SAHPI_LANG_ENGLISH;
        buffer->DataLength = 0;
        return(SA_OK);
}

static inline SaErrorT oh_init_big_textbuffer(oh_big_textbuffer *big_buffer)
{
	if (!big_buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	memset(big_buffer, 0, sizeof(*big_buffer));
        big_buffer->DataType = SAHPI_TL_TYPE_TEXT;
        big_buffer->Language = SAHPI_LANG_ENGLISH;
        big_buffer->DataLength = 0;
        return(SA_OK);
}

/**
 * oh_copy_textbuffer:
 * @dest: SaHpiTextBufferT to copy into.
 * @from:SaHpiTextBufferT to copy from.
 *
 * Initializes an SaHpiTextBufferT. Assumes an english language set.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @dest or @from is NULL.
 **/
SaErrorT oh_copy_textbuffer(SaHpiTextBufferT *dest, const SaHpiTextBufferT *from)
{
	if (!dest || !from) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        dest->DataType = from->DataType;
        dest->Language = from->Language;
        dest->DataLength = from->DataLength;
        memcpy(dest->Data, from->Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        return(SA_OK);
}

static inline SaErrorT oh_copy_big_textbuffer(oh_big_textbuffer *dest, const oh_big_textbuffer *from)
{
	if (!dest || !from) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        dest->DataType = from->DataType;
        dest->Language = from->Language;
        dest->DataLength = from->DataLength;
        memcpy(dest->Data, from->Data, OH_MAX_TEXT_BUFFER_LENGTH);
        return(SA_OK);
}

/**
 * oh_append_textbuffer:
 * @buffer: SaHpiTextBufferT to append to.
 * @from: String to be appended.
 * @size: Size of string.
 *
 * Appends a string to the @buffer->Data.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer or @from is NULL. Or
 *                             @from is NULL and size is non-zero.
 * SA_ERR_HPI_OUT_OF_SPACE - @buffer not big enough to accomodate appended string
 **/
SaErrorT oh_append_textbuffer(SaHpiTextBufferT *buffer, const char *from, size_t size)
{
        SaHpiUint8T *p;

	/* FIXME:: Add a valid_textbuffer check when routine is available */
	if (size == 0) { 
		return(SA_OK); 
	}
	if (!buffer || !from) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        if ((size + buffer->DataLength) >= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                return(SA_ERR_HPI_OUT_OF_SPACE);
        }

        /* Can't trust NULLs to be right, so use a targeted strncpy instead */
        p = buffer->Data;
        p += buffer->DataLength;
        strncpy(p, from, size);
        buffer->DataLength += size;
        
        return(SA_OK);
}

static inline SaErrorT oh_append_big_textbuffer(oh_big_textbuffer *big_buffer, const char *from, size_t size)
{
        SaHpiUint8T *p;

	/* FIXME:: Add a valid_textbuffer check when routine is available */
	if (size == 0 ) {
		return(SA_OK);
	}
	if (!big_buffer || !from) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        if ((size + big_buffer->DataLength) >= OH_MAX_TEXT_BUFFER_LENGTH) {
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }
        
        /* Can't trust NULLs to be right, so use a targeted strncpy instead */
        p = big_buffer->Data;
        p += big_buffer->DataLength;
        strncpy(p, from, size);
        big_buffer->DataLength += size;
        
        return(SA_OK);
}

#if 0
SaHpiBoolT valid_SaHpiTextBufferT(SaHpiTextBufferT text_buffer) {

	if (!valid_SaHpiTextTypeT) { return(SAHPI_FALSE); }
	if (!valid_SaHpiLanguageT) { return(SAHPI_FALSE); }
	/* Compiler checks DataLength <= SAHPI_MAX_TEXT_BUFFER_LENGTH */

	switch(text_buffer.DataType){
	case SAHPI_TL_TYPE_UNICODE:
		
		/* FIXME:: Verify unicode characters in Data */
		/* Check for even number of bytes */
		/* g_unichar_validate(gunichar ch); ???? */
		break;
	case SAHPI_TL_TYPE_BCDPLUS:

		/* FIXME:: Verify BCDPLUS characters in Data */
		/* 8-bit ASCII, '0'-'9' or space, dash, period, colon, comma, or
		   underscore only.(*/
		break;
	case SAHPI_TL_TYPE_ASCII6:
		/* FIXME:: Verify reduced character set in Data */
		/* reduced set, 0x20-0x5f only. */
		break;
	case SAHPI_TL_TYPE_TEXT:
		/* FIXME:: Any check possible for 8-bit ASCII + Latin 1? */
		break;
	case SAHPI_TL_TYPE_BINARY: /* No check possible */
		break;
	default: /* Impossible state */
		return(SA_ERR_HPI_INTERNAL_ERROR);
	}

	return(SAHPI_TRUE);
}

SaErrorT print_SaHpiTextBufferT(SaHpiTextBufferT text_buffer) {

	/* Check for valid */
        /* Support arbitrary tab/indents offsets??*/

	SaHpiUint8T Data[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	
	memset(Data, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        memcpy(Data, text_buffer.Data, text_buffer.DataLength);

	printf("TextBuffer:\n");
	printf("\tDataType: %s\n", SaHpiTextTypeT2str(text_buffer.DataType));
	printf("\tLanguage: %s\n", SaHpiLanguageT2str(text_buffer.Language));
	printf("\tDataLength: %d\n", text_buffer.DataLength);

	switch (text_buffer.DataType) {	
	case SAHPI_TL_TYPE_UNICODE:
		/* FIXME:: print unicode */
		break;
	case SAHPI_TL_TYPE_BCDPLUS:
	case SAHPI_TL_TYPE_ASCII6:
	case SAHPI_TL_TYPE_TEXT:
		printf("\tData: %s\n", Data);
		break;
	case SAHPI_TL_TYPE_BINARY:
		printf("\tData: %x\n", Data);
		break;
	default:
		printf("\tData: ERROR! Invalid HPI SaHpiTextTypeT type\n");
	}

	return SA_OK;
}

#endif
