/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Chris Chia <cchia@users.sf.net>
 */

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <bc_resources.h>
#include <snmp_bc_utils.h>

/**********************************************************************
 * snmp_derive_objid : 
 * starting from end of objid string,
 * replacing letter x, with last instance number of entity
 * path, repeat process until all x are replaced by an instance number.
 * @entity_path: Contains the instance numbers to complete the oid.
 * @objid: string containing the OID
 *
 * return value: new objid string with x replaced
 *               NULL if failed
 **********************************************************************/

gchar * snmp_derive_objid(SaHpiEntityPathT ep, const gchar *oid)
{
        gchar *new_oid = NULL, *oid_walker = NULL;
        gchar **fragments = NULL, **oid_nodes = NULL;
        guint num_epe, num_blanks, oid_strlen = 0;
        guint total_num_digits, i, work_instance_num, num_digits;

        for (num_epe = 0;
             ep.Entry[num_epe].EntityType != 0 && num_epe < SAHPI_MAX_ENTITY_PATH;
             num_epe++);
        /*dbg("Number of elements in entity path: %d", num_epe);*/

        if (num_epe == 0) {
                dbg("derive_objid: Error. Entity path is null.");
                return NULL;
        }
        if ((oid_strlen = strlen(oid)) == 0) return NULL; /* Oid is zero length. */
        if (!strrchr(oid, OID_BLANK_CHAR)) return g_strdup(oid); /* Nothing to replace. */

        for (num_blanks = 0, i = 0; i < oid_strlen; i++) {
                if (oid[i] == OID_BLANK_CHAR) num_blanks++;
        }
        /*dbg("Number of blanks in oid: %d, %s", num_blanks, oid);*/

        if (num_blanks > num_epe) {
                dbg("derive_objid: Error. number of x %d > entity path elements %d\n", num_blanks, num_epe);
                return NULL;
        }

        fragments = g_strsplit(oid, OID_BLANK_STR, -1);
        if (!fragments) {dbg("Could not split oid"); goto CLEANUP;}
        oid_nodes = g_malloc0((num_blanks+1)*sizeof(gchar **));
        if (!oid_nodes) {dbg("Out of memory"); goto CLEANUP;}
        total_num_digits = 0;
        for (i = 0; i < num_blanks; i++) {
                work_instance_num = ep.Entry[num_blanks-1-i].EntityLocation;
                for (num_digits = 1;
                     (work_instance_num = work_instance_num/10) > 0; num_digits++);
                oid_nodes[i] = g_malloc0((num_digits+1)*sizeof(gchar));
                if (!oid_nodes[i]) {dbg("Out of memory"); goto CLEANUP;}
                snprintf(oid_nodes[i], (num_digits+1)*sizeof(gchar), "%d", 
			 ep.Entry[num_blanks-1-i].EntityLocation);
                /*dbg("Instance number: %s", oid_nodes[i]);*/
                total_num_digits = total_num_digits + num_digits;
        }

        new_oid = g_malloc0((oid_strlen-num_blanks+total_num_digits+1)*sizeof(gchar));
        if (!new_oid) {dbg("Out of memory"); goto CLEANUP;}
        oid_walker = new_oid;
        for (i = 0; fragments[i]; i++) {
                oid_walker = strcpy(oid_walker, fragments[i]);
                oid_walker = oid_walker + strlen(fragments[i]);
                if (oid_nodes[i]) {
                        oid_walker = strcpy(oid_walker, oid_nodes[i]);
                        /*dbg("Instance number: %s", oid_nodes[i]);*/
                        oid_walker = oid_walker + strlen(oid_nodes[i]);
                }
                /*dbg("Forming new oid: %s", new_oid);*/
        }

CLEANUP:
        g_strfreev(fragments);
        g_strfreev(oid_nodes);

        return new_oid;
}

/**
 * get_interpreted_value : Converts a string to its numeric equivalent based on 
 * the specified type. 
 * The number is placed in interpreted_value (a union) and success is returned. 
 * If type is SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER or -1,
 * then it simply copies the string to the right place in interpreted_value
 * and success is returned.
 * @string: IN. String to be converted to a number (e.g. " -1.43 Volts").
 * @type: IN. type of number to be extracted from string
 *        (e.g. SAHPI_SENSOR_INTERPRETED_TYPE_INT8).
 * @value: OUT. pointer to a union where the numeric value is placed.
 *
 * return value: 0 success
 *               -1 failure
 **/
