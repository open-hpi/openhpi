/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 *     Kevin Gao <kevin.gao@linux.intel.com>
 *     Rusty Lynch <rusty.lynch@linux.intel.com>
 *     Racing Guo <racing.guo@intel.com>
 */

#include "ipmi.h"

/* 
 * Use for getting sensor reading
 */
struct ohoi_sensor_reading {
	SaHpiSensorReadingT	reading;
        SaHpiEventStateT        ev_state;
	int			done;
	int			rvalue;
};

/*
 * Use for getting/setting sensor threadholds
 */
struct ohoi_sensor_thresholds {
	SaHpiSensorThresholdsT	sensor_thres;
	int			thres_done;
        int                     hyster_done;
	int			rvalue;
};

/*
 * Use for sensor event enable and sensor event masks
 */

struct ohoi_sensor_event_enable_masks {
	SaHpiBoolT   enable;
	SaHpiEventStateT  assert;
	SaHpiEventStateT  deassert;
	int done;
	int rvalue;
};

static int ignore_sensor(ipmi_sensor_t *sensor)
{
        ipmi_entity_t *ent;

        if ( ipmi_sensor_get_ignore_if_no_entity(sensor) )
                return 0;

        ent = ipmi_sensor_get_entity(sensor);

        if (ent && ipmi_entity_is_present(ent))
                return 0;

        return 1;
}
static void sensor_read_states(ipmi_sensor_t *sensor,
			       int	   err,
			       ipmi_states_t *states,
			       void *cb_data)
{
	struct ohoi_sensor_reading *p = cb_data;

	p->done = 1;
	if (err) {
		dbg("sensor reading state error");
		p->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		return;
	}
	p->reading.IsSupported = SAHPI_FALSE;
	p->ev_state = states->__states;
}
static void sensor_reading(ipmi_sensor_t		*sensor,
		  	   int 				err,
			   enum ipmi_value_present_e	value_present,
			   unsigned int 		raw_val,
			   double 			val,
			   ipmi_states_t 		*states,
			   void 			*cb_data)
{
	struct ohoi_sensor_reading *p = cb_data;
 
	p->done = 1;

	if (err) {
		dbg("sensor reading error");
		p->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		return;
	}

	p->reading.IsSupported = SAHPI_FALSE;

	if (value_present == IPMI_BOTH_VALUES_PRESENT) {
		p->reading.IsSupported = SAHPI_TRUE;
		p->reading.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		p->reading.Value.SensorFloat64 = val;
	}else if(value_present == IPMI_RAW_VALUE_PRESENT) {
		p->reading.IsSupported = SAHPI_TRUE;
		p->reading.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		p->reading.Value.SensorFloat64 = raw_val;
	}

	p->ev_state = states->__states;
}

static void get_sensor_reading(ipmi_sensor_t *sensor, void *cb_data)
{
	struct ohoi_sensor_reading *reading_data;	
	int rv;	

        reading_data = cb_data;
        
	if (ignore_sensor(sensor)) {
		reading_data->done = 1;
		reading_data->rvalue = SA_ERR_HPI_NOT_PRESENT;
		dbg("Sensor is not present, ignored");
		return;
	}	
	
	if (ipmi_sensor_get_event_reading_type(sensor) ==
			 IPMI_EVENT_READING_TYPE_THRESHOLD) {
	
		rv = ipmi_reading_get(sensor, sensor_reading, reading_data);
		if (rv) {
			reading_data->done = 1;
			reading_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
			dbg("Unable to get sensor reading: %s\n", strerror( rv ) );
			return;
		}
	} else {
		rv = ipmi_states_get(sensor, sensor_read_states, reading_data);
		if (rv) {
			reading_data->done = 1;
			reading_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
			dbg("Unable to get sensor reading states: %s\n",
					strerror( rv ) );
		}
	}
}

