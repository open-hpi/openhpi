/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 * 
 * Authours:
 *      Racing Guo <racing.guo@intel.com>
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <glib.h>

#include <SaHpi.h>
#include <openhpi.h>

#include "sim_parser.h"

#define trace(f, ...) do {;} while(0)
#ifdef UNIT_TEST
#define dbg(format, ...)                                        \
        do {                                                    \
                fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
                fprintf(stderr, format "\n", ## __VA_ARGS__);   \
        } while(0)
#endif

/* Grammar Part Begin */

typedef struct {
       char* name;
       enum {
          SIM_FHS_STRING,
          SIM_FHS_SEQ,
          SIM_FHS_ARRAY,
       }type;
       union{
          char*   str;
          GSList* seq;
          GSList* array;
       }value;
}fhs_node;

static GScannerConfig sim_scanner_config =
        {
                (
                        " \t\n\r"
                        )                       /* cset_skip_characters */,
                (
                        G_CSET_a_2_z
                        G_CSET_A_2_Z
                        )                       /* cset_identifier_first */,
                (
                        G_CSET_a_2_z
                        "_-0123456789"
                        G_CSET_A_2_Z
                        )                       /* cset_identifier_nth */,
                ( "#\n" )               /* cpair_comment_single */,
                TRUE                    /* case_sensitive */,
                TRUE                    /* skip_comment_multi */,
                TRUE                    /* skip_comment_single */,
                TRUE                    /* scan_comment_multi */,
                TRUE                    /* scan_identifier */,
                TRUE                    /* scan_identifier_1char */,
                FALSE                   /* scan_identifier_NULL */,
                FALSE                   /* scan_symbols */,
                FALSE                   /* scan_binary */,
                FALSE                   /* scan_octal */,
                FALSE                    /* scan_float */,
                FALSE                   /* scan_hex */,
                FALSE                   /* scan_hex_dollar */,
                TRUE                    /* scan_string_sq */,
                TRUE                    /* scan_string_dq */,
                FALSE                   /* numbers_2_int */,
                FALSE                   /* int_2_float */,
                FALSE                   /* identifier_2_string */,
                TRUE                    /* char_2_token */,
                FALSE                   /* symbol_2_token */,
                FALSE                   /* scope_0_fallback */,
        };

static void sim_scanner_msg_handler (GScanner *scanner, gchar *message, 
                                     gboolean is_error)
{
       g_return_if_fail (scanner != NULL);
       dbg("file(%s):line(%d):position(%d):%s\n",
           scanner->input_name, scanner->line, scanner->position, message);
}

static void free_fhs_node(GSList *fhs)
{
    GSList *list;
  
    if (fhs == NULL)
        return;    

    for (list = fhs; list; list = g_slist_next(list)) {
       fhs_node *n = (fhs_node*)(list->data);
       
       if (n == NULL) {
           dbg("some error: node is null\n");
           continue;
       }      
 
       g_free(n->name);
       switch (n->type) {
          case SIM_FHS_STRING:
               g_free(n->value.str);
               break;
          case SIM_FHS_ARRAY:
               {
                   GSList *tmp;  
                   tmp = n->value.array;
                   for (tmp = n->value.array; tmp; tmp = g_slist_next(tmp))
                       g_free(tmp->data);
                   g_slist_free(n->value.array);
               }
               break;
          case SIM_FHS_SEQ:
               free_fhs_node(n->value.seq);
               break;
          default:
               dbg("error type(%d) in fhs_node", n->type);
               break;
       }
       g_free(n);
    }
    g_slist_free(fhs);
}
static GSList* __sim_parser(GScanner* sim_scanner, guint end_token)
{
#define NULL_CHECK(x)                             \
 do {                                             \
     if ((x)==NULL) {                             \
        dbg("Couldn't allocate memory for "#x);   \
        goto error_process ;                      \
     }                                            \
 }while(0)

       GSList    *retval = NULL; 
       fhs_node  *node = NULL;
       char      *str;
       guint     token;
       for (;;) {  

            token = g_scanner_get_next_token (sim_scanner);

            if (token == end_token) {
                 break;
            }else if (token == G_TOKEN_IDENTIFIER) {
                 
                 node = g_malloc0(sizeof(*node));
                 NULL_CHECK(node);
                 retval = g_slist_append(retval, node);
                 str = strdup(sim_scanner->value.v_string);
                 NULL_CHECK(str);
                 node->name = str;
                 trace("id:%s", node->name);
            }else {
                 sim_scanner->msg_handler(sim_scanner, 
                                          "expected id, EOF or }", 1);
                 goto error_process; 
            }
            
            token = g_scanner_get_next_token (sim_scanner);
            if (token != G_TOKEN_EQUAL_SIGN) {
                 sim_scanner->msg_handler(sim_scanner, "expected =", 1);
                 goto error_process;
            }

            token = g_scanner_get_next_token (sim_scanner);
            if (token == G_TOKEN_STRING) {
                  node->type = SIM_FHS_STRING;
                  str = strdup(sim_scanner->value.v_string);
                  NULL_CHECK(str);
                  node->value.str = str;
                  trace("string:%s", node->value.str);
           
            }else if (token == G_TOKEN_LEFT_CURLY) {
                  token = g_scanner_peek_next_token (sim_scanner);
                  if (token == G_TOKEN_IDENTIFIER) {
                      GSList *tmp; 
                      tmp = __sim_parser(sim_scanner, G_TOKEN_RIGHT_CURLY); 
                      if (tmp == NULL)
                          goto error_process;
                      node->type = SIM_FHS_SEQ;
                      node->value.seq = tmp;   
                  }else if (token == G_TOKEN_STRING) {
                       GSList *array = NULL;
                       for(;;) {
                             token = g_scanner_get_next_token (sim_scanner);
                             if (token != G_TOKEN_STRING) {
                                  sim_scanner->msg_handler(sim_scanner,
                                          "expected string", 1);
                                  goto error_process;
                             }
                             str = strdup(sim_scanner->value.v_string);
                             NULL_CHECK(str);
                             trace("string:%s", str);
                             array = g_slist_append(array, str);
                             token = g_scanner_get_next_token (sim_scanner);
                             if (token == G_TOKEN_RIGHT_CURLY) {
                                  break;
                             }else if (token != G_TOKEN_COMMA) {
                                  sim_scanner->msg_handler(sim_scanner,
                                                           "expected ,", 1);
                                  goto error_process;
                             }
                       }
                       node->type = SIM_FHS_ARRAY;
                       node->value.array = array;

                  }else {
                       sim_scanner->msg_handler(sim_scanner, 
                                                "expected id or string", 1);
                       goto error_process;
                  }
            }else {
                  sim_scanner->msg_handler(sim_scanner, 
                                           "expected string or {", 1);
                  goto error_process;
            }
       }
       return retval;
error_process:
       
       if (retval)
           free_fhs_node(retval);
       return NULL;
}

static GSList* sim_parser(char *filename) 
{
        int       sim_parser_file;
        GScanner  *sim_scanner;
        GSList    *retval;
        
        sim_scanner = g_scanner_new(&sim_scanner_config);
        if(!sim_scanner) {
                dbg("Couldn't allocate g_scanner for file parsing");
                return NULL;
        }

        sim_scanner->msg_handler = sim_scanner_msg_handler;
        sim_scanner->input_name = filename;

        sim_parser_file = open(filename, O_RDONLY);
        if(sim_parser_file < 0) {
                dbg("Configuration file '%s' couldn't be opened", filename);
                return NULL;
        }

        g_scanner_input_file(sim_scanner, sim_parser_file);
        
        retval = __sim_parser(sim_scanner, G_TOKEN_EOF);        
        if(close(sim_parser_file) != 0)
               dbg("Configuration file '%s' couldn't be closed", filename);
        g_scanner_destroy(sim_scanner);
        return retval;

}

static int __sim_generate(GSList * fhs, FILE *file, int hi_num)
{
    int i;

    if (hi_num > 0) {
        fprintf(file, "{\n");
    }
  
    for ( ; fhs; fhs = g_slist_next(fhs)) {
       fhs_node *n;

       n = (fhs_node*) fhs->data;
       for (i = 0; i < hi_num; i++)
           fprintf(file, "    ");
       fprintf(file, "%s = ", n->name);
       switch (n->type) {
          case SIM_FHS_STRING:
               fprintf(file, "\"%s\"\n",n->value.str);
               break;
          case SIM_FHS_ARRAY:
               {
                   GSList *list;
         
                   fprintf(file, "{");     
                   list = n->value.array;
                   fprintf(file, "\"%s\"", (char*)list->data);
                   for (list = g_slist_next(list); list; 
                                list = g_slist_next(list)) 
                         fprintf(file, ", \"%s\"", (char*)list->data); 
                   fprintf(file, "}\n");
               }
               break;
          case SIM_FHS_SEQ:
               __sim_generate(n->value.seq, file, hi_num+1);
               break;
          default:
               dbg("error type(%d) in fhs_node", n->type);
       }
    } 

    if (hi_num > 0) {
      for (i = 0; i < hi_num - 1; i++)
           fprintf(file, "    ");
      fprintf(file,"}\n");
    }
    return 0;
}

static int sim_generate(char *filename, GSList * fhs)
{
    int retval;
    FILE*  file;
    
    file = fopen(filename, "w");
    if (file == NULL) {
       dbg("The file %s could not be opened", filename);
       return -1;
    }

    if (fhs == NULL) {
       fclose(file);
       dbg(" fhs is null in file(%s)\n", filename);
       return -1;
    }

    retval = __sim_generate(fhs, file, 0);
    fclose(file);
    return retval;
}
/*Grammar Part End*/


/*Semantic Part Begin*/

static char* sim_get_string_value(GSList *fhs, char *id)
{
    GSList *list;
    for (list = fhs; list; list = g_slist_next(list)) {
       fhs_node *n;
       n = (fhs_node*) list->data;
       if (n == NULL) {
           dbg("error in fhs tree\n");
           return NULL;
       }
       if (!strcmp(id, n->name)) {
          if (n->type == SIM_FHS_STRING) 
               return n->value.str;
          else 
               return NULL;
       }
    }
    return NULL;
}

static GSList* sim_get_seq_value(GSList *fhs, char *id)
{

    GSList *list;
    for (list = fhs; list; list = g_slist_next(list)) {
       fhs_node *n;
       n = (fhs_node*) list->data;
       if (n == NULL) {
           dbg("error in fhs tree\n");
           return NULL;
       }
       if (!strcmp(id, n->name)) {
          if (n->type == SIM_FHS_SEQ)
               return n->value.seq;
          else 
               return NULL;
       }
    }
    return NULL;

}

static GSList* sim_get_array_value(GSList *fhs, char *id)
{
    GSList *list;

    for (list = fhs; list; list = g_slist_next(list)) {
       fhs_node *n;
       n = (fhs_node*) list->data;
       if (n == NULL) {
           dbg("error in fhs tree\n");
           return NULL;
       }
       if (!strcmp(id, n->name)) {
             if (n->type == SIM_FHS_ARRAY)
                  return n->value.array;
             else 
                  return NULL;
       }
    }
    return NULL;

}

/*  Transition of element data begin   */
static int str2text(char *str, SaHpiTextBufferT *text)
{
   int len;

   text->DataType = SAHPI_TL_TYPE_ASCII6;
   text->Language = SAHPI_LANG_ENGLISH;
 
   len = strlen(str);

   if (len > SAHPI_MAX_TEXT_BUFFER_LENGTH) {
       dbg("overflow in SaHpiTextBufferT\n");
       return -1;
   } 

   text->DataLength = len;
   memcpy(text->Data, str, len);
   return 0;
}
#define STR_TO_INT(function_name, TYPE, BYTES_NUM)     \
static int function_name(char *str, TYPE *val)         \
{                                                      \
   int i, len, flag = 0;                               \
   TYPE  value = 0;                                    \
                                                       \
   len = strlen(str);                                  \
   if (*str == '-') {                                  \
      flag = 1;                                        \
      str++;                                           \
      len--;                                           \
   }                                                   \
   if (len > BYTES_NUM || len <= 0)  return -1;        \
   for (i = 0; i< len; i++) {                          \
      if ((str[i] >= '0') && (str[i] <= '9')) {        \
         value = (str[i] - '0') + value*16;            \
      }else if ((str[i] >= 'a') && (str[i] <= 'f')) {  \
         value = (str[i] - 'a'+ 10) + value*16;        \
      }else {                                          \
         return -1;                                    \
      }                                                \
   }                                                   \
   if (flag)                                           \
      *val = -value;                                   \
   else                                                \
      *val = value;                                    \
   return 0;                                           \
}             

STR_TO_INT(str2uint8,  SaHpiUint8T,  2)
STR_TO_INT(str2uint16, SaHpiUint16T, 4)
STR_TO_INT(str2uint32, SaHpiUint32T, 8)
STR_TO_INT(str2int8,   SaHpiInt8T,   2)
STR_TO_INT(str2int16,  SaHpiInt16T,  4)
STR_TO_INT(str2int32,  SaHpiInt32T,  8)

/*  Transition of element data end */


/*  Transition from string to SAHPI type begin */

#define STR_TO_SAHPI_TYPE_FUN(function_name, TYPE, types)    \
static int function_name(char *str, TYPE *type)              \
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

#define SAHPI_TYPE_TO_STR_FUN(function_name, TYPE, types)    \
static int function_name(char *str, int slen, TYPE type)     \
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

static struct{
    char *str;
    SaHpiEntityTypeT type;
} entity_types[] = {
     {"SAHPI_ENT_IPMI_GROUP", SAHPI_ENT_IPMI_GROUP},
     {"SAHPI_ENT_UNSPECIFIED", SAHPI_ENT_UNSPECIFIED },
     {"SAHPI_ENT_OTHER", SAHPI_ENT_OTHER},
     {"SAHPI_ENT_UNKNOWN", SAHPI_ENT_UNKNOWN},
     {"SAHPI_ENT_PROCESSOR", SAHPI_ENT_PROCESSOR},
     {"SAHPI_ENT_DISK_BAY", SAHPI_ENT_DISK_BAY},
     {"SAHPI_ENT_PERIPHERAL_BAY", SAHPI_ENT_PERIPHERAL_BAY},
     {"SAHPI_ENT_SYS_MGMNT_MODULE", SAHPI_ENT_SYS_MGMNT_MODULE},
     {"SAHPI_ENT_SYSTEM_BOARD", SAHPI_ENT_SYSTEM_BOARD},
     {"SAHPI_ENT_MEMORY_MODULE", SAHPI_ENT_MEMORY_MODULE},
     {"SAHPI_ENT_PROCESSOR_MODULE", SAHPI_ENT_PROCESSOR_MODULE},
     {"SAHPI_ENT_POWER_SUPPLY", SAHPI_ENT_POWER_SUPPLY},
     {"SAHPI_ENT_ADD_IN_CARD", SAHPI_ENT_ADD_IN_CARD},
     {"SAHPI_ENT_FRONT_PANEL_BOARD", SAHPI_ENT_FRONT_PANEL_BOARD},
     {"SAHPI_ENT_BACK_PANEL_BOARD", SAHPI_ENT_BACK_PANEL_BOARD},
     {"SAHPI_ENT_POWER_SYSTEM_BOARD", SAHPI_ENT_POWER_SYSTEM_BOARD},
     {"SAHPI_ENT_DRIVE_BACKPLANE", SAHPI_ENT_DRIVE_BACKPLANE},
     {"SAHPI_ENT_SYS_EXPANSION_BOARD", SAHPI_ENT_SYS_EXPANSION_BOARD},
     {"SAHPI_ENT_OTHER_SYSTEM_BOARD", SAHPI_ENT_OTHER_SYSTEM_BOARD},
     {"SAHPI_ENT_PROCESSOR_BOARD", SAHPI_ENT_PROCESSOR_BOARD},
     {"SAHPI_ENT_POWER_UNIT", SAHPI_ENT_POWER_UNIT},
     {"SAHPI_ENT_POWER_MODULE", SAHPI_ENT_POWER_MODULE},
     {"SAHPI_ENT_POWER_MGMNT", SAHPI_ENT_POWER_MGMNT},
     {"SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD", SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD},
     {"SAHPI_ENT_SYSTEM_CHASSIS", SAHPI_ENT_SYSTEM_CHASSIS},
     {"SAHPI_ENT_SUB_CHASSIS", SAHPI_ENT_SUB_CHASSIS},
     {"SAHPI_ENT_OTHER_CHASSIS_BOARD", SAHPI_ENT_OTHER_CHASSIS_BOARD},
     {"SAHPI_ENT_DISK_DRIVE_BAY", SAHPI_ENT_DISK_DRIVE_BAY},
     {"SAHPI_ENT_PERIPHERAL_BAY_2", SAHPI_ENT_PERIPHERAL_BAY_2},
     {"SAHPI_ENT_DEVICE_BAY", SAHPI_ENT_DEVICE_BAY},
     {"SAHPI_ENT_COOLING_DEVICE", SAHPI_ENT_COOLING_DEVICE},
     {"SAHPI_ENT_COOLING_UNIT", SAHPI_ENT_COOLING_UNIT},
     {"SAHPI_ENT_INTERCONNECT", SAHPI_ENT_INTERCONNECT},
     {"SAHPI_ENT_MEMORY_DEVICE", SAHPI_ENT_MEMORY_DEVICE},
     {"SAHPI_ENT_SYS_MGMNT_SOFTWARE", SAHPI_ENT_SYS_MGMNT_SOFTWARE},
     {"SAHPI_ENT_BIOS", SAHPI_ENT_BIOS},
     {"SAHPI_ENT_OPERATING_SYSTEM", SAHPI_ENT_OPERATING_SYSTEM},
     {"SAHPI_ENT_SYSTEM_BUS", SAHPI_ENT_SYSTEM_BUS},
     {"SAHPI_ENT_GROUP", SAHPI_ENT_GROUP},
     {"SAHPI_ENT_REMOTE", SAHPI_ENT_REMOTE},
     {"SAHPI_ENT_EXTERNAL_ENVIRONMENT", SAHPI_ENT_EXTERNAL_ENVIRONMENT},
     {"SAHPI_ENT_BATTERY", SAHPI_ENT_BATTERY},
     {"SAHPI_ENT_CHASSIS_SPECIFIC", SAHPI_ENT_CHASSIS_SPECIFIC},
     {"SAHPI_ENT_BOARD_SET_SPECIFIC", SAHPI_ENT_BOARD_SET_SPECIFIC},
     {"SAHPI_ENT_OEM_SYSINT_SPECIFIC", SAHPI_ENT_OEM_SYSINT_SPECIFIC},
     {"SAHPI_ENT_ROOT", SAHPI_ENT_ROOT },
     {"SAHPI_ENT_RACK", SAHPI_ENT_RACK },
     {"SAHPI_ENT_SUBRACK", SAHPI_ENT_SUBRACK},
     {"SAHPI_ENT_COMPACTPCI_CHASSIS", SAHPI_ENT_COMPACTPCI_CHASSIS},
     {"SAHPI_ENT_ADVANCEDTCA_CHASSIS", SAHPI_ENT_ADVANCEDTCA_CHASSIS},
     {"SAHPI_ENT_SYSTEM_SLOT", SAHPI_ENT_SYSTEM_SLOT},
     {"SAHPI_ENT_SBC_BLADE", SAHPI_ENT_SBC_BLADE},
     {"SAHPI_ENT_IO_BLADE", SAHPI_ENT_IO_BLADE},
     {"SAHPI_ENT_DISK_BLADE", SAHPI_ENT_DISK_BLADE},
     {"SAHPI_ENT_DISK_DRIVE", SAHPI_ENT_DISK_DRIVE},
     {"SAHPI_ENT_FAN", SAHPI_ENT_FAN},
     {"SAHPI_ENT_POWER_DISTRIBUTION_UNIT", SAHPI_ENT_POWER_DISTRIBUTION_UNIT},
     {"SAHPI_ENT_SPEC_PROC_BLADE", SAHPI_ENT_SPEC_PROC_BLADE},
     {"SAHPI_ENT_IO_SUBBOARD", SAHPI_ENT_IO_SUBBOARD},
     {"SAHPI_ENT_SBC_SUBBOARD", SAHPI_ENT_SBC_SUBBOARD},
     {"SAHPI_ENT_ALARM_MANAGER", SAHPI_ENT_ALARM_MANAGER},
     {"SAHPI_ENT_ALARM_MANAGER_BLADE", SAHPI_ENT_ALARM_MANAGER_BLADE},
     {"SAHPI_ENT_SUBBOARD_CARRIER_BLADE", SAHPI_ENT_SUBBOARD_CARRIER_BLADE},
};
STR_TO_SAHPI_TYPE_FUN(str2entity_type, SaHpiEntityTypeT, entity_types)
#ifdef UNIT_TEST
SAHPI_TYPE_TO_STR_FUN(entity_type2str, SaHpiEntityTypeT, entity_types)
#endif

static struct{
    char *str;
    SaHpiSeverityT  type;
} severity_types[] = {
   {"SAHPI_CRITICAL", SAHPI_CRITICAL},
   {"SAHPI_MAJOR", SAHPI_MAJOR},
   {"SAHPI_MINOR", SAHPI_MINOR},
   {"SAHPI_INFORMATIONAL", SAHPI_INFORMATIONAL},
   {"SAHPI_OK", SAHPI_OK},
   {"SAHPI_DEBUG", SAHPI_DEBUG},
};
STR_TO_SAHPI_TYPE_FUN(str2severity, SaHpiSeverityT, severity_types)
#ifdef UNIT_TEST
SAHPI_TYPE_TO_STR_FUN(severity2str, SaHpiSeverityT, severity_types)
#endif

static struct{
    char *str;
    SaHpiRdrTypeT  type;
} rdr_types[] = {
   {"SAHPI_NO_RECORD", SAHPI_NO_RECORD},
   {"SAHPI_CTRL_RDR", SAHPI_CTRL_RDR},
   {"SAHPI_SENSOR_RDR", SAHPI_SENSOR_RDR},
   {"SAHPI_INVENTORY_RDR", SAHPI_INVENTORY_RDR},
   {"SAHPI_WATCHDOG_RDR", SAHPI_WATCHDOG_RDR},
};
STR_TO_SAHPI_TYPE_FUN(str2rdr_type, SaHpiRdrTypeT, rdr_types)

static struct{
    char *str;
    SaHpiSensorTypeT  type;
} sensor_types[] = {
   {"SAHPI_TEMPERATURE", SAHPI_TEMPERATURE},
   {"SAHPI_VOLTAGE", SAHPI_VOLTAGE},
   {"SAHPI_CURRENT", SAHPI_CURRENT},
   {"SAHPI_FAN", SAHPI_FAN},
   {"SAHPI_PHYSICAL_SECURITY", SAHPI_PHYSICAL_SECURITY},
   {"SAHPI_PLATFORM_VIOLATION", SAHPI_PLATFORM_VIOLATION},
   {"SAHPI_PROCESSOR", SAHPI_PROCESSOR},
   {"SAHPI_POWER_SUPPLY", SAHPI_POWER_SUPPLY},
   {"SAHPI_POWER_UNIT", SAHPI_POWER_UNIT},
   {"SAHPI_COOLING_DEVICE", SAHPI_COOLING_DEVICE},
   {"SAHPI_OTHER_UNITS_BASED_SENSOR", SAHPI_OTHER_UNITS_BASED_SENSOR},
   {"SAHPI_MEMORY", SAHPI_MEMORY},
   {"SAHPI_DRIVE_SLOT", SAHPI_DRIVE_SLOT},
   {"SAHPI_POST_MEMORY_RESIZE", SAHPI_POST_MEMORY_RESIZE},
   {"SAHPI_SYSTEM_FW_PROGRESS", SAHPI_SYSTEM_FW_PROGRESS},
   {"SAHPI_EVENT_LOGGING_DISABLED", SAHPI_EVENT_LOGGING_DISABLED},
   {"SAHPI_RESERVED1", SAHPI_RESERVED1},
   {"SAHPI_SYSTEM_EVENT", SAHPI_SYSTEM_EVENT},
   {"SAHPI_CRITICAL_INTERRUPT", SAHPI_CRITICAL_INTERRUPT},
   {"SAHPI_BUTTON", SAHPI_BUTTON},
   {"SAHPI_MODULE_BOARD", SAHPI_MODULE_BOARD},
   {"SAHPI_MICROCONTROLLER_COPROCESSOR", SAHPI_MICROCONTROLLER_COPROCESSOR},
   {"SAHPI_ADDIN_CARD", SAHPI_ADDIN_CARD},
   {"SAHPI_CHASSIS", SAHPI_CHASSIS},
   {"SAHPI_CHIP_SET", SAHPI_CHIP_SET},
   {"SAHPI_OTHER_FRU", SAHPI_OTHER_FRU},
   {"SAHPI_CABLE_INTERCONNECT", SAHPI_CABLE_INTERCONNECT},
   {"SAHPI_TERMINATOR", SAHPI_TERMINATOR},
   {"SAHPI_SYSTEM_BOOT_INITIATED", SAHPI_SYSTEM_BOOT_INITIATED},
   {"SAHPI_BOOT_ERROR", SAHPI_BOOT_ERROR},
   {"SAHPI_OS_BOOT", SAHPI_OS_BOOT},
   {"SAHPI_OS_CRITICAL_STOP", SAHPI_OS_CRITICAL_STOP},
   {"SAHPI_SLOT_CONNECTOR", SAHPI_SLOT_CONNECTOR},
   {"SAHPI_SYSTEM_ACPI_POWER_STATE", SAHPI_SYSTEM_ACPI_POWER_STATE},
   {"SAHPI_RESERVED2", SAHPI_RESERVED2},
   {"SAHPI_PLATFORM_ALERT", SAHPI_PLATFORM_ALERT},
   {"SAHPI_ENTITY_PRESENCE", SAHPI_ENTITY_PRESENCE},
   {"SAHPI_MONITOR_ASIC_IC", SAHPI_MONITOR_ASIC_IC},
   {"SAHPI_LAN", SAHPI_LAN},
   {"SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH", SAHPI_MANAGEMENT_SUBSYSTEM_HEALTH},
   {"SAHPI_BATTERY", SAHPI_BATTERY},
   {"SAHPI_OPERATIONAL", SAHPI_OPERATIONAL},
   {"SAHPI_OEM_SENSOR", SAHPI_OEM_SENSOR}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_type, SaHpiSensorTypeT, sensor_types)

static struct{
    char *str;
    SaHpiSensorEventCtrlT  type;
} event_ctrl_types[] = {
   {"SAHPI_SEC_PER_EVENT", SAHPI_SEC_PER_EVENT},
   {"SAHPI_SEC_ENTIRE_SENSOR", SAHPI_SEC_ENTIRE_SENSOR},
   {"SAHPI_SEC_GLOBAL_DISABLE", SAHPI_SEC_GLOBAL_DISABLE},
   {"SAHPI_SEC_NO_EVENTS", SAHPI_SEC_NO_EVENTS},    
};
STR_TO_SAHPI_TYPE_FUN(str2event_ctrl_type, SaHpiSensorEventCtrlT, event_ctrl_types)

static struct{
    char *str;
    SaHpiSensorSignFormatT  type;
} sign_fmt_types[] = {
   {"SAHPI_SDF_UNSIGNED", SAHPI_SDF_UNSIGNED},
   {"SAHPI_SDF_1S_COMPLEMENT", SAHPI_SDF_1S_COMPLEMENT},
   {"SAHPI_SDF_2S_COMPLEMENT", SAHPI_SDF_2S_COMPLEMENT},
};
STR_TO_SAHPI_TYPE_FUN(str2sign_fmt_type, SaHpiSensorSignFormatT, sign_fmt_types)

static struct{
    char *str;
    SaHpiSensorUnitsT  type;
} sensor_unit_types[] = {
   {"SAHPI_SU_UNSPECIFIED", SAHPI_SU_UNSPECIFIED},
   {"SAHPI_SU_DEGREES_C", SAHPI_SU_DEGREES_C},
   {"SAHPI_SU_DEGREES_F", SAHPI_SU_DEGREES_F},
   {"SAHPI_SU_DEGREES_K", SAHPI_SU_DEGREES_K},
   {"SAHPI_SU_VOLTS", SAHPI_SU_VOLTS},
   {"SAHPI_SU_AMPS", SAHPI_SU_AMPS},
   {"SAHPI_SU_WATTS", SAHPI_SU_WATTS},
   {"SAHPI_SU_JOULES", SAHPI_SU_JOULES},
   {"SAHPI_SU_COULOMBS", SAHPI_SU_COULOMBS},
   {"SAHPI_SU_VA", SAHPI_SU_VA},
   {"SAHPI_SU_NITS", SAHPI_SU_NITS},
   {"SAHPI_SU_LUMEN", SAHPI_SU_LUMEN},
   {"SAHPI_SU_LUX", SAHPI_SU_LUX},
   {"SAHPI_SU_CANDELA", SAHPI_SU_CANDELA},
   {"SAHPI_SU_KPA", SAHPI_SU_KPA},
   {"SAHPI_SU_PSI", SAHPI_SU_PSI},
   {"SAHPI_SU_NEWTON", SAHPI_SU_NEWTON},
   {"SAHPI_SU_CFM", SAHPI_SU_CFM},
   {"SAHPI_SU_RPM", SAHPI_SU_RPM},
   {"SAHPI_SU_HZ", SAHPI_SU_HZ},
   {"SAHPI_SU_MICROSECOND", SAHPI_SU_MICROSECOND},
   {"SAHPI_SU_MILLISECOND", SAHPI_SU_MILLISECOND},
   {"SAHPI_SU_SECOND", SAHPI_SU_SECOND},
   {"SAHPI_SU_MINUTE", SAHPI_SU_MINUTE},
   {"SAHPI_SU_HOUR", SAHPI_SU_HOUR},
   {"SAHPI_SU_DAY", SAHPI_SU_DAY},
   {"SAHPI_SU_WEEK", SAHPI_SU_WEEK},
   {"SAHPI_SU_MIL", SAHPI_SU_MIL},
   {"SAHPI_SU_INCHES", SAHPI_SU_INCHES},
   {"SAHPI_SU_FEET", SAHPI_SU_FEET},
   {"SAHPI_SU_CU_IN", SAHPI_SU_CU_IN},
   {"SAHPI_SU_CU_FEET", SAHPI_SU_CU_FEET},
   {"SAHPI_SU_MM", SAHPI_SU_MM},
   {"SAHPI_SU_CM", SAHPI_SU_CM},
   {"SAHPI_SU_M", SAHPI_SU_M},
   {"SAHPI_SU_CU_CM", SAHPI_SU_CU_CM},
   {"SAHPI_SU_CU_M", SAHPI_SU_CU_M},
   {"SAHPI_SU_LITERS", SAHPI_SU_LITERS},
   {"SAHPI_SU_FLUID_OUNCE", SAHPI_SU_FLUID_OUNCE},
   {"SAHPI_SU_RADIANS", SAHPI_SU_RADIANS},
   {"SAHPI_SU_STERADIANS", SAHPI_SU_STERADIANS},
   {"SAHPI_SU_REVOLUTIONS", SAHPI_SU_REVOLUTIONS},
   {"SAHPI_SU_CYCLES", SAHPI_SU_CYCLES},
   {"SAHPI_SU_GRAVITIES", SAHPI_SU_GRAVITIES},
   {"SAHPI_SU_OUNCE", SAHPI_SU_OUNCE},
   {"SAHPI_SU_POUND", SAHPI_SU_POUND},
   {"SAHPI_SU_FT_LB", SAHPI_SU_FT_LB},
   {"SAHPI_SU_OZ_IN", SAHPI_SU_OZ_IN},
   {"SAHPI_SU_GAUSS", SAHPI_SU_GAUSS},
   {"SAHPI_SU_GILBERTS", SAHPI_SU_GILBERTS},
   {"SAHPI_SU_HENRY", SAHPI_SU_HENRY},
   {"SAHPI_SU_MILLIHENRY", SAHPI_SU_MILLIHENRY},
   {"SAHPI_SU_FARAD", SAHPI_SU_FARAD},
   {"SAHPI_SU_MICROFARAD", SAHPI_SU_MICROFARAD},
   {"SAHPI_SU_OHMS", SAHPI_SU_OHMS},
   {"SAHPI_SU_SIEMENS", SAHPI_SU_SIEMENS},
   {"SAHPI_SU_MOLE", SAHPI_SU_MOLE},
   {"SAHPI_SU_BECQUEREL", SAHPI_SU_BECQUEREL},
   {"SAHPI_SU_PPM", SAHPI_SU_PPM},
   {"SAHPI_SU_RESERVED", SAHPI_SU_RESERVED},
   {"SAHPI_SU_DECIBELS", SAHPI_SU_DECIBELS},
   {"SAHPI_SU_DBA", SAHPI_SU_DBA},
   {"SAHPI_SU_DBC", SAHPI_SU_DBC},
   {"SAHPI_SU_GRAY", SAHPI_SU_GRAY},
   {"SAHPI_SU_SIEVERT", SAHPI_SU_SIEVERT},
   {"SAHPI_SU_COLOR_TEMP_DEG_K", SAHPI_SU_COLOR_TEMP_DEG_K},
   {"SAHPI_SU_BIT", SAHPI_SU_BIT},
   {"SAHPI_SU_KILOBIT", SAHPI_SU_KILOBIT},
   {"SAHPI_SU_MEGABIT", SAHPI_SU_MEGABIT},
   {"SAHPI_SU_GIGABIT", SAHPI_SU_GIGABIT},
   {"SAHPI_SU_BYTE", SAHPI_SU_BYTE},
   {"SAHPI_SU_KILOBYTE", SAHPI_SU_KILOBYTE},
   {"SAHPI_SU_MEGABYTE", SAHPI_SU_MEGABYTE},
   {"SAHPI_SU_GIGABYTE", SAHPI_SU_GIGABYTE},
   {"SAHPI_SU_WORD", SAHPI_SU_WORD},
   {"SAHPI_SU_DWORD", SAHPI_SU_DWORD},
   {"SAHPI_SU_QWORD", SAHPI_SU_QWORD},
   {"SAHPI_SU_LINE", SAHPI_SU_LINE},
   {"SAHPI_SU_HIT", SAHPI_SU_HIT},
   {"SAHPI_SU_MISS", SAHPI_SU_MISS},
   {"SAHPI_SU_RETRY", SAHPI_SU_RETRY},
   {"SAHPI_SU_RESET", SAHPI_SU_RESET},
   {"SAHPI_SU_OVERRUN", SAHPI_SU_OVERRUN},
   {"SAHPI_SU_UNDERRUN", SAHPI_SU_UNDERRUN},
   {"SAHPI_SU_COLLISION", SAHPI_SU_COLLISION},
   {"SAHPI_SU_PACKETS", SAHPI_SU_PACKETS},
   {"SAHPI_SU_MESSAGES", SAHPI_SU_MESSAGES},
   {"SAHPI_SU_CHARACTERS", SAHPI_SU_CHARACTERS},
   {"SAHPI_SU_ERRORS", SAHPI_SU_ERRORS},
   {"SAHPI_SU_CORRECTABLE_ERRORS", SAHPI_SU_CORRECTABLE_ERRORS},
   {"SAHPI_SU_UNCORRECTABLE_ERRORS", SAHPI_SU_UNCORRECTABLE_ERRORS}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_unit, SaHpiSensorUnitsT, sensor_unit_types)

static struct{
    char *str;
    SaHpiSensorModUnitUseT  type;
} unit_uses[] = {
   {"SAHPI_SMUU_NONE", SAHPI_SMUU_NONE},
   {"SAHPI_SMUU_BASIC_OVER_MODIFIER", SAHPI_SMUU_BASIC_OVER_MODIFIER},
   {"SAHPI_SMUU_BASIC_TIMES_MODIFIER", SAHPI_SMUU_BASIC_TIMES_MODIFIER},
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_mod_unit_use, SaHpiSensorModUnitUseT, unit_uses)

static struct{
    char *str;
    SaHpiSensorLinearizationT type;
} sensor_line_types[] = {
   {"SAHPI_SL_LINEAR", SAHPI_SL_LINEAR},
   {"SAHPI_SL_LN", SAHPI_SL_LN},
   {"SAHPI_SL_LOG10", SAHPI_SL_LOG10},
   {"SAHPI_SL_LOG2", SAHPI_SL_LOG2},
   {"SAHPI_SL_E", SAHPI_SL_E},
   {"SAHPI_SL_EXP10", SAHPI_SL_EXP10},
   {"SAHPI_SL_EXP2", SAHPI_SL_EXP2},
   {"SAHPI_SL_1OVERX", SAHPI_SL_1OVERX},
   {"SAHPI_SL_SQRX", SAHPI_SL_SQRX},
   {"SAHPI_SL_CUBEX", SAHPI_SL_CUBEX},
   {"SAHPI_SL_SQRTX", SAHPI_SL_SQRTX},
   {"SAHPI_SL_CUBERTX", SAHPI_SL_CUBERTX},
   {"SAHPI_SL_NONLINEAR", SAHPI_SL_NONLINEAR},
   {"SAHPI_SL_OEM", SAHPI_SL_OEM},
   {"SAHPI_SL_UNSPECIFIED", SAHPI_SL_UNSPECIFIED},
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_line, SaHpiSensorLinearizationT, 
                      sensor_line_types)

static struct{
    char *str;
    SaHpiSensorInterpretedTypeT type;
} interpreted_types[] = {
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT8",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT8},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT16",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT16},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_UINT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_UINT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT8",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT8},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT16",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT16},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_INT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_INT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32",
    SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32},
   {"SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER",
    SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER}, 
};
STR_TO_SAHPI_TYPE_FUN(str2sensor_interpreted_type, SaHpiSensorInterpretedTypeT, 
                      interpreted_types)

