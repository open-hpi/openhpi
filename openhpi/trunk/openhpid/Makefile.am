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

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CFLAGS		= @CFLAGS@
INCLUDES 		= -I $(top_srcdir)/include
# just to clear LIBS
LIBS			=

EXTRA_DIST              = README openhpiclient.conf

# marshal and connection used by the daemon and client library
noinst_LIBRARIES        = libmarshal.a

libmarshal_a_SOURCES    = \
	connection.h \
	connection.c \
	marshal_hpi.h \
	marshal_hpi.c \
	marshal_hpi_types.h \
	marshal_hpi_types.c \
	marshal.h \
        marshal.c

# daemon
bin_PROGRAMS            = openhpid

openhpid_SOURCES        = \
	openhpid.h \
	openhpid.cpp

openhpid_LDADD          = libmarshal.a $(top_builddir)/src/libopenhpi.la
openhpid_LDFLAGS        = -static

# client library
# please do not use any external libraries
# except pthread
lib_LTLIBRARIES	        = libopenhpiclient.la

libopenhpiclient_la_SOURCES = \
	${libmarshal_a_SOURCES} \
	openhpiclient.c \
	simple_config.h \
	simple_config.c

libopenhpiclient_la_LDFLAGS = -version-info @HPI_LIB_VERSION@

clean:
	rm -f *~ core core.*
