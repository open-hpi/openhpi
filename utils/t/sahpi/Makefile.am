# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES		= SaHpi_struct_utils.c

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) @TEST_CLEAN@

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -sf ../../$@; \
	fi

TESTS = \
	tSaHpi_struct_utils

check_PROGRAMS = \
	tSaHpi_struct_utils

tSaHpi_struct_utils_SOURCES = tSaHpi_struct_utils.c $(REMOTE_SOURCES)
