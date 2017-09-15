/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
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
 * Neither the name of the Hewlett-Packard Corporation, nor the names
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
 * Author(s)
 *     Bryan Sutula <Bryan.Sutula@hp.com>
 *     Raghavendra PG <raghavendra.pg@hp.com>
 *     Raghavendra MS <raghavendra.ms@hp.com>
 *     Anand S <S.Anand@hp.com>
 */


#include <stdlib.h>
#include <string.h>
#include <oh_error.h>
#define OA_SOAP_CALLS_FILE              /* Defines ENUM strings in this file */
#include "oa_soap_calls.h"
#include "oa_soap_discover.h"


/* Macros used in this file, to simplify common code */
#define SOAP_PARM_CHECK \
        int     ret; \
        if ((con == NULL) || (request == NULL) || (response == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }

#define SOAP_PARM_CHECK_NRQ \
        int     ret; \
        if ((con == NULL) || (response == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }

#define SOAP_PARM_CHECK_NRS \
        if ((con == NULL) || (request == NULL)) { \
                err("NULL parameter"); \
                return -1; \
        }

#define SOAP_ARRAY_REQ \
        byte    *p = NULL; \
        for (p = request->bayArray.array; \
             p - request->bayArray.array < request->bayArray.size; \
             p++) { \
                snprintf(bay_array + strlen(bay_array), sizeof(BAY), BAY, *p); \
        } 

/* Helper functions used by the main OA SOAP calls for code reuse.  They are
 * not intended to be called by users.
 *
 * Please note that the function comment block format has not been followed
 * for these.  The functions are very repetitive, and their parameters are
 * all predefined structures, well-documented in external documents.  Trying
 * to document that here would duplicate that information, leading to future
 * maintainability problems.
 */


/* This tries to locate a string of diagnosticChecksEx notes within the
 * current XML response tree.  If one is found, it's node is returned.  If
 * not, returns NULL.
 */
static xmlNode  *locate_diagnosticChecksEx(xmlNode *node)
{
        xmlNode         *diag;          /* DiagnosticChecksEx node */

        /* First, is there a diagnosticChecksEx node? */
        diag = soap_walk_tree(node, "diagnosticChecksEx");
        if (diag && diag->children) {   /* Real responses have children */
                diag = diag->children;
                if (! diag->properties) /* Skip formatting (empty) nodes */
                        diag = soap_next_node(diag);
        }
        else
                diag = NULL;            /* No children */

        return(diag);
}


/* parse_xsdBoolean - Parses xsdBoolean strings, returning an hpoa_boolean */
static enum hpoa_boolean parse_xsdBoolean(char *str)
{
        if ((! strcmp(str, "true")) ||
            (! strcmp(str, "1")))
                return(HPOA_TRUE);

        return(HPOA_FALSE);
}

/* parse_eventPid - Parses an eventPid response structure */
static void     parse_eventPid(xmlNode *node, struct eventPid *response)
{
        response->pid = atoi(soap_tree_value(node, "pid"));
}

/* parse_bladeInfo - Parses a bladeInfo response structure */
void     parse_bladeInfo(xmlNode *node, struct bladeInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->bladeType =
                soap_enum(bladeType_S, soap_tree_value(node, "bladeType"));
        response->width = atoi(soap_tree_value(node, "width"));
        response->height = atoi(soap_tree_value(node, "height"));
        response->name = soap_tree_value(node, "name");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->serverName = soap_tree_value(node, "serverName");
        response->uuid = soap_tree_value(node, "uuid");
        response->rbsuOsName = soap_tree_value(node, "rbsuOsName");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->romVersion = soap_tree_value(node, "romVersion");
        response->numberOfCpus = atoi(soap_tree_value(node, "numberOfCpus"));
        response->cpus = soap_walk_tree(node, "cpus:bladeCpuInfo");
        response->memory = atoi(soap_tree_value(node, "memory"));
        response->numberOfNics = atoi(soap_tree_value(node, "numberOfNics"));
        response->nics = soap_walk_tree(node, "nics:bladeNicInfo");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
        response->deviceId = atoi(soap_tree_value(node, "deviceId"));
        response->productId = atoi(soap_tree_value(node, "productId"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_bladePortMap- Parses a bladePortMap response structure */
void     parse_bladePortMap(xmlNode *node, struct bladePortMap *response)
{
        response->bladeBayNumber = soap_tree_value(node, "bladeBayNumber");
        response->status =
                soap_enum(portMapStatus_S, soap_tree_value(node, "status"));
        response->bladeSizeType =
		soap_enum(bladeSizeType_S, 
				soap_tree_value(node, "bladeSizeType"));
        response->numberOfMezzes = soap_tree_value(node, "numberOfMezzes");
        response->mezz = soap_walk_tree(node, "mezz");
       response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_bladeMpInfo - Parses a bladeMpInfo response structure */
static void     parse_bladeMpInfo(xmlNode *node, struct bladeMpInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->ipAddress = soap_tree_value(node, "ipAddress");
        response->macAddress = soap_tree_value(node, "macAddress");
        response->dnsName = soap_tree_value(node, "dnsName");
        response->modelName = soap_tree_value(node, "modelName");
        response->fwVersion = soap_tree_value(node, "fwVersion");
        response->remoteConsoleUrl = soap_tree_value(node, "remoteConsoleUrl");
        response->webUrl = soap_tree_value(node, "webUrl");
        response->ircUrl = soap_tree_value(node, "ircUrl");
        response->loginUrl = soap_tree_value(node, "loginUrl");
        response->ircFullUrl = soap_tree_value(node, "ircFullUrl");
        response->remoteSerialUrl = soap_tree_value(node, "remoteSerialUrl");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_diagnosticChecks - Parses a diagnosticChecks response structure */
static void     parse_diagnosticChecks(xmlNode *node,
                                       struct diagnosticChecks *response)
{
        response->internalDataError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "internalDataError"));
        response->managementProcessorError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "managementProcessorError"));
        response->thermalWarning =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "thermalWarning"));
        response->thermalDanger =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "thermalDanger"));
        response->ioConfigurationError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "ioConfigurationError"));
        response->devicePowerRequestError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "devicePowerRequestError"));
        response->insufficientCooling =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "insufficientCooling"));
        response->deviceLocationError =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceLocationError"));
        response->deviceFailure =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceFailure"));
        response->deviceDegraded =
                soap_enum(diagnosticStatus_S,
                          soap_tree_value(node, "deviceDegraded"));
        response->acFailure = soap_enum(diagnosticStatus_S,
                                        soap_tree_value(node, "acFailure"));
        response->i2cBuses = soap_enum(diagnosticStatus_S,
                                       soap_tree_value(node, "i2cBuses"));
        response->redundancy = soap_enum(diagnosticStatus_S,
                                         soap_tree_value(node, "redundancy"));
}

