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
INCLUDES 		= -I $(top_srcdir)/include -I $(top_srcdir)/utils -I $(top_srcdir)/marshal
# just to clear LIBS
LIBS			=

EXTRA_DIST              = README openhpiclient.conf

pkgconfigdir            = $(libdir)/pkgconfig
pkgconfig_DATA          = openhpiclient.pc

# daemon
bin_PROGRAMS            = openhpid

openhpid_SOURCES        = \
	openhpid.h \
	openhpid.cpp

# this is defined with ./configure --enable-daemon=static 
if OPENHPID_STATIC
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la $(top_builddir)/utils/libopenhpiutils.la
openhpid_LDFLAGS        = -static
else
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la
openhpid_LDFLAGS        =
endif

# client library
# please do not use any external libraries
# except pthread
lib_LTLIBRARIES	        = libopenhpiclient.la

libopenhpiclient_la_SOURCES = \
	client_config.h \
	openhpiclient.h \
	openhpiclient.c \
	simple_config.h \
	simple_config.c

libopenhpiclient_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpiclient_la_LIBADD  = $(top_builddir)/marshal/libopenhpimarshal.la

noinst_PROGRAMS       = event_get

# check saHpiEventGet function of the daemon
event_get_SOURCES     = event_get.c
event_get_LDADD       = libopenhpiclient.la $(top_builddir)/marshal/libopenhpimarshal.la $(top_builddir)/utils/libopenhpiutils.la
event_get_LDFLAGS     = -static


clean:
	rm -f *~ core core.*
