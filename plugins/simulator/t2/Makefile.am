# (C) Copyright IBM Corp 2005
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MOSTLYCLEANFILES 	= uid_map
EXTRA_DIST              = openhpi.conf

INCLUDES		= @OPENHPI_INCLUDES@

AM_CFLAGS		= @NOCFLAGS@

MAINTAINERCLEANFILES 	= Makefile.in


noinst_PROGRAMS                     = SensorEvent_001 \
                                      SensorEnableChangeEvent_001 \
                                      HotSwapEvent_001 \
                                      HotSwapEvent_002 \
                                      HotSwapEvent_003 \
                                      HotSwapEvent_004 \
                                      HotSwapEvent_005 \
                                      WatchdogEvent_001 \
                                      SoftwareEvent_001 \
                                      OemEvent_001 \
                                      UserEvent_001 \
                                      ResourceFailEvent_001 \
                                      ResourceRestoreEvent_001
SensorEvent_001_SOURCES             = SensorEvent_001.c
SensorEnableChangeEvent_001_SOURCES = SensorEnableChangeEvent_001.c
HotSwapEvent_001_SOURCES            = HotSwapEvent_001.c
HotSwapEvent_002_SOURCES            = HotSwapEvent_002.c
HotSwapEvent_003_SOURCES            = HotSwapEvent_003.c
HotSwapEvent_004_SOURCES            = HotSwapEvent_004.c
HotSwapEvent_005_SOURCES            = HotSwapEvent_005.c
WatchdogEvent_001_SOURCES           = WatchdogEvent_001.c
SoftwareEvent_001_SOURCES           = SoftwareEvent_001.c
OemEvent_001_SOURCES                = OemEvent_001.c
UserEvent_001_SOURCES               = UserEvent_001.c
ResourceFailEvent_001_SOURCES       = ResourceFailEvent_001.c
ResourceRestoreEvent_001_SOURCES    = ResourceRestoreEvent_001.c

