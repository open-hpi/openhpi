#
# Copyright (c) 2004 by IBM Corporation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#    W. David Ashley <dashley@us.ibm.com>
#

MAINTAINERCLEANFILES    = Makefile.in *~ core core.*

REMOTE_LIB_SOURCES 	= client.cpp \
		  	  client.h

MOSTLYCLEANFILES 	= @TEST_CLEAN@ \
			  $(REMOTE_LIB_SOURCES)

INCLUDES		= @OPENHPI_INCLUDES@ -I$(top_srcdir)/openhpid2  -I$(top_srcdir)/marshal

AM_CFLAGS		= -g @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_LIB_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

noinst_LTLIBRARIES	= libclient.la

libclient_la_SOURCES = $(REMOTE_LIB_SOURCES)

libclient_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libclient_la_LIBADD  = $(top_builddir)/marshal/libohtcpconnx.la

# Setup environment variables for TESTS programs
TESTS_ENVIRONMENT = OPENHPI_CONF=$(shell pwd)/openhpi.conf
TESTS_ENVIRONMENT += LD_LIBRARY_PATH=$(shell pwd)/../../src/.libs:$(shell pwd)/../../../utils/.libs
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_PATH=$(shell pwd)

TESTS = ctest_01

check_PROGRAMS = $(TESTS)

ctest_01_SOURCES = ctest_01.cpp
ctest_01_LDADD   = libclient.la

