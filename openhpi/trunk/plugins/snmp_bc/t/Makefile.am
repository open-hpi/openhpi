#
# (C) Copyright IBM Corp 2003, 2004
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

# Generated files - need to keep in sync with parent's directory's Makefile.am
XMLDATA = bc_xmlevents.c rsa_xmlevents.c
XMLDATA_BC = $(top_srcdir)/plugins/snmp_bc/BC_STRING.XML
XMLDATA_RSA = $(top_srcdir)/plugins/snmp_bc/RSA_STRING.XML

REMOTE_SIM_SOURCES   = \
			com_xml2event.c \
			bc_resources.c \
			snmp_bc.c \
	     		snmp_bc_discover.c \
			snmp_bc_discover_bc.c \
			snmp_bc_discover_rsa.c \
			snmp_bc_event.c \
			snmp_bc_inventory.c \
			snmp_bc_sel.c \
			snmp_bc_session.c \
	 		snmp_bc_time.c \
			stubs.c

#			  snmp_bc_control.c \
#			  snmp_bc_hotswap.c \
#			  snmp_bc_sensor.c \
#			  snmp_bc_watchdog.c

MOSTLYCLEANFILES 	= @TEST_CLEAN@ \
			  $(XMLDATA) \
			  $(REMOTE_SIM_SOURCES) \
			  uid_map

SNMPUTILSDIR            = $(top_srcdir)/$(SNMPDIR)

INCLUDES                = -I$(top_srcdir)/include \
			  -I$(top_srcdir)/utils \
			  -I$(SNMPUTILSDIR) \
                          -I$(top_srcdir)/plugins/snmp_bc

AM_CFLAGS		= -g @NOCFLAGS@ @SNMPFLAGS@ -ftest-coverage -fprofile-arcs

EXTRA_DIST		= sim_resources.h

noinst_LTLIBRARIES	= libsnmp_bc.la

$(REMOTE_SIM_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

libsnmp_bc_la_SOURCES	= $(XMLDATA) \
			  $(REMOTE_SIM_SOURCES) \
			  sim_init.c \
			  sim_resources.c \
			  snmp_utils.c

libsnmp_bc_la_LIBADD	= $(top_srcdir)/snmp/snmp_derive_oid.o -luuid @SNMPLIBS@
libsnmp_bc_la_LDFLAGS	= -module -avoid-version

libsnmp_bc.la: $(libsnmp_bc_la_OBJECTS) $(libsnmp_bc_la_DEPENDENCIES)
	$(LINK) -rpath $(pkglibdir) $(libsnmp_bc_la_LDFLAGS) $(libsnmp_bc_la_OBJECTS) $(libsnmp_bc_la_LIBADD) $(LIBS)

# Keep in sync with parent directory's Makefile.am
$(XMLDATA): $(XMLDATA_BC) $(XMLDATA_RSA) $(top_srcdir)/scripts/text2cstr.pl
	$(top_srcdir)/scripts/text2cstr.pl -s bc_eventxml $(XMLDATA_BC) > bc_xmlevents.c
	$(top_srcdir)/scripts/text2cstr.pl -s rsa_eventxml $(XMLDATA_RSA) > rsa_xmlevents.c

# Setup environment variables for TESTS programs
TESTS_ENVIRONMENT  = OPENHPI_CONF=$(shell pwd)/openhpi.conf
#TESTS_ENVIRONMENT += OPENHPI_DEBUG=YES
#TESTS_ENVIRONMENT += OPENHPI_DEBUG_TRACE=YES
TESTS_ENVIRONMENT += LD_LIBRARY_PATH=$(shell pwd)/../../../src/.libs:$(shell pwd)/../../../utils/.libs
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_PATH=$(shell pwd)

TESTS = tevent
#	thotswap_state \
#	thotswap_state_err \
#	thotswap_reset \
#	thotswap_reset_err \
#	thotswap_reset_set \
#	thotswap_reset_set_err \
#	thotswap_power \
#	thotswap_power_err \
#	thotswap_power_set \
#	thotswap_power_set_err \
#	thotswap_unsupported \
#	tsensor \
#	testxml

# Need to put expected test failures in BOTH TESTS and XFAIL_TESTS
# XFAIL_TESTS = thotswap_reset

check_PROGRAMS = $(TESTS)

# Unit test using normal IF calls and simulation library
tevent_SOURCES = tevent.c
tevent_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la

# Unit tests with IF and snmp calls stubbed out
#testxml_SOURCES = testxml.c
#testxml_LDADD   = com_xml2event.c

#tsensor_SOURCES = tsensor.c
#tsensor_LDADD   = snmp_bc_sensor.o 

#thotswap_state_SOURCES = thotswap_state.c
#thotswap_state_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_state_err_SOURCES = thotswap_state_err.c
#thotswap_state_err_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_reset_SOURCES = thotswap_reset.c
#thotswap_reset_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_reset_err_SOURCES = thotswap_reset_err.c
#thotswap_reset_err_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_reset_set_SOURCES = thotswap_reset_set.c
#thotswap_reset_set_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_reset_set_err_SOURCES = thotswap_reset_set_err.c
#thotswap_reset_set_err_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_power_SOURCES = thotswap_power.c
#thotswap_power_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_power_err_SOURCES = thotswap_power_err.c
#thotswap_power_err_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_power_set_SOURCES = thotswap_power_set.c
#thotswap_power_set_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_power_set_err_SOURCES = thotswap_power_set_err.c
#thotswap_power_set_err_LDADD   = snmp_bc_hotswap.o @SNMPLIBS@

#thotswap_unsupported_SOURCES = thotswap_unsupported.c
#thotswap_unsupported_LDADD = snmp_bc_hotswap.o @SNMPLIBS@
