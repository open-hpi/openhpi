# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES		= oHpi.c \
                          lock.c \
                          plugin.c \
                          plugin_static.c \
                          config.c \
                          event.c \
                          domain.c \
                          session.c \
                          hotswap.c \
                          init.c \
                          safhpi.c \
                          el_utils.c \
                          rpt_utils.c \
                          epath_utils.c \
                          sahpi_enum_utils.c \
                          sahpi_time_utils.c \
                          sahpi_struct_utils.c \
                          sahpi_event_utils.c \
                          uid_utils.c

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) @TEST_CLEAN@

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
                if test -f ../../$@; then \
                        ln -s ../../$@; \
                else \
                        ln -s $(top_srcdir)/utils/$@ ./; \
                fi \
	fi

TESTS = ohpi_000 

check_PROGRAMS = ohpi_000

ohpi_000_SOURCES = ohpi_000.c $(REMOTE_SOURCES)
