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
 * Contributors:
 *      Racing Guo <racing.guo@intel.com>
 */

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <SaHpi.h>
#include <oh_utils.h>

static inline SaErrorT oh_init_bigtext(oh_big_textbuffer *big_buffer);
static inline SaErrorT oh_append_bigtext(oh_big_textbuffer *big_buffer, const char *from);
static inline SaErrorT oh_copy_bigtext(oh_big_textbuffer *dest, const oh_big_textbuffer *from);

static SaErrorT oh_append_offset(oh_big_textbuffer *buffer, int offsets);
static SaErrorT oh_build_sensorrec(oh_big_textbuffer *buffer, const SaHpiSensorRecT *sensor, int offsets);
static SaErrorT oh_build_sensordataformat(oh_big_textbuffer *buffer, const SaHpiSensorDataFormatT *format, int offsets);
static SaErrorT oh_build_sensorthddefn(oh_big_textbuffer *buffer, const SaHpiSensorThdDefnT *tdef, int offsets);
static SaErrorT oh_build_threshold_mask(oh_big_textbuffer *buffer, const SaHpiSensorThdMaskT tmask, int offsets);
static SaErrorT oh_build_textbuffer(oh_big_textbuffer *buffer, const SaHpiTextBufferT *textbuffer, int offsets);

static SaErrorT oh_build_ctrlrec(oh_big_textbuffer *textbuf, const SaHpiCtrlRecT *ctrlrec, int offsets);
static SaErrorT oh_build_invrec(oh_big_textbuffer *textbuff, const SaHpiInventoryRecT *invrec, int offsets);
static SaErrorT oh_build_wdogrec(oh_big_textbuffer *textbuff, const SaHpiWatchdogRecT *wdogrec, int offsets);
static SaErrorT oh_build_annrec(oh_big_textbuffer *textbuff, const SaHpiAnnunciatorRecT *annrec, int offsets);

