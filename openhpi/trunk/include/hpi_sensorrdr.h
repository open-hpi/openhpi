/**
 *
 * Copyright (c) 2003 Intel Corporation
 *
 * TODO: License
 */

/*******************************************************************************
********************************************************************************
********** 							      **********
********** Sensor Resource Data Records 			      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_SENSORRDR_H
#define HPI_SENSORRDR_H

/*
** Sensor Factors
**
** The sensor factors structure defines the conversion factors for linear and
** linearized sensors.
** The SaHpiSensorLinearizationT enumeration coupled with the various other
** sensor factors define a formula that can be applied to raw sensor data to
** convert it to appropriate engineering units. If linearization is
** SAHPI_SL_NONLINEAR, SAHPI_SL_UNSPECIFIED, or SAHPI_SL_OEM then there is no
** predefined conversion from raw to interpreted and the sensor factors may or
** may not be meaningful depending on the implementation.
** For other linearization values, raw readings may be converted to interpreted
** values using the formula:
** Interpreted = L [( M*raw + B*10^ExpB )*10^ExpR ]
** where "L[x]" is the indicated linearization function
** (for SAHPI_SL_LINEAR, L[x]=x).
** The Tolerance Factor is given as +/- 1/2 raw counts, so tolerance in
** interpreted values can be calculated as:
** L[ M * ToleranceFactor/2 * 10^ExpR ]
** The Accuracy Factor is given as 1/100 of a percent, scaled up by ExpA. Thus
** the accuracy is:
** ( ( AccuracyFactor/100 ) / 10^ExpA )%
*/
typedef enum {
	SAHPI_SL_LINEAR = 0, /* Already linear */
	SAHPI_SL_LN,
	SAHPI_SL_LOG10,
	SAHPI_SL_LOG2,
	SAHPI_SL_E,
	SAHPI_SL_EXP10,
	SAHPI_SL_EXP2,
	SAHPI_SL_1OVERX,
	SAHPI_SL_SQRX,
	SAHPI_SL_CUBEX,
	SAHPI_SL_SQRTX,
	SAHPI_SL_CUBERTX,
	SAHPI_SL_NONLINEAR = 0x70, /* Cannot be linearized with a predefind formula*/
	SAHPI_SL_OEM,
	SAHPI_SL_UNSPECIFIED = 0xFF
} SaHpiSensorLinearizationT;

typedef struct {
	SaHpiInt16T M_Factor; /* M Factor */
	SaHpiInt16T B_Factor; /* B Factor */
	SaHpiUint16T AccuracyFactor; /* Accuracy */
	SaHpiUint8T ToleranceFactor; /* Tolerance */
	SaHpiUint8T ExpA; /* Accuracy Exp */
	SaHpiInt8T ExpR; /* Result Exp */
	SaHpiInt8T ExpB; /* B Exp */
	SaHpiSensorLinearizationT Linearization;
} SaHpiSensorFactorsT;