/* parse_syslog - Parses a syslog response structure */
static void     parse_syslog(xmlNode *node, struct syslog *response)
{
        char    *str;

        if ((str = soap_tree_value(node, "bayNumber")))
                response->bayNumber = atoi(str);
        else
                response->bayNumber = -1;

        if ((str = soap_tree_value(node, "syslogStrlen")))
                response->syslogStrlen = atoi(str);
        else
                response->syslogStrlen = -1;

        response->logContents = soap_tree_value(node, "logContents"); /* May be
                                                                       * NULL
                                                                       */
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_rackTopology - Parses a rackTopology response structure */
static void     parse_rackTopology(xmlNode *node,
                                   struct rackTopology *response)
{
        response->ruid = soap_tree_value(node, "ruid"); /* May be NULL */
        response->enclosures = soap_walk_tree(node, "enclosures:enclosure");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_rackTopology2 - Parses rackTopology2 response structure */
static void     parse_rackTopology2(xmlNode *node,
                                    struct rackTopology2 *response)
{
        response->ruid = soap_tree_value(node, "ruid");
        response->enclosures = soap_walk_tree(node, "enclosures:enclosure");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_enclosureInfo - Parses a enclosureInfo response structure */
static void     parse_enclosureInfo(xmlNode *node,
                                    struct enclosureInfo *response)
{
        response->rackName = soap_tree_value(node, "rackName");
        response->enclosureName = soap_tree_value(node, "enclosureName");
        response->hwVersion = soap_tree_value(node, "hwVersion");
        response->bladeBays = atoi(soap_tree_value(node, "bladeBays"));
        response->fanBays = atoi(soap_tree_value(node, "fanBays"));
        response->powerSupplyBays =
                atoi(soap_tree_value(node, "powerSupplyBays"));
        response->thermalSensors =
                atoi(soap_tree_value(node, "thermalSensors"));
        response->interconnectTrayBays =
                atoi(soap_tree_value(node, "interconnectTrayBays"));
        response->oaBays = atoi(soap_tree_value(node, "oaBays"));
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->uuid = soap_tree_value(node, "uuid");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->chassisSparePartNumber =
                soap_tree_value(node, "chassisSparePartNumber");
        response->interposerManufacturer =
                soap_tree_value(node, "interposerManufacturer");
        response->interposerName = soap_tree_value(node, "interposerName");
        response->interposerPartNumber =
                soap_tree_value(node, "interposerPartNumber");
        response->interposerSerialNumber =
                soap_tree_value(node, "interposerSerialNumber");
        response->pduType = soap_tree_value(node, "pduType");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
        response->pduPartNumber = soap_tree_value(node, "pduPartNumber");
        response->pduSparePartNumber =
                soap_tree_value(node, "pduSparePartNumber");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_powerConfigInfo - Parses a getPowerConfigInfo response structure */
static void parse_powerConfigInfo(xmlNode *node, \
                                  struct powerConfigInfo *response)
{
        response->powerCeiling = atoi(soap_tree_value(node, "powerCeiling"));
        response->redundancyMode =
          soap_enum(powerRedundancy_S, soap_tree_value(node, "redundancyMode"));
        response->dynamicPowerSaverEnabled =
          parse_xsdBoolean(soap_tree_value(node, "dynamicPowerSaverEnabled"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_powerCapConfig - Parses a getPowerCapConfig response structure */
static void parse_powerCapConfig(xmlNode *node, struct powerCapConfig *response)
{
        xmlNode *bay_data;
        int i;
        xmlNode *extra_data;
        struct extraDataInfo extra_data_info;

        response->enclosureMinWattageMeasured = atoi(soap_tree_value(node,
                                                "enclosureMinWattageMeasured"));
        response->enclosureMaxWattageMeasured = atoi(soap_tree_value(node,
                                                "enclosureMaxWattageMeasured"));
        response->enclosurePowerCapLowerBound = atoi(soap_tree_value(node,
                                                "enclosurePowerCapLowerBound"));

        /* These limits default to above limit, unless there is extraData */
        response->deratedCircuitCapLowerBound =
          response->enclosurePowerCapLowerBound; 
        response->ratedCircuitCapLowerBound =
          response->enclosurePowerCapLowerBound; 

        response->enclosurePowerCapUpperBound = atoi(soap_tree_value(node,
                                                "enclosurePowerCapUpperBound"));

        /* These limits default to above limit, unless there is extraData */
        response->deratedCircuitCapUpperBound =
          response->enclosurePowerCapUpperBound; 
        response->ratedCircuitCapUpperBound =
          response->enclosurePowerCapUpperBound; 

        response->enclosureHighLine =
          parse_xsdBoolean(soap_tree_value(node, "enclosureHighLine"));
        response->enclosureAcPhaseType =
          atoi(soap_tree_value(node, "enclosureAcPhaseType"));
        response->enclosureEstimatedVoltage =
          atoi(soap_tree_value(node, "enclosureEstimatedVoltage"));
        response->powerCap = atoi(soap_tree_value(node, "powerCap"));
        response->extraData = soap_walk_tree(node, "extraData");

        bay_data = soap_walk_tree(node, "optOutBayArray");
        bay_data = soap_walk_tree(bay_data, "bay");
        i = 0;
        while (bay_data) {
                /* Copy optOutBayArray data for later use:  */
                /* data is either "true" or "false"         */
                strncpy(response->optOutBayArray[i], soap_value(bay_data), 6);
                response->optOutBayArray[i][6] = '\0';
                bay_data = soap_next_node(bay_data);
                i++;
        }

        response->deratedCircuitCap = 0;
        response->ratedCircuitCap = 0;
        extra_data = response->extraData;
        while (extra_data) {
                soap_getExtraData(extra_data, &extra_data_info);
                if (!(strcmp(extra_data_info.name, "deratedCircuit"))) {
                  response->deratedCircuitCap = atoi(extra_data_info.value);
                } else if (!(strcmp(extra_data_info.name, "ratedCircuit"))) {
                  response->ratedCircuitCap = atoi(extra_data_info.value);
                } else if (!(strcmp(extra_data_info.name,
                      "deratedCircuitLowerBound"))) {
                  response->deratedCircuitCapLowerBound
                    = atoi(extra_data_info.value);
                } else if (!(strcmp(extra_data_info.name,
                             "deratedCircuitUpperBound"))) {
                  response->deratedCircuitCapUpperBound
                    = atoi(extra_data_info.value);
                } else if (!(strcmp(extra_data_info.name,
                             "ratedCircuitLowerBound"))) {
                  response->ratedCircuitCapLowerBound
                    = atoi(extra_data_info.value);
                } else if (!(strcmp(extra_data_info.name,
                             "ratedCircuitUpperBound"))) {
                  response->ratedCircuitCapUpperBound
                    = atoi(extra_data_info.value);
                }
                extra_data = soap_next_node(extra_data);
        }

}

/* parse_oaStatus - Parses an oaStatus response structure */
void     parse_oaStatus(xmlNode *node, struct oaStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->oaName = soap_tree_value(node, "oaName");
        response->oaRole = soap_enum(oaRole_S, soap_tree_value(node, "oaRole"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->restartCause = atoi(soap_tree_value(node, "restartCause"));
        response->oaRedundancy =
                parse_xsdBoolean(soap_tree_value(node, "oaRedundancy"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_oaInfo - Parses an oaInfo response structure */
void     parse_oaInfo(xmlNode *node, struct oaInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->youAreHere =
                parse_xsdBoolean(soap_tree_value(node, "youAreHere"));
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->uuid = soap_tree_value(node, "uuid");
        response->assetTag = soap_tree_value(node, "assetTag");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->hwVersion = soap_tree_value(node, "hwVersion");
        response->fwVersion = soap_tree_value(node, "fwVersion");
        response->mmHeight = atoi(soap_tree_value(node, "mmHeight"));
        response->mmWidth = atoi(soap_tree_value(node, "mmWidth"));
        response->mmDepth = atoi(soap_tree_value(node, "mmDepth"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_oaId - Parses an oaId response structure */
static void     parse_oaId(xmlNode *node, struct OaId *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "oaId")); 
}

/* parse_bladeStatus - Parses a bladeStatus response structure */
void     parse_bladeStatus(xmlNode *node, struct bladeStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->thermal =
                soap_enum(sensorStatus_S, soap_tree_value(node, "thermal"));
        response->powered =
                soap_enum(power_S, soap_tree_value(node, "powered"));
        response->powerState =
                soap_enum(powerState_S, soap_tree_value(node, "powerState"));
        response->shutdown =
                soap_enum(shutdown_S, soap_tree_value(node, "shutdown"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_interconnectTrayStatus - Parses an interconnectTrayStatus
 * response structure
 */
void     parse_interconnectTrayStatus(xmlNode *node,
                                struct interconnectTrayStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->thermal =
                soap_enum(sensorStatus_S, soap_tree_value(node, "thermal"));
        response->cpuFault =
                parse_xsdBoolean(soap_tree_value(node, "cpuFault"));
        response->healthLed =
                parse_xsdBoolean(soap_tree_value(node, "healthLed"));
        response->uid = soap_enum(uidStatus_S, soap_tree_value(node, "uid"));
        response->powered =
                soap_enum(power_S, soap_tree_value(node, "powered"));
        response->ports = soap_walk_tree(node, "ports:port");
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_interconnectTrayInfo - Parses an interconnectTrayInfo
 * response structure
 */
void     parse_interconnectTrayInfo(xmlNode *node,
                                struct interconnectTrayInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->interconnectTrayType =
                soap_enum(interconnectTrayType_S,
                          soap_tree_value(node, "interconnectTrayType"));
        response->passThroughSupport =
                parse_xsdBoolean(soap_tree_value(node, "passThroughSupport"));
        response->portDisableSupport =
                parse_xsdBoolean(soap_tree_value(node, "portDisableSupport"));
        response->temperatureSensorSupport =
                parse_xsdBoolean(soap_tree_value(node,
                                                 "temperatureSensorSupport"));
        response->width = atoi(soap_tree_value(node, "width"));
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->serialNumber = soap_tree_value(node, "serialNumber");
        response->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        response->rs232PortRoute =
                parse_xsdBoolean(soap_tree_value(node, "rs232PortRoute"));
        response->ethernetPortRoute =
                parse_xsdBoolean(soap_tree_value(node, "ethernetPortRoute"));
        response->userAssignedName = soap_tree_value(node, "userAssignedName");
        response->inBandIpAddress = soap_tree_value(node, "inBandIpAddress");
        response->urlToMgmt = soap_tree_value(node, "urlToMgmt");
        response->powerOnWatts = atoi(soap_tree_value(node, "powerOnWatts"));
        response->powerOffWatts = atoi(soap_tree_value(node, "powerOffWatts"));
        response->extraData = soap_walk_tree(node, "extraData");
}
/* parse_interconnectTrayPortMap- parses an interconnectTrayPortMap response 
 * struture.
 */
void    parse_interconnectTrayPortMap(xmlNode *portmap,
					struct interconnectTrayPortMap *result)
{
	result->interconnectTrayBayNumber =
		atoi(soap_tree_value(portmap, "interconnectTrayBayNumber"));
	result->status = soap_enum(portMapStatus_S,
			soap_tree_value(portmap, "status"));
	result->sizeType = soap_enum(interconnectTraySizeType_S,
			soap_tree_value(portmap, "sizeType"));
	result->passThroughModeEnabled = 
		soap_enum(interconnectTrayPassThroughEnabled_S,
				soap_tree_value(portmap, 
					"passThroughModeEnabled"));
	result->slot = soap_walk_tree(portmap, "slot:interconnectTrayPortMap");
	result->numberOfSlots = atoi(soap_tree_value(portmap, "numberOfSlots"));
	result->extraData = soap_walk_tree(portmap, "extraData");
}

/* parse_powerSupplyInfo - Parses a powerSupplyInfo response structure */
void     parse_powerSupplyInfo(xmlNode *node,
                                      struct powerSupplyInfo *response)
{
	char *temp_val=NULL;
	int len=0;
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));

	temp_val=soap_tree_value(node, "modelNumber");
	if (temp_val != NULL && (len = strlen(temp_val) < 
                                 MAX_MODEL_NUM_LENGTH)) { 
		strcpy(response->modelNumber, temp_val);
	}
	else {
		dbg("Internal Error: Power Supply modelNumber does not exist \
or too long");
                response->modelNumber[0] = '\0';
        }

	temp_val=soap_tree_value(node, "sparePartNumber");
	if (temp_val != NULL && (len = strlen(temp_val)
                                 < MAX_PART_NUM_LENGTH)) {
		strcpy(response->sparePartNumber, temp_val);
	}
	else {
		dbg("Internal Error: Power Supply modelNumber does not exist \
or too long");
                response->sparePartNumber[0] = '\0';
        }

	temp_val=soap_tree_value(node, "serialNumber");
	if (temp_val != NULL && (len = strlen(temp_val)
                                 < MAX_SERIAL_NUM_LENGTH)) {
		strcpy(response->serialNumber, temp_val);
	}
	else {
		dbg("Internal Error: Power Supply modelNumber does not exist \
or too long");
                response->serialNumber[0] = '\0';
        }

        response->capacity = atoi(soap_tree_value(node, "capacity"));
        response->actualOutput = atoi(soap_tree_value(node, "actualOutput"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_powerSupplyStatus - Parses a powerSupplyStatus response structure */
void     parse_powerSupplyStatus(xmlNode *node,
                                        struct powerSupplyStatus *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->inputStatus =
                soap_enum(opStatus_S, soap_tree_value(node, "inputStatus"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_powerSubsystemInfo - Parses a powerSubsystemInfo response structure */
static void     parse_powerSubsystemInfo(xmlNode *node,
                                         struct powerSubsystemInfo *response)
{
        char    *str;

        response->subsystemType =
                soap_enum(powerSystemType_S,
                          soap_tree_value(node, "subsystemType"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->redundancy =
                soap_enum(redundancy_S, soap_tree_value(node, "redundancy"));
        response->redundancyMode =
                soap_enum(powerRedundancy_S,
                          soap_tree_value(node, "redundancyMode"));
        response->capacity = atoi(soap_tree_value(node, "capacity"));
        response->redundantCapacity =
                atoi(soap_tree_value(node, "redundantCapacity"));
        response->outputPower = atoi(soap_tree_value(node, "outputPower"));
        response->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        response->inputPowerVa = atof(soap_tree_value(node, "inputPowerVa"));
        response->inputPowerCapacityVa =
                atof(soap_tree_value(node, "inputPowerCapacityVa"));
        if ((str = soap_tree_value(node, "inputPower")))
                response->inputPower = atof(str);
        else
                response->inputPower = -1.0;
        if ((str = soap_tree_value(node, "inputPowerCapacity")))
                response->inputPowerCapacity = atof(str);
        else
                response->inputPowerCapacity = -1.0;
        response->goodPowerSupplies =
                atoi(soap_tree_value(node, "goodPowerSupplies"));
        response->wantedPowerSupplies =
                atoi(soap_tree_value(node, "wantedPowerSupplies"));
        response->neededPowerSupplies =
                atoi(soap_tree_value(node, "neededPowerSupplies"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_thermalInfo - Parses a thermalInfo response structure */
static void     parse_thermalInfo(xmlNode *node, struct thermalInfo *response)
{
        response->sensorType =
                soap_enum(sensorType_S, soap_tree_value(node, "sensorType"));
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->sensorStatus =
                soap_enum(sensorStatus_S,
                          soap_tree_value(node, "sensorStatus"));
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->temperatureC = atoi(soap_tree_value(node, "temperatureC"));
        response->cautionThreshold =
                atoi(soap_tree_value(node, "cautionThreshold"));
        response->criticalThreshold =
                atoi(soap_tree_value(node, "criticalThreshold"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_enclosureBaysSelection - Parses an enclosureBaysSelection response
 * structure
 */
static void     parse_enclosureBaysSelection(xmlNode *node,
                        struct enclosureBaysSelection *response)
{
        char    *str;

        if ((str = soap_tree_value(node, "oaAccess")))
                response->oaAccess = parse_xsdBoolean(str);
        else
                response->oaAccess = HPOA_FALSE; /* Default */
        response->bladeBays = soap_walk_tree(node, "bladeBays:blade");
        response->interconnectTrayBays =
                soap_walk_tree(node, "interconnectTrayBays:interconnectTray");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_userInfo - Parses a userInfo response structure */
static void     parse_userInfo(xmlNode *node, struct userInfo *response)
{
        response->username = soap_tree_value(node, "username");
        response->fullname = soap_tree_value(node, "fullname");
        response->contactInfo = soap_tree_value(node, "contactInfo");
        response->isEnabled =
                parse_xsdBoolean(soap_tree_value(node, "isEnabled"));
        response->acl = soap_enum(userAcl_S, soap_tree_value(node, "acl"));
        parse_enclosureBaysSelection(soap_walk_tree(node, "bayPermissions"),
                                     &(response->bayPermissions));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_oaNetworkInfo - Parses a oaNetworkInfo response structure */
static void     parse_oaNetworkInfo(xmlNode *node,
                                    struct oaNetworkInfo *response)
{
        response->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        response->dhcpEnabled =
                parse_xsdBoolean(soap_tree_value(node, "dhcpEnabled"));
        response->dynDnsEnabled =
                parse_xsdBoolean(soap_tree_value(node, "dynDnsEnabled"));
        response->macAddress = soap_tree_value(node, "macAddress");
        response->ipAddress = soap_tree_value(node, "ipAddress");
        response->netmask = soap_tree_value(node, "netmask");
        response->gateway = soap_tree_value(node, "gateway");
        response->dns = soap_walk_tree(node, "dns:ipAddress");
        response->elinkIpAddress = soap_tree_value(node, "elinkIpAddress");
        response->linkActive =
                parse_xsdBoolean(soap_tree_value(node, "linkActive"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_thermalSubsystemInfo - Parses a thermalSubsystemInfo response
 * structure
 */
static void     parse_thermalSubsystemInfo(xmlNode *node,
                                        struct thermalSubsystemInfo *response)
{
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->redundancy =
                soap_enum(redundancy_S, soap_tree_value(node, "redundancy"));
        response->goodFans = atoi(soap_tree_value(node, "goodFans"));
        response->wantedFans = atoi(soap_tree_value(node, "wantedFans"));
        response->neededFans = atoi(soap_tree_value(node, "neededFans"));
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_fanZoneArray - Parses a fanZoneArray structure */
static void     parse_fanZoneArray(xmlNode *node,
                                   struct getFanZoneArrayResponse *response)
{
        response->fanZoneArray = soap_walk_tree(node, "fanZoneArray:fanZone");
}

/* parse_enclosureNetworkInfo - Parses a enclosureNetworkInfo structure */
static void      parse_enclosureNetworkInfo(xmlNode *node,
                                       struct  enclosureNetworkInfo *response)
{
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_enclosureStatus - Parses a enclosureStatus structure */
static void      parse_enclosureStatus(xmlNode *node,
                                       struct enclosureStatus *response)
{
        response->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        response->uid =  soap_enum(uidStatus_S,
                                   soap_tree_value(node, "uid"));
        response->wizardStatus =
                soap_enum(wizardStatus_S,
                          soap_tree_value(node, "wizardStatus"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_lcdInfo - Parses a lcdInfo structure */
static void     parse_lcdInfo(xmlNode *node, struct lcdInfo *response)
{
        response->name = soap_tree_value(node, "name");
        response->partNumber = soap_tree_value(node, "partNumber");
        response->manufacturer = soap_tree_value(node, "manufacturer");
        response->fwVersion = soap_tree_value(node, "fwVersion");
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_lcdStatus - Parses a lcdStatus structure */
static void     parse_lcdStatus(xmlNode *node, struct lcdStatus *response)
{
        response->status = soap_enum(opStatus_S,
                                     soap_tree_value(node, "status"));
        response->display =  soap_enum(uidStatus_S,
                                       soap_tree_value(node, "display"));
        response->lcdPin = parse_xsdBoolean(soap_tree_value(node, "lcdPin"));
        response->buttonLock =
                parse_xsdBoolean(soap_tree_value(node, "buttonLock"));
        response->lcdSetupHealth =
                soap_enum(lcdSetupHealth_S,
                          soap_tree_value(node, "lcdSetupHealth"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (response->diagnosticChecks));
        response->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        response->extraData = soap_walk_tree(node, "extraData");
}

/* parse_getBladeThermalInfoArray - Parses a bladeThermalInfoArrayResponse 
 * structure */
static void parse_getBladeThermalInfoArray(xmlNode *node, 
				struct bladeThermalInfoArrayResponse *response)
{
	response->bladeThermalInfoArray =
		soap_walk_tree(node, 
			       "bladeThermalInfoArray:bladeThermalInfo");

}

/* parse_getAllEvents - Parses a getAllEventsResponse structure */
static void     parse_getAllEvents(xmlNode *node,
                                   struct getAllEventsResponse *response)
{
        response->eventInfoArray =
                soap_walk_tree(node, "eventInfoArray:eventInfo");
}


/* User-callable functions that are used to walk the lists of response
 * parameters.  Please note that there are many structures involved, which
 * are derived from the SOAP/XML interface specification.  Instead of trying
 * to document each fully in the comments, please refer to the oa_soap_calls.h
 * file which contains structure definitions, and to the OA SOAP interface
 * documentation (HP Onboard Administrator SOAP Interface Guide).
 */

/* soap_getExtraData - Walks list of extraData nodes, providing the name and
 *      value for each.  Used after any SOAP call that returns an extraData
 *      xmlNode pointer.
 *
 * Outputs:
 *      name:           String containing the extra data name
 *      value:          String containing the value for this name
 */
void    soap_getExtraData(xmlNode *extraData, struct extraDataInfo *result)
{
        if ((extraData) &&
            (extraData->properties) &&
            (extraData->properties->children))
                result->name =
                        (char *)(extraData->properties->children->content);
        else
                result->name = NULL;
        result->value = soap_value(extraData);
}

/* soap_getDiagnosticChecksEx - Walks list of diagnosticChecksEx nodes,
 *      providing the name and value for each.  Used after any SOAP call
 *      that returns a diagnosticChecksEx xmlNode pointer.
 *
 * Outputs:
 *      name:           String containing the diagnosticChecksEx node name
 *      value:          diagnosticStatus enum value
 */
void    soap_getDiagnosticChecksEx(xmlNode *diag,
                                   struct diagnosticData *result)

{
        if ((diag) &&
            (diag->properties) &&
            (diag->properties->children))
                result->name = (char *)(diag->properties->children->content);
        else
                result->name = NULL;
        result->value = soap_enum(diagnosticStatus_S, soap_value(diag));
}

/* soap_getBladeCpuInfo - Walks list of bladeCpuInfo nodes, providing details
 *      on each CPU.  Used after calling soap_getBladeInfo().
 *
 * Outputs:
 *      cpuType:        String describing CPU
 *      cpuSpeed:       CPU speed in MHz
 */
void    soap_getBladeCpuInfo(xmlNode *cpus, struct bladeCpuInfo *result)
{
        result->cpuType = soap_tree_value(cpus, "cpuType");
        result->cpuSpeed = atoi(soap_tree_value(cpus, "cpuSpeed"));
}

/* soap_getBladeNicInfo - Walks list of bladeNicInfo nodes, providing details
 *      on each NIC.  Used after calling soap_getBladeInfo().
 *
 * Outputs:
 *      port:           String describing NIC port
 *      macAddress:     String containing the NIC's MAC address
 */
void    soap_getBladeNicInfo(xmlNode *nics, struct bladeNicInfo *result)
{
        result->port = soap_tree_value(nics, "port");
        result->macAddress = soap_tree_value(nics, "macAddress");
}

/* soap_getBladeMezzInfo - Walks list of bladeMezzInfo nodes, providing details
 *      on each Mezz Card.  
 *
 * Outputs:
 *      mezzNumber:           	Mezz number
 *      mezzSlots:  	   	Mezz slots node
 *      mezzDevices: 		Mezz devices node
 */
void    soap_getBladeMezzInfo(xmlNode *mezz, struct bladeMezzInfo *result)
{
	result->mezzNumber = soap_tree_value(mezz, "mezzNumber");
	result->mezzSlots = soap_walk_tree(mezz, "mezzSlots");
	result->mezzDevices = soap_walk_tree(mezz, "mezzDevices");
	result->extraData = soap_walk_tree(mezz, "extraData");


}
/* soap_getBladeMezzDevInfo- Walks list of mezzDevices nodes, providing details
 *      on each Mezz device Info.
 *
 * Outputs:
 *      name:           String containing the mezz device name
 *      type:           enum containing the mezz device type
 *      port:           XML node containg mezz device port info
 */
void    soap_getBladeMezzDevInfo(xmlNode *mezzDevices, 
				 struct bladeMezzDevInfo *result)
{
	result->name = soap_tree_value(mezzDevices, "name");
	result->type = soap_enum(bladeMezzDevType_S,
			soap_tree_value(mezzDevices, "type"));
	result->port = soap_walk_tree(mezzDevices, "port");
	result->extraData = soap_walk_tree(mezzDevices, "extraData");
}

/* soap_getBladeMezzSlotInfo- Walks list of mezzSlots nodes, providing details
 * on each Mezz Card slot.
 *
 * Outputs:
 *      type:     	enum containing the mezz slot type
 *      slot:     	XML node containing the mezz slot port info
 */
void    soap_getBladeMezzSlotInfo(xmlNode *mezzSlots, 
				  struct bladeMezzSlotInfo *result)
{
	result->type = soap_enum(bladeMezzSlotType_S,
			soap_tree_value(mezzSlots, "type"));
	result->slot = soap_walk_tree(mezzSlots, "slot");
	result->extraData = soap_walk_tree(mezzSlots, "extraData");
}
/* soap_getBladeMezzSlotPort- Walks list of bladeMezzSlotPort nodes, providing
 * details on each slot port.
 *
 * Outputs:
 *      slotNumber:                 String describing mezz slot number
 *      interconnectTrayBayNumber:  String containing interconnectTrayBayNumber 
 *      interconnectTrayPortNumber: String containing interconnectTrayPortNumber
 */
void    soap_getBladeMezzSlotPort(xmlNode *slot,
				  struct bladeMezzSlotPort *result)
{
	result->slotNumber = soap_tree_value(slot, "slotNumber");
	result->interconnectTrayBayNumber = 
		soap_tree_value(slot, "interconnectTrayBayNumber");
	result->interconnectTrayPortNumber = 
		soap_tree_value(slot, "interconnectTrayPortNumber");
	result->extraData = soap_walk_tree(slot, "extraData");
}
/* soap_getBladeMezzDevPort- Walks list of bladeMezzDevPort nodes, providing
 * details on each Mezz DevPort.
 *
 * Outputs:
 *      portNumber:             String describing Mezz Device port number
 *      wwpn:                   String containing wwpn
 *      fabric:                 enum containing the fabric type
 *      status:                 enum containing fabricStatus
 */
void    soap_getBladeMezzDevPort(xmlNode *port, struct bladeMezzDevPort *result)
{
	result->portNumber = soap_tree_value(port, "portNumber");
	result->wwpn = soap_tree_value(port, "wwpn");
	result->fabric = soap_enum(fabricType_S,
			soap_tree_value(port, "fabric"));
	result->status = soap_enum(fabricStatus_S,
			soap_tree_value(port, "status"));
	result->extraData = soap_walk_tree(port, "extraData");
}
/* soap_getInterconnectTraySlotInfo- Walks list of interconnectTraySlotInfo 
 * nodes, providing details on each InterconnectTray Slot.  
 *
 * Outputs:
 *      interconnectTraySlotNumber:   InterconnectTray Slot Number
 *      type:           Enum containing interconnect tray type
 *      port:           XML node containing InterconnectTraySlot Port
 */
void    soap_getInterconnectTraySlotInfo(xmlNode *slot, 
		struct interconnectTraySlotInfo *result)
{
	result->interconnectTraySlotNumber = 
		atoi(soap_tree_value(slot, "interconnectTraySlotNumber"));
	result->type = soap_enum(interconnectTrayType_S,
			soap_tree_value(slot, "type"));
	result->port = soap_walk_tree(slot, "port:slot");
	result->extraData = soap_walk_tree(slot, "extraData");
}
/* soap_getInterconnectTrayPortInfo- Walks list of interconnectTrayPortInfo 
 * nodes, providing details on each InterconnectTrayPort.
 *
 * Outputs:
 *      interconnectTraySlotPortNumber:	Interconnect tray slot port number
 *      bladeBayNumber:    	Blade Bay Number 
 *	bladeMezzNumber:        Blade Mezz Number
 *	bladeMezzPortNumber:    BladeMezzPortNumber
 *	portStatus:             Enum containing Interconnect Tray Port Status
 * 	portEnabled:            Enum containing the interconnectTray Port 
 *                              enabled status.
 *	portUidStatus:          Enum containing interconnect tray port 
 *                              Uid status
 *	portLinkLedStatus:      Enum containing interconnect tray port link 
 *                              Led Status
 */
void    soap_getInterconnectTrayPortInfo(xmlNode *port,
		struct interconnectTrayPortInfo *result)
{
	result->interconnectTraySlotPortNumber = 
		atoi(soap_tree_value(port, "interconnectTraySlotPortNumber"));
	result->bladeBayNumber = 
		atoi(soap_tree_value(port, "bladeBayNumber"));
	result->bladeMezzNumber = 
		atoi(soap_tree_value(port, "bladeMezzNumber"));
	result->bladeMezzPortNumber = 
		atoi(soap_tree_value(port, "bladeMezzPortNumber"));
	result->portStatus = soap_enum(interconnectTrayPortStatus_S,
			soap_tree_value(port, "portStatus"));
	result->portEnabled = soap_enum(interconnectTrayPortEnabled_S,
			soap_tree_value(port, "portEnabled"));
	result->portUidStatus = soap_enum(interconnectTrayPortUidStatus_S,
			soap_tree_value(port, "portUidStatus"));
	result->portLinkLedStatus = 
		soap_enum(interconnectTrayPortLinkLedStatus_S,
				soap_tree_value(port, "portLinkLedStatus"));
	result->extraData = soap_walk_tree(port, "extraData");
}

#if 0                                   /* TODO: Not sure the following call
                                         * will work...perhaps same problem
                                         * as extraData
                                         */
/* soap_getDiagnosticData - Gets information from diagnosticData nodes,
 *      providing details on each diagnostic event.  Used after calling
 *      soap_getEventInfo().
 *
 * Outputs:
 *      item:           diagnosticStatus enum
 *      name:           String containing name of diagnostic item
 */
void    soap_getDiagnosticData(xmlNode *data, struct diagnosticData *result)
{
        result->item = soap_enum(diagnosticStatus_S,
                                 soap_tree_value(data, "item"));
        result->name = soap_tree_value(data, "name");
}
#endif

/* soap_getBayAccess - Gets information from bayAccess nodes.
 *
 * Outputs:
 *      bayNumber:      Bay number
 *      access:         True if user can access the bay
 */
void    soap_getBayAccess(xmlNode *bay, struct bayAccess *result)
{
        result->bayNumber = atoi(soap_tree_value(bay, "bayNumber"));
        result->access = parse_xsdBoolean(soap_tree_value(bay, "access"));
}

/* soap_getEncLink - Gets information from encLink nodes, providing enclosure
 *      information.
 *
 * Outputs:
 *      enclosureNumber: Which enclosure is this?
 *      oaName:          Name assigned to the OA
 *      uuid:            UUID
 *      rackName:        Name assigned to the rack
 *      enclosureName:   Name assigned to the enclosure
 *      url:             URL
 *      local:           Local or remote
 */
void    soap_getEncLink(xmlNode *data, struct encLink *result)
{
        result->enclosureNumber =
                atoi(soap_tree_value(data, "enclosureNumber"));
        result->oaName = soap_tree_value(data, "oaName"); /* May be NULL */
        result->uuid = soap_tree_value(data, "uuid"); /* May be NULL */
        result->rackName = soap_tree_value(data, "rackName"); /* May be NULL */
        result->enclosureName =
                soap_tree_value(data, "enclosureName"); /* May be NULL */
        result->url = soap_tree_value(data, "url");
        result->local = parse_xsdBoolean(soap_tree_value(data, "local"));
        result->extraData = soap_walk_tree(data, "extraData");
}

/* soap_getEncLink2 - Gets information from encLink2 nodes,
 * providing enclosure information.
 *
 * Outputs:
 *      enclosureNumber:        Which enclosure is this?
 *      productId:              Product ID
 *      mfgId:                  Manufacturer ID
 *      enclosureUuid:          Enclosure Universal Unique ID
 *      encloseSerialNumber:    Serial number from enclosure
 *      enclosureName:          Enclosure name
 *      enclosureProductName:   Enclosure product name including mfg name
 *      enclosureStatus:        Enclosure status
 *      enclosureRackIpAddress: Rack IP address
 *      enclosureUrl:           URL for the enclosure
 *      rackName:               Rack name assigned by user
 *      primaryEnclosure:       Flag for identifying primary
 *      encLinkOaArray:         encLinkOa array structure
 */
void    soap_getEncLink2(xmlNode *data, struct encLink2 *result)
{
        result->enclosureNumber =
                atoi(soap_tree_value(data, "enclosureNumber"));
        result->productId = atoi(soap_tree_value(data, "productId"));
        result->mfgId = atoi(soap_tree_value(data, "mfgId"));
        result->enclosureUuid = soap_tree_value(data, "enclosureUuid");
        result->enclosureSerialNumber =
                soap_tree_value(data, "enclosureSerialNumber");
        result->enclosureName = soap_tree_value(data, "enclosureName");
        result->enclosureProductName =
                soap_tree_value(data, "enclosureProductName");
        result->enclosureStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(data, "enclosureStatus"));
        result->enclosureRackIpAddress =
                soap_tree_value(data, "enclosureRackIpAddress");
        result->enclosureUrl = soap_tree_value(data, "enclosureUrl");
        result->rackName = soap_tree_value(data, "rackName");
        result->primaryEnclosure =
                parse_xsdBoolean(soap_tree_value(data, "primaryEnclosure"));
        result->encLinkOa = soap_walk_tree(data, "encLinkOaArray:encLinkOa");
        result->extraData = soap_walk_tree(data, "extraData");
}

/* soap_getEncLinkOa - Gets information from encLinkOa nodes,
 * providing enclosure information.
 *
 * Outputs:
 *      activeOa:       Active OA flag indicator
 *      bayNumber:      Bay number of the OA
 *      oaName:         OA name assigned by the user
 *      ipAddress:      IP address for OA ENC Link
 *      macAddress:     MAC address for OA ENC Link
 *      fwVersion:      OA firmware version
 */
void    soap_getEncLinkOa (xmlNode *data, struct encLinkOa *result)
{
        result->activeOa = parse_xsdBoolean(soap_tree_value(data, "activeOa"));
        result->bayNumber = atoi(soap_tree_value(data, "bayNumber"));
        result->oaName = soap_tree_value(data, "oaName");
        result->ipAddress = soap_tree_value(data, "ipAddress");
        result->macAddress = soap_tree_value(data, "macAddress");
        result->fwVersion = soap_tree_value(data, "fwVersion");
        result->extraData = soap_walk_tree(data, "extraData");
}


/* soap_getPortEnabled - Gets information from portEnabled nodes
 *
 * Outputs:
 *      portNumber:     Port number
 *      enabled:        Is it enabled?
 */
void    soap_getPortEnabled(xmlNode *data, struct portEnabled *result)
{
        result->portNumber = atoi(soap_tree_value(data, "portNumber"));
        result->enabled = parse_xsdBoolean(soap_tree_value(data, "enabled"));
}

/* soap_getIpAddress - Gets information from ipAddress nodes
 *
 * Outputs:
 *      result:         String containing IP address
 */
void    soap_getIpAddress(xmlNode *ips, char **result)
{
        *result = soap_value(ips);
}

/* soap_fanZone - Gets information from fanZone nodes
 *
 * Outputs:
 *      zoneNumber:         Zone number
 *      operationalStatus:  Operational status of the fan zone
 *      redundant:          Redundant status of the fan zone
 *      targetRpm:          Target RPM of the fan zone
 *      targetPwm:          Target PWM of the fan zone
 *      deviceBayArray:     Device bays of the fan zone
 *      fanInfoArray:       Fan info of the fan zone
 */
void    soap_fanZone(xmlNode *node, struct fanZone *result)
{
        result->zoneNumber = atoi(soap_tree_value(node, "zoneNumber"));
        result->redundant =
                soap_enum(redundancy_S, soap_tree_value(node, "redundant"));
        result->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        result->targetRpm = atoi(soap_tree_value(node, "targetRpm"));
        result->targetPwm = atoi(soap_tree_value(node, "targetPwm"));
        result->deviceBayArray = soap_walk_tree(node, "deviceBayArray:bay");
        result->fanInfoArray = soap_walk_tree(node, "fanInfoArray:fanInfo");
        result->extraData = soap_walk_tree(node, "extraData");
}

/* soap_deviceBayArray - Gets information from fanZone nodes
 *
 * Outputs:
 *      bay:                bay number
 */
void    soap_deviceBayArray(xmlNode *node, byte *bay)
{
        *bay = atoi(soap_value(node));
}

/* soap_fanInfo - Gets information from fanInfo nodes
 *
 * Outputs:
 *      bayNumber:          Bay number
 *      presence:           Presence status of the fan
 *      name:               Name of the fan
 *      partNumber:         Part number of the fan
 *      sparePartNumber:    Spare part number of the fan
 *      serialNumber:       Serial number of the fan
 *      powerConsumed:      Power consumed by the fan
 *      fanSpeed:           current fan speed
 *      maxFanSpeed:        Maximum fan speed
 *      lowLimitFanSpeed:   Low limit fan speed
 *      operationalStatus:  Operational status of the fan
 *      diagnosticChecks:   Diagnostic checks of the fan
 */

void    soap_fanInfo(xmlNode *node, struct fanInfo *result)
{
        result->bayNumber = atoi(soap_tree_value(node, "bayNumber"));
        result->presence =
                soap_enum(presence_S, soap_tree_value(node, "presence"));
        result->name = soap_tree_value(node, "name");
        result->partNumber = soap_tree_value(node, "partNumber");
        result->sparePartNumber = soap_tree_value(node, "sparePartNumber");
        result->serialNumber = soap_tree_value(node, "serialNumber");
        result->powerConsumed = atoi(soap_tree_value(node, "powerConsumed"));
        result->fanSpeed = atoi(soap_tree_value(node, "fanSpeed"));
        result->maxFanSpeed = atoi(soap_tree_value(node, "maxFanSpeed"));
        result->lowLimitFanSpeed =
                atoi(soap_tree_value(node, "lowLimitFanSpeed"));
        result->operationalStatus =
                soap_enum(opStatus_S,
                          soap_tree_value(node, "operationalStatus"));
        parse_diagnosticChecks(soap_walk_tree(node, "diagnosticChecks"),
                               & (result->diagnosticChecks));
        result->diagnosticChecksEx = locate_diagnosticChecksEx(node);
        result->extraData = soap_walk_tree(node, "extraData");
}

void	soap_bladeThermalInfo(xmlNode *node, struct bladeThermalInfo * result)
{
	result->sensorNumber = atoi(soap_tree_value(node, "sensorNumber"));
	result->sensorType = atoi(soap_tree_value(node, "sensorType"));
	result->entityId = atoi(soap_tree_value(node, "entityId"));
	result->entityInstance = atoi(soap_tree_value(node, "entityInstance"));
	result->criticalThreshold = 
          atoi(soap_tree_value(node, "criticalThreshold"));
	result->cautionThreshold = 
				atoi(soap_tree_value(node, "cautionThreshold"));
	result->temperatureC = atoi(soap_tree_value(node, "temperatureC"));
	result->oem = atoi(soap_tree_value(node, "oem"));
	result->description = soap_tree_value(node, "description");
	result->extraData = soap_walk_tree(node, "extraData");
}

/* soap_getEventInfo - Walks list of eventInfo nodes, providing details
 *      on each event.  Used after calling soap_getAllEvents().
 *
 * Outputs:
 *      event:          indicates the type of event
 *      eventTimeStamp: the time this event happened
 *      queueSize:      size of event queue, or -1 for not present
 *      eventData:      the event information for each type of event
 */
void    soap_getEventInfo(xmlNode *events, struct eventInfo *result)
{
        char    *str;
        xmlNode *node;

	if((str = soap_tree_value(events, "event")))
        result->event =
                soap_enum(eventType_S, str);
	else
        result->event = -1;

	if((str = soap_tree_value(events, "eventTimeStamp")))
		result->eventTimeStamp = atoi(str);
	else
        	result->eventTimeStamp = -1;

        if ((str = soap_tree_value(events, "queueSize")))
                result->queueSize = atoi(str);
        else
                result->queueSize = -1;
	if ((str = soap_tree_value(events, "numValue"))) {
		result->numValue = atoi(str);
	}
        result->extraData = soap_walk_tree(events, "extraData");

        /* The remainder depends on what sort of data is returned by the OA.
         * The current documentation says that only one of these can be
         * returned, and indeed, since we have a union, we can handle only
         * one.  Therefore, we will quit and return early when we get a
         * match.
         *
         * As of this writing, not all events have been handled.  Those that
         * are not handled yet are noted by comments left in the code.  Please
         * note that because some are unimplemented, these events will drop
         * through and be returned as "NOPAYLOAD", an answer that is not
         * correct.
         */
        if ((node = soap_walk_tree(events, "syslog"))) {
                result->enum_eventInfo = SYSLOG;
                parse_syslog(node, &(result->eventData.syslog));
                return;
        }

        if ((node = soap_walk_tree(events, "rackTopology"))) {
                result->enum_eventInfo = RACKTOPOLOGY;
                parse_rackTopology(node, &(result->eventData.rackTopology));
                return;
        }

        if ((node = soap_walk_tree(events, "enclosureNetworkInfo"))) {
                result->enum_eventInfo = ENCLOSURENETWORKINFO;
                parse_enclosureNetworkInfo(node,
                                     &(result->eventData.enclosureNetworkInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "enclosureStatus"))) {
                result->enum_eventInfo = ENCLOSURESTATUS;
                parse_enclosureStatus(node,
                                      &(result->eventData.enclosureStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "enclosureInfo"))) {
                result->enum_eventInfo = ENCLOSUREINFO;
                parse_enclosureInfo(node, &(result->eventData.enclosureInfo));
                if ((node = soap_walk_tree(events, "powerSubsystemInfo"))) {
                        result->eventData.enclosureInfo.powerType = 
                                soap_enum(powerSystemType_S,
                                     soap_tree_value(node, "subsystemType"));
                }
                if ((node = soap_walk_tree(events, "enclosureStatus"))) {
                        result->eventData.enclosureInfo.enclosureStatus = 
                                soap_enum(opStatus_S,
                                     soap_tree_value(node, "operationalStatus"));
                }
                return;
        }

        if ((node = soap_walk_tree(events, "oaStatus"))) {
                result->enum_eventInfo = OASTATUS;
                parse_oaStatus(node, &(result->eventData.oaStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "oaInfo"))) {
                result->enum_eventInfo = OAINFO;
                parse_oaInfo(node, &(result->eventData.oaInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "bladeInfo"))) {
                result->enum_eventInfo = BLADEINFO;
                parse_bladeInfo(node, &(result->eventData.bladeInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "bladeMpInfo"))) {
                result->enum_eventInfo = BLADEMPINFO;
                parse_bladeMpInfo(node, &(result->eventData.bladeMpInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "bladeStatus"))) {
                result->enum_eventInfo = BLADESTATUS;
                parse_bladeStatus(node, &(result->eventData.bladeStatus));
                return;
        }

        /* BLADEPORTMAP */
        if ((node = soap_walk_tree(events, "bladePortMap"))) {
                result->enum_eventInfo = BLADEPORTMAP;
                parse_bladePortMap(node, &(result->eventData.bladePortMap));
                return;
        }


        if ((node = soap_walk_tree(events, "fanInfo"))) {
                result->enum_eventInfo = FANINFO;
                soap_fanInfo(node, &(result->eventData.fanInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "interconnectTrayStatus"))) {
                result->enum_eventInfo = INTERCONNECTTRAYSTATUS;
                parse_interconnectTrayStatus(node,
                        &(result->eventData.interconnectTrayStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "interconnectTrayInfo"))) {
                result->enum_eventInfo = INTERCONNECTTRAYINFO;
                parse_interconnectTrayInfo(node,
                        &(result->eventData.interconnectTrayInfo));
                return;
        }
        /* INTERCONNECTTRAYPORTMAP */
        if ((node = soap_walk_tree(events, "interconnectTrayPortMap"))) {
                result->enum_eventInfo = INTERCONNECTTRAYPORTMAP;
                parse_interconnectTrayPortMap(node,
                        &(result->eventData.interconnectTrayPortMap));
                return;
        }

        if ((node = soap_walk_tree(events, "powerSupplyInfo"))) {
                result->enum_eventInfo = POWERSUPPLYINFO;
                parse_powerSupplyInfo(node,
                        &(result->eventData.powerSupplyInfo));
                return;
        }

        if ((node = soap_walk_tree(events, "powerSupplyStatus"))) {
                result->enum_eventInfo = POWERSUPPLYSTATUS;
                parse_powerSupplyStatus(node,
                        &(result->eventData.powerSupplyStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "powerSubsystemInfo"))) {
                result->enum_eventInfo = POWERSUBSYSTEMINFO;
                parse_powerSubsystemInfo(node,
                        &(result->eventData.powerSubsystemInfo));
                return;
        }

        /* POWERCONFIGINFO */

        if ((node = soap_walk_tree(events, "thermalInfo"))) {
                result->enum_eventInfo = THERMALINFO;
                parse_thermalInfo(node, &(result->eventData.thermalInfo));
                return;
        }

        /* USERINFOARRAY */

        if ((node = soap_walk_tree(events, "userInfo"))) {
                result->enum_eventInfo = USERINFO;
                parse_userInfo(node, &(result->eventData.userInfo));
                return;
        }

        /* LDAPINFO */
        /* LDAPGROUPINFO */
        /* SNMPINFO */
        /* ENCLOSURENETWORKINFO */

        if ((node = soap_walk_tree(events, "oaNetworkInfo"))) {
                result->enum_eventInfo = OANETWORKINFO;
                parse_oaNetworkInfo(node, &(result->eventData.oaNetworkInfo));
                return;
        }

        /* ENCLOSURETIME */
        /* ALERTMAILINFO */
        /* PASSWORDSETTINGS */
        /* EBIPAINFO */
        /* LCDCHATMESSAGE */
        /* LCDUSERNOTES */
        /* LCDBUTTONEVENT */

        if ((node = soap_walk_tree(events, "lcdStatus"))) {
                result->enum_eventInfo = LCDSTATUS;
                parse_lcdStatus(node, &(result->eventData.lcdStatus));
                return;
        }

        if ((node = soap_walk_tree(events, "lcdInfo"))) {
                result->enum_eventInfo = LCDINFO;
                parse_lcdInfo(node, &(result->eventData.lcdInfo));
                return;
        }

        /* HPSIMINFO */

        if ((node = soap_walk_tree(events, "thermalSubsystemInfo"))) {
                result->enum_eventInfo = THERMALSUBSYSTEMINFO;
                parse_thermalSubsystemInfo(node,
                        &(result->eventData.thermalSubsystemInfo));
                return;
        }

        /* BLADEBOOTINFO */
        /* OAVCMMODE */
        /* POWERREDUCTIONSTATUS */
        /* VIRTUALMEDIASTATUS */
        /* OAMEDIADEVICE */

        if ((node = soap_walk_tree(events, "fanZone"))) {
                result->enum_eventInfo = FANZONE;
                soap_fanZone(node, &(result->eventData.fanZone));
                return;
        }

        /* EBIPAINFOEX */
        /* CACERTSINFO */

        if ((node = soap_walk_tree (events, "rackTopology2"))) {
                result->enum_eventInfo = RACKTOPOLOGY2;
                parse_rackTopology2(node, &(result->eventData.rackTopology2));
                return;
        }

        /* USERCERTIFICATEINFO */
        /* SYSLOGSETTINGS */
        /* POWERDELAYSETTINGS */
        /* USBMEDIAFIRMWAREIMAGES */
        /* CONFIGSCRIPTS */
        /* NUMVALUE */
        /* STRING */

        if ((result->eventData.message = soap_tree_value(events, "message"))) {
                result->enum_eventInfo = MESSAGE;
                return;
        }

        result->enum_eventInfo = NOPAYLOAD; /* If we get here, we got nothing */
}


/* Finally, the main body of OA SOAP call functions.  These are the ones that
 * a user would expect to call for most things.  Please refer to the associated
 * .h file for parameter details, and to the HP Onboard Administrator SOAP
 * Interface Guide for further call details.
 */
int soap_subscribeForEvents(SOAP_CON *con, struct eventPid *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, SUBSCRIBE_FOR_EVENTS))) {
                parse_eventPid(soap_walk_doc(con->doc, "Body:eventPid"),
                               response);
        }
        return(ret);
}

int soap_unSubscribeForEvents(SOAP_CON *con,
                              const struct unSubscribeForEvents *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, UN_SUBSCRIBE_FOR_EVENTS, request->pid));
}

int soap_getEvent(SOAP_CON *con,
                  const struct getEvent *request,
                  struct eventInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_EVENT,
                           request->pid,
                           request->waitTilEventHappens, /* xsd:boolean */
                           request->lcdEvents))) {       /* xsd:boolean */
               soap_getEventInfo(soap_walk_doc(con->doc,
                                               "Body:"
                                               "getEventResponse:"
                                               "eventInfo"),
                                 response);
        }
        return(ret);
}

int soap_getAllEventsEx(SOAP_CON *con,
                      const struct getAllEventsEx *request,
                      struct getAllEventsResponse *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_ALL_EVENTSEX,
                           request->pid,
                           request->waitTilEventHappens, /* xsd:boolean */
                           request->lcdEvents, /* xsd:boolean */
                           request->oaFwVersion))) {
                parse_getAllEvents(soap_walk_doc(con->doc,
                                                 "Body:"
                                                 "getAllEventsExResponse"),
                                   response);
        }
        return(ret);
}

int             soap_getBladeInfo(SOAP_CON *con,
                                  const struct getBladeInfo *request,
                                  struct bladeInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_BLADE_INFO, request->bayNumber))) {
                parse_bladeInfo(soap_walk_doc(con->doc,
                                              "Body:"
                                              "getBladeInfoResponse:"
                                              "bladeInfo"),
                                response);
        }
        return(ret);
}
int             soap_getBladePortMap(SOAP_CON *con,
                                  const struct getBladeInfo *request,
				  struct bladePortMap *response)
{
	SOAP_PARM_CHECK
		if (! (ret = soap_request(con, GET_BLADE_PORTMAP,
						request->bayNumber)))
		{
			parse_bladePortMap(soap_walk_doc(con->doc,
						"Body:"
						"getBladePortMapResponse:"
						"bladePortMap"),
					response);
		}
	return(ret);
}
int             soap_getBladeMpInfo(SOAP_CON *con,
                                    const struct getBladeMpInfo *request,
                                    struct bladeMpInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_BLADE_MP_INFO, request->bayNumber))) {
                parse_bladeMpInfo(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getBladeMpInfoResponse:"
                                                "bladeMpInfo"),
                                  response);
        }
        return(ret);
}

int soap_getEnclosureInfo(SOAP_CON *con,
                          struct enclosureInfo *response)
{
        xmlNode *node=NULL;
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_POWER_SUBSYSTEM_INFO))) {
                node = soap_walk_doc(con->doc,
                                     "Body:"
                                     "getPowerSubsystemInfoResponse:"
                                     "powerSubsystemInfo");
                response->powerType = soap_enum(powerSystemType_S,
                                                    soap_tree_value(node,
                                                    "subsystemType"));

        } 
        if (! (ret = soap_request(con, GET_ENCLOSURE_STATUS))) {
                node = soap_walk_doc(con->doc,
                                     "Body:"
                                     "getEnclosureStatusResponse:"
                                     "enclosureStatus");
                response->enclosureStatus = soap_enum(opStatus_S,
                                                    soap_tree_value(node,
                                                    "operationalStatus"));

        } 
	
        if (! (ret = soap_request(con, GET_ENCLOSURE_INFO))) {
                parse_enclosureInfo(soap_walk_doc(con->doc,
                                                  "Body:"
                                                  "getEnclosureInfoResponse:"
                                                  "enclosureInfo"),
                                    response);
        }
        return(ret);
}

int soap_getPowerConfigInfo(SOAP_CON *con,
                            struct powerConfigInfo *response,
                            uint *desired_static_pwr_limit)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_POWER_CONFIG_INFO))) {
                parse_powerConfigInfo(soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerConfigInfoResponse:"
                                      "powerConfigInfo"),
                                      response);
        }

        /* If user's desired static power limit is 0, then update it with  */
        /* the OA value, otherwise preserve the user's intention for a     */
        /* static power limit.                                             */
        if (*desired_static_pwr_limit == 0) {
                *desired_static_pwr_limit = response->powerCeiling;
        }

        return(ret);
}

int soap_setPowerConfigInfo(SOAP_CON *con,
                            const struct powerConfigInfo *request)
{
        char    hpoa_boolean[HPOA_BOOLEAN_LENGTH];
        char    powerRedundancy[POWER_REDUNDANCY_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(hpoa_boolean, hpoa_boolean_S,
            request->dynamicPowerSaverEnabled)) {
                err("invalid dynamic power parameter");
                return(-1);
        }
        if (soap_inv_enum(powerRedundancy, powerRedundancy_S,
            request->redundancyMode)) {
                err("invalid power redundancy mode parameter");
                return(-1);
        }
        return(soap_request(con, SET_POWER_CONFIG_INFO, request->redundancyMode,
               request->powerCeiling, request->dynamicPowerSaverEnabled));
}

int soap_getPowerCapConfig(SOAP_CON *con,
                           struct powerCapConfig *response,
                           uint *desired_dynamic_pwr_cap_limit,
                           uint *desired_derated_circuit_cap_limit,
                           uint *desired_rated_circuit_cap_limit)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_POWER_CAP_CONFIG))) {
                parse_powerCapConfig(soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerCapConfigResponse:"
                                      "powerCapConfig"),
                                      response);
        }

        /* If user's desired dynamic power cap limit is 0, then update it  */
        /* with the OA value, otherwise preserve the user's intention for  */
        /* a dynamic power cap limit.                                      */
        /* Do the same for the derated and rated circuit caps.             */
        if (*desired_dynamic_pwr_cap_limit == 0) {
                *desired_dynamic_pwr_cap_limit = response->powerCap;
        }
        if (*desired_derated_circuit_cap_limit == 0) {
                *desired_derated_circuit_cap_limit =
                  response->deratedCircuitCap;
        }
        if (*desired_rated_circuit_cap_limit == 0) {
                *desired_rated_circuit_cap_limit = response->ratedCircuitCap;
        }

        return(ret);
}

int soap_setPowerCapConfig(SOAP_CON *con,
                           const struct powerCapConfig *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, SET_POWER_CAP_CONFIG, request->powerCap,
               request->optOutBayArray[0],
               request->optOutBayArray[1],
               request->optOutBayArray[2],
               request->optOutBayArray[3],
               request->optOutBayArray[4],
               request->optOutBayArray[5],
               request->optOutBayArray[6],
               request->optOutBayArray[7],
               request->optOutBayArray[8],
               request->optOutBayArray[9],
               request->optOutBayArray[10],
               request->optOutBayArray[11],
               request->optOutBayArray[12],
               request->optOutBayArray[13],
               request->optOutBayArray[14],
               request->optOutBayArray[15],
               request->deratedCircuitCap,
               request->ratedCircuitCap));
}

int soap_getOaStatus(SOAP_CON *con,
                     const struct getOaStatus *request,
                     struct oaStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_OA_STATUS, request->bayNumber))) {
                parse_oaStatus(soap_walk_doc(con->doc,
                                             "Body:"
                                             "getOaStatusResponse:"
                                             "oaStatus"),
                               response);
        }
        return(ret);
}

int soap_getOaInfo(SOAP_CON *con,
                   const struct getOaInfo *request,
                   struct oaInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_OA_INFO, request->bayNumber))) {
                parse_oaInfo(soap_walk_doc(con->doc,
                                           "Body:"
                                           "getOaInfoResponse:"
                                           "oaInfo"),
                             response);
        }
        return(ret);
}

int soap_getOaId(SOAP_CON *con,
                 struct OaId *response)
{
        SOAP_PARM_CHECK_NRQ 
        if (! (ret = soap_request(con, GET_OA_ID))) {
                parse_oaId(soap_walk_doc(con->doc,
                                           "Body:"
                                           "getOaIdResponse:"),
                             response);
        }
        return(ret);
}

int soap_getInterconnectTrayStatus(SOAP_CON *con,
                const struct getInterconnectTrayStatus *request,
                struct interconnectTrayStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_INTERCONNECT_TRAY_STATUS,
                                  request->bayNumber))) {
                parse_interconnectTrayStatus(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getInterconnectTrayStatusResponse:"
                                      "interconnectTrayStatus"),
                        response);
        }
        return(ret);
}

