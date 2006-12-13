/*
 * remote client configurarion for openhpiclient.c
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#ifndef dClientConfig_h
#define dClientConfig_h

#define dOpenHpiClientFunction(name) RemoteClient##name

// use an extra parameter for config
#define dOpenHpiClientParam(...) (cOpenHpiClientConf *config, ##__VA_ARGS__)


#endif
