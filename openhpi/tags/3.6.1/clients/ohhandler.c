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
 *     This routine controls the handlers loaded by OpenHPI daemons
 *     using the OpenHPI extensions as described in ohpi.c 
 *
 * Changes:
 *     03/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                            introduce common options for all clients
 *
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision: 7133 $"

#define OHHANDLER_HELP \
    "Command list                                                   \n" \
    "         List the handlers loaded in specified domain          \n\n" \
    "Command info <handler-id>                                      \n" \
    "         Display info about handler <handler-id>               \n\n" \
    "Command destroy <handler-id>                                   \n" \
    "         Unload handler <handler-id> and delete its config     \n\n" \
    "Command getnext <handler-id>                                   \n" \
    "         Find next valid handler id from <handler-id>          \n\n" \
    "Command find <resource-id>                                     \n" \
    "         Find the right handler for a resource id              \n\n" \
    "Command retry <handler-id>                                     \n" \
    "         Retry loading of handler <handler-id>                 \n\n" \
    "Command create plugin <name> <params>                          \n" \
    "         Create handler with the specified parameters.         \n" \
    "         Pairs of strings in commandline like in openhpi.conf. \n" \
    "         Keyword plugin to select type of handler.             \n" \
    "         Entity root and other complex strings must be         \n" \
    "         enclosed in \".                                       \n" \
    "     Example:                                                  \n" \
    "         ohhandler create plugin libsimulator " \
             "entity_root \"{SYSTEM_CHASSIS,1}\" name sim\n"

/********************************************/ 
/* Function prototypes                      */
/********************************************/
static SaErrorT exechandlercreate(int, char **, int);
static SaErrorT exechandlerdestroy(oHpiHandlerIdT);
static SaErrorT exechandlerinfo(oHpiHandlerIdT);
static SaErrorT exechandlergetnext(oHpiHandlerIdT);
static SaErrorT exechandlerfind(SaHpiResourceIdT);
static SaErrorT exechandlerretry(oHpiHandlerIdT);
static SaErrorT exechandlerlist(void);


/********************************************/ 
/* Globals                                  */
/********************************************/
static oHpiCommonOptionsT copt;
SaHpiSessionIdT sessionid;


