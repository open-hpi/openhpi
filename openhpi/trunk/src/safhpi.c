

/*
 * Copyright (c) 2003, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or 
 * without modification, are permitted provided that the following 
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Intel Corporation nor the names 
 * of its contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


#include <SaHpi.h>

/**
 * This function allows the management service an opportunity to 
 * perform platform-specific initialization. It must be 
 * called before any other functions are used 
 *
 * SA_OK returned on successful completion
 * SA_ERR_HPI_DUPLICATE if HPI was already initialized
 */
SaErrorT SAHPI_API saHpiInitialize(SAHPI_OUT SaHpiVersionT *HpiImplVersion)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

/**
 * This function allows the management service an opportunity
 * to perform platform-specific cleanup. All sessions should
 * be closed before this function is called.
 *
 * SA_OK returned if successful
 */
SaErrorT SAHPI_API saHpiFinalize(void)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_IN void *SecurityParams)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}


/**
 * This function closes a HPI session.
 *
 * SessionId - The session handle previously obtained using 
 * 	saHpiSessionOpen()
 *
 * SA_OK returned on successful completion
 */
SaErrorT SAHPI_API saHpiSessionClose(SAHPI_IN SaHpiSessionIdT SessionId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}


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
SaErrorT SAHPI_API saHpiResourcesDiscover(SAHPI_IN SaHpiSessionIdT SessionId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_OUT SaHpiRptInfoT *RptInfo)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
SaErrorT SAHPI_API saHpiRptEntryGetByResourceId(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiRptEntryT *RptEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_IN SaHpiSeverityT Severity)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_IN SaHpiTextBufferT *ResourceTag)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}


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
		SAHPI_OUT SaHpiResourceIdT *ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

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
		SAHPI_OUT SaHpiUint32T *SchemaId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API saHpiEventLogInfoGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiSelInfoT *Info)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId,
		SAHPI_OUT SaHpiSelEntryIdT *PrevEntryId,
		SAHPI_OUT SaHpiSelEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiSelEntryT *EventLogEntry,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryAdd (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryT *EvtEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogEntryDelete (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSelEntryIdT EntryId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogClear (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogTimeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeT *Time)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogTimeSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeT Time)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiBoolT *Enable)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventLogStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiBoolT Enable)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiBoolT ProvideActiveAlarms)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}


SaErrorT SAHPI_API saHpiUnsubscribe (
		SAHPI_IN SaHpiSessionIdT SessionId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEventGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout,
		SAHPI_OUT SaHpiEventT *Event,
		SAHPI_INOUT SaHpiRdrT *Rdr,
		SAHPI_INOUT SaHpiRptEntryT *RptEntry)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

/*****************************/
SaErrorT SAHPI_API saHpiRdrGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEntryIdT EntryId,
		SAHPI_OUT SaHpiEntryIdT *NextEntryId,
		SAHPI_OUT SaHpiRdrT *Rdr)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorReadingGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorReadingT *Reading)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorReadingConvert (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorReadingT *ReadingInput,
		SAHPI_OUT SaHpiSensorReadingT *ConvertedReading)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorThresholdsGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorThresholdsT *SensorThresholds)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorThresholdsSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorThresholdsT *SensorThresholds)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorTypeT *Type,
		SAHPI_OUT SaHpiEventCategoryT *Category)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorEventEnablesGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_OUT SaHpiSensorEvtEnablesT *Enables)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiSensorEventEnablesSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiSensorNumT SensorNum,
		SAHPI_IN SaHpiSensorEvtEnablesT *Enables)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiControlTypeGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_OUT SaHpiCtrlTypeT *Type)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiControlStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_INOUT SaHpiCtrlStateT *CtrlState)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiControlStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiCtrlNumT CtrlNum,
		SAHPI_IN SaHpiCtrlStateT *CtrlState)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEntityInventoryDataRead (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiUint32T BufferSize,
		SAHPI_OUT SaHpiInventoryDataT *InventData,
		SAHPI_OUT SaHpiUint32T *ActualSize)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiEntityInventoryDataWrite (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiEirIdT EirId,
		SAHPI_IN SaHpiInventoryDataT *InventData)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

/***************************************/

SaErrorT SAHPI_API saHpiWatchdogTimerGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_OUT SaHpiWatchdogT *Watchdog)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiWatchdogTimerSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum,
		SAHPI_IN SaHpiWatchdogT *Watchdog)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiWatchdogTimerReset (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiWatchdogNumT WatchdogNum)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

/**********************/

SaErrorT SAHPI_API saHpiHotSwapControlRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceActiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceInactiveSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_OUT SaHpiTimeoutT *Timeout)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAutoInsertTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiTimeoutT Timeout)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutGet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiTimeoutT *Timeout)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiAutoExtractTimeoutSet(
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiTimeoutT Timeout)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiHotSwapStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsStateT *State)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiHotSwapActionRequest (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsActionT Action)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourcePowerStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsPowerStateT *State)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourcePowerStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsPowerStateT State)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiHsIndicatorStateT *State)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiHotSwapIndicatorStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiHsIndicatorStateT State)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

/**********************************/

SaErrorT SAHPI_API saHpiParmControl (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiParmActionT Action)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceResetStateGet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_OUT SaHpiResetActionT *ResetAction)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}

SaErrorT SAHPI_API saHpiResourceResetStateSet (
		SAHPI_IN SaHpiSessionIdT SessionId,
		SAHPI_IN SaHpiResourceIdT ResourceId,
		SAHPI_IN SaHpiResetActionT ResetAction)
{
	return SA_ERR_HPI_UNSUPPORTED_API;
}





























