# (C) Copyright IBM Corp 2003, 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MAINTAINERCLEANFILES = Makefile.in

REMOTE_SOURCES          = el_utils.c

MOSTLYCLEANFILES        = $(shell ls *data) $(REMOTE_SOURCES) @TEST_CLEAN@

DISTCLEANFILES          = $(top_builddir)/src/t/el/elTest.data

INCLUDES                = @OPENHPI_INCLUDES@

AM_CFLAGS               = @NOCFLAGS@ @COV_CFLAGS@

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
        el_test_014 \
	el_test_015 \
	el_test_016 \
	el_test_017 \
	el_test_018 \
	el_test_019 \
	el_test_020 \
	el_test_021 \
        el_test_022 \
	el_test_023 \
	el_test_024 \
	el_test_025 \
	el_test_026 \
	el_test_027 \
	el_test_028 \
	el_test_029 \
	el_test_030 \
	el_test_031 \
	el_test_032 \
	el_test_033 \
	el_test_034 \
	el_test_035 \
	el_test_036 \
	el_test_037 \
	el_test_038 \
	el_test_039 \
	el_test_040 \
	el_test_041 \
	el_test_042 \
	el_test_043 \
	el_test_044 \
	el_test_045


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
el_test_015_SOURCES = el_test.h el_test_015.c $(REMOTE_SOURCES)
el_test_016_SOURCES = el_test.h el_test_016.c $(REMOTE_SOURCES)
el_test_017_SOURCES = el_test.h el_test_017.c $(REMOTE_SOURCES)
el_test_018_SOURCES = el_test.h el_test_018.c $(REMOTE_SOURCES)
el_test_019_SOURCES = el_test.h el_test_019.c $(REMOTE_SOURCES)
el_test_020_SOURCES = el_test.h el_test_020.c $(REMOTE_SOURCES)
el_test_021_SOURCES = el_test.h el_test_021.c $(REMOTE_SOURCES)
el_test_022_SOURCES = el_test.h el_test_022.c $(REMOTE_SOURCES)
el_test_023_SOURCES = el_test.h el_test_023.c $(REMOTE_SOURCES)
el_test_024_SOURCES = el_test.h el_test_024.c $(REMOTE_SOURCES)
el_test_025_SOURCES = el_test.h el_test_025.c $(REMOTE_SOURCES)
el_test_026_SOURCES = el_test.h el_test_026.c $(REMOTE_SOURCES)
el_test_027_SOURCES = el_test.h el_test_027.c $(REMOTE_SOURCES)
el_test_028_SOURCES = el_test.h el_test_028.c $(REMOTE_SOURCES)
el_test_029_SOURCES = el_test.h el_test_029.c $(REMOTE_SOURCES)
el_test_030_SOURCES = el_test.h el_test_030.c $(REMOTE_SOURCES)
el_test_031_SOURCES = el_test.h el_test_031.c $(REMOTE_SOURCES)
el_test_032_SOURCES = el_test.h el_test_032.c $(REMOTE_SOURCES)
el_test_033_SOURCES = el_test.h el_test_033.c $(REMOTE_SOURCES)
el_test_034_SOURCES = el_test.h el_test_034.c $(REMOTE_SOURCES)
el_test_035_SOURCES = el_test.h el_test_035.c $(REMOTE_SOURCES)
el_test_036_SOURCES = el_test.h el_test_036.c $(REMOTE_SOURCES)
el_test_037_SOURCES = el_test.h el_test_037.c $(REMOTE_SOURCES)
el_test_038_SOURCES = el_test.h el_test_038.c $(REMOTE_SOURCES)
el_test_039_SOURCES = el_test.h el_test_039.c $(REMOTE_SOURCES)
el_test_040_SOURCES = el_test.h el_test_040.c $(REMOTE_SOURCES)
el_test_041_SOURCES = el_test.h el_test_041.c $(REMOTE_SOURCES)
el_test_042_SOURCES = el_test.h el_test_042.c $(REMOTE_SOURCES)
el_test_043_SOURCES = el_test.h el_test_043.c $(REMOTE_SOURCES)
el_test_044_SOURCES = el_test.h el_test_044.c $(REMOTE_SOURCES)
el_test_045_SOURCES = el_test.h el_test_045.c $(REMOTE_SOURCES)