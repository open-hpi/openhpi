#
# (C) Copyright IBM Corp 2004
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
 
REMOTE_SIM_SOURCES 	= rsa_resources.c \
			  rsa_str2event.c \
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

MOSTLYCLEANFILES 	= @TEST_CLEAN@ \
			  $(REMOTE_SIM_SOURCES) \
			  uidmap

SNMPUTILSDIR            = $(top_srcdir)/$(SNMPDIR)

INCLUDES                = -I$(top_srcdir)/include \
			  -I$(top_srcdir)/utils \
			  -I$(SNMPUTILSDIR) \
                          -I$(top_srcdir)/plugins/snmp_rsa

AM_CFLAGS		= -g @NOCFLAGS@ @SNMPFLAGS@ -ftest-coverage -fprofile-arcs

EXTRA_DIST		= sim_resources.h \
			  openhpi.conf

noinst_LTLIBRARIES	= libsnmp_rsa.la

$(REMOTE_SIM_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

libsnmp_rsa_la_SOURCES	= sim_init.c \
			  sim_resources.c \
			  snmp_util.c \
			  $(REMOTE_SIM_SOURCES)

libsnmp_rsa_la_LIBADD	= -luuid @SNMPLIBS@

libsnmp_rsa_la_LDFLAGS	= -module -avoid-version

libsnmp_rsa.la: $(libsnmp_rsa_la_OBJECTS) $(libsnmp_rsa_la_DEPENDENCIES)
	$(LINK) -rpath $(pkglibdir) $(libsnmp_rsa_la_LDFLAGS) $(libsnmp_rsa_la_OBJECTS) $(libsnmp_rsa_la_LIBADD) $(LIBS)

# Setup environment variables for TESTS programs
TESTS_ENVIRONMENT = OPENHPI_CONF=$(srcdir)/openhpi.conf
TESTS_ENVIRONMENT += LD_LIBRARY_PATH=$(shell pwd)/../../../src/.libs:$(shell pwd)/../../../utils/.libs
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_PATH=$(shell pwd)

TESTS = tutils_conv \
	tutils_oid \
 	tevent

check_PROGRAMS = $(TESTS)

tutils_conv_SOURCES = tutils_conv.c
tutils_conv_LDADD   = snmp_rsa_utils.o

tutils_oid_SOURCES = tutils_oid.c
tutils_oid_LDADD   = snmp_rsa_utils.o

tevent_SOURCES = tevent.c
tevent_LDADD   = $(top_builddir)/utils/libopenhpiutils.la \
		 $(top_builddir)/src/libopenhpi.la \
		 $(top_builddir)/plugins/snmp_rsa/t/libsnmp_rsa.la
