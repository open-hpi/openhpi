# (C) Copyright IBM Corp 2004
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MAINTAINERCLEANFILES 	= Makefile.in

MOSTLYCLEANFILES	= @TEST_CLEAN@

SNMPUTILSDIR    = $(top_srcdir)/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/$(SNMPDIR)

SUBDIRS 		= t

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils -I$(SNMPUTILSDIR)

AM_CFLAGS		= @CFLAGS@ @SNMPFLAGS@

pkglib_LTLIBRARIES	= libsnmp_rsa.la


EXTRA_DIST		=	rsa_resources.h \
	     			rsa_str2event.h \
	     			rsa_errorlog.h \
	     			snmp_rsa.h \
	     			snmp_rsa_control.h \
	     			snmp_rsa_discover.h \
	     			snmp_rsa_event.h \
	     			snmp_rsa_hotswap.h \
	     			snmp_rsa_inventory.h \
	     			snmp_rsa_sel.h \
	     			snmp_rsa_sensor.h \
	     			snmp_rsa_session.h \
	     			snmp_rsa_utils.h \
	     			snmp_rsa_watchdog.h \
				snmp_rsa_time.h

libsnmp_rsa_la_SOURCES	=	rsa_resources.c \
	     			rsa_str2event.c \
				sim_init.c \
	     			snmp_rsa.c \
	     			snmp_rsa_control.c \
	     			snmp_rsa_discover.c \
	     			snmp_rsa_event.c \
	     			snmp_rsa_hotswap.c \
	     			snmp_rsa_inventory.c \
	     			snmp_rsa_sel.c \
	     			snmp_rsa_sensor.c \
	     			snmp_rsa_session.c \
	     			snmp_rsa_utils.c \
	     			snmp_rsa_watchdog.c \
				snmp_rsa_time.c

libsnmp_rsa_la_LIBADD	= @SNMPLIBS@ $(SNMPUTILBDIR)/libopenhpi_snmp.la -luuid 
libsnmp_rsa_la_LDFLAGS	= -version-info @HPI_LIB_VERSION@