static SaErrorT oh_build_event(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_resource(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_domain(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_sensor(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_sensor_enable_change(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_hotswap(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_watchdog(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_hpi_sw(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_oem(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);
static SaErrorT oh_build_event_user(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets);

/************************************************************************
 * NOTES!
 *
 * - Several error checks can be removed if valid_xxx routines are defined
 *   for input structures. If this happens, several of the default switch
 *   statements should also return SA_ERR_HPI_INTERNAL_ERROR instead
 *   of SA_ERR_HPI_INVALID_PARMS.
 ************************************************************************/

/**
 * oh_lookup_manufacturerid:
 * @value: enum value of type SaHpiManufacturerIdT.
 * @buffer:  Location to store the string.
 *
 * Converts @value into a string based on @value's enum definition
 * in http://www.iana.org/assignments/enterprise-numbers.
 * String is stored in an SaHpiTextBufferT data structure.
 * 
 * Only a few of the manufacturers in that list have been defined.
 * For all others, this routine returns "Unknown Manufacturer".
 * Feel free to add your own favorite manufacturer to this routine.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is  NULL.
 **/
SaErrorT oh_decode_manufacturerid(SaHpiManufacturerIdT value, SaHpiTextBufferT *buffer) 
{
	SaErrorT err;
	SaHpiTextBufferT working;

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_textbuffer(&working);
	if (err) { return(err); }

	switch(value) {
	case SAHPI_MANUFACTURER_ID_UNSPECIFIED:
		err = oh_append_textbuffer(&working, "UNSPECIFIED Manufacturer");
		if (err) { return(err); }
		break;
	case 2:
		err = oh_append_textbuffer(&working,"IBM");
		if (err) { return(err); }
		break;
	default:
		err = oh_append_textbuffer(&working,  "Unknown Manufacturer");
		if (err) { return(err); }
	}

 	oh_copy_textbuffer(buffer, &working);

	return(SA_OK);
}

/**
 * oh_decode_sensorreading: 
 * @reading: SaHpiSensorReadingT to convert.
 * @format: SaHpiDataFormatT for the sensor reading.
 * @buffer: Location to store the converted string.
 *
 * Converts an HPI sensor reading and format into a string. 
 * String is stored in an SaHpiTextBufferT data structure.
 * 
 * Returns: 
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_CMD - @format or @reading have IsSupported == FALSE.
 * SA_ERR_HPI_INVALID_DATA - @format and @reading types don't match.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL; @reading type != @format type.
 * SA_ERR_HPI_OUT_OF_SPACE - @buffer not big enough to accomodate appended string
 **/
SaErrorT oh_decode_sensorreading(SaHpiSensorReadingT reading,
				 SaHpiSensorDataFormatT format,
				 SaHpiTextBufferT *buffer)
{
	SaErrorT err;
	SaHpiTextBufferT working;
        char text[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	char str[SAHPI_SENSOR_BUFFER_LENGTH + 1];

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
        if (!reading.IsSupported || !format.IsSupported) {
                return(SA_ERR_HPI_INVALID_CMD);
        }
        if (reading.Type != format.ReadingType) {
		
		/* Check for special case */
		if (!( (reading.Type == SAHPI_SENSOR_READING_TYPE_BUFFER) && 
		    ((strncmp(reading.Value.SensorBuffer,"(No temperature)", sizeof("(No temperature)")) == 0) ||
		    		(strncmp(reading.Value.SensorBuffer,"Not Readable!", sizeof("Not Readable!")) == 0)) ))
                	return(SA_ERR_HPI_INVALID_DATA);
        }
	
	oh_init_textbuffer(&working);
	memset(text, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);

        switch(reading.Type) {
        case SAHPI_SENSOR_READING_TYPE_INT64:
                snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH,
			 "%lld", reading.Value.SensorInt64);
		err = oh_append_textbuffer(&working, text);
		if (err) { return(err); }
                break;
        case SAHPI_SENSOR_READING_TYPE_UINT64:
		snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH,
			 "%llu", reading.Value.SensorUint64);
		err = oh_append_textbuffer(&working, text);
		if (err) { return(err); }
                break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                snprintf(text, SAHPI_MAX_TEXT_BUFFER_LENGTH, 
			 "%le", reading.Value.SensorFloat64);
		err = oh_append_textbuffer(&working, text);
		if (err) { return(err); }
 		break;
        case SAHPI_SENSOR_READING_TYPE_BUFFER:
		/* In case Sensor Buffer contains no end of string deliminter */
		memset(str, 0, SAHPI_SENSOR_BUFFER_LENGTH + 1);
		strncpy(str, reading.Value.SensorBuffer, SAHPI_SENSOR_BUFFER_LENGTH);
		err = oh_append_textbuffer(&working, str);
		if (err) { return(err); }
                break;
	default:
		return(SA_ERR_HPI_INVALID_PARAMS);
        }
        
        if (format.Percentage) {
		err = oh_append_textbuffer(&working, "%"); 
		if (err) { return(err); }
        }
	else {
		/* Add units */
		if (format.BaseUnits != SAHPI_SU_UNSPECIFIED) {
			const char *str;

			err = oh_append_textbuffer(&working, " ");
			if (err) { return(err); }
			str = oh_lookup_sensorunits(format.BaseUnits);
			if (str == NULL) { return(SA_ERR_HPI_INVALID_PARAMS); }
			err = oh_append_textbuffer(&working, str);
			if (err) { return(err); }
		}
		
		/* Add modifier units, if appropriate */
		if (format.BaseUnits != SAHPI_SU_UNSPECIFIED && 
		    format.ModifierUse != SAHPI_SMUU_NONE) {
			const char *str;

			switch(format.ModifierUse) {
			case SAHPI_SMUU_BASIC_OVER_MODIFIER:
				err = oh_append_textbuffer(&working, " / "); 
				if (err) { return(err); }
				break;
			case SAHPI_SMUU_BASIC_TIMES_MODIFIER:
				err = oh_append_textbuffer(&working, " * "); 
				if (err) { return(err); }
				break;
			default:
				return(SA_ERR_HPI_INVALID_PARAMS);
			}
			str = oh_lookup_sensorunits(format.ModifierUnits);
			if (str == NULL) { return(SA_ERR_HPI_INVALID_PARAMS); }
			err = oh_append_textbuffer(&working, str); 		
			if (err) { return(err); }
		}
	}

 	oh_copy_textbuffer(buffer, &working);

        return(SA_OK);
}

/**
 * oh_encode_sensorreading:
 * @buffer: Location of SaHpiTextBufferT containing the string to 
 *          convert into a SaHpiSensorReadingT.
 * @type: SaHpiSensorReadingTypeT of converted reading.
 * @reading: SaHpiSensorReadingT location to store converted string.
 * 
 * Converts @buffer->Data string to an HPI SaHpiSensorReadingT structure.
 * Generally @buffer->Data is created by oh_decode_sensorreading() or has
 * been built by a plugin, which gets string values for sensor readings (e.g.
 * through SNMP OID commands). Any non-numeric portion of the string is
 * discarded. For example, the string "-1.43 Volts" is converted to -1.43
 * of type @type.
 * 
 * If type = SAHPI_SENSOR_READING_TYPE_BUFFER, and @buffer->Data > 
 * SAHPI_SENSOR_BUFFER_LENGTH, data is truncated to fit into the reading
 * buffer.
 *
 * Notes!
 * - Numerical strings can contain commas but it is assummed that strings follow
 *   US format (e.g. "1,000,000" = 1 million; not "1.000.000").
 * - Decimal points are always preceded by at least one number (e.g. "0.9")
 * - Numerical percentage strings like "9%" are converted into a float 0.09
 * - Hex notation is not supported (e.g. "0x23")
 * - Scientific notation is not supported (e.g. "e+02").
 *
 * Returns: 
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL; Invalid @type.
 * SA_ERR_HPI_INVALID_DATA - Converted @buffer->Data too large for @type; cannot
 *                           convert string into valid number; @type incorrect
 *                           for resulting number (e.g. percentage not a float).
 **/
SaErrorT oh_encode_sensorreading(SaHpiTextBufferT *buffer,
				 SaHpiSensorReadingTypeT type,
				 SaHpiSensorReadingT *reading)
{
        char  numstr[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	char *endptr;
        int   i, j;
	int   found_sign, found_number, found_float, in_number;
	int   is_percent = 0;
        SaHpiFloat64T num_float64 = 0.0;
        SaHpiInt64T   num_int64 = 0;
        SaHpiUint64T  num_uint64 = 0;
	SaHpiSensorReadingT working;

	if (!buffer || !reading ||
	    buffer->Data == NULL || buffer->Data[0] == '\0' ||
	    !oh_lookup_sensorreadingtype(type)) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

        if (type == SAHPI_SENSOR_READING_TYPE_BUFFER) {
		reading->IsSupported = SAHPI_TRUE;
		reading->Type = type;
		strncpy(reading->Value.SensorBuffer, buffer->Data, SAHPI_SENSOR_BUFFER_LENGTH);
		return(SA_OK);
        }

	memset(numstr, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
        memset(&working, 0, sizeof(SaHpiSensorReadingT));
	working.IsSupported = SAHPI_TRUE;
	working.Type = type;

	/* Search string and pull out first numeric string.
         * The strtol type functions below are pretty good at handling
         * non-numeric junk in string, but they don't handle spaces
         * after a sign or commas within a number.
         * So we normalize the string a bit first.
	 */
	j = found_sign = in_number = found_number = found_float = 0;
	for (i=0; i<buffer->DataLength && !found_number; i++) {
		if (buffer->Data[i] == '+' || buffer->Data[i] == '-') {
			if (found_sign) { 
				dbg("Cannot parse multiple sign values");
				return(SA_ERR_HPI_INVALID_DATA);
			}
			found_sign = 1;
			numstr[j] = buffer->Data[i];
			j++;
		}
		if (isdigit(buffer->Data[i])) {
			if (!found_number) {
				in_number = 1;
				numstr[j] = buffer->Data[i];
				j++;
			}
		}
		else { /* Strip non-numerics */
			if (buffer->Data[i] == '.') { /* Unless its a decimal point */
				if (in_number) {
					if (found_float) { 
						dbg("Cannot parse multiple decimal points");
						return(SA_ERR_HPI_INVALID_DATA);
					}
					found_float = 1;
					numstr[j] = buffer->Data[i];
					j++;
				}
			}
			else {
				/* Delete commas but don't end search for more numbers */
				if (in_number && buffer->Data[i] != ',') { 
					found_number = 1;
				}
			}
		}
	}
	
	if (found_number || in_number) { /* in_number means string ended in a digit character */
		for (j=i-1; j<buffer->DataLength; j++) {
			if (buffer->Data[j] == '%') {
				is_percent = 1;
				break;
			}
		}
		found_number = 1;
	}

	if ((is_percent || found_float) && type != SAHPI_SENSOR_READING_TYPE_FLOAT64) {
		dbg("Number and type incompatible");
		return(SA_ERR_HPI_INVALID_DATA);
	}

	/* Convert string to number */
        switch (type) {
        case SAHPI_SENSOR_READING_TYPE_INT64:
		if (found_number) {
			errno = 0;
			num_int64 = strtoll(numstr, &endptr, 10);
			if (errno) {
				dbg("strtoll failed, errno=%d", errno);
				return(SA_ERR_HPI_INVALID_DATA);
			}
			if (*endptr != '\0') {
				dbg("strtoll failed: End Pointer=%s", endptr);
				return(SA_ERR_HPI_INVALID_DATA);
			}
		}
		else { /* No number in string */
			num_int64 = 0;
		}

                working.Value.SensorInt64 = num_int64;
                break;

        case SAHPI_SENSOR_READING_TYPE_UINT64:
		if (found_number) {
			errno = 0;
			num_uint64 = strtoull(numstr, &endptr, 10);
			if (errno) {
				dbg("strtoull failed, errno=%d", errno);
				return(SA_ERR_HPI_INVALID_DATA);
			}
			if (*endptr != '\0') {
				dbg("strtoull failed: End Pointer=%s", endptr);
				return(SA_ERR_HPI_INVALID_DATA);
			}
		}
		else { /* No number in string */
			num_uint64 = 0;
		}

                working.Value.SensorUint64 = num_uint64;
                break;

        case SAHPI_SENSOR_READING_TYPE_FLOAT64:
		if (found_number) {		
			errno = 0;
			num_float64 = strtold(numstr, &endptr);
			if (errno) {
				dbg("strtold failed, errno=%d", errno);
				return(SA_ERR_HPI_INVALID_DATA);
			}
			if (*endptr != '\0') {
				dbg("strtold failed: End Pointer=%s", endptr);
				return(SA_ERR_HPI_INVALID_DATA);
			}
			
			if (is_percent) {
				working.Value.SensorFloat64 = num_float64/100.0;
			}
			else {
				working.Value.SensorFloat64 = num_float64;
			}
		}
		else { /* No number in string */
			num_float64 = 0;
		}
                break;

        default: /* Should never get here */
                dbg("Invalid type=%d", type);
		return(SA_ERR_HPI_INTERNAL_ERROR);
        }

	*reading = working;

	return(SA_OK);
}

/**
 * oh_fprint_text:
 * @stream: File handle.
 * @buffer: Pointer to SaHpiTextBufferT to be printed.
 * 
 * Prints the text data contained in SaHpiTextBufferT to a file. Data must
 * be of type SAHPI_TL_TYPE_TEXT. @buffer->DataLength is ignored.
 * The MACRO oh_print_text(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_DATA - @buffer->DataType not SAHPI_TL_TYPE_TEXT.
 **/
SaErrorT oh_fprint_text(FILE *stream, const SaHpiTextBufferT *buffer)
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
 * oh_fprint_bigtext:
 * @stream: File handle.
 * @big_buffer: Pointer to oh_big_textbuffer to be printed.
 * 
 * Prints the text data contained in oh_big_textbuffer to a file. Data must
 * be of type SAHPI_TL_TYPE_TEXT. @big_buffer->DataLength is ignored.
 * The MACRO oh_print_bigtext(), uses this function to print to STDOUT.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_DATA - @big_buffer->DataType not SAHPI_TL_TYPE_TEXT.
 **/
SaErrorT oh_fprint_bigtext(FILE *stream, const oh_big_textbuffer *big_buffer)
{
	int err;
	
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
 * Initializes an SaHpiTextBufferT. Assumes an English language set.
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

static inline SaErrorT oh_init_bigtext(oh_big_textbuffer *big_buffer)
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
 * Copies one SaHpiTextBufferT structure to another.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
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

static inline SaErrorT oh_copy_bigtext(oh_big_textbuffer *dest, const oh_big_textbuffer *from)
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
 *
 * Appends a string to @buffer->Data.
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 * SA_ERR_HPI_OUT_OF_SPACE - @buffer not big enough to accomodate appended string.
 **/
SaErrorT oh_append_textbuffer(SaHpiTextBufferT *buffer, const char *from)
{
        SaHpiUint8T *p;
	uint size;

	if (!buffer || !from) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	size = strlen(from);
        if ((size + buffer->DataLength) >= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		dbg("Cannot append to text buffer. Bufsize=%d, size=%u",
		    buffer->DataLength, size);
                return(SA_ERR_HPI_OUT_OF_SPACE);
        }

        /* Can't trust NULLs to be right, so use a targeted strncpy instead */
        p = buffer->Data;
        p += buffer->DataLength;
        strncpy(p, from, size);
        buffer->DataLength += size;
        
        return(SA_OK);
}

static inline SaErrorT oh_append_bigtext(oh_big_textbuffer *big_buffer, const char *from)
{
        SaHpiUint8T *p;
	uint size;

	if (!big_buffer || !from) {
		dbg("Invalid parameters");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	size = strlen(from);
        if ((size + big_buffer->DataLength) >= OH_MAX_TEXT_BUFFER_LENGTH) {
		dbg("Cannot append to buffer. Bufsize=%d, size=%u",
		    big_buffer->DataLength, size);
                return(SA_ERR_HPI_INTERNAL_ERROR);
        }
        
        /* Can't trust NULLs to be right, so use a targeted strncpy instead */
        p = big_buffer->Data;
        p += big_buffer->DataLength;
        strncpy(p, from, size);
        big_buffer->DataLength += size;
        
        return(SA_OK);
}

static inline SaErrorT oh_append_data(oh_big_textbuffer *big_buffer, const SaHpiUint8T *from,
				      SaHpiUint8T len)
{
	SaHpiUint8T i;

	if (!big_buffer || !from || len == 0) {
		dbg("Invalid parameters");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	for (i = 0; i < len; i++) {
        	SaHpiUint8T *p;
		char buff[10];
		int slen;
		
		memset(buff, 0 ,sizeof(buff));	
		snprintf(buff, 10, "%d ", *(from + i));

		slen = strlen(buff);

        	if ((slen + big_buffer->DataLength) >= OH_MAX_TEXT_BUFFER_LENGTH) {
			dbg("Cannot append to buffer. Bufsize=%d, len=%d",
					big_buffer->DataLength, len);
			return(SA_ERR_HPI_INTERNAL_ERROR);
		}

        	p = big_buffer->Data;
        	p += big_buffer->DataLength;
        	strncpy(p, buff, slen);
        	big_buffer->DataLength += slen;
	}

        return(SA_OK);
}

/* Append an arbitrary number of fixed offset strings to a big text buffer */
static SaErrorT oh_append_offset(oh_big_textbuffer *buffer, int offsets)
{
	int i;
	
	for (i=0; i < offsets; i++) {
		oh_append_bigtext(buffer, OH_PRINT_OFFSET);
	}

	return(SA_OK);
}

/**
 * oh_fprint_ctrlrec:
 * @stream: File handle.
 * @control: Pointer to SaHpiCtrlRecT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints a control's SaHpiCtrlRecT data to a file. 
 * The MACRO oh_print_ctrlrec(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/

SaErrorT oh_fprint_ctrlrec(FILE *stream, const SaHpiCtrlRecT *control, int offsets)
{
	SaErrorT err;
	oh_big_textbuffer buffer;
	
	if (!stream || !control) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);
	err = oh_build_ctrlrec(&buffer, control, offsets);
	if (err) { return(err); }

	err = oh_fprint_bigtext(stream, &buffer);
	if (err) { return(err); }
	
	return(SA_OK);
}

/**
 * oh_fprint_watchdogrec:
 * @stream: File handle.
 * @watchdog: Pointer to SaHpiWatchdogRecT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints a watchdog's SaHpiWatchdogRecT data to a file. 
 * The MACRO oh_print_watchdogrec(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_watchdogrec(FILE *stream, const SaHpiWatchdogRecT *watchdog, int offsets)
{
	SaErrorT err;
	oh_big_textbuffer buffer;
	
	if (!stream || !watchdog) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);
	err = oh_build_wdogrec(&buffer, watchdog, offsets);
	if (err) { return(err); }

	err = oh_fprint_bigtext(stream, &buffer);
	if (err) { return(err); }
	
	return(SA_OK);
}

/**
 * oh_fprint_sensorrec:
 * @stream: File handle.
 * @sensor: Pointer to SaHpiSensorRecT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints a sensor's SaHpiSensorRecT data to a file. 
 * The MACRO oh_print_sensorrec(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_sensorrec(FILE *stream, const SaHpiSensorRecT *sensor, int offsets)
{
	int err;
	oh_big_textbuffer buffer;
	
	if (!stream || !sensor) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);
	err = oh_build_sensorrec(&buffer, sensor, offsets);
	if (err) { return(err); }

	err = oh_fprint_bigtext(stream, &buffer);
	if (err) { return(err); }
	
	return(SA_OK);
}

static SaErrorT oh_build_sensorrec(oh_big_textbuffer *buffer, const SaHpiSensorRecT *sensor, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;
	SaHpiTextBufferT tmpbuffer;

	/* Sensor Num */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Sensor Num: %d\n", sensor->Num);
	oh_append_bigtext(buffer, str);
	offsets++;

	/* Sensor Type */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s\n", 
		 oh_lookup_sensortype(sensor->Type));
	oh_append_bigtext(buffer, str);

	/* Sensor Category */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Category: %s\n", 
		 oh_lookup_eventcategory(sensor->Category));
	oh_append_bigtext(buffer, str);

	/* Sensor Enable Control */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EnableCtrl: %s\n",
		(sensor->EnableCtrl == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);

	/* Sensor Event Control */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EventCtrl: %s\n", 
		 oh_lookup_sensoreventctrl(sensor->EventCtrl));
	oh_append_bigtext(buffer, str);

	/* Sensor Supported Events */
	oh_append_offset(buffer, offsets);
	oh_append_bigtext(buffer, "Events: ");
	err = oh_decode_eventstate(sensor->Events, sensor->Category, &tmpbuffer);
	if (err) {oh_append_bigtext(buffer, "\n"); return(err); }
	oh_append_bigtext(buffer, tmpbuffer.Data);
	oh_append_bigtext(buffer, "\n");
	
	/* Sensor Data Format */
	err = oh_build_sensordataformat(buffer, &(sensor->DataFormat), offsets);
	if (err) { return(err); }

	/* Sensor Threshold Definition */
	err = oh_build_sensorthddefn(buffer, &(sensor->ThresholdDefn), offsets);
	if (err) { return(err); }

	/* Sensor OEM Data */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OEM: %x\n", sensor->Oem);
	oh_append_bigtext(buffer, str);

	/* printf("SENSOR LENGTH = %d\n", strlen(buffer->Data)); */
	return(SA_OK);
}

static SaErrorT oh_build_sensordataformat(oh_big_textbuffer *buffer,
					  const SaHpiSensorDataFormatT *format, 
					  int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;
	SaHpiTextBufferT reading_buffer;

	/* Sensor Data Format Title */
	oh_append_offset(buffer, offsets);
	oh_append_bigtext(buffer, "Data Format:\n");
	offsets++;
		
	/* Sensor Data Format IsSupported */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IsSupported: %s\n",
		 (format->IsSupported == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);
		
	if (format->IsSupported) {

		/* Sensor Data Format Reading Type */
		oh_append_offset(buffer, offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Reading Type: %s\n", 
			 oh_lookup_sensorreadingtype(format->ReadingType));
		oh_append_bigtext(buffer, str);
		
		if (format->ReadingType != SAHPI_SENSOR_READING_TYPE_BUFFER) {

			/* Sensor Data Format Base Units */
			oh_append_offset(buffer, offsets);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Base Unit: %s\n", 
				 oh_lookup_sensorunits(format->BaseUnits));
			oh_append_bigtext(buffer, str);
			
			/* Sensor Data Format Modifier Units */
			if (format->ModifierUnits) {
				oh_append_offset(buffer, offsets);
				snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Modifier Unit: %s\n", 
					 oh_lookup_sensorunits(format->ModifierUnits));
				oh_append_bigtext(buffer, str);
				/* Sensor Data Format Modifier Use */
				oh_append_offset(buffer, offsets);
				snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Modifier Use: %s\n",
					 oh_lookup_sensormodunituse(format->ModifierUse));
				oh_append_bigtext(buffer, str);
			}
			
			/* Sensor Data Format Percentage */
			oh_append_offset(buffer, offsets);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Percentage: %s\n",
				 (format->Percentage == SAHPI_TRUE) ? "TRUE" : "FALSE");
			oh_append_bigtext(buffer, str);
		
			/* Sensor Data Format Max Range */
			if (format->Range.Flags & SAHPI_SRF_MAX && 
			    format->Range.Max.IsSupported) { 
				oh_append_offset(buffer, offsets);
				oh_append_bigtext(buffer, "Range Max: ");
				
				err = oh_decode_sensorreading(format->Range.Max,
							      *format,
							      &reading_buffer);
				if (err) { return(err); }
				oh_append_bigtext(buffer, reading_buffer.Data);
				oh_append_bigtext(buffer, "\n");
			}

			/* Sensor Data Format Min Range */
			if (format->Range.Flags & SAHPI_SRF_MIN && 
			    format->Range.Min.IsSupported) { 
				oh_append_offset(buffer, offsets);
				oh_append_bigtext(buffer, "Range Min: ");
				
				err = oh_decode_sensorreading(format->Range.Min,
							      *format,
							      &reading_buffer);
				if (err) { return(err); }
				oh_append_bigtext(buffer, reading_buffer.Data);
				oh_append_bigtext(buffer, "\n");
			}

			/* Sensor Data Format Nominal Range */
			if (format->Range.Flags & SAHPI_SRF_NOMINAL && 
			    format->Range.Nominal.IsSupported) { 
				oh_append_offset(buffer, offsets);
				oh_append_bigtext(buffer, "Range Nominal: ");
				
				err = oh_decode_sensorreading(format->Range.Nominal,
							      *format,
							      &reading_buffer);
				if (err) { return(err); }
				oh_append_bigtext(buffer, reading_buffer.Data);
				oh_append_bigtext(buffer, "\n");
			}

			/* Sensor Data Format Normal Max Range */
			if (format->Range.Flags & SAHPI_SRF_NORMAL_MAX && 
			    format->Range.NormalMax.IsSupported) { 
				oh_append_offset(buffer, offsets);
				oh_append_bigtext(buffer, "Range Normal Max: ");
				
				err = oh_decode_sensorreading(format->Range.NormalMax,
							      *format,
							      &reading_buffer);
				if (err) { return(err); }
				oh_append_bigtext(buffer, reading_buffer.Data);
				oh_append_bigtext(buffer, "\n");
			}

			/* Sensor Data Format Normal Min Range */
			if (format->Range.Flags & SAHPI_SRF_NORMAL_MIN && 
			    format->Range.NormalMin.IsSupported) { 
				oh_append_offset(buffer, offsets);
				oh_append_bigtext(buffer, "Range Normal Min: ");
				
				err = oh_decode_sensorreading(format->Range.NormalMin,
							      *format,
							      &reading_buffer);
				if (err) { return(err); }
				oh_append_bigtext(buffer, reading_buffer.Data);
				oh_append_bigtext(buffer, "\n");
			}

			/* Sensor Data Format Accuracy Factor */
			oh_append_offset(buffer, offsets);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Accuracy: %le\n", format->AccuracyFactor);	
			oh_append_bigtext(buffer, str);
		}
	}
	
	return(SA_OK);
}

static SaErrorT oh_build_sensorthddefn(oh_big_textbuffer *buffer,
				       const SaHpiSensorThdDefnT *tdef, 
				       int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;

	/* Sensor Threshold Definition Title */
	oh_append_offset(buffer, offsets);
	oh_append_bigtext(buffer, "Threshold Definitions:\n");
	offsets++;

	/* Sensor Threshold Definition IsAccessible */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IsAccessible: %s\n",
		 (tdef->IsAccessible == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);
	
	if (tdef->IsAccessible) {

		/* Sensor Threshold Read Threshold */
		if (tdef->ReadThold) {
			oh_append_offset(buffer, offsets);
			oh_append_bigtext(buffer, "Readable Thresholds:\n"); 

			err = oh_build_threshold_mask(buffer, tdef->ReadThold, offsets + 1);
			if (err) { return(err); }
		}
		
		/* Sensor Threshold Write Threshold */
		if (tdef->WriteThold) {
			oh_append_offset(buffer, offsets);
			oh_append_bigtext(buffer, "Writeable Thresholds:\n");

			err = oh_build_threshold_mask(buffer, tdef->WriteThold, offsets + 1);
			if (err) { return(err); }
		}

		/* Sensor Threshold Nonlinear */
		oh_append_offset(buffer, offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Nonlinear: %s\n",
			 (tdef->Nonlinear == SAHPI_TRUE) ? "TRUE" : "FALSE");
		oh_append_bigtext(buffer, str);
	}	

	return(SA_OK);
}

static SaErrorT oh_build_threshold_mask(oh_big_textbuffer *buffer,
					const SaHpiSensorThdMaskT tmask, 
					int offsets)
{
	int i;

	oh_append_offset(buffer, offsets);

	if (tmask & SAHPI_STM_LOW_MINOR) {
		oh_append_bigtext(buffer, "LOW_MINOR");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_LOW_MAJOR) {
		oh_append_bigtext(buffer, "LOW_MAJOR");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_LOW_CRIT) {
		oh_append_bigtext(buffer, "LOW_CRIT");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_LOW_HYSTERESIS) {
		oh_append_bigtext(buffer, "LOW_HYSTERESIS");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_UP_MINOR) {
		oh_append_bigtext(buffer, "UP_MINOR");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_UP_MAJOR) {
		oh_append_bigtext(buffer, "UP_MAJOR");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_UP_CRIT) {
		oh_append_bigtext(buffer, "UP_CRIT");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}
	if (tmask & SAHPI_STM_UP_HYSTERESIS) {
		oh_append_bigtext(buffer, "UP_HYSTERESIS");
		oh_append_bigtext(buffer, OH_ENCODE_DELIMITER);
	}

	/* Remove last delimiter; add NL */
	for (i=0; i<OH_ENCODE_DELIMITER_LENGTH + 1; i++) {
		buffer->Data[buffer->DataLength - i] = 0x00;
	}
	buffer->DataLength = buffer->DataLength - (i-1);
	oh_append_bigtext(buffer, "\n");

	return(SA_OK);
}

/**
 * oh_fprint_idrfield:
 * @stream: File handle.
 * @thisfield: Pointer to SaHpiIdrFieldT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiIdrFieldT struct to a file.
 * The MACRO oh_print_idrfield(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 **/
SaErrorT oh_fprint_idrfield(FILE *stream, const SaHpiIdrFieldT *thisfield, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;
	SaErrorT err;

	if (!stream || !thisfield) return(SA_ERR_HPI_INVALID_PARAMS);
					
	oh_init_bigtext(&mybuf);
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Field Id: %d\n", thisfield->FieldId);
	oh_append_bigtext(&mybuf, str);
						
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Field Type: %s\n",
		 oh_lookup_idrfieldtype(thisfield->Type));
	oh_append_bigtext(&mybuf, str);
						
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ReadOnly: %s\n",
		 (thisfield->ReadOnly == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(&mybuf, str);
						
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DataType: %s\n",
		 oh_lookup_texttype(thisfield->Field.DataType));
	oh_append_bigtext(&mybuf, str);
						
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Language: %s\n",
		 oh_lookup_language(thisfield->Field.Language));
	oh_append_bigtext(&mybuf, str);
						
	oh_append_offset(&mybuf, offsets);
	oh_append_bigtext(&mybuf, "Content: ");
	if (thisfield->Field.DataLength == 0)
		oh_append_bigtext(&mybuf, "NULL\n");
	else {
		if (thisfield->Field.DataType == SAHPI_TL_TYPE_BINARY)
			oh_append_data(&mybuf, thisfield->Field.Data, thisfield->Field.DataLength);
		else
			oh_append_bigtext(&mybuf, thisfield->Field.Data);
	}
							
	err = oh_fprint_bigtext(stream, &mybuf);

	return(err);
}

/**
 * oh_fprint_idrareaheader:
 * @stream: File handle.
 * @areaheader: Pointer to SaHpiIdrAreaHeaderT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiIdrAreaHeaderT struct to a file.
 * The MACRO oh_print_idrareaheader(), uses this function to print to STDOUT. 
 *
 * Returns:       
 * SA_OK - normal operation.
 **/
SaErrorT oh_fprint_idrareaheader(FILE *stream, const SaHpiIdrAreaHeaderT *areaheader, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;
	SaErrorT err;

	if (!stream || !areaheader) return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_init_bigtext(&mybuf);
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "AreaId: %d\n", areaheader->AreaId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "AreaType: %s\n",
		 oh_lookup_idrareatype(areaheader->Type));
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ReadOnly: %s\n",
		 (areaheader->ReadOnly == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "NumFields: %d\n", areaheader->NumFields);
	oh_append_bigtext(&mybuf, str);

	err = oh_fprint_bigtext(stream, &mybuf); 					
	return(err);
}

/**
 * oh_fprint_idrinfo:
 * @stream: File handle.
 * @idrinfo: Pointer to SaHpiIdrInfoT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiIdrInfoT struct to a file.
 * The MACRO oh_print_idrinfo(), uses this function to print to STDOUT. 
 *
 * Returns:       
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_idrinfo(FILE *stream, const SaHpiIdrInfoT *idrinfo, int offsets)
{
	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;
	
	if (!stream || !idrinfo) return(SA_ERR_HPI_INVALID_PARAMS);
			
	oh_init_bigtext(&mybuf);
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IdrId: %d\n", idrinfo->IdrId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "UpdateCount: %d\n", idrinfo->UpdateCount);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ReadOnly: %s\n",
				(idrinfo->ReadOnly == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "NumAreas: %d\n", idrinfo->NumAreas);
	oh_append_bigtext(&mybuf, str);

	err = oh_fprint_bigtext(stream, &mybuf); 					
	return(err);
}

SaErrorT oh_fprint_textbuffer(FILE *stream, const SaHpiTextBufferT *textbuffer, int offsets) {

	SaErrorT err;
	oh_big_textbuffer buffer;
	
	if (!stream || !textbuffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);
	err = oh_build_textbuffer(&buffer, textbuffer, offsets);
	if (err) { return(err); }

	err = oh_fprint_bigtext(stream, &buffer);

	return(err);
}

static SaErrorT oh_build_textbuffer(oh_big_textbuffer *buffer, const SaHpiTextBufferT *textbuffer, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	/* Text Buffer Data Type */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Data Type: %s\n", 
		 oh_lookup_texttype(textbuffer->DataType));
	oh_append_bigtext(buffer, str);

	/* Text Buffer Language */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Language: %s\n", 
		 oh_lookup_language(textbuffer->Language));
	oh_append_bigtext(buffer, str);
	
	/* Text Buffer Data Length */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Data Length: %d\n", 
		 textbuffer->DataLength);
	oh_append_bigtext(buffer, str);

	/* Text Buffer Data */
	if (textbuffer->DataLength) {
		oh_append_offset(buffer, offsets);
		memset(str, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH);
		oh_append_bigtext(buffer, "Data: ");
		if (textbuffer->DataType == SAHPI_TL_TYPE_BINARY)
			oh_append_data(buffer, textbuffer->Data, textbuffer->DataLength);
		else
			oh_append_bigtext(buffer, textbuffer->Data);
		oh_append_bigtext(buffer, "\n");
	}

	return(SA_OK);
}

