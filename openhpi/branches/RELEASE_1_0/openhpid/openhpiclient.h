/*
 * hpi library
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

#ifndef dOpenHpiClient_h
#define dOpenHpiClient_h


#include <SaHpi.h>
#include "config.h"
#include "simple_config.h"
#include "connection.h"
#include <sys/time.h>
#include <pthread.h>


// 30s timeout for messages
#define dDefaultMessageTimeout 30000

// 20 outstanding messages before block
#define dDefaultMaxOutstanding 20

struct sOpenHpiClientRequest;
typedef struct sOpenHpiClientRequest cOpenHpiClientRequest;


struct sOpenHpiClientRequest
{
  cOpenHpiClientRequest *m_next;
  unsigned char   m_seq;
  SaErrorT        m_error;

  cMessageHeader *m_request_header;
  void           *m_request;

  cMessageHeader *m_reply_header;
  void          **m_reply;

  struct timeval  m_timeout;

  pthread_mutex_t *m_lock;
  pthread_cond_t  *m_cond;
};


typedef enum
{
  eOpenHpiClientThreadStateUnknown,
  eOpenHpiClientThreadStateSuspend,
  eOpenHpiClientThreadStateRun,
  eOpenHpiClientThreadStateExit
} tOpenHpiClientThreadState;


typedef struct
{
  char                   m_server[dConfigStringMaxLength];
  int                    m_port;
  int                    m_debug;

  // maximum outstanding requests
  int                    m_max_outstanding; // must be <= 256
  unsigned               m_timeout;

  int                    m_num_sessions;
  cClientConnection     *m_client_connection;
  int                    m_initialize;

  pthread_mutex_t        m_lock;
  cOpenHpiClientRequest *m_queue;
  cOpenHpiClientRequest *m_outstanding[256];
  int                    m_num_outstanding;

  int                    m_current_seq;

  pthread_t              m_thread;
  tOpenHpiClientThreadState m_thread_state;
  int                    m_thread_exit;

} cOpenHpiClientConf;


// default prefix for function names
#ifndef dOpenHpiClientFunction
#define dOpenHpiClientFunction(name) saHpi##name
#define dOpenHpiClientParam(...) (__VA_ARGS__)
#define dOpenHpiClientWithConfig
#else
SaErrorT SAHPI_API dOpenHpiClientFunction(Initialize)dOpenHpiClientParam( SAHPI_OUT SaHpiVersionT *HpiImplVersion );
SaErrorT SAHPI_API dOpenHpiClientFunction(Finalize)dOpenHpiClientParam();
SaErrorT SAHPI_API dOpenHpiClientFunction(SessionOpen)dOpenHpiClientParam( SAHPI_IN  SaHpiDomainIdT   DomainId,
							SAHPI_OUT SaHpiSessionIdT *SessionId,
							SAHPI_IN  void            *SecurityParams );
SaErrorT SAHPI_API dOpenHpiClientFunction(SessionClose)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourcesDiscover)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId );
SaErrorT SAHPI_API dOpenHpiClientFunction(RptInfoGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
						       SAHPI_OUT SaHpiRptInfoT *RptInfo );
SaErrorT SAHPI_API dOpenHpiClientFunction(RptEntryGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId,
							SAHPI_IN SaHpiEntryIdT EntryId,
							SAHPI_OUT SaHpiEntryIdT *NextEntryId,
							SAHPI_OUT SaHpiRptEntryT *RptEntry );
SaErrorT SAHPI_API dOpenHpiClientFunction(RptEntryGetByResourceId)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
								    SAHPI_IN  SaHpiResourceIdT ResourceId,
								    SAHPI_OUT SaHpiRptEntryT   *RptEntry );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceSeveritySet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
								SAHPI_IN  SaHpiResourceIdT ResourceId,
								SAHPI_IN  SaHpiSeverityT   Severity );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceTagSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT    SessionId,
							   SAHPI_IN  SaHpiResourceIdT   ResourceId,
							   SAHPI_IN  SaHpiTextBufferT   *ResourceTag );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceIdGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
							  SAHPI_OUT SaHpiResourceIdT  *ResourceId );
SaErrorT SAHPI_API dOpenHpiClientFunction(EntitySchemaGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
							    SAHPI_OUT SaHpiUint32T        *SchemaId );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogInfoGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
							    SAHPI_IN  SaHpiResourceIdT ResourceId,
							    SAHPI_OUT SaHpiSelInfoT    *Info );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogEntryGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT     SessionId,
							     SAHPI_IN    SaHpiResourceIdT    ResourceId,
							     SAHPI_IN    SaHpiSelEntryIdT    EntryId,
							     SAHPI_OUT   SaHpiSelEntryIdT    *PrevEntryId,
							     SAHPI_OUT   SaHpiSelEntryIdT    *NextEntryId,
							     SAHPI_OUT   SaHpiSelEntryT      *EventLogEntry,
							     SAHPI_INOUT SaHpiRdrT           *Rdr,
							     SAHPI_INOUT SaHpiRptEntryT      *RptEntry );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogEntryAdd)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT      SessionId,
							     SAHPI_IN SaHpiResourceIdT     ResourceId,
							     SAHPI_IN SaHpiSelEntryT       *EvtEntry );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogEntryDelete)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT      SessionId,
								SAHPI_IN SaHpiResourceIdT     ResourceId,
								SAHPI_IN SaHpiSelEntryIdT     EntryId );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogClear)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
							  SAHPI_IN  SaHpiResourceIdT  ResourceId );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogTimeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
							    SAHPI_IN  SaHpiResourceIdT ResourceId,
							    SAHPI_OUT SaHpiTimeT       *Time );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogTimeSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
							    SAHPI_IN SaHpiResourceIdT  ResourceId,
							    SAHPI_IN SaHpiTimeT        Time );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
							     SAHPI_IN  SaHpiResourceIdT ResourceId,
							     SAHPI_OUT SaHpiBoolT       *Enable );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventLogStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
							     SAHPI_IN SaHpiResourceIdT  ResourceId,
							     SAHPI_IN SaHpiBoolT        Enable );
SaErrorT SAHPI_API dOpenHpiClientFunction(Subscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
						      SAHPI_IN SaHpiBoolT       ProvideActiveAlarms );
SaErrorT SAHPI_API dOpenHpiClientFunction(Unsubscribe)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT SessionId );
SaErrorT SAHPI_API dOpenHpiClientFunction(EventGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT      SessionId,
						     SAHPI_IN    SaHpiTimeoutT        Timeout,
						     SAHPI_OUT   SaHpiEventT          *Event,
						     SAHPI_INOUT SaHpiRdrT            *Rdr,
						     SAHPI_INOUT SaHpiRptEntryT       *RptEntry );
SaErrorT SAHPI_API dOpenHpiClientFunction(RdrGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT       SessionId,
						   SAHPI_IN  SaHpiResourceIdT      ResourceId,
						   SAHPI_IN  SaHpiEntryIdT         EntryId,
						   SAHPI_OUT SaHpiEntryIdT         *NextEntryId,
						   SAHPI_OUT SaHpiRdrT             *Rdr );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorReadingGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
							     SAHPI_IN  SaHpiResourceIdT     ResourceId,
							     SAHPI_IN  SaHpiSensorNumT      SensorNum,
							     SAHPI_OUT SaHpiSensorReadingT  *Reading );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorReadingConvert)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT      SessionId,
								 SAHPI_IN  SaHpiResourceIdT     ResourceId,
								 SAHPI_IN  SaHpiSensorNumT      SensorNum,
								 SAHPI_IN  SaHpiSensorReadingT  *ReadingInput,
								 SAHPI_OUT SaHpiSensorReadingT  *ConvertedReading );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorThresholdsGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT        SessionId,
								SAHPI_IN  SaHpiResourceIdT       ResourceId,
								SAHPI_IN  SaHpiSensorNumT        SensorNum,
								SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorThresholdsSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT        SessionId,
								SAHPI_IN  SaHpiResourceIdT       ResourceId,
								SAHPI_IN  SaHpiSensorNumT        SensorNum,
								SAHPI_IN  SaHpiSensorThresholdsT *SensorThresholds );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
							  SAHPI_IN  SaHpiResourceIdT    ResourceId,
							  SAHPI_IN  SaHpiSensorNumT     SensorNum,
							  SAHPI_OUT SaHpiSensorTypeT    *Type,
							  SAHPI_OUT SaHpiEventCategoryT *Category );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventEnablesGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
								  SAHPI_IN  SaHpiResourceIdT        ResourceId,
								  SAHPI_IN  SaHpiSensorNumT         SensorNum,
								  SAHPI_OUT SaHpiSensorEvtEnablesT  *Enables );
SaErrorT SAHPI_API dOpenHpiClientFunction(SensorEventEnablesSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT        SessionId,
								  SAHPI_IN SaHpiResourceIdT       ResourceId,
								  SAHPI_IN SaHpiSensorNumT        SensorNum,
								  SAHPI_IN SaHpiSensorEvtEnablesT *Enables );
SaErrorT SAHPI_API dOpenHpiClientFunction(ControlTypeGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
							   SAHPI_IN  SaHpiResourceIdT ResourceId,
							   SAHPI_IN  SaHpiCtrlNumT    CtrlNum,
							   SAHPI_OUT SaHpiCtrlTypeT   *Type );
SaErrorT SAHPI_API dOpenHpiClientFunction(ControlStateGet)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT  SessionId,
							    SAHPI_IN    SaHpiResourceIdT ResourceId,
							    SAHPI_IN    SaHpiCtrlNumT    CtrlNum,
							    SAHPI_INOUT SaHpiCtrlStateT  *CtrlState );
SaErrorT SAHPI_API dOpenHpiClientFunction(ControlStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
							    SAHPI_IN SaHpiResourceIdT ResourceId,
							    SAHPI_IN SaHpiCtrlNumT    CtrlNum,
							    SAHPI_IN SaHpiCtrlStateT  *CtrlState );
SaErrorT SAHPI_API dOpenHpiClientFunction(EntityInventoryDataRead)dOpenHpiClientParam( SAHPI_IN    SaHpiSessionIdT         SessionId,
								    SAHPI_IN    SaHpiResourceIdT        ResourceId,
								    SAHPI_IN    SaHpiEirIdT             EirId,
								    SAHPI_IN    SaHpiUint32T            BufferSize,
								    SAHPI_OUT   SaHpiInventoryDataT     *InventData,
								    SAHPI_OUT   SaHpiUint32T            *ActualSize );
SaErrorT SAHPI_API dOpenHpiClientFunction(EntityInventoryDataWrite)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT          SessionId,
								     SAHPI_IN SaHpiResourceIdT         ResourceId,
								     SAHPI_IN SaHpiEirIdT              EirId,
								     SAHPI_IN SaHpiInventoryDataT      *InventData );
SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT    SessionId,
							     SAHPI_IN  SaHpiResourceIdT   ResourceId,
							     SAHPI_IN  SaHpiWatchdogNumT  WatchdogNum,
							     SAHPI_OUT SaHpiWatchdogT     *Watchdog );
SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
							     SAHPI_IN SaHpiResourceIdT   ResourceId,
							     SAHPI_IN SaHpiWatchdogNumT  WatchdogNum,
							     SAHPI_IN SaHpiWatchdogT     *Watchdog );
SaErrorT SAHPI_API dOpenHpiClientFunction(WatchdogTimerReset)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT   SessionId,
							       SAHPI_IN SaHpiResourceIdT  ResourceId,
							       SAHPI_IN SaHpiWatchdogNumT WatchdogNum );
SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapControlRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
								  SAHPI_IN SaHpiResourceIdT ResourceId );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceActiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
							      SAHPI_IN SaHpiResourceIdT ResourceId );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceInactiveSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
								SAHPI_IN SaHpiResourceIdT ResourceId );
SaErrorT SAHPI_API dOpenHpiClientFunction(AutoInsertTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT SessionId,
								 SAHPI_OUT SaHpiTimeoutT    *Timeout );
SaErrorT SAHPI_API dOpenHpiClientFunction(AutoInsertTimeoutSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
								 SAHPI_IN SaHpiTimeoutT    Timeout );
SaErrorT SAHPI_API dOpenHpiClientFunction(AutoExtractTimeoutGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
								  SAHPI_IN  SaHpiResourceIdT  ResourceId,
								  SAHPI_OUT SaHpiTimeoutT     *Timeout );
SaErrorT SAHPI_API dOpenHpiClientFunction(AutoExtractTimeoutSet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT   SessionId,
								  SAHPI_IN  SaHpiResourceIdT  ResourceId,
								  SAHPI_IN  SaHpiTimeoutT     Timeout );
SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT  SessionId,
							    SAHPI_IN  SaHpiResourceIdT ResourceId,
							    SAHPI_OUT SaHpiHsStateT    *State );
SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapActionRequest)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
								 SAHPI_IN SaHpiResourceIdT ResourceId,
								 SAHPI_IN SaHpiHsActionT   Action );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourcePowerStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT     SessionId,
								  SAHPI_IN  SaHpiResourceIdT    ResourceId,
								  SAHPI_OUT SaHpiHsPowerStateT  *State );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourcePowerStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT     SessionId,
								  SAHPI_IN SaHpiResourceIdT    ResourceId,
								  SAHPI_IN SaHpiHsPowerStateT  State );
SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapIndicatorStateGet)dOpenHpiClientParam( SAHPI_IN  SaHpiSessionIdT         SessionId,
								     SAHPI_IN  SaHpiResourceIdT        ResourceId,
								     SAHPI_OUT SaHpiHsIndicatorStateT  *State );
SaErrorT SAHPI_API dOpenHpiClientFunction(HotSwapIndicatorStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT         SessionId,
								     SAHPI_IN SaHpiResourceIdT        ResourceId,
								     SAHPI_IN SaHpiHsIndicatorStateT  State );
SaErrorT SAHPI_API dOpenHpiClientFunction(ParmControl)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
							SAHPI_IN SaHpiResourceIdT ResourceId,
							SAHPI_IN SaHpiParmActionT Action );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceResetStateGet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT    SessionId,
								  SAHPI_IN SaHpiResourceIdT   ResourceId,
								  SAHPI_OUT SaHpiResetActionT *ResetAction );
SaErrorT SAHPI_API dOpenHpiClientFunction(ResourceResetStateSet)dOpenHpiClientParam( SAHPI_IN SaHpiSessionIdT  SessionId,
								  SAHPI_IN SaHpiResourceIdT ResourceId,
								  SAHPI_IN SaHpiResetActionT ResetAction );

#endif


#endif
