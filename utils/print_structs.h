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

#ifndef OH_PRINT_STRUCTS_H
#define OH_PRINT_STRUCTS_H

#define reset_buff(buff) memset(buff, 0, SAHPI_MAX_TEXT_BUFFER_LENGTH)

void fprint_text_buffer(FILE *, const SaHpiTextBufferT*);

SaErrorT oh_sensor_reading2str (SaHpiSensorReadingT reading, 
                                SaHpiSensorDataFormatT format, 
                                SaHpiTextBufferT *text);

#endif
/* end of file         */
