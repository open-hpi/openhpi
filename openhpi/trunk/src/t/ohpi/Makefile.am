# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MOSTLYCLEANFILES 	= @TEST_CLEAN@

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

TESTS = ohpi_000 

check_PROGRAMS = ohpi_000

ohpi_000_SOURCES = ohpi_000.c $(REMOTE_SOURCES)
ohpi_000_LDADD   = $(top_builddir)/src/libopenhpi.la
