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

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"baselib\"

AM_CPPFLAGS 		+= \
	@OPENHPI_INCLUDES@  -I$(top_srcdir)/transport -I$(top_srcdir)/marshal @OH_SSL_INCLUDES@

EXTRA_DIST	= Makefile.mingw32 version.rc

lib_LTLIBRARIES	        = libopenhpi.la

libopenhpi_la_SOURCES = conf.c \
                        conf.h \
                        init.cpp \
                        init.h \
                        lock.c \
                        lock.h \
                        ohpi.cpp \
                        safhpi.cpp \
                        session.cpp \
                        session.h

libopenhpi_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpi_la_LIBADD  = $(top_builddir)/transport/libopenhpitransport.la -lstdc++ \
                        $(top_builddir)/marshal/libopenhpimarshal.la \
                        $(top_builddir)/utils/libopenhpiutils.la

clean-local:
	rm -f *~ core core.* 
