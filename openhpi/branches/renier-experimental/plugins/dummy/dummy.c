/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Renier Morales <renierm@users.sf.net>
 *      David Judkovics
 *      Sean Dague
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#include <unistd.h>

#include <SaHpi.h>
#include <oh_handler.h>
#include <oh_domain.h>
#include <oh_utils.h>
#include <oh_error.h>

#define DUMMY_THREADED
#undef  DUMMY_THREADED
#define THREAD_SLEEP_TIME_NS    500000000
#define THREAD_SLEEP_TIME_SECS  1

typedef struct {
        SaHpiResourceIdT        res_Id;
        SaHpiRdrTypeT           rdr_type;
        SaHpiInstrumentIdT      num;
        int                     is_save;
        SaHpiRdrT               rdr_default;
        SaHpiRdrT               rdr_save;
} RdrSaveT;

gpointer event_thread(gpointer data);

#define ELEMENT_NUM(x) (sizeof(x)/sizeof(x[0]))

/* set of defines to make the sample definitions below right */
#define sensor_buffer(string)                                           \
        {                                                               \
                .IsSupported = SAHPI_TRUE,                              \
                        .Type = SAHPI_SENSOR_READING_TYPE_BUFFER,       \
                        .Value = {                                      \
                        .SensorBuffer = string                          \
                },                                                      \
        }

#define sensor_int64(int)                       \
        {                                                               \
                .IsSupported = SAHPI_TRUE,                              \
                        .Type = SAHPI_SENSOR_READING_TYPE_INT64,       \
                        .Value = {                                      \
                        .SensorInt64 = int                          \
                },                                                      \
        }

#define def_text_buffer(string)                                 \
        {                                                       \
                .DataType = SAHPI_TL_TYPE_TEXT,                 \
                        .Language = SAHPI_LANG_ENGLISH,         \
                        .DataLength = sizeof(string) - 1,           \
                        .Data = string                          \
        }


#define unicode_text_buffer(string)                                 \
        {                                                       \
                .DataType = SAHPI_TL_TYPE_UNICODE,                 \
                        .Language = SAHPI_LANG_ENGLISH,         \
                        .DataLength = sizeof(string) - 1,           \
                        .Data = string                          \
        }


#define NUM_RESOURCES   3
#define NUM_RDRS        15

#define VIRTUAL_NODES 2

#define NO_ID 0  /* Arbitrarily define IdrId == 0 as end-of-inventory */
                 /* instead of using a counter of some kind           */

RdrSaveT        Save_rdr[NUM_RDRS];

/* dummy resource status */
static struct {
        SaHpiPowerStateT        power;
        SaHpiHsStateT           hotswap;
        SaHpiHsIndicatorStateT  indicator;
        SaHpiResetActionT       reset;
        SaHpiResourceIdT        ResourceId;
} dummy_resource_status[NUM_RESOURCES] = {
        {
                .power = SAHPI_POWER_ON,
                .hotswap = SAHPI_HS_STATE_ACTIVE,
                .indicator =  SAHPI_HS_INDICATOR_ON,
                .reset = SAHPI_COLD_RESET,
        },
        {
                .power = SAHPI_POWER_ON,
                .hotswap = SAHPI_HS_STATE_ACTIVE,
                .indicator =  SAHPI_HS_INDICATOR_ON,
                .reset = SAHPI_COLD_RESET,
        },
        {
                .power = SAHPI_POWER_ON,
                .hotswap = SAHPI_HS_STATE_ACTIVE,
                .indicator =  SAHPI_HS_INDICATOR_ON,
                .reset = SAHPI_COLD_RESET,
        },
};

/* dummy entity array */
static SaHpiRptEntryT dummy_resources[NUM_RESOURCES] = {

/*  SaHpiEntryIdT        EntryId;
    SaHpiResourceIdT     ResourceId;
    SaHpiResourceInfoT   ResourceInfo;
    SaHpiEntityPathT     ResourceEntity;
    SaHpiCapabilitiesT   ResourceCapabilities;
    SaHpiSeverityT       ResourceSeverity;
    SaHpiDomainIdT       DomainId;
    SaHpiTextBufferT     ResourceTag; */
        /* resource one */
        {
                .EntryId = 0,
                .ResourceCapabilities = SAHPI_CAPABILITY_INVENTORY_DATA |
                SAHPI_CAPABILITY_CONTROL |
                SAHPI_CAPABILITY_RESOURCE |
                SAHPI_CAPABILITY_RDR |
                SAHPI_CAPABILITY_CONFIGURATION |
                SAHPI_CAPABILITY_SENSOR |
                SAHPI_CAPABILITY_POWER |
                SAHPI_CAPABILITY_RESET |
                SAHPI_CAPABILITY_WATCHDOG |
                SAHPI_CAPABILITY_ANNUNCIATOR |
                SAHPI_CAPABILITY_EVENT_LOG,
                .ResourceEntity = {
                        .Entry = {
                                 {SAHPI_ENT_SYSTEM_BOARD, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .ResourceId = 0,
                .ResourceInfo = {0},
                .ResourceSeverity= SAHPI_MAJOR,
                .ResourceTag = def_text_buffer("Dummy-System-Board")
        },
        /* resource two */
        {
                .EntryId = 0,
                .ResourceCapabilities =
                        SAHPI_CAPABILITY_RESOURCE
                        | SAHPI_CAPABILITY_RDR
                        | SAHPI_CAPABILITY_SENSOR
                        | SAHPI_CAPABILITY_FRU
                        | SAHPI_CAPABILITY_POWER
                        | SAHPI_CAPABILITY_MANAGED_HOTSWAP,
                .HotSwapCapabilities = SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED,
                .ResourceEntity = {
                        .Entry = {
                                 {SAHPI_ENT_POWER_SUPPLY, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .ResourceId = 0,
                .ResourceInfo = {0},
                .ResourceSeverity= SAHPI_MAJOR,
                .ResourceTag    = def_text_buffer("Dummy-Power-Supply 1")
        },
        /* resource third */
        {
                .EntryId = 0,
                .ResourceCapabilities = SAHPI_CAPABILITY_RESOURCE,
                .ResourceEntity = {
                        .Entry = {
                                {SAHPI_ENT_POWER_SUPPLY, 2},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .ResourceId = 0,
                .ResourceInfo = {0},
                .ResourceSeverity= SAHPI_MAJOR,
                .ResourceTag    = def_text_buffer("Dummy-Power-Supply 2")
        }
};

static struct oh_event hotswap_event[] = {
        /* This is an hotswap event for insertion pending */
        {
                .did = 1, /* set up domain id */
                .type = OH_ET_HPI,
                .u = {
                        .hpi_event = {
                                .res.ResourceId = 0, /* this needs to be the resourceId */
                                .rdr.RecordId = 1, /* ????, unquie amoung the hotswap events */
                                .event = {
                                        .Source = 0,
                                        .EventType = SAHPI_ET_HOTSWAP,
                                        .Timestamp = 0,
                                        .Severity = SAHPI_CRITICAL,
                                        .EventDataUnion = {
                                                .HotSwapEvent = {
                                                        .HotSwapState = SAHPI_HS_STATE_INSERTION_PENDING,
                                                        .PreviousHotSwapState = SAHPI_HS_STATE_NOT_PRESENT,
                                                },
                                        },
                                },
                        },
                },
        },
        /* This is an hotswap event for insertion pending */
        {
                .did = 1, /* set up domain id */
                .type = OH_ET_HPI,
                .u = {
                        .hpi_event = {
                                .res.ResourceId = 0,  /* this needs to be the resourceId */
                                .rdr.RecordId = 2,      /* ????, unquie amoung the hotswap events */
                                .event = {
                                        .Source = 0,
                                        .EventType = SAHPI_ET_HOTSWAP,
                                        .Timestamp = 0,
                                        .Severity = SAHPI_CRITICAL,
                                        .EventDataUnion = {
                                                .HotSwapEvent = {
                                                        .HotSwapState = SAHPI_HS_STATE_EXTRACTION_PENDING,
                                                        .PreviousHotSwapState = SAHPI_HS_STATE_ACTIVE,
                                                },
                                        },
                                },
                        },
                },
        },
};

static SaHpiRdrT dummy_rdrs[] = {
        /* first rdr */
        /* This is the one temperature sensor on System Board */
        {
                .RecordId = 0, /*no use, should be rewritten by upper layer */
                .RdrType  = SAHPI_SENSOR_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_SYSTEM_BOARD, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                 .Num  = 1, /*no use! should be rewritten by upper layer */
                                 .Type = SAHPI_TEMPERATURE,
                                 .Category = SAHPI_EC_THRESHOLD,
                                 .EventCtrl = SAHPI_SEC_PER_EVENT,
                                 .Events        =
                                 SAHPI_ES_UPPER_MINOR
                                 | SAHPI_ES_UPPER_MAJOR
                                 | SAHPI_ES_UPPER_CRIT,
                                 .DataFormat = {
                                         .BaseUnits = SAHPI_SU_DEGREES_C,
                                         .IsSupported = SAHPI_TRUE,
                                         .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                         .ModifierUse = SAHPI_SMUU_NONE,
                                         .Percentage = SAHPI_FALSE,
                                         .ReadingType = SAHPI_SENSOR_READING_TYPE_INT64,
                                         .Range = {
                                                  .Flags = SAHPI_SRF_MIN |
                                                  SAHPI_SRF_MAX |
                                                  SAHPI_SRF_NORMAL_MIN |
                                                  SAHPI_SRF_NORMAL_MAX |
                                                  SAHPI_SRF_NOMINAL,

                                                  .Max = sensor_int64(200),
                                                  .Min = sensor_int64(0),
                                                  .NormalMin = sensor_int64(20),
                                                  .NormalMax = sensor_int64(100),
                                                  .Nominal = sensor_int64(40),
                                          },
                                 },
                                .ThresholdDefn = {
                                         .IsAccessible = SAHPI_TRUE,
                                         .ReadThold = SAHPI_STM_LOW_MINOR |
                                                     SAHPI_STM_LOW_MAJOR |
                                                     SAHPI_STM_LOW_CRIT |
                                                     SAHPI_STM_UP_MINOR |
                                                     SAHPI_STM_UP_MAJOR |
                                                     SAHPI_STM_UP_CRIT |
                                                     SAHPI_STM_UP_HYSTERESIS |
                                                     SAHPI_STM_LOW_HYSTERESIS,
                                         .WriteThold = SAHPI_STM_LOW_CRIT,
                                 },
                                 .Oem = 0,
                        },
                },
                .IdString = def_text_buffer("Dummy-System-Temperature")
        },
        /* second rdr */
        /* This is the one temperature sensor on Power Supply */
        {
                .RecordId = 0, /*no use, should be rewritten by upper layer */
                .RdrType  = SAHPI_SENSOR_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_POWER_SUPPLY, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                 .Num  = 2, /*no use! should be rewritten by upper layer */
                                 .Type = SAHPI_TEMPERATURE,
                                 .Category = SAHPI_EC_THRESHOLD,
                                 .EventCtrl = SAHPI_SEC_PER_EVENT,
                                 .Events    = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                                 .DataFormat = {
                                         .BaseUnits = SAHPI_SU_DEGREES_C,
                                         .IsSupported = SAHPI_TRUE,
                                         .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                         .ModifierUse = SAHPI_SMUU_NONE,
                                         .Percentage = SAHPI_FALSE,
                                         .ReadingType = SAHPI_SENSOR_READING_TYPE_INT64,
                                         .Range = {
                                                  .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                  .Max = sensor_int64(100),
                                                  .Min = sensor_int64(0)
                                          },
                                 },
                                 .ThresholdDefn = {
                                         .IsAccessible = SAHPI_TRUE,
                                         .ReadThold = SAHPI_STM_LOW_MINOR |
                                         SAHPI_STM_LOW_MAJOR |
                                         SAHPI_STM_LOW_CRIT |
                                         SAHPI_STM_UP_MINOR |
                                         SAHPI_STM_UP_MAJOR |
                                         SAHPI_STM_UP_CRIT |
                                         SAHPI_STM_UP_HYSTERESIS |
                                         SAHPI_STM_LOW_HYSTERESIS,
                                         .WriteThold = SAHPI_STM_LOW_CRIT,
                                 },
                                 .Oem = 0,
                         },
                },
                .IdString = def_text_buffer("Dummy-Power-Temperature 1")
        },

        /* third rdr */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_SYSTEM_BOARD, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 5,
                                .OutputType = SAHPI_CTRL_LED,
                                .Type = SAHPI_CTRL_TYPE_OEM,
                                .TypeUnion = {
                                        .Oem = {
                                                .MId = 0xff,
                                                .ConfigData = "BOGUS CFG",
                                                .Default = {
                                                        .MId = 0xff,
                                                        .BodyLength = 56,
                                                        .Body = "Bogus Data for CTRL on {SAHPI_ENT_SYSTEM_BOARD, 1}{,}{,}",
                                                },
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Digital-Control-1")
        },
        /* fourth rdr */
        /* This is an RDR representing a inventory */
        /* on the first system board */
        {
                .RdrType  = SAHPI_INVENTORY_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .InventoryRec = {
                                .IdrId  = 6,
                                .Oem    = 0x12344321,
                        },
                },
                .IdString = def_text_buffer("System-Inventory-1")
        },
        /* fifth rdr */
        /* This is the one temperature sensor on System Board */
        {
                .RecordId = 0, /*no use, should be rewritten by upper layer */
                .RdrType  = SAHPI_SENSOR_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_SYSTEM_BOARD, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                 .Num  = 3,
                                 .Type = SAHPI_TEMPERATURE,
                                 .Category = SAHPI_EC_THRESHOLD,
                                 .EnableCtrl = SAHPI_TRUE,
                                 .EventCtrl = SAHPI_SEC_PER_EVENT,
                                 .Events    = SAHPI_ES_UPPER_MINOR | SAHPI_ES_UPPER_MAJOR | SAHPI_ES_UPPER_CRIT,
                                 .DataFormat = {
                                         .BaseUnits = SAHPI_SU_DEGREES_K,
                                         .IsSupported = SAHPI_TRUE,
                                         .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                         .ModifierUse = SAHPI_SMUU_NONE,
                                         .Percentage = SAHPI_FALSE,
                                         .ReadingType = SAHPI_SENSOR_READING_TYPE_INT64,
                                         .Range = {
                                                  .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                  .Max = sensor_int64(373),
                                                  .Min = sensor_int64(272),
                                          },
                                 },
                                 .ThresholdDefn = {
                                         .IsAccessible = SAHPI_TRUE,
                                         .ReadThold = SAHPI_STM_LOW_MINOR|
                                         SAHPI_STM_LOW_MAJOR|
                                         SAHPI_STM_LOW_CRIT |
                                         SAHPI_STM_UP_MINOR |
                                         SAHPI_STM_UP_MAJOR |
                                         SAHPI_STM_UP_CRIT |
                                         SAHPI_STM_UP_HYSTERESIS |
                                         SAHPI_STM_LOW_HYSTERESIS,
                                         .WriteThold = SAHPI_STM_LOW_MINOR|
                                         SAHPI_STM_LOW_MAJOR|
                                         SAHPI_STM_LOW_CRIT |
                                         SAHPI_STM_UP_MINOR |
                                         SAHPI_STM_UP_MAJOR |
                                         SAHPI_STM_UP_CRIT |
                                         SAHPI_STM_UP_HYSTERESIS |
                                         SAHPI_STM_LOW_HYSTERESIS,
                                 },
                                 .Oem = 0,
                         },
                },
                .IdString = def_text_buffer("Dummy-System-Temperature")
        },
        /* sixth rdr */
        /* This is the one temperature sensor on Power Supply */
        {
                .RecordId = 0, /*no use,
                                 should be rewritten by upper layer */
                .RdrType  = SAHPI_SENSOR_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_POWER_SUPPLY, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                 .Num  = 4, /*no use!
                                              should be rewritten by upper layer */
                                 .Type = SAHPI_TEMPERATURE,
                                 .Category = SAHPI_EC_THRESHOLD,
                                 .EventCtrl = SAHPI_SEC_PER_EVENT,
                                 .Events        = SAHPI_ES_UPPER_MINOR
                                 | SAHPI_ES_UPPER_MAJOR
                                 | SAHPI_ES_UPPER_CRIT,
                                 .DataFormat = {
                                         .BaseUnits = SAHPI_SU_DEGREES_F,
                                         .IsSupported = SAHPI_TRUE,
                                         .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                         .ModifierUse = SAHPI_SMUU_NONE,
                                         .Percentage = SAHPI_FALSE,
                                         .ReadingType = SAHPI_SENSOR_READING_TYPE_INT64,
                                         .Range = {
                                                  .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                  .Max = sensor_int64(212),
                                                  .Min = sensor_int64(32),
                                          },
                                 },
                                .ThresholdDefn = {
                                        .IsAccessible = SAHPI_TRUE,
                                        .ReadThold = SAHPI_STM_LOW_MINOR|
                                                     SAHPI_STM_LOW_MAJOR|
                                                     SAHPI_STM_LOW_CRIT |
                                                     SAHPI_STM_UP_MINOR |
                                                     SAHPI_STM_UP_MAJOR |
                                                     SAHPI_STM_UP_CRIT |
                                                     SAHPI_STM_UP_HYSTERESIS |
                                                     SAHPI_STM_LOW_HYSTERESIS,
                                        .WriteThold = SAHPI_STM_LOW_CRIT,
                                },
                                .Oem = 0,
                        },
                },
                .IdString = def_text_buffer("Dummy-Power-Temperature 2")
        },
        /* seventh rdr */
        /* This is an RDR representing a digital control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 7,
                                .OutputType = SAHPI_CTRL_LED,
                                .Type = SAHPI_CTRL_TYPE_DIGITAL,
                                .TypeUnion = {
                                        .Digital = {
                                                .Default = SAHPI_CTRL_STATE_OFF,
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Digital-Control-1")
        },
        /* eigth rdr */
        /* This is an RDR representing a inventory */
        /* on the first system board */
        {
                .RdrType  = SAHPI_INVENTORY_RDR,
                .Entity   = {
                        .Entry = {
                                 {SAHPI_ENT_SYSTEM_BOARD, 1},
                                 {SAHPI_ENT_ROOT, 0}
                         },
                },
                .RdrTypeUnion = {
                        .InventoryRec = {
                                 .IdrId = 8,
                                 .Oem   = 0x12344321,
                         },
                },
                .IdString = def_text_buffer("System-Inventory-1")
        },
        /* ninth rdr */
        /* This is an RDR representing a discrete control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 9,
                                .OutputType = SAHPI_CTRL_FAN_SPEED,
                                .Type = SAHPI_CTRL_TYPE_DISCRETE,
                                .TypeUnion = {
                                        .Discrete = {
                                                .Default = 5005,
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Discrete-Control-1")
        },
        /* tenth rdr */
        /* This is an RDR representing an analog control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 10,
                                .OutputType = SAHPI_CTRL_FAN_SPEED,
                                .Type = SAHPI_CTRL_TYPE_ANALOG,
                                .TypeUnion = {
                                        .Analog = {
                                                .Min = 2000,
                                                .Max = 4000,
                                                .Default = 3000
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Analog-Control-1")
        },
        /* eleventh rdr */
        /* This is an RDR representing a stream control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 11,
                                .OutputType = SAHPI_CTRL_LCD_DISPLAY,
                                .Type = SAHPI_CTRL_TYPE_STREAM,
                                .TypeUnion = {
                                        .Stream = {
                                                .Default = {
                                                        .Repeat = 0,
                                                        .StreamLength = 3,
                                                        .Stream = "OK."
                                                },
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Stream-Control-1")
        },
        /* twelfth rdr */
        /* This is an RDR representing a text control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 12,
                                .OutputType = SAHPI_CTRL_LCD_DISPLAY,
                                .Type = SAHPI_CTRL_TYPE_TEXT,
                                .TypeUnion = {
                                        .Text = {
                                                .Language = SAHPI_LANG_ENGLISH,
                                                .DataType = SAHPI_TL_TYPE_TEXT,
                                                .Default = {
                                                        .Line = 0,
                                                        .Text = def_text_buffer("Text-Control-1")
                                                },
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Text-Control-1")
        },
        /* thirteenth rdr */
        /* This is an RDR representing a oem control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 13,
                                .OutputType = SAHPI_CTRL_OEM,
                                .Type = SAHPI_CTRL_TYPE_OEM,
                                .TypeUnion = {
                                        .Oem = {
                                                .MId = 287,
                                                .ConfigData = "Oem-Cont",
                                                .Default = {
                                                        .MId = 287,
                                                        .BodyLength = 3,
                                                        .Body = "Oem"
                                                },
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Oem-Control-1")
        },
        /* 14 rdr */
        /* This is an RDR representing an ANNUNCIATOR */
        /* on the first system board */
        {
                .RdrType  = SAHPI_WATCHDOG_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .WatchdogRec = {
                                .WatchdogNum = 14,
                                .Oem = 314,
                        },
                },
                .IdString = def_text_buffer("Watchdog-1")
        },
        /* 15 rdr */
        /* This is an RDR representing a watchdog */
        /* on the first system board */
        {
                .RdrType  = SAHPI_ANNUNCIATOR_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .AnnunciatorRec = {
                                .AnnunciatorNum = 15,
                                .AnnunciatorType = SAHPI_ANNUNCIATOR_TYPE_LED,
                                .ModeReadOnly = SAHPI_FALSE,
                                .MaxConditions = 5,
                                .Oem = 314,
                        },
                },
                .IdString = def_text_buffer("Annunciator-1")
        },
};


