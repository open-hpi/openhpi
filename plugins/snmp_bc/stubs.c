/* Temp file to contains until we port them over or develop them */

#include <snmp_bc_plugin.h>


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
					  const SaHpiBoolT enables)
{
  return(SA_ERR_HPI_UNSUPPORTED_API);
}

