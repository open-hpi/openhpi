/*      -*- linux-c -*-
 *
 * $Id$
 *
 * Copyright (c) IBM Corp 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Sean Dague <sdague@users.sf.net>
 *
 */

#include <stdlib.h>
#include <string.h>
#include <SaHpi.h>
#include <oh_error.h>
#include <oh_utils.h>
#include <SaHpi_enum_utils.h>
#include <print_structs.h>

/*
 *  setup_text_buffer - internal function to clear out a 
 *  text buffer and make sure it is really clean.  It assumes
 *  an english language set.
 */

static inline SaErrorT setup_text_buffer(SaHpiTextBufferT *buff) 
{
        memset(buff, 0, sizeof(*buff));            
        buff->DataType = SAHPI_TL_TYPE_TEXT;
        buff->Language = SAHPI_LANG_ENGLISH;
        buff->DataLength = 0;
        return SA_OK;
}

/*
 *  oh_text_buffer_append - internal function to added
 *       char * data to a text buffer.
 *
 *  returns:
 *       SA_OK - success
 *       SA_ERR_HPI_OUT_OF_SPACE - operation would overun buffer
 */

static SaErrorT oh_text_buffer_append(SaHpiTextBufferT *text, char * from, size_t size)
{
        SaHpiUint8T *p;
        if((size + text->DataLength) >= SAHPI_MAX_TEXT_BUFFER_LENGTH) {
                dbg("Can't add %d to %d size text! We would overrun the buffer!", size, text->DataLength);
                return SA_ERR_HPI_OUT_OF_SPACE;
        }
        
        
        /* we can't trust NULLs to be right, so use a targetted strncpy instead */
        p = text->Data;
        p += text->DataLength;
        strncpy(p, from, size);
        text->DataLength += size;
        
        // fprint_text_buffer(stderr, text);
        return SA_OK;
}

/**
 * fprint_text_buffer:
 * @stream: 
 * @text: 
 * 
 * 
 **/
void fprint_text_buffer(FILE *stream, const SaHpiTextBufferT *text)
{
        if(text->DataType == SAHPI_TL_TYPE_TEXT) {
                fprintf(stream, "%s\n", text->Data);
        }
}


/**
 * oh_sensor_reading2str: 
 * @reading: SaHpiSensorReadingT value to convert
 * @format: SaHpiDataFormatT for the sensor reading in question
 * @text: SaHpiTextBufferT to be created
 * 
 * Converts a sensor reading & format to an SaHpiTextBufferT
 * 
 * Returns: SA_OK - everything worked
 *          SA_ERR_HPI_INVALID_CMD - format or reading have IsSupported == FALSE
 *          SA_ERR_HPI_INVALID_DATA - format and reading types don't match
 *          SA_ERR_HPI_OUT_OF_SPACE - partial write to text buffer because it
 *                                      ran out of space
 **/
SaErrorT oh_sensor_reading2str (SaHpiSensorReadingT reading, 
                                SaHpiSensorDataFormatT format, 
                                SaHpiTextBufferT *text)
{
        SaHpiTextBufferT temp;
        char buff[SAHPI_MAX_TEXT_BUFFER_LENGTH];
        int buffsize = 0;

        deprecated("Please check out the other util functions");

        setup_text_buffer(&temp);        
        if(!reading.IsSupported || !format.IsSupported) {
                dbg("Can't stringify unsupported numeric reading");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        if(reading.Type != format.ReadingType) {
                dbg("Format definition is different from sensor reading type");
                return SA_ERR_HPI_INVALID_DATA;
        }
        
        switch(reading.Type) {
        case SAHPI_SENSOR_READING_TYPE_BUFFER:
                oh_text_buffer_append(&temp, reading.Value.SensorBuffer, 
                                      SAHPI_SENSOR_BUFFER_LENGTH);
                break;
        case SAHPI_SENSOR_READING_TYPE_INT64:
                reset_buff(buff);
                buffsize = sprintf(buff, "%lld", reading.Value.SensorInt64);
                oh_text_buffer_append(&temp, buff, buffsize);
                break;
        case SAHPI_SENSOR_READING_TYPE_UINT64: 
                reset_buff(buff);
                buffsize = sprintf(buff, "%llu", reading.Value.SensorUint64);
                oh_text_buffer_append(&temp, buff, buffsize);
                break;
       case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                reset_buff(buff);
                buffsize = sprintf(buff, "%le", reading.Value.SensorFloat64);
                oh_text_buffer_append(&temp, buff, buffsize);
                break;
        }

        
        if(format.Percentage) {
                oh_text_buffer_append(&temp, "%", 1);
        }

        /* now we add the units */
        
        if(format.BaseUnits != SAHPI_SU_UNSPECIFIED) {
                oh_text_buffer_append(&temp, " ", 1);
                reset_buff(buff);
                buffsize = sprintf(buff,"%s",oh_lookup_sensorunits(format.BaseUnits));
                oh_text_buffer_append(&temp, buff, buffsize);
        }

        /* add modifier units if appropriate */
        if(format.ModifierUse != SAHPI_SMUU_NONE) {
                if(format.ModifierUse == SAHPI_SMUU_BASIC_OVER_MODIFIER) {
                        oh_text_buffer_append(&temp, " / ", 3);
                } else {
                        oh_text_buffer_append(&temp, " * ", 3);
                }

                reset_buff(buff);
                buffsize = sprintf(buff,"%s",oh_lookup_sensorunits(format.ModifierUnits));
                oh_text_buffer_append(&temp, buff, buffsize);
        }
        
        memcpy(text, &temp, sizeof(temp));
        
        return SA_OK;
}

/* end of file         */