/**
 * oh_decode_capability:
 * @ResourceCapabilities: enum value of type SaHpiCapabilitiesT.
 * @buffer:  Location to store the string.
 *
 * Converts @ResourceCapabilities type into a string based on 
 * @ResourceCapabilities HPI definition. For example:
 * @ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE | SAHPI_CAPABILITY_EVENT_LOG
 * returns a string "RESOURCE | EVENT_LOG".
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - @buffer is NULL
 **/
SaErrorT oh_decode_capability(SaHpiCapabilitiesT ResourceCapabilities,
			      SaHpiTextBufferT *buffer)
{
	int found, i;
	SaErrorT err;
	SaHpiTextBufferT working;

	if (!buffer) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	err = oh_init_textbuffer(&working);
	if (err) { return(err); }

	found = 0;
        if (ResourceCapabilities & SAHPI_CAPABILITY_AGGREGATE_STATUS) {
		found++;
		err = oh_append_textbuffer(&working, "AGGREGATE_STATUS | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR) {
		found++;
		err = oh_append_textbuffer(&working, "ANNUNCIATOR | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
		found++;
		err = oh_append_textbuffer(&working, "CONFIGURATION | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_CONTROL) {
		found++;
		err = oh_append_textbuffer(&working, "CONTROL | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG) {
		found++;
		err = oh_append_textbuffer(&working, "EVENT_LOG | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS) {
		found++;
		err = oh_append_textbuffer(&working, "EVT_DEASSERTS | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_FRU) {
		found++;
		err = oh_append_textbuffer(&working, "FRU | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA) {
		found++;
		err = oh_append_textbuffer(&working, "INVENTORY_DATA | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP) {
		found++;
		err = oh_append_textbuffer(&working, "MANAGED_HOTSWAP | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_POWER) {
		found++;
		err = oh_append_textbuffer(&working, "POWER | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_RDR) {
		found++;
		err = oh_append_textbuffer(&working, "RDR | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_RESET) {
		found++;
		err = oh_append_textbuffer(&working, "RESET | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE) {
		found++;
		err = oh_append_textbuffer(&working, "RESOURCE | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_SENSOR) {
		found++;
		err = oh_append_textbuffer(&working, "SENSOR | ");
		if (err) { return(err); }
	}
        if (ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG) {
		found++;
		err = oh_append_textbuffer(&working, "WATCHDOG | ");
		if (err) { return(err); }
	}

	if (found) {
		for (i=0; i<OH_ENCODE_DELIMITER_LENGTH + 1; i++) {
			working.Data[working.DataLength - i] = 0x00;
		}
		working.DataLength = working.DataLength - (i+1);
	}
				
 	oh_copy_textbuffer(buffer, &working);

	return(SA_OK);
}

/**
 * oh_fprint_rptentry:
 * @stream: File handle.
 * @rptentry: Pointer to SaHpiRptEntryT to be printed.
 * @offsets:  Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiRptEntryT struct to a file.
 * The MACRO oh_print_rptentry(), uses this function to print to STDOUT. 
 *
 * Returns:       
 * SA_OK - normal operation.
 **/
SaErrorT oh_fprint_rptentry(FILE *stream, const SaHpiRptEntryT *rptentry, int offsets)
{
	SaErrorT err = SA_OK;
	oh_big_textbuffer mybuf;
	SaHpiTextBufferT tmpbuffer; 
	char* str = tmpbuffer.Data;

	if (!stream || !rptentry) return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_init_bigtext(&mybuf);
	offsets++;
	
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EntryId: %d\n", rptentry->EntryId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, (offsets));
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ResourceId: %d\n", rptentry->ResourceId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Entity Path: ");
	oh_append_bigtext(&mybuf, str);
        entitypath2string(&rptentry->ResourceEntity, str, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	oh_append_bigtext(&mybuf, str);
	oh_append_bigtext(&mybuf, "\n");

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Capability: ");
	oh_append_bigtext(&mybuf, str);
	oh_decode_capability(rptentry->ResourceCapabilities, &tmpbuffer);
	oh_append_bigtext(&mybuf, str);
	oh_append_bigtext(&mybuf, "\n");	

	oh_append_offset(&mybuf, offsets);
	/* FIXME:: Need a oh_decode_hscapability() that does the pretty OR strings */
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "HotSwap Capability: %d\n",
		 rptentry->HotSwapCapabilities);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ResourceFailed: %s\n",
		 (rptentry->ResourceFailed == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ResourceTag:\n");
	oh_append_bigtext(&mybuf, str);
	oh_build_textbuffer(&mybuf, &(rptentry->ResourceTag), offsets + 1);

	err = oh_fprint_bigtext(stream, &mybuf); 					
	return(err);
}

/**
 * oh_fprint_rdr:
 * @stream: File handle.
 * @thisrdr: Pointer to SaHpiRdrT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiRdrT struct to a file.
 * The MACRO oh_print_rdr(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_rdr(FILE *stream, const SaHpiRdrT *thisrdr, int offsets)
{
	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf, mybuf1;

	if (!stream || !thisrdr) return(SA_ERR_HPI_INVALID_PARAMS);
					
	oh_init_bigtext(&mybuf);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "RecordId: %d\n", thisrdr->RecordId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "RdrType: %s\n", oh_lookup_rdrtype(thisrdr->RdrType));
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Entity Path: ");
	oh_append_bigtext(&mybuf, str);
        entitypath2string(&thisrdr->Entity, str, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	oh_append_bigtext(&mybuf, str);
	oh_append_bigtext(&mybuf, "\n");
	
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IsFru: %s\n",
		 (thisrdr->IsFru == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_init_bigtext(&mybuf1);
	switch(thisrdr->RdrType) 
	{
		case SAHPI_CTRL_RDR:
			err = oh_build_ctrlrec(&mybuf1,
				(const SaHpiCtrlRecT*) &thisrdr->RdrTypeUnion.CtrlRec, offsets);
			break;
		case SAHPI_SENSOR_RDR:
			err = oh_build_sensorrec(&mybuf1,
				(const SaHpiSensorRecT*) &thisrdr->RdrTypeUnion.SensorRec, offsets);
			break;
		case SAHPI_INVENTORY_RDR:
			err = oh_build_invrec(&mybuf1,
				(const SaHpiInventoryRecT*) &thisrdr->RdrTypeUnion.InventoryRec, offsets);
			break;
		case SAHPI_WATCHDOG_RDR:
			err = oh_build_wdogrec(&mybuf1,
				(const SaHpiWatchdogRecT*) &thisrdr->RdrTypeUnion.WatchdogRec, offsets); 
			break;
		case SAHPI_ANNUNCIATOR_RDR:
			err = oh_build_annrec(&mybuf1,
				(const SaHpiAnnunciatorRecT*) &thisrdr->RdrTypeUnion.AnnunciatorRec, offsets);
			break;
		case SAHPI_NO_RECORD:
			oh_append_offset(&mybuf1, offsets);
			oh_append_bigtext(&mybuf1,oh_lookup_rdrtype(thisrdr->RdrType));
			break;
		default:
			oh_append_bigtext(&mybuf1,"Invalid/Unknown RDR Type\n");

	}

	oh_append_bigtext(&mybuf, mybuf1.Data);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IdString:\n");
	oh_append_bigtext(&mybuf, str);
	oh_build_textbuffer(&mybuf, &(thisrdr->IdString), offsets + 1);

	err = oh_fprint_bigtext(stream, &mybuf); 					
	return(err);
}

/**
 * oh_build_ctrlrec:
 * @textbuff: Buffer into which to store flattened ctrl rec structure.
 * @thisrdrunion: Pointer to SaHpiRdrTypeUnionT to be flattened.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Flatten member data contained in SaHpiCtrlRecT struct to a text buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_ctrlrec(oh_big_textbuffer *textbuf, const SaHpiCtrlRecT *ctrlrec, int offsets)
{
	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;
	SaHpiTextBufferT  smallbuf;

	if (!textbuf || !ctrlrec) return(SA_ERR_HPI_INVALID_PARAMS);

	oh_init_bigtext(&mybuf);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Control Number: %d\n",ctrlrec->Num);
	oh_append_bigtext(&mybuf, str);
	
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Control Output Type: %s\n",
		 oh_lookup_ctrloutputtype(ctrlrec->OutputType));
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Control Type: %s\n",
		 oh_lookup_ctrltype(ctrlrec->Type));
	oh_append_bigtext(&mybuf, str);
	offsets++;

	oh_append_offset(&mybuf, offsets);

	switch(ctrlrec->Type) {
	case SAHPI_CTRL_TYPE_DIGITAL:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Digital Default: %s\n",
			 oh_lookup_ctrlstatedigital(ctrlrec->TypeUnion.Digital.Default));
		break;
	case SAHPI_CTRL_TYPE_DISCRETE:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Discrete Default: %d\n",
			 ctrlrec->TypeUnion.Discrete.Default);
		break;
	case SAHPI_CTRL_TYPE_ANALOG:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Analog Min: %d\n",
			 ctrlrec->TypeUnion.Analog.Min);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Analog Max: %d\n",
			 ctrlrec->TypeUnion.Analog.Max);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Analog Default: %d\n",
			 ctrlrec->TypeUnion.Analog.Default);
		break;
	case SAHPI_CTRL_TYPE_STREAM:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Stream Default:\n");
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Repeat: %s\n",
			 &(ctrlrec->TypeUnion.Stream.Default.Repeat));
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "StreamLength: %d\n",
			 ctrlrec->TypeUnion.Stream.Default.StreamLength);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Stream Default: %s\n",
			 ctrlrec->TypeUnion.Stream.Default.Stream);
		break;
	case SAHPI_CTRL_TYPE_TEXT:
		break;
	case SAHPI_CTRL_TYPE_OEM:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem:\n");
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		err = oh_decode_manufacturerid(ctrlrec->TypeUnion.Oem.MId, &smallbuf);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ManufacturerId: %s\n",
			 smallbuf.Data);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ConfigData: %s\n",
			 ctrlrec->TypeUnion.Oem.ConfigData);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 1+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Default:\n");
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 2+offsets);
		err = oh_decode_manufacturerid(ctrlrec->TypeUnion.Oem.Default.MId, &smallbuf);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ManufacturerId: %s\n",
			 smallbuf.Data);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 2+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "BodyLength: %d\n",
			 ctrlrec->TypeUnion.Oem.Default.BodyLength);
		oh_append_bigtext(&mybuf, str);
		oh_append_offset(&mybuf, 2+offsets);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Body: %s\n",
			 ctrlrec->TypeUnion.Oem.Default.Body);
		break;
	default:
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Invalid ControlType Detected\n");
	}

	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DefaultMode:\n");
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, 1+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Mode: %s\n", 
		 oh_lookup_ctrlmode(ctrlrec->DefaultMode.Mode));
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, 1+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ReadOnly: %s\n", 
			(ctrlrec->DefaultMode.ReadOnly == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WriteOnly: %s\n", 
			(ctrlrec->WriteOnly == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem: %d\n",ctrlrec->Oem);
	oh_append_bigtext(&mybuf, str);

	err = oh_copy_bigtext(textbuf, &mybuf);

	return(err);
}

/**
 * oh_build_invrec:
 * @textbuff: Buffer into which to store flattened ctrl rdr structure.
 * @invrec: Pointer to SaHpiInventoryRecT to be flattened.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Flatten member data contained in SaHpiInventoryRecT struct to a text buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_invrec(oh_big_textbuffer *textbuff,const SaHpiInventoryRecT *invrec, int offsets)
{
	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;

	if (!textbuff || !invrec) return(SA_ERR_HPI_INVALID_PARAMS);

	oh_init_bigtext(&mybuf);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "IdrId: %d\n",invrec->IdrId);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Persistent: %s\n",
		 (invrec->Persistent == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem: %d\n",invrec->Oem);
	oh_append_bigtext(&mybuf, str);

	err = oh_copy_bigtext(textbuff, &mybuf);
	return(err);
}

/**
 * oh_build_wdogrec:
 * @textbuff: Buffer into which to store flattened watchdog rec structure.
 * @wdogrec: Pointer to SaHpiWatchdogRecT to be flattened.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Flatten member data contained in SaHpiWatchdogRecT struct to a text buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_wdogrec(oh_big_textbuffer *textbuff,const SaHpiWatchdogRecT *wdogrec, int offsets)
{

	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;

	if (!textbuff || !wdogrec) return(SA_ERR_HPI_INVALID_PARAMS);

	oh_init_bigtext(&mybuf);
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogNum: %d\n",wdogrec->WatchdogNum);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem: %d\n",wdogrec->Oem);
	oh_append_bigtext(&mybuf, str);

	err = oh_copy_bigtext(textbuff, &mybuf);
	return(err);
}

/**
 * oh_build_annrec:
 * @textbuff: Buffer into which to store flattened structure.
 * @annrec: Pointer to SaHpiAnnunciatorRecT to be flattened.
 * @offsets:  Number of offsets to start printing structure.
 * 
 * Flatten member data contained in SaHpiAnnunciatorRecT struct to a text buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_annrec(oh_big_textbuffer *textbuff,const SaHpiAnnunciatorRecT *annrec, int offsets)
{
	SaErrorT err;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	oh_big_textbuffer mybuf;

	if (!textbuff || !annrec) return(SA_ERR_HPI_INVALID_PARAMS);

	oh_init_bigtext(&mybuf);
	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "AnnunciatorNum: %d\n", annrec->AnnunciatorNum);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "AnnunciatorType: %s\n",
		 oh_lookup_annunciatortype(annrec->AnnunciatorType));
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ModeReadOnly: %s\n",
		 (annrec->ModeReadOnly == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "MaxCondition: %d\n", annrec->MaxConditions);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem: %d\n", annrec->Oem);
	oh_append_bigtext(&mybuf, str);

	err = oh_copy_bigtext(textbuff, &mybuf);
	return(err);
}

/**
 * oh_fprint_eventloginfo:
 * @stream: File handle.
 * @thiselinfo: Pointer to SaHpiEventLogInfoT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiEventLogInfoT struct to a file.
 * The MACRO oh_print_evenloginfo(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_eventloginfo(FILE *stream, const SaHpiEventLogInfoT *thiselinfo, int offsets)
{	
	int err;
	oh_big_textbuffer mybuf;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiTextBufferT  minibuf;

	if (!stream || !thiselinfo) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&mybuf);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Entries: %d\n", thiselinfo->Entries);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Size: %d\n", thiselinfo->Size);
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "UserEventMaxSize: %d\n", thiselinfo->UserEventMaxSize);
	oh_append_bigtext(&mybuf, str);
	
	oh_append_offset(&mybuf, offsets);
	err = oh_decode_time(thiselinfo->UpdateTimestamp, &minibuf);
	if (err)
		memcpy(minibuf.Data, "SAHPI_TIME_UNSPECIFIED", 
					sizeof("SAHPI_TIME_UNSPECIFIED"));
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "UpdateTimestamp: %s\n", minibuf.Data);
	oh_append_bigtext(&mybuf, str);	
	
	oh_append_offset(&mybuf, offsets);
	err = oh_decode_time(thiselinfo->CurrentTime, &minibuf);
	if (err)
		memcpy(minibuf.Data, "SAHPI_TIME_UNSPECIFIED", 
					sizeof("SAHPI_TIME_UNSPECIFIED"));
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "CurrentTime: %s\n", minibuf.Data);
	oh_append_bigtext(&mybuf, str);	

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Enabled: %s\n",
		 (thiselinfo->Enabled == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OverflowFlag: %s\n",
		 (thiselinfo->OverflowFlag == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OverflowResetable: %s\n",
		 (thiselinfo->OverflowResetable == SAHPI_TRUE) ? "TRUE" : "FALSE" );
	oh_append_bigtext(&mybuf, str);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OverflowAction: %s\n",
		 oh_lookup_eventlogoverflowaction(thiselinfo->OverflowAction));
	oh_append_bigtext(&mybuf, str);
	
	err = oh_fprint_bigtext(stream, &mybuf);
	return(err);
}

/**
 * oh_fprint_eventlogentry:
 * @stream: File handle.
 * @thiseventlog: Pointer to SaHpiEventLogEntryT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiEventLogEntryT struct to a file.
 * The MACRO oh_print_evenlogentry(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_eventlogentry(FILE *stream, const SaHpiEventLogEntryT *thiseventlog, int offsets)
{	
	int err;
	oh_big_textbuffer mybuf, mybufX;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiTextBufferT  minibuf;

	if (!stream || !thiseventlog) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&mybuf);

	oh_append_offset(&mybuf, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EntryId: %d\n", thiseventlog->EntryId);
	oh_append_bigtext(&mybuf, str);
	oh_append_offset(&mybuf, offsets);
	err = oh_decode_time(thiseventlog->Timestamp, &minibuf);
	if (err)
		memcpy(minibuf.Data, "SAHPI_TIME_UNSPECIFIED", 
					sizeof("SAHPI_TIME_UNSPECIFIED"));
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Timestamp: %s\n", minibuf.Data);
	oh_append_bigtext(&mybuf, str);	
	
	oh_init_bigtext(&mybufX);
	err = oh_build_event(&mybufX, &thiseventlog->Event, offsets);
	oh_append_bigtext(&mybuf, mybufX.Data);	
	
	err = oh_fprint_bigtext(stream, &mybuf);
	return(err);
}

/**
 * oh_fprint_event:
 * @stream: File handle.
 * @event: Pointer to SaHpiEventT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiEventT struct to a file.
 * The MACRO oh_print_event(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_fprint_event(FILE *stream, const SaHpiEventT *event, int offsets) 
{
	int err;
	oh_big_textbuffer buffer;
	
	if (!stream || !event) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);

	err = oh_build_event(&buffer, event, offsets);
	if (err) { return(err); }

	err = oh_fprint_bigtext(stream, &buffer);
	if (err) { return(err); }

	return(SA_OK);
}

/**
 * oh_build_event:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 * 
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;
	SaHpiTextBufferT tmpbuffer;

	memset( buffer->Data, 0, OH_MAX_TEXT_BUFFER_LENGTH );
	memset( tmpbuffer.Data, 0, sizeof( tmpbuffer.Data ) );
	
	/* Event Type */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Type: %s\n", 
		 oh_lookup_eventtype(event->EventType));
	oh_append_bigtext(buffer, str);
	offsets++;
	
	/* Event Source */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Resource: %d\n", event->Source);
	oh_append_bigtext(buffer, str);

	/* Event Time */
	oh_append_offset(buffer, offsets);
	oh_decode_time(event->Timestamp, &tmpbuffer);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Timestamp: %s\n", tmpbuffer.Data);
	oh_append_bigtext(buffer, str);

	/* Event Severity */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Severity: %s\n",
		 oh_lookup_severity(event->Severity));
	oh_append_bigtext(buffer, str);
	
	switch (event->EventType) {
	case SAHPI_ET_RESOURCE:
		err = oh_build_event_resource(buffer, event, offsets);
		break;
	case SAHPI_ET_DOMAIN:
		err = oh_build_event_domain(buffer, event, offsets);
		break;
	case SAHPI_ET_SENSOR:
		err = oh_build_event_sensor(buffer, event, offsets);
		break;
	case SAHPI_ET_SENSOR_ENABLE_CHANGE:
		err = oh_build_event_sensor_enable_change(buffer, event, offsets);
		break;
	case SAHPI_ET_HOTSWAP:
		err = oh_build_event_hotswap(buffer, event, offsets);
		break;
	case SAHPI_ET_WATCHDOG:
		err = oh_build_event_watchdog(buffer, event, offsets);
		break;
	case SAHPI_ET_HPI_SW:
		err = oh_build_event_hpi_sw(buffer, event, offsets);
		break;
	case SAHPI_ET_OEM:
		err = oh_build_event_oem(buffer, event, offsets);
		break;
	case SAHPI_ET_USER:
		err = oh_build_event_user(buffer, event, offsets);
		break;
	default:
		dbg("Unrecognized Event Type=%d.", event->EventType);
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
			
	if (err) { return(err); }
	return(SA_OK);
}

/**
 * oh_build_event_resource:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 * Build ResourceEventTypeT value into string buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_resource(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{

	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	if (!buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);

	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ResourceEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 1+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ResourceEventType: %s\n", 
		 	oh_lookup_resourceeventtype(event->EventDataUnion.ResourceEvent.ResourceEventType));
	oh_append_bigtext(buffer, str);

	return(SA_OK);
}

/**
 * oh_build_event_domain:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 * Build event domain value structure values into string buffer.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_domain(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	if (!buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);

	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DomainEvent:\n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, offsets++);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s\n", 
		 oh_lookup_domaineventtype(event->EventDataUnion.DomainEvent.Type));
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, offsets++);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DomainId: %d\n", 
		 event->EventDataUnion.DomainEvent.DomainId);
	oh_append_bigtext(buffer, str);

	return(SA_OK);
}

#define OH_MAX_SENSOROPTIONALDATA 6

typedef struct {
    SaHpiSensorOptionalDataT datadef;
    unsigned char *str;
} oh_sensor_opt_data_map;

oh_sensor_opt_data_map sensor_event_optdata_strings[] = {
{SAHPI_SOD_TRIGGER_READING, "TRIGGER_READING"},
{SAHPI_SOD_TRIGGER_THRESHOLD, "TRIGGER_THRESHOLD"},
{SAHPI_SOD_OEM, "OEM"},
{SAHPI_SOD_PREVIOUS_STATE, "PREVIOUS_STATE"},
{SAHPI_SOD_CURRENT_STATE, "CURRENT_STATE"},
{SAHPI_SOD_SENSOR_SPECIFIC, "SENSOR_SPECIFIC"},
};

/**
 * oh_encode_sensoroptionaldata:
 * @buffer: Pointer to space to decipher SaHpiSensorOptionalDataT struct
 * @sensor_opt_data: Sensor's optional data bit mask.
 * 
 * Converts a sensor's optional data bit mask field into a string. For example,
 * if the optional data bit mask is SAHPI_SOD_TRIGGER_READING | SAHPI_SOD_CURRENT_STATE,
 * this routine returns the string "TRIGGER_READING | CURRENT_STATE" in a
 * SaHpiTextBufferT structure.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_decode_sensoroptionaldata(SaHpiSensorOptionalDataT sensor_opt_data,
				      SaHpiTextBufferT *buffer)
{
	int i, found;
	SaErrorT err;
	SaHpiTextBufferT working;

	if (!buffer) return(SA_ERR_HPI_INVALID_PARAMS);

	err = oh_init_textbuffer(&working);
	if (err) { return(err); }

	found = 0;
	/* Look for sensor optional data definitions */
	for (i=0; i<OH_MAX_SENSOROPTIONALDATA; i++) {
		if (sensor_event_optdata_strings[i].datadef & sensor_opt_data) {
			found++;
			err = oh_append_textbuffer(&working, sensor_event_optdata_strings[i].str);
			if (err) { return(err); }
			err = oh_append_textbuffer(&working, OH_ENCODE_DELIMITER);
			if (err) { return(err); }
		}
	}

	/* Remove last delimiter */
	if (found) {
		for (i=0; i<OH_ENCODE_DELIMITER_LENGTH + 1; i++) {
			working.Data[working.DataLength - i] = 0x00;
		}
		working.DataLength = working.DataLength - (i+1);
	}
	else {
		err = oh_append_textbuffer(&working, "None");
		if (err) { return(err); }
	}

	err = oh_copy_textbuffer(buffer, &working);

	return(SA_OK);
}

/**
 * oh_build_event_resource:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 * Converts a sensor event structure into a string.
 * String is stored in an oh_big_textbuffer data structure.
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_sensor(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;
	SaHpiTextBufferT tmpbuffer;

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);

	/* Event Sensor Num */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Sensor Num: %d\n", 
		 event->EventDataUnion.SensorEvent.SensorNum);
	oh_append_bigtext(buffer, str);

	/* Event Sensor Type */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Sensor Type: %s\n",
		 oh_lookup_sensortype(event->EventDataUnion.SensorEvent.SensorType));
	oh_append_bigtext(buffer, str);

	/* Event Sensor Category */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Sensor Category: %s\n", 
		 oh_lookup_eventcategory(event->EventDataUnion.SensorEvent.EventCategory));
	oh_append_bigtext(buffer, str);

	/* Event Sensor Assertion */
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Event Sensor Assertion: %s\n",
		(event->EventDataUnion.SensorEvent.Assertion == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);

	/* Event Sensor State */
	oh_append_offset(buffer, offsets);
	oh_append_bigtext(buffer, "Event Sensor State: ");
	err = oh_decode_eventstate(event->EventDataUnion.SensorEvent.EventState,
				   event->EventDataUnion.SensorEvent.EventCategory,
				   &tmpbuffer);
	if (err) { return(err); }
	oh_append_bigtext(buffer, tmpbuffer.Data);
	oh_append_bigtext(buffer, "\n");

	/* Sensor Event - Sensor Optional Data */
	oh_append_offset(buffer, offsets);
	oh_append_bigtext(buffer, "Optional Data: ");

	err = oh_decode_sensoroptionaldata(event->EventDataUnion.SensorEvent.OptionalDataPresent,
					   &tmpbuffer);
	if (err) { return(err); }
	oh_append_bigtext(buffer, tmpbuffer.Data);
	oh_append_bigtext(buffer, "\n");

        /* Sensor Event - Sensor Optional Trigger Reading Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_TRIGGER_READING) {
		SaHpiSensorDataFormatT format;
		
		memset(&format, 0, sizeof(SaHpiSensorDataFormatT));
		oh_append_offset(buffer, offsets + 1);
		oh_append_bigtext(buffer, "Trigger Reading: ");
		
		format.IsSupported = SAHPI_TRUE;
		format.ReadingType = event->EventDataUnion.SensorEvent.TriggerReading.Type;
		format.BaseUnits = SAHPI_SU_UNSPECIFIED;

		err = oh_decode_sensorreading(event->EventDataUnion.SensorEvent.TriggerReading,
					      format, &tmpbuffer);
		if (err) { return(err); }

		oh_append_bigtext(buffer, tmpbuffer.Data);
		oh_append_bigtext(buffer, "\n");
	}

        /* Sensor Event - Sensor Optional Trigger Threshold Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_TRIGGER_THRESHOLD) {
		SaHpiSensorDataFormatT format;
		
		memset(&format, 0, sizeof(SaHpiSensorDataFormatT));
		oh_append_offset(buffer, offsets + 1);
		oh_append_bigtext(buffer, "Trigger Threshold: ");
		
		format.IsSupported = SAHPI_TRUE;
		format.ReadingType = event->EventDataUnion.SensorEvent.TriggerThreshold.Type;
		format.BaseUnits = SAHPI_SU_UNSPECIFIED;

		err = oh_decode_sensorreading(event->EventDataUnion.SensorEvent.TriggerThreshold,
					      format, &tmpbuffer);
		if (err) { return(err); }

		oh_append_bigtext(buffer, tmpbuffer.Data);
		oh_append_bigtext(buffer, "\n");
	}

        /* Sensor Event - Sensor Optional Previous State Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_PREVIOUS_STATE) {
		oh_append_offset(buffer, offsets + 1);
		oh_append_bigtext(buffer, "Previous Sensor State: ");
		err = oh_decode_eventstate(event->EventDataUnion.SensorEvent.PreviousState,
					   event->EventDataUnion.SensorEvent.EventCategory,
					   &tmpbuffer);
		if (err) { return(err); }
		oh_append_bigtext(buffer, tmpbuffer.Data);
		oh_append_bigtext(buffer, "\n");
	}

        /* Sensor Event - Sensor Optional Current State Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_CURRENT_STATE) {
		oh_append_offset(buffer, offsets + 1);
		oh_append_bigtext(buffer, "Current Sensor State: ");
		err = oh_decode_eventstate(event->EventDataUnion.SensorEvent.CurrentState,
					   event->EventDataUnion.SensorEvent.EventCategory,
					   &tmpbuffer);
		if (err) { return(err); }
		oh_append_bigtext(buffer, tmpbuffer.Data);
		oh_append_bigtext(buffer, "\n");
	}
        /* Sensor Event - Sensor Optional OEM Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_OEM) {
		oh_append_offset(buffer, offsets + 1);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OEM: %x\n", 
			 event->EventDataUnion.SensorEvent.Oem);
		oh_append_bigtext(buffer, str);
	}

        /* Sensor Event - Sensor Optional Sensor Specific Data */
	if (event->EventDataUnion.SensorEvent.OptionalDataPresent & SAHPI_SOD_SENSOR_SPECIFIC) {
		oh_append_offset(buffer, offsets + 1);
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Sensor Specific: %x\n",
			 event->EventDataUnion.SensorEvent.SensorSpecific );
		oh_append_bigtext(buffer, str);
	}

	return(SA_OK);
}

/**
 * oh_build_event_sensor_enable_change:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_sensor_enable_change(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);

	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "SensorEnableChangeEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "SensorNum: %d\n", 
		 	event->EventDataUnion.SensorEnableChangeEvent.SensorNum);
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "SensorType: %s\n", 
		 	oh_lookup_sensortype(event->EventDataUnion.SensorEnableChangeEvent.SensorType));
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EventCategory: %d\n", 
		 	event->EventDataUnion.SensorEnableChangeEvent.EventCategory);
	oh_append_bigtext(buffer, str);
	
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "SensorEnable: %s\n",
		(event->EventDataUnion.SensorEnableChangeEvent.SensorEnable == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);
	
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "SensorEventEnable: %s\n",
		(event->EventDataUnion.SensorEnableChangeEvent.SensorEventEnable == SAHPI_TRUE) ? "TRUE" : "FALSE");
	oh_append_bigtext(buffer, str);
	
	/* FIXME: */
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "AssertEventMask: %x. Mapping is TBD\n",
				event->EventDataUnion.SensorEnableChangeEvent.AssertEventMask);
	oh_append_bigtext(buffer, str);
	
	/* FIXME: */
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "DeassertEventMask: %x. Mapping is TBD\n",
				event->EventDataUnion.SensorEnableChangeEvent.DeassertEventMask);
	oh_append_bigtext(buffer, str);
	
	/* FIXME: */
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OptionalDataPresent: %x.\n",
				event->EventDataUnion.SensorEnableChangeEvent.OptionalDataPresent);
	oh_append_bigtext(buffer, str);
	
	/* FIXME: */
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "CurrentState: %x. Mapping is TBD\n",
				event->EventDataUnion.SensorEnableChangeEvent.CurrentState);
	oh_append_bigtext(buffer, str);

	return(SA_OK);
}


