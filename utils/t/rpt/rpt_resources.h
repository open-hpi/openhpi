/* -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Renier Morales <renierm@users.sf.net>
 *
 */

 SaHpiRptEntryT rptentries[] = {
        {
                .EntryId = 1,
                .ResourceId = 1,
                .ResourceInfo = {
                        .ResourceRev = 1,
                        .SpecificVer = 1,
                        .DeviceSupport = 1,
                        .ManufacturerId = 1,
                        .ProductId = 1,
                        .FirmwareMajorRev = 1,
                        .FirmwareMinorRev = 1,
                        .AuxFirmwareRev = 1
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 26,
                        .Data = "This is data for blade 14."
                }
        },
        {
                .EntryId = 2,
                .ResourceId = 2,
                .ResourceInfo = {
                        .ResourceRev = 2,
                        .SpecificVer = 2,
                        .DeviceSupport = 2,
                        .ManufacturerId = 2,
                        .ProductId = 2,
                        .FirmwareMajorRev = 2,
                        .FirmwareMinorRev = 2,
                        .AuxFirmwareRev = 2
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 13
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 26,
                        .Data = "This is data for blade 13."
                }
        },
        {
                .EntryId = 3,
                .ResourceId = 3,
                .ResourceInfo = {
                        .ResourceRev = 3,
                        .SpecificVer = 3,
                        .DeviceSupport = 3,
                        .ManufacturerId = 3,
                        .ProductId = 3,
                        .FirmwareMajorRev = 3,
                        .FirmwareMinorRev = 3,
                        .AuxFirmwareRev = 3
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 12
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 26,
                        .Data = "This is data for blade 12."
                }
        },
        {
                .EntryId = 4,
                .ResourceId = 4,
                .ResourceInfo = {
                        .ResourceRev = 4,
                        .SpecificVer = 4,
                        .DeviceSupport = 4,
                        .ManufacturerId = 4,
                        .ProductId = 4,
                        .FirmwareMajorRev = 4,
                        .FirmwareMinorRev = 4,
                        .AuxFirmwareRev = 4
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 11
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 2
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 26,
                        .Data = "This is data for blade 11."
                }
        },
        {
                .EntryId = 5,
                .ResourceId = 5,
                .ResourceInfo = {
                        .ResourceRev = 5,
                        .SpecificVer = 5,
                        .DeviceSupport = 5,
                        .ManufacturerId = 5,
                        .ProductId = 5,
                        .FirmwareMajorRev = 5,
                        .FirmwareMinorRev = 5,
                        .AuxFirmwareRev = 5
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 10
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 2
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 26,
                        .Data = "This is data for blade 10."
                }
        },
        {
                .EntryId = 6,
                .ResourceId = 6,
                .ResourceInfo = {
                        .ResourceRev = 6,
                        .SpecificVer = 6,
                        .DeviceSupport = 6,
                        .ManufacturerId = 6,
                        .ProductId = 6,
                        .FirmwareMajorRev = 6,
                        .FirmwareMinorRev = 6,
                        .AuxFirmwareRev = 6
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 9
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 2
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
		                        SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
		                        SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE |
		                        SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 25,
                        .Data = "This is data for blade 9."
                }
        },
        {
                .EntryId = 7,
                .ResourceId = 7,
                .ResourceInfo = {
                        .ResourceRev = 7,
                        .SpecificVer = 7,
                        .DeviceSupport = 7,
                        .ManufacturerId = 7,
                        .ProductId = 7,
                        .FirmwareMajorRev = 7,
                        .FirmwareMinorRev = 7,
                        .AuxFirmwareRev = 7
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SYS_MGMNT_MODULE,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_FRU |
		                        SAHPI_CAPABILITY_INVENTORY_DATA |
			                SAHPI_CAPABILITY_RDR |
		                        SAHPI_CAPABILITY_RESOURCE,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 39,
                        .Data = "This is data for the management module."
                }
        },
        {
                .EntryId = 8,
                .ResourceId = 8,
                .ResourceInfo = {
                        .ResourceRev = 8,
                        .SpecificVer = 8,
                        .DeviceSupport = 8,
                        .ManufacturerId = 8,
                        .ProductId = 8,
                        .FirmwareMajorRev = 8,
                        .FirmwareMinorRev = 8,
                        .AuxFirmwareRev = 8
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_INTERCONNECT,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_FRU |
			                SAHPI_CAPABILITY_INVENTORY_DATA |
			                SAHPI_CAPABILITY_RDR |
			                SAHPI_CAPABILITY_RESOURCE,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 35,
                        .Data = "This is data for the switch module."
                }
        },
        {
                .EntryId = 9,
                .ResourceId = 9,
                .ResourceInfo = {
                        .ResourceRev = 9,
                        .SpecificVer = 9,
                        .DeviceSupport = 9,
                        .ManufacturerId = 9,
                        .ProductId = 9,
                        .FirmwareMajorRev = 9,
                        .FirmwareMinorRev = 9,
                        .AuxFirmwareRev = 9
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_POWER_SUPPLY,
                                .EntityInstance = 3
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 2
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_FRU |
			                SAHPI_CAPABILITY_INVENTORY_DATA |
			                SAHPI_CAPABILITY_RDR |
			                SAHPI_CAPABILITY_RESOURCE,
                .ResourceSeverity = SAHPI_MAJOR,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 34,
                        .Data = "This is data for the power module."
                }
        },
        {
                .EntryId = 10,
                .ResourceId = 10,
                .ResourceInfo = {
                        .ResourceRev = 10,
                        .SpecificVer = 10,
                        .DeviceSupport = 10,
                        .ManufacturerId = 10,
                        .ProductId = 10,
                        .FirmwareMajorRev = 10,
                        .FirmwareMinorRev = 10,
                        .AuxFirmwareRev = 10
                },
                .ResourceEntity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .ResourceCapabilities = SAHPI_CAPABILITY_CONTROL |
			                SAHPI_CAPABILITY_INVENTORY_DATA |
                                        SAHPI_CAPABILITY_RDR |
			                SAHPI_CAPABILITY_RESOURCE |
			                SAHPI_CAPABILITY_SEL |
			                SAHPI_CAPABILITY_SENSOR,
                .ResourceSeverity = SAHPI_CRITICAL,
                .ResourceTag = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 25,
                        .Data = "This is data for chassis."
                }
        },
        {}
};

