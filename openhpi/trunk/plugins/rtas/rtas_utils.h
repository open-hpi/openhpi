/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Renier Morales <renierm@users.sf.net>
 *        Daniel de Araujo <ddearauj@us.ibm.com>
 */

#ifndef RTAS_UTILS_H
#define RTAS_UTILS_H


#include <glib.h>
#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_utils.h>
#include <oh_error.h>
#include <oh_domain.h>
#include "/home/dan/downloads/librtas-1.2.4/librtas_src/librtas.h"

void decode_rtas_error (int error, char *buf, size_t size, int token, int index);
		       
void populate_rtas_sensor_info(int token, SaHpiSensorRecT *sensor_info);

#endif /* RTAS_UTILS_H */
