# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES =  epath_utils.c \
		  sahpi_enum_utils.c \
		  sahpi_event_encode.c \
		  sahpi_event_utils.c \
	          sahpi_struct_utils.c \
	          sahpi_time_utils.c 

MOSTLYCLEANFILES = $(REMOTE_SOURCES) @TEST_CLEAN@
INCLUDES	= -I$(top_srcdir)/include -I$(top_srcdir)/utils
AM_CFLAGS	= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -sf ../../$@; \
	fi

TESTS = sahpi_enum_utils_test \
	sahpi_event_encode_test \
	sahpi_event_utils_test \
	sahpi_time_utils_test \
	sahpi_struct_idrinfo_test \
	sahpi_struct_idrarea_test \
	sahpi_struct_idrfield_test \
	sahpi_struct_rptentry_test \
	sahpi_struct_utils_test

check_PROGRAMS = $(TESTS)

sahpi_enum_utils_test_SOURCES = sahpi_enum_utils_test.c  $(REMOTE_SOURCES)
sahpi_event_encode_test_SOURCES = sahpi_event_encode_test.c  $(REMOTE_SOURCES)
sahpi_event_utils_test_SOURCES = sahpi_event_utils_test.c  $(REMOTE_SOURCES)
sahpi_struct_utils_test_SOURCES = sahpi_struct_utils_test.c $(REMOTE_SOURCES)
sahpi_time_utils_test_SOURCES = sahpi_time_utils_test.c $(REMOTE_SOURCES)
sahpi_struct_idrinfo_test_SOURCES = sahpi_struct_idrinfo_test.c $(REMOTE_SOURCES)
sahpi_struct_idrarea_test_SOURCES = sahpi_struct_idrarea_test.c $(REMOTE_SOURCES)
sahpi_struct_idrfield_test_SOURCES = sahpi_struct_idrfield_test.c $(REMOTE_SOURCES)
sahpi_struct_rptentry_test_SOURCES = sahpi_struct_rptentry_test.c $(REMOTE_SOURCES)