/*****************
 * Start of RDRs *
 *****************/
SaHpiRdrT rdrs[] = {
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 1,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 1
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 1 for Blade 14."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 2,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 2
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 2 for Blade 14."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 3,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 3
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 3 for Blade 14."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 4,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 4
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 4 for Blade 14."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SBC_BLADE,
                                .EntityInstance = 14
                        },
                        {
                                .EntityType = SAHPI_ENT_SUB_CHASSIS,
			        .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 5,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 5
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 5 for Blade 14."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 1,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 6
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 21,
                        .Data = "Sensor 1 for Chassis."
                }
        },
        {
                .RdrType = SAHPI_SENSOR_RDR,
                .Entity = {
                        .Entry[0] = {
                                .EntityType = SAHPI_ENT_SYSTEM_CHASSIS,
                                .EntityInstance = 1
                        },
                        {
                                .EntityType = SAHPI_ENT_ROOT,
                                .EntityInstance = 0
                        }
                },
                .RdrTypeUnion = {
                        .SensorRec = {
                                .Num = 2,
                                .Type = SAHPI_PLATFORM_VIOLATION,
                                .Category = SAHPI_EC_SEVERITY,
                                .EventCtrl = SAHPI_SEC_ENTIRE_SENSOR,
                                .Events = SAHPI_ES_OK | SAHPI_ES_CRITICAL,
                                .Ignore = SAHPI_FALSE,
                                .DataFormat = {
                                        .ReadingFormats = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                        .IsNumeric = SAHPI_TRUE,
                                        .SignFormat = SAHPI_SDF_UNSIGNED,
                                        .BaseUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUnits = SAHPI_SU_UNSPECIFIED,
                                        .ModifierUse = SAHPI_SMUU_NONE,
                                        .FactorsStatic = SAHPI_TRUE,
                                        .Factors = {
                                                .Linearization = SAHPI_SL_LINEAR,
                                        },
                                        .Percentage = SAHPI_FALSE,
                                        .Range = {
                                                .Flags = SAHPI_SRF_MIN | SAHPI_SRF_MAX,
                                                .Max = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 1,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_CRITICAL
                                                        }
                                                },
                                                .Min = {
                                                        .ValuesPresent = SAHPI_SRF_EVENT_STATE | SAHPI_SRF_RAW,
                                                        .Raw = 0,
                                                        .EventStatus = {
                                                                .SensorStatus = SAHPI_SENSTAT_EVENTS_ENABLED,
                                                                .EventStatus = SAHPI_ES_OK
                                                        }
                                                }
                                        }
                                },
                                .ThresholdDefn = {
                                        .IsThreshold = SAHPI_FALSE
                                },
                                .Oem = 7
                        }
                },
                .IdString = {
                        .DataType = SAHPI_TL_TYPE_LANGUAGE,
                        .Language = SAHPI_LANG_ENGLISH,
                        .DataLength = 22,
                        .Data = "Sensor 2 for Chassis."
                }
        },
        {}        
};
