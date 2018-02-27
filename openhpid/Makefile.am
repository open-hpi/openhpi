#
# Copyright (c) 2004-2006 by IBM Corporation.
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
#    Renier Morales <renier@openhpi.org>
#

.NOTPARALLEL:

MAINTAINERCLEANFILES 	= Makefile.in *~

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"openhpid\" @CRYPT_FLAG@
AM_CFLAGS = @CRYPT_FLAG@

AM_CPPFLAGS 		+= \
	@OPENHPI_INCLUDES@  -I$(top_srcdir)/transport -I$(top_srcdir)/marshal @OH_SSL_INCLUDES@

SUBDIRS = t  
DIST_SUBDIRS = t

EXTRA_DIST		= \
    openhpid.sh.in \
    Makefile.mingw32 \
    openhpid-win32.cpp \
    version.rc

# daemon library
noinst_LTLIBRARIES      = libopenhpidaemon.la
libopenhpidaemon_la_SOURCES        = \
    alarm.c \
    alarm.h \
    conf.c \
    conf.h \
    domain.c \
    event.c \
    event.h \
    hotswap.c \
    hotswap.h \
    init.c \
    init.h \
    lock.c \
    lock.h \
    ohpi.c \
    plugin.c \
    safhpi.c \
    session.c \
    threaded.c \
    threaded.h

libopenhpidaemon_la_LIBADD         = $(top_builddir)/utils/libopenhpiutils.la \
                                     @GMODULE_ONLY_LIBS@

if HAVE_OPENSSL
libopenhpidaemon_la_LIBADD += $(top_builddir)/$(SSLDIR)/libopenhpi_ssl.la
endif
libopenhpidaemon_la_LDFLAGS   = @HPI_LIB_VERSION@

# daemon
sbin_PROGRAMS           = openhpid

openhpid_SOURCES        = server.cpp server.h openhpid-posix.cpp

openhpid_LDADD         = libopenhpidaemon.la \
		                 $(top_builddir)/marshal/libopenhpimarshal.la \
						 $(top_builddir)/transport/libopenhpitransport.la

openhpid_LDFLAGS        = -export-dynamic

# this is defined with ./configure --disable-dynamic-daemon
if OPENHPID_STATIC
openhpid_LDFLAGS        += -static
else
openhpid_LDADD          += -lstdc++
endif


install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(sysconfdir)/init.d
	$(INSTALL) -m 0755 openhpid.sh $(DESTDIR)$(sysconfdir)/init.d/openhpid

uninstall-local:
	rm -f $(DESTDIR)$(sysconfdir)/init.d/openhpid
	
clean-local:
	rm -f *~ core core.* 
