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
 *      Others (please add yourself here)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#include <unistd.h>

#include <SaHpi.h>
#include <openhpi.h>
#include <epath_utils.h>
#include <uid_utils.h>
#include <oh_error.h>
#include <oh_domain.h>

#define DUMMY_THREADED
#undef  DUMMY_THREADED
#define THREAD_SLEEP_TIME_NS 	500000000
#define THREAD_SLEEP_TIME_SECS 	1


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
                        .DataLength = strlen(string),           \
                        .Data = string                          \
        }


#define NUM_RESOURCES   3
#define NUM_RDRS        8

#define VIRTUAL_NODES 2

#define NO_ID 0  /* Arbitrarily define IdrId == 0 as end-of-inventory */
                 /* instead of using a counter of some kind           */

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
                                .Type = SAHPI_CTRL_TYPE_DIGITAL,
                                .TypeUnion = {
                                        .Digital = {
                                                .Default = SAHPI_CTRL_STATE_OFF,
                                        },
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
                                         .WriteThold = SAHPI_STM_LOW_CRIT,
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
};


/* SaHpiCtrlStateT */
static SaHpiCtrlStateT dummy_controls = {
        .Type = SAHPI_CTRL_TYPE_DIGITAL,
        .StateUnion = {
                .Digital = SAHPI_CTRL_STATE_OFF,
        },
};
/************************************************************************/
/* sensor data                                                          */
/************************************************************************/
static struct dummy_sensor {
        SaHpiSensorReadingT reading;
        SaHpiSensorThresholdsT thresholds;
        SaHpiBoolT enabled;
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
                .assert = SAHPI_ES_UPPER_CRIT,
                .deassert = SAHPI_ES_LOWER_CRIT,
        },
};

/************************************************************************/
/* Resource one inventory data                                          */
/************************************************************************/
struct  dummy_idr_area {
        SaHpiIdrAreaHeaderT  idrareas;
        SaHpiIdrFieldT  idrfields[20];
};

static struct dummy_inventories {
        SaHpiIdrInfoT   idrinfo;
        struct dummy_idr_area my_idr_area[5];
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
        },
        {} /* Terminate Array */
           /* Arbitrarily define IdrId == 0 as end-of-inventory */
           /* instead of using a counter of some kind           */
},
{}
};