int ohoi_get_sensor_reading(ipmi_sensor_id_t sensor_id, 
                            SaHpiSensorReadingT *reading,
			    SaHpiEventStateT * ev_state,
			    void *cb_data)
{
	struct ohoi_handler *ipmi_handler = cb_data;
	struct ohoi_sensor_reading reading_data;	
        int rv;

	memset(&reading_data, 0, sizeof(reading_data));

        rv = ipmi_sensor_pointer_cb(sensor_id, 
				    get_sensor_reading,
				    &reading_data);
	if (rv) {
		dbg("Unable to convert sensor_id to pointer");
		return SA_ERR_HPI_INVALID_CMD;
	}

	rv = ohoi_loop(&reading_data.done, ipmi_handler);

	if (rv)
		return rv;
	if (reading_data.rvalue)
		return reading_data.rvalue;

	*reading = reading_data.reading;
	*ev_state = reading_data.ev_state;

	return SA_OK;
}

static void thres_get(ipmi_sensor_t		*sensor,
		      ipmi_thresholds_t		*th,
		      unsigned int		event,
		      SaHpiSensorReadingT	*thres)
{
	int val;
	
	ipmi_sensor_threshold_readable(sensor, event, &val);
	if (val) {
		thres->IsSupported = SAHPI_TRUE;
		thres->Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
		thres->Value.SensorFloat64 = th->vals[event].val;
	}else {
		thres->IsSupported = SAHPI_FALSE;
	}
}

static void thresholds_read(ipmi_sensor_t	*sensor,
			    int			err,
			    ipmi_thresholds_t	*th,
			    void		*cb_data)
{
	struct ohoi_sensor_thresholds *p = cb_data;
	
	p->thres_done = 1;

	if (err) {
		p->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		dbg("sensor thresholds reading error");
		return;
	}
	
	thres_get(sensor, th, IPMI_LOWER_NON_CRITICAL,
		  &p->sensor_thres.LowMinor);
	thres_get(sensor, th, IPMI_LOWER_CRITICAL,
		  &p->sensor_thres.LowMajor);
	thres_get(sensor, th, IPMI_LOWER_NON_RECOVERABLE,
		  &p->sensor_thres.LowCritical);
	thres_get(sensor, th, IPMI_UPPER_NON_CRITICAL,
		  &p->sensor_thres.UpMinor);
	thres_get(sensor, th, IPMI_UPPER_CRITICAL,
		  &p->sensor_thres.UpMajor);
	thres_get(sensor, th, IPMI_UPPER_NON_RECOVERABLE,
		  &p->sensor_thres.UpCritical);

}

static SaErrorT get_thresholds(ipmi_sensor_t	*sensor,
			       struct ohoi_sensor_thresholds	*thres_data)
{
	int		rv;

	rv = ipmi_thresholds_get(sensor, thresholds_read, thres_data);
	if (rv) 
		dbg("Unable to get sensor thresholds: 0x%x\n", rv);
	return (rv? SA_ERR_HPI_INVALID_CMD : SA_OK);
}

static void hysteresis_read(ipmi_sensor_t	*sensor,
			    int			err,
			    unsigned int	positive_hysteresis,
			    unsigned int 	negative_hysteresis,
			    void 		*cb_data)
{
	struct ohoi_sensor_thresholds *p = cb_data;
	
	p->hyster_done = 1;

	if (err) {
		p->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		dbg("sensor hysteresis reading error");
		return;		
	}
	
	p->sensor_thres.PosThdHysteresis.IsSupported = SAHPI_TRUE;
	p->sensor_thres.PosThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
	p->sensor_thres.PosThdHysteresis.Value.SensorFloat64 = positive_hysteresis;

	p->sensor_thres.NegThdHysteresis.IsSupported = SAHPI_TRUE;
        p->sensor_thres.NegThdHysteresis.Type = SAHPI_SENSOR_READING_TYPE_FLOAT64;
        p->sensor_thres.NegThdHysteresis.Value.SensorFloat64 = negative_hysteresis;

}

static SaErrorT get_hysteresis(ipmi_sensor_t			*sensor,
			  struct ohoi_sensor_thresholds	*thres_data)
{
	int		rv;
	
	rv = ipmi_sensor_get_hysteresis(sensor, hysteresis_read, thres_data);
        if (rv)
                dbg("Unable to get sensor hysteresis: 0x%x\n", rv);
        
	return (rv? SA_ERR_HPI_INVALID_CMD : SA_OK);

}