/**
 * oh_build_event_hotswap:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_hotswap(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);

	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "HotswapEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "HotSwapState: %s\n", 
		 	oh_lookup_hsstate(event->EventDataUnion.HotSwapEvent.HotSwapState));
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "PreviousHotSwapState: %s\n", 
		 	oh_lookup_hsstate(event->EventDataUnion.HotSwapEvent.PreviousHotSwapState));
	oh_append_bigtext(buffer, str);

	return(SA_OK);
}


/**
 * oh_build_event_watchdog:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_watchdog(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{

	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	int i;
	SaHpiBoolT matchFound = SAHPI_FALSE;
	

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogNum: %d\n",
					event->EventDataUnion.WatchdogEvent.WatchdogNum); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogNum: %s\n",
			oh_lookup_watchdogactionevent(event->EventDataUnion.WatchdogEvent.WatchdogAction)); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogPreTimerAction: %s\n",
			oh_lookup_watchdogpretimerinterrupt(event->EventDataUnion.WatchdogEvent.WatchdogPreTimerAction)); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	for(i = 0; i < OH_MAX_WATCHDOGTIMERUSE; i++) {
		if (event->EventDataUnion.WatchdogEvent.WatchdogUse == 
					watchdogtimeruse_strings[i].entity_type) {
			matchFound = SAHPI_TRUE;
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogUse: %s\n",
								watchdogtimeruse_strings[i].str);
			break;
		}
	}
	
	 if (!matchFound)  
		snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "WatchdogUse: %s\n",
									"Invalid data");
	oh_append_bigtext(buffer, str);
			
	return(SA_OK);
}


/**
 * oh_build_event_hpi_sw:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_hpi_sw(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaHpiTextBufferT tmpbuffer;
	SaErrorT err;

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "HpiSwEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	err = oh_decode_manufacturerid(event->EventDataUnion.HpiSwEvent.MId, &tmpbuffer);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ManufacturerId: %s\n", 
		 							tmpbuffer.Data);
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "EventData: \n"); 
	oh_append_bigtext(buffer, str);
	
	oh_build_textbuffer(buffer, &event->EventDataUnion.HpiSwEvent.EventData, 8+offsets);
	return(SA_OK);
}


/**
 * oh_build_event_oem:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_oem(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	SaErrorT err;
	SaHpiTextBufferT tmpbuffer;

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OemEvent: \n"); 
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	err = oh_decode_manufacturerid(event->EventDataUnion.OemEvent.MId, &tmpbuffer);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ManufacturerId: %s\n", 
		 							tmpbuffer.Data);
	oh_append_bigtext(buffer, str);

	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "OemEventData: \n"); 
	oh_append_bigtext(buffer, str);

	oh_build_textbuffer(buffer, &event->EventDataUnion.OemEvent.OemEventData, 8+offsets);

	return(SA_OK);
}


/**
 * oh_build_event_user:
 * @buffer: Pointer to space to decipher SaHpiEventT struct
 * @event: Pointer to the event to be deciphered
 * @offset: Number of offsets to start printing structure.
 * 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
static SaErrorT oh_build_event_user(oh_big_textbuffer *buffer, const SaHpiEventT *event, int offsets)
{
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];

	if ( !buffer || !event)
		return(SA_ERR_HPI_INVALID_PARAMS);
	
	oh_append_offset(buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "UserEvent: \n"); 
	oh_append_bigtext(buffer, str);
	
	oh_append_offset(buffer, 4+offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "UserEventData: \n"); 
	oh_append_bigtext(buffer, str);

	oh_build_textbuffer(buffer, &event->EventDataUnion.UserEvent.UserEventData, 8+offsets);
	return(SA_OK);
}

/**
 * oh_fprint_ctrlstate:
 * @stream: File handle.
 * @event: Pointer to SaHpitrlStateT to be printed.
 * @offsets: Number of offsets to start printing structure.
 * 
 * Prints the member data contained in SaHpiCtrlStateT struct to a file.
 * The MACRO oh_print_ctrlstate(), uses this function to print to STDOUT. 
 *
 * Returns:
 * SA_OK - normal operation.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/

SaErrorT oh_fprint_ctrlstate(FILE *stream, const SaHpiCtrlStateT *thisctrlstate, int offsets)
{

	SaErrorT rv = SA_OK;
	oh_big_textbuffer buffer;
	SaHpiTextBufferT smallbuf;
	char str[SAHPI_MAX_TEXT_BUFFER_LENGTH];
	
	if (!stream || !thisctrlstate) {
		return(SA_ERR_HPI_INVALID_PARAMS);
	}
	
	oh_init_bigtext(&buffer);
	oh_append_offset(&buffer, offsets);
	snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Type: %s\n",
				oh_lookup_ctrltype(thisctrlstate->Type));
	oh_append_bigtext(&buffer, str);
	oh_append_offset(&buffer, offsets);
	
	switch(thisctrlstate->Type) {
		case SAHPI_CTRL_TYPE_DIGITAL:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "State: %s\n",
				oh_lookup_ctrlstatedigital(thisctrlstate->StateUnion.Digital));
			break;
		case SAHPI_CTRL_TYPE_DISCRETE:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "State: %d\n",
							thisctrlstate->StateUnion.Discrete);
			break;
		case SAHPI_CTRL_TYPE_ANALOG:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Analog: %d\n",
							thisctrlstate->StateUnion.Analog);
			break;
		case SAHPI_CTRL_TYPE_STREAM:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Stream:\n");
			oh_append_bigtext(&buffer, str);
			oh_append_offset(&buffer, 4+offsets);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Repeat: %s\n",
				(thisctrlstate->StateUnion.Stream.Repeat == SAHPI_TRUE) ? "TRUE" : "FALSE");
			oh_append_bigtext(&buffer, str);
			oh_append_offset(&buffer, 4+offsets);
			snprintf(str, thisctrlstate->StateUnion.Stream.StreamLength, "%s\n",
				 			thisctrlstate->StateUnion.Stream.Stream);
					
			break;
		case SAHPI_CTRL_TYPE_TEXT:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Line: %d\n", 
						thisctrlstate->StateUnion.Text.Line);
			oh_append_bigtext(&buffer, str);
			oh_append_offset(&buffer, offsets);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "%s\n", 
						thisctrlstate->StateUnion.Text.Text.Data);		
			break;
		case SAHPI_CTRL_TYPE_OEM:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Oem:\n");
			oh_append_bigtext(&buffer, str);
			oh_append_offset(&buffer, 4+offsets);
			rv = oh_decode_manufacturerid(thisctrlstate->StateUnion.Oem.MId, &smallbuf);
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "ManufacturerId: %s\n",
										 smallbuf.Data);
			oh_append_bigtext(&buffer, str);
			oh_append_offset(&buffer, 4+offsets);
			snprintf(str, thisctrlstate->StateUnion.Oem.BodyLength, "%s\n",
							thisctrlstate->StateUnion.Oem.Body);
			
			break;
		default:
			snprintf(str, SAHPI_MAX_TEXT_BUFFER_LENGTH, "Invalid Control Type\n");
			rv = SA_ERR_HPI_INVALID_DATA;
			break;
	} 
	oh_append_bigtext(&buffer, str);
	rv = oh_fprint_bigtext(stream, &buffer);
	return(rv);
}

/**
 * oh_valid_textbuffer:
 * @buffer: Pointer to a SaHpiTextBufferT structure.
 * 
 * Verifies @buffer is a valid SaHpiTextBufferT structure.
 * Used in routines where the user can set SaHpiTextBufferT values.
 *
 * Returns:
 * SAHPI_TRUE - if @buffer is valid.
 * SAHPI_FALSE - if @buffer not valid.
 **/