int soap_getInterconnectTrayInfo(SOAP_CON *con,
                                 const struct getInterconnectTrayInfo *request,
                                 struct interconnectTrayInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_INTERCONNECT_TRAY_INFO,
                                  request->bayNumber))) {
                parse_interconnectTrayInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getInterconnectTrayInfoResponse:"
                                      "interconnectTrayInfo"),
                        response);
        }
        return(ret);
}
int soap_getInterconnectTrayPortMap(SOAP_CON *con,
                                 const struct getInterconnectTrayInfo *request,
                                 struct interconnectTrayPortMap *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_INTERCONNECT_TRAY_PORT_MAP,
                                  request->bayNumber))) {
                parse_interconnectTrayPortMap(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getInterconnectTrayPortMapResponse:"
                                      "interconnectTrayPortMap"),
                        response);
        }
        return(ret);
}
int soap_getFanInfo(SOAP_CON *con,
                    const struct getFanInfo *request,
                    struct fanInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_FAN_INFO, request->bayNumber))) {
                soap_fanInfo(soap_walk_doc(con->doc,
                                           "Body:"
                                           "getFanInfoResponse:"
                                           "fanInfo"),
                             response);
        }
        return(ret);
}

int soap_getPowerSubsystemInfo(SOAP_CON *con,
                               struct powerSubsystemInfo *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_POWER_SUBSYSTEM_INFO))) {
                parse_powerSubsystemInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerSubsystemInfoResponse:"
                                      "powerSubsystemInfo"),
                        response);
        }
        return(ret);
}

