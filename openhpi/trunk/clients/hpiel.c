/* -*- linux-c -*-
 *
 * (C) Copyright IBM Corp 2006
 * (C) Copyright Nokia Siemens Networks 2010
 * (C) Copyright Ulrich Kleber 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     Renier Morales <renier@openhpi.org>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * hpiel - Displays HPI event log entries.
 *
 *
 * Changes:
 *    09/06/2010  ulikleber  New option -D to select domain
 *    01/02/2011  ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 */

#include "oh_clients.h"

#define OH_SVN_REV "$Revision$"

#define err(format, ...) \
        do { \
                if (copt.debug) { \
                        fprintf(stderr, format "\n", ## __VA_ARGS__); \
                } \
        } while(0)

#define show_error_quit(msg) \
        do { \
                if (error) { \
			copt.debug = TRUE; \
                        err(msg, oh_lookup_error(error)); \
                        exit(-1); \
                } \
        } while(0)

/* Globals */
static struct hpiel_opts {
        gboolean  del;        /* Domain Event Log option. */
        gboolean  clear;      /* Clear the event log before traversing it. */
        gboolean  resource;   /* Get resource along with event log entry. */
        gboolean  rdr;        /* Get RDR along with event log entry. */
} opts = { FALSE, FALSE, FALSE, FALSE };
static oHpiCommonOptionsT copt;

static GOptionEntry my_options[] =
{
  { "del",      'd', 0, G_OPTION_ARG_NONE, &opts.del,      "Display domain event log entries",           NULL },
  { "clear",    'c', 0, G_OPTION_ARG_NONE, &opts.clear,    "Clear log before reading event log entries", NULL },
  { "resource", 'p', 0, G_OPTION_ARG_NONE, &opts.resource, "Pull resource info along with log entry",    NULL },
  { "rdr",      'r', 0, G_OPTION_ARG_NONE, &opts.rdr,      "Pull RDR info along with log entry",         NULL },
  { NULL }
};


SaHpiDomainIdT domainid = SAHPI_UNSPECIFIED_DOMAIN_ID;

/* Prototypes */
SaErrorT parse_options(int argc, char ***argv);
SaErrorT harvest_sels(SaHpiSessionIdT sid, SaHpiDomainInfoT *dinfo);
SaErrorT display_el(SaHpiSessionIdT sid, SaHpiResourceIdT rid, SaHpiTextBufferT *tag);

int main(int argc, char **argv)
{
        SaErrorT error = SA_OK;
        SaHpiSessionIdT sid;
        SaHpiDomainInfoT dinfo;
        GError *gerror = NULL;
        GOptionContext *context;

        /* Print version strings */
	oh_prog_version(argv[0], OH_SVN_REV);

        /* Parsing options */
        context = g_option_context_new ("- Displays HPI event log entries\n"
                       "Option E (entity-path) displays resource event log entries.\n"
                       "If neither -d or -E \"<arg>\" are specified, "
                       "event log entries will be shown\n" 
                       "for all supporting resources by default."); 
        g_option_context_add_main_entries (context, my_options, NULL);

        if (!ohc_option_parse(&argc, argv, 
                context, &copt, 
                OHC_ALL_OPTIONS 
                    - OHC_VERBOSE_OPTION,    // no verbose mode implemented
                gerror)) { 
                g_print ("option parsing failed: %s\n", gerror->message);
                g_option_context_free (context);
		exit(1);
	}
        g_option_context_free (context);

        /* Program really begins here - all options parsed at this point */
        error = ohc_session_open_by_option ( &copt, &sid);
	if (error != SA_OK) exit(-1);
        show_error_quit("saHpiSessionOpen() returned %s. Exiting.\n");

        error = saHpiDiscover(sid);
        show_error_quit("saHpiDiscover() returned %s. Exiting.\n");

        error = saHpiDomainInfoGet(sid, &dinfo);
        show_error_quit("saHpiDomainInfoGet() returned %s. Exiting.\n");

        printf("Domain Info: UpdateCount = %u, UpdateTime = %lx\n",
      	       dinfo.RptUpdateCount, (unsigned long)dinfo.RptUpdateTimestamp);

        if (copt.withentitypath) { /* Entity path specified */
                error = harvest_sels(sid, &dinfo);  
        } else if (opts.del) { /* Domain event log specified */
                error = display_el(sid, SAHPI_UNSPECIFIED_RESOURCE_ID, &dinfo.DomainTag);
        } else { /* Else, show SELs of all supporting resources */
                error = harvest_sels(sid, &dinfo);
        }

        if (error) err("An error happened. Gathering event log entries returned %s",
                       oh_lookup_error(error));

        error = saHpiSessionClose(sid);
        if (error) err("saHpiSessionClose() returned %s.",
                       oh_lookup_error(error));

        return error;
}

SaErrorT harvest_sels(SaHpiSessionIdT sid, SaHpiDomainInfoT *dinfo)
{
        SaErrorT error = SA_OK;
        SaHpiRptEntryT rptentry;
        SaHpiEntryIdT entryid, nextentryid;
        SaHpiResourceIdT rid;
        oh_big_textbuffer bigbuf;
        SaHpiBoolT found_entry = SAHPI_FALSE;

        if (!sid || !dinfo) {
                err("Invalid parameters in havest_sels()\n");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        entryid = SAHPI_FIRST_ENTRY;
        while (error == SA_OK && entryid != SAHPI_LAST_ENTRY) {
                error = saHpiRptEntryGet(sid, entryid, &nextentryid, &rptentry);

                err("saHpiRptEntryGet() returned %s\n", oh_lookup_error(error));
                if (error == SA_OK) {
                        if (copt.withentitypath) {
                                if (!oh_cmp_ep(&copt.entitypath, &rptentry.ResourceEntity)) {
                                        entryid = nextentryid;
                                        continue;
                                }
                        }

                        if (!(rptentry.ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)) {
                                err("RPT doesn't have SEL\n");
                                entryid = nextentryid;
                                continue;  /* no SEL here, try next RPT */
                        }
                        found_entry = SAHPI_TRUE;

                        rid = rptentry.ResourceId;
                        err("RPT %u capabilities = %x\n",
                            rid, rptentry.ResourceCapabilities);
                        rptentry.ResourceTag.Data[rptentry.ResourceTag.DataLength] = 0;

                        oh_init_bigtext(&bigbuf);
                        error = oh_decode_entitypath(&rptentry.ResourceEntity, &bigbuf);
                        printf("%s\n", bigbuf.Data);
                        printf("rptentry[%u] tag: %s\n", rid, rptentry.ResourceTag.Data);

                        error = display_el(sid, rid, &rptentry.ResourceTag);

                        if (copt.withentitypath) return SA_OK;
                }

                entryid = nextentryid;
        }

        if (!found_entry) {
                if (copt.withentitypath) {
                       fprintf(stderr,   //cannot use err macro here
                                  "Could not find resource matching ");
                       oh_fprint_ep(stderr, &copt.entitypath, 0);
                       fprintf(stderr,"\n");
                } else {
                        err("No resources supporting event logs were found.\n");
                }
        }

        return error;
}

SaErrorT display_el(SaHpiSessionIdT sid, SaHpiResourceIdT rid, SaHpiTextBufferT *tag)
{
        SaErrorT error = SA_OK;
        SaHpiEventLogEntryIdT entryid, nextentryid, preventryid;
        SaHpiEventLogInfoT elinfo;
        SaHpiEventLogEntryT elentry;
        SaHpiRdrT rdr;
        SaHpiRptEntryT res;

        if (!sid || !rid) {
                err("Invalid parameters in display_el()\n");
                return SA_ERR_HPI_INVALID_PARAMS;
        }

        error = saHpiEventLogInfoGet(sid, rid, &elinfo);
        if (error) {
                err("saHpiEventLogInfoGet() returned %s. Exiting\n",
                    oh_lookup_error(error));
                return error;
        }

        printf("EventLogInfo for %s, ResourceId %u\n",
               tag->Data, rid);
        oh_print_eventloginfo(&elinfo, 4);

        if (elinfo.Entries == 0) {
                printf("%s Resource %u has an empty event log.\n", tag->Data, rid);
                return SA_OK;
        }

        if (opts.clear) {
                error = saHpiEventLogClear(sid, rid);
                if (error == SA_OK)
                        printf("EventLog successfully cleared\n");
                else {
                        printf("saHpiEventLogClear() returned %s\n",
                               oh_lookup_error(error));
                        return error;
                }

        }

        entryid = SAHPI_OLDEST_ENTRY;
        while (entryid != SAHPI_NO_MORE_ENTRIES) {
                error = saHpiEventLogEntryGet(sid, rid,
                                              entryid, &preventryid,
                                              &nextentryid, &elentry,
                                              &rdr,
                                              &res);

                err("saHpiEventLogEntryGet() returned %s\n", oh_lookup_error(error));
                if (error == SA_OK) {
                	SaHpiEntityPathT *ep = NULL;
                	/* Get a reference to the entity path for this log entry */
                	if (res.ResourceCapabilities) {
                		ep = &res.ResourceEntity;
                	} else if (rdr.RdrType != SAHPI_NO_RECORD) {
                		ep = &rdr.Entity;
                	}
                	/* Print the event log entry */
                        oh_print_eventlogentry(&elentry, ep, 6);
                        if (opts.rdr) {
                                if (rdr.RdrType == SAHPI_NO_RECORD)
                                        printf("            No RDR associated with EventType =  %s\n\n",
                                               oh_lookup_eventtype(elentry.Event.EventType));
                                else
                                        oh_print_rdr(&rdr, 12);
                        }

                        if (opts.resource) {
                                if (res.ResourceCapabilities == 0)
                                        printf("            No RPT associated with EventType =  %s\n\n",
                                               oh_lookup_eventtype(elentry.Event.EventType));
                                else
                                        oh_print_rptentry(&res, 10);
                        }

                        preventryid = entryid;
                        entryid = nextentryid;
                } else {
                        return error;
                }
        }

        return SA_OK;
}
