
/**
 * TODO: License
 */

#ifndef HPI_API_H
#define HPI_API_H

#include "hpi_types.h"
#include "hpi_events.h"
#include "hpi_sensors.h"
#include "hpi_hotswap.h"
#include "hpi_watchdog.h"
#include "hpi_events2.h"
#include "hpi_controls.h"
#include "hpi_ctrlrdr.h"
#include "hpi_eid.h"
#include "hpi_entities.h"
#include "hpi_invrdr.h"
#include "hpi_paramctrl.h"
#include "hpi_sensorrdr.h"
#include "hpi_watchdogrdr.h"
#include "hpi_rdr.h"
#include "hpi_reset.h"
#include "hpi_rpt.h"
#include "hpi_sel.h"
#include "hpi_status.h"

/**
 * This function allows the management service an opportunity to 
 * perform platform-specific initialization. It must be 
 * called before any other functions are used 
 *
 * SA_OK returned on successful completion
 * SA_ERR_HPI_DUPLICATE if HPI was already initialized
 */
SaErrorT SAHPI_API saHpiInitialize(SAHPI_OUT SaHpiVersionT *HpiImplVersion);

/**
 * This function allows the management service an opportunity
 * to perform platform-specific cleanup. All sessions should
 * be closed before this function is called.
 *
 * SA_OK returned if successful
 */
SaErrorT SAHPI_API saHpiFinalize(void);

/**
 * This function opens a HPI connection for a given domain.
 *
 * DomainId - SAHPI_DEFAULT_DOMAIN_ID indicates the default domain
 * SessionId - A pointer to a location to store a handle to the
 * 	newly opened session.
 * SecurityParams - Reserved for future use. Set to NULL.
 *
 * SA_OK returned on successful completion
 * SA_ERR_HPI_INVALID_DOMAIN returned if no domain matching the 
 * 	specified domain ID exists.
 */
SaErrorT SAHPI_API saHpiSessionOpen(
		SAHPI_IN SaHpiDomainIdT DomainId,
		SAHPI_OUT SaHpiSessionIdT *SessionId,
		SAHPI_IN void *SecurityParams);


/**
 * This function closes a HPI session.
 *
 * SessionId - The session handle previously obtained using 
 * 	saHpiSessionOpen()
 *
 * SA_OK returned on successful completion
 */
SaErrorT SAHPI_API saHpiSessionClose(SAHPI_IN SaHpiSessionIdT SessionId);


/**
 * This function requests the underlying management service to
 * discover information about the resources it controls
 * for the domain associated with the open session.  This 
 * function may be called during operation to regenerate the
 * RPT table.
 *
 * SessionId - handle to session context
 *
 * SA_OK returned upon successful completion
 */
SaErrorT SAHPI_API saHpiResourcesDiscover(SAHPI_IN SaHpiSessionIdT SessionId);

/**
 * This function is used for requesting information about the
 * resource presence table (RPT) such as an
 * update counter and timestamp.
 *
 * SessionId - Handle to session context
 * RptInfo - Pointer to the information describing the RPT
 *
 * SA_OK returned on successful completion
 */
SaErrorT SAHPI_API saHpiRptInfoGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiRptInfoT *RptInfo);

/**
 * This function retrieves resource information for the specified
 * entry of the resource presence table. It allows the
 * caller to read the RPT entry-by-entry.
 *
 * SessionId - Handle to session context
 * EntryId - Handle of the entry to retrieve from the RPT.
 * NextEntryId - Pointer to location to store the record ID of next entry
 * 	in RPT
 * RptEntry - pointer to the structure to hold the returned RPT entry
 *
 * SA_OK returned on successful completion
 */
SaErrorT SAHPI_API saHpiRptEntryGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiEntryIdT EntryId,
		SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiRptEntryT *RptEntry);

/**
 * This function retrieves resource information from the RPT
 * for the specified resource using its resource ID.
 *
 * SessionId - handle to session context
 * ResourceId - Resource ID of the resource whose RPT entry should
 * 	be returned
 * RptEntry - Pointer to structure to hold the returned RPT entry.
 *
 * SA_OK returned on successful completion
 */
SaErrorT SAHPI_API saHpiEntryGetByResourceId(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiRptEntryT *RptEntry);

/**
 * This function allows the caller to set the severity level
 * applied to an event issued if a resouce unexpectedly
 * becomes unavailable to the HPI.
 *
 * SessionId - Handle to session context
 * ResourceId - Resource ID of the resource for which the severity level 
 * 	will be read.
 * Severity - Severity level of event issued when the resource 
 * 	unexpectedly becomes unavailable to the HPI.
 *
 * SA_OK returned on successful completion.
 */
SaErrorT SAHPI_API saHpiResourceSeveritySet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSeverityT Severity);

/**
 * This function allows the caller to set the resource tag
 * for a particular resource.
 *
 * SessionId - Handle to session context
 * ResourceId - Resource ID of the resource for which the
 *  	resouce tag should be set.
 * ResourceTag - Pointer to string representing the resource tag.
 *
 * SA_OK returned on successful completion.
 */
