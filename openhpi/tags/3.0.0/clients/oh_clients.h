/*      -*- linux-c -*-
 *
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
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
 *      Shuah Khan <shuah.khan@hp.com>
 *      Ulrich Kleber <ulikleber@users.sourceforge.net>
 *
 * Changes:
 *    20/01/2011  ulikleber  Refactoring to use glib for option parsing and
 *                           introduce common options for all clients
 *
 */

#ifndef __OH_CLIENTS_H
#define __OH_CLIENTS_H

#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <SaHpi.h> 

#include <oHpi.h>
#include <oh_utils.h>
#include <oh_error.h>

#define OHC_ALL_OPTIONS        (SaHpiUint8T) 0x03F
#define OHC_DEBUG_OPTION       (SaHpiUint8T) 0x001
#define OHC_DOMAIN_OPTION      (SaHpiUint8T) 0x002
#define OHC_VERBOSE_OPTION     (SaHpiUint8T) 0x004
#define OHC_ENTITY_PATH_OPTION (SaHpiUint8T) 0x008
#define OHC_HOST_OPTION        (SaHpiUint8T) 0x010
#define OHC_CLIENTCONF_OPTION  (SaHpiUint8T) 0x020
#define OHC_SESSION_OPTIONS    (SaHpiUint8T) 0x032  // -D -N and -C

//Macro to replace $ with () in OH_SVN_REV string for print
#define OHC_PREPARE_REVISION(txt)  \
        char *dollar=strchr(txt, '$'); \
        if (dollar!=NULL) *dollar = '(';   \
        dollar=strchr(txt, '$');       \
        if (dollar!=NULL) *dollar = ')';

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
        gboolean         debug;
        gboolean         verbose;
        SaHpiDomainIdT   domainid;
        gboolean         withentitypath;
        SaHpiEntityPathT entitypath;
        gboolean         withdaemonhost;
        SaHpiTextBufferT daemonhost;
        SaHpiUint16T     daemonport;
        SaHpiTextBufferT clientconf;
} oHpiCommonOptionsT;

void oh_prog_version(const char *prog_name);

gboolean ohc_option_parse(int *argc, char *argv[],  
                          GOptionContext     *context,
                          oHpiCommonOptionsT *common_options,
                          SaHpiUint8T        optionmask);

SaErrorT ohc_session_open_by_option (
                      oHpiCommonOptionsT *opt,
                      SaHpiSessionIdT    *SessionId);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __OH_CLIENTS_H  */

