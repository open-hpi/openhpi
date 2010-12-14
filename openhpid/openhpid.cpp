/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004-2008
 * (C) Copyright Pigeon Point Systems. 2010
 * (C) Copyright Nokia Siemens Networks 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *     W. David Ashley <dashley@us.ibm.com>
 *     David Judkoivcs <djudkovi@us.ibm.com>
 *     Renier Morales <renier@openhpi.org>
 *     Anton Pak <anton.pak@pigeonpoint.com>
 *     Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 */

#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <glib.h>

#include <SaHpi.h>
#include <oHpi.h>

#include <marshal_hpi.h>
#include <oh_domain.h>
#include <oh_error.h>
#include <oh_init.h>
#include <strmsock.h>


/*--------------------------------------------------------------------*/
/* Local definitions                                                  */
/*--------------------------------------------------------------------*/

extern "C"
{

static void service_thread(gpointer sock_ptr, gpointer /* user_data */);
static SaErrorT ProcessMsg(cHpiMarshal * hm,
                           int rq_byte_order,
                           char * data,
                           uint32_t& data_len,
                           SaHpiSessionIdT& changed_sid);
}

#define CLIENT_TIMEOUT 0  // Unlimited
#define PID_FILE "/var/run/openhpid.pid"

static bool stop_server = false;
static int sock_timeout = CLIENT_TIMEOUT;
static int verbose_flag = 0;

/* verbose macro */
#define PVERBOSE1(msg, ...) if (verbose_flag) dbg(msg, ## __VA_ARGS__)
#define PVERBOSE2(msg, ...) if (verbose_flag) err(msg, ## __VA_ARGS__)
#define PVERBOSE3(msg, ...) if (verbose_flag) printf("CRITICAL: "msg, ## __VA_ARGS__)

/*--------------------------------------------------------------------*/
/* Function to dehash handler cfg for oHpiHandlerInfo                 */
/*--------------------------------------------------------------------*/
static void dehash_handler_cfg(gpointer key, gpointer value, gpointer data)
{
    oHpiHandlerConfigT *hc = (oHpiHandlerConfigT *)data;

    strncpy((char *)hc->Params[hc->NumberOfParams].Name,
            (const char *)key,
            SAHPI_MAX_TEXT_BUFFER_LENGTH);
    strncpy((char *)hc->Params[hc->NumberOfParams].Value,
            (const char *)value,
            SAHPI_MAX_TEXT_BUFFER_LENGTH);

    ++hc->NumberOfParams;
}

/*--------------------------------------------------------------------*/
/* Function: display_help                                             */
/*--------------------------------------------------------------------*/

void display_help(void)
{
    printf("Help for openhpid:\n\n");
    printf("   openhpid -c conf_file [-v] [-p port] [-f pidfile]\n\n");
    printf("   -c conf_file  Sets path/name of the configuration file.\n");
    printf("                 This option is required unless the environment\n");
    printf("                 variable OPENHPI_CONF has been set to a valid\n");
    printf("                 configuration file.\n");
    printf("   -v            This option causes the daemon to display verbose\n");
    printf("                 messages. This option is optional.\n");
    printf("   -p port       Overrides the default listening port (%d) of\n",
           OPENHPI_DEFAULT_DAEMON_PORT);
    printf("                 the daemon. The option is optional.\n");
    printf("   -f pidfile    Overrides the default path/name for the daemon.\n");
    printf("                 pid file. The option is optional.\n");
    printf("   -s seconds    Overrides the default socket read timeout of 30\n");
    printf("                 minutes. The option is optional.\n");
    printf("   -t threads    Sets the maximum number of connection threads.\n");
    printf("                 The default is umlimited. The option is optional.\n");
    printf("   -n            Forces the code to run as a foreground process\n");
    printf("                 and NOT as a daemon. The default is to run as\n");
    printf("                 a daemon. The option is optional.\n\n");
    printf("A typical invocation might be\n\n");
    printf("   ./openhpid -c /etc/openhpi/openhpi.conf\n\n");
}

/*--------------------------------------------------------------------*/
/* PID File Unility Functions                                         */
/*--------------------------------------------------------------------*/
bool check_pidfile(const char *pidfile)
{
    // TODO add more checks here
    if (!pidfile) {
        return false;
    }

    int fd = open(pidfile, O_RDONLY);
    if (fd >= 0) {
        char buf[32];
        memset(buf, 0, sizeof(buf));
        ssize_t len = read(fd, buf, sizeof(buf) - 1);
        if (len < 0) {
            err("Error: Cannot read from PID file.\n");
            return false;
        }
        close(fd);
        int pid = atoi(buf);
        if ((pid > 0) && (pid == getpid() || (kill(pid, 0) < 0))) {
            unlink(pidfile);
        } else {
            err("Error: There is another active OpenHPI daemon.\n");
            return false;
        }
    }

    return true;
}

bool update_pidfile(const char *pidfile)
{
    // TODO add more checks here
    if (!pidfile) {
        return false;
    }

    int fd = open(pidfile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP);
    if (fd < 0) {
        err("Error: Cannot open PID file.\n");
        return false;
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%d\n", (int)getpid());
    write(fd, buf, strlen(buf));
    close(fd);

    return true;
}

/*--------------------------------------------------------------------*/
/* Function: daemonize                                                */
/*--------------------------------------------------------------------*/

static bool daemonize(const char *pidfile)
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        return false;
    }

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid > 0) {
        exit(0);
    }
    // Become the session leader
    setsid();
    // Second fork to make sure we are detached
    // from any controlling terminal.
    pid = fork();
    if (pid < 0) {
        return false;
    } else if (pid > 0) {
        exit(0);
    }

    update_pidfile(pidfile);

    //chdir("/");
    umask(0); // Reset default file permissions

    // Close unneeded inherited file descriptors
    // Keep stdout and stderr open if they already are.