SaHpiBoolT oh_valid_textbuffer(SaHpiTextBufferT *buffer)
{
	int i;
        if (!buffer) return SAHPI_FALSE;
	if (oh_lookup_texttype(buffer->DataType) == NULL) return SAHPI_FALSE;
	/* Compiler checks DataLength <= SAHPI_MAX_TEXT_BUFFER_LENGTH */

	switch (buffer->DataType) {
	case SAHPI_TL_TYPE_UNICODE:                
		if (oh_lookup_language(buffer->Language) == NULL) return SAHPI_FALSE;
		if (buffer->DataLength % 2 != 0) return SAHPI_FALSE;
                /* FIXME: Need to find a way to validate UTF-16 strings.
                 * Using http://www.campusprogram.com/reference/en/wikipedia/u/ut/utf_16.html
                 * as reference. -- RM 09/29/04
                 */                
		break;
	case SAHPI_TL_TYPE_BCDPLUS:                
                /* 8-bit ASCII, '0'-'9', space, dash, period, colon, comma, or
		   underscore. Always encoded in HPI as 8-bit values */
                for (i = 0;
                     i < SAHPI_MAX_TEXT_BUFFER_LENGTH || buffer->Data[i] != '\0';
                     i++)
                {
                        if ((buffer->Data[i] < 0x30 || buffer->Data[i] > 0x39) &&
                             buffer->Data[i] != 0x20 && buffer->Data[i] != 0x2d &&
                             buffer->Data[i] != 0x2e && buffer->Data[i] != 0x3a &&
                             buffer->Data[i] != 0x2c && buffer->Data[i] != 0x5f)
                                return SAHPI_FALSE;
                }
		break;
	case SAHPI_TL_TYPE_ASCII6:		                
                /* reduced set, 0x20-0x5f only. Always encoded in HPI as 8-bit values */
                for (i = 0;
                     i < SAHPI_MAX_TEXT_BUFFER_LENGTH || buffer->Data[i] != '\0';
                     i++)
                {
                        if (buffer->Data[i] < 0x20 || buffer->Data[i] > 0x5f)
                                return SAHPI_FALSE;
                }
		break;
	case SAHPI_TL_TYPE_TEXT:
		if (oh_lookup_language(buffer->Language) == NULL) { return(SAHPI_FALSE); }
                /* all character values are good 0x00 - 0xff */		
		break;
	case SAHPI_TL_TYPE_BINARY: /* No check possible */
		break;
	default:
		dbg("Invalid data type");
		return(SAHPI_FALSE);
	}

	return(SAHPI_TRUE);
}

