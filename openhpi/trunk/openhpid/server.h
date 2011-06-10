/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Anton Pak <anton.pak@pigeonpoint.com>
 *
 */

#ifndef __OH_SERVER_H
#define __OH_SERVER_H

#include <stdint.h>

#include <strmsock.h>


bool oh_server_run( int ipvflags,
                    const char * bindaddr,
                    uint16_t port,
                    unsigned int sock_timeout,
                    int max_threads );

void oh_server_request_stop( void );


#endif /* __OH_SERVER_H */