#ifdef NR_OPEN
    for (int i = 3; i < NR_OPEN; i++) {
#else
    for (int i = 3; i < 1024; i++) {
#endif
        close(i);
    }

    return true;
}

/*--------------------------------------------------------------------*/
/* Function: main                                                     */
/*--------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    struct option options[] = {
        {"verbose",   no_argument,       0, 'v'},
        {"nondaemon", no_argument,       0, 'n'},
        {"cfg",       required_argument, 0, 'c'},
        {"port",      required_argument, 0, 'p'},
        {"pidfile",   required_argument, 0, 'f'},
        {"timeout",   required_argument, 0, 's'},
        {"threads",   required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    char *cfgfile    = 0;
    int  port        = OPENHPI_DEFAULT_DAEMON_PORT;
    char *pidfile    = 0;
    int  max_threads = -1;  // unlimited
    bool runasdaemon = true;

    /* get the command line options */
    int c;
    while (1) {
        c = getopt_long(argc, argv, "nvc:p:f:s:t:", options, 0);
        /* detect when done scanning options */
        if (c == -1) {
            break;
        }
        switch (c) {
            case 0:
                /* no need to do anything here */
                break;
            case 'c':
                setenv("OPENHPI_CONF", optarg, 1);
                cfgfile = g_new0(char, strlen(optarg) + 1);
                strcpy(cfgfile, optarg);
                break;
            case 'p':
                setenv("OPENHPI_DAEMON_PORT", optarg, 1);
                port = atoi(optarg);
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case 'f':
                pidfile = g_new0(char, strlen(optarg) + 1);
                strcpy(pidfile, optarg);
                break;
            case 's':
                sock_timeout = atoi(optarg);
                if (sock_timeout < 0) {
                    sock_timeout = CLIENT_TIMEOUT;
                }
                break;
            case 't':
                max_threads = atoi(optarg);
                if (max_threads <= 0) {
                    max_threads = -1;
                }
                break;
            case 'n':
                runasdaemon = false;
                break;
            case '?':
            default:
                /* they obviously need it */
                display_help();
                exit(-1);
        }
    }
    if (optind < argc) {
        fprintf(stderr, "Error: Unknown command line option specified. Exiting.\n");
        display_help();
        exit(-1);
    }

    // see if we have a valid configuration file
    if ((!cfgfile) || (!g_file_test(cfgfile, G_FILE_TEST_EXISTS))) {
        fprintf(stderr, "Error: Cannot find configuration file. Exiting.\n");
        display_help();
        exit(-1);
    }

    if (!pidfile) {
        pidfile = g_new0(char, strlen(PID_FILE) + 1);
        strcpy(pidfile, PID_FILE);
    }
    // see if we are already running
    if (!check_pidfile(pidfile)) {
        fprintf(stderr, "Error: PID file check failed. Exiting.\n");
        display_help();
        exit(1);
    }
    if (!update_pidfile(pidfile)) {
        fprintf(stderr, "Error: Cannot update PID file. Exiting.\n");
        display_help();
        exit(1);
    }

    if (runasdaemon) {
        if (!daemonize(pidfile)) {
            exit(8);
        }
    }

    if (!g_thread_supported()) {
        g_thread_init(0);
    }

    if (oh_init()) { // Initialize OpenHPI
        err("There was an error initializing OpenHPI. Exiting.\n");
        return 8;
    }

    // create the server socket
    cServerStreamSock * ssock = new cServerStreamSock;
    if (!ssock->Create(port)) {
        err("Error creating server socket. Exiting.\n");
        delete ssock;
        return 8;
    }

    // announce ourselves
    dbg("%s started.\n", argv[0]);
    dbg("OPENHPI_CONF = %s.\n", cfgfile);
    dbg("OPENHPI_DAEMON_PORT = %d.\n\n", port);

    // create the thread pool
    GThreadPool *pool;
    pool = g_thread_pool_new(service_thread, 0, max_threads, FALSE, 0);

    // wait for a connection and then service the connection
    while (!stop_server) {
        cStreamSock * sock = ssock->Accept();
        if (!sock) {
            PVERBOSE1("Error accepting server socket.\n");
            break;
        }
        PVERBOSE1("### Spawning thread to handle connection. ###\n");
        g_thread_pool_push(pool, (gpointer)sock, 0);
    }

    // ensure all threads are complete
    g_thread_pool_free(pool, FALSE, TRUE);

    delete ssock;
    PVERBOSE1("Server socket closed.\n");

    return 0;
}




/*--------------------------------------------------------------------*/
/* Function: service_thread                                           */
/*--------------------------------------------------------------------*/