int soap_getPowerSupplyInfo(SOAP_CON *con,
                            const struct getPowerSupplyInfo *request,
                            struct powerSupplyInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_POWER_SUPPLY_INFO,
                                  request->bayNumber))) {
                parse_powerSupplyInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getPowerSupplyInfoResponse:"
                                      "powerSupplyInfo"),
                        response);
        }
        return(ret);
}

int soap_getOaNetworkInfo(SOAP_CON *con,
                          const struct getOaNetworkInfo *request,
                          struct oaNetworkInfo *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con,
                                  GET_OA_NETWORK_INFO,
                                  request->bayNumber))) {
                parse_oaNetworkInfo(
                        soap_walk_doc(con->doc,
                                      "Body:"
                                      "getOaNetworkInfoResponse:"
                                      "oaNetworkInfo"),
                        response);
        }
        return(ret);
}

int soap_getBladeStatus(SOAP_CON *con,
                        const struct getBladeStatus *request,
                        struct bladeStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_BLADE_STATUS, request->bayNumber))) {
                parse_bladeStatus(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getBladeStatusResponse:"
                                                "bladeStatus"),
                                  response);
        }
        return(ret);
}

int soap_setBladePower(SOAP_CON *con,
                       const struct setBladePower *request)
{
        char    power[POWER_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(power, powerControl_S, request->power)) {
                err("invalid power parameter");
                return(-1);
        }
        return(soap_request(con, SET_BLADE_POWER, request->bayNumber, power));
}

