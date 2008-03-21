/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *      Raghavendra P.G. <raghavendra.pg@hp.com>
 */

#ifndef _OA_SOAP_PLUGIN_H
#define _OA_SOAP_PLUGIN_H

/* TODO: Check and remove the unneccessary system .h files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

/* TODO: Check and remove the unneccessary SSL libs */
#include <zlib.h>
#include <openssl/opensslconf.h>
#include <openssl/lhash.h>
#include <openssl/crypto.h>
#include <openssl/buffer.h>
#include <openssl/e_os2.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/blowfish.h>
#include <openssl/err.h>

#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_domain.h>
#include <oh_utils.h>
#include <oh_error.h>

#include <oa_soap_calls.h>
#include <oa_soap_callsupport.h>
#include <oa_soap_ssl.h>
#include <oa_soap.h>
#include <oa_soap_power.h>
#include <oa_soap_reset.h>
#include <oa_soap_hotswap.h>
#include <oa_soap_utils.h>
#include <oa_soap_control.h>
#include <oa_soap_inventory.h>
#include <oa_soap_sensor.h>
#include <oa_soap_discover.h>
#include <oa_soap_re_discover.h>
#include <oa_soap_oa_event.h>
#include <oa_soap_ps_event.h>
#include <oa_soap_fan_event.h>
#include <oa_soap_server_event.h>
#include <oa_soap_interconnect_event.h>
#include <oa_soap_event.h>
#include <oa_soap_sel.h>
#include <oa_soap_watchdog.h>
#include <oa_soap_annunciator.h>

#endif
