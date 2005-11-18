#      -*- linux-c -*-
#
# (C) Copyright IBM Corp. 2005
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Author(s):
#        Renier Morales <renierm@users.sf.net>
#

MAINTAINERCLEANFILES 	= Makefile.in

INCLUDES		= @OPENHPI_INCLUDES@ 

AM_CFLAGS		= @CFLAGS@

pkglib_LTLIBRARIES	= librtas.la 

librtas_la_SOURCES	= oh_rtas.c rtas_utils.c rtas_discover.c 
librtas_la_LIBADD	= -luuid
librtas_la_LDFLAGS	= -module -version-info @HPI_LIB_VERSION@ -export-dynamic