SaErrorT SAHPI_API saHpiResourceTagSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTextBufferT *ResourceTag);


/**
 * This function returns the resource ID of the resource associated
 * with the entity upon which the caller is running.
 *
 * SessionId - The handle to session context
 * ResourceId - Pointer to location to hold the returned resource ID
 *
 * SA_OK returned upon successful completion
 * SA_ERR_HPI_NOT_PRESENT returned if the entity the caller is running 
 * 	on is not manageable in the addressed domain.
 * SA_ERR_HPI_UNKNOWN returned if the domain controller cannot
 * 	determine an appropriate response.
 */

SaErrorT SAHPI_API saHpiResourceIdGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiResourceIdT *ResourceId);

/**
 * This function returns the identifier of the Entity Schema for the
 * HPI implementation
 *
 * SessionId - handle to session context
 * SchemaId - Pointer to the ID of teh schma in use; zero 
 * 	indicates that a custom schema is in use.
 *
 * SA_OK returned on successful completion.
 */
SaErrorT SAHPI_API saHpiEntitySchemaGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiUint32T *SchemaId);


SaErrorT SAHPI_API saHpiEventLogInfoGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiSelInfoT *Info);

SaErrorT SAHPI_API saHpiEventLogEntryGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId,
		SAHPI_OUT SaHpiSelEntryIdT *PrevEntryId,
		SAHPI_OUT SaHpiSelEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiSelEntryT *EventLogEntry,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry);

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryT *EvtEntry);

SaErrorT SAHPI_API saHpiEventLogEntryDelete (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId);

SaErrorT SAHPI_API saHpiEventLogClear (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId);

SaErrorT SAHPI_API saHpiEventLogTimeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeT *Time);

SaErrorT SAHPI_API saHpiEventLogTimeSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeT Time);

SaErrorT SAHPI_API saHpiEventLogStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiBoolT *Enable);

SaErrorT SAHPI_API saHpiEventLogStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiBoolT Enable);

SaErrorT SAHPI_API saHpiSubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiBoolT ProvideActiveAlarms);


SaErrorT SAHPI_API saHpiUnsubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId);

SaErrorT SAHPI_API saHpiEventGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout,
		SAHPI_OUT SaHpiEventT *Event,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry);

/*****************************/
SaErrorT SAHPI_API saHpiRdrGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEntryIdT EntryId,
		SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiRdrT *Rdr);

SaErrorT SAHPI_API saHpiSensorReadingGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorReadingT *Reading);

SaErrorT SAHPI_API saHpiSensorReadingConvert (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorReadingT *ReadingInput,
		SAHPI_OUT SaHpiSensorReadingT *ConvertedReading);

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds);

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds);

SaErrorT SAHPI_API saHpiSensorTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorTypeT *Type,
		SAHPI_OUT SaHpiEventCategoryT *Category);

SaErrorT SAHPI_API saHpiSensorEventEnablesGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorEvtEnablesT *Enables);

SaErrorT SAHPI_API saHpiSensorEventEnablesSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorEvtEnablesT *Enables);

SaErrorT SAHPI_API saHpiControlTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_OUT SaHpiCtrlTypeT *Type);

SaErrorT SAHPI_API saHpiControlStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_INOUT SaHpiCtrlStateT *CtrlState);

SaErrorT SAHPI_API saHpiControlStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_IN SaHpiCtrlStateT *CtrlState);

SaErrorT SAHPI_API saHpiEntityInventoryDataRead (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiUint32T BufferSize,
		SAHPI_OUT SaHpiInventoryDataT *InventData,
		SAHPI_OUT SaHpiUint32T *ActualSize);

SaErrorT SAHPI_API saHpiEntityInventoryDataWrite (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiInventoryDataT *InventData);

/***************************************/

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_OUT SaHpiWatchdogT *Watchdog);

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_IN SaHpiWatchdogT *Watchdog);

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum);

/**********************/

SaErrorT SAHPI_API saHpiHotSwapControlRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId);

SaErrorT SAHPI_API saHpiResourceActiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId);

SaErrorT SAHPI_API saHpiResourceInactiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId);

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiTimeoutT *Timeout);

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout);

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeoutT *Timeout);

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeoutT Timeout);

SaErrorT SAHPI_API saHpiHotSwapStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsStateT *State);

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsActionT Action);

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsPowerStateT *State);

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsPowerStateT State);

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsIndicatorStateT *State);

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsIndicatorStateT State);

/**********************************/

SaErrorT SAHPI_API saHpiParmControl (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiParmActionT Action);

SaErrorT SAHPI_API saHpiResourceResetStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiResetActionT *ResetAction);

SaErrorT SAHPI_API saHpiResourceResetStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiResetActionT ResetAction);

#endif /* HPI_H */


