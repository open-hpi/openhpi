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

MOSTLYCLEANFILES	= @TEST_CLEAN@

SNMPUTILSDIR    = $(top_srcdir)/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/$(SNMPDIR)

XMLDATA = bc_xmlevents.c rsa_xmlevents.c

SUBDIRS 		= t

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils -I$(SNMPUTILSDIR)

AM_CFLAGS		= @CFLAGS@ @SNMPFLAGS@

pkglib_LTLIBRARIES	= libsnmp_bc.la

EXTRA_DIST		=	bc_errorlog.h \
				bc_resources.h \
				snmp_bc.h \
				com_xml2event.h \
				snmp_bc_control.h \
				snmp_bc_event.h \
	     			snmp_bc_discover.h \
				snmp_bc_hotswap.h \
	     			snmp_bc_sel.h \
				snmp_bc_time.h \
				snmp_bc_sensor.h \
				snmp_bc_session.h \
				snmp_bc_inventory.h \
				snmp_bc_watchdog.h

#				bc_str2event.h \

# Generated files (e.g.bc_xmlevents.c rsa_xmlevents.c) must be listed first
libsnmp_bc_la_SOURCES	= \
                                bc_xmlevents \
                                rsa_xmlevents.c \
                                com_xml2event.c \
                                bc_resources.c \
				sim_init.c \
				snmp_bc.c \
	     			snmp_bc_discover.c \
				snmp_bc_discover_bc.c \
				snmp_bc_discover_rsa.c \
				snmp_bc_inventory.c \
				snmp_bc_session.c \
	     			snmp_bc_time.c \
				stubs.c

#				snmp_bc_control.c \
#				snmp_bc_event.c \
#				snmp_bc_hotswap.c \
#	     			snmp_bc_sel.c \
#				snmp_bc_sensor.c \
#				snmp_bc_inventory.c \
#				snmp_bc_watchdog.c

libsnmp_bc_la_LIBADD	= -luuid @SNMPLIBS@ $(SNMPUTILBDIR)/libopenhpi_snmp.la
libsnmp_bc_la_LDFLAGS	= -avoid-version

$(XMLDATA): RSA_STRING.XML BC_STRING.XML $(top_srcdir)/scripts/text2cstr.pl
	$(top_srcdir)/scripts/text2cstr.pl -s bc_eventxml BC_STRING.XML > bc_xmlevents.c
	$(top_srcdir)/scripts/text2cstr.pl -s rsa_eventxml RSA_STRING.XML > rsa_xmlevents.c

