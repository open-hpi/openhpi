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
 *     This routine controls the handlers loaded by OpenHPI daemons
 *     using the OpenHPI extensions as described in ohpi.c 
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

#define OH_SVN_REV "$Revision: 7133 $"

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
int fdebug     = 0;
SaHpiDomainIdT domainid = SAHPI_UNSPECIFIED_DOMAIN_ID;
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

   enum cmdT {eUndefined,
      eHandlerCreate, 
      eHandlerDestroy, 
      eHandlerInfo, 
      eHandlerGetNext, 
      eHandlerFind,
      eHandlerRetry,
      eHandlerList} cmd=eUndefined;
       
   oh_prog_version(argv[0], OH_SVN_REV);

   while (i<argc && !printusage && cmd!=eHandlerCreate) {
      if (strcmp(argv[i],"-D")==0) {
         if (++i<argc) domainid = atoi(argv[i]);
         else printusage = TRUE;
      }
      else if (strcmp(argv[i],"-x")==0) fdebug=1;

      else if (strcmp(argv[i],"create")==0) {
         cmd=eHandlerCreate;
         // exechandlercreate will do the remaining reading of 
         // parameters itself 
         rv = exechandlercreate (argc, argv, ++i);
         if (rv == SA_OK) exit (0);
         if (rv != SA_ERR_HPI_INVALID_PARAMS) exit (1);
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
      if (rv == SA_OK) exit (0);
      if (rv == SA_ERR_HPI_INVALID_PARAMS)
         printusage = TRUE;
      else exit (1);
   }

   if (printusage == TRUE || cmd == eUndefined)
   {
      printf("\n");
      printf("Usage: %s [-D domain] [-x] command [specific arguments]\n\n",
         argv[0]);
      printf("      -D nn  Select domain id nn (not supported yet by oh-functions)\n");
      printf("      -x     Display debug messages\n");
      printf("\n");
      printf("    ohhander [-D domain] [-x] list\n");
      printf("             List the handlers loaded in specified domain\n\n");
      printf("    ohhander [-D domain] [-x] info <handler-id>\n");
      printf("             Display info about handler <handler-id> \n\n");
      printf("    ohhander [-D domain] [-x] destroy <handler-id>\n");
      printf("             Unload handler <handler-id> and delete its config \n\n");
      printf("    ohhander [-D domain] [-x] getnext <handler-id>\n");
      printf("             Find next valid handler id from <handler-id> \n\n");
      printf("    ohhander [-D domain] [-x] find <resource-id>\n");
      printf("             Find the right handler for a resource id \n\n");
      printf("    ohhander [-D domain] [-x] retry <handler-id>\n");
      printf("             Retry loading of handler <handler-id> \n\n");
      printf("    ohhander [-D domain] [-x] create plugin <name> <params>\n");
      printf("             Create handler with the specified parameters.\n");
      printf("             Pairs of strings in commandline like in openhpi.conf.\n");
      printf("             Keyword plugin to select type of handler.\n");
      printf("             Entity root and other complex strings must be \n");
      printf("             enclosed in \".\n");
      printf("     example:\n");
      printf("             ohhandler create plugin libsimulator "
             "entity_root \"{SYSTEM_CHASSIS,1}\" name sim\n");

      printf("\n\n");
      exit(1);
   }

   if (fdebug) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("saHpiSessionOpen\n");
      else printf("saHpiSessionOpen to domain %d\n",domainid);
   }
        rv = saHpiSessionOpen(domainid,&sessionid,NULL);
   if (rv != SA_OK) {
      printf("saHpiSessionOpen returns %d (%s)\n",
         rv, oh_lookup_error(rv));
      exit(-1);
   }
   if (fdebug)
             printf("saHpiSessionOpen returns with SessionId %d\n", 
         sessionid);

   switch (cmd){
                case eHandlerCreate: break; //already done
                case eHandlerDestroy:
                                   exechandlerdestroy ( handlerid );
                                   break;
                case eHandlerInfo:
                                   exechandlerinfo ( handlerid );
                                   break;
                case eHandlerGetNext:
                                   exechandlergetnext ( handlerid );
                                   break;
                case eHandlerFind:
                                   exechandlerfind ( resid );
                                   break;
                case eHandlerRetry:
                                   exechandlerretry ( handlerid );
                                   break;
                case eHandlerList:
                                   exechandlerlist ( );
                                   break;
                case eUndefined:   break; //already done
   }

   rv = saHpiSessionClose(sessionid);
        
   exit(0);
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

   if (fdebug) printf ("createhandler started\n");

   while (i<argc){
      if (strcmp(argv[i],"-f")==0) {
         printf("input from file not implemented yet\n");
         return (SA_OK);
      }
      else if (++i<argc) {
         if (strcmp(argv[i-1],"plugin")==0) pluginnamegiven = SAHPI_TRUE;
         g_hash_table_insert( createparams,
            g_strdup( argv[i-1] ), 
            g_strdup( argv[i] ));
          if (fdebug) printf ("Pair of arguments: %s - %s\n",
            g_strdup( argv[i-1] ), 
            g_strdup( argv[i] ));
      }
      else // parameters not in pairs
         return (SA_ERR_HPI_INVALID_PARAMS);
      i++;
   }

   if (!pluginnamegiven) {
      printf("You must enter a valid plugin name\n");
      return (SA_ERR_HPI_INVALID_PARAMS);
   }

   if (fdebug) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID)
         printf("saHpiSessionOpen\n");
      else printf("saHpiSessionOpen to domain %d\n",domainid);
   }
   rv = saHpiSessionOpen(domainid,&sessionid,NULL);
   if (rv != SA_OK) {
      printf("saHpiSessionOpen returns %d (%s)\n",
             rv, oh_lookup_error(rv));
      return (SA_ERR_HPI_INVALID_PARAMS);
   }
   if (fdebug)
      printf("saHpiSessionOpen returns with SessionId %d\n",
             sessionid);

   if (fdebug) printf ("Calling oHpiHandlerCreate!\n");
   rv = oHpiHandlerCreate(createparams, &handlerid );

   if ( rv != SA_OK ) {
      printf("oHpiHandlerCreate returned %d (%s)\n",
         rv, oh_lookup_error(rv));
      return(rv);
   }
   
   printf("Handler %d successfully created!\n", handlerid);

   rv = saHpiSessionClose(sessionid);
        
   return(SA_OK);

}

