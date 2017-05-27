# (C) Copyright IBM Corp 2003, 2004
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MAINTAINERCLEANFILES 	= Makefile.in

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"ssl\"

AM_CPPFLAGS		+= @OPENHPI_INCLUDES@

lib_LTLIBRARIES		= libopenhpi_ssl.la

libopenhpi_ssl_la_SOURCES = oh_ssl.c oh_ssl.h

libopenhpi_ssl_la_LDFLAGS = -version-info @HPI_LIB_VERSION@
libopenhpi_ssl_la_LIBADD  = @CRYPTO_LIB@

