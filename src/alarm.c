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
 *      Renier Morales <renierm@users.sf.net>
 *
 */

#include <oh_alarm.h>

/**
 * oh_detect_event_alarm
 * @e:
 *
 * Return value:
 **/
SaErrorT oh_detect_event_alarm(struct oh_event *e)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * oh_detect_resource_alarm
 * @res:
 *
 * Return value:
 **/
SaErrorT oh_detect_resource_alarm(SaHpiRptEntryT *res)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * oh_detect_sensor_alarm
 * @rdr:
 * @enable:
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_alarm(SaHpiRdrT *rdr, SaHpiBoolT enable)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * oh_detect_sensor_mask_alarm
 * @rdr:
 * @state:
 *
 * Return value:
 **/
SaErrorT oh_detect_sensor_mask_alarm(SaHpiRdrT *rdr, SaHpiEventStateT state)
{
        return SA_ERR_HPI_UNSUPPORTED_API;
}