static void get_sensor_thresholds(ipmi_sensor_t *sensor, 
                                  void          *cb_data)
{
	struct ohoi_sensor_thresholds *thres_data;
	int rv;
	
        thres_data = cb_data;

	if (ignore_sensor(sensor)) {
		thres_data->hyster_done = 1;
		thres_data->thres_done = 1;
		thres_data->rvalue = SA_ERR_HPI_NOT_PRESENT;
                dbg("ENTITY_NOT_PRESENT");
		return;
	}	

	if (ipmi_sensor_get_event_reading_type(sensor) ==
			IPMI_EVENT_READING_TYPE_THRESHOLD) {
		if (ipmi_sensor_get_threshold_access(sensor) ==
				IPMI_THRESHOLD_ACCESS_SUPPORT_NONE)
			dbg("sensor doesn't support threshold read");
		else {
			rv = get_thresholds(sensor, thres_data);
			if (rv < 0) {
                                dbg("Unable to get sensor thresholds");
				return;
                        }
		}
					
		rv = ipmi_sensor_get_hysteresis_support(sensor);
		if (rv == IPMI_HYSTERESIS_SUPPORT_NONE) {
#if 0
			/* I'm zeroing them so we return but invalid data FIXME? */
			thres_data->sensor_thres->PosThdHysteresis.ValuesPresent = 0;
			thres_data->sensor_thres->NegThdHysteresis.ValuesPresent = 0;
#else
			thres_data->sensor_thres.PosThdHysteresis.IsSupported = SAHPI_FALSE;
			thres_data->sensor_thres.NegThdHysteresis.IsSupported = SAHPI_FALSE;
#endif
                        thres_data->hyster_done = 1; /* read no more */
			return;
		} else {
			if (rv == IPMI_HYSTERESIS_SUPPORT_READABLE ||
					rv == IPMI_HYSTERESIS_SUPPORT_SETTABLE) { 
				rv = get_hysteresis(sensor, thres_data);
				if (rv < 0)
					dbg("failed to get hysteresis");
			}
		}
		
	} else {
		dbg("Not threshold sensor!");
        }
	
	return;
}

static int is_get_sensor_thresholds_done(const void *cb_data)
{
        const struct ohoi_sensor_thresholds *thres_data;
        
        thres_data = cb_data;
        return (thres_data->thres_done && thres_data->hyster_done);
}

int ohoi_get_sensor_thresholds(ipmi_sensor_id_t sensor_id,
			       SaHpiSensorThresholdsT *thres, void *cb_data)
{
	struct ohoi_handler *ipmi_handler = cb_data;
	struct ohoi_sensor_thresholds	thres_data;
        int rv;
	
        memset(&thres_data, 0, sizeof(thres_data));
        
        rv = ipmi_sensor_pointer_cb(sensor_id,
                                    get_sensor_thresholds,
                                    &thres_data);
        if (rv) {
                dbg("Unable to convert sensor id into pointer");
		return SA_ERR_HPI_INVALID_CMD;
        }

        rv = ohoi_loop_until(is_get_sensor_thresholds_done, 
                               &thres_data, 5, ipmi_handler);
	if (rv)
		return rv;
	
	if (thres_data.rvalue)
		return thres_data.rvalue;

	if (thres)
		*thres = thres_data.sensor_thres;
	return SA_OK;
}

static void set_data(ipmi_sensor_t *sensor, int err, void *cb_data)
{
	int *done = cb_data;
	
	if (err)
		dbg("Something wrong in setting thresholds");
	*done = 1;
}

