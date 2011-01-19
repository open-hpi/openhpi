/*      -*- c++ -*-
 *
 * Copyright (c) 2011 by Pigeon Point Systems.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Anton Pak <avpak@pigeonpoint.com>
 *
 */

#include <string>

#include <glib.h>

#include <oh_utils.h>

#include "flags.h"
#include "hpi_xml_writer.h"


/****************************
 * Flags Types
 ***************************/
static Flags::Names NamesSaHpiEventStateT =
{
    /* bit 0,  0x00000001 */  "STATE_00",
    /* bit 1,  0x00000002 */  "STATE_01",
    /* bit 2,  0x00000004 */  "STATE_02",
    /* bit 3,  0x00000008 */  "STATE_03",
    /* bit 4,  0x00000010 */  "STATE_04",
    /* bit 5,  0x00000020 */  "STATE_05",
    /* bit 6,  0x00000040 */  "STATE_06",
    /* bit 7,  0x00000080 */  "STATE_07",
    /* bit 8,  0x00000100 */  "STATE_08",
    /* bit 9,  0x00000200 */  "STATE_09",
    /* bit 10, 0x00000400 */  "STATE_10",
    /* bit 11, 0x00000800 */  "STATE_11",
    /* bit 12, 0x00001000 */  "STATE_12",
    /* bit 13, 0x00002000 */  "STATE_13",
    /* bit 14, 0x00004000 */  "STATE_14",
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiSensorRangeFlagsT =
{
    /* bit 0,  0x00000001 */  "NOMINAL",
    /* bit 1,  0x00000002 */  "NORMAL_MAX",
    /* bit 2,  0x00000004 */  "NORMAL_MIN",
    /* bit 3,  0x00000008 */  "MAX",
    /* bit 4,  0x00000010 */  "MIN",
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiSensorThdMaskT =
{
    /* bit 0,  0x00000001 */  "LOW_MINOR",
    /* bit 1,  0x00000002 */  "LOW_MAJOR",
    /* bit 2,  0x00000004 */  "LOW_CRIT",
    /* bit 3,  0x00000008 */  "UP_MINOR",
    /* bit 4,  0x00000010 */  "UP_MAJOR",
    /* bit 5,  0x00000020 */  "UP_CRIT",
    /* bit 6,  0x00000040 */  "UP_HYSTERESIS",
    /* bit 7,  0x00000080 */  "LOW_HYSTERESIS",
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiWatchdogExpFlagsT =
{
    /* bit 0,  0x00000001 */  0,
    /* bit 1,  0x00000002 */  "BIOS_FRB2",
    /* bit 2,  0x00000004 */  "BIOS_POST",
    /* bit 3,  0x00000008 */  "OS_LOAD",
    /* bit 4,  0x00000010 */  "SMS_OS",
    /* bit 5,  0x00000020 */  "OEM",
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiDimiTestCapabilityT =
{
    /* bit 0,  0x00000001 */  "RESULTSOUTPUT",
    /* bit 1,  0x00000002 */  "SERVICEMODE",
    /* bit 2,  0x00000004 */  "LOOPCOUNT",
    /* bit 3,  0x00000008 */  "LOOPTIME",
    /* bit 4,  0x00000010 */  "LOGGING",
    /* bit 5,  0x00000020 */  "TESTCANCEL",
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiFumiLogicalBankStateFlagsT =
{
    /* bit 0,  0x00000001 */  "NO_MAIN_PERSISTENT_COPY",
    /* bit 1,  0x00000002 */  0,
    /* bit 2,  0x00000004 */  0,
    /* bit 3,  0x00000008 */  0,
    /* bit 4,  0x00000010 */  0,
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiFumiProtocolT =
{
    /* bit 0,  0x00000001 */  "TFTP",
    /* bit 1,  0x00000002 */  "FTP",
    /* bit 2,  0x00000004 */  "HTTP",
    /* bit 3,  0x00000008 */  "LDAP",
    /* bit 4,  0x00000010 */  "LOCAL",
    /* bit 5,  0x00000020 */  "NFS",
    /* bit 6,  0x00000040 */  "DBACCESS",
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiFumiCapabilityT =
{
    /* bit 0,  0x00000001 */  "ROLLBACK",
    /* bit 1,  0x00000002 */  "BANKCOPY",
    /* bit 2,  0x00000004 */  "BANKREORDER",
    /* bit 3,  0x00000008 */  "BACKUP",
    /* bit 4,  0x00000010 */  "TARGET_VERIFY",
    /* bit 5,  0x00000020 */  "TARGET_VERIFY_MAIN",
    /* bit 6,  0x00000040 */  "COMPONENTS",
    /* bit 7,  0x00000080 */  "AUTOROLLBACK",
    /* bit 8,  0x00000100 */  "AUTOROLLBACK_CAN_BE_DISABLED",
    /* bit 9,  0x00000200 */  "MAIN_NOT_PERSISTENT",
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiSensorOptionalDataT =
{
    /* bit 0,  0x00000001 */  "TRIGGER_READING",
    /* bit 1,  0x00000002 */  "TRIGGER_THRESHOLD",
    /* bit 2,  0x00000004 */  "OEM",
    /* bit 3,  0x00000008 */  "PREVIOUS_STATE",
    /* bit 4,  0x00000010 */  "CURRENT_STATE",
    /* bit 5,  0x00000020 */  "SENSOR_SPECIFIC",
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiSensorEnableOptDataT =
{
    /* bit 0,  0x00000001 */  0,
    /* bit 1,  0x00000002 */  0,
    /* bit 2,  0x00000004 */  0,
    /* bit 3,  0x00000008 */  0,
    /* bit 4,  0x00000010 */  "CURRENT_STATE",
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  "ALARM_STATES",
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiEvtQueueStatusT =
{
    /* bit 0,  0x00000001 */  "OVERFLOW",
    /* bit 1,  0x00000002 */  0,
    /* bit 2,  0x00000004 */  0,
    /* bit 3,  0x00000008 */  0,
    /* bit 4,  0x00000010 */  0,
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiCapabilitiesT =
{
    /* bit 0,  0x00000001 */  "SENSOR",
    /* bit 1,  0x00000002 */  "RDR",
    /* bit 2,  0x00000004 */  "EVENT_LOG",
    /* bit 3,  0x00000008 */  "INVENTORY_DATA",
    /* bit 4,  0x00000010 */  "RESET",
    /* bit 5,  0x00000020 */  "POWER",
    /* bit 6,  0x00000040 */  "ANNUNCIATOR",
    /* bit 7,  0x00000080 */  "LOAD_ID",
    /* bit 8,  0x00000100 */  "FRU",
    /* bit 9,  0x00000200 */  "CONTROL",
    /* bit 10, 0x00000400 */  "WATCHDOG",
    /* bit 11, 0x00000800 */  "MANAGED_HOTSWAP",
    /* bit 12, 0x00001000 */  "CONFIGURATION",
    /* bit 13, 0x00002000 */  "AGGREGATE_STATUS",
    /* bit 14, 0x00004000 */  "DIMI",
    /* bit 15, 0x00008000 */  "EVT_DEASSERTS",
    /* bit 16, 0x00010000 */  "FUMI",
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  "RESOURCE",
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiHsCapabilitiesT =
{
    /* bit 0,  0x00000001 */  0,
    /* bit 1,  0x00000002 */  0,
    /* bit 2,  0x00000004 */  0,
    /* bit 3,  0x00000008 */  0,
    /* bit 4,  0x00000010 */  0,
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  "AUTOINSERT_IMMEDIATE",
    /* bit 30, 0x40000000 */  "INDICATOR_SUPPORTED",
    /* bit 31, 0x80000000 */  "AUTOEXTRACT_READ_ONLY"
};

static Flags::Names NamesSaHpiDomainCapabilitiesT =
{
    /* bit 0,  0x00000001 */  "AUTOINSERT_READ_ONLY",
    /* bit 1,  0x00000002 */  0,
    /* bit 2,  0x00000004 */  0,
    /* bit 3,  0x00000008 */  0,
    /* bit 4,  0x00000010 */  0,
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};

static Flags::Names NamesSaHpiEventLogCapabilitiesT =
{
    /* bit 0,  0x00000001 */  "ENTRY_ADD",
    /* bit 1,  0x00000002 */  "CLEAR",
    /* bit 2,  0x00000004 */  "TIME_SET",
    /* bit 3,  0x00000008 */  "STATE_SET",
    /* bit 4,  0x00000010 */  "OVERFLOW_RESET",
    /* bit 5,  0x00000020 */  0,
    /* bit 6,  0x00000040 */  0,
    /* bit 7,  0x00000080 */  0,
    /* bit 8,  0x00000100 */  0,
    /* bit 9,  0x00000200 */  0,
    /* bit 10, 0x00000400 */  0,
    /* bit 11, 0x00000800 */  0,
    /* bit 12, 0x00001000 */  0,
    /* bit 13, 0x00002000 */  0,
    /* bit 14, 0x00004000 */  0,
    /* bit 15, 0x00008000 */  0,
    /* bit 16, 0x00010000 */  0,
    /* bit 17, 0x00020000 */  0,
    /* bit 18, 0x00040000 */  0,
    /* bit 19, 0x00080000 */  0,
    /* bit 20, 0x00100000 */  0,
    /* bit 21, 0x00200000 */  0,
    /* bit 22, 0x00400000 */  0,
    /* bit 23, 0x00800000 */  0,
    /* bit 24, 0x01000000 */  0,
    /* bit 25, 0x02000000 */  0,
    /* bit 26, 0x04000000 */  0,
    /* bit 27, 0x08000000 */  0,
    /* bit 28, 0x10000000 */  0,
    /* bit 29, 0x20000000 */  0,
    /* bit 30, 0x40000000 */  0,
    /* bit 31, 0x80000000 */  0
};


/***************************************************
 * class cHpiXmlWriter
 ***************************************************/
cHpiXmlWriter::cHpiXmlWriter( int indent_step, bool use_names )
    : cXmlWriter( "http://openhpi.org/SAI-HPI-B.03.02",
                  "http://openhpi.org hpixml.xsd",
                  indent_step,
                  use_names )
{
    // empty
}

void cHpiXmlWriter::Begin( void )
{
    cXmlWriter::Begin( "HPI" );
}

void cHpiXmlWriter::End( void )
{
    cXmlWriter::End( "HPI" );
}

void cHpiXmlWriter::VersionNode( const SaHpiVersionT& ver )
{
    NodeSaHpiVersionT( "Version", ver );
}

void cHpiXmlWriter::BeginDomainNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "Domain" );
    NodeSaHpiDomainIdT( "DomainId", di.DomainId );
    NodeSaHpiDomainCapabilitiesT( "DomainCapabilities", di.DomainCapabilities );
    NodeSaHpiBoolT( "IsPeer", di.IsPeer );
    NodeSaHpiTextBufferT( "DomainTag", di.DomainTag );
    NodeSaHpiGuidT( "Guid", di.Guid );
}

void cHpiXmlWriter::EndDomainNode()
{
    cXmlWriter::EndNode( "Domain" );
}

void cHpiXmlWriter::BeginDrtNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "DomainReferenceTable" );
    NodeSaHpiUint32T( "DrtUpdateCount", di.DrtUpdateCount );
    NodeSaHpiTimeT( "DrtUpdateTimestamp", di.DrtUpdateTimestamp );
}

void cHpiXmlWriter::EndDrtNode()
{
    cXmlWriter::EndNode( "DomainReferenceTable" );
}

void cHpiXmlWriter::DrtEntryNode( const SaHpiDrtEntryT& drte )
{
    cXmlWriter::BeginNode( "Reference" );
    NodeSaHpiDomainIdT( "DomainId", drte.DomainId );
    NodeSaHpiBoolT( "IsPeer", drte.IsPeer );
    cXmlWriter::EndNode( "Reference" );
}

void cHpiXmlWriter::BeginRptNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "ResourcePresenceTable" );
    NodeSaHpiUint32T( "RptUpdateCount", di.RptUpdateCount );
    NodeSaHpiTimeT( "RptUpdateTimestamp", di.RptUpdateTimestamp );
}

void cHpiXmlWriter::EndRptNode()
{
    cXmlWriter::EndNode( "ResourcePresenceTable" );
}

void cHpiXmlWriter::BeginResourceNode( const SaHpiRptEntryT& rpte,
                                       SaHpiUint32T rdr_update_count )
{
    cXmlWriter::BeginNode( "Resource" );
    NodeSaHpiRptEntryT( "RptEntry", rpte );
    NodeSaHpiUint32T( "RdrUpdateCount", rdr_update_count );
}

void cHpiXmlWriter::EndResourceNode()
{
    cXmlWriter::EndNode( "Resource" );
}

void cHpiXmlWriter::BeginInstrumentNode( const SaHpiRdrT& rdr )
{
    cXmlWriter::BeginNode( "Instrument" );
    NodeSaHpiRdrT( "Rdr", rdr );
}

void cHpiXmlWriter::EndInstrumentNode()
{
    cXmlWriter::EndNode( "Instrument" );
}

void cHpiXmlWriter::BeginEventLogNode( SaHpiResourceIdT rid,
                                       const SaHpiEventLogInfoT& info,
                                       const SaHpiEventLogCapabilitiesT& caps )
{
    if ( rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        cXmlWriter::BeginNode( "DomainEventLog" );
    } else {
        cXmlWriter::BeginNode( "EventLog" );
    }

    NodeSaHpiUint32T( "Entries", info.Entries );
    NodeSaHpiUint32T( "Size", info.Size );
    NodeSaHpiUint32T( "UserEventMaxSize", info.UserEventMaxSize );
    NodeSaHpiTimeT( "UpdateTimestamp", info.UpdateTimestamp );
    NodeSaHpiTimeT( "CurrentTime", info.CurrentTime );
    NodeSaHpiBoolT( "Enabled", info.Enabled );
    NodeSaHpiBoolT( "OverflowFlag", info.OverflowFlag );
    NodeSaHpiBoolT( "OverflowResetable", info.OverflowResetable );
    NodeSaHpiEventLogOverflowActionT( "OverflowAction", info.OverflowAction );
    NodeSaHpiEventLogCapabilitiesT( "Capabilities", caps );
}

void cHpiXmlWriter::EndEventLogNode( SaHpiResourceIdT rid )
{
    if ( rid == SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        cXmlWriter::EndNode( "DomainEventLog" );
    } else {
        cXmlWriter::EndNode( "EventLog" );
    }
}

void cHpiXmlWriter::LogEntryNode( const SaHpiEventLogEntryT& entry,
                                  const SaHpiRdrT& rdr,
                                  const SaHpiRptEntryT& rpte )
{
    cXmlWriter::BeginNode( "Entry" );
    NodeSaHpiTimeT( "Timestamp", entry.Timestamp );
    NodeSaHpiEventT( "Event", entry.Event );
    if ( rdr.RdrType != SAHPI_NO_RECORD ) {
        NodeSaHpiRdrT( "Rdr", rdr );
    }
    if ( rpte.ResourceId != SAHPI_UNSPECIFIED_RESOURCE_ID ) {
        if ( rpte.ResourceCapabilities != 0 ) {
            NodeSaHpiRptEntryT( "RptEntry", rpte );
        }
    }
    cXmlWriter::EndNode( "Entry" );
}

void cHpiXmlWriter::BeginDatNode( const SaHpiDomainInfoT& di )
{
    cXmlWriter::BeginNode( "DomainAlarmTable" );
    NodeSaHpiUint32T( "DatUpdateCount", di.DatUpdateCount );
    NodeSaHpiTimeT( "DatUpdateTimestamp", di.DatUpdateTimestamp );
    NodeSaHpiUint32T( "ActiveAlarms", di.ActiveAlarms );
    NodeSaHpiUint32T( "CriticalAlarms", di.CriticalAlarms );
    NodeSaHpiUint32T( "MajorAlarms", di.MajorAlarms );
    NodeSaHpiUint32T( "MinorAlarms", di.MinorAlarms );
    NodeSaHpiUint32T( "DatUserAlarmLimit", di.DatUserAlarmLimit );
    NodeSaHpiBoolT( "DatOverflow", di.DatOverflow );
}

void cHpiXmlWriter::EndDatNode()
{
    cXmlWriter::EndNode( "DomainAlarmTable" );
}

void cHpiXmlWriter::AlarmNode( const SaHpiAlarmT& a )
{
    cXmlWriter::BeginNode( "Alarm" );
    NodeSaHpiAlarmIdT( "AlarmId", a.AlarmId );
    NodeSaHpiTimeT( "Timestamp", a.Timestamp );
    NodeSaHpiSeverityT( "Severity", a.Severity );
    NodeSaHpiBoolT( "Acknowledged", a.Acknowledged );
    NodeSaHpiConditionT( "AlarmCond", a.AlarmCond );
    cXmlWriter::EndNode( "Alarm" );
}


/****************************
 * Basic Types
 ***************************/
void cHpiXmlWriter::NodeSaHpiUint8T(
    const char * name,
    const SaHpiUint8T& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiUint16T(
    const char * name,
    const SaHpiUint16T& x )

{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiUint32T(
    const char * name,
    const SaHpiUint32T& x )
{
    cXmlWriter::Node( name, "%u", x );
}

void cHpiXmlWriter::NodeSaHpiUint64T(
    const char * name,
    const SaHpiUint64T& x )
{
    cXmlWriter::Node( name, "%llu", x );
}

void cHpiXmlWriter::NodeSaHpiInt8T(
    const char * name,
    const SaHpiInt8T& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInt16T(
    const char * name,
    const SaHpiInt16T& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInt32T(
    const char * name,
    const SaHpiInt32T& x )
{
    cXmlWriter::Node( name, "%d", x );
}

void cHpiXmlWriter::NodeSaHpiInt64T(
    const char * name,
    const SaHpiInt64T& x )
{
    cXmlWriter::Node( name, "%lld", x );
}

void cHpiXmlWriter::NodeSaHpiFloat64T(
    const char * name,
    const SaHpiFloat64T& x )
{
    cXmlWriter::Node( name, "%f", x );
}


/****************************
 * Typedefs
 ***************************/
void cHpiXmlWriter::NodeSaHpiBoolT(
    const char * name,
    const SaHpiBoolT& x )
{
    cXmlWriter::Node( name, "%s", ( x == SAHPI_FALSE ) ? "false" : "true" );
}

void cHpiXmlWriter::NodeSaHpiManufacturerIdT(
    const char * name,
    const SaHpiManufacturerIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiVersionT(
    const char * name,
    const SaHpiVersionT& x )
{
    SaHpiUint8T comp  = ( x >> 16 ) & 0xFF;
    SaHpiUint8T major = ( x >> 8 ) & 0xFF;
    SaHpiUint8T minor = x & 0xFF;
    cXmlWriter::Node( name, "%c.%02u.%02u", 'A' - 1 + comp, major, minor );
}

void cHpiXmlWriter::NodeSaHpiDomainIdT(
    const char * name,
    const SaHpiDomainIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiSessionIdT(
    const char * name,
    const SaHpiSessionIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiResourceIdT(
    const char * name,
    const SaHpiResourceIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEntryIdT(
    const char * name,
    const SaHpiEntryIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTimeT(
    const char * name,
    const SaHpiTimeT& x )
{
    NodeSaHpiInt64T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTimeoutT(
    const char * name,
    const SaHpiTimeoutT& x )
{
    NodeSaHpiInt64T( name, x );
}

void cHpiXmlWriter::NodeSaHpiInstrumentIdT(
    const char * name,
    const SaHpiInstrumentIdT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEntityLocationT(
    const char * name,
    const SaHpiEntityLocationT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventStateT(
    const char * name,
    const SaHpiEventStateT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiEventStateT );
}

void cHpiXmlWriter::NodeSaHpiSensorNumT(
    const char * name,
    const SaHpiSensorNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiSensorRangeFlagsT(
    const char * name,
    const SaHpiSensorRangeFlagsT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiSensorRangeFlagsT );
}

void cHpiXmlWriter::NodeSaHpiSensorThdMaskT(
    const char * name,
    const SaHpiSensorThdMaskT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiSensorThdMaskT );
}

void cHpiXmlWriter::NodeSaHpiCtrlNumT(
    const char * name,
    const SaHpiCtrlNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateDiscreteT(
    const char * name,
    const SaHpiCtrlStateDiscreteT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateAnalogT(
    const char * name,
    const SaHpiCtrlStateAnalogT& x )
{
    NodeSaHpiInt32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiTxtLineNumT(
    const char * name,
    const SaHpiTxtLineNumT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiIdrIdT(
    const char * name,
    const SaHpiIdrIdT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiWatchdogNumT(
    const char * name,
    const SaHpiWatchdogNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiWatchdogExpFlagsT(
    const char * name,
    const SaHpiWatchdogExpFlagsT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiWatchdogExpFlagsT );
}

void cHpiXmlWriter::NodeSaHpiDimiNumT(
    const char * name,
    const SaHpiDimiNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiTestCapabilityT(
    const char * name,
    const SaHpiDimiTestCapabilityT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiDimiTestCapabilityT );
}

void cHpiXmlWriter::NodeSaHpiDimiTestNumT(
    const char * name,
    const SaHpiDimiTestNumT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiDimiTestPercentCompletedT(
    const char * name,
    const SaHpiDimiTestPercentCompletedT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiNumT(
    const char * name,
    const SaHpiFumiNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiBankNumT(
    const char * name,
    const SaHpiBankNumT& x )
{
    NodeSaHpiUint8T( name, x );
}

void cHpiXmlWriter::NodeSaHpiFumiLogicalBankStateFlagsT(
    const char * name,
    const SaHpiFumiLogicalBankStateFlagsT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiFumiLogicalBankStateFlagsT );
}

void cHpiXmlWriter::NodeSaHpiFumiProtocolT(
    const char * name,
    const SaHpiFumiProtocolT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiFumiProtocolT );
}

void cHpiXmlWriter::NodeSaHpiFumiCapabilityT(
    const char * name,
    const SaHpiFumiCapabilityT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiFumiCapabilityT );
}

void cHpiXmlWriter::NodeSaHpiSensorOptionalDataT(
    const char * name,
    const SaHpiSensorOptionalDataT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiSensorOptionalDataT );
}

void cHpiXmlWriter::NodeSaHpiSensorEnableOptDataT(
    const char * name,
    const SaHpiSensorEnableOptDataT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiSensorEnableOptDataT );
}

void cHpiXmlWriter::NodeSaHpiEvtQueueStatusT(
    const char * name,
    const SaHpiEvtQueueStatusT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiEvtQueueStatusT );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorNumT(
    const char * name,
    const SaHpiAnnunciatorNumT& x )
{
    NodeSaHpiInstrumentIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiLoadNumberT(
    const char * name,
    const SaHpiLoadNumberT& x )
{
    NodeSaHpiUint32T( name, x );
}

void cHpiXmlWriter::NodeSaHpiCapabilitiesT(
    const char * name,
    const SaHpiCapabilitiesT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiCapabilitiesT );
}

void cHpiXmlWriter::NodeSaHpiHsCapabilitiesT(
    const char * name,
    const SaHpiHsCapabilitiesT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiHsCapabilitiesT );
}

void cHpiXmlWriter::NodeSaHpiDomainCapabilitiesT(
    const char * name,
    const SaHpiDomainCapabilitiesT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiDomainCapabilitiesT );
}

void cHpiXmlWriter::NodeSaHpiAlarmIdT(
    const char * name,
    const SaHpiAlarmIdT& x )
{
    NodeSaHpiEntryIdT( name, x );
}

void cHpiXmlWriter::NodeSaHpiEventLogCapabilitiesT(
    const char * name,
    const SaHpiEventLogCapabilitiesT& x )
{
    cXmlWriter::NodeFlags( name, x, &NamesSaHpiEventLogCapabilitiesT );
}

void cHpiXmlWriter::NodeSaHpiEventLogEntryIdT(
    const char * name,
    const SaHpiEventLogEntryIdT& x )
{
    NodeSaHpiUint32T( name, x );
}


/****************************
 * Enums
 ***************************/
void cHpiXmlWriter::NodeSaHpiLanguageT(
    const char * name,
    const SaHpiLanguageT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_language );
}

void cHpiXmlWriter::NodeSaHpiTextTypeT(
    const char * name,
    const SaHpiTextTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_texttype );
}

void cHpiXmlWriter::NodeSaHpiEntityTypeT(
    const char * name,
    const SaHpiEntityTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_entitytype );
}

void cHpiXmlWriter::NodeSaHpiSensorTypeT(
    const char * name,
    const SaHpiSensorTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensortype );
}

void cHpiXmlWriter::NodeSaHpiSensorReadingTypeT(
    const char * name,
    const SaHpiSensorReadingTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensorreadingtype );
}

void cHpiXmlWriter::NodeSaHpiSensorEventMaskActionT(
    const char * name,
    const SaHpiSensorEventMaskActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensoreventmaskaction );
}

void cHpiXmlWriter::NodeSaHpiSensorUnitsT(
    const char * name,
    const SaHpiSensorUnitsT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensorunits );
}

void cHpiXmlWriter::NodeSaHpiSensorModUnitUseT(
    const char * name,
    const SaHpiSensorModUnitUseT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensormodunituse );
}

void cHpiXmlWriter::NodeSaHpiSensorEventCtrlT(
    const char * name,
    const SaHpiSensorEventCtrlT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sensoreventctrl );
}

void cHpiXmlWriter::NodeSaHpiCtrlTypeT(
    const char * name,
    const SaHpiCtrlTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrltype );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateDigitalT(
    const char * name,
    const SaHpiCtrlStateDigitalT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrlstatedigital );
}

void cHpiXmlWriter::NodeSaHpiCtrlModeT(
    const char * name,
    const SaHpiCtrlModeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrlmode );
}

void cHpiXmlWriter::NodeSaHpiCtrlOutputTypeT(
    const char * name,
    const SaHpiCtrlOutputTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_ctrloutputtype );
}

void cHpiXmlWriter::NodeSaHpiIdrAreaTypeT(
    const char * name,
    const SaHpiIdrAreaTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_idrareatype );
}

void cHpiXmlWriter::NodeSaHpiIdrFieldTypeT(
    const char * name,
    const SaHpiIdrFieldTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_idrfieldtype );
}

void cHpiXmlWriter::NodeSaHpiWatchdogActionT(
    const char * name,
    const SaHpiWatchdogActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogaction );
}

void cHpiXmlWriter::NodeSaHpiWatchdogActionEventT(
    const char * name,
    const SaHpiWatchdogActionEventT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogactionevent );
}

void cHpiXmlWriter::NodeSaHpiWatchdogPretimerInterruptT(
    const char * name,
    const SaHpiWatchdogPretimerInterruptT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogpretimerinterrupt );
}