#define validate_threshold(thdname, thdmask) \
do { \
if (thds->thdname.IsSupported) { \
	if (!(writable_thds & thdmask)) return(SA_ERR_HPI_INVALID_CMD); \
	if (format->ReadingType != thds->thdname.Type) return(SA_ERR_HPI_INVALID_CMD); \
	switch (format->ReadingType) { \
	case SAHPI_SENSOR_READING_TYPE_INT64: \
		if (format->Range.Flags & SAHPI_SRF_MAX) { \
			if (thds->thdname.Value.SensorInt64 > format->Range.Max.Value.SensorInt64) \
                                return(SA_ERR_HPI_INVALID_CMD); \
		} \
		if (format->Range.Flags & SAHPI_SRF_MIN) { \
			if (thds->thdname.Value.SensorInt64 < format->Range.Min.Value.SensorInt64) \
				return(SA_ERR_HPI_INVALID_CMD); \
		} \
		if (thds->PosThdHysteresis.IsSupported) { \
			if (thds->PosThdHysteresis.Value.SensorInt64 < 0) return(SA_ERR_HPI_INVALID_DATA); \
		} \
		if (thds->NegThdHysteresis.IsSupported) { \
			if (thds->NegThdHysteresis.Value.SensorInt64 < 0) return(SA_ERR_HPI_INVALID_DATA); \
		} \
		break; \
	case SAHPI_SENSOR_READING_TYPE_FLOAT64: \
		if (format->Range.Flags & SAHPI_SRF_MAX) { \
			if (thds->thdname.Value.SensorFloat64 > format->Range.Max.Value.SensorFloat64) \
				return(SA_ERR_HPI_INVALID_CMD); \
		} \
		if (format->Range.Flags & SAHPI_SRF_MIN) { \
			if (thds->thdname.Value.SensorFloat64 < format->Range.Min.Value.SensorFloat64) \
				return(SA_ERR_HPI_INVALID_CMD); \
		} \
		if (thds->PosThdHysteresis.IsSupported) { \
			if (thds->PosThdHysteresis.Value.SensorFloat64 < 0) return(SA_ERR_HPI_INVALID_DATA); \
		} \
		if (thds->NegThdHysteresis.IsSupported) { \
			if (thds->NegThdHysteresis.Value.SensorFloat64 < 0) return(SA_ERR_HPI_INVALID_DATA); \
		} \
		break; \
	case SAHPI_SENSOR_READING_TYPE_UINT64: \
		if (format->Range.Flags & SAHPI_SRF_MAX) { \
			if (thds->thdname.Value.SensorUint64 > format->Range.Max.Value.SensorUint64) \
				return(SA_ERR_HPI_INVALID_CMD); \
		} \
		if (format->Range.Flags & SAHPI_SRF_MIN) { \
			if (thds->thdname.Value.SensorUint64 < format->Range.Min.Value.SensorUint64) \
				return(SA_ERR_HPI_INVALID_CMD); \
		} \
		break; \
	case SAHPI_SENSOR_READING_TYPE_BUFFER: \
	default: \
		dbg("Invalid threshold reading type."); \
		return(SA_ERR_HPI_INVALID_CMD); \
	} \
} \
} while(0)

