/*
 * marshaling/demarshaling of hpi functions
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include "marshal_hpi.h"


static const cMarshalType *saHpiSessionOpenIn[] =
{
  &SaHpiDomainIdType, // domain id (SaHpiDomainIdT)
  0
};


static const cMarshalType *saHpiSessionOpenOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};


static const cMarshalType *saHpiSessionCloseIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};


static const cMarshalType *saHpiSessionCloseOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourcesDiscoverIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};


static const cMarshalType *saHpiResourcesDiscoverOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiRptInfoGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};


static const cMarshalType *saHpiRptInfoGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiRptInfoType,
  0
};


static const cMarshalType *saHpiRptEntryGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiEntryIdType,
  0
};

static const cMarshalType *saHpiRptEntryGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiEntryIdType,
  &SaHpiRptEntryType,
  0
};


static const cMarshalType *saHpiRptEntryGetByResourceIdIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiRptEntryGetByResourceIdOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiRptEntryType,
  0
};


static const cMarshalType *saHpiResourceSeveritySetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSeverityType,
  0
};

static const cMarshalType *saHpiResourceSeveritySetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourceTagSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiTextBufferType,
  0
};

static const cMarshalType *saHpiResourceTagSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourceIdGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};

static const cMarshalType *saHpiResourceIdGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiResourceIdType,
  0
};


static const cMarshalType *saHpiEntitySchemaGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};

static const cMarshalType *saHpiEntitySchemaGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiUint32Type,
  0
};


static const cMarshalType *saHpiEventLogInfoGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiEventLogInfoGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSelInfoType,
  0
};


static const cMarshalType *saHpiEventLogEntryGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSelEntryIdType,
  0
};

static const cMarshalType *saHpiEventLogEntryGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSelEntryIdType,
  &SaHpiSelEntryIdType,
  &SaHpiSelEntryType,
  &SaHpiRdrType,
  &SaHpiRptEntryType,
  0
};


static const cMarshalType *saHpiEventLogEntryAddIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSelEntryType,
  0
};

static const cMarshalType *saHpiEventLogEntryAddOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEventLogEntryDeleteIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSelEntryIdType,
  0
};

static const cMarshalType *saHpiEventLogEntryDeleteOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEventLogClearIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiEventLogClearOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEventLogTimeGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiEventLogTimeGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiTimeType,
  0
};


static const cMarshalType *saHpiEventLogTimeSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiTimeType,
  0
};

static const cMarshalType *saHpiEventLogTimeSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEventLogStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiEventLogStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiBoolType,
  0
};


static const cMarshalType *saHpiEventLogStateSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiBoolType,
  0
};

static const cMarshalType *saHpiEventLogStateSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiSubscribeIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiBoolType,
  0
};

static const cMarshalType *saHpiSubscribeOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiUnsubscribeIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};

static const cMarshalType *saHpiUnsubscribeOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEventGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiTimeoutType,
  0
};

static const cMarshalType *saHpiEventGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiEventType,
  &SaHpiRdrType,
  &SaHpiRptEntryType,
  0
};


static const cMarshalType *saHpiRdrGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiEntryIdType,
  0
};

static const cMarshalType *saHpiRdrGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiEntryIdType,
  &SaHpiRdrType,
  0
};


static const cMarshalType *saHpiSensorReadingGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  0
};

static const cMarshalType *saHpiSensorReadingGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSensorReadingType,
  0
};


static const cMarshalType *saHpiSensorReadingConvertIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  &SaHpiSensorReadingType,
  0
};

static const cMarshalType *saHpiSensorReadingConvertOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSensorReadingType,
  0
};


static const cMarshalType *saHpiSensorThresholdsGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  0
};

static const cMarshalType *saHpiSensorThresholdsGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSensorThresholdsType,
  0
};


static const cMarshalType *saHpiSensorThresholdsSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  &SaHpiSensorThresholdsType,
  0
};

static const cMarshalType *saHpiSensorThresholdsSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiSensorTypeGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  0
};

static const cMarshalType *saHpiSensorTypeGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSensorTypeType,
  &SaHpiEventCategoryType,
  0
};


static const cMarshalType *saHpiSensorEventEnablesGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  0
};

static const cMarshalType *saHpiSensorEventEnablesGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiSensorEvtEnablesType,
  0
};


static const cMarshalType *saHpiSensorEventEnablesSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiSensorNumType,
  &SaHpiSensorEvtEnablesType,
  0
};

static const cMarshalType *saHpiSensorEventEnablesSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiControlTypeGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiCtrlNumType,
  0
};

static const cMarshalType *saHpiControlTypeGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiCtrlTypeType,
  0
};


static const cMarshalType *saHpiControlStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiCtrlNumType,
  0
};

static const cMarshalType *saHpiControlStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiCtrlStateType,
  0
};


static const cMarshalType *saHpiControlStateSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiCtrlNumType,
  &SaHpiCtrlStateType,
  0
};

static const cMarshalType *saHpiControlStateSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiEntityInventoryDataReadIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiEirIdType,
  &SaHpiUint32Type,
  0
};

static const cMarshalType *saHpiEntityInventoryDataReadOut[] =
{
  &SaErrorType,            // result (SaErrorT)
  &SaHpiUint32Type,        // actual size
  &SaHpiInventoryDataType, // inventory data
  0
};


static const cMarshalType *saHpiEntityInventoryDataWriteIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiEirIdType,
  &SaHpiInventoryDataType,
  0
};

static const cMarshalType *saHpiEntityInventoryDataWriteOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiWatchdogTimerGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiWatchdogNumType,
  0
};

static const cMarshalType *saHpiWatchdogTimerGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiWatchdogType,
  0
};


static const cMarshalType *saHpiWatchdogTimerSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiWatchdogNumType,
  &SaHpiWatchdogType,
  0
};

static const cMarshalType *saHpiWatchdogTimerSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiWatchdogTimerResetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiWatchdogNumType,
  0
};

static const cMarshalType *saHpiWatchdogTimerResetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiHotSwapControlRequestIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiHotSwapControlRequestOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourceActiveSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiResourceActiveSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourceInactiveSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiResourceInactiveSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiAutoInsertTimeoutGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  0
};

static const cMarshalType *saHpiAutoInsertTimeoutGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiTimeoutType,
  0
};


static const cMarshalType *saHpiAutoInsertTimeoutSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiTimeoutType,
  0
};

static const cMarshalType *saHpiAutoInsertTimeoutSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiAutoExtractTimeoutGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiAutoExtractTimeoutGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiTimeoutType,
  0
};


static const cMarshalType *saHpiAutoExtractTimeoutSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiTimeoutType,
  0
};

static const cMarshalType *saHpiAutoExtractTimeoutSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiHotSwapStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiHotSwapStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiHsStateType,
  0
};


static const cMarshalType *saHpiHotSwapActionRequestIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiHsActionType,
  0
};

static const cMarshalType *saHpiHotSwapActionRequestOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourcePowerStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiResourcePowerStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiHsPowerStateType,
  0
};


static const cMarshalType *saHpiResourcePowerStateSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiHsPowerStateType,
  0
};

static const cMarshalType *saHpiResourcePowerStateSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiHotSwapIndicatorStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,

  0
};

static const cMarshalType *saHpiHotSwapIndicatorStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiHsIndicatorStateType,
  0
};


static const cMarshalType *saHpiHotSwapIndicatorStateSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiHsIndicatorStateType,
  0
};

static const cMarshalType *saHpiHotSwapIndicatorStateSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiParmControlIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiParmActionType,
  0
};

static const cMarshalType *saHpiParmControlOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static const cMarshalType *saHpiResourceResetStateGetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  0
};

static const cMarshalType *saHpiResourceResetStateGetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  &SaHpiResetActionType,
  0
};


static const cMarshalType *saHpiResourceResetStateSetIn[] =
{
  &SaHpiSessionIdType, // session id (SaHpiSessionIdT)
  &SaHpiResourceIdType,
  &SaHpiResetActionType,
  0
};

static const cMarshalType *saHpiResourceResetStateSetOut[] =
{
  &SaErrorType, // result (SaErrorT)
  0
};


static cHpiMarshal hpi_marshal[] = 
{
  dHpiMarshalEntry( saHpiSessionOpen ),
  dHpiMarshalEntry( saHpiSessionClose ),
  dHpiMarshalEntry( saHpiResourcesDiscover ),
  dHpiMarshalEntry( saHpiRptInfoGet ),
  dHpiMarshalEntry( saHpiRptEntryGet ),
  dHpiMarshalEntry( saHpiRptEntryGetByResourceId ),
  dHpiMarshalEntry( saHpiResourceSeveritySet ),
  dHpiMarshalEntry( saHpiResourceTagSet ),
  dHpiMarshalEntry( saHpiResourceIdGet ),
  dHpiMarshalEntry( saHpiEntitySchemaGet ),
  dHpiMarshalEntry( saHpiEventLogInfoGet ),
  dHpiMarshalEntry( saHpiEventLogEntryGet ),
  dHpiMarshalEntry( saHpiEventLogEntryAdd ),
  dHpiMarshalEntry( saHpiEventLogEntryDelete ),
  dHpiMarshalEntry( saHpiEventLogClear ),
  dHpiMarshalEntry( saHpiEventLogTimeGet ),
  dHpiMarshalEntry( saHpiEventLogTimeSet ),
  dHpiMarshalEntry( saHpiEventLogStateGet ),
  dHpiMarshalEntry( saHpiEventLogStateSet ),
  dHpiMarshalEntry( saHpiSubscribe ),
  dHpiMarshalEntry( saHpiUnsubscribe ),
  dHpiMarshalEntry( saHpiEventGet ),
  dHpiMarshalEntry( saHpiRdrGet ),
  dHpiMarshalEntry( saHpiSensorReadingGet ),
  dHpiMarshalEntry( saHpiSensorReadingConvert ),
  dHpiMarshalEntry( saHpiSensorThresholdsGet ),
  dHpiMarshalEntry( saHpiSensorThresholdsSet ),
  dHpiMarshalEntry( saHpiSensorTypeGet ),
  dHpiMarshalEntry( saHpiSensorEventEnablesGet ),
  dHpiMarshalEntry( saHpiSensorEventEnablesSet ),
  dHpiMarshalEntry( saHpiControlTypeGet ),
  dHpiMarshalEntry( saHpiControlStateGet ),
  dHpiMarshalEntry( saHpiControlStateSet ),
  dHpiMarshalEntry( saHpiEntityInventoryDataRead ),
  dHpiMarshalEntry( saHpiEntityInventoryDataWrite ),
  dHpiMarshalEntry( saHpiWatchdogTimerGet ),
  dHpiMarshalEntry( saHpiWatchdogTimerSet ),
  dHpiMarshalEntry( saHpiWatchdogTimerReset ),
  dHpiMarshalEntry( saHpiHotSwapControlRequest ),
  dHpiMarshalEntry( saHpiResourceActiveSet ),
  dHpiMarshalEntry( saHpiResourceInactiveSet ),
  dHpiMarshalEntry( saHpiAutoInsertTimeoutGet ),
  dHpiMarshalEntry( saHpiAutoInsertTimeoutSet ),
  dHpiMarshalEntry( saHpiAutoExtractTimeoutGet ),
  dHpiMarshalEntry( saHpiAutoExtractTimeoutSet ),
  dHpiMarshalEntry( saHpiHotSwapStateGet ),
  dHpiMarshalEntry( saHpiHotSwapActionRequest ),
  dHpiMarshalEntry( saHpiResourcePowerStateGet ),
  dHpiMarshalEntry( saHpiResourcePowerStateSet ),
  dHpiMarshalEntry( saHpiHotSwapIndicatorStateGet ),
  dHpiMarshalEntry( saHpiHotSwapIndicatorStateSet ),
  dHpiMarshalEntry( saHpiParmControl ),
  dHpiMarshalEntry( saHpiResourceResetStateGet ),
  dHpiMarshalEntry( saHpiResourceResetStateSet )
};


static int hpi_marshal_num = sizeof( hpi_marshal ) / sizeof( cHpiMarshal );

static int hpi_marshal_init = 0;


cHpiMarshal *
HpiMarshalFind( int id )
{
  if ( !hpi_marshal_init )
     {
       int i;

       for( i = 0; i < hpi_marshal_num; i++ )
	  {
	    hpi_marshal[i].m_request_len = MarshalSizeArray( hpi_marshal[i].m_request );
	    hpi_marshal[i].m_reply_len   = MarshalSizeArray( hpi_marshal[i].m_reply );
	  }

       hpi_marshal_init = 1;
     }

  id--;

  if ( id < 0 || id >= hpi_marshal_num )
       return 0;

  return &hpi_marshal[id];
}


int
HpiMarshalRequest( cHpiMarshal *m, void *buffer, const void **param )
{
  return MarshalArray( m->m_request, param, buffer );
}


int
HpiMarshalRequest1( cHpiMarshal *m, void *buffer, const void *p1 )
{
  const void *param[1];
  param[0] = p1;

  return HpiMarshalRequest( m, buffer, param );  
}


int
HpiMarshalRequest2( cHpiMarshal *m, void *buffer, const void *p1, const void *p2  )
{
  const void *param[2];
  param[0] = p1;
  param[1] = p2;

  return HpiMarshalRequest( m, buffer, param );  
}


int
HpiMarshalRequest3( cHpiMarshal *m, void *buffer, const void *p1, const void *p2, const void *p3 )
{
  const void *param[3];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;

  return HpiMarshalRequest( m, buffer, param );
}


int
HpiMarshalRequest4( cHpiMarshal *m, void *buffer, const void *p1, const void *p2, 
		    const void *p3, const void *p4 )
{
  const void *param[4];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;
  param[3] = p4;

  return HpiMarshalRequest( m, buffer, param );
}


int
HpiMarshalRequest5( cHpiMarshal *m, void *buffer, const void *p1, const void *p2,
		    const void *p3, const void *p4, const void *p5 )
{
  const void *param[5];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;
  param[3] = p4;
  param[4] = p5;

  return HpiMarshalRequest( m, buffer, param );
}


int
HpiDemarshalRequest( int byte_order, cHpiMarshal *m, const void *buffer, void **params )
{
  return DemarshalArray( byte_order, m->m_request, params, buffer );
}


int
HpiDemarshalRequest1( int byte_order, cHpiMarshal *m, const void *buffer, void *p1 )
{
  void *param[1];
  param[0] = p1;
  
  return HpiDemarshalRequest( byte_order, m, buffer, param );
}


int
HpiDemarshalRequest2( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2 )
{
  void *param[2];
  param[0] = p1;
  param[1] = p2;

  return HpiDemarshalRequest( byte_order, m, buffer, param );
}


int
HpiDemarshalRequest3( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3 )
{
  void *param[3];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;

  return HpiDemarshalRequest( byte_order, m, buffer, param );
}


int
HpiDemarshalRequest4( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3, void *p4 )
{
  void *param[4];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;
  param[3] = p4;

  return HpiDemarshalRequest( byte_order, m, buffer, param );
}


int
HpiDemarshalRequest5( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3, void *p4, void *p5 )
{
  void *param[5];
  param[0] = p1;
  param[1] = p2;
  param[2] = p3;
  param[3] = p4;
  param[4] = p5;

  return HpiDemarshalRequest( byte_order, m, buffer, param );
}


int
HpiMarshalReply( cHpiMarshal *m, void *buffer, const void **params )
{
  // the first value is the result.
  SaErrorT err = *(const SaErrorT *)params[0];

  if ( err == SA_OK )
       return MarshalArray( m->m_reply, params, buffer );

  return Marshal( &SaErrorType, &err, buffer );
}


int
HpiMarshalReply0( cHpiMarshal *m, void *buffer, const void *result )
{
  const void *param[1];
  param[0] = result;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiMarshalReply1( cHpiMarshal *m, void *buffer, const void *result, const void *p1  )
{
  const void *param[2];
  param[0] = result;
  param[1] = p1;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiMarshalReply2( cHpiMarshal *m, void *buffer, const void *result,
		  const void *p1, const void *p2 )
{
  const void *param[3];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiMarshalReply3( cHpiMarshal *m, void *buffer, const void *result, const void *p1,
		  const void *p2, const void *p3 )
{
  const void *param[4];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiMarshalReply4( cHpiMarshal *m, void *buffer, const void *result, const void *p1,
		  const void *p2, const void *p3, const void *p4 )
{
  const void *param[5];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;
  param[4] = p4;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiMarshalReply5( cHpiMarshal *m, void *buffer, const void *result, const void *p1,
		  const void *p2, const void *p3, const void *p4, const void *p5 )
{
  const void *param[6];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;
  param[4] = p4;
  param[5] = p5;

  return HpiMarshalReply( m, buffer, param );
}


int
HpiDemarshalReply( int byte_order, cHpiMarshal *m, const void *buffer, void **params )
{
  // the first value is the error code
  int rv = Demarshal( byte_order, &SaErrorType, params[0], buffer );

  if ( rv < 0 )
       return -1;

  SaErrorT err = *(SaErrorT *)params[0];

  if ( err == SA_OK )
       return DemarshalArray( byte_order, m->m_reply, params, buffer );

  return rv;
}


int
HpiDemarshalReply0( int byte_order, cHpiMarshal *m, const void *buffer, void *result )
{
  void *param[1];
  param[0] = result;

  return HpiDemarshalReply( byte_order, m, buffer, param );
}


int
HpiDemarshalReply1( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1 )
{
  void *param[2];
  param[0] = result;
  param[1] = p1;

  return HpiDemarshalReply( byte_order, m, buffer, param );
}


int
HpiDemarshalReply2( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2 )
{
  void *param[3];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;

  return HpiDemarshalReply( byte_order, m, buffer, param );
}


int
HpiDemarshalReply3( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3 )
{
  void *param[4];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;

  return HpiDemarshalReply( byte_order, m, buffer, param );  
}


int
HpiDemarshalReply4( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3, void *p4 )
{
  void *param[5];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;
  param[4] = p4;

  return HpiDemarshalReply( byte_order, m, buffer, param );  
}


int
HpiDemarshalReply5( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3, void *p4, void *p5 )
{
  void *param[6];
  param[0] = result;
  param[1] = p1;
  param[2] = p2;
  param[3] = p3;
  param[4] = p4;
  param[5] = p5;

  return HpiDemarshalReply( byte_order, m, buffer, param );  
}