/********************************************/ 
/* exechandlerdestroy                       */
/********************************************/
static SaErrorT exechandlerdestroy(oHpiHandlerIdT handlerid)
{
   SaErrorT rv = SA_OK;
   if (fdebug) printf("Go and unload handler %d in domain %d\n", 
            handlerid, domainid);
   
   rv = oHpiHandlerDestroy ( handlerid );

   if (fdebug) printf("oHpiHandlerDestroy returned %d (%s)\n", 
         rv, oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Handler %d successfully unloaded.\n\n", handlerid);
      return rv;
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("Handler %d is not existing in default domain.\n",
                handlerid);
      else printf("Handler %d is not existing in domain %d.\n",
                handlerid, domainid);
      return rv;
   }
   else printf("\nHandler %d couldn't be unloaded, Returncode %d (%s)\n", 
         handlerid, rv, oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerinfo                          */
/********************************************/
// function to print from hash table
static void print_pair (gpointer key, gpointer value, gpointer user_data)
{
   printf ("   %s %s\n ", (gchar *)key, (gchar *)value);
}

static 
SaErrorT exechandlerinfo(oHpiHandlerIdT handlerid)
{
   oHpiHandlerInfoT handlerinfo;
   SaErrorT rv = SA_OK;
   GHashTable *handlerconfig = g_hash_table_new_full(
                        g_str_hash, g_str_equal,
                        g_free, g_free );


   if (fdebug) printf("Go and display handler info for %d\n", handlerid);
   
   rv = oHpiHandlerInfo ( /* sessionid,*/ handlerid, &handlerinfo, &handlerconfig );

   if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("Handler %d is not existing in default domain.\n",
                handlerid);
      else printf("Handler %d is not existing in domain %d.\n",
                handlerid, domainid);
      g_hash_table_destroy(handlerconfig);
      return SA_OK;
   }
   else if (rv!=SA_OK) {
      printf("oHpiHandlerInfo returned %d (%s)\n", 
                rv, oh_lookup_error(rv));
      g_hash_table_destroy(handlerconfig);
      return rv;
   }
   if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("\n\nInfo for handler %d in default domain:\n\n",handlerid);
   else printf("\n\nInfo for handler %d in domain %d:\n\n",handlerid, domainid);
   printf("Plugin name: %s\n",handlerinfo.plugin_name);

   printf("Root Entity Path: ");
   oh_print_ep (&(handlerinfo.entity_root),0);

   printf("Failed attempts to load handler: %d\n",handlerinfo.load_failed);

   printf("Handler configuration:\n");
   g_hash_table_foreach(handlerconfig, print_pair, NULL);
   printf("\n");

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

   if (fdebug) printf("Go and get next handler from %d in domain %d\n",
                      handlerid, domainid);

   rv = oHpiHandlerGetNext ( handlerid, &nexthandlerid );

   if (fdebug) printf("oHpiHandlerGetNext returned %d (%s)\n",
                      rv, oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Next Handler found from %d is %d.\n\n",
             handlerid, nexthandlerid);
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("No next Handler found from %d in default domain.\n",
                        handlerid);
      else printf("No next Handler found from %d in domain %d.\n",
                        handlerid, domainid);
   }
   else printf("\noHpiHandlerGetNext returned %d (%s)\n",
               rv, oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerfind                          */
/********************************************/
static SaErrorT exechandlerfind(SaHpiResourceIdT resid)
{
   SaErrorT rv = SA_OK;
   oHpiHandlerIdT handlerid;

   if (fdebug) printf("Go and find handler for resource %d in domain "
                      "%d using session %d\n",
                      resid, domainid, sessionid);

   rv = oHpiHandlerFind ( sessionid, resid, &handlerid );

   if (fdebug) printf("oHpiHandlerFind returned %d (%s)\n",
                      rv, oh_lookup_error(rv));

   if (rv==SA_OK) {
      printf("Handler found for resource %d:   Handler-id %d.\n\n",
             resid, handlerid);
   }
   else if (rv==SA_ERR_HPI_NOT_PRESENT) {
      if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
         printf("No Handler found for resource %d in default domain.\n",
                      resid);
      else printf("No Handler found for resource %d in domain %d.\n",
                      resid, domainid);
   }
   else printf("\noHpiHandlerFind returned %d (%s)\n",
               rv, oh_lookup_error(rv));
   return rv;
}

/********************************************/ 
/* exechandlerretry                         */
/********************************************/
static SaErrorT exechandlerretry(oHpiHandlerIdT handlerid)
{
   SaErrorT rv = SA_OK;

   if (fdebug) printf("Go and retry loading handler %d in domain %d\n", 
                      handlerid, domainid);

   rv = oHpiHandlerRetry ( handlerid );

   if (rv!=SA_OK) {
      printf("oHpiHandlerRetry returned %d (%s)\n",
                        rv, oh_lookup_error(rv));
      return rv;
   }
   if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("\nHandler %d in default domain successfully re-loaded.\n",
                        handlerid);
   else printf("\nHandler %d in domain %d successfully re-loaded.\n",
                        handlerid, domainid);
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

   if (domainid==SAHPI_UNSPECIFIED_DOMAIN_ID) 
      printf("Handlers defined in default domain:\n");
   else printf("Handlers defined in Domain %d:\n",domainid);
   while (rv==SA_OK) {
      rv = oHpiHandlerGetNext ( handlerid, &nexthandlerid );

      if (fdebug) printf("oHpiHandlerGetNext (%d) returned %d (%s)\n",
                      handlerid, rv, oh_lookup_error(rv));

      if (rv==SA_OK) {
         //display info for that handler
         rv = oHpiHandlerInfo ( /* sessionid, */ nexthandlerid, &handlerinfo, &config );
         if (rv!=SA_OK) {
            printf("oHpiHandlerInfo for handler %d returned %d (%s)\n", 
                   nexthandlerid, rv, oh_lookup_error(rv));
            rv = SA_OK;
         }
         else {
            printf("Handler %d: %s, ",
               nexthandlerid, handlerinfo.plugin_name);
            oh_print_ep (&(handlerinfo.entity_root),0);
            // g_hash_table_destroy(config);
         }
      }
      else if (rv!=SA_ERR_HPI_NOT_PRESENT) {
         printf("Error: oHpiHandlerGetNext (%d) returned %d (%s)\n",
                        handlerid, rv, oh_lookup_error(rv));
      }
      handlerid = nexthandlerid;
   }
   printf("\n");
   if (rv==SA_ERR_HPI_NOT_PRESENT) return SA_OK;
   return rv;
}

/* end ohhandler.c */