/* SaHpiCtrlStateT */
//static SaHpiCtrlStateT dummy_controls = {
//        .Type = SAHPI_CTRL_TYPE_DIGITAL,
//        .StateUnion = {
//                .Digital = SAHPI_CTRL_STATE_OFF,
//        },
//};
/************************************************************************/
/* sensor data                                                          */
/************************************************************************/
static struct dummy_sensor {
        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thresholds;
        SaHpiBoolT enabled;
        SaHpiBoolT sen_enable;
        SaHpiBoolT event_enable;
        SaHpiEventStateT assert;
        SaHpiEventStateT deassert;
} dummy_sensors[]= {
        {
                /*This is temp sensor on system board*/
                .reading = sensor_int64(50),
                .thresholds = {
                        .LowCritical = sensor_buffer("35 tics"),
                        .LowMajor = sensor_buffer("88 tocs"),
                        .LowMinor = sensor_buffer("20,0000 leagues"),
                        .UpCritical = sensor_buffer("556 hands"),
                        .UpMajor = sensor_buffer("297 hectares"),
                        .UpMinor = sensor_buffer("10 kilometers"),
                        .PosThdHysteresis = sensor_buffer("5 decaliters"),
                        .NegThdHysteresis = sensor_buffer("1 centillion"),
                },
                /* Sensor Event Enables */
                /* typedef struct {
                   SaHpiSensorStatusT SensorStatus;
                   SaHpiEventStateT   AssertEvents;
                   SaHpiEventStateT   DeassertEvents;
                   } SaHpiSensorEvtEnablesT; */
                /* .Category = SAHPI_EC_THRESHOLD, from above*/
                .enabled = SAHPI_TRUE,
                .sen_enable = SAHPI_TRUE,
                .event_enable = SAHPI_TRUE,
                .assert = SAHPI_ES_LOWER_MINOR,
                .deassert = SAHPI_ES_UPPER_MINOR,
        },
        {
            /*This is temp sensor on system board*/
                .reading = sensor_int64(180),
                .thresholds = {
                        .LowCritical = sensor_int64(201),
                        .LowMajor = sensor_int64(202),
                        .LowMinor = sensor_int64(203),
                        .UpCritical = sensor_int64(204),
                        .UpMajor = sensor_int64(205),
                        .UpMinor = sensor_int64(206),
                        .PosThdHysteresis = sensor_int64(207),
                        .NegThdHysteresis = sensor_int64(208)
                },
                .enabled = SAHPI_TRUE,
                .sen_enable = SAHPI_TRUE,
                .event_enable = SAHPI_TRUE,
                .assert = SAHPI_ES_LOWER_MAJOR,
                .deassert = SAHPI_ES_UPPER_MAJOR,
        },
        {
        /*This is temp sensor on system board*/
                .reading = sensor_int64(0xff),
                .thresholds = {
                        .LowCritical = sensor_int64(301),
                        .LowMajor = sensor_int64(302),
                        .LowMinor = sensor_int64(303),
                        .UpCritical = sensor_int64(304),
                        .UpMajor = sensor_int64(305),
                        .UpMinor = sensor_int64(306),
                        .PosThdHysteresis = sensor_int64(307),
                        .NegThdHysteresis = sensor_int64(308)
                },
                .enabled = SAHPI_TRUE,
                .sen_enable = SAHPI_TRUE,
                .event_enable = SAHPI_TRUE,
                .assert = SAHPI_ES_LOWER_CRIT,
                .deassert = SAHPI_ES_UPPER_CRIT,
        },
        {
            /*This is temp sensor on system board*/
                .reading = sensor_int64(0xcc),
                .thresholds = {
                        .LowCritical = sensor_int64(0x401),
                        .LowMajor = sensor_int64(0x402),
                        .LowMinor = sensor_int64(0x403),
                        .UpCritical = sensor_int64(0x404),
                        .UpMajor = sensor_int64(0x405),
                        .UpMinor = sensor_int64(0x406),
                        .PosThdHysteresis = sensor_int64(0x407),
                        .NegThdHysteresis = sensor_int64(0x408)
                },
                .enabled = SAHPI_TRUE,
                .sen_enable = SAHPI_TRUE,
                .event_enable = SAHPI_TRUE,
                .assert = SAHPI_ES_UPPER_CRIT,
                .deassert = SAHPI_ES_LOWER_CRIT,
        },
};

/************************************************************************/
/* Resource one watchdog data                                          */
/************************************************************************/
#define WTD_MAX 1
typedef struct {
        SaHpiWatchdogNumT       Num;
        SaHpiWatchdogT          wtd;
        SaHpiWatchdogT          reset;
} dummy_wtd_t;

dummy_wtd_t dummy_wtd_timers[] = {
        {
                .Num = 14,
                .wtd = {
                        .Log = SAHPI_TRUE,
                        .Running = SAHPI_TRUE,
                        .TimerUse = SAHPI_WTU_BIOS_FRB2,
                        .TimerAction = SAHPI_WA_RESET,
                        .PretimerInterrupt = SAHPI_WPI_MESSAGE_INTERRUPT,
                        .PreTimeoutInterval = 0,
                        .TimerUseExpFlags = SAHPI_WATCHDOG_EXP_OEM,
                        .InitialCount = 100,
                        .PresentCount = 200
                },
                .reset = {
                        .Log = SAHPI_TRUE,
                        .Running = SAHPI_TRUE,
                        .TimerUse = SAHPI_WTU_BIOS_FRB2,
                        .TimerAction = SAHPI_WA_RESET,
                        .PretimerInterrupt = SAHPI_WPI_MESSAGE_INTERRUPT,
                        .PreTimeoutInterval = 0,
                        .TimerUseExpFlags = SAHPI_WATCHDOG_EXP_OEM,
                        .InitialCount = 100,
                        .PresentCount = 200
                }
        }
};

