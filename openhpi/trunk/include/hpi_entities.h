/**
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Entities 						      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_ENTITIES_H
#define HPI_ENTITIES_H

/*
** Entity Types
**
** Entities are used to associate specific hardware components with sensors,
** controls, watchdogs, or resources. Entities are defined with an entity
** type enumeration, and an entity instance number (to distinguish between
** multiple instances of a particular type of entity; e.g., multiple power
** supplies in a system).
**
** Entities are uniquely identified in a system with an ordered series of
** Entity Type / Entity Instance pairs called an ?Entity Path?. Each subsequent
** Entity Type/Entity Instance in the path is the next higher ?containing?
** entity. The ?root? of the Entity Path (the outermost level of containment)
** is designated with an Entity Type of SAHPI_ENT_ROOT if the entire Entity Path
** is fewer than SAHPI_MAX_ENTITY_PATH entries in length.
**
** Enumerated Entity Types include those types enumerated by the IPMI Consortium
** for IPMI-managed entities, as well as additional types defined by the
** HPI specification. Room is left in the enumeration for the inclusion of
** Entity Types taken from other lists, if needed in the future.
*/

/* Base values for entity types from various sources. */
#define SAHPI_ENT_IPMI_GROUP 0
#define SAHPI_ENT_SAFHPI_GROUP 0x10000
#define SAHPI_ENT_ROOT_VALUE 0xFFFF

