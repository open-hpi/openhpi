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

TESTS_ENVIRONMENT = OPENHPI_PATH=$(top_builddir)/plugins/simulator
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_CONF=$(srcdir)/openhpi.conf

TESTS = 

