#
# Copyright (c) 2004 by FORCE Computers.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#    Thomas Kanngieser <thomas.kanngieser@fci.com>
#

REMOTE_SOURCES		= simple_config.c

MOSTLYCLEANFILES 	= $(REMOTE_SOURCES) $(shell ls *.bb*) $(shell ls *.da) $(shell ls *.gcov)

INCLUDES		= -I..

AM_CFLAGS		= @CFLAGS@ -ftest-coverage -fprofile-arcs


$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
		ln -s ../$@; \
	fi

TESTS = \
	config_000

check_PROGRAMS = $(TESTS)

config_000_SOURCES = config_000.c $(REMOTE_SOURCES)
