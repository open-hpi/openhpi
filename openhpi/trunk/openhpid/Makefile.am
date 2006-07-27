#
# Copyright (c) 2004-2005 by IBM Corporation.
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

.NOTPARALLEL:

SUBDIRS			= t
DIST_SUBDIRS 		= t

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CFLAGS		= @CFLAGS@
INCLUDES 		= @OPENHPI_INCLUDES@  -I $(top_srcdir)/marshal

#EXTRA_DIST              = README openhpid.conf openhpid.sh

#pkgconfigdir            = $(libdir)/pkgconfig
#pkgconfig_DATA          =

# daemon
sbin_PROGRAMS            = openhpid

openhpid_SOURCES        = \
	openhpid.cpp \
	$(top_srcdir)/src/config.c \
        $(top_srcdir)/src/domain.c \
        $(top_srcdir)/src/event.c \
        $(top_srcdir)/src/threaded.c \
        $(top_srcdir)/src/alarm.c \
        $(top_srcdir)/src/hotswap.c \
        $(top_srcdir)/src/lock.c \
        $(top_srcdir)/src/plugin.c \
        $(top_srcdir)/src/plugin_static.c \
        $(top_srcdir)/src/init.c \
        $(top_srcdir)/src/safhpi.c \
        $(top_srcdir)/src/session.c \
        $(top_srcdir)/src/ohpi.c	

# this is defined with ./configure --enable-daemon=static
if OPENHPID_STATIC
openhpid_LDADD          = $(top_builddir)/utils/libopenhpiutils.la \
	$(top_builddir)/marshal/libohtcpconnx.la
openhpid_LDFLAGS        = -static
else
openhpid_LDADD          = $(top_builddir)/utils/libopenhpiutils.la \
	$(top_builddir)/marshal/libohtcpconnx.la \
	-lstdc++
openhpid_LDFLAGS        =
endif


lib_LTLIBRARIES	        = libopenhpi.la

libopenhpi_la_SOURCES = \
	openhpiclient.cpp \
	openhpiclient.h

libopenhpi_la_LDFLAGS = -version-info @HPI_LIB_VERSION@ -Xcompiler -nostartfiles
libopenhpi_la_LIBADD  = $(top_builddir)/marshal/libohtcpconnx.la -lstdc++ \
                        $(top_builddir)/utils/libopenhpiutils.la


install-data-local:
	install -D -m 0755 openhpid.sh $(DESTDIR)$(sysconfdir)/init.d/openhpid

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/init.d/openhpid
	
clean-local:
	rm -f *~ core core.*
