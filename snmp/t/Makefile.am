# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES		= snmp_util.c

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) @TEST_CLEAN@

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/snmp

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

LDADD   		= $(top_builddir)/snmp/libopenhpi_snmp.la

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -sf ../$@; \
	fi

TESTS = snmp_utils_test

check_PROGRAMS = $(TESTS)

snmp_utils_test_SOURCES = snmp_utils_test.c  $(REMOTE_SOURCES)
