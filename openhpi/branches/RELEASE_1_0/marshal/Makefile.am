#
# Copyright (c) 2004 by FORCE Computers.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#    Thomas Kanngieser <thomas.kanngieser@fci.com>
#

SUBDIRS			= t
DIST_SUBDIRS 		= t
EXTRA_DIST              = connection.h
MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CFLAGS		= @CFLAGS@
INCLUDES 		= -I $(top_srcdir)/include -I $(srcdir)
# just to clear LIBS
LIBS			=

# marshal and connection used by the daemon and client library
lib_LTLIBRARIES = libopenhpimarshal.la

libopenhpimarshal_la_SOURCES    = \
	connection.h \
	connection.c \
	marshal_hpi.h \
	marshal_hpi.c \
	marshal_hpi_types.h \
	marshal_hpi_types.c \
	marshal.h \
        marshal.c

libopenhpimarshal_la_LDFLAGS= -version-info @HPI_LIB_VERSION@

clean-local:
	rm -f *~ core core.*
