# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MOSTLYCLEANFILES 	= @TEST_CLEAN@ uid_map

INCLUDES		= @OPENHPI_INCLUDES@ 

AM_CFLAGS		= @NOCFLAGS@ 

# removed until the gcc 3.4 bug is fixed
# @COV_CFLAGS@

TDEPLIB                 = $(top_builddir)/src/libopenhpi.la \
			  $(top_builddir)/utils/libopenhpiutils.la

CDEPLIB                 = $(top_builddir)/openhpid2/libopenhpi.la \
			  $(top_builddir)/utils/libopenhpiutils.la

TESTS_ENVIRONMENT = LD_LIBRARY_PATH=$(top_builddir)/openhpid2/.libs:$(top_builddir)/utils/.libs:$(top_builddir)/marshal/.libs
TESTS_ENVIRONMENT += OPENHPI_PATH=$(top_builddir)/plugins/dummy:$(top_builddir)/plugins/ipmi:$(top_builddir)/plugins/ipmidirect:$(top_builddir)/plugins/watchdog:$(top_builddir)/plugins/sysfs:$(top_builddir)/plugins/text_remote:$(top_builddir)/plugins/snmp_bc:$(top_builddir)/plugins/snmp_rsa
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map

TESTS = ohpi_version_client

check_PROGRAMS = $(TESTS)

ohpi_version_client_SOURCES = ohpi_version_client.c
ohpi_version_client_LDADD   = $(CDEPLIB)