static int thres_cpy(ipmi_sensor_t			*sensor, 
		      const SaHpiSensorReadingT		reading,
		      unsigned int			event,
		      ipmi_thresholds_t			*info) 
{
	int	val;

	ipmi_sensor_threshold_settable(sensor, event, &val);
	if (!val)
	       return 0;
	
#if 0
	if (reading.ValuesPresent & SAHPI_SRF_RAW) {
		rv = ipmi_sensor_convert_from_raw(sensor, reading.Raw, &tmp);
		if (rv < 0) {
			dbg("Invalid raw value");
			return -1;
		}
		info->vals[event].status = 1;
		info->vals[event].val = tmp;
	}
	else if (reading.ValuesPresent & SAHPI_SRF_INTERPRETED) {
		if (reading.Interpreted.Type ==
		    SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32) {
		info->vals[event].status = 1;
		info->vals[event].val = reading.Interpreted.Value.SensorFloat32;
		}
		else {
			dbg("Invalid input thresholds");
			return -1;
		}
	}
#else
	info->vals[event].status = 1;
	switch (reading.Type) {
		/*Fix Me* case...*/
		case  SAHPI_SENSOR_READING_TYPE_INT64:
		case  SAHPI_SENSOR_READING_TYPE_UINT64:
		case  SAHPI_SENSOR_READING_TYPE_BUFFER:
			break;
		case SAHPI_SENSOR_READING_TYPE_FLOAT64:
        		info->vals[event].val = 
				reading.Value.SensorFloat64;
			break;
	}
#endif
	return 0;
}

static int init_thresholeds_info(ipmi_sensor_t			*sensor, 
				  const SaHpiSensorThresholdsT	*thres,
				  ipmi_thresholds_t		*info)
{
	int rv;
	
	rv = thres_cpy(sensor, thres->LowMinor, IPMI_LOWER_NON_CRITICAL, info);
	if (rv < 0)
		return -1;
	
	rv = thres_cpy(sensor, thres->LowMajor, IPMI_LOWER_CRITICAL, info);
	if (rv < 0)
		return -1;
	
	rv = thres_cpy(sensor, thres->LowCritical, IPMI_LOWER_NON_RECOVERABLE,
		       info);
	if (rv < 0)
		return -1;
	
	rv = thres_cpy(sensor, thres->UpMinor, IPMI_UPPER_NON_CRITICAL, info);
	if (rv < 0)
		return -1;
	
	rv = thres_cpy(sensor, thres->UpMajor, IPMI_UPPER_CRITICAL, info);
	if (rv < 0)
		return -1;
	
	rv = thres_cpy(sensor, thres->UpCritical, IPMI_UPPER_NON_RECOVERABLE,
		       info);
	if (rv < 0)
		return -1;

	return 0;
}

static int set_thresholds(ipmi_sensor_t                 *sensor, 
			  struct ohoi_sensor_thresholds *thres_data)
{
	ipmi_thresholds_t	info;
	int			rv;	
	
	memset(&info, 0, sizeof(info));
	rv = init_thresholeds_info(sensor, &thres_data->sensor_thres, &info);
	if (rv < 0)
		return -1;
	
	rv = ipmi_thresholds_set(sensor, &info, set_data, 
                                 &thres_data->thres_done);
	if (rv) {
		dbg("Unable to set sensor thresholds: 0x%x\n", rv);
		return -1;
	}

	return rv;
}

static int set_hysteresis(ipmi_sensor_t	                *sensor,
			  struct ohoi_sensor_thresholds	*thres_data)
{
	int			rv;	
	unsigned int		pos = 0, neg = 0;
	SaHpiSensorReadingT	pos_reading 
                = thres_data->sensor_thres.PosThdHysteresis;
	SaHpiSensorReadingT	neg_reading 
                = thres_data->sensor_thres.NegThdHysteresis;	
	
        switch (pos_reading.Type) {
                case SAHPI_SENSOR_READING_TYPE_INT64:
                        pos = pos_reading.Value.SensorInt64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_UINT64:
                        pos = pos_reading.Value.SensorUint64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                        pos = pos_reading.Value.SensorFloat64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                        dbg("ipmi sensor doesn't support this type of reading");
                        return -1;   
        }
	
        switch (neg_reading.Type) {
                case SAHPI_SENSOR_READING_TYPE_INT64:
                        neg = neg_reading.Value.SensorInt64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_UINT64:
                        neg = neg_reading.Value.SensorUint64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_FLOAT64:
                        neg = neg_reading.Value.SensorFloat64;
                        break;
                case SAHPI_SENSOR_READING_TYPE_BUFFER:
                        dbg("ipmi sensor doesn't support this type of reading");
                        return -1;
        }

	rv = ipmi_sensor_set_hysteresis(sensor, pos, neg, set_data, 
                                        &thres_data->hyster_done);
	if (rv) {
		dbg("Unable to set sensor thresholds: 0x%x\n", rv);
		return -1;
	}