/************************************************************************/
/* Resource one annunciator data                                          */
/************************************************************************/
#define ANNUN_MAX       1
#define ANNOUN_MAX      1
typedef struct {
        SaHpiAnnunciatorNumT    Num;
        SaHpiAnnunciatorModeT   mode;
        int                     count;
        SaHpiAnnouncementT      def_announs[ANNOUN_MAX];
        SaHpiAnnouncementT      *announs;
} dummy_announ_t;

dummy_announ_t dummy_announs[] = {
        {
                .Num = 15,
                .mode = SAHPI_ANNUNCIATOR_MODE_USER,
                .count = ANNOUN_MAX,
                .def_announs[0] = {
                        .EntryId = 1,
                        .Timestamp = 0,
                        .AddedByUser = SAHPI_FALSE,
                        .Severity = SAHPI_MAJOR,
                        .Acknowledged = SAHPI_FALSE,
                        .StatusCond = {
                                .Type = SAHPI_STATUS_COND_TYPE_SENSOR,
                                .Entity   = {
                                        .Entry = {
                                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                                {SAHPI_ENT_ROOT, 0}
                                        },
                                },
                                .DomainId = 1,
                                .ResourceId = 1,
                                .SensorNum = 1,
                                .EventState = SAHPI_ES_UNSPECIFIED,
                                .Name = {
                                        .Length = 5,
                                        .Value = "announ"
                                },
                                .Mid = 123,
                                .Data = def_text_buffer("Announcement-1")
                        }
                },
                .announs = NULL
        }
};

/************************************************************************/
/* Resource one inventory data                                          */
/************************************************************************/
#define IDR_AREAS_MAX   5
#define IDR_FIELDS_MAX  20
struct  dummy_idr_area {
        SaHpiIdrAreaHeaderT  idrareas;
        SaHpiIdrFieldT  idrfields[IDR_FIELDS_MAX];
};

static struct dummy_inventories {
        SaHpiIdrInfoT   idrinfo;
        struct dummy_idr_area my_idr_area[IDR_AREAS_MAX];
} dummy_inventory[] = {

{
        .idrinfo = {
                    .IdrId = 8,
                    .UpdateCount = 1,
                    .ReadOnly = SAHPI_TRUE,
                    .NumAreas = 1
        },
        .my_idr_area[0] =
        {
                .idrareas = {
                        .AreaId = 1,
                        .Type = SAHPI_IDR_AREATYPE_BOARD_INFO,
                        .ReadOnly = SAHPI_TRUE,
                        .NumFields = 10,
                },

                .idrfields[0] =
                {
                        .AreaId = 1,
                        .FieldId = 1,
                        .Type = SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("Main Chassis")
                },
                {
                        .AreaId = 1,
                        .FieldId = 2,
                        .Type = SAHPI_IDR_FIELDTYPE_MFG_DATETIME,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("10/01/2004")
                },
                {
                        .AreaId = 1,
                        .FieldId = 3,
                        .Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("openHPI Inc.")
                },
                {
                        .AreaId = 1,
                        .FieldId = 4,
                        .Type = SAHPI_IDR_FIELDTYPE_PRODUCT_NAME,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("Imaginary HPI Machine")
                },
                {
                        .AreaId = 1,
                        .FieldId = 5,
                        .Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION, /* Model number? */
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("17")
                },
                {
                        .AreaId = 1,
                        .FieldId = 6,
                        .Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("12HPI345")
                },
                {
                        .AreaId = 1,
                        .FieldId = 7,
                        .Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("78758")
                },
                {
                        .AreaId = 1,
                        .FieldId = 8,
                        .Type = SAHPI_IDR_FIELDTYPE_FILE_ID,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("3")
                },

                {
                        .AreaId = 1,
                        .FieldId = 9,
                        .Type = SAHPI_IDR_FIELDTYPE_ASSET_TAG,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("My Precious")
                },

                {
                        .AreaId = 1,
                        .FieldId = 10,
                        .Type = SAHPI_IDR_FIELDTYPE_CUSTOM,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("Hot")
                },

                {}
        }
},
{
        .idrinfo = {
                    .IdrId = 6,
                    .UpdateCount = 1,
                    .ReadOnly = SAHPI_FALSE,
                    .NumAreas = 1
        },
        .my_idr_area[0] =
        {
                .idrareas = {
                        .AreaId = 1,
                        .Type = SAHPI_IDR_AREATYPE_CHASSIS_INFO,
                        .ReadOnly = SAHPI_FALSE,
                        .NumFields = 10,
                },

                .idrfields[0] =
                {
                        .AreaId = 1,
                        .FieldId = 1,
                        .Type = SAHPI_IDR_FIELDTYPE_CHASSIS_TYPE,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = def_text_buffer("Main Chassis")
                },
                {
                        .AreaId = 1,
                        .FieldId = 2,
                        .Type = SAHPI_IDR_FIELDTYPE_MFG_DATETIME,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = def_text_buffer("10/01/2004")
                },
                {
                        .AreaId = 1,
                        .FieldId = 3,
                        .Type = SAHPI_IDR_FIELDTYPE_MANUFACTURER,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = def_text_buffer("openHPI Inc.")
                },
                {
                        .AreaId = 1,
                        .FieldId = 4,
                        .Type = SAHPI_IDR_FIELDTYPE_PRODUCT_NAME,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = def_text_buffer("Imaginary HPI Machine")
                },
                {
                        .AreaId = 1,
                        .FieldId = 5,
                        .Type = SAHPI_IDR_FIELDTYPE_PRODUCT_VERSION, /* Model number? */
                        .ReadOnly = SAHPI_TRUE,
                        .Field = def_text_buffer("17")
                },
                {
                        .AreaId = 1,
                        .FieldId = 6,
                        .Type = SAHPI_IDR_FIELDTYPE_SERIAL_NUMBER,
                        .ReadOnly = SAHPI_TRUE,
                        .Field = {
                                .DataType = SAHPI_TL_TYPE_BINARY,
                                .Language = SAHPI_LANG_UNDEF,
                                .DataLength = 4,
                                .Data = "\001\002\003\004"
                        }
                },
                {
                        .AreaId = 1,
                        .FieldId = 7,
                        .Type = SAHPI_IDR_FIELDTYPE_PART_NUMBER,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = {
                                .DataType = SAHPI_TL_TYPE_ASCII6,
                                .Language = SAHPI_LANG_UNDEF,
                                .DataLength = 6,
                                .Data = "\051\334\246\051\334\246"
                        }
                },
                {
                        .AreaId = 1,
                        .FieldId = 8,
                        .Type = SAHPI_IDR_FIELDTYPE_FILE_ID,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = {
                                .DataType = SAHPI_TL_TYPE_BCDPLUS,
                                .Language = SAHPI_LANG_UNDEF,
                                .DataLength = 7,
                                .Data = "\001\043\105\147\211\253\315"
                        }
                },

                {
                        .AreaId = 1,
                        .FieldId = 9,
                        .Type = SAHPI_IDR_FIELDTYPE_ASSET_TAG,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = unicode_text_buffer("My Precious")
                },

                {
                        .AreaId = 1,
                        .FieldId = 10,
                        .Type = SAHPI_IDR_FIELDTYPE_CUSTOM,
                        .ReadOnly = SAHPI_FALSE,
                        .Field = def_text_buffer("Hot")
                },

                {}
        },
        {} /* Terminate Array */
           /* Arbitrarily define IdrId == 0 as end-of-inventory */
           /* instead of using a counter of some kind           */
},
{}
};


/*
 * Used if dummy represents 3 domains. In this case Resource i belongs to domain i.
 */
struct dummy_md_data {
        unsigned int hid;
        SaHpiDomainIdT dids[NUM_RESOURCES];
};


/************************************************************************/
/* System Event Log data                                                                */
/************************************************************************/
static struct dummy_rel {
        SaHpiEventLogEntryT entry;
} rel_entries = {
        .entry = {
                .EntryId = 1,
                .Timestamp = 0,
                .Event = {
                        .Source = 0,
                        .EventType = SAHPI_ET_HOTSWAP,
                        .Timestamp = 0,
                        .Severity = SAHPI_CRITICAL,
                        .EventDataUnion = {
                                .HotSwapEvent = {
                                        .HotSwapState = SAHPI_HS_STATE_INSERTION_PENDING,
                                        .PreviousHotSwapState = SAHPI_HS_STATE_NOT_PRESENT,
                                },
                        },
                },
        },

};



/* The event is used to fill event list */
static struct oh_event dummy_user_event = {
        .did = 1, /* set up domain id */
        .type = OH_ET_HPI,
        .u = {
                .hpi_event = {
                         .res.ResourceId = 0,
                         .rdr.RecordId = 0,
                         .event = {
                                 .Source = 0,
                                 .EventType = SAHPI_ET_USER,
                                 .Timestamp = 0,
                                 .Severity = SAHPI_CRITICAL,
                         },
                 },
        },
};

#if 0
/* The sensor event is used */
static struct oh_event dummy_user_event = {
        .did = 1, /* set up domain id */
        .type = OH_ET_RESOURCE_DEL,
        .u = {
                .res_event = {
                         .res.ResourceId = 0,
                         .domain = 0,
                         .entry = {
                                 .Source = 0,
                                 .EventType = SAHPI_ET_USER,
                                 .Timestamp = 0,
                                 .Severity = SAHPI_CRITICAL,
                         },
                 },
        },
};
#endif

static struct oh_event *__eventdup(const struct oh_event *event)
{
        struct oh_event *e;
        e = g_malloc0(sizeof(*e));
        if (!e) {
                dbg("Out of memory!");
                return NULL;
        }
        memcpy(e, event, sizeof(*e));
        return e;
}


static int __build_the_rpt_cache(struct oh_handler_state *oh_hnd)
{
        int i;
        int id;
        SaHpiRptEntryT res;
        SaHpiRptEntryT *rpt;
        SaHpiRdrT res_rdr;
        SaHpiRdrT *rdr;
        SaHpiEntityPathT root_ep;
        char *entity_root;

        entity_root = (char *)g_hash_table_lookup(oh_hnd->config,"entity_root");
        oh_encode_entitypath(entity_root, &root_ep);
        /* append entity root to resource entity paths */
        for (i=0; i < NUM_RESOURCES; i++) {
                memcpy(&res, &dummy_resources[i], sizeof(SaHpiRptEntryT));

                oh_concat_ep( &res.ResourceEntity, &root_ep);

                res.ResourceId = oh_uid_from_entity_path(&res.ResourceEntity);

                /* add the resource */
                if (oh_add_resource(oh_hnd->rptcache, &res, NULL, 0)) {
                        dbg("oh_add_resource failed for RESOURCE %d", i);
                        return -1;
                }

                /* save the resource id for tracking resource status */
                dummy_resource_status[i].ResourceId = res.ResourceId;

                dbg("oh_add_resource succeeded for RESOURCE %d", i);

        }
/*
        for (i = 0; dummy_inventory[i].idrinfo.IdrId != 0; i++) {
                int     j, k;
                char    bf[256];

                for (j = 0; j < dummy_inventory[i].idrinfo.NumAreas; j++) {
                        for (k = 0; k < dummy_inventory[i].my_idr_area[j].NumFields; k++) {
                                if (dummy_inventory[i].my_idr_area[j].idrfields[k].Field.DataType !=
                                                SAHPI_TL_TYPE_UNICODE) continue;
                        }
                }
        };
*/
        /* append entity root to rdrs entity paths */
        for (i=0; i < NUM_RDRS; i++) {
                memcpy(&res_rdr, &dummy_rdrs[i], sizeof(SaHpiRdrT));
                oh_concat_ep( &res_rdr.Entity, &root_ep);

                id = oh_uid_lookup(&res_rdr.Entity);

                if( id < 0 ) {
                        dbg("error looking up uid in dummy_open");
                        return-1;
                }

                /* add rdrs */
                if (oh_add_rdr(oh_hnd->rptcache, id, &res_rdr, NULL, 0)) {
                        dbg("oh_add_resource failed for RDR %d", i);
                        return -1;
                }

                dbg("oh_add_resource succeeded for RDR %d", i);
        };

        i = 0;
        rpt = oh_get_resource_next(oh_hnd->rptcache, SAHPI_FIRST_ENTRY);
        while (rpt) {
                if (rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) {
                        rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, SAHPI_FIRST_ENTRY);
                        while (rdr) {
                                memcpy(&(Save_rdr[i].rdr_default), rdr, sizeof(SaHpiRdrT));
                                Save_rdr[i].is_save = 0;
                                Save_rdr[i].res_Id = rpt->EntryId;
                                Save_rdr[i].rdr_type = rdr->RdrType;
                                switch (rdr->RdrType) {
                                        case SAHPI_SENSOR_RDR:
                                                Save_rdr[i].num = rdr->RdrTypeUnion.SensorRec.Num;
                                                break;
                                        case SAHPI_CTRL_RDR:
                                                Save_rdr[i].num = rdr->RdrTypeUnion.CtrlRec.Num;
                                                break;
                                        case SAHPI_INVENTORY_RDR:
                                                Save_rdr[i].num = rdr->RdrTypeUnion.InventoryRec.IdrId;
                                                break;
                                        case SAHPI_WATCHDOG_RDR:
                                                Save_rdr[i].num = rdr->RdrTypeUnion.WatchdogRec.WatchdogNum;
                                                break;
                                        case SAHPI_ANNUNCIATOR_RDR:
                                                Save_rdr[i].num =
                                                        rdr->RdrTypeUnion.AnnunciatorRec.AnnunciatorNum;
                                                break;
                                        default:
                                                rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId,
                                                        rdr->RecordId);
                                                continue;
                                };
                                i++;
                                rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, rdr->RecordId);
                        }
                };
                rpt = oh_get_resource_next(oh_hnd->rptcache, rpt->EntryId);
        }

        rpt = oh_get_resource_next(oh_hnd->rptcache, SAHPI_FIRST_ENTRY);
        while (rpt) {
                SaHpiAnnunciatorNumT    num;

                rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, SAHPI_FIRST_ENTRY);
                while (rdr) {
                        switch (rdr->RdrType) {
                                case SAHPI_ANNUNCIATOR_RDR:
                                        num = rdr->RdrTypeUnion.AnnunciatorRec.AnnunciatorNum;
                                        for (i = 0; i < ANNUN_MAX; i++)
                                                if (dummy_announs[i].Num == num) break;
                                        if (i >= ANNUN_MAX) break;
                                        dummy_announs[i].announs =
                                                (SaHpiAnnouncementT *)malloc(sizeof(SaHpiAnnouncementT) *
                                                        dummy_announs[i].count);
                                        memcpy(dummy_announs[i].announs, dummy_announs[i].def_announs,
                                                sizeof(SaHpiAnnouncementT) * dummy_announs[i].count);
                                        break;
                                default:
                                        break;
                        };
                        rdr = oh_get_rdr_next(oh_hnd->rptcache, rpt->EntryId, rdr->RecordId);
                };
                rpt = oh_get_resource_next(oh_hnd->rptcache, rpt->EntryId);
        };

        return(0);
}


