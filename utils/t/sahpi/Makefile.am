# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES	= SaHpi_enum_utils.c \
		  SaHpi_event_utils.c \
		  SaHpi_event_utils.c.embed \
	          SaHpi_struct_utils.c \
	          SaHpi_time_utils.c 

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) @TEST_CLEAN@

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -sf ../../$@; \
	fi

TESTS = tSaHpi_enum_utils \
	tSaHpi_time_utils \
	tSaHpi_event_embed \
	tSaHpi_event_utils \
	tSaHpi_struct_utils

check_PROGRAMS = $(TESTS)

tSaHpi_enum_utils_SOURCES = tSaHpi_enum_utils.c  $(REMOTE_SOURCES)
tSaHpi_event_embed_SOURCES = tSaHpi_event_embed.c  $(REMOTE_SOURCES)
tSaHpi_event_utils_SOURCES = tSaHpi_event_utils.c  $(REMOTE_SOURCES)
tSaHpi_struct_utils_SOURCES = tSaHpi_struct_utils.c $(REMOTE_SOURCES)
tSaHpi_time_utils_SOURCES = tSaHpi_time_utils.c $(REMOTE_SOURCES)