void cHpiXmlWriter::NodeSaHpiWatchdogTimerUseT(
    const char * name,
    const SaHpiWatchdogTimerUseT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_watchdogtimeruse );
}

void cHpiXmlWriter::NodeSaHpiDimiTestServiceImpactT(
    const char * name,
    const SaHpiDimiTestServiceImpactT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestserviceimpact );
}

void cHpiXmlWriter::NodeSaHpiDimiTestRunStatusT(
    const char * name,
    const SaHpiDimiTestRunStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestrunstatus );
}

void cHpiXmlWriter::NodeSaHpiDimiTestErrCodeT(
    const char * name,
    const SaHpiDimiTestErrCodeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitesterrcode );
}

void cHpiXmlWriter::NodeSaHpiDimiTestParamTypeT(
    const char * name,
    const SaHpiDimiTestParamTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimitestparamtype );
}

void cHpiXmlWriter::NodeSaHpiDimiReadyT(
    const char * name,
    const SaHpiDimiReadyT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_dimiready );
}

void cHpiXmlWriter::NodeSaHpiFumiSpecInfoTypeT(
    const char * name,
    const SaHpiFumiSpecInfoTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumispecinfotype );
}

void cHpiXmlWriter::NodeSaHpiFumiSafDefinedSpecIdT(
    const char * name,
    const SaHpiFumiSafDefinedSpecIdT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumisafdefinedspecid );
}

