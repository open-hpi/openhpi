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

# Note same rule and files are generated in the t sub-directory - Make sure
# you change the t/Makefile.am, if you change these
GENERATED_EVENT_HEADER_FILES = \
				el.h \
				el2event.h

GENERATED_EVENT_XML_MAP = event.xml
GENERATED_EVENT_CODE = el2event.c

GENERATED_CODE = $(GENERATED_EVENT_HEADER_FILES) $(GENERATED_EVENT_XML_MAP) $(GENERATED_EVENT_CODE)

MOSTLYCLEANFILES = @TEST_CLEAN@ $(GENERATED_CODE)

SNMPUTILSDIR    = $(top_srcdir)/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/$(SNMPDIR)

SUBDIRS		= t

INCLUDES	= @OPENHPI_INCLUDES@ -I$(top_srcdir)/snmp -I$(SNMPUTILSDIR)

AM_CFLAGS	= @CFLAGS@ @SNMPFLAGS@

pkglib_LTLIBRARIES = libsnmp_bc.la

EXTRA_DIST = \
		$(GENERATED_EVENT_HEADER_FILES) \
		eventmap2code.pl \
		snmp_bc_event.map \
		snmp_bc.h \
		snmp_bc_control.h \
		snmp_bc_discover.h \
		snmp_bc_event.h \
		snmp_bc_hotswap.h \
		snmp_bc_inventory.h \
		snmp_bc_plugin.h \
		snmp_bc_power.h \
		snmp_bc_reset.h \
		snmp_bc_resources.h \
	     	snmp_bc_sel.h \
		snmp_bc_sensor.h \
		snmp_bc_session.h \
		snmp_bc_time.h \
		snmp_bc_utils.h \
		snmp_bc_watchdog.h \
		sim_init.h

# Generated files must be listed first
libsnmp_bc_la_SOURCES = \
			$(GENERATED_EVENT_CODE) \
			sim_init.c \
			snmp_bc.c \
			snmp_bc_control.c \
			snmp_bc_discover.c \
			snmp_bc_discover_bc.c \
			snmp_bc_discover_rsa.c \
			snmp_bc_event.c \
			snmp_bc_hotswap.c \
			snmp_bc_inventory.c \
			snmp_bc_power.c \
			snmp_bc_reset.c \
			snmp_bc_resources.c \
			snmp_bc_resources_rsa.c \
	    		snmp_bc_sel.c \
			snmp_bc_sensor.c \
			snmp_bc_session.c \
	     		snmp_bc_time.c \
	     		snmp_bc_utils.c \
			snmp_bc_watchdog.c \
			snmp_bc_xml2event.c

libsnmp_bc_la_LIBADD	= -luuid @SNMPLIBS@ $(SNMPUTILBDIR)/libopenhpi_snmp.la
libsnmp_bc_la_LDFLAGS	= -avoid-version

# Note same rules and files are generated in the t sub-directory - Make sure
# you change the t/Makefile.am, if you change these
EVENT_MAP_FILE = $(top_srcdir)/plugins/snmp_bc/snmp_bc_event.map
EVENT_MAP_SCRIPT = $(top_srcdir)/plugins/snmp_bc/eventmap2code.pl
EVENT_XML_MAP_SCRIPT = $(top_srcdir)/scripts/text2cstr.pl

$(GENERATED_EVENT_CODE): $(GENERATED_EVENT_HEADER_FILES)
	$(EVENT_XML_MAP_SCRIPT) -s eventxml $(GENERATED_EVENT_XML_MAP) > $(GENERATED_EVENT_CODE)

$(GENERATED_EVENT_HEADER_FILES): $(EVENT_MAP_FILE) $(EVENT_MAP_SCRIPT)
	$(EVENT_MAP_SCRIPT) -xml -idir $(top_srcdir)/plugins/snmp_bc -mapfile snmp_bc_event.map
