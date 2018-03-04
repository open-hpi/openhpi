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

.NOTPARALLEL:

SUBDIRS			= t
DIST_SUBDIRS 		= t

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"marshal\"

EXTRA_DIST = Makefile.mingw32 version.rc

AM_CPPFLAGS 		+= @OPENHPI_INCLUDES@
# just to clear LIBS
LIBS			=

# marshal and connection used by the daemon and client library
lib_LTLIBRARIES = libopenhpimarshal.la

libopenhpimarshal_la_SOURCES    = \
    marshal_hpi.c \
    marshal_hpi.h \
    marshal_hpi_types.c \
    marshal_hpi_types.h \
    marshal.c \
    marshal.h

# we need glib-2.0 for gmalloc
libopenhpimarshal_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpimarshal_la_LIBADD  = @GLIB_ONLY_LIBS@

clean-local:
	rm -f *~ core core.*
