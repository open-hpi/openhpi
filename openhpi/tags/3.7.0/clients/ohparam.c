/*      -*- linux-c -*-
 *
 * Copyright (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Log: 
 *     Start from hpidomain.c 
 *     This routine can display and manipulate parameters of the
 *     openHPI daemon as specified in openhpi.conf file, using
 *     the OpenHPI extensions as described in ohpi.c 
 *
 * Changes:
 *     03/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision: 7142 $"

#define OHPARAM_HELP \
    "Command get: display info about all global parameters  \n" \
    "         no specific arguments                         \n" \
    "Command set:                                           \n" \
    "         one of the daemon's global parameters:        \n" \
    "         (without the OPENHPI prefix)                  \n" \
    "           LOG_ON_SEV, EVT_QUEUE_LIMIT,                \n" \
    "           DEL_SIZE_LIMIT, DEL_SAVE                    \n" \
    "           DAT_SIZE_LIMIT, DAT_USER_LIMIT, DAT_SAVE    \n" \
    "           PATH, VARPATH, CONF                         \n" \
    "         and the desired new value. Example:           \n" \
    "           ohparam set DEL_SIZE_LIMIT 20000            " 


/* 
 * Function prototypes
 */
static SaErrorT execglobalparamget (void); 
static SaErrorT execglobalparamset (oHpiGlobalParamTypeT,char *); 


/* 
 * Globals 
 */
static oHpiCommonOptionsT copt;
SaHpiSessionIdT sessionid;


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
   SaErrorT    rv = SA_OK;
   oHpiGlobalParamTypeT paramtype = OHPI_CONF;
   char setparam[OH_PATH_PARAM_MAX_LENGTH];
   SaHpiBoolT printusage = FALSE;
   int i=1;
   GOptionContext *context;

   enum cmdT {eUndefined,
      eGlobalParamGet,
      eGlobalParamSet} cmd=eUndefined;
       
   /* Print version strings */
   oh_prog_version(argv[0]);

   /* Parsing options */
   static char usetext[]="command [specific arguments] - "
                         "Control openhpi configuration parameters.\n  "
                         OH_SVN_REV "\n\n" OHPARAM_HELP ;

   OHC_PREPARE_REVISION(usetext);
   context = g_option_context_new (usetext);

   if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION  // not applicable
                    - OHC_VERBOSE_OPTION )) { // no verbose mode
       printusage = TRUE;
   }
   g_option_context_free (context);

   /* Parse ohparam specific commands */
   while (i<argc && !printusage) {
      if (strcmp(argv[i],"get")==0) {
         cmd=eGlobalParamGet;
      }
      else if (strcmp(argv[i],"set")==0) {
         cmd=eGlobalParamSet;
         if (++i<argc) {
            if (strcmp(argv[i],"OPENHPI_LOG_ON_SEV")==0) 
               paramtype=OHPI_LOG_ON_SEV;
            else if (strcmp(argv[i],"OPENHPI_EVT_QUEUE_LIMIT")==0) 
               paramtype=OHPI_EVT_QUEUE_LIMIT;
            else if (strcmp(argv[i],"OPENHPI_DEL_SIZE_LIMIT")==0) 
               paramtype=OHPI_DEL_SIZE_LIMIT;
            else if (strcmp(argv[i],"OPENHPI_DEL_SAVE")==0) 
               paramtype=OHPI_DEL_SAVE;
            else if (strcmp(argv[i],"OPENHPI_DAT_SIZE_LIMIT")==0) 
               paramtype=OHPI_DAT_SIZE_LIMIT;
            else if (strcmp(argv[i],"OPENHPI_DAT_USER_LIMIT")==0) 
               paramtype=OHPI_DAT_USER_LIMIT;
            else if (strcmp(argv[i],"OPENHPI_DAT_SAVE")==0) 
               paramtype=OHPI_DAT_SAVE;
            else if (strcmp(argv[i],"OPENHPI_PATH")==0) 
               paramtype=OHPI_PATH;
            else if (strcmp(argv[i],"OPENHPI_VARPATH")==0) 
               paramtype=OHPI_VARPATH;
            else if (strcmp(argv[i],"OPENHPI_CONF")==0) 
               paramtype=OHPI_CONF;
            else  printusage = TRUE;
         }
         else printusage = TRUE;

         if (++i<argc)    strcpy(setparam, argv[i]);
         
         else  if (i>=argc) printusage = TRUE;
      }
      
      else printusage = TRUE;
      i++;
   }

   if (printusage == TRUE || cmd == eUndefined)
   {
      printf("\n");
      printf("Usage: %s [Option...] command [specific arguments]\n\n",
         argv[0]);
      printf(OHPARAM_HELP"\n");
      printf("    Options: \n");
      printf("      -h, --help                   Show help options       \n");
      printf("      -D, --domain=nn              Select domain id nn     \n");
      printf("      -X, --debug                  Display debug messages  \n");
      printf("      -N, --host=\"host<:port>\"     Open session to the domain served by the daemon  \n");
      printf("                                   at the specified URL (host:port)  \n");
      printf("                                   This option overrides the OPENHPI_DAEMON_HOST and  \n");
      printf("                                   OPENHPI_DAEMON_PORT environment variables.  \n");
      printf("      -C, --cfgfile=\"file\"         Use passed file as client configuration file  \n");
      printf("                                   This option overrides the OPENHPICLIENT_CONF  \n");
      printf("                                   environment variable.  \n\n");
      return 1;
   }

   rv = ohc_session_open_by_option ( &copt, &sessionid);
   if (rv != SA_OK) return -1;

   switch (cmd){
      case eGlobalParamGet:
           rv = execglobalparamget ( );
           break;
      case eGlobalParamSet:
           rv = execglobalparamset ( paramtype, setparam );
           break;
      default: 
           printf("\n\nSorry, this function is not implemented yet\n\n");
           rv = SA_ERR_HPI_INVALID_PARAMS;
   }

   if (rv == SA_OK) {
      rv = saHpiSessionClose(sessionid);
      return 0;
   }
   printf("Param set failed with returncode %s\n", oh_lookup_error(rv));
   return rv;

}

