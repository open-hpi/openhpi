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

SUBDIRS			= t
DIST_SUBDIRS 		= t

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CFLAGS		= @CFLAGS@
INCLUDES 		= -I $(top_srcdir)/include -I $(top_srcdir)/utils -I $(top_srcdir)/marshal

#EXTRA_DIST              = README openhpiclient.conf openhpid.conf

#pkgconfigdir            = $(libdir)/pkgconfig
#pkgconfig_DATA          =

# daemon
bin_PROGRAMS            = openhpid

openhpid_SOURCES        = \
	openhpid.cpp

# this is defined with ./configure --enable-daemon=static
if OPENHPID_STATIC
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libohtcpconnx.la
openhpid_LDFLAGS        = -static
else
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libohtcpconnx.la
openhpid_LDFLAGS        =
endif


lib_LTLIBRARIES	        = libclient.la

libclient_la_SOURCES = \
	client.cpp \
	client.h

libclient_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libclient_la_LIBADD  = $(top_builddir)/marshal/libohtcpconnx.la

clean-local:
	rm -f *~ core core.*