static SaHpiDomainIdT create_new_domain(unsigned int hid, int j) {
        SaHpiTextBufferT tag = {
                .DataType = SAHPI_TL_TYPE_TEXT,
                .Language = SAHPI_LANG_ENGLISH,
        };

        snprintf((char *)(tag.Data), 15, "Dummy Child %d", j);
        tag.DataLength = strlen((char *)(tag.Data)) + 1;
        return oh_request_new_domain(hid, &tag, 0, 0, 0);
}


static void *dummy_open(GHashTable *handler_config)
{
        struct oh_handler_state *i;
        char *tok = NULL;
        char *md_str = NULL;
        unsigned int *hidp;
        int j;
        struct dummy_md_data *md_data;



        tok = g_hash_table_lookup(handler_config, "entity_root");
        hidp = g_hash_table_lookup(handler_config, "handler-id");
        md_str = g_hash_table_lookup(handler_config, "MultipleDomains");

        if (!handler_config) {
                dbg("GHashTable *handler_config is NULL!");
                return(NULL);
        }

        trace("%s, %s, %s",
              (char *)g_hash_table_lookup(handler_config, "plugin"),
              (char *)g_hash_table_lookup(handler_config, "name"),
              tok);

        if (!tok) {
                dbg("entity_root is needed and not present");
                return(NULL);
        }

        i = g_malloc0(sizeof(*i));
        if (!i) {
                dbg("out of memory");
                return( (struct oh_handler_state *)NULL );
        }

        /* save the handler config has table it holds   */
        /* the openhpi.conf file config info            */
        i->config = handler_config;

        /* initialize hashtable pointer */
        i->rptcache = (RPTable *)g_malloc0(sizeof(RPTable));
        oh_init_rpt(i->rptcache);

        /* fill in the local rpt cache */
        __build_the_rpt_cache(i);
        /* associate the static hotswap_event data with a resource */
        hotswap_event[0].u.hpi_event.res = dummy_resources[1];
        hotswap_event[1].u.hpi_event.res = dummy_resources[1];


        /* make sure the glib threading subsystem is initialized */
        if (!g_thread_supported ()) {
                g_thread_init (NULL);
                printf("thread not initialized\n");
                g_free(i);
                return NULL;
        }

        /* initialize mutex */
//        i->handler_lock = g_malloc0(sizeof(GStaticRecMutex));
//        if (!i->handler_lock) {
//                dbg("GStaticRecMutex: out of memory");
//                return NULL;
//        }
//        g_static_rec_mutex_init (i->handler_lock);

        /* create event queue for async events*/
        if ( !(i->eventq_async = g_async_queue_new()) ) {
                printf("g_async_queue_new failed\n");
                g_free(i);
                return NULL;
        }

#ifdef DUMMY_THREADED
        /* add to oh_handler_state */
        // GThread *thread_handle;
        GError **e = NULL;

        /* spawn a thread */
        if ( !(i->thread_handle = g_thread_create(
                                        event_thread,
                                        i,  /* oh_handler_state */
                                        TRUE,
                                        e)) ) {
             printf("g_thread_create failed\n");
             g_free(i);
             return NULL;
        }
#endif

        if (hidp == NULL || md_str == NULL) {
                /* dummy represents the single domain */
                return( (void *)i );
        }

        /* dummy represents multiple domains */
        md_data = (struct dummy_md_data *)g_malloc0(
                                        sizeof(struct dummy_md_data));
        if (md_data == NULL) {
                dbg("Out of memory");
                g_free(i);
                return NULL;
        }
        md_data->hid = *hidp;
        for (j = 0; j < NUM_RESOURCES; j++) {
                md_data->dids[j] = create_new_domain(*hidp, j);
                if (md_data->dids[j] == 0) {
                        printf("Couldn't create domain %d\n", j);
                        break;
                }
        }
        if (j < NUM_RESOURCES && md_data->dids[j] == 0) {
                for (j = j; j >= 0; j--) {
                        if (md_data->dids[j] != 0) {
                                oh_request_domain_delete(*hidp, md_data->dids[j]);
                        } else {
                                dbg("Couldn't create domain %d", j);
                        }
                }
                g_free(md_data);
                g_free(i);
                return NULL;
        }
        i->data = (void *)md_data;


        return( (void *)i );
}

static void dummy_close(void *hnd)
{
        //struct oh_handler_state *inst = hnd;

        /* TODO: free the rptcache RPTable *rptcache */

        /* destroy mutex */
//        g_static_rec_mutex_free(inst->handler_lock);

        /* destroy async queue */

        /* signal thread to exit and wait */
        //gpointer rtval_thread;
        // need to signal thread to close, possibly extra flag in oh_handler_state
        //rtval_thread = g_thread_join(inst->thread_handle);
        //dbg("rtval_thread [%d]\n", *(int*)rtval_thread);

        /* TODO: free the GHashTabel GHashTable *config */

        /* TODO: free *data void *data */

        free(hnd);

        return;
}


static struct oh_event *remove_resource(struct oh_handler_state *inst)
{
        SaHpiRptEntryT *rpt_e = NULL;
        SaHpiRptEntryT *rpt_e_pre = NULL;

        static struct oh_event e;

        memset(&e, 0, sizeof(e));

        /* get the last resouce in our rptache  */
        /* since we know it has no rdrs         */
        rpt_e = oh_get_resource_next(inst->rptcache, SAHPI_FIRST_ENTRY);
        while(rpt_e) {
                rpt_e_pre = rpt_e;
                rpt_e = oh_get_resource_next(inst->rptcache, rpt_e->ResourceId);
        }

        /* generate remove resource event */
        if(rpt_e_pre) {
                e.did = 1;
                e.type = OH_ET_RESOURCE_DEL;
                e.u.res_event.entry.ResourceId = rpt_e_pre->ResourceId;
                /*memcpy(&e.u.res_event.entry, rpt_e_pre, sizeof(SaHpiRptEntryT));*/
        }

        dbg("**** ResourceId %d ******", e.u.res_event.entry.ResourceId);

        return(&e);

}

static struct oh_event *add_resource(struct oh_handler_state *inst)
{
        SaHpiRptEntryT *rpt_e = NULL;
        SaHpiRptEntryT *rpt_e_pre = NULL;

        static struct oh_event e;

        memset(&e,0,sizeof(e));


        /* get the last resouce in our rptache  */
        /* since we know it has no rdrs         */
        rpt_e = oh_get_resource_next(inst->rptcache, SAHPI_FIRST_ENTRY);
        while(rpt_e) {
                rpt_e_pre = rpt_e;
                rpt_e = oh_get_resource_next(inst->rptcache, rpt_e->ResourceId);
        }

        /* generate remove resource event */
        if(rpt_e_pre) {
                e.did = 1;
                e.type = OH_ET_RESOURCE;
                memcpy(&e.u.res_event.entry, rpt_e_pre, sizeof(SaHpiRptEntryT));
        }

        dbg("**** ResourceId %d ******", e.u.res_event.entry.ResourceId);

        return(&e);

}


static struct oh_event *user_event_to_domain(SaHpiDomainIdT did)
{
        static struct oh_event e = {
                .type = OH_ET_HPI,
                .u = {
                        .hpi_event = {
                                .res.ResourceId = 0,
                                .rdr.RecordId = 0,
                                .event = {
                                        .Source = 0,
                                        .EventType = SAHPI_ET_USER,
                                        .Timestamp = 0,
                                        .Severity = SAHPI_CRITICAL,
                                },
                        },
                },
        };
        SaHpiTextBufferT *tb;

        e.did = did;
        tb = &e.u.hpi_event.event.EventDataUnion.UserEvent.UserEventData;
        tb->DataType = SAHPI_TL_TYPE_TEXT;
        tb->Language = SAHPI_LANG_ENGLISH;
        snprintf((char *)(tb->Data), SAHPI_MAX_TEXT_BUFFER_LENGTH,
                "User Event for domain %d", did);
        tb->DataLength = strlen((char *)(tb->Data)) + 1;
        return &e;
}



static int dummy_get_event(void *hnd, struct oh_event *event)
{
        struct oh_handler_state *inst = hnd;

        struct oh_event *e = NULL;
        struct oh_event *qse = NULL;

        SaHpiRptEntryT *rpt_entry = NULL;

        static unsigned int toggle = 0;
        static unsigned int count = 0;
        static unsigned int dmn_for_event = 0;

        if ( (qse = g_async_queue_try_pop(inst->eventq_async)) ) {

                trace("List has an event, send it up");

                memcpy(event, qse, sizeof(*event));

                event->did = oh_get_default_domain_id();

                g_free(qse);

                dbg("*************** dummy_get_event, g_async_queue_try_pop");

                return(1);

        } else if (count == 0) {
                trace("List is empty, getting next resource");

                count++;

                /* just stick this user event on the first resource */
                rpt_entry = oh_get_resource_next(inst->rptcache, SAHPI_FIRST_ENTRY);
                if (!rpt_entry) {
                        g_free(e);
                        return(-1);
                }

                dummy_user_event.did = oh_get_default_domain_id();
                *event = dummy_user_event;
                memcpy(&(event->u.hpi_event.res),&rpt_entry,sizeof(rpt_entry));

                return(1);

        } else if (count == 1) {
                trace("list is empty, when count is even gen new event");
                count++;
                return(0);
        }

//        g_static_rec_mutex_lock (inst->handler_lock);

        toggle++;

#ifndef DUMMY_THREADED
        if( (toggle%3) == 0 ) {
                /* once initial reporting of events toggle between      */
                /* removing and adding resource, removes resource 3     */
                /* since it has no rdr's to add back later              */
                if ( (count%2) == 0 ) {
                        count++;
                        dbg("**** EVEN ****, remove the resource");
                        if ( (e = remove_resource(inst)) ) {
                                *event = *e;
//                                g_static_rec_mutex_unlock (inst->handler_lock);
                                return(1);
                        }
                } else {
                        count++;
                        dbg("**** ODD ****, add the resource");
                        if ( (e = add_resource(inst)) ) {
                                *event = *e;
//                                g_static_rec_mutex_unlock (inst->handler_lock);
                                return(1);
                        }
                }
        }

        if (toggle%5 == 0 && inst->data != NULL) {
                /* dummy represents several domains */
                /* lets send USER event to one of them */
                struct dummy_md_data *md = (struct dummy_md_data *)inst->data;
                int id = dmn_for_event%NUM_RESOURCES;
                SaErrorT rv;

                if (md->dids[id] == 0) {
                        /* this domain released. Let's create it again */
                        md->dids[id] = create_new_domain(md->hid, id);
                        if (md->dids[id] == 0) {
                                dbg("+++ couldn't create new domain %d", id);
                                dmn_for_event++;
                                return -1;
                        } else {
                                dbg("+++ child domain %d created = %d",
                                        id, md->dids[id]);
                        }
                }
                *event = *user_event_to_domain(md->dids[id]);
                dbg("+++ send user event to domain %d",md->dids[id]);
                dmn_for_event++;
                id = dmn_for_event%NUM_RESOURCES;
                if (toggle%50 == 0 && md->dids[id] != 0) {
                        /* release this domain */
                        dbg("+++ release domain %d(%d)", id, md->dids[id]);
                        rv = oh_request_domain_delete(md->hid, md->dids[id]);
                        if (rv != SA_OK) {
                                dbg("+++ couldn't release domain %d(%d)", id,
                                        md->dids[id]);
                        } else {
                                md->dids[id] = 0;
                        }
                }
                return (1);
        }

#endif
//        g_static_rec_mutex_unlock (inst->handler_lock);

        return(-1);

}

