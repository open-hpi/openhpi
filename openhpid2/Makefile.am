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

#SUBDIRS			= t
#DIST_SUBDIRS 		= t

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CFLAGS		= @CFLAGS@
INCLUDES 		= -I $(top_srcdir)/include -I $(top_srcdir)/utils -I $(top_srcdir)/marshal

#EXTRA_DIST              = README openhpiclient.conf

#pkgconfigdir            = $(libdir)/pkgconfig
#pkgconfig_DATA          = openhpiclient.pc

# daemon
bin_PROGRAMS            = openhpid client

openhpid_SOURCES        = \
	openhpid.cpp

client_SOURCES        = \
	client.cpp

# this is defined with ./configure --enable-daemon=static 
#if OPENHPID_STATIC
#openhpid_LDADD = $(top_builddir)/marshal/libohtcpconnx.la
#openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la $(top_builddir)/utils/libopenhpiutils.la
#openhpid_LDFLAGS        = -static
client_LDADD            = $(top_builddir)/marshal/libohtcpconnx.la
#client_LDADD            = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la $(top_builddir)/utils/libopenhpiutils.la
#client_LDFLAGS          = -static
#else
openhpid_LDADD = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libohtcpconnx.la
#openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la
openhpid_LDFLAGS        =
client_LDADD            = $(top_builddir)/marshal/libohtcpconnx.la
#client_LDADD            = $(top_builddir)/src/libopenhpi.la $(top_builddir)/marshal/libopenhpimarshal.la
client_LDFLAGS          =
#endif


clean-local:
	rm -f *~ core core.*
