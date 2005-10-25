# (C) Copyright IBM Corp 2005
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MOSTLYCLEANFILES 	= @TEST_CLEAN@

INCLUDES                = @OPENHPI_INCLUDES@ -I$(top_builddir)/cpp

AM_CFLAGS		= @NOCFLAGS@

MAINTAINERCLEANFILES 	= Makefile.in

TDEPLIB			= $(top_builddir)/cpp/libosahpi.la

TESTS = tSaHpiTextBuffer \
	tSaHpiEntityPath \
	tSaHpiSensorReading \
	tSaHpiSensorThresholds \
	tSaHpiSensorRec \
	tSaHpiCtrlRec \
	tSaHpiIdrField \
	tSaHpiIdrAreaHeader \
	tSaHpiIdrInfo \
	tSaHpiInventoryRec \
	tSaHpiWatchdog \
	tSaHpiWatchdogRec

check_PROGRAMS = $(TESTS)

tSaHpiTextBuffer_SOURCES = tSaHpiTextBuffer.cpp
tSaHpiTextBuffer_LDADD = $(TDEPLIB)
tSaHpiEntityPath_SOURCES = tSaHpiEntityPath.cpp
tSaHpiEntityPath_LDADD = $(TDEPLIB)
tSaHpiSensorReading_SOURCES = tSaHpiSensorReading.cpp
tSaHpiSensorReading_LDADD = $(TDEPLIB)
tSaHpiSensorThresholds_SOURCES = tSaHpiSensorThresholds.cpp
tSaHpiSensorThresholds_LDADD = $(TDEPLIB)
tSaHpiSensorRec_SOURCES = tSaHpiSensorRec.cpp
tSaHpiSensorRec_LDADD = $(TDEPLIB)
tSaHpiCtrlRec_SOURCES = tSaHpiCtrlRec.cpp
tSaHpiCtrlRec_LDADD = $(TDEPLIB)
tSaHpiIdrField_SOURCES = tSaHpiIdrField.cpp
tSaHpiIdrField_LDADD = $(TDEPLIB)
tSaHpiIdrAreaHeader_SOURCES = tSaHpiIdrAreaHeader.cpp
tSaHpiIdrAreaHeader_LDADD = $(TDEPLIB)
tSaHpiIdrInfo_SOURCES = tSaHpiIdrInfo.cpp
tSaHpiIdrInfo_LDADD = $(TDEPLIB)
tSaHpiInventoryRec_SOURCES = tSaHpiInventoryRec.cpp
tSaHpiInventoryRec_LDADD = $(TDEPLIB)
tSaHpiWatchdog_SOURCES = tSaHpiWatchdog.cpp
tSaHpiWatchdog_LDADD = $(TDEPLIB)
tSaHpiWatchdogRec_SOURCES = tSaHpiWatchdogRec.cpp
tSaHpiWatchdogRec_LDADD = $(TDEPLIB)