static int dummy_discover_resources(void *hnd)
{
        static int done_once = FALSE;
        struct oh_handler_state *inst = hnd;
        struct oh_event event;
        SaHpiRptEntryT *rpt_entry;
        SaHpiRdrT      *rdr_entry;

        /* create a counter, on even remove your favorite resource
           on odd add it back */
        if (!done_once) {

                /* get the first rpt entry */

                rpt_entry = oh_get_resource_next(inst->rptcache, SAHPI_FIRST_ENTRY);

                while (rpt_entry) {
                        /*dbg("here resource event id %d", rpt_entry->ResourceId);*/
                        memset(&event, 0, sizeof(event));
                        event.type = OH_ET_RESOURCE;
                        memcpy(&event.u.res_event.entry, rpt_entry, sizeof(SaHpiRptEntryT));
                        g_async_queue_push(inst->eventq_async, __eventdup(&event));


                        /* get every resource rdr's */
                        rdr_entry = oh_get_rdr_next(inst->rptcache,
                                                    rpt_entry->ResourceId, SAHPI_FIRST_ENTRY);
                        while (rdr_entry) {
                                /*dbg("here rdr event id %d", rdr_entry->RecordId);*/
                                memset(&event, 0, sizeof(event));
                                event.type = OH_ET_RDR;
                                event.u.rdr_event.parent = rpt_entry->ResourceId;
                                memcpy(&event.u.rdr_event.rdr, rdr_entry, sizeof(SaHpiRdrT));
                                g_async_queue_push(inst->eventq_async, __eventdup(&event));

                                rdr_entry = oh_get_rdr_next(inst->rptcache,
                                                            rpt_entry->ResourceId, rdr_entry->RecordId);
                        }
                        /* get any resource rdr's end */


                        rpt_entry = oh_get_resource_next(inst->rptcache, rpt_entry->ResourceId);
                }

                done_once = TRUE;
        }

        return 0;
}


static int dummy_discover_domain_resources(void *hnd, SaHpiDomainIdT did)
{
        dbg("dummy_discover_domain_resources called for doamin %d", did);
        return dummy_discover_resources(hnd);
}


static SaErrorT dummy_set_resource_tag(void *hnd, SaHpiResourceIdT id, SaHpiTextBufferT *tag)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRptEntryT *resource = NULL;

        if (!tag)
                return SA_ERR_HPI_INVALID_PARAMS;

        resource = oh_get_resource_by_id(inst->rptcache, id);
        if (!resource) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        memcpy(&resource->ResourceTag, tag, sizeof(SaHpiTextBufferT));

        return SA_OK;
}

static SaErrorT dummy_set_resource_severity(void *hnd, SaHpiResourceIdT id, SaHpiSeverityT sev)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRptEntryT *resource = NULL;

        resource = oh_get_resource_by_id(inst->rptcache, id);
        if (!resource) {
                return SA_ERR_HPI_NOT_PRESENT;
        }

        resource->ResourceSeverity = sev;

        return SA_OK;
}

static int sel_enabled;

static int dummy_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info)
{
        info->Entries                   = 0;
        info->Size                      = 0xFFFFFFFF;
        info->UserEventMaxSize          = SAHPI_MAX_TEXT_BUFFER_LENGTH;
        info->UpdateTimestamp           = 0;
        oh_gettimeofday(&info->CurrentTime);
        info->Enabled                   = sel_enabled;
        info->OverflowFlag              = 0;
        info->OverflowAction            = SAHPI_EL_OVERFLOW_DROP;

        return SA_OK;
}

static int dummy_set_sel_time(void *hnd, SaHpiResourceIdT id, SaHpiTimeT time)
{
        return SA_OK;
}

static int dummy_add_sel_entry(void *hnd, SaHpiResourceIdT id, const SaHpiEventT *Event)
{
        dbg("TODO: dummy_add_sel_entry(), need to set res based on id");


#if 0
        struct dummy_instance *inst = hnd;
        struct dummy_rel *rel;
        struct oh_event *e;

        e = malloc(sizeof(*e));
        if (!e) {
                dbg("Out of memory");
                return -1;
        }

        rel = malloc(sizeof(*rel));
        if (!rel) {
                dbg("Out of memory");
                return -1;
        }

        memcpy(&rel->entry, Event, sizeof(*Event));
        memset(e, 0, sizeof(*e));
        e->type = OH_ET_REL;
        dbg("TODO: need to set res based on id");
        /*e->u.rel_event.rel.parent = id;*/

        e->u.rel_event.rel.oid.ptr = rel;

        /* new */
        g_async_queue_push(inst->eventq_async, e);

#endif
        return SA_ERR_HPI_INVALID_CMD;
}

static int dummy_clear_el(void *hnd, SaHpiResourceIdT id)
{
        return(0);
}

static SaErrorT dummy_reset_el_overflow(void *hnd, SaHpiResourceIdT id)
{
        return(0);
}

static int dummy_get_sel_entry(void *hnd,
                               SaHpiResourceIdT id,
                               SaHpiEventLogEntryIdT current,
                               SaHpiEventLogEntryIdT *prev,
                               SaHpiEventLogEntryIdT *next,
                               SaHpiEventLogEntryT *entry,
                               SaHpiRdrT *rdr,
                               SaHpiRptEntryT  *rptentry)

{
        dbg("dummy_get_sel_entry(): This is a very bad implementation");
        if (current != SAHPI_OLDEST_ENTRY &&
            current != SAHPI_NEWEST_ENTRY &&
            current != rel_entries.entry.EntryId)
                return SA_ERR_HPI_NOT_PRESENT;

        *prev = SAHPI_NO_MORE_ENTRIES;
        *next = SAHPI_NO_MORE_ENTRIES;
        memcpy(entry, &rel_entries.entry, sizeof(SaHpiEventLogEntryT));

        if (rdr) {
                SaHpiRdrT l_rdr;
                memset(&l_rdr, 0, sizeof(SaHpiRdrT));
                l_rdr.RdrType = SAHPI_NO_RECORD;  /* Redundant because we have set the  */
                                                  /* whole structure to zero, do it for */
                                                  /* clarity.                           */
                *rdr = l_rdr;
        }

        if (rptentry) {
                SaHpiRptEntryT l_rptentry;
                /* l_rptentry.ResourceCapabilities = 0 */
                memset(&l_rptentry, 0, sizeof(SaHpiRptEntryT));

                *rptentry = l_rptentry;
        }

        return 0;
}

/************************************************************************/
/* Sensor functions                                                     */
/************************************************************************/
static int dummy_get_sensor_reading(void *hnd, SaHpiResourceIdT id,
                           SaHpiSensorNumT num,
                           SaHpiSensorReadingT *data,
                           SaHpiEventStateT    *state)
{
        int rval = -1;
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);

        while ( (rdr->RdrTypeUnion.CtrlRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }

        /* NULL is a valid value for data, check before use */
        if (rdr && data) {
                memcpy(data,
                       &dummy_sensors[rdr->RdrTypeUnion.SensorRec.Num - 1].reading,
                       sizeof(SaHpiSensorReadingT));
                rval = 0;
        }

        /* NULL is a valid value for state, check before use */
        /* Setting it to NOT SUPPORTED (0), for now  */
        if(state)
                memset(state, 0, sizeof(SaHpiEventStateT));

        return(rval);
}

static int dummy_get_sensor_thresholds(void *hnd, SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       SaHpiSensorThresholdsT *thres)
{
        memset(thres, 0, sizeof(*thres));
        memcpy(thres, &dummy_sensors[num - 1].thresholds, sizeof(SaHpiSensorThresholdsT));

        return 0;
}


static int reading_has_values(const SaHpiSensorReadingT *reading)
{
        if (reading->IsSupported)
                return 1;
        return 0;
}

static void reading_cpy(SaHpiSensorReadingT *r1,
                        const SaHpiSensorReadingT *r2)
{
        if (reading_has_values(r2))
                memcpy(r1, r2, sizeof(*r1));
}

static int dummy_set_sensor_thresholds(void *hnd, SaHpiResourceIdT id,
                                       SaHpiSensorNumT num,
                                       const SaHpiSensorThresholdsT *thres)
{
        dbg(" ********* dummy_set_sensor_thresholds *******");
        reading_cpy(&dummy_sensors[num - 1].thresholds.LowCritical,
                    &thres->LowCritical);

        reading_cpy(&dummy_sensors[num - 1].thresholds.LowMajor,
                    &thres->LowMajor);

        reading_cpy(&dummy_sensors[num - 1].thresholds.LowMinor,
                    &thres->LowMinor);

        reading_cpy(&dummy_sensors[num - 1].thresholds.UpCritical,
                    &thres->UpCritical);

        reading_cpy(&dummy_sensors[num - 1].thresholds.UpMajor,
                    &thres->UpMajor);

        reading_cpy(&dummy_sensors[num - 1].thresholds.UpMinor,
                    &thres->UpMinor);

        reading_cpy(&dummy_sensors[num - 1].thresholds.PosThdHysteresis,
                    &thres->PosThdHysteresis);

        reading_cpy(&dummy_sensors[num - 1].thresholds.NegThdHysteresis,
                    &thres->NegThdHysteresis);

        return 0;
}

static int dummy_get_sensor_event_enabled(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiBoolT *enabled)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        dbg(" ********* dummy_get_sensor_event_enables *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrTypeUnion.SensorRec.EnableCtrl != SAHPI_TRUE)
                return(SA_ERR_HPI_READ_ONLY);
        *enabled = dummy_sensors[num - 1].enabled;
        return 0;
}

static int dummy_set_sensor_event_enabled(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          const SaHpiBoolT enabled)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        dbg(" ********* dummy_set_sensor_event_enables *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrTypeUnion.SensorRec.EnableCtrl != SAHPI_TRUE)
                return(SA_ERR_HPI_READ_ONLY);
        dummy_sensors[num - 1].enabled = enabled;
        return 0;
}

static int dummy_get_sensor_enable(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiBoolT *enabled)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        dbg(" ********* dummy_get_sensor_enabled *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrTypeUnion.SensorRec.EnableCtrl != SAHPI_TRUE)
                return(SA_ERR_HPI_READ_ONLY);
        *enabled = dummy_sensors[num - 1].sen_enable;
        return 0;
}

static int dummy_set_sensor_enable(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiBoolT enabled)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

       dbg(" ********* dummy_set_sensor_enabled *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrTypeUnion.SensorRec.EnableCtrl != SAHPI_TRUE)
                return(SA_ERR_HPI_READ_ONLY);
        dummy_sensors[num - 1].sen_enable = enabled;
        return 0;
}

static int dummy_get_sensor_event_masks(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiEventStateT *assert,
                                          SaHpiEventStateT *deassert)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        dbg(" ********* dummy_get_sensor_event_masks *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        *assert = dummy_sensors[num - 1].assert;
        *deassert = dummy_sensors[num - 1].deassert;
        return 0;
}

static int dummy_set_sensor_event_masks(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          SaHpiSensorEventMaskActionT act,
                                          SaHpiEventStateT assert,
                                          SaHpiEventStateT deassert)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        dbg(" ********* dummy_set_sensor_event_masks *******");
        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);
        while ( (rdr->RdrTypeUnion.SensorRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }
        if (rdr == NULL)
                return(SA_ERR_HPI_NOT_PRESENT);
        if (act == SAHPI_SENS_ADD_EVENTS_TO_MASKS) {
                dummy_sensors[num - 1].assert |= assert;
                dummy_sensors[num - 1].deassert |= deassert;
                return(0);
        };
        if (act == SAHPI_SENS_REMOVE_EVENTS_FROM_MASKS) {
                dummy_sensors[num - 1].assert &= ~assert;
                dummy_sensors[num - 1].deassert &= ~deassert;
                return(0);
        };
        return SA_ERR_HPI_INVALID_PARAMS;
}

/************************************************************************/
/* Control functions                                                    */
/************************************************************************/

static int dummy_get_control_state(void *hnd, SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT *mode,
                                   SaHpiCtrlStateT *state)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiCtrlRecT           *ctrl;
        SaHpiCtrlTypeT          type;
        SaHpiCtrlModeT          md;
        SaHpiCtrlStateT         st;
        SaHpiCtrlRecDigitalT    *digit;
        SaHpiCtrlRecDiscreteT   *discr;
        SaHpiCtrlRecAnalogT     *analog;
        SaHpiCtrlRecStreamT     *stream;
        SaHpiCtrlRecTextT       *text;
        SaHpiCtrlRecOemT        *oem;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_CTRL_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrType != SAHPI_CTRL_RDR) return(SA_ERR_HPI_NOT_PRESENT);
        ctrl = &(rdr->RdrTypeUnion.CtrlRec);
        md = ctrl->DefaultMode.Mode;
        if (mode != (SaHpiCtrlModeT *)NULL) *mode = md;
        type = ctrl->Type;
        st.Type = type;
        switch (type) {
                case SAHPI_CTRL_TYPE_DIGITAL:
                        digit = &(ctrl->TypeUnion.Digital);
                        st.StateUnion.Digital = digit->Default;
                        break;
                case SAHPI_CTRL_TYPE_DISCRETE:
                        discr = &(ctrl->TypeUnion.Discrete);
                        st.StateUnion.Discrete = discr->Default;
                        break;
                case SAHPI_CTRL_TYPE_ANALOG:
                        analog = &(ctrl->TypeUnion.Analog);
                        st.StateUnion.Analog = analog->Default;
                        break;
                case SAHPI_CTRL_TYPE_STREAM:
                        stream = &(ctrl->TypeUnion.Stream);
                        st.StateUnion.Stream = stream->Default;
                        break;
                case SAHPI_CTRL_TYPE_TEXT:
                        text = &(ctrl->TypeUnion.Text);
                        st.StateUnion.Text = text->Default;
                        break;
                case SAHPI_CTRL_TYPE_OEM:
                        oem = &(ctrl->TypeUnion.Oem);
                        st.StateUnion.Oem = oem->Default;
                        break;
        };
        if (state != (SaHpiCtrlStateT *)NULL) *state = st;

        return 0;
}

