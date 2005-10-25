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
	tSaHpiSensorThresholds

check_PROGRAMS = $(TESTS)

tSaHpiTextBuffer_SOURCES = tSaHpiTextBuffer.cpp
tSaHpiTextBuffer_LDADD = $(TDEPLIB)
tSaHpiEntityPath_SOURCES = tSaHpiEntityPath.cpp
tSaHpiEntityPath_LDADD = $(TDEPLIB)
tSaHpiSensorThresholds_SOURCES = tSaHpiSensorThresholds.cpp
tSaHpiSensorThresholds_LDADD = $(TDEPLIB)