/************************************************************************/
/* System Event Log data                                                                */
/************************************************************************/
static struct dummy_rel {
        SaHpiEventLogEntryT entry;
} rel_entries = {
        .entry = {
                .EntryId = 0,
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
        SaHpiRdrT res_rdr;

        SaHpiEntityPathT root_ep;
        char *entity_root;

        entity_root = (char *)g_hash_table_lookup(oh_hnd->config,"entity_root");

        string2entitypath(entity_root, &root_ep);

        /* append entity root to resource entity paths */
        for (i=0; i < NUM_RESOURCES; i++) {
                memcpy(&res, &dummy_resources[i], sizeof(SaHpiRptEntryT));

                ep_concat( &res.ResourceEntity, &root_ep);

                res.ResourceId = oh_uid_from_entity_path(&res.ResourceEntity);

                /* add the resource */
                if (oh_add_resource(oh_hnd->rptcache, &res, NULL, 0)) {
                        dbg("oh_add_resource failed for RESOURCE %d", i);
			return -1;
		}

                /* save the resource id for tracking resource status */
                dummy_resource_status[i].ResourceId = res.ResourceId;

		printf("oh_add_resource succeeded for RESOURCE %d\n", i);

        }
        /* append entity root to rdrs entity paths */
        for (i=0; i < NUM_RDRS; i++) {
                memcpy(&res_rdr, &dummy_rdrs[i], sizeof(SaHpiRdrT));

                ep_concat( &res_rdr.Entity, &root_ep);

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

		printf("oh_add_resource succeeded for RDR %d\n", i);
        }

        return(0);
}

static void *dummy_open(GHashTable *handler_config)
{
        struct oh_handler_state *i;
        char *tok = NULL;
        tok = g_hash_table_lookup(handler_config, "entity_root");
        
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
        oh_init_rpt(i->rptcache, NULL);

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
        i->handler_lock = g_malloc0(sizeof(GStaticRecMutex));
        if (!i->handler_lock) {
                dbg("GStaticRecMutex: out of memory");
                return NULL;
        }
        g_static_rec_mutex_init (i->handler_lock);

        /* create event queue for async events*/
        if ( !(i->eventq_async = g_async_queue_new()) ) {
                printf("g_async_queue_new failed\n");
                g_free(i);
                return NULL;
        }

#ifdef DUMMY_THREADED
        /* add to oh_handler_state */
        GThread *thread_handle;
        GError **e = NULL;
        
        /* spawn a thread */
        if ( !(thread_handle = g_thread_create (event_thread,
                                                i,      /* oh_handler_state */
                                                FALSE,
                                                e)) ) {
             printf("g_thread_create failed\n");
             g_free(i);
             return NULL;
        }
#endif
        
        return( (void *)i );
}

static void dummy_close(void *hnd)
{
        struct oh_handler_state *inst = hnd;
        /* TODO: free the rptcache RPTable *rptcache */

        /* free eventq entries GSList *eventq */
        while ( g_slist_length( inst->eventq ) > 0 ) {
                free(inst->eventq->data);
                inst->eventq = g_slist_remove_link(inst->eventq, inst->eventq);
        }

	/* destroy mutex */
        g_static_rec_mutex_free(inst->handler_lock);
	
	/* destroy async queue */


        /* TODO: free the GHashTabel GHashTable *config */

        /* TODO: free *data void *data */

        free(hnd);

        return;
}

#if 1
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

#endif
static int dummy_get_event(void *hnd, struct oh_event *event, struct timeval *timeout)
{
        struct oh_handler_state *inst = hnd;

        struct oh_event *e = NULL;
        struct oh_event *qse = NULL;
	qse = NULL;

        SaHpiRptEntryT *rpt_entry = NULL;

        static unsigned int toggle = 0;
        static unsigned int count = 0;


#if 0
        if (g_slist_length(inst->eventq)>0) {

                trace("List has an event, send it up");

                memcpy(event, inst->eventq->data, sizeof(*event));

                event->did = oh_get_default_domain_id(); 

                free(inst->eventq->data);

                inst->eventq = g_slist_remove_link(inst->eventq, inst->eventq);

		printf("*************** dummy_get_event, g_slist_length\n");

                return(1);

#else
	if ( (qse = g_async_queue_try_pop(inst->eventq_async)) ) {

                trace("List has an event, send it up");

		memcpy(event, qse, sizeof(*event));

                event->did = oh_get_default_domain_id(); 

                g_free(qse);

		printf("*************** dummy_get_event, g_async_queue_try_pop\n");

                return(1);
#endif

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
                return(-1);
        }

        g_static_rec_mutex_lock (inst->handler_lock);
        
        toggle++;

#ifndef DUMMY_THREADED

        if( (toggle%3) == 0 ) {
                /* once initial reporting of events toggle between      */
                /* removing and adding resource, removes resource 3     */
                /* since it has no rdr's to add back later              */
                if ( (count%2) == 0 ) {
                        count++;
                        dbg("\n**** EVEN ****, remove the resource\n");
                        if ( (e = remove_resource(inst)) ) {
                                *event = *e;
                                g_static_rec_mutex_unlock (inst->handler_lock);
                                return(1);
                        }
                } else {
                        count++;
                        dbg("\n**** ODD ****, add the resource\n");
                        if ( (e = add_resource(inst)) ) {
                                *event = *e;
                                g_static_rec_mutex_unlock (inst->handler_lock);
                                return(1);
                        }
                }
        }
#endif
        g_static_rec_mutex_unlock (inst->handler_lock);

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

                        /* old */
			inst->eventq = g_slist_append(inst->eventq, __eventdup(&event) );

			/* new */
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

				/* old */
                                inst->eventq = g_slist_append(inst->eventq, __eventdup(&event));

				/* new */
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

static int sel_enabled;

static int dummy_get_sel_info(void *hnd, SaHpiResourceIdT id, SaHpiEventLogInfoT *info)
{
        info->Entries                   = 0;
        info->Size                      = 0xFFFFFFFF;
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
        //inst->eventq = g_slist_append(inst->eventq, e);

#endif
        return 0;
}

static int dummy_get_sel_entry(void *hnd,
                               SaHpiResourceIdT id,
                               SaHpiEventLogEntryIdT current,
                               SaHpiEventLogEntryIdT *prev,
                               SaHpiEventLogEntryIdT *next,
                               SaHpiEventLogEntryT *entry)
{
        dbg("dummy_get_sel_entry(): This is a very bad implementation");
        *prev = SAHPI_NO_MORE_ENTRIES;
        *next = SAHPI_NO_MORE_ENTRIES;
        memcpy(entry, &rel_entries.entry, sizeof(SaHpiEventLogEntryT));

        return 0;
}

/************************************************************************/
/* Sensor functions                                                     */
/************************************************************************/
static int dummy_get_sensor_data(void *hnd, SaHpiResourceIdT id,
                           SaHpiSensorNumT num,
                           SaHpiSensorReadingT *data)
{
        int rval = -1;
        struct oh_handler_state *inst = hnd;
        SaHpiRdrT *rdr;

        /* get any resource rdr's */
        rdr = oh_get_rdr_next(inst->rptcache, id, SAHPI_FIRST_ENTRY);

        while ( (rdr->RdrTypeUnion.CtrlRec.Num != num) && rdr) {
                rdr = oh_get_rdr_next(inst->rptcache, id, rdr->RecordId);
        }

        if (rdr) {
                memcpy(data,
                       &dummy_sensors[rdr->RdrTypeUnion.SensorRec.Num - 1].reading,
                       sizeof(SaHpiSensorReadingT));
                rval = 0;
        }

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

        dbg(" ********* dummy_get_sensor_event_enables *******");
        memcpy(enabled, &dummy_sensors[num - 1].enabled, sizeof(*enabled));

        return 0;
}

static int dummy_set_sensor_event_enabled(void *hnd, SaHpiResourceIdT id,
                                          SaHpiSensorNumT num,
                                          const SaHpiBoolT enabled)
{

        dbg(" ********* dummy_set_sensor_event_enables *******");

        memcpy(&dummy_sensors[num - 1].enabled, &enabled, sizeof(enabled));

        return 0;
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
        SaHpiRdrT *rdr;

        /* yes, nothing is done with rdr, but           */
        /* in the real world you would need to correlate        */
        /* this rdr with the appropriate data                   */
        rdr = oh_get_rdr_by_type(inst->rptcache, id, SAHPI_CTRL_RDR, num);

        state->StateUnion = dummy_controls.StateUnion;
        state->Type = dummy_controls.Type;

        return 0;
}

static int dummy_set_control_state(void *hnd, SaHpiResourceIdT id,
                                   SaHpiCtrlNumT num,
                                   SaHpiCtrlModeT mode,
                                   SaHpiCtrlStateT *state)
{
        dbg("TODO: dummy_set_control_state(), need to set control based on id");
        /*struct dummy_control *control = id.ptr;*/
        /*memcpy(&control->state,state,sizeof(SaHpiCtrlStateT));*/

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
        SaHpiEntryIdT            thisAreaId;
        SaHpiBoolT foundArea = SAHPI_FALSE;
        int num_areas, i;

        if (rdr != NULL) {
                struct dummy_inventories *s = dummy_inventory;
                
                num_areas =  s->idrinfo.NumAreas;
                i = 0;
                do { 
                
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
                        i++;
                } while (i < num_areas);
                
                i++;
                if (foundArea) {
                        foundArea = SAHPI_FALSE;
                        if (i < num_areas) {
                                do { 
                                        thisAreaType = s->my_idr_area[i].idrareas.Type;
                                        if ((thisAreaId == AreaId) && (thisAreaType == AreaType)) {      
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
        return SA_ERR_HPI_READ_ONLY;
}

static SaErrorT dummy_del_idr_area( void *hnd,
                SaHpiResourceIdT       ResourceId,
                SaHpiIdrIdT            IdrId,
                SaHpiEntryIdT          AreaId)
{
        return SA_ERR_HPI_READ_ONLY;
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
                struct dummy_inventories *s = dummy_inventory;
                
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
        return SA_ERR_HPI_READ_ONLY;
}

static SaErrorT dummy_set_idr_field( void *hnd,
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiIdrFieldT           *Field)
{
        return SA_ERR_HPI_READ_ONLY;
}

static SaErrorT dummy_del_idr_field( void *hnd, 
                SaHpiResourceIdT         ResourceId,
                SaHpiIdrIdT              IdrId,
                SaHpiEntryIdT            AreaId,
                SaHpiEntryIdT            FieldId)
{
        return SA_ERR_HPI_READ_ONLY;
}

/************************************************************************/
/* Watchdog functions                                                   */
/************************************************************************/


static int dummy_get_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        return -1;
}

static int dummy_set_watchdog_info(void *hnd, SaHpiResourceIdT id,
                                   SaHpiWatchdogNumT num,
                                   SaHpiWatchdogT *wdt)
{
        return -1;
}

static int dummy_reset_watchdog(void *hnd, SaHpiResourceIdT id,
                                SaHpiWatchdogNumT num)
{
        return -1;
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
			/* old */
                        inst->eventq = g_slist_append(inst->eventq, __eventdup(&hotswap_event[0]));

			/* new */
			g_async_queue_push(inst->eventq_async, __eventdup(&hotswap_event[0]));


                } else {
                        dbg("Fail insertion, HotSwap");
                        rval = -1;
                }

        } else if (!rval && act == SAHPI_HS_ACTION_EXTRACTION) {
                if (dummy_resource_status[1].hotswap == SAHPI_HS_STATE_ACTIVE) {
                        dummy_resource_status[1].hotswap = SAHPI_HS_STATE_EXTRACTION_PENDING;
                        hotswap_event[1].u.hpi_event.res.ResourceId = id;

			/* old */
                        inst->eventq = g_slist_append(inst->eventq, __eventdup(&hotswap_event[1]));

			/* new */
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
        return 0;
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
                default:
                        rval = -1;
                }
        }

        return(rval);
}

static struct oh_abi_v2 oh_dummy_plugin = {
        .open                   = dummy_open,
        .close                  = dummy_close,
        .get_event              = dummy_get_event,
        .discover_resources     = dummy_discover_resources,
        .get_el_info            = dummy_get_sel_info,
        .set_el_time            = dummy_set_sel_time,
        .add_el_entry           = dummy_add_sel_entry,  
        .get_el_entry           = dummy_get_sel_entry,
        .get_sensor_data        = dummy_get_sensor_data,
        .get_sensor_thresholds  = dummy_get_sensor_thresholds,
        .set_sensor_thresholds  = dummy_set_sensor_thresholds,
        .get_sensor_event_enables = dummy_get_sensor_event_enabled,
        .set_sensor_event_enables = dummy_set_sensor_event_enabled,
        .get_control_state      = dummy_get_control_state,
        .set_control_state      = dummy_set_control_state,
        .get_idr_info           = dummy_get_idr_info,
        .get_idr_area_header    = dummy_get_idr_area_header,
        .add_idr_area           = dummy_add_idr_area,
        .del_idr_area           = dummy_del_idr_area,
        .get_idr_field          = dummy_get_idr_field,
        .add_idr_field          = dummy_add_idr_field,
        .set_idr_field          = dummy_set_idr_field,
        .del_idr_field          = dummy_del_idr_field,
        .get_watchdog_info      = dummy_get_watchdog_info,
        .set_watchdog_info      = dummy_set_watchdog_info,
        .reset_watchdog         = dummy_reset_watchdog,
        .get_hotswap_state      = dummy_get_hotswap_state,
        .set_hotswap_state      = dummy_set_hotswap_state,
        .request_hotswap_action = dummy_request_hotswap_action,
        .get_power_state        = dummy_get_power_state,
        .set_power_state        = dummy_set_power_state,
        .get_indicator_state    = dummy_get_indicator_state,
        .set_indicator_state    = dummy_set_indicator_state,
        .control_parm           = dummy_control_parm,
        .get_reset_state        = dummy_get_reset_state,
        .set_reset_state        = dummy_set_reset_state,
};

int dummy_get_interface(void **pp, const uuid_t uuid);

int dummy_get_interface(void **pp, const uuid_t uuid)
{
        if (uuid_compare(uuid, UUID_OH_ABI_V2)==0) {
                *(struct oh_abi_v2 **)pp = &oh_dummy_plugin;
                return 0;
        }

        *pp = NULL;
        return -1;
}

int get_interface(void **pp, const uuid_t uuid) __attribute__ ((weak, alias("dummy_get_interface")));


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


        struct oh_handler_state *inst = (struct oh_handler_state *)data;

        struct oh_event *e;

        sleep(1);

        int c=0;
        for (;;) {
                c++;
printf("address of data [%u]\n", (int)data);
printf("address of inst [%u]\n", (int)inst);
printf("address of mutex [%u]\n", (int)inst->handler_lock);
printf("loop count [%d]\n", c);
printf("event burp!\n");

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


                while (!g_static_rec_mutex_trylock (inst->handler_lock))
                        printf("mutex is going to block [%d]\n", (int)inst->handler_lock);

                //g_static_rec_mutex_lock (inst->handler_lock);
                
                if ( (toggle%2) == 0 ) {
                        toggle++;
                        printf("\n**** EVEN ****, remove the resource\n");
                        if ( (e = remove_resource(inst)) ) {
                                memcpy(event, e, sizeof(*event));
                                g_async_queue_push(inst->eventq_async, event);
                        }
                } else {
                        toggle++;
                        printf("\n**** ODD ****, add the resource\n");
                        if ( (e = add_resource(inst)) ) {
                                memcpy(event, e, sizeof(*event));
                                g_async_queue_push(inst->eventq_async, event);
                        }
                }

                g_static_rec_mutex_unlock (inst->handler_lock);

		oh_cond_signal();
		dbg("dummy thread, signaled");

                nanosleep(&req, &rem);

        }
        
        g_thread_exit(0);

        printf("THREAD END\n"); 
        return 0 ;
}
#endif