/*
 *
 */
static SaErrorT execglobalparamget ()
{
   SaErrorT rv = SA_OK;
   oHpiGlobalParamT param;

   if (copt.debug) DBG("Go and read global parameters in domain %u", copt.domainid);

   param.Type = OHPI_LOG_ON_SEV;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_LOG_ON_SEV) returned %s", oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_LOG_ON_SEV = %s\n", oh_lookup_severity (param.u.LogOnSev));
   
   param.Type =OHPI_EVT_QUEUE_LIMIT; 
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_EVT_QUEUE_LIMIT) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_EVT_QUEUE_LIMIT = %u\n", param.u.EvtQueueLimit);

   param.Type =OHPI_DEL_SIZE_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_DEL_SIZE_LIMIT) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DEL_SIZE_LIMIT = %u\n", param.u.DelSizeLimit);

   param.Type =OHPI_DEL_SAVE;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_DEL_SAVE) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DEL_SAVE = ");
   if (param.u.DelSave) printf("TRUE\n");
   else printf("FALSE\n");
   
   param.Type =OHPI_DAT_SIZE_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_DAT_SIZE_LIMIT) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_SIZE_LIMIT = %u\n", param.u.DatSizeLimit);

   param.Type =OHPI_DAT_USER_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_DAT_USER_LIMIT) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_USER_LIMIT = %u\n", param.u.DatUserLimit);

   param.Type =OHPI_DAT_SAVE;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_DAT_SAVE) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_SAVE = ");
   if (param.u.DatSave) printf("TRUE\n");
   else printf("FALSE\n");
        
   param.Type =OHPI_PATH;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_PATH) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_PATH = %s\n",(const char*)param.u.Path);

   param.Type =OHPI_VARPATH;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_VARPATH) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_VARPATH = %s\n",(const char*)param.u.VarPath);

   param.Type =OHPI_CONF;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamGet(OHPI_CONF) returned %s",
             oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_CONF = %s\n",(const char*)param.u.Conf);

   return SA_OK;
}

/*
 *
 */
