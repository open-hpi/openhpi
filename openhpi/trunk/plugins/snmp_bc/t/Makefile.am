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
GENERATED_CODE = event_array.c 
# event_array_rsa.c

REMOTE_SIM_SOURCES   = \
			snmp_bc.c \
			snmp_bc_control.c \
	     		snmp_bc_discover.c \
			snmp_bc_discover_bc.c \
			snmp_bc_discover_rsa.c \
			snmp_bc_event.c \
			snmp_bc_inventory.c \
			snmp_bc_hotswap.c \
			snmp_bc_resources.c \
			snmp_bc_sel.c \
			snmp_bc_sensor.c \
			snmp_bc_session.c \
	 		snmp_bc_time.c \
			snmp_bc_watchdog.c \
			snmp_bc_xml2event.c

MOSTLYCLEANFILES 	= @TEST_CLEAN@ \
			  $(GENERATED_CODE) \
			  $(REMOTE_SIM_SOURCES) \
			  uid_map

SNMPUTILSDIR            = $(top_srcdir)/$(SNMPDIR)

INCLUDES                = -I$(top_srcdir)/include \
			  -I$(top_srcdir)/utils \
			  -I$(top_srcdir)/snmp \
			  -I$(SNMPUTILSDIR) \
                          -I$(top_srcdir)/plugins/snmp_bc

AM_CFLAGS		= -g @NOCFLAGS@ @SNMPFLAGS@ -ftest-coverage -fprofile-arcs

EXTRA_DIST		= sim_resources.h openhpi.conf tsetup.h

noinst_LTLIBRARIES	= libsnmp_bc.la

$(REMOTE_SIM_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

libsnmp_bc_la_SOURCES	= $(GENERATED_CODE) \
			  $(REMOTE_SIM_SOURCES) \
			  sim_init.c \
			  sim_resources.c \
			  snmp_utils.c \
			  tsetup.c     \
			  $(top_srcdir)/snmp/snmp_derive_oid.c 

libsnmp_bc_la_LIBADD	= -luuid @SNMPLIBS@
libsnmp_bc_la_LDFLAGS	= -module -avoid-version

libsnmp_bc.la: $(libsnmp_bc_la_OBJECTS) $(libsnmp_bc_la_DEPENDENCIES)
	$(LINK) -rpath $(pkglibdir) $(libsnmp_bc_la_LDFLAGS) $(libsnmp_bc_la_OBJECTS) $(libsnmp_bc_la_LIBADD) $(LIBS)

# Keep in sync with parent directory's Makefile.am
XMLDATA_BC = $(top_srcdir)/plugins/snmp_bc/event.xml

#XMLDATA_RSA = $(top_srcdir)/plugins/snmp_bc/event_rsa.xml

$(GENERATED_CODE): $(XMLDATA_BC) $(XMLDATA_RSA) $(top_srcdir)/scripts/text2cstr.pl
	$(top_srcdir)/scripts/text2cstr.pl -s bc_eventxml $(XMLDATA_BC) > event_array.c


#	$(top_srcdir)/scripts/text2cstr.pl -s rsa_eventxml $(XMLDATA_RSA) > event_array_rsa.c

# Setup environment variables for TESTS programs
TESTS_ENVIRONMENT  = OPENHPI_CONF=$(srcdir)/openhpi.conf
#TESTS_ENVIRONMENT += OPENHPI_DEBUG=YES
#TESTS_ENVIRONMENT += OPENHPI_DEBUG_TRACE=YES
TESTS_ENVIRONMENT += LD_LIBRARY_PATH=$(top_builddir)/src/.libs:$(top_builddir)/utils/.libs
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_PATH=$(shell pwd)

TESTS = tevent \
	tcontrol_parms \
	tset_resource_tag \
	tset_resource_sev \
	tsnmp_bc_getset \
	tsensorget001 \
	tsensorget002 \
	tsensorget003 \
	tsensorget004 \
	tsensorget005 \
	tsensorget006 \
	tsensorget008 \
	tsensorget010 \
	tsensorget013 \
	tsensorget016 \
	tsensorget020 \
	tsensorget021 \
	tsensorget022 \
	tsensorget023 \
	tsensorget025 \
	tsensorget027 \
	tsensorget031 \
	tsensorget033 \
	tsensorget034 \
	tsensorget036 \
	tsensorget040 \
	tsensorget041 \
	tsensorset001 \
	tsensorset002 \
	tsensorset003 \
	tsensorset004 \
	tsensorset005 \
	tsensorset006 \
	tsensorset007 \
	tsensorset008
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
#	testxml

# Need to put expected test failures in BOTH TESTS and XFAIL_TESTS
# XFAIL_TESTS = thotswap_reset

check_PROGRAMS = $(TESTS)

# Unit test using normal IF calls and simulation library
tevent_SOURCES = tevent.c
tevent_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la

# Unit test using normal IF calls and simulation library
tcontrol_parms_SOURCES = tcontrol_parms.c
tcontrol_parms_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# Unit test using normal IF calls and simulation library
tset_resource_tag_SOURCES = tset_resource_tag.c
tset_resource_tag_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# Unit test using normal IF calls and simulation library
tset_resource_sev_SOURCES = tset_resource_sev.c
tset_resource_sev_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# Unit test using normal IF calls and simulation library
tsnmp_bc_getset_SOURCES = tsnmp_bc_getset.c
tsnmp_bc_getset_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget001_SOURCES = tsensorget001.c
tsensorget001_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget002_SOURCES = tsensorget002.c
tsensorget002_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget003_SOURCES = tsensorget003.c
tsensorget003_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget004_SOURCES = tsensorget004.c
tsensorget004_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget005_SOURCES = tsensorget005.c
tsensorget005_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget006_SOURCES = tsensorget006.c
tsensorget006_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget008_SOURCES = tsensorget008.c
tsensorget008_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget010_SOURCES = tsensorget010.c
tsensorget010_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget013_SOURCES = tsensorget013.c
tsensorget013_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget016_SOURCES = tsensorget016.c
tsensorget016_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget020_SOURCES = tsensorget020.c
tsensorget020_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget021_SOURCES = tsensorget021.c
tsensorget021_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget022_SOURCES = tsensorget022.c
tsensorget022_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget023_SOURCES = tsensorget023.c
tsensorget023_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget025_SOURCES = tsensorget025.c
tsensorget025_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget027_SOURCES = tsensorget027.c
tsensorget027_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget031_SOURCES = tsensorget031.c
tsensorget031_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget033_SOURCES = tsensorget033.c
tsensorget033_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget034_SOURCES = tsensorget034.c
tsensorget034_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget036_SOURCES = tsensorget036.c
tsensorget036_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget040_SOURCES = tsensorget040.c
tsensorget040_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorget041_SOURCES = tsensorget041.c
tsensorget041_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset001_SOURCES = tsensorset001.c
tsensorset001_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset002_SOURCES = tsensorset002.c
tsensorset002_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset003_SOURCES = tsensorset003.c
tsensorset003_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset004_SOURCES = tsensorset004.c
tsensorset004_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset005_SOURCES = tsensorset005.c
tsensorset005_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset006_SOURCES = tsensorset006.c
tsensorset006_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset007_SOURCES = tsensorset007.c
tsensorset007_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 

# 
tsensorset008_SOURCES = tsensorset008.c
tsensorset008_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_bc/t/libsnmp_bc.la 


# Unit tests with IF and snmp calls stubbed out
#testxml_SOURCES = testxml.c
#testxml_LDADD   = com_xml2event.c

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
