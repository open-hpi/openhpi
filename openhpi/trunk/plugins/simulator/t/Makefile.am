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
EXTRA_DIST              = openhpi.conf

INCLUDES		= @OPENHPI_INCLUDES@ 

AM_CFLAGS		= @NOCFLAGS@ 

# removed until the gcc 3.4 bug is fixed
# -ftest-coverage -fprofile-arcs
TDEPLIB			= $(top_builddir)/src/libopenhpi.la \
				$(top_builddir)/utils/libopenhpiutils.la

TESTS_ENVIRONMENT = OPENHPI_PATH=$(top_builddir)/plugins/simulator
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_CONF=$(srcdir)/openhpi.conf

TESTS = sim_sanity_000 \
	sim_sanity_001 \
	sim_sanity_002 \
	sim_sanity_003 \
	sim_sanity_004 \
	sim_sanity_005 \
	sim_sanity_100 \
	sim_sanity_101 \
	sim_sanity_006 \
	sim_sanity_007 \
	sim_sanity_102

check_PROGRAMS = $(TESTS)

sim_sanity_000_SOURCES = sim_sanity_000.c
sim_sanity_000_LDADD = $(TDEPLIB)
sim_sanity_001_SOURCES = sim_sanity_001.c
sim_sanity_001_LDADD = $(TDEPLIB)
sim_sanity_002_SOURCES = sim_sanity_002.c
sim_sanity_002_LDADD = $(TDEPLIB)
sim_sanity_003_SOURCES = sim_sanity_003.c
sim_sanity_003_LDADD = $(TDEPLIB)
sim_sanity_004_SOURCES = sim_sanity_004.c
sim_sanity_004_LDADD = $(TDEPLIB)
sim_sanity_005_SOURCES = sim_sanity_005.c
sim_sanity_005_LDADD = $(TDEPLIB)
sim_sanity_100_SOURCES = sim_sanity_100.c
sim_sanity_100_LDADD = $(TDEPLIB)
sim_sanity_101_SOURCES = sim_sanity_101.c
sim_sanity_101_LDADD = $(TDEPLIB)
sim_sanity_006_SOURCES = sim_sanity_006.c
sim_sanity_006_LDADD = $(TDEPLIB)
sim_sanity_007_SOURCES = sim_sanity_007.c
sim_sanity_007_LDADD = $(TDEPLIB)
sim_sanity_102_SOURCES = sim_sanity_102.c
sim_sanity_102_LDADD = $(TDEPLIB)