typedef enum
{
	SAHPI_ENT_UNSPECIFIED = SAHPI_ENT_IPMI_GROUP,
	SAHPI_ENT_OTHER,
	SAHPI_ENT_UNKNOWN,
	SAHPI_ENT_PROCESSOR,
	SAHPI_ENT_DISK_BAY, /* Disk or disk bay */
	SAHPI_ENT_PERIPHERAL_BAY,
	SAHPI_ENT_SYS_MGMNT_MODULE, /* System management module */
	SAHPI_ENT_SYSTEM_BOARD, /* Main system board, may also be
				 * processor board and/or internal
				 * expansion board */
	SAHPI_ENT_MEMORY_MODULE, /* Board holding memory devices */
	SAHPI_ENT_PROCESSOR_MODULE, /* Holds processors, use this
			  	     * designation when processors are not
				     * mounted on system board */
	SAHPI_ENT_POWER_SUPPLY, /* Main power supply (supplies) for the
				 * system */
	SAHPI_ENT_ADD_IN_CARD,
	SAHPI_ENT_FRONT_PANEL_BOARD, /* Control panel */
	SAHPI_ENT_BACK_PANEL_BOARD,
	SAHPI_ENT_POWER_SYSTEM_BOARD,
	SAHPI_ENT_DRIVE_BACKPLANE,
	SAHPI_ENT_SYS_EXPANSION_BOARD, /* System internal expansion board
					* (contains expansion slots). */
	SAHPI_ENT_OTHER_SYSTEM_BOARD, /* Part of board set */
	SAHPI_ENT_PROCESSOR_BOARD, /* Holds 1 or more processors. Includes
				    * boards that hold SECC modules) */
	SAHPI_ENT_POWER_UNIT, /* Power unit / power domain (typically
			       * used as a pre-defined logical entity
			       * for grouping power supplies)*/
	SAHPI_ENT_POWER_MODULE, /* Power module / DC-to-DC converter.
			         * Use this value for internal
				 * converters. Note: You should use
				 * entity ID (power supply) for the
				 * main power supply even if the main
				 * supply is a DC-to-DC converter */
	SAHPI_ENT_POWER_MGMNT, /* Power management/distribution
				* board */
	SAHPI_ENT_CHASSIS_BACK_PANEL_BOARD,
	SAHPI_ENT_SYSTEM_CHASSIS,
	SAHPI_ENT_SUB_CHASSIS,
	SAHPI_ENT_OTHER_CHASSIS_BOARD,
	SAHPI_ENT_DISK_DRIVE_BAY,
	SAHPI_ENT_PERIPHERAL_BAY_2,
	SAHPI_ENT_DEVICE_BAY,
	SAHPI_ENT_COOLING_DEVICE, /* Fan/cooling device */
	SAHPI_ENT_COOLING_UNIT, /* Can be used as a pre-defined logical
				 * entity for grouping fans or other
				 * cooling devices. */
	SAHPI_ENT_INTERCONNECT, /* Cable / interconnect */
	SAHPI_ENT_MEMORY_DEVICE, /* This Entity ID should be used for
				  * replaceable memory devices, e.g.
				  * DIMM/SIMM. It is recommended that
				  * Entity IDs not be used for
				  * individual non-replaceable memory
				  * devices. Rather, monitoring and
				  * error reporting should be associated
				  * with the FRU [e.g. memory card]
				  * holding the memory. */
	SAHPI_ENT_SYS_MGMNT_SOFTWARE, /* System Management Software */
	SAHPI_ENT_BIOS,
	SAHPI_ENT_OPERATING_SYSTEM,
	SAHPI_ENT_SYSTEM_BUS,
	SAHPI_ENT_GROUP, /* This is a logical entity for use with
			  * Entity Association records. It is
			  * provided to allow a sensor data
			  * record to point to an entity-
			  * association record when there is no
			  * appropriate pre-defined logical
			  * entity for the entity grouping.
			  * This Entity should not be used as a
			  * physical entity. */
	SAHPI_ENT_REMOTE, /* Out of band management communication
			   * device */
	SAHPI_ENT_EXTERNAL_ENVIRONMENT,
	SAHPI_ENT_BATTERY,
	SAHPI_ENT_CHASSIS_SPECIFIC = SAHPI_ENT_IPMI_GROUP + 0x90,
	SAHPI_ENT_BOARD_SET_SPECIFIC = SAHPI_ENT_IPMI_GROUP + 0xB0,
	SAHPI_ENT_OEM_SYSINT_SPECIFIC = SAHPI_ENT_IPMI_GROUP + 0xD0,
	SAHPI_ENT_ROOT = SAHPI_ENT_ROOT_VALUE,
	SAHPI_ENT_RACK = SAHPI_ENT_SAFHPI_GROUP,
	SAHPI_ENT_SUBRACK,
	SAHPI_ENT_COMPACTPCI_CHASSIS,
	SAHPI_ENT_ADVANCEDTCA_CHASSIS,
	SAHPI_ENT_SYSTEM_SLOT,
	SAHPI_ENT_SBC_BLADE,
	SAHPI_ENT_IO_BLADE,
	SAHPI_ENT_DISK_BLADE,
	SAHPI_ENT_DISK_DRIVE,
	SAHPI_ENT_FAN,
	SAHPI_ENT_POWER_DISTRIBUTION_UNIT,
	SAHPI_ENT_SPEC_PROC_BLADE, /* Special Processing Blade,
				    * including DSP */
	SAHPI_ENT_IO_SUBBOARD, /* I/O Sub-Board, including
				* PMC I/O board */
	SAHPI_ENT_SBC_SUBBOARD, /* SBC Sub-Board, including PMC
				 * SBC board */
	SAHPI_ENT_ALARM_MANAGER, /* Chassis alarm manager board */
	SAHPI_ENT_ALARM_MANAGER_BLADE, /* Blade-based alarm manager */
	SAHPI_ENT_SUBBOARD_CARRIER_BLADE /* Includes PMC Carrier Blade --
					  * Use only if "carrier" is only
					  * function of blade. Else use
					  * primary function (SBC_BLADE,
					  * DSP_BLADE, etc.). */
} SaHpiEntityTypeT;

typedef SaHpiUint32T SaHpiEntityInstanceT;

typedef struct {
	SaHpiEntityTypeT EntityType;
	SaHpiEntityInstanceT EntityInstance;
} SaHpiEntityT;

#define SAHPI_MAX_ENTITY_PATH 16

typedef struct {
	SaHpiEntityT Entry[SAHPI_MAX_ENTITY_PATH];
} SaHpiEntityPathT;

#endif /* HPI_ENTITIES_H */

