/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Racing Guo <racing.guo@intel.com>
 *     Aaron  Chen <yukun.chen@intel.com>
 *     Nick   Yin <hu.yin@intel.com>
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SaHpi.h>

/*  Transition from string to SAHPI type begin */

int sa_discover(void);
int sa_list_res(void);
int sa_show_rpt(SaHpiResourceIdT resourceid);
int sa_show_rdr(SaHpiResourceIdT resourceid);

//int str2entity_type(char *str, SaHpiEntityTypeT entity_types);
#define DECLARE_STR_TO_SAHPI_TYPE_FUN(function_name, TYPE) \
int function_name(char *str, TYPE *type);

#define STR_TO_SAHPI_TYPE_FUN(function_name, TYPE, types)    \
int function_name(char *str, TYPE *type)              \
{                                                            \
    int i;                                                   \
                                                             \
    for (i = 0; i < sizeof(types)/sizeof(types[0]); i++) {   \
         if (!strcmp(str, types[i].str)) {                   \
             *type = types[i].type;                          \
             return 0;                                       \
         }                                                   \
    }                                                        \
    return -1;                                               \
}
#define DECLARE_SAHPI_TYPE_TO_STR_FUN(function_name, TYPE) \
int function_name(char *str, int slen, TYPE type);

#define SAHPI_TYPE_TO_STR_FUN(function_name, TYPE, types)    \
int function_name(char *str, int slen, TYPE type)     \
{                                                            \
     int i;                                                  \
                                                             \
     for (i = 0; i < sizeof(types)/sizeof(types[0]); i++) {  \
         if (type == types[i].type) {                        \
              int len;                                       \
                                                             \
              len = strlen(types[i].str);                    \
              if (len < slen) {                              \
                   strcpy(str, types[i].str);                \
                   return 0;                                 \
              }                                              \
              return (-1-len);                               \
         }                                                   \
    }                                                        \
    return -1;                                               \
}


DECLARE_STR_TO_SAHPI_TYPE_FUN(str2entity_type, SaHpiEntityTypeT)
DECLARE_SAHPI_TYPE_TO_STR_FUN(entity_type2str, SaHpiEntityTypeT)

DECLARE_STR_TO_SAHPI_TYPE_FUN(str2severity, SaHpiSeverityT)
DECLARE_SAHPI_TYPE_TO_STR_FUN(severity2str, SaHpiSeverityT)

DECLARE_STR_TO_SAHPI_TYPE_FUN(str2rdr_type, SaHpiRdrTypeT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sensor_type, SaHpiSensorTypeT)
DECLARE_SAHPI_TYPE_TO_STR_FUN(sensor_type2str, SaHpiEntityTypeT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2event_ctrl_type, SaHpiSensorEventCtrlT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sign_fmt_type, SaHpiSensorSignFormatT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sensor_unit, SaHpiSensorUnitsT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sensor_mod_unit_use, SaHpiSensorModUnitUseT)
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sensor_line, SaHpiSensorLinearizationT) 
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2sensor_interpreted_type, SaHpiSensorInterpretedTypeT) 
DECLARE_SAHPI_TYPE_TO_STR_FUN(sensor_interpreted_type2str, SaHpiSensorInterpretedTypeT) 
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2ctrl_output_type, SaHpiCtrlOutputTypeT) 
DECLARE_SAHPI_TYPE_TO_STR_FUN(ctrl_output_type2str, SaHpiCtrlOutputTypeT) 
DECLARE_STR_TO_SAHPI_TYPE_FUN(str2ctrl_type, SaHpiCtrlTypeT) 
DECLARE_SAHPI_TYPE_TO_STR_FUN(ctrl_type2str, SaHpiCtrlTypeT) 