/********************************************/ 
/* Main                                     */
/********************************************/
int
main(int argc, char **argv)
{
   SaErrorT    rv = SA_OK;
   oHpiHandlerIdT handlerid = 0;
   SaHpiResourceIdT resid = 0;
   SaHpiBoolT printusage = FALSE;
   int i=1;
   GOptionContext *context;

   enum cmdT {eUndefined,
      eHandlerCreate, 
      eHandlerDestroy, 
      eHandlerInfo, 
      eHandlerGetNext, 
      eHandlerFind,
      eHandlerRetry,
      eHandlerList} cmd=eUndefined;
       
   /* Print version strings */
   oh_prog_version(argv[0]);

   /* Parsing options */
   static char usetext[]="command [specific arguments] - "
                         "Control openhpi plugin instances (handlers).\n  "
                         OH_SVN_REV "\n\n" OHHANDLER_HELP ;

   OHC_PREPARE_REVISION(usetext);
   context = g_option_context_new (usetext);

   if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_ENTITY_PATH_OPTION // not applicable
                    - OHC_VERBOSE_OPTION )) {    // no verbose mode
       printusage = TRUE;
   }
   g_option_context_free (context);

   /* Parse ohparam specific commands */
   while (i<argc && !printusage && cmd!=eHandlerCreate) {
      if (strcmp(argv[i],"create")==0) {
         cmd=eHandlerCreate;
         // exechandlercreate will do the remaining reading of 
         // parameters itself 
         rv = exechandlercreate (argc, argv, ++i);
         if (rv == SA_OK) return 0;
         if (rv != SA_ERR_HPI_INVALID_PARAMS) return 1;
         printusage = TRUE;
      }

      else if (strcmp(argv[i],"destroy")==0) {
         cmd=eHandlerDestroy;
         if (++i<argc) handlerid = atoi(argv[i]);
         else printusage = TRUE;
      }

      else if (strcmp(argv[i],"info")==0) {
         cmd=eHandlerInfo;
         if (++i<argc) handlerid = atoi(argv[i]);
         else printusage = TRUE;
      }

      else if (strcmp(argv[i],"getnext")==0) {
         cmd=eHandlerGetNext;
         if (++i<argc) handlerid = atoi(argv[i]);
         else printusage = TRUE;
      }

      else if (strcmp(argv[i],"find")==0) {
         cmd=eHandlerFind;
         if (++i<argc) resid = atoi(argv[i]);
         else printusage = TRUE;
      }

      else if (strcmp(argv[i],"retry")==0) {
         cmd=eHandlerRetry;
         if (++i<argc) handlerid = atoi(argv[i]);
         else printusage = TRUE;
      }

      else if (strcmp(argv[i],"list")==0) {
         cmd=eHandlerList;
         if (++i<argc) printusage = TRUE;
      }

      else printusage = TRUE;

      i++;
   }

   if (cmd == eHandlerCreate) {
      rv = exechandlercreate (argc, argv, i);
      if (rv == SA_OK) return 0;
      if (rv == SA_ERR_HPI_INVALID_PARAMS)
         printusage = TRUE;
      else return 1;
   }

   if (printusage == TRUE || cmd == eUndefined)
   {
      printf("\n");
      printf("Usage: %s [Option...] command [specific arguments]\n\n",
         argv[0]);
      printf(OHHANDLER_HELP"\n");
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
   if (rv != SA_OK) return rv;

   switch (cmd){
                case eHandlerCreate: break; //already done
                case eHandlerDestroy:
                                   rv = exechandlerdestroy ( handlerid );
                                   break;
                case eHandlerInfo:
                                   rv = exechandlerinfo ( handlerid );
                                   break;
                case eHandlerGetNext:
                                   rv = exechandlergetnext ( handlerid );
                                   break;
                case eHandlerFind:
                                   rv = exechandlerfind ( resid );
                                   break;
                case eHandlerRetry:
                                   rv = exechandlerretry ( handlerid );
                                   break;
                case eHandlerList:
                                   rv = exechandlerlist ( );
                                   break;
                case eUndefined:   break; //already done
   }

   if (copt.debug) {
      DBG("Internal processing returns %s", oh_lookup_error(rv));
      DBG("Calling saHpiSessionClose for session %u",sessionid);
   }

   rv = saHpiSessionClose(sessionid);
   if (copt.debug) {
       DBG("saHpiSessionClose returns %s", oh_lookup_error(rv));
   }     
       
   return 0;
}

/********************************************/ 
/* exechandlercreate                        */
/********************************************/
static SaErrorT exechandlercreate (int argc, char **argv, int i)
{
   SaErrorT rv = SA_OK;
   oHpiHandlerIdT handlerid = 0;
   GHashTable * createparams = g_hash_table_new_full (
      g_str_hash, g_str_equal, g_free, g_free);
   SaHpiBoolT pluginnamegiven = SAHPI_FALSE;

   if (copt.debug) DBG ("createhandler started\n");

   while (i<argc){
      if (strcmp(argv[i],"-f")==0) {
         CRIT("input from file not implemented yet");
         return (SA_OK);
      }
      else if (++i<argc) {
         if (strcmp(argv[i-1],"plugin")==0) pluginnamegiven = SAHPI_TRUE;
         g_hash_table_insert( createparams,
            g_strdup( argv[i-1] ), 
            g_strdup( argv[i] ));
          if (copt.debug) DBG ("Pair of arguments: %s - %s\n",
            g_strdup( argv[i-1] ), 
            g_strdup( argv[i] ));
      }
      else // parameters not in pairs
         return (SA_ERR_HPI_INVALID_PARAMS);
      i++;
   }

   if (!pluginnamegiven) {
      CRIT("You must enter a valid plugin name");
      return (SA_ERR_HPI_INVALID_PARAMS);
   }

   rv = ohc_session_open_by_option ( &copt, &sessionid);
   if (rv != SA_OK) return rv;

   if (copt.debug) DBG("Calling oHpiHandlerCreate!");
   rv = oHpiHandlerCreate(sessionid, createparams, &handlerid );

   if ( rv != SA_OK ) {
      CRIT("oHpiHandlerCreate returned %s", oh_lookup_error(rv));
      saHpiSessionClose(sessionid);
      return(rv);
   }
   
   printf("Handler %u successfully created!\n", handlerid);

   rv = saHpiSessionClose(sessionid);
        
   return(SA_OK);

}