int soap_setInterconnectTrayPower(SOAP_CON *con,
                const struct setInterconnectTrayPower *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, SET_INTERCONNECT_TRAY_POWER,
                            request->bayNumber, request->on));
}

int soap_resetInterconnectTray(SOAP_CON *con,
                               const struct resetInterconnectTray *request)
{
        SOAP_PARM_CHECK_NRS
        return(soap_request(con, RESET_INTERCONNECT_TRAY, request->bayNumber));
}

int soap_getThermalInfo(SOAP_CON *con,
                        const struct getThermalInfo *request,
                        struct thermalInfo *response)
{
        char    sensor[SENSOR_TYPE_LENGTH];

        SOAP_PARM_CHECK
        if (soap_inv_enum(sensor, sensorType_S, request->sensorType)) {
                err("invalid sensorType parameter");
                return(-1);
        }
        if (! (ret = soap_request(con, GET_THERMAL_INFO,
                                  sensor, request->bayNumber))) {
                parse_thermalInfo(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getThermalInfoResponse:"
                                                "thermalInfo"),
                                  response);
        }
        return(ret);
}

int soap_getUserInfo(SOAP_CON *con,
                     const struct getUserInfo *request,
                     struct userInfo *response)
{
        /* On this one, are there some special rules that have to be followed
         * while sending usernames over XML/SOAP?  I could imagine that a user
         * name could contain characters that need to be escaped in XML.
         *
         * TODO: This needs to be checked.
         */
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_USER_INFO, request->username))) {
                parse_userInfo(soap_walk_doc(con->doc,
                                             "Body:"
                                             "getUserInfoResponse:"
                                             "userInfo"),
                               response);
        }
        return(ret);
}

