/*
 * Copyright (c) 2003, Service Availability Forum
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
 * Neither the name of the Service Availalability Forum nor the names 
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


/*******************************************************************************
********************************************************************************
********** 							      **********
********** Resource Presence Table 				      **********
********** 							      **********
********************************************************************************
*******************************************************************************/

#ifndef HPI_RPT_H
#define HPI_RPT_H

/* This section defines the types associated with the RPT. */
typedef struct {
	SaHpiUint32T UpdateCount; /* This count is incremented any time the table
		        	   * is changed. It rolls over to zero when the
				   * maximum value is reached */
	SaHpiTimeT UpdateTimestamp; /* This time is set any time the table is
				     * changed. If the implementation cannot
				     * supply an absolute timestamp, then it may
				     * supply a timestamp relative to some system-
				     * defined epoch, such as system boot. The
				     * value SAHPI_TIME_UNSPECIFIED indicates that
				     * the time of the update cannot be determined.
				     * Otherwise, If the value is less than or
				     * equal to SAHPI_TIME_MAX_RELATIVE, then it
				     * is relative; if it is greater than
				     * SAHPI_TIME_MAX_RELATIVE, then it is absolute. */
} SaHpiRptInfoT;

/*
** Resource Info Type Definitions
**
**
** SaHpiResourceInfoT contains static configuration data concerning the
** management controller associated with the resource, or the resource itself.
** Note this information is used to describe the resource; that is, the piece of
** infrastructure which manages an entity (or multiple entities) - NOT the entities
** for which the resource provides management. The purpose of the
** SaHpiResourceInfoT structure is to provide information that the HPI user may
** need in order to interact correctly with the resource (e.g., recognize a
** specific management controller which may have defined OEM fields in sensors,
** OEM controls, etc.).
**
** All of the fields in the following structure may or may not be used by a
** given resource.
*/
typedef struct {
	SaHpiUint8T ResourceRev;
	SaHpiUint8T SpecificVer;
	SaHpiUint8T DeviceSupport;
	SaHpiManufacturerIdT ManufacturerId;
	SaHpiUint16T ProductId;
	SaHpiUint8T FirmwareMajorRev;
	SaHpiUint8T FirmwareMinorRev;
	SaHpiUint8T AuxFirmwareRev;
} SaHpiResourceInfoT;

/*
** Resource Capabilities
**
** This definition defines the capabilities of a given resource. One resource
** may support any number of capabilities using the bit mask. Every resource
** must set at least one of these capabilities; ie. zero is not a valid value
** for a resource's capabilities.
**
** SAHPI_CAPABILITY_DOMAIN
** SAHPI_CAPABILITY_RESOURCE
** SAHPI_CAPABILITY_EVT_DEASSERTS
** Indicates that all sensors on the resource have the property that their
** Assertion and Deassertion event enable flags are the same. That is,
** for all event states whose assertion triggers an event, it is
** guaranteed that the deassertion of that event will also
** trigger an event. Thus, the user may track the state of sensors on the
** resource by monitoring events rather than polling for state changes.
** SAHPI_CAPABILITY_AGGREGATE_STATUS
** SAHPI_CAPABILITY_CONFIGURATION
** SAHPI_CAPABILITY_MANAGED_HOTSWAP
** Indicates that the resource supports managed hotswap. Since hotswap only
** makes sense for field-replaceable units, the SAHPI_CAPABILITY_FRU
** capability bit must also be set for this resource.
** SAHPI_CAPABILITY_WATCHDOG
** SAHPI_CAPABILITY_CONTROL
** SAHPI_CAPABILITY_FRU
** Indicates that the resource is a field-replaceable unit; i.e., it is
** capable of being removed and replaced in a live system. This does not
** necessarily imply that the resource supports managed hotswap.
** SAHPI_CAPABILITY_INVENTORY_DATA
** SAHPI_CAPABILITY_SEL
** SAHPI_CAPABILITY_RDR
** Indicates that a resource data record (RDR) repository is supplied
** by the resource. Since the existence of an RDR is mandatory, this
** capability must be asserted.
** SAHPI_CAPABILITY_SENSOR
*/
typedef SaHpiUint32T SaHpiCapabilitiesT;
#define SAHPI_CAPABILITY_DOMAIN (SaHpiCapabilitiesT)0x80000000
#define SAHPI_CAPABILITY_RESOURCE (SaHpiCapabilitiesT)0X40000000
#define SAHPI_CAPABILITY_EVT_DEASSERTS (SaHpiCapabilitiesT)0x00008000
#define SAHPI_CAPABILITY_AGGREGATE_STATUS (SaHpiCapabilitiesT)0x00002000
#define SAHPI_CAPABILITY_CONFIGURATION (SaHpiCapabilitiesT)0x00001000
#define SAHPI_CAPABILITY_MANAGED_HOTSWAP (SaHpiCapabilitiesT)0x00000800
#define SAHPI_CAPABILITY_WATCHDOG (SaHpiCapabilitiesT)0x00000400
#define SAHPI_CAPABILITY_CONTROL (SaHpiCapabilitiesT)0x00000200
#define SAHPI_CAPABILITY_FRU (SaHpiCapabilitiesT)0x00000100
#define SAHPI_CAPABILITY_INVENTORY_DATA (SaHpiCapabilitiesT)0x00000008
#define SAHPI_CAPABILITY_SEL (SaHpiCapabilitiesT)0x00000004
#define SAHPI_CAPABILITY_RDR (SaHpiCapabilitiesT)0x00000002
#define SAHPI_CAPABILITY_SENSOR (SaHpiCapabilitiesT)0x00000001
/*
** RPT Entry
**
** This structure is used to store the RPT entry information.
**
** The ResourceCapabilities field definies the capabilities of the resource.
** This field must be non-zero for all valid resources.
**
** The ResourceTag field is an informational value that supplies the caller with naming
** information for the resource. This should be set to the "user-visible" name for a
** resource, which can be used to identify the resource in messages to a human operator.
** For example, it could be set to match a physical printed label attached to the primary
** entity which the resource manages. See section 5.2.6, saHpiResourceTagSet(), on page 33.
*/
typedef struct {
	SaHpiEntryIdT EntryId;
	SaHpiResourceIdT ResourceId;
	SaHpiResourceInfoT ResourceInfo;
	SaHpiEntityPathT ResourceEntity; /* If resource manages a FRU, entity path of the FRU 
					 * If resource manages a single entity, entity path of
					 * that entity. 
					 * If resource manages multiple entities, the
					 * entity path of the "primary" entity managed by the
					 * resource 
					 * Must be set to the same value in every domain which
					 * contains this resource */
	SaHpiCapabilitiesT ResourceCapabilities; /* Must be non-0. */
	SaHpiSeverityT ResourceSeverity; /* Indicates the criticality that
					  * should be raised when the resource
					  * is not responding */
	SaHpiDomainIdT DomainId; /* The Domain ID is used when the resource
				  * is also a domain. */
	SaHpiTextBufferT ResourceTag;
} SaHpiRptEntryT;

#endif /* HPI_RPT_H */

