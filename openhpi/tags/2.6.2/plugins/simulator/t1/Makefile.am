# (C) Copyright IBM Corp 2005
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

INCLUDES		= @OPENHPI_INCLUDES@ -I/usr/local/include

AM_CFLAGS		= @NOCFLAGS@

MAINTAINERCLEANFILES 	= Makefile.in

# removed until the gcc 3.4 bug is fixed
# @COV_CFLAGS@
TDEPLIB			= $(top_builddir)/src/libopenhpi.la \
			  $(top_builddir)/utils/libopenhpiutils.la


pkglib_LTLIBRARIES	   = librxlinuxqueue.la
librxlinuxqueue_la_SOURCES = rxlinuxqueue.c
librxlinuxqueue_la_CFLAGS  = -DEAPI -Wall -fPIC
librxlinuxqueue_la_LDFLAGS = -shared -no-undefined -lc

noinst_PROGRAMS        = sim_event_test
sim_event_test_SOURCES = sim_event_test.c
sim_event_test_LDADD   = $(TDEPLIB)