void cHpiXmlWriter::NodeSaHpiFumiServiceImpactT(
    const char * name,
    const SaHpiFumiServiceImpactT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumiserviceimpact );
}

void cHpiXmlWriter::NodeSaHpiFumiSourceStatusT(
    const char * name,
    const SaHpiFumiSourceStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumisourcestatus );
}

void cHpiXmlWriter::NodeSaHpiFumiBankStateT(
    const char * name,
    const SaHpiFumiBankStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumibankstate );
}

void cHpiXmlWriter::NodeSaHpiFumiUpgradeStatusT(
    const char * name,
    const SaHpiFumiUpgradeStatusT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_fumiupgradestatus );
}

void cHpiXmlWriter::NodeSaHpiHsIndicatorStateT(
    const char * name,
    const SaHpiHsIndicatorStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsindicatorstate );
}

void cHpiXmlWriter::NodeSaHpiHsActionT(
    const char * name,
    const SaHpiHsActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsaction );
}

void cHpiXmlWriter::NodeSaHpiHsStateT(
    const char * name,
    const SaHpiHsStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hsstate );
}

void cHpiXmlWriter::NodeSaHpiHsCauseOfStateChangeT(
    const char * name,
    const SaHpiHsCauseOfStateChangeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_hscauseofstatechange );
}