static SaErrorT execglobalparamset (oHpiGlobalParamTypeT ptype, char *setparam)
{
   SaErrorT rv = SA_OK;
   oHpiGlobalParamT param;
   SaHpiTextBufferT buffer;

   if (copt.debug) DBG("Go and set global parameter %u in domain %u to %s", 
                       ptype, copt.domainid, setparam);

   param.Type = ptype;
   switch (ptype){
      //strings
      case OHPI_PATH: strcpy((char*)param.u.Path, setparam);break;
      case OHPI_VARPATH: strcpy((char*)param.u.VarPath, setparam);break;
      case OHPI_CONF: strcpy((char*)param.u.Conf, setparam);break;
      //severity
      case OHPI_LOG_ON_SEV: 
         strncpy((char *)buffer.Data, setparam, SAHPI_MAX_TEXT_BUFFER_LENGTH);
         if (oh_encode_severity(&buffer, &param.u.LogOnSev)!=SA_OK)
            return SA_ERR_HPI_INVALID_PARAMS;
         break;
      //integers
      case OHPI_EVT_QUEUE_LIMIT: param.u.EvtQueueLimit = atoi(setparam);break;
      case OHPI_DEL_SIZE_LIMIT:  param.u.DelSizeLimit  = atoi(setparam);break;
      case OHPI_DAT_SIZE_LIMIT:  param.u.DatSizeLimit  = atoi(setparam);break;
      case OHPI_DAT_USER_LIMIT:  param.u.DatUserLimit  = atoi(setparam);break;
      //bools
      case OHPI_DEL_SAVE: 
         if (strcmp(setparam,"TRUE")==0) param.u.DelSave = SAHPI_TRUE;
         else if (strcmp(setparam,"FALSE")==0) param.u.DelSave = SAHPI_FALSE;
         else return SA_ERR_HPI_INVALID_PARAMS;
         break;
      case OHPI_DAT_SAVE:
         if (strcmp(setparam,"TRUE")==0) param.u.DatSave = SAHPI_TRUE;
         else if (strcmp(setparam,"FALSE")==0) param.u.DatSave = SAHPI_FALSE;
         else return SA_ERR_HPI_INVALID_PARAMS;
         break;
      default: return SA_ERR_HPI_UNSUPPORTED_PARAMS;
   }
   rv = oHpiGlobalParamSet (sessionid, &param);

   if (rv!=SA_OK) {
      CRIT("oHpiGlobalParamSet returned %s", oh_lookup_error(rv));
      return rv;
   }

   switch (param.Type){
      case OHPI_LOG_ON_SEV: printf("OPENHPI_LOG_ON_SEV = %s\n",
         oh_lookup_severity (param.u.LogOnSev));
         break;
      case OHPI_EVT_QUEUE_LIMIT: printf("OPENHPI_EVT_QUEUE_LIMIT = %u\n",
         param.u.EvtQueueLimit);
         break;
      case OHPI_DEL_SIZE_LIMIT: printf("OPENHPI_DEL_SIZE_LIMIT = %u\n",
                                       param.u.DelSizeLimit);
         break;
      case OHPI_DEL_SAVE: printf("OPENHPI_DEL_SAVE = ");
         if (param.u.DelSave) printf("TRUE\n");
         else printf("FALSE\n");
         break;
      case OHPI_DAT_SIZE_LIMIT: printf("OPENHPI_DAT_SIZE_LIMIT = %u\n",
                                       param.u.DatSizeLimit);
         break;
      case OHPI_DAT_USER_LIMIT: printf("OPENHPI_DAT_USER_LIMIT = %u\n",
                                       param.u.DatUserLimit);
         break;
      case OHPI_DAT_SAVE: printf("OPENHPI_DAT_SAVE = ");
         if (param.u.DatSave) printf("TRUE\n");
         else printf("FALSE\n");
         break;
      case OHPI_PATH: printf("OHPENPI_PATH = %s\n",(const char*)param.u.Path);break;
      case OHPI_VARPATH: printf("OPENHPI_VARPATH = %s\n",(const char*)param.u.VarPath);break;
      case OHPI_CONF: printf("OPENHPI_CONF = %s\n",(const char*)param.u.Conf);break;
      default: return SA_ERR_HPI_UNSUPPORTED_PARAMS;
   }
   return SA_OK;
   
}

/* end ohparam.c */
