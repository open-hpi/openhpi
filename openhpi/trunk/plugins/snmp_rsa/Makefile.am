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

SNMPUTILSDIR    = $(top_srcdir)/src/$(SNMPDIR)
SNMPUTILBDIR    = $(top_builddir)/src/$(SNMPDIR)

# SUBDIRS 		= t

INCLUDES		= -I$(top_srcdir)/include -I$(SNMPUTILSDIR)

AM_CFLAGS		= @CFLAGS@ @SNMPFLAGS@ -DDEBUG

pkglib_LTLIBRARIES	= libsnmp_rsa.la

EXTRA_DIST		=	rsa_resources.h \
	     			snmp_rsa.h \
	     			snmp_rsa_discover.h \
	     			snmp_rsa_session.h

libsnmp_rsa_la_SOURCES	=	rsa_resources.c \
	     			snmp_rsa.c \
	     			snmp_rsa_discover.c \
	     			snmp_rsa_session.c

libsnmp_rsa_la_LIBADD	= -luuid @SNMPLIBS@ $(SNMPUTILBDIR)/libsnmp_util.la
libsnmp_rsa_la_LDFLAGS	= -version-info @HPI_LIB_VERSION@
