#
# (C) Copyright IBM Corp 2003
# All rights reserved.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
 
MAINTAINERCLEANFILES 	= Makefile.in

SNMPUTILSDIR    = $(top_srcdir)/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/$(SNMPDIR)

EXTRA_DIST 		= \
			  snmp_client.h \
			  snmp_client_res.h \
			  snmp_client_utils.h \
			  sc_sensor_data.h

INCLUDES		= @OPENHPI_INCLUDES@  -I$(SNMPUTILSDIR)

AM_CFLAGS		= -g @CFLAGS@ @SNMPFLAGS@

pkglib_LTLIBRARIES		= libsnmp_client.la

libsnmp_client_la_SOURCES	= \
				  snmp_client.c \
				  snmp_client_discover.c \
				  snmp_client_get_rdrs.c \
				  snmp_client_gen_evts.c \
				  snmp_client_utils.c \
				  sc_sensor_data.c
				  
libsnmp_client_la_LIBADD	= -luuid @SNMPLIBS@ $(SNMPUTILSDIR)/libopenhpi_snmp.la
libsnmp_client_la_LDFLAGS	= -avoid-version