#define validate_threshold_order(thdtype) \
do { \
if (thds->UpCritical.IsSupported == SAHPI_TRUE) { \
	if (thds->UpMajor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpCritical.Value.thdtype < thds->UpMajor.Value.thdtype) \
                        return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->UpMinor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpCritical.Value.thdtype < thds->UpMinor.Value.thdtype) \
                        return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowMinor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpCritical.Value.thdtype < thds->LowMinor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowMajor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpCritical.Value.thdtype < thds->LowMajor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowCritical.IsSupported == SAHPI_TRUE) { \
		if (thds->UpCritical.Value.thdtype < thds->LowCritical.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
} \
if (thds->UpMajor.IsSupported == SAHPI_TRUE) { \
	if (thds->UpMinor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMajor.Value.thdtype < thds->UpMinor.Value.thdtype) \
                               return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowMinor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMajor.Value.thdtype < thds->LowMinor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowMajor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMajor.Value.thdtype < thds->LowMajor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowCritical.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMajor.Value.thdtype < thds->LowCritical.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
} \
if (thds->UpMinor.IsSupported == SAHPI_TRUE) { \
	if (thds->LowMinor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMinor.Value.thdtype < thds->LowMinor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowMajor.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMinor.Value.thdtype < thds->LowMajor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowCritical.IsSupported == SAHPI_TRUE) { \
		if (thds->UpMinor.Value.thdtype < thds->LowCritical.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
} \
if (thds->LowMinor.IsSupported == SAHPI_TRUE) { \
	if (thds->LowMajor.IsSupported == SAHPI_TRUE) { \
		if (thds->LowMinor.Value.thdtype < thds->LowMajor.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
	if (thds->LowCritical.IsSupported == SAHPI_TRUE) { \
		if (thds->LowMinor.Value.thdtype < thds->LowCritical.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
} \
if (thds->LowMajor.IsSupported == SAHPI_TRUE) { \
	if (thds->LowCritical.IsSupported == SAHPI_TRUE) { \
		if (thds->LowMajor.Value.thdtype < thds->LowCritical.Value.thdtype) \
			return(SA_ERR_HPI_INVALID_DATA); \
	} \
} \
} while(0)
 
/**
 * oh_valid_thresholds:
 * @thds: Location of threshold definitions to verify.
 * @format: Location of sensor's data format structure.
 * @writable_thds: List of sensor's writeable thresholds.
 *
 * Validates that the threshold values defined in @thds are valid for a sensor with
 * a data format described in @format. Callers to this routine still need to
 * verify that the sensor has a category of SAHPI_EC_THRESHOLD and has writable thresholds
 * (ThresholdDefn.IsAccessible and ThresholdDefn.WriteThold non-zero). 
 *
 * In addition, the caller may need to read the sensor's existing sensors first, since
 * @thds may only contain a subset of the possible writable thresholds.
 *
 * Return values:
 * SA_OK - normal case.
 * SA_ERR_HPI_INVALID_CMD - Non-writable thresholds, invalid thresholds values, or invalid data type.
 * SA_ERR_HPI_INVALID_DATA - Threshold values out of order; negative hysteresis.
 * SA_ERR_HPI_INVALID_PARAMS - Pointer parameter(s) are NULL.
 **/
SaErrorT oh_valid_thresholds(SaHpiSensorThresholdsT *thds,
			     SaHpiSensorDataFormatT *format,
			     SaHpiSensorThdMaskT writable_thds)
{
	if (!thds || !format) {
		dbg("Invalid parameter.");
		return(SA_ERR_HPI_INVALID_PARAMS);
	}

	validate_threshold(LowCritical, SAHPI_STM_LOW_CRIT);
	validate_threshold(LowMajor, SAHPI_STM_LOW_MAJOR);
	validate_threshold(LowMinor, SAHPI_STM_LOW_MINOR);
	validate_threshold(UpCritical, SAHPI_STM_UP_CRIT);
	validate_threshold(UpMajor, SAHPI_STM_UP_MAJOR);
	validate_threshold(UpMinor, SAHPI_STM_UP_MINOR);
	validate_threshold(PosThdHysteresis, SAHPI_STM_UP_HYSTERESIS);
	validate_threshold(NegThdHysteresis, SAHPI_STM_LOW_HYSTERESIS);

        /* Validate defined thresholds are in order:
	 * upper critical >= upper major >= upper minor >= 
	 * lower minor >= lower major >= lower critical
	 */
	switch (format->ReadingType) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
		validate_threshold_order(SensorInt64);
		break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
		validate_threshold_order(SensorFloat64);
		break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
		validate_threshold_order(SensorUint64);
		break;
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
	default:
		dbg("Invalid threshold reading type.");
		return(SA_ERR_HPI_INVALID_CMD);
	}

	return(SA_OK);
}