/*
** Sensor Range
** Sensor range values can include minimum, maximum, normal minimum, normal
** maximum, and nominal values.
*/
typedef SaHpiUint8T SaHpiSensorRangeFlagsT;
#define SAHPI_SRF_MIN (SaHpiSensorRangeFlagsT)0x10
#define SAHPI_SRF_MAX (SaHpiSensorRangeFlagsT)0x08
#define SAHPI_SRF_NORMAL_MIN (SaHpiSensorRangeFlagsT)0x04
#define SAHPI_SRF_NORMAL_MAX (SaHpiSensorRangeFlagsT)0x02
#define SAHPI_SRF_NOMINAL (SaHpiSensorRangeFlagsT)0x01
typedef struct {
	SaHpiSensorRangeFlagsT Flags;
	SaHpiSensorReadingT Max;
	SaHpiSensorReadingT Min;
	SaHpiSensorReadingT Nominal;
	SaHpiSensorReadingT NormalMax;
	SaHpiSensorReadingT NormalMin;
} SaHpiSensorRangeT;
/*
** Sensor Units
** This is a list of all the sensor units supported by HPI.
*/
typedef enum {
	SAHPI_SU_UNSPECIFIED = 0, SAHPI_SU_DEGREES_C, SAHPI_SU_DEGREES_F,
	SAHPI_SU_DEGREES_K, SAHPI_SU_VOLTS, SAHPI_SU_AMPS,
	SAHPI_SU_WATTS, SAHPI_SU_JOULES, SAHPI_SU_COULOMBS,
	SAHPI_SU_VA, SAHPI_SU_NITS, SAHPI_SU_LUMEN,
	SAHPI_SU_LUX, SAHPI_SU_CANDELA, SAHPI_SU_KPA,
	SAHPI_SU_PSI, SAHPI_SU_NEWTON, SAHPI_SU_CFM,
	SAHPI_SU_RPM, SAHPI_SU_HZ, SAHPI_SU_MICROSECOND,
	SAHPI_SU_MILLISECOND, SAHPI_SU_SECOND, SAHPI_SU_MINUTE,
	SAHPI_SU_HOUR, SAHPI_SU_DAY, SAHPI_SU_WEEK,
	SAHPI_SU_MIL, SAHPI_SU_INCHES, SAHPI_SU_FEET,
	SAHPI_SU_CU_IN, SAHPI_SU_CU_FEET, SAHPI_SU_MM,
	SAHPI_SU_CM, SAHPI_SU_M, SAHPI_SU_CU_CM,
	SAHPI_SU_CU_M, SAHPI_SU_LITERS, SAHPI_SU_FLUID_OUNCE,
	SAHPI_SU_RADIANS, SAHPI_SU_STERADIANS, SAHPI_SU_REVOLUTIONS,
	SAHPI_SU_CYCLES, SAHPI_SU_GRAVITIES, SAHPI_SU_OUNCE,
	SAHPI_SU_POUND, SAHPI_SU_FT_LB, SAHPI_SU_OZ_IN,
	SAHPI_SU_GAUSS, SAHPI_SU_GILBERTS, SAHPI_SU_HENRY,
	SAHPI_SU_MILLIHENRY, SAHPI_SU_FARAD, SAHPI_SU_MICROFARAD,
	SAHPI_SU_OHMS, SAHPI_SU_SIEMENS, SAHPI_SU_MOLE,
	SAHPI_SU_BECQUEREL, SAHPI_SU_PPM, SAHPI_SU_RESERVED,
	SAHPI_SU_DECIBELS, SAHPI_SU_DBA, SAHPI_SU_DBC,
	SAHPI_SU_GRAY, SAHPI_SU_SIEVERT, SAHPI_SU_COLOR_TEMP_DEG_K,
	SAHPI_SU_BIT, SAHPI_SU_KILOBIT, SAHPI_SU_MEGABIT,
	SAHPI_SU_GIGABIT, SAHPI_SU_BYTE, SAHPI_SU_KILOBYTE,
	SAHPI_SU_MEGABYTE, SAHPI_SU_GIGABYTE, SAHPI_SU_WORD,
	SAHPI_SU_DWORD, SAHPI_SU_QWORD, SAHPI_SU_LINE,
	SAHPI_SU_HIT, SAHPI_SU_MISS, SAHPI_SU_RETRY,
	SAHPI_SU_RESET, SAHPI_SU_OVERRUN, SAHPI_SU_UNDERRUN,
	SAHPI_SU_COLLISION, SAHPI_SU_PACKETS, SAHPI_SU_MESSAGES,
	SAHPI_SU_CHARACTERS, SAHPI_SU_ERRORS, SAHPI_SU_CORRECTABLE_ERRORS,
	SAHPI_SU_UNCORRECTABLE_ERRORS
} SaHpiSensorUnitsT;
/*
** Modifier Unit Use
** This type defines how the modifier unit is used. For example: base unit ==
** meter, modifier unit == seconds, and modifier unit use ==
** SAHPI_SMUU_BASIC_OVER_MODIFIER. The resulting unit would be meters per second.
*/
typedef enum {
	SAHPI_SMUU_NONE = 0,
	SAHPI_SMUU_BASIC_OVER_MODIFIER, /* Basic Unit / Modifier Unit */
	SAHPI_SMUU_BASIC_TIMES_MODIFIER /* Basic Unit * Modifier Unit */
} SaHpiSensorModUnitUseT;
/*
** Sign Format
** This type defines what the sign format of the sensor's raw value is (1's
** complement, unsigned, etc.).
*/
typedef enum {
	SAHPI_SDF_UNSIGNED = 0,
	SAHPI_SDF_1S_COMPLEMENT,
	SAHPI_SDF_2S_COMPLEMENT
} SaHpiSensorSignFormatT;
/*
** Data Format
** This structure encapsulates all of the various types that make up the
** definition of sensor data.
*/
typedef struct {
	SaHpiSensorReadingFormatsT ReadingFormats; /* Indicates if sensor supports
						    * readings in raw, interpreted,
						    * and/or event status formats */
	SaHpiBoolT IsNumeric; /* If FALSE, rest of this
			       * structure is not
			       * meaningful */
	SaHpiSensorSignFormatT SignFormat; /* Signed format */
	SaHpiSensorUnitsT BaseUnits; /* Base units (meters, etc.) */
	SaHpiSensorUnitsT ModifierUnits; /* Modifier unit (second, etc.) */
	SaHpiSensorModUnitUseT ModifierUse; /* Modifier use(m/sec, etc.) */
	SaHpiBoolT FactorsStatic; /* True if the sensor factors
				   * are static. If false
				   * factors vary over sensor
				   * range, and are not
				   * accessible through HPI */
	SaHpiSensorFactorsT Factors;
	SaHpiBoolT Percentage; /* Is value a percentage */
	SaHpiSensorRangeT Range; /* Valid range of sensor */
} SaHpiSensorDataFormatT;
/*
** Threshold Support
**
** These types define what threshold values are readable, writable, and fixed.
** It also defines how the threshold values are read and written.
*/
typedef SaHpiUint8T SaHpiSensorThdMaskT;
#define SAHPI_STM_LOW_MINOR (SaHpiSensorThdMaskT)0x01
#define SAHPI_STM_LOW_MAJOR (SaHpiSensorThdMaskT)0x02
#define SAHPI_STM_LOW_CRIT (SaHpiSensorThdMaskT)0x04
#define SAHPI_STM_UP_MINOR (SaHpiSensorThdMaskT)0x08
#define SAHPI_STM_UP_MAJOR (SaHpiSensorThdMaskT)0x10
#define SAHPI_STM_UP_CRIT (SaHpiSensorThdMaskT)0x20
#define SAHPI_STM_UP_HYSTERESIS (SaHpiSensorThdMaskT)0x40
#define SAHPI_STM_LOW_HYSTERESIS (SaHpiSensorThdMaskT)0x80