void cHpiXmlWriter::NodeSaHpiSeverityT(
    const char * name,
    const SaHpiSeverityT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_severity );
}

void cHpiXmlWriter::NodeSaHpiResourceEventTypeT(
    const char * name,
    const SaHpiResourceEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_resourceeventtype );
}

void cHpiXmlWriter::NodeSaHpiDomainEventTypeT(
    const char * name,
    const SaHpiDomainEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_domaineventtype );
}

void cHpiXmlWriter::NodeSaHpiSwEventTypeT(
    const char * name,
    const SaHpiSwEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_sweventtype );
}

void cHpiXmlWriter::NodeSaHpiEventTypeT(
    const char * name,
    const SaHpiEventTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_eventtype );
}

void cHpiXmlWriter::NodeSaHpiStatusCondTypeT(
    const char * name,
    const SaHpiStatusCondTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_statuscondtype );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorModeT(
    const char * name,
    const SaHpiAnnunciatorModeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_annunciatormode );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorTypeT(
    const char * name,
    const SaHpiAnnunciatorTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_annunciatortype );
}

void cHpiXmlWriter::NodeSaHpiRdrTypeT(
    const char * name,
    const SaHpiRdrTypeT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_rdrtype );
}

void cHpiXmlWriter::NodeSaHpiParmActionT(
    const char * name,
    const SaHpiParmActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_parmaction );
}