static void service_thread(gpointer sock_ptr, gpointer /* user_data */)
{
    cStreamSock * sock = (cStreamSock *)sock_ptr;
    gpointer thrdid = g_thread_self();
    // TODO several sids for one connection
    SaHpiSessionIdT my_sid = 0;

    PVERBOSE1("%p Servicing connection.\n", thrdid);

    /* set the read timeout for the socket */
    // TODO
    //sock->SetReadTimeout(sock_timeout);

    PVERBOSE1("### service_thread, thrdid [%p] ###\n", (void *)thrdid);

    while (true) {
        bool     rc;
        char     data[dMaxPayloadLength];
        uint32_t data_len;
        uint8_t  type;
        uint32_t id;
        int      rq_byte_order;

        rc = sock->ReadMsg(type, id, data, data_len, rq_byte_order);
        if (!rc) {
            PVERBOSE3("%p Error or Timeout while reading socket.\n", thrdid);
            break;
        } else if (type != eMhMsg) {
            PVERBOSE3("%p Unsupported message type. Discarding.\n", thrdid);
            sock->WriteMsg(eMhError, id, 0, 0);
        } else {
            cHpiMarshal *hm = HpiMarshalFind(id);
            SaErrorT process_rv;
            SaHpiSessionIdT changed_sid = 0;
            process_rv = ProcessMsg(hm, rq_byte_order, data, data_len, changed_sid);
            if (process_rv != SA_OK) {
                int mr = HpiMarshalReply0(hm, data, &process_rv);
                if (mr < 0) {
                    PVERBOSE2("%p Marshal failed.\n", thrdid);
                    break;
                }
                data_len = (uint32_t)mr;
            }
            rc = sock->WriteMsg(eMhMsg, id, data, data_len);
            if (!rc) {
                PVERBOSE2("%p Socket write failed.\n", thrdid);
                break;
            }
            if ((process_rv == SA_OK) && (changed_sid != 0)) {
                if (id == eFsaHpiSessionOpen) {
                    my_sid = changed_sid;
                } else if (id == eFsaHpiSessionClose) {
                    my_sid = 0;
                    break;
                }
            }
        }
    }

    // if necessary, clean up HPI lib data
    if (my_sid != 0) {
        saHpiSessionClose(my_sid);
    }
    delete sock; // cleanup thread instance data

    PVERBOSE1("%p Connection closed.\n", thrdid);
    // TODO why?
    return; // do NOT use g_thread_exit here!
}


/*----------------------------------------------------------------------------*/
/* RPC Call Processing                                                        */
/*----------------------------------------------------------------------------*/

struct Params
{
    explicit Params(void * p0 = 0, void * p1 = 0, void * p2 = 0,
                    void * p3 = 0, void * p4 = 0, void * p5 = 0)
    {
        array[0] = p0;
        array[1] = p1;
        array[2] = p2;
        array[3] = p3;
        array[4] = p4;
        array[5] = p5;
    }

    union {
        void * array[6];
        const void * const_array[6];
    };
};

#define Demarshal_Rq(rq_byte_order, hm, data, iparams) \
{ \
    int mr = HpiDemarshalRequest(rq_byte_order, hm, data, iparams.array); \
    if (mr < 0) { \
        return SA_ERR_HPI_INVALID_PARAMS; \
    } \
}

#define Marshal_Rp(hm, data, data_len, oparams) \
{ \
    int mr = HpiMarshalReply(hm, data, oparams.const_array); \
    if (mr < 0) { \
        return SA_ERR_HPI_INTERNAL_ERROR; \
    } \
    data_len = (uint32_t)mr; \
}


/*--------------------------------------------------------------------*/
/* Function: ProcessMsg                                                */
/*--------------------------------------------------------------------*/