int get_interpreted_value(gchar *string,
			  SaHpiSensorReadingTypeT type,
			  SaHpiSensorReadingUnionT *value)
{
        gchar *tail;
        gchar *str = g_strdup((const gchar*)string);
        guint len = 0;
        gboolean percent = FALSE;
        SaHpiFloat64T result_f = 0.0;
        SaHpiInt64T   result_l = 0;
        SaHpiUint64T  result_ul = 0;

        memset(value, 0, sizeof(SaHpiSensorReadingUnionT));

        /*dbg("%s to be converted to type %d value\n", string, type);*/
        if(str == NULL) {
                dbg("Error: can\'t convert a null string\n");
                return -1;
        }
        if((type == SAHPI_SENSOR_READING_TYPE_BUFFER) ||
           (type == -1)) {                
                if((strlen(str)+1) > sizeof(SaHpiSensorReadingUnionT)) {
                        dbg("Error: string too long\n");
                        g_free(str);
                        return -1;
                }
                else {
                        strncpy(value->SensorBuffer, str, SAHPI_SENSOR_BUFFER_LENGTH);
                        /*dbg("no conversion, just a string copy job\n");*/
                        g_free(str);
                        return 0;
                }
        }
        g_strstrip(str);  // get rid of leading and trailing blanks
        len = strlen(str);
        if (str[0] == '+' || str[0] == '-') {
                guint i,j;
                for (i = 1; i < len; i++)
                        if (str[i] != ' ') break;

                for (j = i; j < len; j++)
                        if (str[j] == ' ') break;

                if (i != 1) {
                        g_memmove(str+1, str+i, j-i);
                        str[j-1] = '\0';
                }
                else str[j] = '\0';
        } else str = strtok(str," ");
        len = strlen(str);

        // number can end with a character of % or v such as 50% or 3.3v
        if (str[len - 1] == '%' || str[len - 1] == 'v') {
                if (str[len - 1] == '%') percent = TRUE;
                str[len - 1] = '\0';  // delete % or v
                if (type != SAHPI_SENSOR_READING_TYPE_FLOAT64) {
                        dbg("Error: can\'t convert fraction to an integer\n");
                        g_free(str);
                        return -1;
                }
        }

        switch (type) {
        case SAHPI_SENSOR_READING_TYPE_INT64:
                errno = 0;
                result_l = strtoll(str, &tail, 10);
                if (errno) {
                        dbg("Error: strtoll failed, errno = %d\n", errno);
                        return -1;
                }
                if (*tail != '\0') {
                        dbg("Error: strtoll failed: %s\n", str);
                        if (tail == str) {
                                dbg("Info: no numeric value found in string, default to zero\n");
                                value->SensorInt64 = 0;
                                g_free(str);
                                return 0;
                        }
                        g_free(str);
                        return -1;
                }

                dbg("converted signed value = %d\n", (int)result_l);
                value->SensorInt64 = result_l;
                break;

        case SAHPI_SENSOR_READING_TYPE_UINT64:
                errno = 0;
                result_ul = strtoull(str, &tail, 10);
                if (errno) {
                        dbg("Error: strtoull failed, errno = %d\n", errno);
                        return -1;
                }
                if (*tail != '\0') {
                        dbg("Error: strtoull failed\n");
                        if (tail == str) {
                                dbg("Info: no numeric value found in string, default to zero\n");
                                value->SensorUint64 = 0;
                                g_free(str);
                                return 0;
                        }
                        g_free(str);
                        return -1;
                }

                dbg("converted unsigned value = %u\n", (uint)result_ul);
                value->SensorUint64 = result_ul;
                break;

        case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                result_f = strtold(str, &tail);
                if (*tail != '\0') {
                        dbg("Error: strtold failed\n");
                        if (tail == str) {
                                dbg("Info: no numeric value found in string, default to zero\n");
                                value->SensorFloat64 = 0.0;
                                g_free(str);
                                return 0;
                        }
                        g_free(str);
                        return -1;
                }

                /*dbg("converted floating point value = %f\n", (double)result_f);*/
                if (percent) {
                        value->SensorFloat64 = result_f/100.0;
                }
                else {
                        value->SensorFloat64 = result_f;
                }
                break;

        default:
                dbg("Error: invalid type %d\n", type);
                g_free(str);
                return -1;
        }
        g_free(str);
	return 0;
}