void cHpiXmlWriter::NodeSaHpiResetActionT(
    const char * name,
    const SaHpiResetActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_resetaction );
}

void cHpiXmlWriter::NodeSaHpiPowerStateT(
    const char * name,
    const SaHpiPowerStateT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_powerstate );
}

void cHpiXmlWriter::NodeSaHpiEventLogOverflowActionT(
    const char * name,
    const SaHpiEventLogOverflowActionT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_eventlogoverflowaction );
}

/****************************
 * Enum-Like Types
 ***************************/
void cHpiXmlWriter::NodeSaErrorT(
    const char * name,
    const SaErrorT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_error );
}

void cHpiXmlWriter::NodeSaHpiEventCategoryT(
    const char * name,
    const SaHpiEventCategoryT& x )
{
    cXmlWriter::NodeEnum( name, x, oh_lookup_eventcategory );
}




/****************************
 * Complex Types
 ***************************/
void cHpiXmlWriter::NodeSaHpiTextBufferT(
    const char * name,
    const SaHpiTextBufferT& x )
{
    std::string s;

    cXmlWriter::BeginNode( name );
    NodeSaHpiTextTypeT( "DataType", x.DataType );
    NodeSaHpiLanguageT( "Language", x.Language );
    NodeSaHpiUint8T( "DataLength", x.DataLength );
    switch ( x.DataType ) {
        case SAHPI_TL_TYPE_UNICODE: // UTF-16, Little-Endian
            for ( size_t i = 0; i < x.DataLength; i += 2 ) {
                gunichar2 utf16;
                utf16 = ( ( (gunichar2)x.Data[i + 1] ) << 8 ) | x.Data[i];
                gchar * utf8 = g_utf16_to_utf8( &utf16, 1, 0, 0, 0 );
                if ( utf8 ) {
                    s.append( utf8 );
                    g_free( utf8 );
                } else {
                    s.push_back( '?' );
                }
            }
            cXmlWriter::Node( "Data", "%s", s.c_str() );
            break;
        case SAHPI_TL_TYPE_BCDPLUS:
        case SAHPI_TL_TYPE_ASCII6:
        case SAHPI_TL_TYPE_TEXT:
            s.assign( reinterpret_cast<const char *>(&x.Data[0]), x.DataLength );
            cXmlWriter::Node( "Data", "%s", s.c_str() );
            break;
        case SAHPI_TL_TYPE_BINARY:
        default:
            cXmlWriter::NodeHex( "Data", &x.Data[0], x.DataLength );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiEntityPathT(
    const char * name,
    const SaHpiEntityPathT& x )
{
    SaErrorT rv;
    oh_big_textbuffer buf;
    rv = oh_init_bigtext( &buf );
    if ( rv == SA_OK ) {
        rv = oh_decode_entitypath( &x, &buf );
    }
    if ( rv == SA_OK ) {
        cXmlWriter::Node( name,
                          "%s",
                          reinterpret_cast<const char *>(&buf.Data[0]) );
    } else {
        cXmlWriter::Node( name, "%s", "{UNKNOWN, 0}" );
    }
}

void cHpiXmlWriter::NodeSaHpiNameT(
    const char * name,
    const SaHpiNameT& x )
{
    std::string s;
    s.assign( reinterpret_cast<const char *>(&x.Value[0]), x.Length );

    cXmlWriter::BeginNode( name );
    NodeSaHpiUint16T( "Length", x.Length );
    cXmlWriter::Node( "Value", "%s", s.c_str() );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiConditionT(
    const char * name,
    const SaHpiConditionT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiStatusCondTypeT( "Type", x.Type );
    NodeSaHpiEntityPathT( "Entity", x.Entity );
    NodeSaHpiDomainIdT( "DomainId", x.DomainId );
    NodeSaHpiResourceIdT( "ResourceId", x.ResourceId );
    NodeSaHpiSensorNumT( "SensorNum", x.SensorNum );
    NodeSaHpiEventStateT( "EventState", x.EventState );
    NodeSaHpiNameT( "Name", x.Name );
    NodeSaHpiManufacturerIdT( "Mid", x.Mid );
    NodeSaHpiTextBufferT( "Data", x.Data );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiGuidT(
    const char * name,
    const SaHpiGuidT& x )
{
    cXmlWriter::NodeHex( name, &x[0], sizeof(x) );
}

void cHpiXmlWriter::NodeSaHpiRptEntryT(
    const char * name,
    const SaHpiRptEntryT& x )
{
    const SaHpiResourceInfoT info = x.ResourceInfo;

    cXmlWriter::BeginNode( name );
    NodeSaHpiResourceIdT( "ResourceId", x.ResourceId );
    cXmlWriter::BeginNode( "ResourceInfo" );
    NodeSaHpiUint8T( "ResourceRev", info.ResourceRev );
    NodeSaHpiUint8T( "SpecificVer", info.SpecificVer );
    NodeSaHpiUint8T( "DeviceSupport", info.DeviceSupport );
    NodeSaHpiManufacturerIdT( "ManufacturerId", info.ManufacturerId );
    NodeSaHpiUint16T( "ProductId", info.ProductId );
    NodeSaHpiUint8T( "FirmwareMajorRev", info.FirmwareMajorRev );
    NodeSaHpiUint8T( "FirmwareMinorRev", info.FirmwareMinorRev );
    NodeSaHpiUint8T( "AuxFirmwareRev", info.AuxFirmwareRev );
    NodeSaHpiGuidT( "Guid", info.Guid );
    cXmlWriter::EndNode( "ResourceInfo" );
    NodeSaHpiEntityPathT( "ResourceEntity", x.ResourceEntity );
    NodeSaHpiCapabilitiesT( "ResourceCapabilities", x.ResourceCapabilities );
    NodeSaHpiHsCapabilitiesT( "HotSwapCapabilities", x.HotSwapCapabilities );
    NodeSaHpiSeverityT( "ResourceSeverity", x.ResourceSeverity );
    NodeSaHpiBoolT( "ResourceFailed", x.ResourceFailed );
    NodeSaHpiTextBufferT( "ResourceTag", x.ResourceTag );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateStreamT(
    const char * name,
    const SaHpiCtrlStateStreamT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiBoolT( "Repeat", x.Repeat );
    NodeSaHpiUint32T( "StreamLength", x.StreamLength );
    cXmlWriter::NodeHex( "Stream", &x.Stream[0], x.StreamLength );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateTextT(
    const char * name,
    const SaHpiCtrlStateTextT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiTxtLineNumT( "Line", x.Line );
    NodeSaHpiTextBufferT( "Text", x.Text );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlStateOemT(
    const char * name,
    const SaHpiCtrlStateOemT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiManufacturerIdT( "MId", x.MId );
    NodeSaHpiUint8T( "BodyLength", x.BodyLength );
    cXmlWriter::NodeHex( "Body", &x.Body[0], x.BodyLength );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecDigitalT(
    const char * name,
    const SaHpiCtrlRecDigitalT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiCtrlStateDigitalT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecDiscreteT(
    const char * name,
    const SaHpiCtrlRecDiscreteT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiCtrlStateDiscreteT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecAnalogT(
    const char * name,
    const SaHpiCtrlRecAnalogT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiCtrlStateAnalogT( "Min", x.Min );
    NodeSaHpiCtrlStateAnalogT( "Max", x.Max );
    NodeSaHpiCtrlStateAnalogT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecStreamT(
    const char * name,
    const SaHpiCtrlRecStreamT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiCtrlStateStreamT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecTextT(
    const char * name,
    const SaHpiCtrlRecTextT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiUint8T( "MaxChars", x.MaxChars );
    NodeSaHpiUint8T( "MaxLines", x.MaxLines );
    NodeSaHpiLanguageT( "Language", x.Language );
    NodeSaHpiTextTypeT( "DataType", x.DataType );
    NodeSaHpiCtrlStateTextT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecOemT(
    const char * name,
    const SaHpiCtrlRecOemT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiManufacturerIdT( "MId", x.MId );
    cXmlWriter::NodeHex( "ConfigData",
                         &x.ConfigData[0],
                         SAHPI_CTRL_OEM_CONFIG_LENGTH );
    NodeSaHpiCtrlStateOemT( "Default", x.Default );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiCtrlRecT(
    const char * name,
    const SaHpiCtrlRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiCtrlNumT( "Num", x.Num );
    NodeSaHpiCtrlOutputTypeT( "OutputType", x.OutputType );
    NodeSaHpiCtrlTypeT( "Type", x.Type );
    cXmlWriter::BeginNode( "TypeUnion" );
    switch ( x.Type ) {
        case SAHPI_CTRL_TYPE_DIGITAL:
            NodeSaHpiCtrlRecDigitalT( "Digital", x.TypeUnion.Digital );
            break;
        case SAHPI_CTRL_TYPE_DISCRETE:
            NodeSaHpiCtrlRecDiscreteT( "Discrete", x.TypeUnion.Discrete );
            break;
        case SAHPI_CTRL_TYPE_ANALOG:
            NodeSaHpiCtrlRecAnalogT( "Analog", x.TypeUnion.Analog );
            break;
        case SAHPI_CTRL_TYPE_STREAM:
            NodeSaHpiCtrlRecStreamT( "Stream", x.TypeUnion.Stream );
            break;
        case SAHPI_CTRL_TYPE_TEXT:
            NodeSaHpiCtrlRecTextT( "Text", x.TypeUnion.Text );
            break;
        case SAHPI_CTRL_TYPE_OEM:
            NodeSaHpiCtrlRecOemT( "Oem", x.TypeUnion.Oem );
            break;
        default:
            break;
    }
    cXmlWriter::EndNode( "TypeUnion" );
    cXmlWriter::BeginNode( "DefaultMode" );
    NodeSaHpiCtrlModeT( "Mode", x.DefaultMode.Mode );
    NodeSaHpiBoolT( "ReadOnly", x.DefaultMode.ReadOnly );
    cXmlWriter::EndNode( "DefaultMode" );
    NodeSaHpiBoolT( "WriteOnly", x.WriteOnly );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorReadingT(
    const char * name,
    const SaHpiSensorReadingT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiBoolT( "IsSupported", x.IsSupported );
    if ( x.IsSupported != SAHPI_FALSE ) {
        NodeSaHpiSensorReadingTypeT( "Type", x.Type );
        cXmlWriter::BeginNode( "Value" );
        switch( x.Type ) {
            case SAHPI_SENSOR_READING_TYPE_INT64:
                NodeSaHpiInt64T( "SensorInt64", x.Value.SensorInt64 );
                break;
            case SAHPI_SENSOR_READING_TYPE_UINT64:
                NodeSaHpiUint64T( "SensorUint64", x.Value.SensorUint64 );
                break;
            case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                NodeSaHpiFloat64T( "SensorFloat64", x.Value.SensorFloat64 );
                break;
            case SAHPI_SENSOR_READING_TYPE_BUFFER:
                cXmlWriter::NodeHex( "SensorBuffer",
                                     &x.Value.SensorBuffer[0],
                                     SAHPI_SENSOR_BUFFER_LENGTH );
                break;
            default:
                break;
        }
        cXmlWriter::EndNode( "Value" );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorRangeT(
    const char * name,
    const SaHpiSensorRangeT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiSensorRangeFlagsT( "Flags", x.Flags );
    if ( x.Flags & SAHPI_SRF_MAX ) {
        NodeSaHpiSensorReadingT( "Max", x.Max );
    }
    if ( x.Flags & SAHPI_SRF_MIN ) {
        NodeSaHpiSensorReadingT( "Min", x.Min );
    }
    if ( x.Flags & SAHPI_SRF_NOMINAL ) {
        NodeSaHpiSensorReadingT( "Nominal", x.Nominal );
    }
    if ( x.Flags & SAHPI_SRF_NORMAL_MAX ) {
        NodeSaHpiSensorReadingT( "NormalMax", x.NormalMax );
    }
    if ( x.Flags & SAHPI_SRF_NORMAL_MIN ) {
        NodeSaHpiSensorReadingT( "NormalMin", x.NormalMin );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorDataFormatT(
    const char * name,
    const SaHpiSensorDataFormatT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiBoolT( "IsSupported", x.IsSupported );
    if ( x.IsSupported != SAHPI_FALSE ) {
        NodeSaHpiSensorReadingTypeT( "ReadingType", x.ReadingType );
        NodeSaHpiSensorUnitsT( "BaseUnits", x.BaseUnits );
        NodeSaHpiSensorUnitsT( "ModifierUnits", x.ModifierUnits );
        NodeSaHpiSensorModUnitUseT( "ModifierUse", x.ModifierUse );
        NodeSaHpiBoolT( "Percentage", x.Percentage );
        NodeSaHpiSensorRangeT( "Range", x.Range );
        NodeSaHpiFloat64T( "AccuracyFactor", x.AccuracyFactor );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorThdDefnT(
    const char * name,
    const SaHpiSensorThdDefnT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiBoolT( "IsAccessible", x.IsAccessible );
    if ( x.IsAccessible != SAHPI_FALSE ) {
        NodeSaHpiSensorThdMaskT( "ReadThold", x.ReadThold );
        NodeSaHpiSensorThdMaskT( "WriteThold", x.WriteThold );
        NodeSaHpiBoolT( "Nonlinear", x.Nonlinear );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorRecT(
    const char * name,
    const SaHpiSensorRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiSensorNumT( "Num", x.Num );
    NodeSaHpiSensorTypeT( "Type", x.Type );
    NodeSaHpiEventCategoryT( "Category", x.Category );
    NodeSaHpiBoolT( "EnableCtrl", x.EnableCtrl );
    NodeSaHpiSensorEventCtrlT( "EventCtrl", x.EventCtrl );
    NodeSaHpiEventStateT( "Events", x.Events );
    NodeSaHpiSensorDataFormatT( "DataFormat", x.DataFormat );
    NodeSaHpiSensorThdDefnT( "ThresholdDefn", x.ThresholdDefn );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiInventoryRecT(
    const char * name,
    const SaHpiInventoryRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiIdrIdT( "IdrId", x.IdrId );
    NodeSaHpiBoolT( "Persistent", x.Persistent );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiWatchdogRecT(
    const char * name,
    const SaHpiWatchdogRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiWatchdogNumT( "WatchdogNum", x.WatchdogNum );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiAnnunciatorRecT(
    const char * name,
    const SaHpiAnnunciatorRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiAnnunciatorNumT( "AnnunciatorNum", x.AnnunciatorNum );
    NodeSaHpiAnnunciatorTypeT( "AnnunciatorType", x.AnnunciatorType );
    NodeSaHpiBoolT( "ModeReadOnly", x.ModeReadOnly );
    NodeSaHpiUint32T( "MaxConditions", x.MaxConditions );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiDimiRecT(
    const char * name,
    const SaHpiDimiRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiDimiNumT( "DimiNum", x.DimiNum );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiFumiRecT(
    const char * name,
    const SaHpiFumiRecT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiFumiNumT( "Num", x.Num );
    NodeSaHpiFumiProtocolT( "AccessProt", x.AccessProt );
    NodeSaHpiFumiCapabilityT( "Capability", x.Capability );
    NodeSaHpiUint8T( "NumBanks", x.NumBanks );
    NodeSaHpiUint32T( "Oem", x.Oem );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiRdrT(
    const char * name,
    const SaHpiRdrT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiRdrTypeT( "RdrType", x.RdrType );
    NodeSaHpiEntityPathT( "Entity", x.Entity );
    NodeSaHpiBoolT( "IsFru", x.IsFru );

    cXmlWriter::BeginNode( "RdrTypeUnion" );
    switch ( x.RdrType ) {
        case SAHPI_CTRL_RDR:
            NodeSaHpiCtrlRecT( "CtrlRec", x.RdrTypeUnion.CtrlRec );
            break;
        case SAHPI_SENSOR_RDR:
            NodeSaHpiSensorRecT( "SensorRec", x.RdrTypeUnion.SensorRec );
            break;
        case SAHPI_INVENTORY_RDR:
            NodeSaHpiInventoryRecT( "InventoryRec", x.RdrTypeUnion.InventoryRec );
            break;
        case SAHPI_WATCHDOG_RDR:
            NodeSaHpiWatchdogRecT( "WatchdogRec", x.RdrTypeUnion.WatchdogRec );
            break;
        case SAHPI_ANNUNCIATOR_RDR:
            NodeSaHpiAnnunciatorRecT( "AnnunciatorRec", x.RdrTypeUnion.AnnunciatorRec );
            break;
        case SAHPI_DIMI_RDR:
            NodeSaHpiDimiRecT( "DimiRec", x.RdrTypeUnion.DimiRec );
            break;
        case SAHPI_FUMI_RDR:
            NodeSaHpiFumiRecT( "FumiRec", x.RdrTypeUnion.FumiRec );
            break;
        default:
            break;
    }
    cXmlWriter::EndNode( "RdrTypeUnion" );
    NodeSaHpiTextBufferT( "IdString", x.IdString );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiResourceEventT(
        const char * name,
        const SaHpiResourceEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiResourceEventTypeT( "ResourceEventType", x.ResourceEventType );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiDomainEventT(
        const char * name,
        const SaHpiDomainEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiDomainEventTypeT( "Type", x.Type );
    NodeSaHpiDomainIdT( "DomainId", x.DomainId );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorEventT(
        const char * name,
        const SaHpiSensorEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiSensorNumT( "SensorNum", x.SensorNum );
    NodeSaHpiSensorTypeT( "SensorType", x.SensorType );
    NodeSaHpiEventCategoryT( "EventCategory", x.EventCategory );
    NodeSaHpiBoolT( "Assertion", x.Assertion );
    NodeSaHpiEventStateT( "EventState", x.EventState );
    NodeSaHpiSensorOptionalDataT( "OptionalDataPresent", x.OptionalDataPresent );
    if ( x.OptionalDataPresent & SAHPI_SOD_TRIGGER_READING ) {
        NodeSaHpiSensorReadingT( "TriggerReading", x.TriggerReading );
    }
    if ( x.OptionalDataPresent & SAHPI_SOD_TRIGGER_THRESHOLD ) {
        NodeSaHpiSensorReadingT( "TriggerThreshold", x.TriggerThreshold );
    }
    if ( x.OptionalDataPresent & SAHPI_SOD_PREVIOUS_STATE ) {
        NodeSaHpiEventStateT( "PreviousState", x.PreviousState );
    }
    if ( x.OptionalDataPresent & SAHPI_SOD_CURRENT_STATE ) {
        NodeSaHpiEventStateT( "CurrentState", x.CurrentState );
    }
    if ( x.OptionalDataPresent & SAHPI_SOD_OEM ) {
        NodeSaHpiUint32T( "Oem", x.Oem );
    }
    if ( x.OptionalDataPresent & SAHPI_SOD_SENSOR_SPECIFIC ) {
        NodeSaHpiUint32T( "SensorSpecific", x.SensorSpecific );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiSensorEnableChangeEventT(
        const char * name,
        const SaHpiSensorEnableChangeEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiSensorNumT( "SensorNum", x.SensorNum );
    NodeSaHpiSensorTypeT( "SensorType", x.SensorType );
    NodeSaHpiEventCategoryT( "EventCategory", x.EventCategory );
    NodeSaHpiBoolT( "SensorEnable", x.SensorEnable );
    NodeSaHpiBoolT( "SensorEventEnable", x.SensorEventEnable );
    NodeSaHpiEventStateT( "AssertEventMask", x.AssertEventMask );
    NodeSaHpiEventStateT( "DeassertEventMask", x.DeassertEventMask );
    NodeSaHpiSensorEnableOptDataT( "OptionalDataPresent", x.OptionalDataPresent );
    if ( x.OptionalDataPresent & SAHPI_SEOD_CURRENT_STATE ) {
        NodeSaHpiEventStateT( "CurrentState", x.CurrentState );
    }
    if ( x.OptionalDataPresent & SAHPI_SEOD_ALARM_STATES ) {
        NodeSaHpiEventStateT( "CriticalAlarms", x.CriticalAlarms );
        NodeSaHpiEventStateT( "MajorAlarms", x.MajorAlarms );
        NodeSaHpiEventStateT( "MinorAlarms", x.MinorAlarms );
    }
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiHotSwapEventT(
        const char * name,
        const SaHpiHotSwapEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiHsStateT( "HotSwapState", x.HotSwapState );
    NodeSaHpiHsStateT( "PreviousHotSwapState", x.PreviousHotSwapState );
    NodeSaHpiHsCauseOfStateChangeT( "CauseOfStateChange", x.CauseOfStateChange );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiWatchdogEventT(
        const char * name,
        const SaHpiWatchdogEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiWatchdogNumT( "WatchdogNum", x.WatchdogNum );
    NodeSaHpiWatchdogActionEventT( "WatchdogAction", x.WatchdogAction );
    NodeSaHpiWatchdogPretimerInterruptT( "WatchdogPreTimerAction",
                                         x.WatchdogPreTimerAction );
    NodeSaHpiWatchdogTimerUseT( "WatchdogUse", x.WatchdogUse );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiHpiSwEventT(
        const char * name,
        const SaHpiHpiSwEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiManufacturerIdT( "MId", x.MId );
    NodeSaHpiSwEventTypeT( "Type", x.Type );
    NodeSaHpiTextBufferT( "EventData", x.EventData );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiOemEventT(
        const char * name,
        const SaHpiOemEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiManufacturerIdT( "MId", x.MId );
    NodeSaHpiTextBufferT( "OemEventData", x.OemEventData );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiUserEventT(
        const char * name,
        const SaHpiUserEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiTextBufferT( "UserEventData", x.UserEventData );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiDimiEventT(
        const char * name,
        const SaHpiDimiEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiDimiNumT( "DimiNum", x.DimiNum );
    NodeSaHpiDimiTestNumT( "TestNum", x.TestNum );
    NodeSaHpiDimiTestRunStatusT( "DimiTestRunStatus", x.DimiTestRunStatus );
    NodeSaHpiDimiTestPercentCompletedT( "DimiTestPercentCompleted",
                                        x.DimiTestPercentCompleted );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiDimiUpdateEventT(
        const char * name,
        const SaHpiDimiUpdateEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiDimiNumT( "DimiNum", x.DimiNum );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiFumiEventT(
        const char * name,
        const SaHpiFumiEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiFumiNumT( "FumiNum", x.FumiNum );
    NodeSaHpiUint8T( "BankNum", x.BankNum );
    NodeSaHpiFumiUpgradeStatusT( "UpgradeStatus", x.UpgradeStatus );
    cXmlWriter::EndNode( name );
}

void cHpiXmlWriter::NodeSaHpiEventT(
    const char * name,
    const SaHpiEventT& x )
{
    cXmlWriter::BeginNode( name );
    NodeSaHpiResourceIdT( "Source", x.Source );
    NodeSaHpiEventTypeT( "EventType", x.EventType );
    NodeSaHpiTimeT( "Timestamp", x.Timestamp );
    NodeSaHpiSeverityT( "Severity", x.Severity );
    cXmlWriter::BeginNode( "EventDataUnion" );
    switch ( x.EventType ) {
        case SAHPI_ET_RESOURCE:
            NodeSaHpiResourceEventT( "ResourceEvent",
                                     x.EventDataUnion.ResourceEvent );
            break;
        case SAHPI_ET_DOMAIN:
            NodeSaHpiDomainEventT( "DomainEvent",
                                   x.EventDataUnion.DomainEvent );
            break;
        case SAHPI_ET_SENSOR:
            NodeSaHpiSensorEventT( "SensorEvent",
                                   x.EventDataUnion.SensorEvent );
            break;
        case SAHPI_ET_SENSOR_ENABLE_CHANGE:
            NodeSaHpiSensorEnableChangeEventT( "SensorEnableChangeEvent",
                                               x.EventDataUnion.SensorEnableChangeEvent );
            break;
        case SAHPI_ET_HOTSWAP:
            NodeSaHpiHotSwapEventT( "HotSwapEvent",
                                    x.EventDataUnion.HotSwapEvent );
            break;
        case SAHPI_ET_WATCHDOG:
            NodeSaHpiWatchdogEventT( "WatchdogEvent",
                                     x.EventDataUnion.WatchdogEvent );
            break;
        case SAHPI_ET_HPI_SW:
            NodeSaHpiHpiSwEventT( "HpiSwEvent",
                                  x.EventDataUnion.HpiSwEvent );
            break;
        case SAHPI_ET_OEM:
            NodeSaHpiOemEventT( "OemEvent",
                                x.EventDataUnion.OemEvent );
            break;
        case SAHPI_ET_USER:
            NodeSaHpiUserEventT( "UserEvent",
                                 x.EventDataUnion.UserEvent );
            break;
        case SAHPI_ET_DIMI:
            NodeSaHpiDimiEventT( "DimiEvent",
                                 x.EventDataUnion.DimiEvent );
            break;
        case SAHPI_ET_DIMI_UPDATE:
            NodeSaHpiDimiUpdateEventT( "DimiUpdateEvent",
                                       x.EventDataUnion.DimiUpdateEvent );
            break;
        case SAHPI_ET_FUMI:
            NodeSaHpiFumiEventT( "FumiEvent",
                                 x.EventDataUnion.FumiEvent );
            break;
        default:
            break;
    }
    cXmlWriter::EndNode( "EventDataUnion" );
    cXmlWriter::EndNode( name );
}