/********************************************/ 
/* exechandlerdestroy                       */
/********************************************/
static SaErrorT exechandlerdestroy(oHpiHandlerIdT handlerid)
{
   SaErrorT rv = SA_OK;
   if (copt.debug) DBG("Go and unload handler %u in domain %u", 
            handlerid, copt.domainid);
   
   rv = oHpiHandlerDestroy ( sessionid, handlerid );

   if (copt.debug) DBG("oHpiHandlerDestroy returned %s", oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Handler %u successfully unloaded.\n\n", handlerid);
      return rv;
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         CRIT("Handler %u is not existing in default domain.",
                handlerid);
      else CRIT("Handler %u is not existing in domain %u.",
                handlerid, copt.domainid);
      return rv;
   }
   else CRIT("\nHandler %u couldn't be unloaded, Returncode %s", 
                handlerid, oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerinfo                          */
/********************************************/
// function to print from hash table
static void print_pair (gpointer key, gpointer value, gpointer user_data)
{
   // we already printed the pluginname and entity-root
   if (strcmp((gchar *)key, "plugin")==0) return;
   if (strcmp((gchar *)key, "entity_root")==0) return;
   printf ("   %s %s\n", (gchar *)key, (gchar *)value);
}

static 
SaErrorT exechandlerinfo(oHpiHandlerIdT handlerid)
{
   oHpiHandlerInfoT handlerinfo;
   SaErrorT rv = SA_OK;
   GHashTable *handlerconfig = g_hash_table_new_full(
                        g_str_hash, g_str_equal,
                        g_free, g_free );

   if (copt.debug) DBG("Go and display handler info for %u", handlerid);
   
   rv = oHpiHandlerInfo ( sessionid, handlerid, &handlerinfo, handlerconfig );

   if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         CRIT("Handler %u is not existing in default domain.",
                handlerid);
      else CRIT("Handler %u is not existing in domain %u.",
                handlerid, copt.domainid);
      g_hash_table_destroy(handlerconfig);
      return SA_OK;
   }
   else if (rv!=SA_OK) {
      CRIT("oHpiHandlerInfo returned %s", oh_lookup_error(rv));
      g_hash_table_destroy(handlerconfig);
      return rv;
   }
   if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("\n\nInfo for handler %u in default domain:\n\n",handlerid);
   else printf("\n\nInfo for handler %u in domain %u:\n\n",handlerid, copt.domainid);
   printf("Plugin name: %s\n",(const char *)handlerinfo.plugin_name);

   printf("Root ");
   oh_print_ep (&(handlerinfo.entity_root),0);

   printf("Failed attempts to load handler: %u\n",handlerinfo.load_failed);

   printf("\nHandler configuration:\n");
   printf ("   plugin %s\n", (const char *)handlerinfo.plugin_name);
   printf ("   entity_root \"%s\"\n",(const char *) g_hash_table_lookup(handlerconfig, "entity_root")); 
   g_hash_table_foreach(handlerconfig, print_pair, NULL);
   printf("\n\n");

   g_hash_table_destroy(handlerconfig);
   return rv;
}

/********************************************/ 
/* exechandlergetnext                       */
/********************************************/
static SaErrorT exechandlergetnext(oHpiHandlerIdT handlerid)
{
   SaErrorT rv = SA_OK;
   oHpiHandlerIdT nexthandlerid;

   if (copt.debug) DBG("Go and get next handler from %u in domain %u",
                      handlerid, copt.domainid);

   rv = oHpiHandlerGetNext ( sessionid, handlerid, &nexthandlerid );

   if (copt.debug) DBG("oHpiHandlerGetNext returned %s", oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Next Handler found from %u is %u.\n\n",
             handlerid, nexthandlerid);
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("No next Handler found from %u in default domain.\n",
                        handlerid);
      else printf("No next Handler found from %u in domain %u.\n",
                        handlerid, copt.domainid);
   }
   else printf("\noHpiHandlerGetNext returned %s\n", oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerfind                          */
/********************************************/
static SaErrorT exechandlerfind(SaHpiResourceIdT resid)
{
   SaErrorT rv = SA_OK;
   oHpiHandlerIdT handlerid;

   if (copt.debug) DBG("Go and find handler for resource %u in domain "
                      "%u using session %u",
                      resid, copt.domainid, sessionid);

   rv = oHpiHandlerFind ( sessionid, resid, &handlerid );

   if (copt.debug) DBG("oHpiHandlerFind returned %s", oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Handler found for resource %u:   Handler-id %u.\n\n",
             resid, handlerid);
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("No Handler found for resource %u in default domain.\n",
                      resid);
      else printf("No Handler found for resource %u in domain %u.\n",
                      resid, copt.domainid);
   }
   else printf("\noHpiHandlerFind returned %s\n", oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerretry                         */
/********************************************/
static SaErrorT exechandlerretry(oHpiHandlerIdT handlerid)
{
   SaErrorT rv = SA_OK;

   if (copt.debug) DBG("Go and retry loading handler %u in domain %u", 
                      handlerid, copt.domainid);

   rv = oHpiHandlerRetry ( sessionid, handlerid );

   if (rv!=SA_OK) {
      CRIT("oHpiHandlerRetry returned %s", oh_lookup_error(rv));
      return rv;
   }
   if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("\nHandler %u in default domain successfully re-loaded.\n",
                        handlerid);
   else printf("\nHandler %u in domain %u successfully re-loaded.\n",
                        handlerid, copt.domainid);
   return rv;
}

/********************************************/ 
/* exechandlerlist                          */
/********************************************/
static SaErrorT exechandlerlist()
{
   SaErrorT rv = SA_OK;
   oHpiHandlerIdT handlerid = 0;
   oHpiHandlerIdT nexthandlerid;
   oHpiHandlerInfoT handlerinfo;
   GHashTable *config = 0;

   if (copt.domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("Handlers defined in default domain:\n");
   else printf("Handlers defined in Domain %u:\n",copt.domainid);
   while (rv==SA_OK) {
      rv = oHpiHandlerGetNext ( sessionid, handlerid, &nexthandlerid );

      if (copt.debug) DBG("oHpiHandlerGetNext (%u) returned %s",
                      handlerid, oh_lookup_error(rv));

      if (rv==SA_OK) {
         //display info for that handler
         config = g_hash_table_new_full(
                        g_str_hash, g_str_equal,
                        g_free, g_free );
         rv = oHpiHandlerInfo ( sessionid, nexthandlerid, &handlerinfo, config );
         if (rv!=SA_OK) {
            printf("oHpiHandlerInfo for handler %u returned %s\n", 
                   nexthandlerid, oh_lookup_error(rv));
            rv = SA_OK;
         }
         else {
            printf("Handler %u: %s, ",
               nexthandlerid, (const char *)handlerinfo.plugin_name);
            oh_print_ep (&(handlerinfo.entity_root),0);
         }
         g_hash_table_destroy(config);
      }
      else if (rv!=SA_ERR_HPI_NOT_PRESENT) {
         printf("Error: oHpiHandlerGetNext (%u) returned %s\n",
                        handlerid, oh_lookup_error(rv));
      }
      handlerid = nexthandlerid;
   }
   printf("\n");
   if (rv==SA_ERR_HPI_NOT_PRESENT) return SA_OK;
   return rv;
}

/* end ohhandler.c */
