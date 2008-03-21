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
#include <oa_soap_calls.h>
#include <oa_soap_callsupport.h>
#ifdef MCHECK
#include <mcheck.h>
#endif


/* Configuration values, to set this up for your c-class cage */
#if 1
/* These are for the OSLO cage */
#define	OA_DEST		"ccc2-ilo.telco:443"
#define	OA_USERNAME	"admin"
#define	OA_PASSWORD	"hosehead"
#else
/* These are for the GDIC cage */
#define	OA_DEST		"16.138.181.39:443"
#define	OA_USERNAME	"hpi"
#define	OA_PASSWORD	"hpi123"
#endif


int main(int argc, char *argv[])
{
    SOAP_CON			*con;
    struct getBladeInfo		bladeInfo_request;
    struct bladeInfo		bladeInfo_response;
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
	}
    }


    /* Example of printing errors from a prior SOAP call */
    if (soap_error_number(con)) {
	printf("\nPrevious call had error %d: %s\n",
	       soap_error_number(con), soap_error_string(con));
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
#if 0					/* TODO: Doesn't work yet */
			if (event.eventData.bladeStatus.diagnosticChecksEx) {
			    printf("      There are Diagnostic Checks Ex\n");
			}
			else {
			    printf("      No Diagnostic Checks Ex\n");
			}
#endif
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
