/* Temp file to contains until we port them over or develop them */

#include <snmp_bc_plugin.h>

#if 0
SaErrorT snmp_bc_get_sensor_reading(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiSensorNumT num,
				 SaHpiSensorReadingT *data, 
				 SaHpiEventStateT    *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}


SaErrorT snmp_bc_get_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       SaHpiSensorThresholdsT *thres)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_sensor_thresholds(void *hnd,
				       SaHpiResourceIdT id,
				       SaHpiSensorNumT num,
				       const SaHpiSensorThresholdsT *thres)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

#endif

SaErrorT snmp_bc_get_sensor_event_enables(void *hnd,
					  SaHpiResourceIdT id,
					  SaHpiSensorNumT num,
					  SaHpiBoolT *enables)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_sensor_event_enables(void *hnd,
					  SaHpiResourceIdT id,
					  SaHpiSensorNumT num,
					  const SaHpiBoolT *enables)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}


#if 0
SaErrorT snmp_bc_get_control_state(void *hnd, 
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlModeT *mode,
				   SaHpiCtrlStateT *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}


SaErrorT snmp_bc_set_control_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiCtrlNumT num,
				   SaHpiCtrlModeT *mode,
				   SaHpiCtrlStateT *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}
#endif

#if 0
SaErrorT snmp_bc_get_watchdog_info(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_watchdog_info(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiWatchdogNumT num,
				   SaHpiWatchdogT *wdt)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}
#endif

SaErrorT snmp_bc_reset_watchdog(void *hnd,
				SaHpiResourceIdT id,
				SaHpiWatchdogNumT num)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_get_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_hotswap_state(void *hnd,
				   SaHpiResourceIdT id,
				   SaHpiHsStateT state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_request_hotswap_action(void *hnd,
					SaHpiResourceIdT id,
					SaHpiHsActionT act)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_get_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT *act)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_reset_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiResetActionT act)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_get_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiPowerStateT *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_power_state(void *hnd,
				 SaHpiResourceIdT id,
				 SaHpiPowerStateT state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_get_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT *state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

SaErrorT snmp_bc_set_indicator_state(void *hnd,
				     SaHpiResourceIdT id,
				     SaHpiHsIndicatorStateT state)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