int soap_getRackTopology2(SOAP_CON *con, struct rackTopology2 *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_RACK_TOPOLOGY2))) {
                parse_rackTopology2(soap_walk_doc(con->doc,
                                                  "Body:"
                                                  "getRackTopology2Response:"
                                                  "rackTopology2"),
                                    response);
        }
        return(ret);
}

int soap_isValidSession(SOAP_CON *con)
{
        if (con == NULL) {
                err("NULL parameter");
                return -1;
        }
        return(soap_request(con, IS_VALID_SESSION));
}

int soap_getThermalSubsystemInfo(SOAP_CON *con,
                                 struct thermalSubsystemInfo *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_THERMAL_SUBSYSTEM_INFO))) {
                parse_thermalSubsystemInfo(soap_walk_doc(con->doc,
                                             "Body:"
                                             "getThermalSubsystemInfoResponse:"
                                             "thermalSubsystemInfo"),
                                           response);
        }
        return(ret);
}

int soap_getFanZoneArray(SOAP_CON *con,
                         const struct getFanZoneArray *request,
                         struct getFanZoneArrayResponse *response)
{
        /* TODO: There is messy code below.  It should be encapsulated,
         * either in a routine or macro.  Holding off doing this for now,
         * until we're sure how array input parameters will be used.
         */
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        byte    *p;
        SOAP_PARM_CHECK
        /* Generate the fan zone array necessary for this request */
        bay_array[0] = 0;
        for (p = request->bayArray.array;
             p - request->bayArray.array < request->bayArray.size;
             p++) {
                snprintf(bay_array + strlen(bay_array), sizeof(BAY), BAY, *p);
        }
        if (! (ret = soap_request(con, GET_FAN_ZONE_ARRAY, bay_array))) {
                parse_fanZoneArray(soap_walk_doc(con->doc,
                                                 "Body:"
                                                 "getFanZoneArrayResponse"),
                                   response);
        }
        return(ret);
}

