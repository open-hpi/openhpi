#
# Copyright (c) 2004,2005 by IBM Corporation.
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

pkglibdir = $(libdir)/openhpi/client

EXTRA_DIST = openhpid.sh

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
bin_PROGRAMS            = openhpid

openhpid_SOURCES        = \
	openhpid.cpp

# this is defined with ./configure --enable-daemon=static
if OPENHPID_STATIC
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la \
	$(top_builddir)/marshal/libohtcpconnx.la
openhpid_LDFLAGS        = -static
else
openhpid_LDADD          = $(top_builddir)/src/libopenhpi.la \
	$(top_builddir)/marshal/libohtcpconnx.la \
	-lstdc++
openhpid_LDFLAGS        =
endif


pkglib_LTLIBRARIES	        = libopenhpi.la

libopenhpi_la_SOURCES = \
	openhpiclient.cpp \
	openhpiclient.h

libopenhpi_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpi_la_LIBADD  = $(top_builddir)/marshal/libohtcpconnx.la -lstdc++

install-exec-local: $(srcdir)/openhpid.sh
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/init.d
	$(INSTALL_SCRIPT) $(srcdir)/openhpid.sh $(DESTDIR)$(sysconfdir)/init.d/openhpid

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/init.d/openhpid

clean-local:
	rm -f *~ core core.*