static int dummy_set_control_state(void *hnd, SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT mode,
                                   SaHpiCtrlStateT *state)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiCtrlRecT           *ctrl;
        SaHpiCtrlTypeT          type;
        SaHpiCtrlRecDigitalT    *digit;
        SaHpiCtrlRecDiscreteT   *discr;
        SaHpiCtrlRecAnalogT     *analog;
        SaHpiCtrlRecStreamT     *stream;
        SaHpiCtrlRecTextT       *text;
        SaHpiCtrlRecOemT        *oem;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_CTRL_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);
        if (rdr->RdrType != SAHPI_CTRL_RDR) return(SA_ERR_HPI_NOT_PRESENT);
        ctrl = &(rdr->RdrTypeUnion.CtrlRec);
        if (ctrl->DefaultMode.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        if (mode == SAHPI_CTRL_MODE_AUTO) {
                ctrl->DefaultMode.Mode = mode;
                return(SA_OK);
        };
        if (mode != SAHPI_CTRL_MODE_MANUAL) return(SA_ERR_HPI_INVALID_PARAMS);
        if (state == (SaHpiCtrlStateT *)NULL) return(SA_ERR_HPI_INVALID_PARAMS);
        type = ctrl->Type;
        if (state->Type != type) return(SA_ERR_HPI_INVALID_DATA);
        switch (type) {
                case SAHPI_CTRL_TYPE_DIGITAL:
                        digit = &(ctrl->TypeUnion.Digital);
                        if (oh_lookup_ctrlstatedigital(state->StateUnion.Digital) == NULL)
                                return(SA_ERR_HPI_INVALID_PARAMS);
                        if (state->StateUnion.Digital == digit->Default) {
                                if ((digit->Default == SAHPI_CTRL_STATE_PULSE_ON) ||
                                        (digit->Default == SAHPI_CTRL_STATE_PULSE_OFF))
                                        return(SA_ERR_HPI_INVALID_REQUEST);
                        };
                        digit->Default = state->StateUnion.Digital;
                        break;
                case SAHPI_CTRL_TYPE_DISCRETE:
                        discr = &(ctrl->TypeUnion.Discrete);
                        discr->Default = state->StateUnion.Discrete;
                        break;
                case SAHPI_CTRL_TYPE_ANALOG:
                        analog = &(ctrl->TypeUnion.Analog);
                        if ((state->StateUnion.Analog < analog->Min) ||
                                (state->StateUnion.Analog > analog->Max))
                                return(SA_ERR_HPI_INVALID_DATA);
                        analog->Default = state->StateUnion.Analog;
                        break;
                case SAHPI_CTRL_TYPE_STREAM:
                        stream = &(ctrl->TypeUnion.Stream);
                        if (state->StateUnion.Stream.StreamLength > SAHPI_CTRL_MAX_STREAM_LENGTH)
                                return(SA_ERR_HPI_INVALID_PARAMS);
                        stream->Default = state->StateUnion.Stream;
                        break;
                case SAHPI_CTRL_TYPE_TEXT:
                        text = &(ctrl->TypeUnion.Text);
                        if (oh_lookup_texttype(state->StateUnion.Text.Text.DataType) == NULL)
                                return(SA_ERR_HPI_INVALID_DATA);
                        text->Default = state->StateUnion.Text;
                        break;
                case SAHPI_CTRL_TYPE_OEM:
                        oem = &(ctrl->TypeUnion.Oem);
                        oem->Default = state->StateUnion.Oem;
                        break;
        }
        return 0;
}


/************************************************************************/
/* Inventory functions                                                  */
/************************************************************************/
static SaErrorT dummy_get_idr_info( void *hnd,
                SaHpiResourceIdT        ResourceId,
                SaHpiIdrIdT             IdrId,
                SaHpiIdrInfoT          *IdrInfo)
{
        SaErrorT  rv = SA_OK;
        int i = 0;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        SaHpiBoolT foundmatch = SAHPI_FALSE;

        if (rdr != NULL) {

                while (dummy_inventory[i].idrinfo.IdrId != NO_ID) {
                        if (dummy_inventory[i].idrinfo.IdrId == IdrId) {
                                memcpy(IdrInfo, &(dummy_inventory[i].idrinfo.IdrId), sizeof(SaHpiIdrInfoT));
                                foundmatch = SAHPI_TRUE;
                                break;
                        } else
                                i++;
                }

                if (!foundmatch)
                        rv = SA_ERR_HPI_NOT_PRESENT;

        } else {
                rv = SA_ERR_HPI_NOT_PRESENT;
        }

        return rv;
}

static SaErrorT dummy_get_idr_area_header( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiIdrAreaTypeT        AreaType,
                SaHpiEntryIdT            AreaId,
                SaHpiEntryIdT           *NextAreaId,
                SaHpiIdrAreaHeaderT     *Header)
{
        SaErrorT  rv = SA_OK;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        SaHpiIdrAreaTypeT        thisAreaType;
        SaHpiEntryIdT            thisAreaId = 0;
        SaHpiBoolT foundArea = SAHPI_FALSE;
        int num_areas, i;

        if (rdr != NULL) {
                struct dummy_inventories *s;

                for (i = 0; ; i++) {
                        if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                        if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                        else {
                                s = dummy_inventory + i;
                                break;
                        }
                };

                num_areas =  s->idrinfo.NumAreas;
                for (i = 0; i < num_areas; i++) {
                        thisAreaId = s->my_idr_area[i].idrareas.AreaId;
                        thisAreaType = s->my_idr_area[i].idrareas.Type;
                        if ( ((AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) && (AreaId == SAHPI_FIRST_ENTRY)) ||
                             ((thisAreaType == AreaType) && ((AreaId == SAHPI_FIRST_ENTRY) || (AreaId == thisAreaId))) ||
                             ((AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED) && (thisAreaId == AreaId)) )
                        {
                                memcpy(Header, &(s->my_idr_area[i].idrareas) , sizeof(SaHpiIdrInfoT));
                                foundArea = SAHPI_TRUE;
                                break;
                        }
                };

                i++;
                if (foundArea) {
                        foundArea = SAHPI_FALSE;
                        if (i < num_areas) {
                                do {
                                       thisAreaType = s->my_idr_area[i].idrareas.Type;
                                        if ((thisAreaType == AreaType) || (AreaType == SAHPI_IDR_AREATYPE_UNSPECIFIED)) {
                                                *NextAreaId = s->my_idr_area[i].idrareas.AreaId;
                                                foundArea = SAHPI_TRUE;
                                                break;
                                        }
                                        i++;
                                } while (i < num_areas);

                                if (!foundArea)
                                        *NextAreaId = SAHPI_LAST_ENTRY;

                        } else  {
                                *NextAreaId = SAHPI_LAST_ENTRY;
                        }

                } else
                        rv = SA_ERR_HPI_NOT_PRESENT;

        } else {
                rv = SA_ERR_HPI_NOT_PRESENT;
        }

        return rv;

}

static SaErrorT dummy_add_idr_area( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiIdrAreaTypeT        AreaType,
                SaHpiEntryIdT           *AreaId)

{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT               *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int                     num_areas, i;
        SaHpiEntryIdT           lastId;

        struct dummy_inventories *s;

        if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;
        for (i = 0; ; i++) {
                if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                else {
                        s = dummy_inventory + i;
                        break;
                }
        };

        if (s->idrinfo.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        num_areas =  s->idrinfo.NumAreas;
        if (num_areas >= IDR_AREAS_MAX) return(SA_ERR_HPI_OUT_OF_SPACE);
        if (num_areas == 0) lastId = 0;
        else  lastId = s->my_idr_area[num_areas - 1].idrareas.AreaId;
        lastId++;
        s->my_idr_area[num_areas].idrareas.AreaId = lastId;
        s->my_idr_area[num_areas].idrareas.Type = AreaType;
        s->my_idr_area[num_areas].idrareas.ReadOnly = SAHPI_FALSE;
        s->my_idr_area[num_areas].idrareas.NumFields = 0;
        s->idrinfo.NumAreas++;
        return SA_OK;
}

static SaErrorT dummy_del_idr_area( void *hnd,
                SaHpiResourceIdT       ResourceId,
                SaHpiIdrIdT            IdrId,
                SaHpiEntryIdT          AreaId)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT               *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int                     num_areas, num_fields, i, j;

        struct dummy_inventories *s;

        if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;
        for (i = 0; ; i++) {
                if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                else {
                        s = dummy_inventory + i;
                        break;
                }
        };

        if (s->idrinfo.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        num_areas =  s->idrinfo.NumAreas;
        for (i = 0; i < num_areas; i++) {
                if (s->my_idr_area[i].idrareas.AreaId == AreaId)
                        break;
        };
        if (i >= num_areas) return(SA_ERR_HPI_NOT_PRESENT);
        num_fields = s->my_idr_area[i].idrareas.NumFields;
        for (j = 0; j < num_fields; j++) {
                if (s->my_idr_area[i].idrfields[j].ReadOnly)
                        return(SA_ERR_HPI_READ_ONLY);
        };
        if (i != (num_areas - 1)) {
                for (j = i; j < (num_areas - 1); j++) {
                        s->my_idr_area[j].idrareas = s->my_idr_area[j + 1].idrareas;
                }
        };
        s->idrinfo.NumAreas--;
        return SA_OK;
}

static SaErrorT dummy_get_idr_field( void *hnd,
                SaHpiResourceIdT       ResourceId,
                SaHpiIdrIdT             IdrId,
                SaHpiEntryIdT           AreaId,
                SaHpiIdrFieldTypeT      FieldType,
                SaHpiEntryIdT           FieldId,
                SaHpiEntryIdT          *NextFieldId,
                SaHpiIdrFieldT         *Field)
{
        SaErrorT  rv = SA_OK;
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int num_areas = 0, area_index = 0, field_index = 0, num_fields = 0;
        SaHpiEntryIdT            thisAreaId;
        SaHpiIdrFieldTypeT      thisFieldType;
        SaHpiEntryIdT           thisFieldId;
        SaHpiBoolT foundArea = FALSE;
        SaHpiBoolT foundField = FALSE;
        struct dummy_idr_area *thisArea;


        if (rdr != NULL) {
                struct dummy_inventories        *s;
                int                             i;

                for (i = 0; ; i++) {
                        if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                        if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                        else {
                                s = dummy_inventory + i;
                                break;
                        }
                };

                num_areas =  s->idrinfo.NumAreas;
                area_index = 0;
                do {

                        thisAreaId = s->my_idr_area[area_index].idrareas.AreaId;
                        if ((thisAreaId == AreaId) || (AreaId == SAHPI_FIRST_ENTRY)) {
                                foundArea = SAHPI_TRUE;
                                break;
                        }
                        area_index++;
                } while (area_index < num_areas);


                if (foundArea) {
                        num_fields = s->my_idr_area[area_index].idrareas.NumFields;
                        if (num_fields == 0) return SA_ERR_HPI_NOT_PRESENT;
                        thisArea   = &(s->my_idr_area[area_index]);

                        do {
                                thisFieldType = thisArea->idrfields[field_index].Type;
                                thisFieldId = thisArea->idrfields[field_index].FieldId;
                                if (((thisFieldId == FieldId) || (FieldId == SAHPI_FIRST_ENTRY))
                                   && ((thisFieldType == FieldType) || (FieldType == SAHPI_IDR_FIELDTYPE_UNSPECIFIED)))
                                {
                                        memcpy(Field, &(thisArea->idrfields[field_index]), sizeof(SaHpiIdrFieldT)) ;
                                        foundField = SAHPI_TRUE;
                                        break;
                                }
                                field_index++;
                        } while (field_index < num_fields);

                        field_index++;
                        if (foundField) {
                                foundField = SAHPI_FALSE;
                                if (field_index < num_fields) {
                                        do {
                                                thisFieldType = thisArea->idrfields[field_index].Type;
                                                if ((thisFieldType == FieldType) || (FieldType == SAHPI_IDR_FIELDTYPE_UNSPECIFIED))
                                                {
                                                        *NextFieldId = thisArea->idrfields[field_index].FieldId;
                                                        foundField = SAHPI_TRUE;
                                                        break;
                                                }
                                                field_index++;
                                        } while (field_index < num_fields);

                                        if (!foundField) {
                                                *NextFieldId = SAHPI_LAST_ENTRY;
                                        }
                                } else
                                        *NextFieldId = SAHPI_LAST_ENTRY;

                        } else {
                                rv = SA_ERR_HPI_NOT_PRESENT;
                        }

                } else {
                        rv = SA_ERR_HPI_NOT_PRESENT;
                }

        } else {
                rv = SA_ERR_HPI_NOT_PRESENT;
        }
        return rv;
}

static SaErrorT dummy_add_idr_field( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiIdrFieldT        *Field)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT               *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int                     num_areas, area_number, num_fields, i;
        SaHpiEntryIdT           lastId, entryId;

        struct dummy_inventories *s;

        if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;
        for (i = 0; ; i++) {
                if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                else {
                        s = dummy_inventory + i;
                        break;
                }
        };

        if (s->idrinfo.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        entryId = Field->AreaId;
        num_areas =  s->idrinfo.NumAreas;
        for (area_number = 0; area_number < num_areas; area_number++) {
                if (s->my_idr_area[area_number].idrareas.AreaId == entryId) break;
        };
        if (area_number >= num_areas) return SA_ERR_HPI_NOT_PRESENT;

        num_fields = s->my_idr_area[area_number].idrareas.NumFields;
        if (num_fields >= IDR_FIELDS_MAX) return(SA_ERR_HPI_OUT_OF_SPACE);
        if (num_fields == 0) lastId = 0;
        else  lastId = s->my_idr_area[area_number].idrfields[num_fields - 1].FieldId;
        lastId++;
        s->my_idr_area[area_number].idrfields[num_fields] = *Field;
        s->my_idr_area[area_number].idrfields[num_fields].FieldId = lastId;
        s->my_idr_area[area_number].idrareas.NumFields++;
        return SA_OK;
}

static SaErrorT dummy_set_idr_field( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiIdrFieldT           *Field)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT               *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int                     num_areas, area_number, num_fields, i;
        SaHpiEntryIdT           entryId, fieldId;

        struct dummy_inventories *s;

        if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;
        for (i = 0; ; i++) {
                if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                else {
                        s = dummy_inventory + i;
                        break;
                }
        };

        if (s->idrinfo.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        entryId = Field->AreaId;
        fieldId = Field->FieldId;
        num_areas =  s->idrinfo.NumAreas;
        for (area_number = 0; area_number < num_areas; area_number++) {
                if (s->my_idr_area[area_number].idrareas.AreaId == entryId) break;
        };
        if (area_number >= num_areas) return SA_ERR_HPI_NOT_PRESENT;

        num_fields = s->my_idr_area[area_number].idrareas.NumFields;
        for (i = 0; i < num_fields; i++)
                if (s->my_idr_area[area_number].idrfields[i].FieldId == fieldId)
                        break;
        if (i >= num_fields) return SA_ERR_HPI_NOT_PRESENT;
        if (s->my_idr_area[area_number].idrfields[i].ReadOnly) return SA_ERR_HPI_READ_ONLY;
        s->my_idr_area[area_number].idrfields[i] = *Field;
        return SA_OK;
}

static SaErrorT dummy_del_idr_field( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiEntryIdT            AreaId,
                SaHpiEntryIdT            FieldId)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRdrT               *rdr = oh_get_rdr_by_type(handle->rptcache, ResourceId, SAHPI_INVENTORY_RDR, IdrId);
        int                     num_areas, area_number, num_fields, i, j;

        struct dummy_inventories *s;

        if (rdr == NULL) return SA_ERR_HPI_NOT_PRESENT;
        for (i = 0; ; i++) {
                if (dummy_inventory[i].idrinfo.IdrId == 0) return SA_ERR_HPI_NOT_PRESENT;
                if (dummy_inventory[i].idrinfo.IdrId != IdrId) continue;
                else {
                        s = dummy_inventory + i;
                        break;
                }
        };

        if (s->idrinfo.ReadOnly) return SA_ERR_HPI_READ_ONLY;
        num_areas =  s->idrinfo.NumAreas;
        for (area_number = 0; area_number < num_areas; area_number++) {
                if (s->my_idr_area[area_number].idrareas.AreaId == AreaId) break;
        };
        if (area_number >= num_areas) return SA_ERR_HPI_NOT_PRESENT;

        num_fields = s->my_idr_area[area_number].idrareas.NumFields;
        for (i = 0; i < num_fields; i++)
                if (s->my_idr_area[area_number].idrfields[i].FieldId == FieldId)
                        break;
        if (i >= num_fields) return SA_ERR_HPI_NOT_PRESENT;
        if (s->my_idr_area[area_number].idrfields[i].ReadOnly) return SA_ERR_HPI_READ_ONLY;
        if (i < (num_fields - 1)) {
                for (j = i; j < num_fields - 1; j++)
                        s->my_idr_area[area_number].idrfields[j] =
                                s->my_idr_area[area_number].idrfields[j + 1];
        };
        s->my_idr_area[area_number].idrareas.NumFields--;
        return SA_OK;
}

