#      -*- linux-c -*-
#
# (C) Copyright IBM Corp. 2005
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#        Christina Hernandez <hernanc@us.ibm.com>
#        W. David Ashley <dashley@us.ibm.com>
#

SUBDIRS = t

MAINTAINERCLEANFILES 	= Makefile.in

INCLUDES		= @OPENHPI_INCLUDES@

AM_CFLAGS		= -g @CFLAGS@

pkglib_LTLIBRARIES	= libsimulator.la

libsimulator_la_LIBADD	= -luuid
libsimulator_la_SOURCES	= sim_init.c sim_init.h \
                          sim_resources.c sim_resources.h \
			  sim_sensors.c sim_sensors.h \
			  sim_sensor_func.c sim_sensor_func.h \
			  sim_controls.c sim_controls.h \
			  sim_control_func.c sim_control_func.h \
			  sim_injector.c sim_injector.h
libsimulator_la_LIBADD	= -luuid
libsimulator_la_LDFLAGS	= -version-info @HPI_LIB_VERSION@
