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

SUBDIRS 		= t

INCLUDES		= -I$(top_srcdir)/include -I$(top_srcdir)/utils

AM_CFLAGS		= -g @CFLAGS@ @SNMPFLAGS@

lib_LTLIBRARIES		= libopenhpi_snmp.la

EXTRA_DIST = snmp_util.h 

libopenhpi_snmp_la_SOURCES = snmp_util.c 

libopenhpi_snmp_la_LIBADD  = -luuid @SNMPLIBS@
libopenhpi_snmp_la_LDFLAGS = -version-info @HPI_LIB_VERSION@

