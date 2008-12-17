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
 *
 *
 * This is an example program, written to demonstrate how to use the
 * SOAP stub library, and to provide a simple test bed during development
 * of these stubs.  See the comments inline for a more detailed description
 * of functionality.
 *
 * You will probably want to customize the soap_open() parameters, so that
 * this works with your c-class cage.  See the defines after the include
 * files.
 */


/* Include files */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <oh_error.h>
#include <oa_soap_calls.h>
#include <oa_soap_callsupport.h>
#ifdef MCHECK
#include <mcheck.h>
#endif


/* Configuration values, to set this up for your c-class cage */
#if 1
/* These are for the OSLO cage */
#define	OA_DEST		"ccc2-oa-1.hpi.telco:443"
#define	OA_USERNAME	"root"
#define	OA_PASSWORD	"hosehead"
#else
/* These are for the GDIC cage; note that the pair is .38 and .39, and only
 * the active one will respond.
 */
#define	OA_DEST		"16.138.181.57:443"
#define	OA_USERNAME	"hpi"
#define	OA_PASSWORD	"hpi123"
#endif


int main(int argc, char *argv[])
{
    SOAP_CON			*con;
    struct getBladeInfo		bladeInfo_request;
    struct bladeInfo		bladeInfo_response;
    struct getBladeStatus	bladeStatus_request;
    struct bladeStatus		bladeStatus_response;
    struct extraDataInfo	extraDataInfo_response;
    struct diagnosticData	diagnosticChecksEx_response;
    struct bladeCpuInfo		bladeCpuInfo;
    struct bladeNicInfo		bladeNicInfo;
    struct eventPid		subscribeForEvents_response;
    struct unSubscribeForEvents	unSubscribeForEvents_request;
    struct getOaNetworkInfo	getOaNetworkInfo_request;
    struct oaNetworkInfo	getOaNetworkInfo_response;
    struct getThermalInfo	getThermalInfo_request;
    struct thermalInfo		getThermalInfo_response;
    struct getUserInfo		getUserInfo_request;
    struct userInfo		getUserInfo_response;
    struct getAllEvents		getAllEvents_request;
    struct getAllEventsResponse	getAllEvents_response;
    struct eventInfo		event;
    struct bayAccess		bayAccess;
    struct rackTopology2        grt2_response;
    struct encLink2             encLink2_res;
    struct encLinkOa            encLinkOa_res;
    struct enclosureStatus	enclosureStatus_response;
    struct lcdStatus		lcdStatus_response;
    struct lcdInfo		lcdInfo_response;
    struct getPowerSupplyStatus powerSupplyStatus_request;
    struct powerSupplyStatus	powerSupplyStatus_response;
    struct thermalSubsystemInfo thermalSubsystemInfo_response;
    struct getFanZoneArray	fanZoneArray_request;
    struct getFanZoneArrayResponse fanZoneArray_response;
    struct fanZone		fanZone;
    struct fanInfo		fanInfo;
    byte			bay;
    int				slot;
    int				i;
    int				ret;
    int				old_timeout = 0;
    char			*str;
#if 0				/* Don't use this all the time */
    struct getEvent		getEvent_request;
#endif


#ifdef MCHECK
    mtrace();
    malloc(0x1234);		/* Test memory segment to see it show up */
#endif


    /* In order to report errors, we need OPENHPI_ERROR set to YES */
    putenv("OPENHPI_ERROR=YES");


    /* The OpenHPI infrastructure does the SSL initialization.  We need to
     * do that here.
     */
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    if (! SSL_library_init()) {
        err("SSL_library_init() failed");
        return(-1);
    }


    /* Open a new OA connection.  There needs to be one of these for each
     * thread that intends to access the OA.
     *
     * Parameters:
     *   OA_DEST	A combined hostname and port that will be accessed.
     *   OA_USERNAME	The username for OA logins
     *   OA_PASSWORD	The password for the above username
     *   Timeout	A timeout value, in seconds, for this connection
     */
    con = soap_open(OA_DEST, OA_USERNAME, OA_PASSWORD, 20);
    if (! con) {
    	err("failed soap_open() in main()");
	exit(1);
    }


#if 0
    /* Example of ignoring errors.  Without this, errors will cause normal
     * OpenHPI error output using the err() macro.  With this, some of the
     * error output is supressed, but failed calls still return failure
     * information.
     *
     * The second parameter controls whether errors are ignored (True) or
     * are handled normally (False).
     */
    soap_ignore_errors(con, 1);
#endif


    /* Having opened a session, make sure the session is valid.
     *
     * A comment here: Since the support code can do automatic login,
     * there is no way to really know if the current session is invalid.
     * If it was, simply making this call will cause a new login sequence
     * to occur, and then the session contained in the "con" structure
     * will be valid.  In this case, soap_isValidSession() will return
     * successfully since the session is now valid.  The only way this
     * call will fail is if a valid session cannot be obtained, based
     * on the username and password previously presented to soap_open().
     */
    if (soap_isValidSession(con)) {
    	err("failed soap_isValidSession() in main()");
    }
    else {
        printf("Valid session check passed\n");
    }


#if 0
	/* Trash the session key, to simulate a stale session.  Don't do
	 * this in your code.  It's just here to test whether the auto-login
	 * code can correctly recover from a stale or lost login session.
	 */
	con->session_id[1] = 'k';
#endif


    /* Try the getBladeInfo() call.  In this loop, you can print information
     * for lots of blades.  As shipped, it is only looking at the blade in
     * slot 1.
     *
     * Warning, warning, warning!!!  This code is not correct!!!
     *
     * The interface rules for the OA SOAP library specify that the data
     * returned by the calls is valid only until the next OA SOAP call.  Once
     * the next call is made, the data which is referenced by any pointers
     * becomes invalid.
     *
     * In the code below, it "happens" to work, probably because the first
     * call allocates a lot of data, and the subsequent calls (CpuInfo and
     * NicInfo) only overwrite a little (the beginning) of the previously
     * allocated data.  But to be strictly correct, you MUST copy the data
     * from any pointers (e.g. strings, arrays) that you want BEFORE making
     * another OA SOAP call.
     *
     * BJS: Fix in this example code.
     */
    for (slot = 1; slot <= 1; slot++) {
    	bladeInfo_request.bayNumber = slot;
	if (soap_getBladeInfo(con, &bladeInfo_request, &bladeInfo_response)) {
    	    err("failed soap_getBladeInfo() in main()");
	    /* Falling through here to (eventually) close the connection */
	    break;
	}
	else {
	    /* Successful call...print response parameters */
	    printf("\ngetBladeInfo response values for slot %d:\n", slot);
	    printf("  bayNumber    = %d\n", bladeInfo_response.bayNumber);
	    printf("  presence     = %d\n", bladeInfo_response.presence);
	    printf("  bladeType    = %d\n", bladeInfo_response.bladeType);
	    printf("  width        = %d\n", bladeInfo_response.width);
	    printf("  height       = %d\n", bladeInfo_response.height);
	    printf("  name         = %s\n", bladeInfo_response.name);
	    printf("  manufacturer = %s\n", bladeInfo_response.manufacturer);
	    printf("  partNumber   = %s\n", bladeInfo_response.partNumber);
	    printf("  sparePartNumber = %s\n",
	    	   bladeInfo_response.sparePartNumber);
	    printf("  serialNumber = %s\n", bladeInfo_response.serialNumber);
	    printf("  serverName   = %s\n", bladeInfo_response.serverName);
	    printf("  uuid         = %s\n", bladeInfo_response.uuid);
	    printf("  rbsuOsName   = %s\n", bladeInfo_response.rbsuOsName);
	    printf("  assetTag     = %s\n", bladeInfo_response.assetTag);
	    printf("  romVersion   = %s\n", bladeInfo_response.romVersion);
	    printf("  numberOfCpus = %d\n", bladeInfo_response.numberOfCpus);
	    /* This is how to process an array of items.  Note that if you
	     * wanted a definite loop, you could use "numberOfCpus" as the
	     * maximum index.  But for this, we'll loop until all entries
	     * are exhausted.
	     */
	    while (bladeInfo_response.cpus) {
		soap_getBladeCpuInfo(bladeInfo_response.cpus,
				     &bladeCpuInfo);
		printf("    CPU:\n");
		printf("      cpuType  = %s\n", bladeCpuInfo.cpuType);
		printf("      cpuSpeed = %d\n", bladeCpuInfo.cpuSpeed);
		bladeInfo_response.cpus =
			soap_next_node(bladeInfo_response.cpus);
	    }
	    printf("  memory       = %d\n", bladeInfo_response.memory);
	    printf("  numberOfNics = %d\n", bladeInfo_response.numberOfNics);
	    /* Similarly, process the NIC info array */
	    while (bladeInfo_response.nics) {
		soap_getBladeNicInfo(bladeInfo_response.nics,
				     &bladeNicInfo);
		printf("    NIC:\n");
		printf("      port       = %s\n", bladeNicInfo.port);
		printf("      macAddress = %s\n", bladeNicInfo.macAddress);
		bladeInfo_response.nics =
			soap_next_node(bladeInfo_response.nics);
	    }
	    printf("  mmHeight     = %d\n", bladeInfo_response.mmHeight);
	    printf("  mmWidth      = %d\n", bladeInfo_response.mmWidth);
	    printf("  mmDepth      = %d\n", bladeInfo_response.mmDepth);
	    printf("  deviceId     = %d\n", bladeInfo_response.deviceId);
	    printf("  productId    = %d\n", bladeInfo_response.productId);
	    /* The extra data is a list of values...print each */
	    printf("  extraData Values:\n");
	    while (bladeInfo_response.extraData) {
		soap_getExtraData(bladeInfo_response.extraData,
				  & extraDataInfo_response);
		printf("    %s: %s\n",
		       extraDataInfo_response.name,
		       extraDataInfo_response.value);
		bladeInfo_response.extraData =
			soap_next_node(bladeInfo_response.extraData);
	    }
	}
    }


    /* Example of printing errors from a prior SOAP call */
    if (soap_error_number(con)) {
	printf("\nPrevious call had error %d: %s\n",
	       soap_error_number(con), soap_error_string(con));
    }


    /* Try the getBladeStatus() call on blade 1.  */
    slot = 1;				/* Doing BladeStatus on this slot */
    bladeStatus_request.bayNumber = slot;
    if (soap_getBladeStatus(con, &bladeStatus_request, &bladeStatus_response)) {
	err("failed soap_getBladeStatus() in main()");
	/* Falling through here to (eventually) close the connection */
    }
    else {
	/* Successful call...print response parameters */
	printf("\ngetBladeStatus response values for slot %d:\n", slot);
	printf("  bayNumber     = %d\n", bladeStatus_response.bayNumber);
	printf("  presence      = %d\n", bladeStatus_response.presence);
	printf("  op status     = %d\n",
	       bladeStatus_response.operationalStatus);
	printf("  thermal       = %d\n", bladeStatus_response.thermal);
	printf("  powered       = %d\n", bladeStatus_response.powered);
	printf("  powerState    = %d\n", bladeStatus_response.powerState);
	printf("  shutdown      = %d\n", bladeStatus_response.shutdown);
	printf("  uid           = %d\n", bladeStatus_response.uid);
	printf("  powerConsumed = %d\n", bladeStatus_response.powerConsumed);
	/* DiagnosticChecks structure */
	printf("  Diagnostic Checks:\n");
	printf("    internalDataError        = %d\n",
	       bladeStatus_response.diagnosticChecks.internalDataError);
	printf("    managementProcessorError = %d\n",
	       bladeStatus_response.diagnosticChecks.managementProcessorError);
	printf("    thermalWarning           = %d\n",
	       bladeStatus_response.diagnosticChecks.thermalWarning);
	printf("    thermalDanger            = %d\n",
	       bladeStatus_response.diagnosticChecks.thermalDanger);
	printf("    ioConfigurationError     = %d\n",
	       bladeStatus_response.diagnosticChecks.ioConfigurationError);
	printf("    devicePowerRequestError  = %d\n",
	       bladeStatus_response.diagnosticChecks.devicePowerRequestError);
	printf("    insufficientCooling      = %d\n",
	       bladeStatus_response.diagnosticChecks.insufficientCooling);
	printf("    deviceLocationError      = %d\n",
	       bladeStatus_response.diagnosticChecks.deviceLocationError);
	printf("    deviceFailure            = %d\n",
	       bladeStatus_response.diagnosticChecks.deviceFailure);
	printf("    deviceDegraded           = %d\n",
	       bladeStatus_response.diagnosticChecks.deviceDegraded);
	printf("    acFailure                = %d\n",
	       bladeStatus_response.diagnosticChecks.acFailure);
	printf("    i2cBuses                 = %d\n",
	       bladeStatus_response.diagnosticChecks.i2cBuses);
	printf("    redundancy               = %d\n",
	       bladeStatus_response.diagnosticChecks.redundancy);
	/* The extra data is a list of values...print each */
	printf("  extraData Values:\n");
	while (bladeStatus_response.extraData) {
		soap_getExtraData(bladeStatus_response.extraData,
				  & extraDataInfo_response);
		printf("    %s: %s\n",
		       extraDataInfo_response.name,
		       extraDataInfo_response.value);
		bladeStatus_response.extraData =
			soap_next_node(bladeStatus_response.extraData);
	}
        if (bladeStatus_response.diagnosticChecksEx) {
            printf("  There are Diagnostic Checks Ex\n");
	    while (bladeStatus_response.diagnosticChecksEx) {
		soap_getDiagnosticChecksEx(
			bladeStatus_response.diagnosticChecksEx,
			& diagnosticChecksEx_response);
		printf("    %s: %d\n",
		       diagnosticChecksEx_response.name,
		       diagnosticChecksEx_response.value);
		bladeStatus_response.diagnosticChecksEx =
			soap_next_node(bladeStatus_response.diagnosticChecksEx);
	    }
	}
    }


    /* Try a getRackTopology2 call */
    if (soap_getRackTopology2(con, &grt2_response)) {
        err("failed soap_getRackTopology2() in soaptest.c");
    }
    else {
        printf("\ngetRackTopology2 responses: \n");
        printf("  RUID       = %s\n", grt2_response.ruid);
        printf("  Enclosures:\n");

	/* Process the EncLink2 array */
        while (grt2_response.enclosures) {
            soap_getEncLink2(grt2_response.enclosures, &encLink2_res);
            printf("    Enclosure Number    = %d\n",
                   encLink2_res.enclosureNumber);
            printf("    Product ID         = %d\n", encLink2_res.productId);
            printf("    Manufacturer ID    = %d\n", encLink2_res.mfgId);
            printf("    Enclosure UUID     = %s\n", encLink2_res.enclosureUuid);
            printf("    Enclosure SerialNo = %s\n",
                   encLink2_res.enclosureSerialNumber);
            printf("    Enclosure Prodcut  = %s\n",
                   encLink2_res.enclosureProductName);
            printf("    Enclosure Rack IP  = %s\n",
                   encLink2_res.enclosureRackIpAddress);
            printf("    Enclosure URL      = %s\n", encLink2_res.enclosureUrl);
            printf("    Rack Name          = %s\n", encLink2_res.rackName);
            printf("    EncLinkOa:\n");

	    /* Process the EncLinkOa array */
            while (encLink2_res.encLinkOa) {
                soap_getEncLinkOa(encLink2_res.encLinkOa, &encLinkOa_res);
                printf("      Bay Number       = %d\n",
                       encLinkOa_res.bayNumber);
                printf("      OA Name          = %s\n",
		       encLinkOa_res.oaName);
                printf("      IP Address       = %s\n",
                       encLinkOa_res.ipAddress);
                printf("      MAC Address      = %s\n",
                       encLinkOa_res.macAddress);
                printf("      Firmware Version = %s\n",
                       encLinkOa_res.fwVersion);

                encLink2_res.encLinkOa = soap_next_node(encLink2_res.encLinkOa);
            }

            grt2_response.enclosures = soap_next_node(grt2_response.enclosures);
        }
    }


    /* Try a getOaNetworkInfo call */
    slot = 1;
    getOaNetworkInfo_request.bayNumber = slot;
    if (soap_getOaNetworkInfo(con, &getOaNetworkInfo_request,
			      &getOaNetworkInfo_response)) {
    	err("failed soap_getOaNetworkInfo() in main()");
	/* Falling through here to (eventually) close the connection */
    }
    else {
	/* Successful call...print response parameters */
	printf("\ngetOaNetworkInfo response values for slot %d:\n", slot);
	printf("  bayNumber     = %d\n", getOaNetworkInfo_response.bayNumber);
	printf("  dhcpEnabled   = %d\n", getOaNetworkInfo_response.dhcpEnabled);
	printf("  dynDnsEnabled = %d\n",
	       getOaNetworkInfo_response.dynDnsEnabled);
	printf("  macAddress    = %s\n", getOaNetworkInfo_response.macAddress);
	printf("  ipAddress     = %s\n", getOaNetworkInfo_response.ipAddress);
	printf("  netmask       = %s\n", getOaNetworkInfo_response.netmask);
	printf("  gateway       = %s\n", getOaNetworkInfo_response.gateway);
	/* Process the DNS array */
	printf("  DNS Servers:\n");
	while (getOaNetworkInfo_response.dns) {
	    soap_getIpAddress(getOaNetworkInfo_response.dns, &str);
	    printf("    IP Address: %s\n", str);
	    getOaNetworkInfo_response.dns =
			soap_next_node(getOaNetworkInfo_response.dns);
	}
	printf("  elinkIpAddr   = %s\n",
	       getOaNetworkInfo_response.elinkIpAddress);
	printf("  linkActive    = %d\n", getOaNetworkInfo_response.linkActive);
    }


    /* Try a getThermalInfo call */
    slot = 1;
    getThermalInfo_request.sensorType = SENSOR_TYPE_BLADE;
    getThermalInfo_request.bayNumber = slot;
    if (soap_getThermalInfo(con, &getThermalInfo_request,
			    &getThermalInfo_response)) {
    	err("failed soap_getThermalInfo() in main()");
	/* Falling through here to (eventually) close the connection */
    }
    else {
	/* Successful call...print response parameters */
	printf("\ngetThermalInfo response values for slot %d:\n", slot);
	printf("  sensorType     = %d\n", getThermalInfo_response.sensorType);
	printf("  bayNumber      = %d\n", getThermalInfo_response.bayNumber);
	printf("  sensorStatus   = %d\n", getThermalInfo_response.sensorStatus);
	printf("  opStatus       = %d\n",
	       getThermalInfo_response.operationalStatus);
	printf("  temperatureC   = %d\n", getThermalInfo_response.temperatureC);
	printf("  cautionThresh  = %d\n",
	       getThermalInfo_response.cautionThreshold);
	printf("  criticalThresh = %d\n",
	       getThermalInfo_response.criticalThreshold);
	/* The extra data is a list of values...print each */
	printf("  extraData Values:\n");
	while (getThermalInfo_response.extraData) {
		soap_getExtraData(getThermalInfo_response.extraData,
				  & extraDataInfo_response);
		printf("    %s: %s\n",
		       extraDataInfo_response.name,
		       extraDataInfo_response.value);
		getThermalInfo_response.extraData =
			soap_next_node(getThermalInfo_response.extraData);
	}
    }


    /* Try a getUserInfo call */
    getUserInfo_request.username = OA_USERNAME;
    if (soap_getUserInfo(con, &getUserInfo_request, &getUserInfo_response)) {
    	err("failed soap_getUserInfo() in main()");
	/* Falling through here to (eventually) close the connection */
    }
    else {
	/* Successful call...print response parameters */
	printf("\ngetUserInfo response values for user %s:\n",
	       getUserInfo_request.username);
	printf("  username       = %s\n", getUserInfo_response.username);
	printf("  fullname       = %s\n", getUserInfo_response.fullname);
	printf("  contactInfo    = %s\n", getUserInfo_response.contactInfo);
	printf("  isEnabled      = %d\n", getUserInfo_response.isEnabled);
	printf("  AccessCtrlList = %d\n", getUserInfo_response.acl);
	printf("  Bay Permissions:\n");
	printf("    OA Access    = %d\n",
	       getUserInfo_response.bayPermissions.oaAccess);
	printf("    Blade Bays:\n");
	while (getUserInfo_response.bayPermissions.bladeBays) {
	    soap_getBayAccess(getUserInfo_response.bayPermissions.bladeBays,
	    		      &bayAccess);
	    printf("      Bay Number: %d\n", bayAccess.bayNumber);
	    printf("      Access:     %d\n", bayAccess.access);
	    getUserInfo_response.bayPermissions.bladeBays =
		soap_next_node(getUserInfo_response.bayPermissions.bladeBays);
	}
	printf("    Interconnect Tray Bays:\n");
	while (getUserInfo_response.bayPermissions.interconnectTrayBays) {
	    soap_getBayAccess(
	    	getUserInfo_response.bayPermissions.interconnectTrayBays,
	    	&bayAccess);
	    printf("      Bay Number: %d\n", bayAccess.bayNumber);
	    printf("      Access:     %d\n", bayAccess.access);
	    getUserInfo_response.bayPermissions.interconnectTrayBays =
		soap_next_node(
		    getUserInfo_response.bayPermissions.interconnectTrayBays);
	}
    }


    /* Try getEnclosureStatus call. */
    if (soap_getEnclosureStatus(con, &enclosureStatus_response)) {
    	    err("failed soap_getEnclosureStatus() in main()");
    } else {
	    /* Successful call...print response parameters */
	    printf("\ngetEnclosureStatus response values \n");
	    printf("  operationalStatus    = %d\n",
                    enclosureStatus_response.operationalStatus);
	    printf("  uid    = %d\n", enclosureStatus_response.uid);
	    printf("  wizardStatus    = %d\n",
                   enclosureStatus_response.wizardStatus);
            printf("  Diagnostic Checks:\n");
            printf("    internalDataError:        %d\n",
                   enclosureStatus_response.diagnosticChecks.internalDataError);
            printf("    managementProcessorError: %d\n",
                   enclosureStatus_response.
                       diagnosticChecks.managementProcessorError);
            printf("    thermalWarning:           %d\n",
                   enclosureStatus_response.diagnosticChecks.thermalWarning);
            printf("    thermalDanger:            %d\n",
                   enclosureStatus_response.diagnosticChecks.thermalDanger);
            printf("    ioConfigurationError:     %d\n",
                   enclosureStatus_response.
                       diagnosticChecks.ioConfigurationError);
            printf("    devicePowerRequestError:  %d\n",
                   enclosureStatus_response.
                       diagnosticChecks.devicePowerRequestError);
            printf("    insufficientCooling:      %d\n",
                   enclosureStatus_response.
                       diagnosticChecks.insufficientCooling);
            printf("    deviceLocationError:      %d\n",
                   enclosureStatus_response.
                       diagnosticChecks.deviceLocationError);
            printf("    deviceFailure:            %d\n",
                   enclosureStatus_response.diagnosticChecks.deviceFailure);
            printf("    deviceDegraded:           %d\n",
                   enclosureStatus_response.diagnosticChecks.deviceDegraded);
            printf("    acFailure:                %d\n",
                   enclosureStatus_response.diagnosticChecks.acFailure);
            printf("    i2cBuses:                 %d\n",
                   enclosureStatus_response.diagnosticChecks.i2cBuses);
            printf("    redundancy:               %d\n",
                   enclosureStatus_response.diagnosticChecks.redundancy);
            if (enclosureStatus_response.diagnosticChecksEx) 
                printf("  There are Diagnostic Checks Ex\n");
    }


    /* Try getThermalSubsystemInfo call. */
    if ( soap_getThermalSubsystemInfo(con, &thermalSubsystemInfo_response)) {
    	    err("failed  soap_getThermalSubsystemInfo() in main()");
    } else {
	    /* Successful call...print response parameters */
	    printf("\n soap_getThermalSubsystemInfo response values \n");
	    printf("  operationalStatus    = %d\n",
                    thermalSubsystemInfo_response.operationalStatus);
	    printf("  redundancy    = %d\n",
                   thermalSubsystemInfo_response.redundancy);
	    printf("  goodFans    = %d\n",
                   thermalSubsystemInfo_response.goodFans);
	    printf("  wantedFans    = %d\n",
                   thermalSubsystemInfo_response.wantedFans);
	    printf("  neededFans    = %d\n",
                   thermalSubsystemInfo_response.neededFans);
    }


    /* Try getLcdStatus call. */
    if ( soap_getLcdStatus(con, &lcdStatus_response)) {
            err("failed  soap_getLcdStatus() in main()");
    } else {
            /* Successful call...print response parameters */
            printf("\n soap_getLcdStatus response values \n");
            printf("  status    = %d\n", lcdStatus_response.status);
            printf("  display    = %d\n", lcdStatus_response.display);
            printf("  lcdPin    = %d\n", lcdStatus_response.lcdPin);
            printf("  buttonLock    = %d\n", lcdStatus_response.buttonLock);
            printf("  lcdSetupHealth    = %d\n",
                   lcdStatus_response.lcdSetupHealth);
            printf("  Diagnostic Checks:\n");
            printf("    internalDataError:        %d\n",
                   lcdStatus_response.diagnosticChecks.internalDataError);
            printf("    managementProcessorError: %d\n",
                   lcdStatus_response.
                       diagnosticChecks.managementProcessorError);
            printf("    thermalWarning:           %d\n",
                   lcdStatus_response.diagnosticChecks.thermalWarning);
            printf("    thermalDanger:            %d\n",
                   lcdStatus_response.diagnosticChecks.thermalDanger);
            printf("    ioConfigurationError:     %d\n",
                   lcdStatus_response.diagnosticChecks.ioConfigurationError);
            printf("    devicePowerRequestError:  %d\n",
                   lcdStatus_response.diagnosticChecks.devicePowerRequestError);
            printf("    insufficientCooling:      %d\n",
                   lcdStatus_response.diagnosticChecks.insufficientCooling);
            printf("    deviceLocationError:      %d\n",
                   lcdStatus_response.diagnosticChecks.deviceLocationError);
            printf("    deviceFailure:            %d\n",
                   lcdStatus_response.diagnosticChecks.deviceFailure);
            printf("    deviceDegraded:           %d\n",
                   lcdStatus_response.diagnosticChecks.deviceDegraded);
            printf("    acFailure:                %d\n",
                   lcdStatus_response.diagnosticChecks.acFailure);
            printf("    i2cBuses:                 %d\n",
                   lcdStatus_response.diagnosticChecks.i2cBuses);
            printf("    redundancy:               %d\n",
                   lcdStatus_response.diagnosticChecks.redundancy);
            if (lcdStatus_response.diagnosticChecksEx) 
                printf("  There are Diagnostic Checks Ex\n");
    }
   

    /* Try getLcdInfo call. */
    if ( soap_getLcdInfo(con, &lcdInfo_response)) {
            err("failed  soap_getLcdInfo() in main()");
    } else {
            /* Successful call...print response parameters */
            printf("\n soap_getLcdInfo response values \n");
            printf("  name    = %s\n", lcdInfo_response.name);
            printf("  partNumber    = %s\n", lcdInfo_response.partNumber);
            printf("  manufacturer    = %s\n", lcdInfo_response.manufacturer);
            printf("  fwVersion    = %s\n", lcdInfo_response.fwVersion);
    }


    /* Try getPowerSupplyStatus call */
    powerSupplyStatus_request.bayNumber = 1;
    if ( soap_getPowerSupplyStatus(con,  &powerSupplyStatus_request,
                                   &powerSupplyStatus_response)) {
            err("failed  soap_getPowerSupplyStatus() in main()");
    } else {
            /* Successful call...print response parameters */
            printf("\n soap_getPowerSupplyStatus response values \n");
            printf("  bayNumber    = %d\n",
                   powerSupplyStatus_response.bayNumber);
            printf("  presence    = %d\n", powerSupplyStatus_response.presence);
            printf("  operationalStatus    = %d\n",
                   powerSupplyStatus_response.operationalStatus);
            printf("  inputStatus    = %d\n",
                   powerSupplyStatus_response.inputStatus);
            printf("  Diagnostic Checks:\n");
            printf("    internalDataError:        %d\n",
                   powerSupplyStatus_response.
                        diagnosticChecks.internalDataError);
            printf("    managementProcessorError: %d\n",
                   powerSupplyStatus_response.
                       diagnosticChecks.managementProcessorError);
            printf("    thermalWarning:           %d\n",
                   powerSupplyStatus_response.diagnosticChecks.thermalWarning);
            printf("    thermalDanger:            %d\n",
                   powerSupplyStatus_response.diagnosticChecks.thermalDanger);
            printf("    ioConfigurationError:     %d\n",
                   powerSupplyStatus_response.
                        diagnosticChecks.ioConfigurationError);
            printf("    devicePowerRequestError:  %d\n",
                   powerSupplyStatus_response.
                        diagnosticChecks.devicePowerRequestError);
            printf("    insufficientCooling:      %d\n",
                   powerSupplyStatus_response.
                        diagnosticChecks.insufficientCooling);
            printf("    deviceLocationError:      %d\n",
                   powerSupplyStatus_response.
                        diagnosticChecks.deviceLocationError);
            printf("    deviceFailure:            %d\n",
                   powerSupplyStatus_response.diagnosticChecks.deviceFailure);
            printf("    deviceDegraded:           %d\n",
                   powerSupplyStatus_response.diagnosticChecks.deviceDegraded);
            printf("    acFailure:                %d\n",
                   powerSupplyStatus_response.diagnosticChecks.acFailure);
            printf("    i2cBuses:                 %d\n",
                   powerSupplyStatus_response.diagnosticChecks.i2cBuses);
            printf("    redundancy:               %d\n",
                   powerSupplyStatus_response.diagnosticChecks.redundancy);
            if (powerSupplyStatus_response.diagnosticChecksEx) 
                printf("  There are Diagnostic Checks Ex\n");
    }


    /* Try fanZoneArray call */
    byte zones[] = {1, 4};		/* Zones 1 and 4, for example */
    struct bayArray requestedZones = {
    		.size = 2,		/* Number of requested zones */
		.array = zones		/* The array of zones */
    };
    fanZoneArray_request.bayArray = requestedZones;
    if ( soap_getFanZoneArray(con, &fanZoneArray_request,
                              &fanZoneArray_response)) {
            err("failed  soap_getFanZoneArray() in main()");
    } else {
            /* Successful call...print response parameters */
            printf("\n soap_getFanZoneArray response values \n");
            while (fanZoneArray_response.fanZoneArray) {
                soap_fanZone(fanZoneArray_response.fanZoneArray, &fanZone);
                printf("  zoneNumber    = %d\n", fanZone.zoneNumber);
                printf("  redundant    = %d\n", fanZone.redundant);
                printf("  operationalStatus    = %d\n",
                       fanZone.operationalStatus);
                printf("  targetRpm    = %d\n", fanZone.targetRpm);
                printf("  targetPwm    = %d\n", fanZone.targetPwm);
                printf("  deviceBayArray values \n");
                while (fanZone.deviceBayArray) {
                     soap_deviceBayArray(fanZone.deviceBayArray, &bay);
                     printf("    bay    = %d\n", bay);
                     fanZone.deviceBayArray =
			soap_next_node(fanZone.deviceBayArray); 
                }
                printf("  fanInfoArray values \n");
                while (fanZone.fanInfoArray) {
                    soap_fanInfo(fanZone.fanInfoArray, &fanInfo);
                    printf("    fanInfo values \n");
                    printf("      bayNumber    = %d\n", fanInfo.bayNumber);
                    printf("      presence    = %d\n", fanInfo.presence);
                    printf("      name    = %s\n", fanInfo.name);
                    printf("      partNumber    = %s\n", fanInfo.partNumber);
                    printf("      sparePartNumber    = %s\n",
                           fanInfo.sparePartNumber);
                    printf("      serialNumber    = %s\n",
                           fanInfo.serialNumber);
                    printf("      powerConsumed    = %d\n",
                           fanInfo.powerConsumed);
                    printf("      fanSpeed    = %d\n", fanInfo.fanSpeed);
                    printf("      maxFanSpeed    = %d\n", fanInfo.maxFanSpeed);
                    printf("      lowLimitFanSpeed    = %d\n",
                           fanInfo.lowLimitFanSpeed);
                    printf("      operationalStatus    = %d\n",
                           fanInfo.operationalStatus);
                    printf("      Diagnostic Checks:\n");
                    printf("        internalDataError:        %d\n",
                           fanInfo.diagnosticChecks.internalDataError);
                    printf("        managementProcessorError: %d\n",
                           fanInfo.diagnosticChecks.managementProcessorError);
                    printf("        thermalWarning:           %d\n",
                           fanInfo.diagnosticChecks.thermalWarning);
                    printf("        thermalDanger:            %d\n",
                           fanInfo.diagnosticChecks.thermalDanger);
                    printf("        ioConfigurationError:     %d\n",
                           fanInfo.diagnosticChecks.ioConfigurationError);
                    printf("        devicePowerRequestError:  %d\n",
                           fanInfo.diagnosticChecks.devicePowerRequestError);
                    printf("        insufficientCooling:      %d\n",
                           fanInfo.diagnosticChecks.insufficientCooling);
                    printf("        deviceLocationError:      %d\n",
                           fanInfo.diagnosticChecks.deviceLocationError);
                    printf("        deviceFailure:            %d\n",
                           fanInfo.diagnosticChecks.deviceFailure);
                    printf("        deviceDegraded:           %d\n",
                           fanInfo.diagnosticChecks.deviceDegraded);
                    printf("        acFailure:                %d\n",
                           fanInfo.diagnosticChecks.acFailure);
                    printf("        i2cBuses:                 %d\n",
                           fanInfo.diagnosticChecks.i2cBuses);
                    printf("        redundancy:               %d\n",
                           fanInfo.diagnosticChecks.redundancy);
                    if (fanInfo.diagnosticChecksEx) {
                        printf("      There are Diagnostic Checks Ex\n");
	    		while (fanInfo.diagnosticChecksEx) {
			    soap_getDiagnosticChecksEx(
			    		fanInfo.diagnosticChecksEx,
					& diagnosticChecksEx_response);
			    printf("        %s: %d\n",
		       		   diagnosticChecksEx_response.name,
				   diagnosticChecksEx_response.value);
			    fanInfo.diagnosticChecksEx =
				    soap_next_node(fanInfo.diagnosticChecksEx);
			}
		     }
                     fanZone.fanInfoArray =
			soap_next_node(fanZone.fanInfoArray); 
                }
                fanZoneArray_response.fanZoneArray =
		     soap_next_node(fanZoneArray_response.fanZoneArray); 
            }
    }


    /* Try setEnclosureUid() */
    {
	struct setEnclosureUid setEnclosureUid;

	printf("\nTrying to turn enclosure UID on and off\n");
	setEnclosureUid.uid = UID_CMD_ON;
        if (soap_setEnclosureUid(con, &setEnclosureUid)) {
	    err("failed soap_setEnclosureUid() in main()");
	    /* Falling through here to (eventually) close the connection */
        }
	setEnclosureUid.uid = UID_CMD_OFF; /* Leave it off */
        if (soap_setEnclosureUid(con, &setEnclosureUid)) {
	    err("failed soap_setEnclosureUid() in main()");
        }
    }


    /* Try setOaUid() */
    {
        struct setOaUid setOaUid;

	printf("\nTrying to turn OA UID on and off\n");
        setOaUid.bayNumber = 1;
	setOaUid.uid = UID_CMD_ON;
        if (soap_setOaUid(con, &setOaUid)) {
	    err("failed soap_setOaUid() in main()");
	    /* Falling through here to (eventually) close the connection */
        }
	setOaUid.uid = UID_CMD_OFF;	/* Leave it off */
        if (soap_setOaUid(con, &setOaUid)) {
	    err("failed soap_setOaUid() in main()");
        }
    }


    /* Try setBladeUid() */
    {
        struct setBladeUid setBladeUid;

	printf("\nTrying to turn blade UID on and off\n");
        setBladeUid.bayNumber = 1;
	setBladeUid.uid = UID_CMD_ON;
        if (soap_setBladeUid(con, &setBladeUid)) {
	    err("failed soap_setBladeUid() in main()");
	    /* Falling through here to (eventually) close the connection */
        }
	setBladeUid.uid = UID_CMD_OFF;	/* Leave it off */
        if (soap_setBladeUid(con, &setBladeUid)) {
	    err("failed soap_setBladeUid() in main()");
        }
    }


    /* Try setInterconnectTrayUid() */
    {
        struct setInterconnectTrayUid setInterconnectTrayUid;

	printf("\nTrying to turn interconnect tray UID on and off\n");
        setInterconnectTrayUid.bayNumber = 1;
	setInterconnectTrayUid.uid = UID_CMD_ON;
        if (soap_setInterconnectTrayUid(con, &setInterconnectTrayUid)) {
	    err("failed soap_setInterconnectTrayUid() in main()");
	    /* Falling through here to (eventually) close the connection */
        }
	setInterconnectTrayUid.uid = UID_CMD_OFF; /* Leave it off */
        if (soap_setInterconnectTrayUid(con, &setInterconnectTrayUid)) {
	    err("failed soap_setInterconnectTrayUid() in main()");
        }
    }

    /* Try setLcdButtonLock call */
    printf("\nSetting LCD button lock to on and off\n");
    if (soap_setLcdButtonLock(con, HPOA_TRUE)) {
	err("failed soap_setLcdButtonLock() wth TRUE in main");
    } else {
	/* Revert back the LCD button lock */
	if (soap_setLcdButtonLock(con, HPOA_FALSE)) {
	    err("failed soap_setLcdButtonLock() with FALSE in main");
	}
   }
	

#if 1					/* Sometimes, we want to skip events */
    /* Try to subscribe for events */
    if (soap_subscribeForEvents(con, &subscribeForEvents_response)) {
	err("failed soap_subscribeForEvents() in main()");
	/* Falling through here to (eventually) close the connection */
    }
    else {
	printf("\nsubscribeForEvents response pid: %d\n",
	       subscribeForEvents_response.pid);
    }


#if 0					/* Not a safe thing to do, in general */
    {
	struct setBladePower	setBladePower_request;

	/* Try setBladePower() */
	setBladePower_request.bayNumber = 1;
	setBladePower_request.power = PRESS_AND_HOLD;
	printf("\nTrying to turn off blade %d\n",
	       setBladePower_request.bayNumber);
	if (soap_setBladePower(con, &setBladePower_request)) {
	    err("failed soap_setBladePower() in main()");
	    /* Falling through here to (eventually) close the connection */
	}
    }
#endif


#if 0					/* Don't use this all the time */
    /* Get a single event.  For this event, we'll increase the timeout
     * value to 40 seconds, to capture a heartbeat event.
     */
    old_timeout = soap_timeout(con);
    soap_timeout(con) = 40;		/* New timeout of 40 seconds */
    getEvent_request.pid = subscribeForEvents_response.pid;
    getEvent_request.waitTilEventHappens = HPOA_TRUE;
    getEvent_request.lcdEvents = HPOA_FALSE;
    printf("\nGetting a single event from the OA:\n");
    if ((ret = soap_getEvent(con, &getEvent_request, &event))) {
	if (ret == -2) {
	    err("timeout from soap_getEvent()");
	}
	else {
	    err("failed (%d) soap_getEvent() in main()", ret);
	}
	/* Falling through here to (eventually) close the connection */
    }
    else {
	printf("  Event:\n");
	printf("    event type = %d\n",  event.event);
	printf("    time stamp = %ld\n", event.eventTimeStamp);
	printf("    queue size = %d\n",  event.queueSize);
	printf("    Event details:\n");
	switch (event.event) {	/* Abbreviated event printing */
	    case EVENT_HEARTBEAT:
	    	printf("      type: HEARTBEAT\n");
		if (event.eventData.message)
		    printf("      message: %s\n",
			   event.eventData.message);
		break;
	    default:
	    	printf("      type: other\n");
		break;
	}
    }
    soap_timeout(con) = old_timeout;
#endif


#if 0					/* This is specific debug code to help
					 * isolate an issue with an 8000
					 * character buffer limit
					 */
    printf("Waiting for you to generate some events..."
	   "press enter when ready.\n");
    (void) getchar();
#endif


    /* Loop for events for a while.  Note that with the 20 second timeout
     * set above, we won't get an event the first time, but will get a
     * a timeout.  The second time, we increase the timeout to 40 seconds
     * in order to at least capture a heartbeat event.
     */
    getAllEvents_request.pid = subscribeForEvents_response.pid;
    getAllEvents_request.waitTilEventHappens = HPOA_TRUE;
    getAllEvents_request.lcdEvents = HPOA_FALSE;
    for (i = 0; i < 2; i++) {
	/* Get the next set of events */
	if (i == 1) {			/* Change timeout 2nd time through */
	    old_timeout = soap_timeout(con);
	    soap_timeout(con) = 40;	/* New timeout of 40 seconds */
	}
	printf("\nGetting an event from the OA:\n");
	if ((ret = soap_getAllEvents(con, &getAllEvents_request,
			      &getAllEvents_response))) {
	    if (ret == -2) {
		err("timeout from soap_getAllEvents()");
	    }
	    else {
		err("failed (%d) soap_getAllEvents() in main()", ret);
	    }
	    /* Falling through here to (eventually) close the connection */
	}
	else {
	    /* Successful call...print what we got from the events array.
	     * Note that not all event information is printed below.
	     */
	    if (! getAllEvents_response.eventInfoArray) {
	    	printf("  Note: getAllEvents() returned successfully, but "
		       "with no events.  (This is not an error.)\n");
	    }
	    while (getAllEvents_response.eventInfoArray) {
		soap_getEventInfo(getAllEvents_response.eventInfoArray, &event);
		printf("  Event:\n");
		printf("    event type = %d\n",  event.event);
		printf("    time stamp = %ld\n", event.eventTimeStamp);
		printf("    queue size = %d\n",  event.queueSize);
		printf("    Event details:\n");
		switch (event.event) {
		    case EVENT_HEARTBEAT:
		    	printf("      type: HEARTBEAT\n");
			if (event.eventData.message)
			    printf("      message: %s\n",
				   event.eventData.message);
			break;
		    case EVENT_BLADE_UID:
		    	printf("      type: BLADE_UID\n");
			printf("      bayNumber:         %d\n",
			       event.eventData.bladeStatus.bayNumber);
			printf("      presence:          %d\n",
			       event.eventData.bladeStatus.presence);
			printf("      operationalStatus: %d\n",
			       event.eventData.bladeStatus.operationalStatus);
			printf("      thermal:           %d\n",
			       event.eventData.bladeStatus.thermal);
			printf("      powered:           %d\n",
			       event.eventData.bladeStatus.powered);
			printf("      powerState:        %d\n",
			       event.eventData.bladeStatus.powerState);
			printf("      shutdown:          %d\n",
			       event.eventData.bladeStatus.shutdown);
			printf("      uid:               %d\n",
			       event.eventData.bladeStatus.uid);
			printf("      powerConsumed:     %d\n",
			       event.eventData.bladeStatus.powerConsumed);
			printf("      Diagnostic Checks:\n");
			printf("        internalDataError:        %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.internalDataError);
			printf("        managementProcessorError: %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.managementProcessorError);
			printf("        thermalWarning:           %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.thermalWarning);
			printf("        thermalDanger:            %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.thermalDanger);
			printf("        ioConfigurationError:     %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.ioConfigurationError);
			printf("        devicePowerRequestError:  %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.devicePowerRequestError);
			printf("        insufficientCooling:      %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.insufficientCooling);
			printf("        deviceLocationError:      %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.deviceLocationError);
			printf("        deviceFailure:            %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.deviceFailure);
			printf("        deviceDegraded:           %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.deviceDegraded);
			printf("        acFailure:                %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.acFailure);
			printf("        i2cBuses:                 %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.i2cBuses);
			printf("        redundancy:               %d\n",
			       event.eventData.bladeStatus.
				   diagnosticChecks.redundancy);
			if (event.eventData.bladeStatus.diagnosticChecksEx) {
			    printf("  There are Diagnostic Checks Ex\n");
			}
			else {
			    printf("  No Diagnostic Checks Ex\n");
			}
			break;
		    default:
		    	printf("      type: other\n");
			break;
		}
		getAllEvents_response.eventInfoArray =
			soap_next_node(getAllEvents_response.eventInfoArray);
	    }
	}
    }
    soap_timeout(con) = old_timeout;	/* Put it back to 20 seconds */


    /* Unsubscribe events.  Note that this succeeds even if the wrong pid is
     * used.  (This behavior was confirmed by ISS, and they don't consider it
     * a problem.
     */
    unSubscribeForEvents_request.pid = subscribeForEvents_response.pid;
    if (soap_unSubscribeForEvents(con, &unSubscribeForEvents_request)) {
	err("failed soap_unSubscribeForEvents() in main()");
	/* Falling through here to close the connection */
    }
    else {
	printf("\nunSubscribeForEvents call succeeded\n");
    }
#endif /* If we're skipping all event stuff */


#if 0					/* Print buffer usage number.  Only
					 * used while working on buffer sizes
					 */
    printf("Buffer usage:\n");
    printf("  Request buffer used %d out of %d\n",
	   con->req_high_water,
	   OA_SOAP_REQ_BUFFER_SIZE);
#endif


    /* Log off and close the connection, done only when thread shuts down */
    printf("\nClosing connection\n");
    soap_close(con);


#ifdef MCHECK
    muntrace();
#endif


    return(0);
}
