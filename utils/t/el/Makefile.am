# (C) Copyright IBM Corp 2003, 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

REMOTE_SOURCES          = el_utils.c

MOSTLYCLEANFILES        = $(shell ls *data) $(REMOTE_SOURCES) @TEST_CLEAN@

DISTCLEANFILES          = $(top_builddir)/src/t/el/elTest.data

INCLUDES                = @OPENHPI_INCLUDES@ 

AM_CFLAGS               = @NOCFLAGS@ -ftest-coverage -fprofile-arcs

$(REMOTE_SOURCES):
	if test ! -f $@ -a ! -L $@; then \
                $(LN_S) $(top_srcdir)/utils/$@; \
        fi

TESTS = el_test_001 \
        el_test_002 \
        el_test_003 \
        el_test_004 \
	el_test_005 \
	el_test_006 \
	el_test_007 \
	el_test_008 \
	el_test_009 \
	el_test_010 \
	el_test_011 \
	el_test_012 \
	el_test_013 \
	el_test_014


check_PROGRAMS = $(TESTS)

el_test_001_SOURCES = el_test.h el_test_001.c $(REMOTE_SOURCES)
el_test_002_SOURCES = el_test.h el_test_002.c addel.c $(REMOTE_SOURCES)
el_test_003_SOURCES = el_test.h el_test_003.c addel.c $(REMOTE_SOURCES)
el_test_004_SOURCES = el_test.h el_test_004.c $(REMOTE_SOURCES)
el_test_005_SOURCES = el_test.h el_test_005.c el_compare.c $(REMOTE_SOURCES)
el_test_006_SOURCES = el_test.h el_test_006.c el_compare.c $(REMOTE_SOURCES)
el_test_007_SOURCES = el_test.h el_test_007.c $(REMOTE_SOURCES)
el_test_008_SOURCES = el_test.h el_test_008.c $(REMOTE_SOURCES)
el_test_009_SOURCES = el_test.h el_test_009.c $(REMOTE_SOURCES)
el_test_010_SOURCES = el_test.h el_test_010.c $(REMOTE_SOURCES)
el_test_011_SOURCES = el_test.h el_test_011.c $(REMOTE_SOURCES)
el_test_012_SOURCES = el_test.h el_test_012.c $(REMOTE_SOURCES)
el_test_013_SOURCES = el_test.h el_test_013.c $(REMOTE_SOURCES)
el_test_014_SOURCES = el_test.h el_test_014.c $(REMOTE_SOURCES)
