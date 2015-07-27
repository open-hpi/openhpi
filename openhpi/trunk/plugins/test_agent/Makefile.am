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

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"test_agent\" -D__STDC_FORMAT_MACROS

AM_CPPFLAGS += @OPENHPI_INCLUDES@

EXTRA_DIST = Makefile.mingw32 version.rc

pkglib_LTLIBRARIES = libtest_agent.la

libtest_agent_la_SOURCES = abi.cpp \
                           abi.h \
                           announcement.cpp \
                           announcement.h \
                           annunciator.cpp \
                           annunciator.h \
                           area.cpp \
                           area.h \
                           bank.cpp \
                           bank.h \
                           codec.cpp \
                           codec.h \
                           console.cpp \
                           console.h \
                           control.cpp \
                           control.h \
                           dimi.cpp \
                           dimi.h \
                           fumi.cpp \
                           fumi.h \
                           field.cpp \
                           field.h \
                           handler.cpp \
                           handler.h \
                           instrument.cpp \
                           instrument.h \
                           instruments.cpp \
                           instruments.h \
                           inventory.cpp \
                           inventory.h \
                           log.cpp \
                           log.h \
                           object.cpp \
                           object.h \
                           resource.cpp \
                           resource.h \
                           sensor.cpp \
                           sensor.h \
                           server.cpp \
                           server.h \
                           structs.cpp \
                           structs.h \
                           test.cpp \
                           test.h \
                           timers.cpp \
                           timers.h \
                           utils.cpp \
                           utils.h \
                           vars.cpp \
                           vars.h \
                           watchdog.cpp \
                           watchdog.h

libtest_agent_la_LDFLAGS = -module -version-info @HPI_LIB_VERSION@
libtest_agent_la_LIBADD  = @GMODULE_ONLY_LIBS@