static SaErrorT ProcessMsg(cHpiMarshal * hm,
                           int rq_byte_order,
                           char * data,
                           uint32_t& data_len,
                           SaHpiSessionIdT& changed_sid)
{
    gpointer thrdid = g_thread_self();

    PVERBOSE1("%p Processing RPC request %d.\n", thrdid, hm->m_id);

    changed_sid = 0;

    // These vars are used in many places
    SaErrorT             rv;
    SaHpiSessionIdT      sid;
    SaHpiResourceIdT     rid;
    SaHpiSensorNumT      snum;
    SaHpiCtrlNumT        cnum;
    SaHpiIdrIdT          iid;
    SaHpiWatchdogNumT    wnum;
    SaHpiAnnunciatorNumT anum;
    SaHpiDimiNumT        dnum;
    SaHpiFumiNumT        fnum;
    SaHpiRdrT            rdr;
    SaHpiRptEntryT       rpte;

    switch(hm->m_id) {
        case eFsaHpiSessionOpen: {
            SaHpiDomainIdT  did;
            void            *security = 0;

            Params iparams(&did);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSessionOpen(OH_DEFAULT_DOMAIN_ID, &sid, security);
            if (rv == SA_OK) {
                changed_sid = sid;
            }

            Params oparams(&rv, &sid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSessionClose: {

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSessionClose(sid);
            if (rv == SA_OK) {
                changed_sid = sid;
            }

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDiscover: {

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDiscover(sid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDomainInfoGet: {
            SaHpiDomainInfoT info;

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDomainInfoGet(sid, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDrtEntryGet: {
            SaHpiEntryIdT   eid;
            SaHpiEntryIdT   next_eid;
            SaHpiDrtEntryT  drte;

            Params iparams(&sid, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDrtEntryGet(sid, eid, &next_eid, &drte);

            Params oparams(&rv, &next_eid, &drte);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDomainTagSet: {
            SaHpiTextBufferT tag;

            Params iparams(&sid, &tag);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDomainTagSet(sid, &tag);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiRptEntryGet: {
            SaHpiEntryIdT   eid;
            SaHpiEntryIdT   next_eid;

            Params iparams(&sid, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiRptEntryGet(sid, eid, &next_eid, &rpte);

            Params oparams(&rv, &next_eid, &rpte);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiRptEntryGetByResourceId: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiRptEntryGetByResourceId(sid, rid, &rpte);

            Params oparams(&rv, &rpte);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceSeveritySet: {
            SaHpiSeverityT   sev;

            Params iparams(&sid, &rid, &sev);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceSeveritySet(sid, rid, sev);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceTagSet: {
            SaHpiTextBufferT tag;

            Params iparams(&sid, &rid, &tag);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceTagSet(sid, rid, &tag);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiMyEntityPathGet: {
            SaHpiEntityPathT ep;

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiMyEntityPathGet(sid, &ep);

            Params oparams(&rv, &ep);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceIdGet: {

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceIdGet(sid, &rid);

            Params oparams(&rv, &rid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiGetIdByEntityPath: {
            SaHpiEntityPathT   ep;
            SaHpiRdrTypeT      instr_type;
            SaHpiUint32T       instance;
            SaHpiInstrumentIdT instr_id;
            SaHpiUint32T       rpt_update_cnt;

            Params iparams(&sid, &ep, &instr_type, &instance);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiGetIdByEntityPath(sid, ep, instr_type, &instance, &rid, &instr_id, &rpt_update_cnt);

            Params oparams(&rv, &instance, &rid, &instr_id, &rpt_update_cnt);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiGetChildEntityPath: {
            SaHpiEntityPathT parent_ep;
            SaHpiUint32T     instance;
            SaHpiEntityPathT child_ep;
            SaHpiUint32T     rpt_update_cnt;

            Params iparams(&sid, &parent_ep, &instance);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiGetChildEntityPath(sid, parent_ep, &instance, &child_ep, &rpt_update_cnt);

            Params oparams(&rv, &instance, &child_ep, &rpt_update_cnt);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceFailedRemove: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceFailedRemove(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogInfoGet: {
            SaHpiEventLogInfoT info;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogInfoGet(sid, rid, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogCapabilitiesGet: {
            SaHpiEventLogCapabilitiesT caps;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogCapabilitiesGet(sid, rid, &caps);

            Params oparams(&rv, &caps);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogEntryGet: {
            SaHpiEventLogEntryIdT eid;
            SaHpiEventLogEntryIdT prev_eid;
            SaHpiEventLogEntryIdT next_eid;
            SaHpiEventLogEntryT   ele;

            Params iparams(&sid, &rid, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogEntryGet(sid, rid, eid, &prev_eid, &next_eid, &ele, &rdr, &rpte);

            Params oparams(&rv, &prev_eid, &next_eid, &ele, &rdr, &rpte);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogEntryAdd: {
            SaHpiEventT evt;

            Params iparams(&sid, &rid, &evt);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogEntryAdd(sid, rid, &evt);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogClear: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogClear(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogTimeGet: {
            SaHpiTimeT time;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogTimeGet(sid, rid, &time);

            Params oparams(&rv, &time);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogTimeSet: {
            SaHpiTimeT time;

            Params iparams(&sid, &rid, &time);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogTimeSet(sid, rid, time);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogStateGet: {
            SaHpiBoolT enable;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogStateGet(sid, rid, &enable);

            Params oparams(&rv, &enable);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogStateSet: {
            SaHpiBoolT enable;

            Params iparams(&sid, &rid, &enable);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogStateSet(sid, rid, enable);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventLogOverflowReset: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventLogOverflowReset(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSubscribe: {

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSubscribe(sid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiUnsubscribe: {

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiUnsubscribe(sid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventGet: {
            SaHpiTimeoutT        timeout;
            SaHpiEventT          evt;
            SaHpiEvtQueueStatusT status;

            Params iparams(&sid, &timeout);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventGet(sid, timeout, &evt, &rdr, &rpte, &status);

            Params oparams(&rv, &evt, &rdr, &rpte, &status);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiEventAdd: {
            SaHpiEventT evt;

            Params iparams(&sid, &evt);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiEventAdd(sid, &evt);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAlarmGetNext: {
            SaHpiSeverityT sev;
            SaHpiBoolT     unack;
            SaHpiAlarmT    alarm;

            Params iparams(&sid, &sev, &unack, &alarm);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAlarmGetNext(sid, sev, unack, &alarm);

            Params oparams(&rv, &alarm);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAlarmGet: {
            SaHpiAlarmIdT aid;
            SaHpiAlarmT   alarm;

            Params iparams(&sid, &aid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAlarmGet(sid, aid, &alarm);

            Params oparams(&rv, &alarm);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAlarmAcknowledge: {
            SaHpiAlarmIdT  aid;
            SaHpiSeverityT sev;

            Params iparams(&sid, &aid, &sev);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAlarmAcknowledge(sid, aid, sev);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAlarmAdd: {
            SaHpiAlarmT alarm;

            Params iparams(&sid, &alarm);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAlarmAdd(sid, &alarm);

            Params oparams(&rv, &alarm);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAlarmDelete: {
            SaHpiAlarmIdT  aid;
            SaHpiSeverityT sev;

            Params iparams(&sid, &aid, &sev);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAlarmDelete(sid, aid, sev);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiRdrGet: {
            SaHpiEntryIdT eid;
            SaHpiEntryIdT next_eid;

            Params iparams(&sid, &rid, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiRdrGet(sid, rid, eid, &next_eid, &rdr);

            Params oparams(&rv, &next_eid, &rdr);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiRdrGetByInstrumentId: {
            SaHpiRdrTypeT      rdr_type;
            SaHpiInstrumentIdT instr_id;

            Params iparams(&sid, &rid, &rdr_type, &instr_id);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiRdrGetByInstrumentId(sid, rid, rdr_type, instr_id, &rdr);

            Params oparams(&rv, &rdr);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiRdrUpdateCountGet: {
            SaHpiUint32T rdr_update_cnt;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiRdrUpdateCountGet(sid, rid, &rdr_update_cnt);

            Params oparams(&rv, &rdr_update_cnt);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorReadingGet: {
            SaHpiSensorReadingT reading;
            SaHpiEventStateT    state;

            Params iparams(&sid, &rid, &snum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorReadingGet(sid, rid, snum, &reading, &state);

            Params oparams(&rv, &reading, &state);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorThresholdsGet: {
            SaHpiSensorThresholdsT tholds;

            Params iparams(&sid, &rid, &snum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorThresholdsGet(sid, rid, snum, &tholds);

            Params oparams(&rv, &tholds);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorThresholdsSet: {
            SaHpiSensorThresholdsT tholds;

            Params iparams(&sid, &rid, &snum, &tholds);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorThresholdsSet(sid, rid, snum, &tholds);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorTypeGet: {
            SaHpiSensorTypeT    type;
            SaHpiEventCategoryT cat;

            Params iparams(&sid, &rid, &snum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorTypeGet(sid, rid, snum, &type, &cat);

            Params oparams(&rv, &type, &cat);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEnableGet: {
            SaHpiBoolT enabled;

            Params iparams(&sid, &rid, &snum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEnableGet(sid, rid, snum, &enabled);

            Params oparams(&rv, &enabled);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEnableSet: {
            SaHpiBoolT enabled;

            Params iparams(&sid, &rid, &snum, &enabled);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEnableSet(sid, rid, snum, enabled);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEventEnableGet: {
            SaHpiBoolT enabled;

            Params iparams(&sid, &rid, &snum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEventEnableGet(sid, rid, snum, &enabled);

            Params oparams(&rv, &enabled);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEventEnableSet: {
            SaHpiBoolT enabled;

            Params iparams(&sid, &rid, &snum, &enabled);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEventEnableSet(sid, rid, snum, enabled);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEventMasksGet: {
            SaHpiEventStateT amask;
            SaHpiEventStateT dmask;

            Params iparams(&sid, &rid, &snum, &amask, &dmask);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEventMasksGet(sid, rid, snum, &amask, &dmask);

            Params oparams(&rv, &amask, &dmask);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiSensorEventMasksSet: {
            SaHpiSensorEventMaskActionT action;
            SaHpiEventStateT            amask;
            SaHpiEventStateT            dmask;

            Params iparams(&sid, &rid, &snum, &action, &amask, &dmask);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiSensorEventMasksSet(sid, rid, snum, action, amask, dmask);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiControlTypeGet: {
            SaHpiCtrlTypeT type;

            Params iparams(&sid, &rid, &cnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiControlTypeGet(sid, rid, cnum, &type);

            Params oparams(&rv, &type);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiControlGet: {
            SaHpiCtrlModeT  mode;
            SaHpiCtrlStateT state;

            Params iparams(&sid, &rid, &cnum, &state);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiControlGet(sid, rid, cnum, &mode, &state);

            Params oparams(&rv, &mode, &state);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiControlSet: {
            SaHpiCtrlModeT  mode;
            SaHpiCtrlStateT state;

            Params iparams(&sid, &rid, &cnum, &mode, &state);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiControlSet(sid, rid, cnum, mode, &state);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrInfoGet: {
            SaHpiIdrInfoT info;

            Params iparams(&sid, &rid, &iid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrInfoGet(sid, rid, iid, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrAreaHeaderGet: {
            SaHpiIdrAreaTypeT   area;
            SaHpiEntryIdT       aid;
            SaHpiEntryIdT       next_aid;
            SaHpiIdrAreaHeaderT hdr;

            Params iparams(&sid, &rid, &iid, &area, &aid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrAreaHeaderGet(sid, rid, iid, area, aid, &next_aid, &hdr);

            Params oparams(&rv, &next_aid, &hdr);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrAreaAdd: {
            SaHpiIdrAreaTypeT area;
            SaHpiEntryIdT     aid;

            Params iparams(&sid, &rid, &iid, &area);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrAreaAdd(sid, rid, iid, area, &aid);

            Params oparams(&rv, &aid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrAreaAddById: {
            SaHpiIdrAreaTypeT type;
            SaHpiEntryIdT     aid;

            Params iparams(&sid, &rid, &iid, &type, &aid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrAreaAddById(sid, rid, iid, type, aid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrAreaDelete: {
            SaHpiEntryIdT aid;

            Params iparams(&sid, &rid, &iid, &aid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrAreaDelete(sid, rid, iid, aid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrFieldGet: {
            SaHpiEntryIdT      aid;
            SaHpiIdrFieldTypeT type;
            SaHpiEntryIdT      fid;
            SaHpiEntryIdT      next_fid;
            SaHpiIdrFieldT     field;

            Params iparams(&sid, &rid, &iid, &aid, &type, &fid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrFieldGet(sid, rid, iid, aid, type, fid, &next_fid, &field);

            Params oparams(&rv, &next_fid, &field);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrFieldAdd: {
            SaHpiIdrFieldT field;

            Params iparams(&sid, &rid, &iid, &field);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrFieldAdd(sid, rid, iid, &field);

            Params oparams(&rv, &field);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrFieldAddById: {
            SaHpiIdrFieldT field;

            Params iparams(&sid, &rid, &iid, &field);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrFieldAddById(sid, rid, iid, &field);

            Params oparams(&rv, &field);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrFieldSet: {
            SaHpiIdrFieldT field;

            Params iparams(&sid, &rid, &iid, &field);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrFieldSet(sid, rid, iid, &field);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiIdrFieldDelete: {
            SaHpiEntryIdT aid;
            SaHpiEntryIdT fid;

            Params iparams(&sid, &rid, &iid, &aid, &fid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiIdrFieldDelete(sid, rid, iid, aid, fid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiWatchdogTimerGet: {
            SaHpiWatchdogT wdt;

            Params iparams(&sid, &rid, &wnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiWatchdogTimerGet(sid, rid, wnum, &wdt);

            Params oparams(&rv, &wdt);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiWatchdogTimerSet: {
            SaHpiWatchdogT wdt;

            Params iparams(&sid, &rid, &wnum, &wdt);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiWatchdogTimerSet(sid, rid, wnum, &wdt);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiWatchdogTimerReset: {

            Params iparams(&sid, &rid, &wnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiWatchdogTimerReset(sid, rid, wnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorGetNext: {
            SaHpiSeverityT     sev;
            SaHpiBoolT         unack;
            SaHpiAnnouncementT ann;

            Params iparams(&sid, &rid, &anum, &sev, &unack, &ann);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorGetNext(sid, rid, anum, sev, unack, &ann);

            Params oparams(&rv, &ann);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorGet: {
            SaHpiEntryIdT      eid;
            SaHpiAnnouncementT ann;

            Params iparams(&sid, &rid, &anum, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorGet(sid, rid, anum, eid, &ann);

            Params oparams(&rv, &ann);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorAcknowledge: {
            SaHpiEntryIdT  eid;
            SaHpiSeverityT sev;

            Params iparams(&sid, &rid, &anum, &eid, &sev);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorAcknowledge(sid, rid, anum, eid, sev);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorAdd: {
            SaHpiAnnouncementT ann;

            Params iparams(&sid, &rid, &anum, &ann);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorAdd(sid, rid, anum, &ann);

            Params oparams(&rv, &ann);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorDelete: {
            SaHpiEntryIdT  eid;
            SaHpiSeverityT sev;

            Params iparams(&sid, &rid, &anum, &eid, &sev);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorDelete(sid, rid, anum, eid, sev);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorModeGet: {
            SaHpiAnnunciatorModeT mode;

            Params iparams(&sid, &rid, &anum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorModeGet(sid, rid, anum, &mode);

            Params oparams(&rv, &mode);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAnnunciatorModeSet: {
            SaHpiAnnunciatorModeT mode;

            Params iparams(&sid, &rid, &anum, &mode);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAnnunciatorModeSet(sid, rid, anum, mode);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiInfoGet: {
            SaHpiDimiInfoT info;

            Params iparams(&sid, &rid, &dnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiInfoGet(sid, rid, dnum, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestInfoGet: {
            SaHpiDimiTestNumT tnum;
            SaHpiDimiTestT    test;

            Params iparams(&sid, &rid, &dnum, &tnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestInfoGet(sid, rid, dnum, tnum, &test);

            Params oparams(&rv, &test);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestReadinessGet: {
            SaHpiDimiTestNumT tnum;
            SaHpiDimiReadyT   ready;

            Params iparams(&sid, &rid, &dnum, &tnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestReadinessGet(sid, rid, dnum, tnum, &ready);

            Params oparams(&rv, &ready);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestStart: {
            SaHpiDimiTestNumT                tnum;
            SaHpiDimiTestVariableParamsListT pl;
            SaHpiUint8T&                     ntestparams = pl.NumberOfParams;
            SaHpiDimiTestVariableParamsT*&   testparams  = pl.ParamsList;

            Params iparams(&sid, &rid, &dnum, &tnum, &pl);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestStart(sid, rid, dnum, tnum, ntestparams, testparams);
            g_free(pl.ParamsList);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestCancel: {
            SaHpiDimiTestNumT tnum;

            Params iparams(&sid, &rid, &dnum, &tnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestCancel(sid, rid, dnum, tnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestStatusGet: {
            SaHpiDimiTestNumT              tnum;
            SaHpiDimiTestPercentCompletedT percent;
            SaHpiDimiTestRunStatusT        status;

            Params iparams(&sid, &rid, &dnum, &tnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestStatusGet(sid, rid, dnum, tnum, &percent, &status);

            Params oparams(&rv, &percent, &status);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiDimiTestResultsGet: {
            SaHpiDimiTestNumT     tnum;
            SaHpiDimiTestResultsT results;

            Params iparams(&sid, &rid, &dnum, &tnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiDimiTestResultsGet(sid, rid, dnum, tnum, &results);

            Params oparams(&rv, &results);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiSpecInfoGet: {
            SaHpiFumiSpecInfoT info;

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiSpecInfoGet(sid, rid, fnum, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiServiceImpactGet: {
            SaHpiFumiServiceImpactDataT impact;

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiServiceImpactGet(sid, rid, fnum, &impact);

            Params oparams(&rv, &impact);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiSourceSet: {
            SaHpiBankNumT    bnum;
            SaHpiTextBufferT uri;

            Params iparams(&sid, &rid, &fnum, &bnum, &uri);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiSourceSet(sid, rid, fnum, bnum, &uri);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiSourceInfoValidateStart: {
            SaHpiBankNumT bnum;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiSourceInfoValidateStart(sid, rid, fnum, bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiSourceInfoGet: {
            SaHpiBankNumT        bnum;
            SaHpiFumiSourceInfoT info;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiSourceInfoGet(sid, rid, fnum, bnum, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiSourceComponentInfoGet: {
            SaHpiBankNumT           bnum;
            SaHpiEntryIdT           eid;
            SaHpiEntryIdT           next_eid;
            SaHpiFumiComponentInfoT info;


            Params iparams(&sid, &rid, &fnum, &bnum, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiSourceComponentInfoGet(sid, rid, fnum, bnum, eid, &next_eid, &info);

            Params oparams(&rv, &next_eid, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiTargetInfoGet: {
            SaHpiBankNumT      bnum;
            SaHpiFumiBankInfoT info;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiTargetInfoGet(sid, rid, fnum, bnum, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiTargetComponentInfoGet: {
            SaHpiBankNumT           bnum;
            SaHpiEntryIdT           eid;
            SaHpiEntryIdT           next_eid;
            SaHpiFumiComponentInfoT info;

            Params iparams(&sid, &rid, &fnum, &bnum, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiTargetComponentInfoGet(sid, rid, fnum, bnum, eid, &next_eid, &info);

            Params oparams(&rv, &next_eid, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiLogicalTargetInfoGet: {
            SaHpiFumiLogicalBankInfoT info;

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiLogicalTargetInfoGet(sid, rid, fnum, &info);

            Params oparams(&rv, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiLogicalTargetComponentInfoGet: {
            SaHpiEntryIdT                  eid;
            SaHpiEntryIdT                  next_eid;
            SaHpiFumiLogicalComponentInfoT info;

            Params iparams(&sid, &rid, &fnum, &eid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiLogicalTargetComponentInfoGet(sid, rid, fnum, eid, &next_eid, &info);

            Params oparams(&rv, &next_eid, &info);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiBackupStart: {

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiBackupStart(sid, rid, fnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiBankBootOrderSet: {
            SaHpiBankNumT bnum;
            SaHpiUint32T  pos;

            Params iparams(&sid, &rid, &fnum, &bnum, &pos);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiBankBootOrderSet(sid, rid, fnum, bnum, pos);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiBankCopyStart: {
            SaHpiBankNumT src_bnum;
            SaHpiBankNumT dst_bnum;

            Params iparams(&sid, &rid, &fnum, &src_bnum, &dst_bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiBankCopyStart(sid, rid, fnum, src_bnum, dst_bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiInstallStart: {
            SaHpiBankNumT bnum;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiInstallStart(sid, rid, fnum, bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiUpgradeStatusGet: {
            SaHpiBankNumT           bnum;
            SaHpiFumiUpgradeStatusT status;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiUpgradeStatusGet(sid, rid, fnum, bnum, &status);

            Params oparams(&rv, &status);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiTargetVerifyStart: {
            SaHpiBankNumT bnum;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiTargetVerifyStart(sid, rid, fnum, bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiTargetVerifyMainStart: {

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiTargetVerifyMainStart(sid, rid, fnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiUpgradeCancel: {
            SaHpiBankNumT bnum;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiUpgradeCancel(sid, rid, fnum, bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiAutoRollbackDisableGet: {
            SaHpiBoolT disable;

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiAutoRollbackDisableGet(sid, rid, fnum, &disable);

            Params oparams(&rv, &disable);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiAutoRollbackDisableSet: {
            SaHpiBoolT disable;

            Params iparams(&sid, &rid, &fnum, &disable);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiAutoRollbackDisableSet(sid, rid, fnum, disable);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiRollbackStart: {

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiRollbackStart(sid, rid, fnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiActivate: {

            Params iparams(&sid, &rid, &fnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiActivate(sid, rid, fnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiActivateStart: {
            SaHpiBoolT logical;

            Params iparams(&sid, &rid, &fnum, &logical);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiActivateStart(sid, rid, fnum, logical);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiFumiCleanup: {
            SaHpiBankNumT bnum;

            Params iparams(&sid, &rid, &fnum, &bnum);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiFumiCleanup(sid, rid, fnum, bnum);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiHotSwapPolicyCancel: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiHotSwapPolicyCancel(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceActiveSet: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceActiveSet(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceInactiveSet: {

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceInactiveSet(sid, rid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAutoInsertTimeoutGet: {
            SaHpiTimeoutT timeout;

            Params iparams(&sid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAutoInsertTimeoutGet(sid, &timeout);

            Params oparams(&rv, &timeout);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAutoInsertTimeoutSet: {
            SaHpiTimeoutT timeout;

            Params iparams(&sid, &timeout);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAutoInsertTimeoutSet(sid, timeout);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAutoExtractTimeoutGet: {
            SaHpiTimeoutT timeout;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAutoExtractTimeoutGet(sid, rid, &timeout);

            Params oparams(&rv, &timeout);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiAutoExtractTimeoutSet: {
            SaHpiTimeoutT timeout;

            Params iparams(&sid, &rid, &timeout);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiAutoExtractTimeoutSet(sid, rid, timeout);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiHotSwapStateGet: {
            SaHpiHsStateT state;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiHotSwapStateGet(sid, rid, &state);

            Params oparams(&rv, &state);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiHotSwapActionRequest: {
            SaHpiHsActionT action;

            Params iparams(&sid, &rid, &action);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiHotSwapActionRequest(sid, rid, action);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiHotSwapIndicatorStateGet: {
            SaHpiHsIndicatorStateT state;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiHotSwapIndicatorStateGet(sid, rid, &state);

            Params oparams(&rv, &state);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiHotSwapIndicatorStateSet: {
            SaHpiHsIndicatorStateT state;

            Params iparams(&sid, &rid, &state);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiHotSwapIndicatorStateSet(sid, rid, state);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiParmControl: {
            SaHpiParmActionT action;

            Params iparams(&sid, &rid, &action);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiParmControl(sid, rid, action);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceLoadIdGet: {
            SaHpiLoadIdT lid;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceLoadIdGet(sid, rid, &lid);

            Params oparams(&rv, &lid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceLoadIdSet: {
            SaHpiLoadIdT lid;

            Params iparams(&sid, &rid, &lid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceLoadIdSet(sid, rid, &lid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceResetStateGet: {
            SaHpiResetActionT action;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceResetStateGet(sid, rid, &action);

            Params oparams(&rv, &action);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourceResetStateSet: {
            SaHpiResetActionT action;

            Params iparams(&sid, &rid, &action);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourceResetStateSet(sid, rid, action);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourcePowerStateGet: {
            SaHpiPowerStateT state;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourcePowerStateGet(sid, rid, &state);

            Params oparams(&rv, &state);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFsaHpiResourcePowerStateSet: {
            SaHpiPowerStateT state;

            Params iparams(&sid, &rid, &state);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = saHpiResourcePowerStateSet(sid, rid, state);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiHandlerCreate: {
            oHpiHandlerIdT     hid;
            oHpiHandlerConfigT cfg;

            Params iparams(&sid, &cfg);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            GHashTable *cfg_tbl;
            cfg_tbl = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
            for (int n = 0; n < cfg.NumberOfParams; n++) {
                g_hash_table_insert(cfg_tbl,
                                    g_strdup((const gchar *)cfg.Params[n].Name),
                                    g_strdup((const gchar *)cfg.Params[n].Value));
            }
            g_free(cfg.Params);

            rv = oHpiHandlerCreate(sid, cfg_tbl, &hid);
            g_hash_table_destroy(cfg_tbl);

            Params oparams(&rv, &hid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiHandlerDestroy: {
            oHpiHandlerIdT hid;

            Params iparams(&sid, &hid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiHandlerDestroy(sid, hid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiHandlerInfo: {
            oHpiHandlerIdT hid;
            oHpiHandlerInfoT info;
            oHpiHandlerConfigT cfg;
            GHashTable *cfg_tbl;

            Params iparams(&sid, &hid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            cfg_tbl = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

            rv = oHpiHandlerInfo(sid, hid, &info, cfg_tbl);

            cfg.NumberOfParams = 0;
            cfg.Params = g_new0(oHpiHandlerConfigParamT, g_hash_table_size(cfg_tbl));
            // add each hash tbl entry to the marshable handler_cfg
            g_hash_table_foreach(cfg_tbl, dehash_handler_cfg, &cfg);

            Params oparams(&rv, &info, &cfg);
            Marshal_Rp(hm, data, data_len, oparams);
// TODO memory leak
            // cleanup
            g_hash_table_destroy(cfg_tbl);
        }
        break;

        case eFoHpiHandlerGetNext: {
            oHpiHandlerIdT hid, next_hid;

            Params iparams(&sid, &hid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiHandlerGetNext(sid, hid, &next_hid);

            Params oparams(&rv, &next_hid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiHandlerFind: {
            oHpiHandlerIdT hid;

            Params iparams(&sid, &rid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiHandlerFind(sid, rid, &hid);

            Params oparams(&rv, &hid);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiHandlerRetry: {
            oHpiHandlerIdT hid;

            Params iparams(&sid, &hid);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiHandlerRetry(sid, hid);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiGlobalParamGet: {
            oHpiGlobalParamT param;

            Params iparams(&sid, &param);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiGlobalParamGet(sid, &param);

            Params oparams(&rv, &param);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiGlobalParamSet: {
            oHpiGlobalParamT param;

            Params iparams(&sid, &param);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiGlobalParamSet(sid, &param);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        case eFoHpiInjectEvent: {
            oHpiHandlerIdT  hid;
            SaHpiEventT     evt;

            Params iparams(&sid, &hid, &evt, &rpte, &rdr);
            Demarshal_Rq(rq_byte_order, hm, data, iparams);

            rv = oHpiInjectEvent(sid, hid, &evt, &rpte, &rdr);

            Params oparams(&rv);
            Marshal_Rp(hm, data, data_len, oparams);
        }
        break;

        default:
            PVERBOSE2("%p Function not found\n", thrdid);
            return SA_ERR_HPI_UNSUPPORTED_API; 
    }

    PVERBOSE1("%p Return code = %d\n", thrdid, rv);

    return SA_OK;
}