/************************************************************************/
/* Annunciator functions                                                   */
/************************************************************************/

static int dummy_get_next_announce(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiSeverityT sever, SaHpiBoolT unackn,
        SaHpiAnnouncementT *announ)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiAnnouncementT      *tmp;
        int                     i, j;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        if (dummy_announs[i].count == 0)  return(SA_ERR_HPI_NOT_PRESENT);
        tmp = dummy_announs[i].announs;
        if (announ->EntryId == SAHPI_FIRST_ENTRY) {
                memcpy(announ, tmp, sizeof(SaHpiAnnouncementT));
                return(SA_OK);
        };
        for (j = 0; j < dummy_announs[i].count; j++)
                if (tmp[j].EntryId == announ->EntryId) break;
        j++;
        if (j >= dummy_announs[i].count) return(SA_ERR_HPI_NOT_PRESENT);
        memcpy(announ, tmp + j, sizeof(SaHpiAnnouncementT));
        return SA_OK;
}

static int dummy_get_announce(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiEntryIdT entry, SaHpiAnnouncementT *announ)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiAnnouncementT      *tmp;
        int                     i, j;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        tmp = dummy_announs[i].announs;
        for (j = 0; j < dummy_announs[i].count; j++)
                if (tmp[j].EntryId == entry) break;
        if (j >= dummy_announs[i].count) return(SA_ERR_HPI_NOT_PRESENT);
        memcpy(announ, tmp + j, sizeof(SaHpiAnnouncementT));
        return SA_OK;
}

static int dummy_ack_announce(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiEntryIdT entry, SaHpiSeverityT sever)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiAnnouncementT      *tmp;
        int                     i, j;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        tmp = dummy_announs[i].announs;
        for (j = 0; j < dummy_announs[i].count; j++) {
                if (entry != SAHPI_ENTRY_UNSPECIFIED) {
                        if (tmp[j].EntryId == entry) {
                                tmp[j].Acknowledged = 1;
                                return(SA_OK);
                        };
                        continue;
                };
                if (tmp[j].Severity == sever) tmp[j].Acknowledged = 1;
        };
        return SA_OK;
}

static int dummy_get_annunc_mode(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiAnnunciatorModeT *mode)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        *mode = dummy_announs[i].mode;
        return SA_OK;
}

static int dummy_set_annunc_mode(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiAnnunciatorModeT mode)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        dummy_announs[i].mode = mode;
        return SA_OK;
}

static void annun_del_announ(int annun_num, int announ_num)
{
        SaHpiAnnouncementT      *tmp;
        int                     n, i, j;

        n = dummy_announs[annun_num].count - 1;
        if (n <= 0) {
                if (n == 0) free(dummy_announs[annun_num].announs);
                dummy_announs[annun_num].announs = 0;
                dummy_announs[annun_num].count = 0;
                return;
        };
        tmp = (SaHpiAnnouncementT *)malloc(n * sizeof(SaHpiAnnouncementT));
        for (i = 0, j = 0; i <= n; i++) {
                if (i == announ_num) continue;
                tmp[j++] = dummy_announs[annun_num].announs[i];
        };
        free(dummy_announs[annun_num].announs);
        dummy_announs[annun_num].announs = tmp;
        dummy_announs[annun_num].count--;
}

static int annun_add_announ(int annun_num)
{
        SaHpiAnnouncementT      *tmp;
        SaHpiEntryIdT           id = 1;
        int                     n, i;

        n = dummy_announs[annun_num].count;
        for (i = 0; i < n; i++) {
                if (dummy_announs[annun_num].announs[i].EntryId > id)
                        id = dummy_announs[annun_num].announs[i].EntryId;
        };
        id++;
        n++;
        tmp = (SaHpiAnnouncementT *)malloc(n * sizeof(SaHpiAnnouncementT));
        memset(tmp, 0, n * sizeof(SaHpiAnnouncementT));
        for (i = 0; i < n - 1; i++) {
                tmp[i] = dummy_announs[annun_num].announs[i];
        };
        tmp[i].EntryId = id;
        if (n > 1) free(dummy_announs[annun_num].announs);
        dummy_announs[annun_num].announs = tmp;
        dummy_announs[annun_num].count++;
        return(i);
}

static int dummy_del_announce(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiEntryIdT entry, SaHpiSeverityT sever)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        SaHpiAnnouncementT      *tmp;
        int                     i, j, done = 0;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        while (done == 0) {
                tmp = dummy_announs[i].announs;
                for (j = 0; j < dummy_announs[i].count; j++) {
                        if (entry != SAHPI_ENTRY_UNSPECIFIED) {
                                if (tmp[j].EntryId == entry) {
                                        annun_del_announ(i, j);
                                        done = 1;
                                        break;
                                };
                                continue;
                        };
                        if ((sever == SAHPI_ALL_SEVERITIES) ||
                                (tmp[j].Severity == sever)) {
                                annun_del_announ(i, j);
                                break;
                        }
                };
                if (j >= dummy_announs[i].count) break;
        };
        return SA_OK;
}

static int dummy_add_announce(void *hnd, SaHpiResourceIdT id,
        SaHpiAnnunciatorNumT num, SaHpiAnnouncementT *announ)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i, j;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_ANNUNCIATOR_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < ANNUN_MAX; i++) {
                if (dummy_announs[i].Num == num) break;
        };
        if (i >= ANNUN_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        j = annun_add_announ(i);
        announ->EntryId = dummy_announs[i].announs[j].EntryId;
        announ->AddedByUser = 1;
        dummy_announs[i].announs[j] = *announ;
        return SA_OK;
}

/************************************************************************/
/* Watchdog functions                                                   */
/************************************************************************/

static int dummy_get_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_WATCHDOG_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < WTD_MAX; i++) {
                if (dummy_wtd_timers[i].Num == num) break;
        };
        if (i >= WTD_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        memcpy(wdt, &(dummy_wtd_timers[i].wtd), sizeof(SaHpiWatchdogT));
        return SA_OK;
}

static int dummy_set_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_WATCHDOG_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < WTD_MAX; i++) {
                if (dummy_wtd_timers[i].Num == num) break;
        };
        if (i >= WTD_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        memcpy(&(dummy_wtd_timers[i].wtd), wdt, sizeof(SaHpiWatchdogT));
        return SA_OK;
}

static int dummy_reset_watchdog(void *hnd, SaHpiResourceIdT id,
                                SaHpiWatchdogNumT num)
{
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT               *rdr;
        int                     i;

        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_WATCHDOG_RDR, num);
        if (rdr == (SaHpiRdrT *)NULL) return(SA_ERR_HPI_NOT_PRESENT);

        for (i = 0; i < WTD_MAX; i++) {
                if (dummy_wtd_timers[i].Num == num) break;
        };
        if (i >= WTD_MAX) return(SA_ERR_HPI_NOT_PRESENT);
        memcpy(&(dummy_wtd_timers[i].wtd), &(dummy_wtd_timers[i].reset), sizeof(SaHpiWatchdogT));
        return SA_OK;
}

/************************************************************************/
/* Hotswap functions                                                    */
/************************************************************************/
static int dummy_get_hotswap_state(void *hnd, SaHpiResourceIdT id,
                                   SaHpiHsStateT *state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;

        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                *state = dummy_resource_status[i].hotswap;
                rval = 0;
        }

        return(rval);
}

static int dummy_set_hotswap_state(void *hnd, SaHpiResourceIdT id,
                                   SaHpiHsStateT state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;


        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                dummy_resource_status[i].hotswap = state;
                rval = 0;
        }


        return(rval);

}

static int dummy_request_hotswap_action(void *hnd, SaHpiResourceIdT id,
                                        SaHpiHsActionT act)
{
        struct oh_handler_state *inst = hnd;
        /*SaHpiRptEntryT *e = NULL;*/

        int rval = -1;
        int i = 0;
        int done = FALSE;

        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                rval = 0;
        }
        else {
                dbg("The resource does not hotswapable!");
                rval = -1;
        }

        if (!rval && act == SAHPI_HS_ACTION_INSERTION) {
                if (dummy_resource_status[1].hotswap == SAHPI_HS_STATE_INACTIVE) {
                        dummy_resource_status[1].hotswap = SAHPI_HS_STATE_INSERTION_PENDING;
                        hotswap_event[0].u.hpi_event.res.ResourceId = id;
                        g_async_queue_push(inst->eventq_async, __eventdup(&hotswap_event[0]));


                } else {
                        dbg("Fail insertion, HotSwap");
                        rval = -1;
                }

        } else if (!rval && act == SAHPI_HS_ACTION_EXTRACTION) {
                if (dummy_resource_status[1].hotswap == SAHPI_HS_STATE_ACTIVE) {
                        dummy_resource_status[1].hotswap = SAHPI_HS_STATE_EXTRACTION_PENDING;
                        hotswap_event[1].u.hpi_event.res.ResourceId = id;
                        g_async_queue_push(inst->eventq_async, __eventdup(&hotswap_event[1]));


                } else {
                        dbg("Cannot extraction");
                        rval = -1;
                }
        } else
                rval = -1;

        return(rval);

}

static int dummy_get_power_state(void *hnd, SaHpiResourceIdT id, SaHpiPowerStateT *state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;

        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                *state = dummy_resource_status[i].power;
                rval = 0;
        }

        return(rval);
}

static int dummy_set_power_state(void *hnd, SaHpiResourceIdT id,
                                 SaHpiPowerStateT state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;


        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                dummy_resource_status[i].power = state;
                rval = 0;
        }


        return(rval);
}

static int dummy_get_indicator_state(void *hnd, SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT *state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;

        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                *state = dummy_resource_status[i].indicator;
                rval = 0;
        }

        return(rval);

}

static int dummy_set_indicator_state(void *hnd, SaHpiResourceIdT id,
                                     SaHpiHsIndicatorStateT state)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;


        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                dummy_resource_status[i].indicator = state;
                rval = 0;
        }


        return(rval);
}

