# (C) Copyright IBM Corp 2004
# All rights reserved.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.

MOSTLYCLEANFILES 	= @TEST_CLEAN@ uid_map
EXTRA_DIST              = openhpi.conf

INCLUDES		= @OPENHPI_INCLUDES@ 

AM_CFLAGS		= @NOCFLAGS@ -ftest-coverage -fprofile-arcs

TDEPLIB                 = $(top_builddir)/src/libopenhpi.la \
			  $(top_builddir)/utils/libopenhpiutils.la

CDEPLIB                 = $(top_builddir)/openhpid2/libopenhpi.la \
			  $(top_builddir)/utils/libopenhpiutils.la

TESTS_ENVIRONMENT = OPENHPI_PATH=$(top_builddir)/plugins/dummy:$(top_builddir)/plugins/ipmi:$(top_builddir)/plugins/ipmidirect:$(top_builddir)/plugins/watchdog:$(top_builddir)/plugins/sysfs:$(top_builddir)/plugins/text_remote:$(top_builddir)/plugins/snmp_bc:$(top_builddir)/plugins/snmp_rsa
TESTS_ENVIRONMENT += OPENHPI_UID_MAP=$(shell pwd)/uid_map
TESTS_ENVIRONMENT += OPENHPI_CONF=$(top_srcdir)/src/t/ohpi/openhpi.conf

TESTS = ohpi_000 \
        ohpi_001 \
        ohpi_002 \
        ohpi_003 \
        ohpi_004 \
        ohpi_005 \
        ohpi_006 \
        ohpi_007 \
        ohpi_008 \
        ohpi_009 \
        ohpi_010 \
        ohpi_011 \
        ohpi_012 \
        ohpi_013 \
        ohpi_014 \
        ohpi_015 \
        ohpi_016 \
        ohpi_017 \
        ohpi_018 \
        ohpi_019 \
        ohpi_020 \
        ohpi_021 \
        ohpi_022 \
        ohpi_023 \
        ohpi_024 \
        ohpi_025 \
        ohpi_026 \
        ohpi_027 \
        ohpi_028 \
        ohpi_029 \
        ohpi_030 \
        ohpi_031 \
        ohpi_032 \
        ohpi_033 \
        ohpi_034 \
        ohpi_035 \
        ohpi_036 \
        ohpi_037 \
        ohpi_038 \
        ohpi_039 \
	ohpi_version \
	ohpi_version_client

check_PROGRAMS = $(TESTS)

ohpi_000_SOURCES = ohpi_000.c
ohpi_000_LDADD   = $(TDEPLIB)

ohpi_001_SOURCES = ohpi_001.c
ohpi_001_LDADD   = $(TDEPLIB)

ohpi_002_SOURCES = ohpi_002.c
ohpi_002_LDADD   = $(TDEPLIB)

ohpi_003_SOURCES = ohpi_003.c
ohpi_003_LDADD   = $(TDEPLIB)

ohpi_004_SOURCES = ohpi_004.c
ohpi_004_LDADD   = $(TDEPLIB)

ohpi_005_SOURCES = ohpi_005.c
ohpi_005_LDADD   = $(TDEPLIB)

ohpi_006_SOURCES = ohpi_006.c
ohpi_006_LDADD   = $(TDEPLIB)

ohpi_007_SOURCES = ohpi_007.c
ohpi_007_LDADD   = $(TDEPLIB)

ohpi_008_SOURCES = ohpi_008.c
ohpi_008_LDADD   = $(TDEPLIB)

ohpi_009_SOURCES = ohpi_009.c
ohpi_009_LDADD   = $(TDEPLIB)

ohpi_010_SOURCES = ohpi_010.c
ohpi_010_LDADD   = $(TDEPLIB)

ohpi_011_SOURCES = ohpi_011.c
ohpi_011_LDADD   = $(TDEPLIB)

ohpi_012_SOURCES = ohpi_012.c
ohpi_012_LDADD   = $(TDEPLIB)

ohpi_013_SOURCES = ohpi_013.c
ohpi_013_LDADD   = $(TDEPLIB)

ohpi_014_SOURCES = ohpi_014.c
ohpi_014_LDADD   = $(TDEPLIB)

ohpi_015_SOURCES = ohpi_015.c
ohpi_015_LDADD   = $(TDEPLIB)

ohpi_016_SOURCES = ohpi_016.c
ohpi_016_LDADD   = $(TDEPLIB)

ohpi_017_SOURCES = ohpi_017.c
ohpi_017_LDADD   = $(TDEPLIB)

ohpi_018_SOURCES = ohpi_018.c
ohpi_018_LDADD   = $(TDEPLIB)

ohpi_019_SOURCES = ohpi_019.c
ohpi_019_LDADD   = $(TDEPLIB)

ohpi_020_SOURCES = ohpi_020.c
ohpi_020_LDADD   = $(TDEPLIB)

ohpi_021_SOURCES = ohpi_021.c
ohpi_021_LDADD   = $(TDEPLIB)

ohpi_022_SOURCES = ohpi_022.c
ohpi_022_LDADD   = $(TDEPLIB)

ohpi_023_SOURCES = ohpi_023.c
ohpi_023_LDADD   = $(TDEPLIB)

ohpi_024_SOURCES = ohpi_024.c
ohpi_024_LDADD   = $(TDEPLIB)

ohpi_025_SOURCES = ohpi_025.c
ohpi_025_LDADD   = $(TDEPLIB)

ohpi_026_SOURCES = ohpi_026.c
ohpi_026_LDADD   = $(TDEPLIB)

ohpi_027_SOURCES = ohpi_027.c
ohpi_027_LDADD   = $(TDEPLIB)

ohpi_028_SOURCES = ohpi_028.c
ohpi_028_LDADD   = $(TDEPLIB)

ohpi_029_SOURCES = ohpi_029.c
ohpi_029_LDADD   = $(TDEPLIB)

ohpi_030_SOURCES = ohpi_030.c
ohpi_030_LDADD   = $(TDEPLIB)

ohpi_031_SOURCES = ohpi_031.c
ohpi_031_LDADD   = $(TDEPLIB)

ohpi_032_SOURCES = ohpi_032.c
ohpi_032_LDADD   = $(TDEPLIB)

ohpi_033_SOURCES = ohpi_033.c
ohpi_033_LDADD   = $(TDEPLIB)

ohpi_034_SOURCES = ohpi_034.c
ohpi_034_LDADD   = $(TDEPLIB)

ohpi_035_SOURCES = ohpi_035.c
ohpi_035_LDADD   = $(TDEPLIB)

ohpi_036_SOURCES = ohpi_036.c
ohpi_036_LDADD   = $(TDEPLIB)

ohpi_037_SOURCES = ohpi_037.c
ohpi_037_LDADD   = $(TDEPLIB)

ohpi_038_SOURCES = ohpi_038.c
ohpi_038_LDADD   = $(TDEPLIB)

ohpi_039_SOURCES = ohpi_039.c
ohpi_039_LDADD   = $(TDEPLIB)

ohpi_version_SOURCES = ohpi_version.c
ohpi_version_LDADD   = $(TDEPLIB)

ohpi_version_client_SOURCES = ohpi_version_client.c
ohpi_version_client_LDADD   = $(CDEPLIB)

