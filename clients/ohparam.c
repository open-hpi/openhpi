/*      -*- linux-c -*-
 *
 * Copyright (C) Copyright Nokia Siemens Networks 2010
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <uuid/uuid.h>
#include <SaHpi.h> 
#include <oh_utils.h>
#include <oh_clients.h>
#include <oHpi.h>

#define OH_SVN_REV "$Revision: 7142 $"

/* 
 * Function prototypes
 */
static SaErrorT execglobalparamget (void); 
static SaErrorT execglobalparamset (oHpiGlobalParamTypeT,char *); 


/* 
 * Globals 
 */
int fdebug     = 0;
SaHpiDomainIdT domainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
SaHpiSessionIdT sessionid;


/* 
 * Main                
 */
int
main(int argc, char **argv)
{
   SaErrorT    rv = SA_OK;
   
   oHpiGlobalParamTypeT paramtype = OHPI_CONF;
   char setparam[OH_MAX_TEXT_BUFFER_LENGTH];
   SaHpiBoolT printusage = FALSE;
   int i=1;

   enum cmdT {eUndefined,
      eGlobalParamGet,
      eGlobalParamSet} cmd=eUndefined;
       
   oh_prog_version(argv[0], OH_SVN_REV);

   while (i<argc && !printusage) {
      if (strcmp(argv[i],"-D")==0) {
         if (++i<argc) domainid = atoi(argv[i]);
         else printusage = TRUE;
      }
      else if (strcmp(argv[i],"-x")==0) fdebug=1;
      else if (strcmp(argv[i],"get")==0) {
         cmd=eGlobalParamGet;
      }
      else if (strcmp(argv[i],"set")==0) {
         cmd=eGlobalParamSet;
         if (++i<argc) {
            if (strcmp(argv[i],"OPENHPI_ON_EP")==0) 
               paramtype=OHPI_ON_EP;
            else if (strcmp(argv[i],"OPENHPI_LOG_ON_SEV")==0) 
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
      printf("Usage: %s [-D domain] [-x] command [specific arguments]\n\n",
         argv[0]);
      printf("      -D nn  Select domain id nn (not supported yet by oh-functions)\n");
      printf("      -x     Display debug messages\n");
      printf("\n");
      printf("    Command get: display info about all global parameters\n");
      printf("             no specific arguments\n");
      printf("    Command set:\n");
      printf("             one of the daemon's global parameters:\n");
      printf("             (without the OPENHPI prefix)\n");
      printf("               ON_EP, LOG_ON_SEV, EVT_QUEUE_LIMIT, \n");
      printf("               DEL_SIZE_LIMIT, DEL_SAVE\n");
      printf("               DAT_SIZE_LIMIT, DAT_USER_LIMIT, DAT_SAVE\n");
      printf("               PATH, VARPATH, CONF\n");
      printf("             and the desired new value. Example:\n");
      printf("               ohparam set DEL_SIZE_LIMIT 20000\n");
      printf("\n\n");
      exit(1);
   }

   if (fdebug) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("saHpiSessionOpen\n");
      else printf("saHpiSessionOpen to domain %u\n",domainid);
   }
        rv = saHpiSessionOpen(domainid,&sessionid,NULL);
   if (rv != SA_OK) {
      printf("saHpiSessionOpen returns %d (%s)\n",
         rv, oh_lookup_error(rv));
      exit(-1);
   }
   if (fdebug)
      printf("saHpiSessionOpen returns with SessionId %u\n", 
              sessionid);

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
      exit(0);
   }
   printf("Param set failed with returncode %d (%s)\n",
         rv, oh_lookup_error(rv));
   exit(-1);

}

/*
 *
 */
static SaErrorT execglobalparamget ()
{
   SaErrorT rv = SA_OK;
   oHpiGlobalParamT param;

   if (fdebug) printf("Go and read global parameters in domain %u\n", domainid);

   param.Type = OHPI_ON_EP;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_ON_EP) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_ON_EP: ");
   oh_print_ep (&(param.u.OnEP),0);
   
   param.Type = OHPI_LOG_ON_SEV;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_LOG_ON_SEV) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_LOG_ON_SEV = %s\n", oh_lookup_severity (param.u.LogOnSev));
   
   param.Type =OHPI_EVT_QUEUE_LIMIT; 
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_EVT_QUEUE_LIMIT) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_EVT_QUEUE_LIMIT = %u\n", param.u.EvtQueueLimit);

   param.Type =OHPI_DEL_SIZE_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_DEL_SIZE_LIMIT) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DEL_SIZE_LIMIT = %u\n", param.u.DelSizeLimit);

   param.Type =OHPI_DEL_SAVE;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_DEL_SAVE) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DEL_SAVE = ");
   if (param.u.DelSave) printf("TRUE\n");
   else printf("FALSE\n");
   
   param.Type =OHPI_DAT_SIZE_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_DAT_SIZE_LIMIT) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_SIZE_LIMIT = %u\n", param.u.DatSizeLimit);

   param.Type =OHPI_DAT_USER_LIMIT;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_DAT_USER_LIMIT) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_USER_LIMIT = %u\n", param.u.DatUserLimit);

   param.Type =OHPI_DAT_SAVE;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_DAT_SAVE) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_DAT_SAVE = ");
   if (param.u.DatSave) printf("TRUE\n");
   else printf("FALSE\n");
        
   param.Type =OHPI_PATH;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_PATH) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_PATH = %s\n",param.u.Path);

   param.Type =OHPI_VARPATH;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_VARPATH) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_VARPATH = %s\n",param.u.VarPath);

   param.Type =OHPI_CONF;
   rv = oHpiGlobalParamGet (sessionid, &param);
   if (rv!=SA_OK) {
      printf("oHpiGlobalParamGet(OHPI_CONF) returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }
   printf("OPENHPI_CONF = %s\n",param.u.Conf);

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

   if (fdebug) printf("Go and set global parameter %u in domain %u to %s\n", ptype, domainid, setparam);

   param.Type = ptype;
   switch (ptype){
      //strings
      case OHPI_PATH: strcpy(param.u.Path, setparam);break;
      case OHPI_VARPATH: strcpy(param.u.VarPath, setparam);break;
      case OHPI_CONF: strcpy(param.u.Conf, setparam);break;
      //ep
      case OHPI_ON_EP: 
         if (oh_encode_entitypath(setparam,&param.u.OnEP)!=SA_OK)
            return SA_ERR_HPI_INVALID_PARAMS;
         break;
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
   }
   rv = oHpiGlobalParamSet (sessionid, &param);

   if (rv!=SA_OK) {
      printf("oHpiGlobalParamSet returned %d (%s)\n",
             rv, oh_lookup_error(rv));
      return rv;
   }

   switch (param.Type){
      case OHPI_ON_EP: printf("OPENHPI_ON_EP: ");
         oh_print_ep (&(param.u.OnEP),0);
         break;
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
      case OHPI_PATH: printf("OHPENPI_PATH = %s\n",param.u.Path);break;
      case OHPI_VARPATH: printf("OPENHPI_VARPATH = %s\n",param.u.VarPath);break;
      case OHPI_CONF: printf("OPENHPI_CONF = %s\n",param.u.Conf);break;
   }
   return SA_OK;
   
}

/* end ohparam.c */