static int dummy_control_parm(void *hnd, SaHpiResourceIdT id,
                              SaHpiParmActionT act)
{
        struct oh_handler_state *handle = (struct oh_handler_state *)hnd;
        SaHpiRptEntryT          *rpt;
        SaHpiRdrT               *rdr;
        int                     i;
        SaHpiInstrumentIdT      num;

        rpt = oh_get_resource_by_id(handle->rptcache, id);
        if (rpt == (SaHpiRptEntryT *)NULL) return SA_ERR_HPI_NOT_PRESENT;
        if ((rpt->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION) == 0)
                return SA_ERR_HPI_CAPABILITY;
        rdr = oh_get_rdr_next(handle->rptcache, id, SAHPI_FIRST_ENTRY);
        while (rdr) {
                for (i = 0; i < NUM_RDRS; i++) {
                        if (Save_rdr[i].res_Id != id) continue;
                        if (Save_rdr[i].rdr_type != rdr->RdrType) continue;
                        switch (rdr->RdrType) {
                                case SAHPI_SENSOR_RDR:
                                        num = rdr->RdrTypeUnion.SensorRec.Num;
                                        break;
                                case SAHPI_CTRL_RDR:
                                        num = rdr->RdrTypeUnion.CtrlRec.Num;
                                        break;
                                case SAHPI_INVENTORY_RDR:
                                        num = rdr->RdrTypeUnion.InventoryRec.IdrId;
                                        break;
                                case SAHPI_WATCHDOG_RDR:
                                        num = rdr->RdrTypeUnion.WatchdogRec.WatchdogNum;
                                        break;
                                default:
                                        continue;
                        };
                        if (Save_rdr[i].num != num) continue;
                        switch (act) {
                                case SAHPI_DEFAULT_PARM:
                                        memcpy(rdr, &(Save_rdr[i].rdr_default), sizeof(SaHpiRdrT));
                                        break;
                                case SAHPI_SAVE_PARM:
                                        memcpy(&(Save_rdr[i].rdr_save), rdr, sizeof(SaHpiRdrT));
                                        Save_rdr[i].is_save = 1;
                                        break;
                                case SAHPI_RESTORE_PARM:
                                        if (Save_rdr[i].is_save == 0) continue;
                                        memcpy(rdr, &(Save_rdr[i].rdr_save), sizeof(SaHpiRdrT));
                                        break;
                                default: continue;
                        };
                        break;
                };
                rdr = oh_get_rdr_next(handle->rptcache, id, rdr->RecordId);
        }

        return SA_OK;
}

static int dummy_get_reset_state(void *hnd, SaHpiResourceIdT id,
                                 SaHpiResetActionT *act)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;

        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                *act = dummy_resource_status[i].reset;
                rval = 0;
        }

        return(rval);
}

static int dummy_set_reset_state(void *hnd, SaHpiResourceIdT id,
                                 SaHpiResetActionT act)
{
        int rval = -1;
        int i = 0;
        int done = FALSE;


        done = FALSE;
        i = 0;
        do {
                if(dummy_resource_status[i].ResourceId == id) {
                        done = TRUE;
                }
                else  {
                        i++;
                }

        } while ( (i < NUM_RESOURCES) && !done  );

        if ( i < NUM_RESOURCES ) {
                rval = 0;
                switch (act) {
                case SAHPI_COLD_RESET:
                case SAHPI_WARM_RESET:
                        dummy_resource_status[i].reset = SAHPI_RESET_DEASSERT;
                        break;
                case SAHPI_RESET_ASSERT:
                        dummy_resource_status[i].reset = SAHPI_RESET_ASSERT;
                        break;
                                case SAHPI_RESET_DEASSERT:
                        dummy_resource_status[i].reset = SAHPI_RESET_DEASSERT;
                        break;
                default:
                        rval = -1;
                }
        }

        return(rval);
}

void * oh_open (GHashTable *) __attribute__ ((weak, alias("dummy_open")));

void * oh_close (void *) __attribute__ ((weak, alias("dummy_close")));

void * oh_get_event (void *, struct oh_event *)
                __attribute__ ((weak, alias("dummy_get_event")));

void * oh_discover_resources (void *)
                __attribute__ ((weak, alias("dummy_discover_resources")));

void * oh_discover_domain_resource (void *, SaHpiDomainIdT)
                __attribute__ ((weak, alias("dummy_discover_domain_resources")));

void * oh_set_resource_tag (void *, SaHpiResourceIdT, SaHpiTextBufferT *)
                __attribute__ ((weak, alias("dummy_set_resource_tag")));

void * oh_set_resource_severity (void *, SaHpiResourceIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("dummy_set_resource_severity")));

void * oh_get_el_info (void *, SaHpiResourceIdT, SaHpiEventLogInfoT *)
                __attribute__ ((weak, alias("dummy_get_sel_info")));

void * oh_set_el_time (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("dummy_set_sel_time")));

void * oh_add_el_entry (void *, SaHpiResourceIdT, const SaHpiEventT *)
                __attribute__ ((weak, alias("dummy_add_sel_entry")));

void * oh_get_el_entry (void *, SaHpiResourceIdT, SaHpiEventLogEntryIdT,
                       SaHpiEventLogEntryIdT *, SaHpiEventLogEntryIdT *,
                       SaHpiEventLogEntryT *, SaHpiRdrT *, SaHpiRptEntryT  *)
                __attribute__ ((weak, alias("dummy_get_sel_entry")));

void * oh_clear_el (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("dummy_clear_el")));

void * oh_reset_el_overflow (void *, SaHpiResourceIdT)
                __attribute__ ((weak, alias("dummy_reset_el_overflow")));

void * oh_get_sensor_reading (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiSensorReadingT *,
                             SaHpiEventStateT    *)
                __attribute__ ((weak, alias("dummy_get_sensor_reading")));

void * oh_get_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("dummy_get_sensor_thresholds")));

void * oh_set_sensor_thresholds (void *, SaHpiResourceIdT,
                                 SaHpiSensorNumT,
                                 const SaHpiSensorThresholdsT *)
                __attribute__ ((weak, alias("dummy_set_sensor_thresholds")));

void * oh_get_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT *)
                __attribute__ ((weak, alias("dummy_get_sensor_enable")));

void * oh_set_sensor_enable (void *, SaHpiResourceIdT,
                             SaHpiSensorNumT,
                             SaHpiBoolT)
                __attribute__ ((weak, alias("dummy_set_sensor_enable")));

void * oh_get_sensor_event_enables (void *, SaHpiResourceIdT,
                                    SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("dummy_get_sensor_event_enabled")));

void * oh_set_sensor_event_enables (void *, SaHpiResourceIdT id, SaHpiSensorNumT,
                                    SaHpiBoolT *)
                __attribute__ ((weak, alias("dummy_set_sensor_event_enabled")));

void * oh_get_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiEventStateT *, SaHpiEventStateT *)
                __attribute__ ((weak, alias("dummy_get_sensor_event_masks")));

void * oh_set_sensor_event_masks (void *, SaHpiResourceIdT, SaHpiSensorNumT,
                                  SaHpiSensorEventMaskActionT,
                                  SaHpiEventStateT,
                                  SaHpiEventStateT)
                __attribute__ ((weak, alias("dummy_set_sensor_event_masks")));

void * oh_get_control_state (void *, SaHpiResourceIdT, SaHpiCtrlNumT,
                             SaHpiCtrlModeT *, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("dummy_get_control_state")));

void * oh_set_control_state (void *, SaHpiResourceIdT,SaHpiCtrlNumT,
                             SaHpiCtrlModeT, SaHpiCtrlStateT *)
                __attribute__ ((weak, alias("dummy_set_control_state")));

void * oh_get_idr_info (void *hnd, SaHpiResourceIdT, SaHpiIdrIdT,SaHpiIdrInfoT)
                __attribute__ ((weak, alias("dummy_get_idr_info")));

void * oh_get_idr_area_header (void *, SaHpiResourceIdT, SaHpiIdrIdT,
                                SaHpiIdrAreaTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                                SaHpiIdrAreaHeaderT)
                __attribute__ ((weak, alias("dummy_get_idr_area_header")));

void * oh_add_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrAreaTypeT,
                        SaHpiEntryIdT)
                __attribute__ ((weak, alias("dummy_add_idr_area")));

void * oh_del_idr_area (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT)
                __attribute__ ((weak, alias("dummy_del_idr_area")));

void * oh_get_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldTypeT, SaHpiEntryIdT, SaHpiEntryIdT,
                         SaHpiIdrFieldT)
                __attribute__ ((weak, alias("dummy_get_idr_field")));

void * oh_add_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
                __attribute__ ((weak, alias("dummy_add_idr_field")));

void * oh_set_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiIdrFieldT)
                __attribute__ ((weak, alias("dummy_set_idr_field")));

void * oh_del_idr_field (void *, SaHpiResourceIdT, SaHpiIdrIdT, SaHpiEntryIdT,
                         SaHpiEntryIdT)
                __attribute__ ((weak, alias("dummy_del_idr_field")));

void * oh_get_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("dummy_get_watchdog_info")));

void * oh_set_watchdog_info (void *, SaHpiResourceIdT, SaHpiWatchdogNumT,
                             SaHpiWatchdogT *)
                __attribute__ ((weak, alias("dummy_set_watchdog_info")));

void * oh_get_next_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                             SaHpiSeverityT, SaHpiBoolT, SaHpiAnnouncementT)
                __attribute__ ((weak, alias("dummy_get_next_announce")));

void * oh_get_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("dummy_get_announce")));

void * oh_ack_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiEntryIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("dummy_ack_announce")));


void * oh_add_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                        SaHpiAnnouncementT *)
                __attribute__ ((weak, alias("dummy_add_announce")));

void * oh_del_announce (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                       SaHpiEntryIdT, SaHpiSeverityT)
                __attribute__ ((weak, alias("dummy_del_announce")));

void * oh_get_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT *)
                __attribute__ ((weak, alias("dummy_get_annunc_mode")));

void * oh_set_annunc_mode (void *, SaHpiResourceIdT, SaHpiAnnunciatorNumT,
                           SaHpiAnnunciatorModeT)
                __attribute__ ((weak, alias("dummy_set_annunc_mode")));

void * oh_reset_watchdog (void *, SaHpiResourceIdT, SaHpiWatchdogNumT)
                __attribute__ ((weak, alias("dummy_reset_watchdog")));

void * oh_get_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT *)
                __attribute__ ((weak, alias("dummy_get_hotswap_state")));

void * oh_set_hotswap_state (void *, SaHpiResourceIdT, SaHpiHsStateT)
                __attribute__ ((weak, alias("dummy_set_hotswap_state")));

void * oh_request_hotswap_action (void *, SaHpiResourceIdT, SaHpiHsActionT)
                __attribute__ ((weak, alias("dummy_request_hotswap_action")));

void * oh_get_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT *)
                __attribute__ ((weak, alias("dummy_get_power_state")));

void * oh_set_power_state (void *, SaHpiResourceIdT, SaHpiPowerStateT)
                __attribute__ ((weak, alias("dummy_set_power_state")));

void * oh_get_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT *)
                __attribute__ ((weak, alias("dummy_get_indicator_state")));

void * oh_set_indicator_state (void *, SaHpiResourceIdT,
                               SaHpiHsIndicatorStateT)
                __attribute__ ((weak, alias("dummy_set_indicator_state")));

void * oh_control_parm (void *, SaHpiResourceIdT, SaHpiParmActionT)
                __attribute__ ((weak, alias("dummy_control_parm")));

void * oh_get_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT *)
                __attribute__ ((weak, alias("dummy_get_reset_state")));

void * oh_set_reset_state (void *, SaHpiResourceIdT, SaHpiResetActionT)
                __attribute__ ((weak, alias("dummy_set_reset_state")));

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
#ifdef DUMMY_THREADED
gpointer event_thread(gpointer data)
{
        struct timespec req, rem;
        memset(&req, 0, sizeof(req));
        req.tv_nsec = THREAD_SLEEP_TIME_NS;
        req.tv_sec =  THREAD_SLEEP_TIME_SECS;

        int rtval = 999;


        struct oh_handler_state *inst = (struct oh_handler_state *)data;

        struct oh_event *e;

        sleep(1);

        for (;;) {
                /*
                printf("address of data [%u]\n", (int)data);
                printf("address of inst [%u]\n", (int)inst);
                printf("address of mutex [%u]\n", (int)inst->handler_lock);
                printf("loop count [%d]\n", c);
                printf("event burp!\n");
                */

                /* allocate memory for event */
                e = g_malloc0(sizeof(*e));
                if (!e) {
                        dbg("Out of memory!");
                        return NULL;
                }

                /* build event */
                static unsigned int toggle = 0;
                struct oh_event *event;
                /* once initial reporting of events toggle between      */
                /* removing and adding resource, removes resource 3     */
                /* since it has no rdr's to add back later              */
                event = g_malloc0(sizeof(*event));
                if (!event) {
                        printf("event: memory alloc failure\n");
                        return 0;
                }


//                while (!g_static_rec_mutex_trylock (inst->handler_lock))
//                        printf("mutex is going to block [%d]\n", (int)inst->handler_lock);

                //g_static_rec_mutex_lock (inst->handler_lock);

                if ( (toggle%2) == 0 ) {
                        toggle++;
                        dbg("**** EVEN ****, remove the resource");
                        if ( (e = remove_resource(inst)) ) {
                                memcpy(event, e, sizeof(*event));
                                g_async_queue_push(inst->eventq_async, event);
                        }
                } else {
                        toggle++;
                        dbg("**** ODD ****, add the resource");
                        if ( (e = add_resource(inst)) ) {
                                memcpy(event, e, sizeof(*event));
                                g_async_queue_push(inst->eventq_async, event);
                        }
                }

//                g_static_rec_mutex_unlock (inst->handler_lock);

                /* signal threaded infrastructure */
                oh_wake_event_thread(SAHPI_FALSE);
                dbg("dummy thread, signaled");

                nanosleep(&req, &rem);

        }


        printf("THREAD END\n");

        g_thread_exit(&rtval);

        return 0 ;
}
#endif