typedef SaHpiUint8T SaHpiSensorThdCapT;

#define SAHPI_STC_RAW (SaHpiSensorThdMaskT)0x01 /* read/write as
					         * raw counts */
#define SAHPI_STC_INTERPRETED (SaHpiSensorThdMaskT)0x02 /* read/write as
							 * interpreted */

typedef struct {
	SaHpiBoolT IsThreshold; /* True if the sensor
				 * supports thresholds. If false,
				 * rest of structure is not
				 * meaningful. */
	SaHpiSensorThdCapT TholdCapabilities;
	SaHpiSensorThdMaskT ReadThold; /* Readable thresholds */
	SaHpiSensorThdMaskT WriteThold; /* Writable thresholds */
	SaHpiSensorThdMaskT FixedThold; /* Fixed thresholds */
} SaHpiSensorThdDefnT;

/*
** Event Control
**
** This type defines how sensor event messages can be controlled (can be turned
** off and on for each type of event, etc.).
*/
typedef enum {
	SAHPI_SEC_PER_EVENT = 0, /* Event message control per event */
	SAHPI_SEC_ENTIRE_SENSOR, /* Control for entire sensor only */
	SAHPI_SEC_GLOBAL_DISABLE, /* Global disable of events only */
	SAHPI_SEC_NO_EVENTS /* Events not supported */
} SaHpiSensorEventCtrlT;
/*
** Record
**
** This is the sensor resource data record which describes all of the static
** data associated with a sensor.
*/
typedef struct {
	SaHpiSensorNumT Num; /* Sensor Number/Index */
	SaHpiSensorTypeT Type; /* General Sensor Type */
	SaHpiEventCategoryT Category; /* Event category */
	SaHpiSensorEventCtrlT EventCtrl; /* How events can be controlled */
	SaHpiEventStateT Events; /* Bit mask of event states
				  * supported */
	SaHpiBoolT Ignore; /* Ignore sensor (entity not
			    * present, disabled, etc.) */
	SaHpiSensorDataFormatT DataFormat; /* Format of the data */
	SaHpiSensorThdDefnT ThresholdDefn; /* Threshold Definition */
	SaHpiUint32T Oem; /* Reserved for OEM use */
} SaHpiSensorRecT;

#endif /* HPI_SENSORRDR_H */