        return rv;
}

static void set_sensor_thresholds(ipmi_sensor_t *sensor, 
                                  void          *cb_data)
{
        struct ohoi_sensor_thresholds *thres_data;
	int rv;	

        thres_data = cb_data;
	if (ignore_sensor(sensor)) {
		dbg("sensor is ignored");
		return;
	}	
	
	if (ipmi_sensor_get_event_reading_type(sensor) ==
			IPMI_EVENT_READING_TYPE_THRESHOLD) {
		if (ipmi_sensor_get_threshold_access(sensor) ==
		    IPMI_THRESHOLD_ACCESS_SUPPORT_SETTABLE) {
			rv = set_thresholds(sensor, thres_data);
			if (rv < 0) {
                                dbg("Unable to set thresholds");
				return;
                        }
		} else
			dbg("sensor doesn't support threshold set");	

		rv = ipmi_sensor_get_hysteresis_support(sensor);
		if (rv == IPMI_HYSTERESIS_SUPPORT_SETTABLE) { 
			rv = set_hysteresis(sensor, thres_data);
			if (rv < 0) {
                                dbg("Unable to set hysteresis");
				return;
                        }
		} else
			dbg("sensor doesn't support hysteresis set");
	} else
		dbg("Not threshold sensor!");
}

int ohoi_set_sensor_thresholds(ipmi_sensor_id_t		        sensor_id, 
			       const SaHpiSensorThresholdsT     *thres,
			       void *cb_data)
{
	struct ohoi_handler *ipmi_handler = cb_data;
        struct ohoi_sensor_thresholds thres_data;
        int rv;
        
	memset(&thres_data, 0, sizeof(thres_data));
	thres_data.sensor_thres = *thres;
        
        rv = ipmi_sensor_pointer_cb(sensor_id,
				    set_sensor_thresholds,
				    &thres_data);
		
        if (rv) {
		dbg("Unable to convert sensor_id to pointer");
                return SA_ERR_HPI_INVALID_CMD;
	}

        return ohoi_loop_until(is_get_sensor_thresholds_done, 
                               &thres_data, 5, ipmi_handler);
}

static void set_sensor_enable(ipmi_sensor_t *sensor,
			      void          *cb_data)
{
	SaHpiBoolT *enable = cb_data;
	ipmi_sensor_set_ignore_if_no_entity(sensor, *enable);
}

int ohoi_set_sensor_enable(ipmi_sensor_id_t sensor_id,
			   SaHpiBoolT   enable,
			   void *cb_data)
{
	SaErrorT rv;

        rv = ipmi_sensor_pointer_cb(sensor_id,
				    set_sensor_enable,
		  		    &enable);
	if (rv) {
		dbg("Unable to convert sensor_id to pointer");
                return SA_ERR_HPI_INVALID_CMD;
        }
	return SA_OK;
}

static void event_enable_masks_read(ipmi_sensor_t	*sensor,
			           int 			err,
			           ipmi_event_state_t	*state,
			           void			*cb_data)
{
	struct ohoi_sensor_event_enable_masks *p = cb_data;
	int 				rv;

	p->done = 1;

	if (err) {
		dbg("Sensor event enable reading error");
		p->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		return;
	}

	p->enable = SAHPI_FALSE;

	rv = ipmi_event_state_get_events_enabled(state);
        if (rv)
		p->enable = SAHPI_TRUE;

	p->assert = (SaHpiEventStateT)state->__assertion_events;
	p->deassert = (SaHpiEventStateT)state->__deassertion_events;

}

static void get_sensor_event_enable_masks(ipmi_sensor_t *sensor, 
                                          void          *cb_data)
{
	struct ohoi_sensor_event_enable_masks *enable_data;
	int rv;
	
        enable_data = cb_data;
        
	if (ignore_sensor(sensor)) {
		dbg("sensor is ignored");
                enable_data->done = 1;
		enable_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		return;
	}	

	if ((ipmi_sensor_get_event_support(sensor) == IPMI_EVENT_SUPPORT_PER_STATE)||
	   (ipmi_sensor_get_event_support(sensor) == IPMI_EVENT_SUPPORT_ENTIRE_SENSOR)){
		rv = ipmi_sensor_events_enable_get(sensor, event_enable_masks_read,
					           enable_data);
		if (rv) {
			dbg("Unable to sensor event enable: 0x%x\n", rv);
                	enable_data->done = 1;
			enable_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
			return;
		}
	} else {
                dbg("Sensor do not support event");
                enable_data->done = 1;
		enable_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
        }
}

