#      -*- linux-c++ -*-
#
# (C) Copyright Pigeon Point Systems. 2010
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#        Anton Pak <anton.pak@pigeonpoint.com>
#

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = @OPENHPI_INCLUDES@

pkglib_LTLIBRARIES = libslave.la

libslave_la_SOURCES = baselib.cpp \
                      baselib.h \
                      handler.cpp \
                      handler.h \
                      lock.h \
                      resourcemap.cpp \
                      resourcemap.h \
                      slave.cpp \
                      util.cpp \
                      util.h

libslave_la_LIBADD =  -ldl
libslave_la_LDFLAGS = -module -version-info @HPI_LIB_VERSION@

