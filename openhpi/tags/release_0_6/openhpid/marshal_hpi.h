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

#ifndef dMarshalHpi_h
#define dMarshalHpi_h


#ifndef dMarshalHpiTypes_h
#include "marshal_hpi_types.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define dDefaultDaemonPort 4743


// inventory functions are missing, because
// it is quite diffenent handled in hpi 1.1
typedef enum
{
  eFsaHpiNull,
  eFsaHpiSessionOpen,
  eFsaHpiSessionClose,
  eFsaHpiResourcesDiscover,
  eFsaHpiRptInfoGet,
  eFsaHpiRptEntryGet,
  eFsaHpiRptEntryGetByResourceId,
  eFsaHpiResourceSeveritySet,
  eFsaHpiResourceTagSet,
  eFsaHpiResourceIdGet,
  eFsaHpiEntitySchemaGet,
  eFsaHpiEventLogInfoGet,
  eFsaHpiEventLogEntryGet,
  eFsaHpiEventLogEntryAdd,
  eFsaHpiEventLogEntryDelete,
  eFsaHpiEventLogClear,
  eFsaHpiEventLogTimeGet,
  eFsaHpiEventLogTimeSet,
  eFsaHpiEventLogStateGet,
  eFsaHpiEventLogStateSet,
  eFsaHpiSubscribe,
  eFsaHpiUnsubscribe,
  eFsaHpiEventGet,
  eFsaHpiRdrGet,
  eFsaHpiSensorReadingGet,
  eFsaHpiSensorReadingConvert,
  eFsaHpiSensorThresholdsGet,
  eFsaHpiSensorThresholdsSet,
  eFsaHpiSensorTypeGet,
  eFsaHpiSensorEventEnablesGet,
  eFsaHpiSensorEventEnablesSet,
  eFsaHpiControlTypeGet,
  eFsaHpiControlStateGet,
  eFsaHpiControlStateSet,
  eFsaHpiWatchdogTimerGet,
  eFsaHpiWatchdogTimerSet,
  eFsaHpiWatchdogTimerReset,
  eFsaHpiHotSwapControlRequest,
  eFsaHpiResourceActiveSet,
  eFsaHpiResourceInactiveSet,
  eFsaHpiAutoInsertTimeoutGet,
  eFsaHpiAutoInsertTimeoutSet,
  eFsaHpiAutoExtractTimeoutGet,
  eFsaHpiAutoExtractTimeoutSet,
  eFsaHpiHotSwapStateGet,
  eFsaHpiHotSwapActionRequest,
  eFsaHpiResourcePowerStateGet,
  eFsaHpiResourcePowerStateSet,
  eFsaHpiHotSwapIndicatorStateGet,
  eFsaHpiHotSwapIndicatorStateSet,
  eFsaHpiParmControl,
  eFsaHpiResourceResetStateGet,
  eFsaHpiResourceResetStateSet
} tHpiFucntionId;


typedef struct
{
  int                  m_id;
  const cMarshalType **m_request;
  const cMarshalType **m_reply; // the first param is the result
  unsigned int         m_request_len;
  unsigned int         m_reply_len;
} cHpiMarshal;


#define dHpiMarshalEntry(name) \
{                              \
  eF ## name,                  \
  name ## In,                  \
  name ## Out,                 \
  0, 0                         \
}


cHpiMarshal *HpiMarshalFind( int id );

void HpiMarshalRequest ( cHpiMarshal *m, void *buffer, const void **params );
void HpiMarshalRequest1( cHpiMarshal *m, void *buffer, const void *p1 );
void HpiMarshalRequest2( cHpiMarshal *m, void *buffer, const void *p1, const void *p2 );
void HpiMarshalRequest3( cHpiMarshal *m, void *buffer, const void *p1, const void *p2, const void *p3 );
void HpiMarshalRequest4( cHpiMarshal *m, void *buffer, const void *p1, const void *p2, const void *p3, const void *p4 );
void HpiMarshalRequest5( cHpiMarshal *m, void *buffer, const void *p1, const void *p2, const void *p3, const void *p4, const void *p5 );

void HpiDemarshalRequest ( int byte_order, cHpiMarshal *m, const void *buffer, void **params );
void HpiDemarshalRequest1( int byte_order, cHpiMarshal *m, const void *buffer, void *p1 );
void HpiDemarshalRequest2( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2 );
void HpiDemarshalRequest3( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3 );
void HpiDemarshalRequest4( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3, void *p4 );
void HpiDemarshalRequest5( int byte_order, cHpiMarshal *m, const void *buffer, void *p1, void *p2, void *p3, void *p4, void *p5 );

unsigned int HpiMarshalReply ( cHpiMarshal *m, void *buffer, const void **params );
unsigned int HpiMarshalReply0( cHpiMarshal *m, void *buffer, const void *result );
unsigned int HpiMarshalReply1( cHpiMarshal *m, void *buffer, const void *result, const void *p1  );
unsigned int HpiMarshalReply2( cHpiMarshal *m, void *buffer, const void *result, const void *p1, const void *p2 );
unsigned int HpiMarshalReply3( cHpiMarshal *m, void *buffer, const void *result, const void *p1, const void *p2, const void *p3 );
unsigned int HpiMarshalReply4( cHpiMarshal *m, void *buffer, const void *result, const void *p1, const void *p2, const void *p3, const void *p4 );
unsigned int HpiMarshalReply5( cHpiMarshal *m, void *buffer, const void *result, const void *p1, const void *p2, const void *p3, const void *p4, const void *p5 );

void HpiDemarshalReply ( int byte_order, cHpiMarshal *m, const void *buffer, void **params );
void HpiDemarshalReply0( int byte_order, cHpiMarshal *m, const void *buffer, void *result );
void HpiDemarshalReply1( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1 );
void HpiDemarshalReply2( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2 );
void HpiDemarshalReply3( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3 );
void HpiDemarshalReply4( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3, void *p4 );
void HpiDemarshalReply5( int byte_order, cHpiMarshal *m, const void *buffer, void *result, void *p1, void *p2, void *p3, void *p4, void *p5 );

#ifdef __cplusplus
}
#endif

#endif