SAHPI_TYPE_TO_STR_FUN(sensor_interpreted_type2str, SaHpiSensorInterpretedTypeT, 
                      interpreted_types)

#if 0
in order to write code easily, the sample is provided. pls ignore it:)
sample:
static struct{
    char *str;
    XXX_TYPE  type;
} xxx_types[] = {
    {"XXX",XXX},
};
STR_TO_SAHPI_TYPE_FUN(xxx_name, XXX_TYPE, xxx_types)
#endif

/* Transition from string to SAHPI type end */

#define NODE_CHECK_RETVAL(x)                     \
do{                                              \
     if (retval) {                               \
         dbg("error in "#x"\n");                 \
         return -1;                              \
     }                                           \
}while(0)

#define NODE_CHECK_NULL(p, x)                    \
do{                                              \
     if (p==NULL) {                              \
         dbg("error in "#x"\n");                 \
         return -1;                              \
     }                                           \
}while(0)



static int node2entitypath(GSList *node, SaHpiEntityPathT *path)
{
    int retval = 0;
    int i, len;

    len = g_slist_length(node)/2;

    if (len > SAHPI_MAX_ENTITY_PATH)
        return -1;

    for (i = 0; g_slist_nth(node, 2*i+1); i++) {
        char *str;

        str = (char*)g_slist_nth_data(node, 2*i);
        NODE_CHECK_NULL(str, "Entity Type");
        
        retval = str2entity_type(str, &path->Entry[i].EntityType);
        NODE_CHECK_RETVAL("Entity Type");

        str = (char*)g_slist_nth_data(node, 2*i+1);
        NODE_CHECK_NULL(str, "Instance");
        retval = str2uint32(str, &path->Entry[i].EntityInstance);
        NODE_CHECK_RETVAL("Instance");
    }
    if (len < SAHPI_MAX_ENTITY_PATH)
        path->Entry[i].EntityType = SAHPI_ENT_ROOT;

    return 0;
}

static int node2uint8_array(GSList *node, SaHpiUint8T *array, int len)
{
     GSList *list;
     int i, retval;
     if (g_slist_length(node) > len)
        return -1;
     for (list=node, i=0; list; list=g_slist_next(list), i++) {
         retval = str2uint8((char*)list->data, &array[i]);
         NODE_CHECK_RETVAL("ARRAY");
     } 
     return 0;
}

static int node2sensor_interpreted(GSList *node,SaHpiSensorInterpretedT *intr)
{
     int retval = 0;
     char *str;
     GSList *tmp;
     
     str = sim_get_string_value(node, "Type");
     NODE_CHECK_NULL(str, "Type");
     retval = str2sensor_interpreted_type(str, &intr->Type);
     NODE_CHECK_RETVAL("Type");

     tmp =  sim_get_seq_value(node, "Value");
     NODE_CHECK_NULL(tmp, "Value");
     switch (intr->Type) {  
        case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
             str = sim_get_string_value(tmp, "SensorUint8");
             NODE_CHECK_NULL(str, "SensorUint8");
             retval = str2uint8(str, &intr->Value.SensorUint8);
             NODE_CHECK_RETVAL("SensorUint8");
             break; 
        case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
             str = sim_get_string_value(tmp, "SensorUint16");
             NODE_CHECK_NULL(str, "SensorUint16");
             retval = str2uint16(str, &intr->Value.SensorUint16);
             NODE_CHECK_RETVAL("SensorUint16");
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
             str = sim_get_string_value(tmp, "SensorUint32");
             NODE_CHECK_NULL(str, "SensorUint32");
             retval = str2uint32(str, &intr->Value.SensorUint32);
             NODE_CHECK_RETVAL("SensorUint32");
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
             str = sim_get_string_value(tmp, "SensorInt8");
             NODE_CHECK_NULL(str, "SensorInt8");
             retval = str2int8(str, &intr->Value.SensorInt8);
             NODE_CHECK_RETVAL("SensorInt8");
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
             str = sim_get_string_value(tmp, "SensorInt16");
             NODE_CHECK_NULL(str, "SensorInt16");
             retval = str2int16(str, &intr->Value.SensorInt16);
             NODE_CHECK_RETVAL("SensorInt16");
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
             str = sim_get_string_value(tmp, "SensorInt32");
             NODE_CHECK_NULL(str, "SensorInt32");
             retval = str2int32(str, &intr->Value.SensorInt32);
             NODE_CHECK_RETVAL("SensorInt32");
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
             str = sim_get_string_value(tmp, "SensorFloat32");
             NODE_CHECK_NULL(str, "SensorFloat32");
             intr->Value.SensorFloat32 = atof(str);
             break;
        case SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
             tmp = sim_get_array_value(tmp, "SensorBuffer");
             NODE_CHECK_NULL(tmp, "SensorBuffer");
             retval = node2uint8_array(tmp, intr->Value.SensorBuffer,
                                SAHPI_SENSOR_BUFFER_LENGTH);
             NODE_CHECK_RETVAL("SensorBuffer");
             break;
     }   
     
     return 0;
}
static int node2sensor_evt_status(GSList *node, SaHpiSensorEvtStatusT *status)
{
       int retval = 0;
       char *str;
   
       str = sim_get_string_value(node, "SensorStatus");
       NODE_CHECK_NULL(str, "SensorStatus");
       retval = str2uint8(str, &status->SensorStatus);
       NODE_CHECK_RETVAL("SensorStatus");

       str = sim_get_string_value(node, "EventStatus");
       NODE_CHECK_NULL(str, "EventStatus");
       retval = str2uint16(str, &status->EventStatus);
       NODE_CHECK_RETVAL("EventStatus");

       return 0;
}
static int node2sensor_reading(GSList *node, SaHpiSensorReadingT *reading)
{     
       int retval = 0;
       char *str;
       GSList *tmp;

       str = sim_get_string_value(node, "ValuesPresent");
       NODE_CHECK_NULL(str, "ValuesPresent");
       retval = str2uint8(str, &reading->ValuesPresent);
       NODE_CHECK_RETVAL("ValuesPresent");  

       str = sim_get_string_value(node, "Raw");
       NODE_CHECK_NULL(str, "Raw");
       retval = str2uint32(str, &reading->Raw);
       NODE_CHECK_RETVAL("Raw");
   
       tmp = sim_get_seq_value(node, "Interpreted");
       NODE_CHECK_NULL(tmp, "Interpreted");
       retval = node2sensor_interpreted(tmp, &reading->Interpreted);
       NODE_CHECK_RETVAL("Interpreted"); 

       tmp = sim_get_seq_value(node, "EventStatus");
       NODE_CHECK_NULL(tmp, "EventStatus");
       retval = node2sensor_evt_status(tmp, &reading->EventStatus);
       NODE_CHECK_RETVAL("EventStatus");

       return 0;
}

static int node2sensor_factor(GSList *node, SaHpiSensorFactorsT *fct)
{
      int retval = 0;
      char *str;

      str = sim_get_string_value(node, "M_Factor");
      NODE_CHECK_NULL(str, "M_Factor");
      retval = str2uint16(str, &fct->M_Factor);
      NODE_CHECK_RETVAL("M_Factor");
 
      str = sim_get_string_value(node, "B_Factor");
      NODE_CHECK_NULL(str, "B_Factor");
      retval = str2uint16(str, &fct->B_Factor);
      NODE_CHECK_RETVAL("B_Factor");

      str = sim_get_string_value(node, "AccuracyFactor");
      NODE_CHECK_NULL(str, "AccuracyFactor");
      retval = str2uint16(str, &fct->AccuracyFactor);
      NODE_CHECK_RETVAL("AccuracyFactor");

      str = sim_get_string_value(node, "ToleranceFactor");
      NODE_CHECK_NULL(str, "ToleranceFactor");
      retval = str2uint8(str, &fct->ToleranceFactor);
      NODE_CHECK_RETVAL("ToleranceFactor");

      str = sim_get_string_value(node, "ExpA");
      NODE_CHECK_NULL(str, "ExpA");
      retval = str2uint8(str, &fct->ExpA);
      NODE_CHECK_RETVAL("ExpA");

      str = sim_get_string_value(node, "ExpR");
      NODE_CHECK_NULL(str, "ExpR");
      retval = str2uint8(str, &fct->ExpR);
      NODE_CHECK_RETVAL("ExpR");

      str = sim_get_string_value(node, "ExpB");
      NODE_CHECK_NULL(str, "ExpB");
      retval = str2uint8(str, &fct->ExpB);
      NODE_CHECK_RETVAL("ExpB");
      
      str = sim_get_string_value(node, "Linearization");
      NODE_CHECK_NULL(str, "Linearization");
      retval = str2sensor_line(str, &fct->Linearization);
      NODE_CHECK_RETVAL("Linearization");
      
      return 0;
}
static int node2sensor_range(GSList *node, SaHpiSensorRangeT *range)
{     
       int retval = 0;
       char *str;
       GSList *tmp;

       str = sim_get_string_value(node, "Flags");
       NODE_CHECK_NULL(str, "Flags");
       retval = str2uint8(str, &range->Flags);
       NODE_CHECK_RETVAL("Flags");

       tmp = sim_get_seq_value(node, "Max");
       NODE_CHECK_NULL(tmp, "Max");
       retval = node2sensor_reading(tmp, &range->Max);
       NODE_CHECK_RETVAL("Max");

       tmp = sim_get_seq_value(node, "Min");
       NODE_CHECK_NULL(tmp, "Min");
       retval = node2sensor_reading(tmp, &range->Min);
       NODE_CHECK_RETVAL("Min");

       tmp = sim_get_seq_value(node, "Nominal");
       NODE_CHECK_NULL(tmp, "Nominal");
       retval = node2sensor_reading(tmp, &range->Nominal);
       NODE_CHECK_RETVAL("Nominal");

       tmp = sim_get_seq_value(node, "NormalMax");
       NODE_CHECK_NULL(tmp, "NormalMax");
       retval = node2sensor_reading(tmp, &range->NormalMax);
       NODE_CHECK_RETVAL("NormalMax");

       tmp = sim_get_seq_value(node, "NormalMin");
       NODE_CHECK_NULL(tmp, "NormalMin");
       retval = node2sensor_reading(tmp, &range->NormalMin);
       NODE_CHECK_RETVAL("NormalMin");

       return 0;
}
static int node2sensor_data_format(GSList *node, SaHpiSensorDataFormatT *fmt)
{
       int retval = 0;
       char *str;
       GSList *tmp;

       str = sim_get_string_value(node, "ReadingFormats");
       NODE_CHECK_NULL(str, "ReadingFormats");
       retval = str2uint8(str, &fmt->ReadingFormats);
       NODE_CHECK_RETVAL("ReadingFormats");
  
       str = sim_get_string_value(node, "IsNumeric");
       NODE_CHECK_NULL(str, "IsNumeric");
       retval = str2uint8(str, &fmt->IsNumeric);
       NODE_CHECK_RETVAL("IsNumeric");

       str = sim_get_string_value(node, "SignFormat");
       NODE_CHECK_NULL(str, "SignFormat");
       retval = str2sign_fmt_type(str, &fmt->SignFormat);
       NODE_CHECK_RETVAL("SignFormat");
       
       str = sim_get_string_value(node, "BaseUnits");
       NODE_CHECK_NULL(str, "BaseUnits");
       retval = str2sensor_unit(str, &fmt->BaseUnits);
       NODE_CHECK_RETVAL("BaseUnits");
       
       str = sim_get_string_value(node, "ModifierUnits");
       NODE_CHECK_NULL(str, "ModifierUnits");
       retval = str2sensor_unit(str, &fmt->ModifierUnits);
       NODE_CHECK_RETVAL("ModifierUnits");

       str = sim_get_string_value(node, "ModifierUse");
       NODE_CHECK_NULL(str, "ModifierUse");
       retval = str2sensor_mod_unit_use(str, &fmt->ModifierUse);
       NODE_CHECK_RETVAL("ModifierUnits");

       str = sim_get_string_value(node, "FactorsStatic");
       NODE_CHECK_NULL(str, "FactorsStatic");
       retval = str2uint8(str, &fmt->FactorsStatic);
       NODE_CHECK_RETVAL("FactorsStatic");

       tmp = sim_get_seq_value(node, "Factors");
       NODE_CHECK_NULL(str, "Factors");
       retval = node2sensor_factor(tmp, &fmt->Factors);
       NODE_CHECK_RETVAL("Factors");

       str = sim_get_string_value(node, "Percentage");
       NODE_CHECK_NULL(str, "Percentage");
       retval = str2uint8(str, &fmt->Percentage); 
       NODE_CHECK_RETVAL("Percentage");

       tmp = sim_get_seq_value(node, "Range");
       NODE_CHECK_NULL(tmp, "Range");
       retval = node2sensor_range(tmp, &fmt->Range);
       NODE_CHECK_RETVAL("Range");

       return 0;
}
static int node2sensor_thrdefn(GSList *node, SaHpiSensorThdDefnT *def)
{
      int retval = 0;
      char *str;

      str = sim_get_string_value(node, "IsThreshold");
      NODE_CHECK_NULL(str, "IsThreshold");
      retval = str2uint8(str, &def->IsThreshold);
      NODE_CHECK_RETVAL("IsThreshold");

      str = sim_get_string_value(node, "TholdCapabilities");
      NODE_CHECK_NULL(str, "TholdCapabilities");
      retval = str2uint8(str, &def->TholdCapabilities);
      NODE_CHECK_RETVAL("TholdCapabilities");
            
      str = sim_get_string_value(node, "ReadThold");
      NODE_CHECK_NULL(str, "ReadThold");
      retval = str2uint8(str, &def->ReadThold);
      NODE_CHECK_RETVAL("ReadThold");
   
      str = sim_get_string_value(node, "WriteThold");
      NODE_CHECK_NULL(str, "WriteThold");
      retval = str2uint8(str, &def->WriteThold);
      NODE_CHECK_RETVAL("WriteThold");

      str = sim_get_string_value(node, "FixedThold");
      NODE_CHECK_NULL(str, "FixedThold");
      retval = str2uint8(str, &def->FixedThold);
      NODE_CHECK_RETVAL("FixedThold");

      return 0;
}

static int node2ctrl_rdr(GSList *node, SaHpiCtrlRecT *rec)
{
     dbg("not implemented\n");
     return -1;
}
static int node2sensor_rdr(GSList *node, SaHpiSensorRecT *rec)
{    
     int retval = 0;
     char *str;
     GSList *tmp;

     str = sim_get_string_value(node, "Num");
     NODE_CHECK_NULL(str, "Num");
     retval = str2uint8(str, &rec->Num);
     NODE_CHECK_RETVAL("Num");     

     str = sim_get_string_value(node, "Type");
     NODE_CHECK_NULL(str, "Type");
     retval = str2sensor_type(str, &rec->Type);
     NODE_CHECK_RETVAL("Type");

     str = sim_get_string_value(node, "Category");
     NODE_CHECK_NULL(str, "Category");
     retval = str2uint8(str, &rec->Category);
     NODE_CHECK_RETVAL("Category");

     str = sim_get_string_value(node, "EventCtrl");
     NODE_CHECK_NULL(str, "EventCtrl");
     retval = str2event_ctrl_type(str, &rec->EventCtrl);
     NODE_CHECK_RETVAL("EventCtrl");

     str = sim_get_string_value(node, "Events");
     NODE_CHECK_NULL(str, "Events");
     retval = str2uint16(str, &rec->Events);
     NODE_CHECK_RETVAL("Events");

     str = sim_get_string_value(node, "Ignore");
     NODE_CHECK_NULL(str, "Ignore");
     retval = str2uint8(str, &rec->Ignore);
     NODE_CHECK_RETVAL("Ignore");
 
     tmp = sim_get_seq_value(node, "DataFormat");
     NODE_CHECK_NULL(tmp, "DataFormat");
     retval = node2sensor_data_format(tmp, &rec->DataFormat);
     NODE_CHECK_RETVAL("DataFormat");
     
     tmp = sim_get_seq_value(node, "ThresholdDefn");
     NODE_CHECK_NULL(tmp, "ThresholdDefn");
     retval = node2sensor_thrdefn(tmp, &rec->ThresholdDefn);
     NODE_CHECK_RETVAL("ThresholdDefn");

     str = sim_get_string_value(node, "Oem");
     NODE_CHECK_NULL(str, "Oem");
     retval = str2uint32(str, &rec->Oem);
     NODE_CHECK_RETVAL("Oem");


     return 0;
}
static int node2inventory_rdr(GSList *node, SaHpiInventoryRecT *rec)
{
     dbg("not implemented\n");
     return -1;
}
static int node2watchdog_rdr(GSList *node, SaHpiWatchdogRecT *rec)
{
    dbg("not implemented\n");
    return -1;
}

static fhs_node* get_string_fhs_node(char *name, char *value)
{ 
    fhs_node *data;
    
    data = g_malloc0(sizeof(*data));
    if (data == NULL) {
        dbg("Couldn't allocate memory for data\n");
        goto out1;
    }

    data->name = g_malloc0(strlen(name)+1);
    if (data->name == NULL) {
        dbg("Couldn't allocate memory for data->name\n");
        goto out2;
    }
    strcpy(data->name, name);

    data->type = SIM_FHS_STRING;

    data->value.str = g_malloc0(strlen(value)+1);
    if (data->value.str == NULL) {
        dbg("Couldn't allocate memory for data->value\n");
        goto out3;
    }
    strcpy(data->value.str, value);

    return data;

out3:
    g_free(data->name);
out2:
    g_free(data);
out1:
    return NULL;
}

static fhs_node* get_seq_fhs_node(char *name, GSList *value)
{
    fhs_node *data;

    data = g_malloc0(sizeof(*data));
    if (data == NULL) {
        dbg("Couldn't allocate memory for data\n");
        goto out1;
    }

    data->name = g_malloc0(strlen(name)+1);
    if (data->name == NULL) {
        dbg("Couldn't allocate memory for data->name\n");
        goto out2;
    }
    strcpy(data->name, name);

    data->type = SIM_FHS_SEQ;

    data->value.seq = value;

    return data;

out2:
    g_free(data);
out1:
    return NULL;

}

static fhs_node* get_array_fhs_node(char* name, GSList *value)
{
    fhs_node *data;

    data = g_malloc0(sizeof(*data));
    if (data == NULL) {
        dbg("Couldn't allocate memory for data\n");
        goto out1;
    }

    data->name = g_malloc0(strlen(name)+1);
    if (data->name == NULL) {
        dbg("Couldn't allocate memory for data->name\n");
        goto out2;
    }
    strcpy(data->name, name);

    data->type = SIM_FHS_ARRAY;

    data->value.array = value;

    return data;

out2:
    g_free(data);
out1:
    return NULL;

}

static GSList* array2node(const SaHpiUint8T *array, int len)
{

   GSList *list = NULL;
   char *str;
   int i;
 
   for (i = 0; i< len; i++) {
       str = g_malloc0(5);
       sprintf(str, "%x", array[i]);
       list = g_slist_append(list, (gpointer)str);
   }
   return list;
}


static GSList* sensor_interpreted2node(const SaHpiSensorInterpretedT *inter)
{

    GSList* list = NULL;
    fhs_node *node;
    char str[100];
    
    sensor_interpreted_type2str(str, 100, inter->Type);
    node = get_string_fhs_node("Type", str);
    list = g_slist_append(list, (gpointer)node);

    switch(inter->Type) {
       case  SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
             sprintf(str, "%x", inter->Value.SensorUint8);          
             node = get_string_fhs_node("SensorUint8", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
             sprintf(str, "%x", inter->Value.SensorUint16);
             node = get_string_fhs_node("SensorUint16", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
             sprintf(str, "%x", inter->Value.SensorUint32);
             node = get_string_fhs_node("SensorUint32", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
             sprintf(str, "%x", inter->Value.SensorInt8);
             node = get_string_fhs_node("SensorInt8", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
             sprintf(str, "%x", inter->Value.SensorInt16);
             node = get_string_fhs_node("SensorInt16", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
             sprintf(str, "%x", inter->Value.SensorInt32);
             node = get_string_fhs_node("SensorInt32", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
             sprintf(str, "%f", inter->Value.SensorFloat32);
             node = get_string_fhs_node("SensorFloat32", str);
             break;
       case  SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER:
             {
                GSList *tmp;
                tmp = array2node(inter->Value.SensorBuffer, SAHPI_SENSOR_BUFFER_LENGTH);
                node = get_array_fhs_node("SensorBuffer", tmp);
             }
             break;
    }
        
    node = get_seq_fhs_node("Value", g_slist_append(NULL, (gpointer)node));
    list = g_slist_append(list, (gpointer)node);
    
    return list;
}

static GSList* sensor_evt_status2node(const SaHpiSensorEvtStatusT *status)
{   
     GSList* list = NULL;
     fhs_node *data;
     char   str[10];


     sprintf(str, "%x", status->SensorStatus);
     data = get_string_fhs_node("SensorStatus", str);
     list = g_slist_append(list, (gpointer)data);

     sprintf(str, "%x", status->EventStatus);
     data = get_string_fhs_node("EventStatus", str);
     list = g_slist_append(list, (gpointer)data);
     
     return list;
}
static GSList* sensor_reading2node(const SaHpiSensorReadingT *reading)
{
    GSList* list = NULL;
    GSList* tmp;
    fhs_node *data;
    
    char str[100];    

    sprintf(str, "%x", reading->ValuesPresent);
    data = get_string_fhs_node("ValuesPresent", str);
    NULL_CHECK(data);
    list = g_slist_append(list, data); 

    sprintf(str, "%x", reading->Raw);
    data = get_string_fhs_node("Raw", str);
    NULL_CHECK(data);
    list = g_slist_append(list, data);     
 
    tmp = sensor_interpreted2node(&reading->Interpreted);
    data = get_seq_fhs_node("Interpreted", tmp);
    list = g_slist_append(list, data);    

    tmp = sensor_evt_status2node(&reading->EventStatus);
    data = get_seq_fhs_node("EventStatus", tmp);
    list = g_slist_append(list, data);

    return list;

error_process:
    free_fhs_node(list);
    return NULL;
} 


static GSList* sensor_enable2node(const SaHpiSensorEvtEnablesT *enable)
{
    GSList* list = NULL;
    fhs_node *data;    
    char   str[10];
   
    sprintf(str, "%x", enable->SensorStatus);
    data = get_string_fhs_node("SensorStatus", str);
    list = g_slist_append(list, (gpointer)data);

    sprintf(str, "%x", enable->AssertEvents);
    data = get_string_fhs_node("AssertEvents", str);
    list = g_slist_append(list, (gpointer)data);

    sprintf(str, "%x", enable->DeassertEvents);
    data = get_string_fhs_node("DeassertEvents", str);
    list = g_slist_append(list, (gpointer)data);

    return list;
}

#define FILE_CHECK_RETVAL(x)                                 \
do{                                                          \
    if (retval) {                                            \
        dbg("error in retrieving %s from file:%s\n",         \
                     x, filename);                           \
        goto out;                                            \
    }                                                        \
}while(0)


#define FILE_CHECK_NULL(p, x)                                \
do{                                                          \
    retval = -1;                                             \
    if (p == NULL) {                                         \
        dbg("%s is null\n", x);                              \
        goto out;                                            \
    }                                                        \
}while(0)

int sim_parser_get_rpt(char *filename, SaHpiRptEntryT *rpt)
{

    GSList *fhs;
    char   *str;
    GSList *node;
    int    retval = 0;

    memset(rpt, sizeof(*rpt), 0);
    fhs = sim_parser(filename);
    if (fhs == NULL) return -1;
     
    node = sim_get_array_value(fhs, "ResourceEntity");
    FILE_CHECK_NULL(node, "ResourceEntity");
    retval = node2entitypath(node, &rpt->ResourceEntity);
    FILE_CHECK_RETVAL("ResourceEntity");

    str = sim_get_string_value(fhs, "ResourceCapabilities");
    FILE_CHECK_NULL(str, "ResourceCapabilities");
    retval = str2uint32(str, &rpt->ResourceCapabilities);
    FILE_CHECK_RETVAL("ResourceCapabilities");

    str = sim_get_string_value(fhs, "ResourceSeverity");
    FILE_CHECK_NULL(str, "ResourceSeverity");
    retval = str2severity(str, &rpt->ResourceSeverity);
    FILE_CHECK_RETVAL("ResourceSeverity");

    str = sim_get_string_value(fhs, "ResourceTag");
    FILE_CHECK_NULL(str, "ResourceTag");
    retval = str2text(str, &rpt->ResourceTag);
    FILE_CHECK_RETVAL("ResourceTag");

    node = sim_get_seq_value(fhs, "ResourceInfo");
    FILE_CHECK_NULL(node, "ResourceInfo");

    str = sim_get_string_value(node, "ResourceRev");
    FILE_CHECK_NULL(str, "ResourceRev");
    retval = str2uint8(str, &rpt->ResourceInfo.ResourceRev);
    FILE_CHECK_RETVAL("ResourceRev");

    str = sim_get_string_value(node, "SpecificVer");
    FILE_CHECK_NULL(str, "SpecificVer");
    retval = str2uint8(str, &rpt->ResourceInfo.SpecificVer);
    FILE_CHECK_RETVAL("SpecificVer");    
   
    str = sim_get_string_value(node, "DeviceSupport");
    FILE_CHECK_NULL(str, "DeviceSupport");
    retval = str2uint8(str, &rpt->ResourceInfo.DeviceSupport);
    FILE_CHECK_RETVAL("DeviceSupport");
     
    str = sim_get_string_value(node, "ManufacturerId");
    FILE_CHECK_NULL(str, "ManufacturerId");
    retval = str2uint32(str, &rpt->ResourceInfo.ManufacturerId);    
    FILE_CHECK_RETVAL("ManufacturerId");
    
    str = sim_get_string_value(node, "ProductId");
    FILE_CHECK_NULL(str, "ProductId");
    retval = str2uint16(str, &rpt->ResourceInfo.ProductId);
    FILE_CHECK_RETVAL("ProductId");
  
    str = sim_get_string_value(node, "FirmwareMajorRev");
    FILE_CHECK_NULL(str, "FirmwareMajorRev");
    retval = str2uint8(str, &rpt->ResourceInfo.FirmwareMajorRev);
    FILE_CHECK_RETVAL("FirmwareMajorRev");

    str = sim_get_string_value(node, "FirmwareMinorRev");
    FILE_CHECK_NULL(str, "FirmwareMinorRev");
    retval = str2uint8(str, &rpt->ResourceInfo.FirmwareMinorRev);
    FILE_CHECK_RETVAL("FirmwareMinorRev");

    str = sim_get_string_value(node, "AuxFirmwareRev");
    FILE_CHECK_NULL(str, "AuxFirmwareRev");
    retval = str2uint8(str, &rpt->ResourceInfo.AuxFirmwareRev);
    FILE_CHECK_RETVAL("AuxFirmwareRev");
out:
    free_fhs_node(fhs);
    return retval;
}

int sim_parser_rpt_set_tag(char *filename, SaHpiTextBufferT *tag)
{
    GSList *fhs;
    GSList *list;
    SaHpiUint8T  data[SAHPI_MAX_TEXT_BUFFER_LENGTH+1];

    fhs = sim_parser(filename);
    if (fhs == NULL)
        return -1;
    memcpy(data, tag->Data, SAHPI_MAX_TEXT_BUFFER_LENGTH);
    data[tag->DataLength] = 0;

    for (list = fhs; list; list = g_slist_next(list)) {
       fhs_node *n;
       n = (fhs_node*) list->data;

       if (!strcmp("ResourceTag", n->name)) {
          if (n->type == SIM_FHS_STRING) {
              g_free(n->value.str);
              n->value.str = g_strdup(data);
              break;
          }
       }
    }
    sim_generate(filename, fhs);
    free_fhs_node(fhs);
    return 0;
}




int sim_parser_get_rdr(char *filename, SaHpiRdrT *rdr)
{
    GSList *fhs;
    GSList *node;
    char   *str;
    int    retval = 0;

    memset(rdr, sizeof(*rdr), 0);

    fhs = sim_parser(filename);
    if (fhs == NULL) return -1;

    str = sim_get_string_value(fhs, "RdrType");
    FILE_CHECK_NULL(str, "RdrType");
    retval = str2rdr_type(str, &rdr->RdrType);
    FILE_CHECK_RETVAL("RdrType");

    if (rdr->RdrType == SAHPI_NO_RECORD)
        return 0;  

    node = sim_get_array_value(fhs, "Entity");
    FILE_CHECK_NULL(node, "Entity");
    retval = node2entitypath(node, &rdr->Entity);
    FILE_CHECK_RETVAL("Entity");
     
    node = sim_get_seq_value(fhs, "RdrTypeUnion");
    FILE_CHECK_NULL(node, "RdrTypeUnion");

    switch (rdr->RdrType) {
       case SAHPI_CTRL_RDR:
            node = sim_get_seq_value(node, "CtrlRec");
            FILE_CHECK_NULL(node, "CtrlRec");
            retval = node2ctrl_rdr(node, &rdr->RdrTypeUnion.CtrlRec);
            FILE_CHECK_RETVAL("CtrlRec");
            break;
       case SAHPI_SENSOR_RDR:
            node = sim_get_seq_value(node, "SensorRec");
            FILE_CHECK_NULL(node, "SensorRec");
            retval = node2sensor_rdr(node, &rdr->RdrTypeUnion.SensorRec);
            FILE_CHECK_RETVAL("SensorRec");
            break;
       case SAHPI_INVENTORY_RDR:
            node = sim_get_seq_value(node, "InventoryRec");
            FILE_CHECK_NULL(node, "InventoryRec");
            retval = node2inventory_rdr(node, &rdr->RdrTypeUnion.InventoryRec);
            FILE_CHECK_RETVAL("SensorRec");
            break;
       case SAHPI_WATCHDOG_RDR:
            node = sim_get_seq_value(node, "WatchdogRec");
            FILE_CHECK_NULL(node, "WatchdogRec");
            retval = node2watchdog_rdr(node, &rdr->RdrTypeUnion.WatchdogRec);
            FILE_CHECK_RETVAL("WatchdogRec");
            break;
       default:
            dbg("error");
            goto out;
    }  

    str = sim_get_string_value(fhs, "IdString");
    FILE_CHECK_NULL(str, "IdString");
    retval = str2text(str, &rdr->IdString);
    FILE_CHECK_RETVAL("IdString");
out:
    free_fhs_node(fhs);
    return retval;
}


int sim_parser_get_sensor_enables(char *filename, SaHpiSensorEvtEnablesT *enables)
{
    GSList *fhs;
    char   *str;
    int    retval = 0;

    memset(enables, sizeof(*enables), 0);

    fhs = sim_parser(filename);
    if (fhs == NULL) return -1;
 
    str = sim_get_string_value(fhs, "SensorStatus");
    FILE_CHECK_NULL(str, "SensorStatus");
    retval = str2uint8(str, &enables->SensorStatus);
    FILE_CHECK_RETVAL("SensorStatus");

    str = sim_get_string_value(fhs, "AssertEvents");
    FILE_CHECK_NULL(str, "AssertEvents");
    retval = str2uint16(str, &enables->AssertEvents);
    FILE_CHECK_RETVAL("AssertEvents");

    str = sim_get_string_value(fhs, "DeassertEvents");
    FILE_CHECK_NULL(str, "DeassertEvents");
    retval = str2uint16(str, &enables->DeassertEvents);
    FILE_CHECK_RETVAL("DeassertEvents");

out:
    free_fhs_node(fhs);
    return retval;
}
int sim_parser_set_sensor_enables(char *filename, const SaHpiSensorEvtEnablesT *enables)
{
    GSList *fhs = NULL;
    fhs = sensor_enable2node(enables);
    sim_generate(filename, fhs);
    return 0;
}
int sim_parser_get_sensor_reading(char *filename, SaHpiSensorReadingT *reading)
{
    GSList *fhs;
    int    retval = 0;

    memset(reading, sizeof(*reading), 0);
    
    fhs = sim_parser(filename);
    if (fhs == NULL) return -1;

    retval = node2sensor_reading(fhs, reading);
    free_fhs_node(fhs);
    return retval;
}
int sim_parser_set_sensor_reading(char *filename, const SaHpiSensorReadingT *reading)
{
    GSList *fhs = NULL;

    fhs = sensor_reading2node(reading);
    sim_generate(filename, fhs);
    return 0;
}

int sim_parser_get_sensor_thres(char *filename, SaHpiSensorThresholdsT *thres)
{ 
    GSList *fhs;
    GSList *node;
    int    retval = 0;
  
    fhs = sim_parser(filename);
    if (fhs == NULL) return -1;

    memset(thres, sizeof(*thres), 0);
   
    node = sim_get_seq_value(fhs, "LowCritical");
    FILE_CHECK_NULL(node, "LowCritical");        
    retval = node2sensor_reading(node, &thres->LowCritical);
    FILE_CHECK_RETVAL("LowCritical");

    node = sim_get_seq_value(fhs, "LowMajor");
    FILE_CHECK_NULL(node, "LowMajor");
    retval = node2sensor_reading(node, &thres->LowMajor);
    FILE_CHECK_RETVAL("LowMajor");

    node = sim_get_seq_value(fhs, "LowMinor");
    FILE_CHECK_NULL(node, "LowMinor");
    retval = node2sensor_reading(node, &thres->LowMinor);
    FILE_CHECK_RETVAL("LowMinor");

    node = sim_get_seq_value(fhs, "UpCritical");
    FILE_CHECK_NULL(node, "UpCritical");
    retval = node2sensor_reading(node, &thres->UpCritical);
    FILE_CHECK_RETVAL("UpCritical");

    node = sim_get_seq_value(fhs, "UpMajor");
    FILE_CHECK_NULL(node, "UpMajor");
    retval = node2sensor_reading(node, &thres->UpMajor);
    FILE_CHECK_RETVAL("UpMajor");

    node = sim_get_seq_value(fhs, "UpMinor");
    FILE_CHECK_NULL(node, "UpMinor");
    retval = node2sensor_reading(node, &thres->UpMinor);
    FILE_CHECK_RETVAL("UpMinor");

    node = sim_get_seq_value(fhs, "PosThdHysteresis");
    FILE_CHECK_NULL(node, "PosThdHysteresis");
    retval = node2sensor_reading(node, &thres->PosThdHysteresis);
    FILE_CHECK_RETVAL("PosThdHysteresis");

    node = sim_get_seq_value(fhs, "NegThdHysteresis");
    FILE_CHECK_NULL(node, "NegThdHysteresis");
    retval = node2sensor_reading(node, &thres->NegThdHysteresis);
    FILE_CHECK_RETVAL("NegThdHysteresis");

out:
    free_fhs_node(fhs);
    return retval;
}
int sim_parser_set_sensor_thres(char *filename, const SaHpiSensorThresholdsT *thres)
{
    
    GSList *fhs = NULL;
    GSList *node;
    fhs_node *tmp;
   
    node = sensor_reading2node(&thres->LowCritical);
    tmp = get_seq_fhs_node("LowCritical", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->LowMajor);
    tmp = get_seq_fhs_node("LowMajor", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->LowMinor);
    tmp = get_seq_fhs_node("LowMinor", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->UpCritical);
    tmp = get_seq_fhs_node("UpCritical", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->UpMajor);
    tmp = get_seq_fhs_node("UpMajor", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->UpMinor);
    tmp = get_seq_fhs_node("UpMinor", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);
    
    node = sensor_reading2node(&thres->PosThdHysteresis);
    tmp = get_seq_fhs_node("PosThdHysteresis", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    node = sensor_reading2node(&thres->NegThdHysteresis);
    tmp = get_seq_fhs_node("NegThdHysteresis", node);
    fhs = g_slist_append(fhs, (gpointer)tmp);

    sim_generate(filename, fhs);
    return 0;
}

/* Semantic Part End */


/* Unit Test Begin */
#ifdef UNIT_TEST

static void text_print(SaHpiTextBufferT *buf)
{
     int i;
     for (i = 0; i < buf->DataLength; i++)
        printf("%c", buf->Data[i]);
}

static void entitypath_print(SaHpiEntityPathT *path)
{
     int i;
     char str[100];
 
     printf("{");
     for (i = 0; i < SAHPI_MAX_ENTITY_PATH; i++) {
         if (path->Entry[i].EntityType == SAHPI_ENT_ROOT)
              break;
         entity_type2str(str, 100, path->Entry[i].EntityType);
         printf("(%s,%x)", str, path->Entry[i].EntityInstance);
     }
     printf("}");
}
static void rpt_print(SaHpiRptEntryT  *rpt)
{
     char str[100];

     printf("---SaHpiRptEntryT---\n");
     printf("ResourceCapabilities:%x\n", rpt->ResourceCapabilities);

     severity2str(str, 100, rpt->ResourceSeverity); 
     printf("ResourceSeverity:%s\n", str);

     printf("ResourceTag:"); 
     text_print(&rpt->ResourceTag);
     printf("\n");
     
     printf("ResourceEntity:");
     entitypath_print(&rpt->ResourceEntity);
     printf("\n");

     printf("ResourceInfo.ResourceRev:%x\n", rpt->ResourceInfo.ResourceRev);
     printf("ResourceInfo.SpecificVer:%x\n", rpt->ResourceInfo.SpecificVer);
     printf("ResourceInfo.DeviceSupport:%x\n", rpt->ResourceInfo.DeviceSupport);
     printf("ResourceInfo.ManufacturerId:%x\n", rpt->ResourceInfo.ManufacturerId);
     printf("ResourceInfo.ProductId:%x\n", rpt->ResourceInfo.ProductId);
     printf("ResourceInfo.FirmwareMajorRev:%x\n", rpt->ResourceInfo.FirmwareMajorRev);
     printf("ResourceInfo.FirmwareMinorRev:%x\n", rpt->ResourceInfo.FirmwareMinorRev);
     printf("ResourceInfo.AuxFirmwareRev:%x\n", rpt->ResourceInfo.AuxFirmwareRev);
     printf("\n\n");
}
static void rdr_print(SaHpiRdrT  *rdr)
{
     printf("---SaHpiRdrT---\n");
     printf("RdrType:%x\n", rdr->RdrType);

     printf("Entity:");
     entitypath_print(&rdr->Entity);
     printf("\n");

     printf("IdString:");
     text_print(&rdr->IdString);
     printf("\n");

     switch (rdr->RdrType) {
         case SAHPI_NO_RECORD:
              printf("no record\n");
              break;
         case SAHPI_CTRL_RDR:
              printf("not implemented\n");
              break;
         case SAHPI_SENSOR_RDR:
              {
                  SaHpiSensorRecT *rec;
                  
                  rec = &rdr->RdrTypeUnion.SensorRec;
                  printf("SensorRec.Num:%x\n", rec->Num);
                  printf("SensorRec.Type:%x\n", rec->Type);
                  printf("SensorRec.Category:%x\n", rec->Category);
                  printf("SensorRec.Ignore:%x\n", rec->Ignore);
                  printf("SensorRec.Oem:%x\n", rec->Oem);
              }
              break;
         case SAHPI_INVENTORY_RDR:
              printf("not implemented\n");
              break;
         case SAHPI_WATCHDOG_RDR:
              printf("not implemented\n");
              break;
     }
     printf("\n\n");
}

static void reading_print(SaHpiSensorReadingT *reading)
{
     printf("---SaHpiSensorReadingT---\n");
     printf("\n\n");
}

static void threshold_print(SaHpiSensorThresholdsT *thres)
{
     printf("---SaHpiSensorThresholdsT---\n");
     printf("\n\n");
}
static void enable_print(SaHpiSensorEvtEnablesT *enable)
{
     printf("---SaHpiSensorEvtEnablesT---\n");
     printf("SensorStatus:%x\n", enable->SensorStatus);
     printf("AssertEvents:%x\n", enable->AssertEvents);
     printf("DeassertEvents:%x\n", enable->DeassertEvents);
     printf("\n\n");
}

int main()
{
    GSList *fhs;
    SaHpiRptEntryT  rpt;
    SaHpiRdrT  rdr;
    SaHpiSensorReadingT reading;
    SaHpiSensorThresholdsT thres;
    SaHpiSensorEvtEnablesT enable;
    int retval = 0;

    fhs = sim_parser("test/rpt"); 
    sim_generate("test/rpt.out", fhs);
    free_fhs_node(fhs);

    fhs = sim_parser("test/rdr");
    sim_generate("test/rdr.out", fhs);
    free_fhs_node(fhs);

    fhs = sim_parser("test/reading");
    sim_generate("test/reading.out", fhs);
    free_fhs_node(fhs);
    
    retval = sim_parser_get_rpt("test/rpt", &rpt);
    if (!retval) 
        rpt_print(&rpt);
    else
        printf("some error in parsing rpt\n");

    retval = sim_parser_get_rdr("test/rdr", &rdr);
    if (!retval)
        rdr_print(&rdr);
    else 
       printf("some error in parsing rdr\n");

    retval = sim_parser_get_sensor_reading("test/reading", &reading);
    if (!retval)
        reading_print(&reading);
    else
       printf("some error in parsing reading\n");
    


    retval = sim_parser_get_sensor_thres("test/thres", &thres);
    if (!retval)
        threshold_print(&thres);
    else
       printf("some error in parsing thres\n");

     retval = sim_parser_set_sensor_thres("test/thres.out", &thres);
    if (!retval)
        threshold_print(&thres);
    else
       printf("some error in generating thres\n");


    retval = sim_parser_get_sensor_enables("test/enable", &enable);
    if (!retval)
        enable_print(&enable);
    else
       printf("some error in parsing enable\n");

    retval = sim_parser_set_sensor_enables("test/enable.out", &enable);
    if (!retval)
        enable_print(&enable);
    else
       printf("some error in generating enable\n");

    return 0;
}

/*
 *   How to do module test
 *   gcc -DUNIT_TEST -I/usr/include/glib-1.2/ -I/usr/lib/glib/include/ 
 *   -I../../../ -I../../../include -Wall -lglib  sim_parser.c
 */

#endif
/* Unit Test End */