int soap_getEnclosureStatus(SOAP_CON *con, struct enclosureStatus *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_ENCLOSURE_STATUS))) {
                parse_enclosureStatus(soap_walk_doc(con->doc,
                                                   "Body:"
                                                   "getEnclosureStatusResponse:"
                                                   "enclosureStatus"),
                                      response);
        }
        return(ret);
}

int soap_getLcdInfo(SOAP_CON *con, struct lcdInfo *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_LCD_INFO))) {
                parse_lcdInfo(soap_walk_doc(con->doc,
                                            "Body:"
                                            "getLcdInfoResponse:"
                                            "lcdInfo"),
                              response);
        }
        return(ret);
}

int soap_getLcdStatus(SOAP_CON *con, struct lcdStatus *response)
{
        SOAP_PARM_CHECK_NRQ
        if (! (ret = soap_request(con, GET_LCD_STATUS))) {
                parse_lcdStatus(soap_walk_doc(con->doc,
                                              "Body:"
                                              "getLcdStatusResponse:"
                                              "lcdStatus"),
                                response);
        }
        return(ret);
}

int soap_getPowerSupplyStatus(SOAP_CON *con,
                              const struct getPowerSupplyStatus *request,
                              struct powerSupplyStatus *response)
{
        SOAP_PARM_CHECK
        if (! (ret = soap_request(con, GET_POWER_SUPPLY_STATUS,
                                  request->bayNumber))) {
                parse_powerSupplyStatus(soap_walk_doc(con->doc,
                                                "Body:"
                                                "getPowerSupplyStatusResponse:"
                                                "powerSupplyStatus"),
                                        response);
        }
        return(ret);
}

int soap_setEnclosureUid(SOAP_CON *con,
                         const struct setEnclosureUid *request)
{
        char    uid[UID_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(uid, uidControl_S, request->uid)) {
                err("invalid UID parameter");
                return(-1);
        }
        return(soap_request(con, SET_ENCLOSURE_UID, uid));
}

int soap_setOaUid(SOAP_CON *con,
                  const struct setOaUid *request)
{
        char    uid[UID_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(uid, uidControl_S, request->uid)) {
                err("invalid UID parameter");
                return(-1);
        }
        return(soap_request(con, SET_OA_UID, request->bayNumber, uid));
}

int soap_setBladeUid(SOAP_CON *con,
                     const struct setBladeUid *request)
{
        char    uid[UID_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(uid, uidControl_S, request->uid)) {
                err("invalid UID parameter");
                return(-1);
        }
        return(soap_request(con, SET_BLADE_UID, request->bayNumber, uid));
}

int soap_setInterconnectTrayUid(SOAP_CON *con,
                                const struct setInterconnectTrayUid *request)
{
        char    uid[UID_CONTROL_LENGTH];

        SOAP_PARM_CHECK_NRS
        if (soap_inv_enum(uid, uidControl_S, request->uid)) {
                err("invalid UID parameter");
                return(-1);
        }
        return(soap_request(con, SET_INTERCONNECT_TRAY_UID, request->bayNumber,
                            uid));
}

int soap_setLcdButtonLock(SOAP_CON *con,
			  enum hpoa_boolean buttonLock)
{
        return(soap_request(con, SET_LCD_BUTTON_LOCK, buttonLock));
}