static void set_sensor_event_enable_masks(ipmi_sensor_t      *sensor,
                                    void               *cb_data)
{
	struct ohoi_sensor_event_enable_masks *enable_data;
	int			rv;
	ipmi_event_state_t	info;

        enable_data = cb_data;

	if (ignore_sensor(sensor)) {
		dbg("sensor is ignored");
		enable_data->done = 1;
		enable_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
		return;
	}

	ipmi_event_state_init(&info);
	if (enable_data->enable == SAHPI_TRUE)
		ipmi_event_state_set_events_enabled(&info, 1);
	else 
		ipmi_event_state_set_events_enabled(&info, 0);

	if ((ipmi_sensor_get_event_support(sensor) == IPMI_EVENT_SUPPORT_PER_STATE)||
	    (ipmi_sensor_get_event_support(sensor) == IPMI_EVENT_SUPPORT_ENTIRE_SENSOR)) {
		int i;

		for (i = 0; i < 32 ; i++) {
			if (enable_data->assert & 1<<i ) 
				ipmi_discrete_event_set(&info, i, IPMI_ASSERTION);
			else 
				ipmi_discrete_event_clear(&info, i, IPMI_ASSERTION);

			if (enable_data->deassert & 1<<i ) 
				ipmi_discrete_event_set(&info, i, IPMI_DEASSERTION);
			else
				ipmi_discrete_event_clear(&info, i, IPMI_DEASSERTION);
		}
	}

	rv = ipmi_sensor_events_enable_set(sensor, &info, set_data,
					   &enable_data->done);
	if (rv) {
		dbg("Unable to sensor event enable: 0x%x\n", rv);
		enable_data->done = 1;
		enable_data->rvalue = SA_ERR_HPI_INTERNAL_ERROR;
	}
}


int ohoi_get_sensor_event_enable_masks(ipmi_sensor_id_t sensor_id,
				       SaHpiBoolT   *enable,
				       SaHpiEventStateT  *assert,
				       SaHpiEventStateT  *deassert,
				       void *cb_data)
{
        int rv;
	struct ohoi_handler *ipmi_handler = cb_data;
	struct ohoi_sensor_event_enable_masks enable_data;
        
	memset(&enable_data, 0, sizeof(enable_data));
        
        rv = ipmi_sensor_pointer_cb(sensor_id,
				    get_sensor_event_enable_masks,
				    &enable_data);
        if (rv) {
		dbg("Unable to convert sensor_id to pointer");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        rv = ohoi_loop(&enable_data.done, ipmi_handler);

	if (rv || enable_data.rvalue)
		return SA_ERR_HPI_INVALID_CMD;

	*enable = enable_data.enable;
	*assert = enable_data.assert;
	*deassert = enable_data.deassert;

	return SA_OK;

}

int ohoi_set_sensor_event_enable_masks(ipmi_sensor_id_t sensor_id,
			               SaHpiBoolT enable,
			 	       SaHpiEventStateT  assert,
				       SaHpiEventStateT  deassert,
				       void *cb_data)
{

        int rv;
	struct ohoi_handler *ipmi_handler = cb_data;
	struct ohoi_sensor_event_enable_masks enable_data;


	memset(&enable_data, 0, sizeof(enable_data));
	enable_data.enable = enable;
	enable_data.assert = assert;
	enable_data.deassert = deassert;

        rv = ipmi_sensor_pointer_cb(sensor_id,
				    set_sensor_event_enable_masks,
		  		    &enable_data);
	if (rv) {
		dbg("Unable to convert sensor_id to pointer");
                return SA_ERR_HPI_INVALID_CMD;
        }
        
        rv = ohoi_loop(&enable_data.done, ipmi_handler);

	if (rv || enable_data.rvalue)
		return SA_ERR_HPI_INTERNAL_ERROR;

	return SA_OK;
}
