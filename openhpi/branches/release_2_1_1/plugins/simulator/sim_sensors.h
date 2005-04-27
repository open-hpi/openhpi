#ifndef __SIM_SENSORS_H
#define __SIM_SENSORS_H

#include <sim_init.h>
#include <sim_resources.h>
#include <SaHpi.h>

#define DUMMY_MAX_EVENTS_PER_SENSOR 24
#define DUMMY_MAX_READING_MAPS_PER_SENSOR 3

/* Includes an ending NULL entry */
#define DUMMY_MAX_SENSOR_EVENT_ARRAY_SIZE  (DUMMY_MAX_EVENTS_PER_SENSOR + 1)
#define DUMMY_MAX_SENSOR_READING_MAP_ARRAY_SIZE (DUMMY_MAX_READING_MAPS_PER_SENSOR + 1)


/********************
 * Sensor Definitions
 ********************/

struct DummySensorThresholdOids {
       const char *LowMinor;
       const char *LowMajor;
       const char *LowCritical;
       const char *UpMinor;
       const char *UpMajor;
       const char *UpCritical;
       const char *PosThdHysteresis;
       const char *NegThdHysteresis;
       const char *TotalPosThdHysteresis;
       const char *TotalNegThdHysteresis;
};

struct DummySensorWritableThresholdOids {
        const char *LowMinor;
        const char *LowMajor;
        const char *LowCritical;
        const char *UpMinor;
        const char *UpMajor;
        const char *UpCritical;
        const char *PosThdHysteresis;
        const char *NegThdHysteresis;
};

struct SensorMibInfo {
        unsigned int not_avail_indicator_num; /* 0 for none, n>0 otherwise */
        SaHpiBoolT write_only; /* TRUE - Write-only SNMP command */
        const char *oid;
        struct DummySensorThresholdOids threshold_oids;
	struct DummySensorWritableThresholdOids threshold_write_oids;
};

struct sensor_event_map {
      char *event;
      SaHpiBoolT event_assertion;
      SaHpiBoolT event_res_failure;
      SaHpiBoolT event_res_failure_unexpected;
      SaHpiEventStateT event_state;
      SaHpiEventStateT recovery_state;
};

struct sensor_reading_map {
	int num;
	SaHpiSensorRangeT rangemap;
	SaHpiEventStateT state;
};

struct SensorInfo {
	struct SensorMibInfo mib;
        SaHpiEventStateT cur_state; /* This really records the last state read from the SEL */
                           /* Which probably isn't the current state of the sensor */
	SaHpiBoolT sensor_enabled;
	SaHpiBoolT events_enabled;
	SaHpiEventStateT assert_mask;
	SaHpiEventStateT deassert_mask;
	struct sensor_event_map event_array[DUMMY_MAX_SENSOR_EVENT_ARRAY_SIZE];
	struct sensor_reading_map reading2event[DUMMY_MAX_SENSOR_READING_MAP_ARRAY_SIZE];
};

struct dummy_sensor {
	
        /* Usually sensor.Num = index; index is used to search thru sensor arrays. It allows
           sensor.Num to be independent from array index (e.g. for aggregate sensors) */
	
      int index;
      SaHpiSensorRecT sensor;
      struct SensorInfo sensor_info;
      const char *comment;
};


extern struct dummy_sensor dummy_voltage_sensors[];

SaErrorT sim_discover_sensors(RPTable *rpt);
SaErrorT new_sensor(RPTable *rptcache, SaHpiResourceIdT ResId, int Index);
int sim_get_next_sensor_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type);


#endif