int soap_getBladeThermalInfoArray(SOAP_CON *con,
				  struct getBladeThermalInfoArray *request,
				struct bladeThermalInfoArrayResponse *response)
{
	SOAP_PARM_CHECK
	if (! (ret = soap_request(con, GET_BLADE_THERMAL_INFO_ARRAY,
				  request->bayNumber))) {
            parse_getBladeThermalInfoArray(soap_walk_doc(con->doc,
					   "Body:"
					   "getBladeThermalInfoArrayResponse"),
					   response);
	}
	return(ret);
}

int soap_getPowerSupplyInfoArray(SOAP_CON *con,
                         const struct getPowerSupplyInfoArray *request,
                         struct getPowerSupplyInfoArrayResponse *response,
                         xmlDocPtr *ps_info_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the PS info array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (!(ret = soap_request(con, GET_POWER_SUPPLY_INFO_ARRAY, bay_array))){
                if( *ps_info_doc != NULL)  { 
                    dbg("ps_info_doc is NOT NULL, Please check");
                    xmlFreeDoc(*ps_info_doc);
                }
                *ps_info_doc = xmlCopyDoc (con->doc,1); 
                if( *ps_info_doc == NULL)
                    return(-1);
                tmp = soap_walk_doc(*ps_info_doc,
                                    "Body:""getPowerSupplyInfoArrayResponse:");
                response->powerSupplyInfoArray = soap_walk_tree(tmp,
                                        "powerSupplyInfoArray:powerSupplyInfo");
        }
        return(ret);
}

int soap_getPowerSupplyStatusArray(SOAP_CON *con,
                         const struct getPowerSupplyStsArray *request,
                         struct getPowerSupplyStsArrayResponse *response,
                         xmlDocPtr *ps_sts_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the PS status array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (!(ret = soap_request(con, GET_POWER_SUPPLY_STATUS_ARRAY, bay_array))){
                if( *ps_sts_doc != NULL) {
                    dbg("ps_sts_doc is NOT NULL, Please check");
                    xmlFreeDoc(*ps_sts_doc);
                }
                *ps_sts_doc = xmlCopyDoc (con->doc,1);
                if( *ps_sts_doc == NULL)
                    return(-1);
                tmp = soap_walk_doc(*ps_sts_doc,
                                    "Body:""getPowerSupplyStatusArrayResponse:");
                response->powerSupplyStsArray = soap_walk_tree(tmp,
                                        "powerSupplyStatusArray:powerSupplyStatus");
        }
        return(ret);
}

int soap_getFanInfoArray(SOAP_CON *con,
                         const struct getFanInfoArray *request,
                         struct getFanInfoArrayResponse *response,
                         xmlDocPtr *fan_info_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the fan info array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_FAN_INFO_ARRAY, bay_array))) {
                if( *fan_info_doc != NULL) {
                    dbg("fan_info_doc is NOT NULL, Please check");
                    xmlFreeDoc(*fan_info_doc);
                }
                *fan_info_doc = xmlCopyDoc (con->doc,1);
                if( *fan_info_doc == NULL)
                    return(-1);
                tmp = soap_walk_doc(*fan_info_doc,
                                    "Body:""getFanInfoArrayResponse");
                response->fanInfoArray = soap_walk_tree(tmp,
                                          "fanInfoArray:fanInfo");
        }
        return(ret);
}

int soap_getBladeInfoArray(SOAP_CON *con,
                         const struct getBladeInfoArray *request,
                         struct getBladeInfoArrayResponse *response,
                         xmlDocPtr *bl_info_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the blade info array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_BLADE_INFO_ARRAY, bay_array))) {
                if( *bl_info_doc != NULL) {
                    dbg("bl_info_doc is NOT NULL, Please check");
                    xmlFreeDoc(*bl_info_doc);
                }
                *bl_info_doc = xmlCopyDoc (con->doc,1);
                if( *bl_info_doc == NULL)
                    return(-1);
                tmp = soap_walk_doc(*bl_info_doc,
                                    "Body:""getBladeInfoArrayResponse");
                response->bladeInfoArray = soap_walk_tree(tmp,
                                          "bladeInfoArray:bladeInfo");
        }
        return(ret);
}

int soap_getBladeStatusArray(SOAP_CON *con,
                         const struct getBladeStsArray *request,
                         struct getBladeStsArrayResponse *response,
                         xmlDocPtr *bl_sts_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the blade status array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_BLADE_STATUS_ARRAY, bay_array))) {
                if( *bl_sts_doc != NULL) {
                    dbg("bl_sts_doc is NOT NULL, Please check");
                    xmlFreeDoc(*bl_sts_doc);
                }
                *bl_sts_doc = xmlCopyDoc(con->doc,1);
                if( *bl_sts_doc == NULL)
                    return (-1);
                tmp = soap_walk_doc(*bl_sts_doc,
                                    "Body:""getBladeStatusArrayResponse");

                response->bladeStsArray = soap_walk_tree(tmp,
                                                     "bladeStatusArray:bladeStatus");
        }
        return(ret);
}

int soap_getBladePortMapArray(SOAP_CON *con,
                         const struct getBladePortMapArray *request,
                         struct getBladePortMapArrayResponse *response,
                         xmlDocPtr *bl_pm_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the blade portmap array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_BLADE_PORTMAP_ARRAY, bay_array))) {
                if( *bl_pm_doc != NULL) {
                    dbg("bl_pm_doc is NOT NULL, Please check");
                    xmlFreeDoc(*bl_pm_doc);
                }
                *bl_pm_doc = xmlCopyDoc(con->doc,1);
                if( *bl_pm_doc == NULL)
                    return (-1);
                tmp = soap_walk_doc(*bl_pm_doc,
                                    "Body:""getBladePortMapArrayResponse");

                response->portMapArray = soap_walk_tree(tmp,
                                                     "bladePortMapArray:bladePortMap");
        }
        return(ret);
}
int soap_getInterconnectTrayInfoArray(SOAP_CON *con,
                            const struct getInterconnectTrayInfoArray *request,
                            struct interconnectTrayInfoArrayResponse *response,
                            xmlDocPtr *intr_info_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the interconnect tray info array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_INTERCONNECT_TRAY_INFO_ARRAY,
                                    bay_array))) {
                if( *intr_info_doc != NULL) {
                    dbg("intr_info_doc is NOT NULL, Please check");
                    xmlFreeDoc(*intr_info_doc);
                }
                *intr_info_doc = xmlCopyDoc( con->doc,1);
                if( *intr_info_doc ==NULL)
                    return(-1);
                tmp = soap_walk_doc(*intr_info_doc,
                                    "Body:"
                                    "getInterconnectTrayInfoArrayResponse");
                response->interconnectTrayInfoArray = 
                              soap_walk_tree(tmp,
                              "interconnectTrayInfoArray:interconnectTrayInfo");
        }
        return(ret);
}

int soap_getInterconnectTrayStatusArray(SOAP_CON *con,
                            const struct interconnectTrayStsArray *request,
                            struct interconnectTrayStsArrayResponse *response,
                            xmlDocPtr *intr_sts_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the interconnect tray status array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con,GET_INTERCONNECT_TRAY_STATUS_ARRAY, 
                                    bay_array))) {
                if( *intr_sts_doc != NULL) {
                    dbg("intr_sts_doc is NOT NULL, Please check");
                    xmlFreeDoc(*intr_sts_doc);
                }
                *intr_sts_doc = xmlCopyDoc(con->doc,1);
                if( *intr_sts_doc == NULL)
                    return( -1);
                tmp = soap_walk_doc( *intr_sts_doc,
                                     "Body:"
                                     "getInterconnectTrayStatusArrayResponse");
                response->interconnectTrayStsArray = soap_walk_tree(tmp, 
                         "interconnectTrayStatusArray:interconnectTrayStatus");
        }
        return(ret);
}
              
int soap_getInterconnectTrayPortMapArray(SOAP_CON *con,
                            const struct interconnectTrayPmArray *request,
                            struct interconnectTrayPmArrayResponse *response,
                            xmlDocPtr *intr_pm_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the interconnect tray portmap array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con,GET_INTERCONNECT_TRAY_PORTMAP_ARRAY,
                                    bay_array))) {
                if( *intr_pm_doc != NULL) {
                    dbg("intr_pm_doc is NOT NULL, Please check");
                    xmlFreeDoc(*intr_pm_doc);
                }
                *intr_pm_doc = xmlCopyDoc(con->doc,1);
                if( *intr_pm_doc == NULL)
                    return( -1);
                tmp = soap_walk_doc( *intr_pm_doc,
                                     "Body:"
                                     "getInterconnectTrayPortMapArrayResponse");
                response->interconnectTrayPmArray = soap_walk_tree(tmp,
                         "interconnectTrayPortMapArray:interconnectTrayPortMap");
        }
        return(ret);
}

int soap_getOaInfoArray(SOAP_CON *con,
                        const struct getOaInfoArray *request,
                        struct getOaInfoArrayResponse *response,
                        xmlDocPtr *oa_info_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the OA info array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_OA_INFO_ARRAY, bay_array))) {
                if( *oa_info_doc != NULL) {
                    dbg("oa_info_doc is NOT NULL, Please check");
                    xmlFreeDoc(*oa_info_doc);
                }
                *oa_info_doc = xmlCopyDoc(con->doc,1);
                if( *oa_info_doc == NULL)
                    return (-1);
                tmp = soap_walk_doc(*oa_info_doc,
                                    "Body:""getOaInfoArrayResponse");
                response->oaInfoArray = soap_walk_tree(tmp,
                                                       "oaInfoArray:oaInfo");
        }
        return(ret);
}

int soap_getOaStatusArray(SOAP_CON *con,
                         const struct getOaStatusArray *request,
                         struct getOaStatusArrayResponse *response,
                         xmlDocPtr *oa_sts_doc)
{
        SOAP_PARM_CHECK
        char    bay_array[(sizeof(BAY) + 1) * request->bayArray.size];
        xmlNode *tmp = NULL;

        /* Generate the OA status array necessary for this request */
        bay_array[0] = 0;
        SOAP_ARRAY_REQ

        if (! (ret = soap_request(con, GET_OA_STATUS_ARRAY, bay_array))) {
                if( *oa_sts_doc != NULL) {
                    dbg("oa_sts_doc is NOT NULL, Please check");
                    xmlFreeDoc(*oa_sts_doc);
                }
                *oa_sts_doc = xmlCopyDoc(con->doc,1);
                if( *oa_sts_doc == NULL)
                    return (-1);
                tmp = soap_walk_doc(*oa_sts_doc,
                                    "Body:""getOaStatusArrayResponse");

                response->oaStatusArray = soap_walk_tree(tmp,
                                                     "oaStatusArray:oaStatus");
        }
        return(ret);
}


