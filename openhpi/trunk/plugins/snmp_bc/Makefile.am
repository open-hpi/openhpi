# (C) Copyright IBM Corp 2003, 2004
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MAINTAINERCLEANFILES	= Makefile.in

# Note same rule and files are generated in the t sub-directory - Make sure
# you change the t/Makefile.am, if you change these
GENERATED_CODE = event_array.c event_array_rsa.c

MOSTLYCLEANFILES = @TEST_CLEAN@ $(GENERATED_CODE)

SNMPUTILSDIR    = $(top_srcdir)/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/$(SNMPDIR)

SUBDIRS 		= t

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils -I$(SNMPUTILSDIR)

AM_CFLAGS		= @CFLAGS@ @SNMPFLAGS@

pkglib_LTLIBRARIES	= libsnmp_bc.la

EXTRA_DIST		=	snmp_bc.h \
				snmp_bc_control.h \
	     			snmp_bc_discover.h \
				snmp_bc_errorlog.h \
				snmp_bc_event.h \
				snmp_bc_hotswap.h \
				snmp_bc_inventory.h \
				snmp_bc_plugin.h \
				snmp_bc_resources.h \
	     			snmp_bc_sel.h \
				snmp_bc_sensor.h \
				snmp_bc_session.h \
				snmp_bc_time.h \
				snmp_bc_watchdog.h \
				snmp_bc_xml2event.h \
				sim_init.h

# Generated files must be listed first
libsnmp_bc_la_SOURCES	= \
				$(GENERATED_CODE) \
				sim_init.c \
				snmp_bc.c \
	     			snmp_bc_discover.c \
				snmp_bc_discover_bc.c \
				snmp_bc_discover_rsa.c \
				snmp_bc_event.c \
				snmp_bc_inventory.c \
                                snmp_bc_resources.c \
	     			snmp_bc_sel.c \
				snmp_bc_session.c \
	     			snmp_bc_time.c \
                                snmp_bc_xml2event.c \
				stubs.c

# FIXME:: Add these when ported over
#				snmp_bc_control.c \
#				snmp_bc_hotswap.c \
#				snmp_bc_sensor.c \
#				snmp_bc_watchdog.c

libsnmp_bc_la_LIBADD	= -luuid @SNMPLIBS@ $(SNMPUTILBDIR)/libopenhpi_snmp.la
libsnmp_bc_la_LDFLAGS	= -avoid-version

# Note same rule and files are generated in the t sub-directory - Make sure
# you change the t/Makefile.am, if you change these
XMLDATA_BC = $(top_srcdir)/plugins/snmp_bc/event.xml
XMLDATA_RSA = $(top_srcdir)/plugins/snmp_bc/event_rsa.xml

$(GENERATED_CODE): $(XMLDATA_BC) $(XMLDATA_RSA) $(top_srcdir)/scripts/text2cstr.pl
	$(top_srcdir)/scripts/text2cstr.pl -s bc_eventxml $(XMLDATA_BC) > event_array.c
	$(top_srcdir)/scripts/text2cstr.pl -s rsa_eventxml $(XMLDATA_RSA) > event_array_rsa.c

