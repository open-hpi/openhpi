#include <stdio.h>
#include <SaHpi.h>

#define SA_ERR_HPI_ERROR_STR 		"SA_ERR_HPI_ERROR"
#define SA_ERR_HPI_UNSUPPORTED_API_STR	"SA_ERR_HPI_UNSUPPORTED_API"
#define SA_ERR_UNSUPPORTED_API_STR 	"SA_ERR_UNSUPPORTED_API"
#define SA_ERR_HPI_BUSY_STR		"SA_ERR_HPI_BUSY"
#define SA_ERR_HPI_INVALID_STR		"SA_ERR_HPI_INVALID"
#define  SA_ERR_HPI_INVALID_CMD_STR 	"SA_ERR_HPI_INVALID_CMD"
#define SA_ERR_HPI_TIMEOUT_STR 		"SA_ERR_HPI_TIMEOUT"
#define SA_ERR_HPI_OUT_OF_SPACE_STR 	"SA_ERR_HPI_OUT_OF_SPACE"
#define SA_ERR_HPI_DATA_TRUNCATED_STR 	"SA_ERR_HPI_DATA_TRUNCATED"
#define SA_ERR_HPI_DATA_LEN_INVALID_STR	"SA_ERR_HPI_DATA_LEN_INVALID"
#define SA_ERR_HPI_DATA_EX_LIMITS_STR	"SA_ERR_HPI_DATA_EX_LIMITS"
#define SA_ERR_HPI_INVALID_PARAMS_STR 	"SA_ERR_HPI_INVALID_PARAMS"
#define SA_ERR_HPI_INVALID_DATA_STR	"SA_ERR_HPI_INVALID_DATA"
#define SA_ERR_HPI_NOT_PRESENT_STR	"SA_ERR_HPI_NOT_PRESENT"
#define SA_ERR_HPI_INVALID_DATA_FIELD_STR 	"SA_ERR_HPI_INVALID_DATA_FIELD"
#define SA_ERR_HPI_INVALID_SENSOR_CMD_STR 	"SA_ERR_HPI_INVALID_SENSOR_CMD"
#define SA_ERR_HPI_NO_RESPONSE_STR	"SA_ERR_HPI_NO_RESPONSE"
#define SA_ERR_HPI_DUPLICATE_STR	"SA_ERR_HPI_DUPLICATE"
#define SA_ERR_HPI_INITIALIZING_STR	"SA_ERR_HPI_INITIALIZING"
#define SA_ERR_HPI_UNKNOWN_STR		"SA_ERR_HPI_UNKNOWN"
#define SA_ERR_HPI_INVALID_SESSION_STR	"SA_ERR_HPI_INVALID_SESSION"
#define SA_ERR_HPI_INVALID_RESOURCE_STR	"SA_ERR_HPI_INVALID_RESOURCE"
#define SA_ERR_HPI_INVALID_REQUEST_STR	"SA_ERR_HPI_INVALID_REQUEST"
#define SA_ERR_HPI_ENTITY_NOT_PRESENT_STR	"SA_ERR_HPI_ENTITY_NOT_PRESENT"
#define SA_ERR_HPI_UNINITIALIZED_STR	"SA_ERR_HPI_UNINITIALIZED"

int discover_resources(SaHpiDomainIdT domainId);
void display_resource_entry(SaHpiRptEntryT rptEntry);
const char * get_error_string(SaErrorT error);

int main(int argc, char *argv[])
{
	SaHpiVersionT 	version;
	SaErrorT 	err;

	err = saHpiInitialize(&version);

	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiInitialize: %s\n", get_error_string(err));
		return 0;
	}

	discover_resources(SAHPI_DEFAULT_DOMAIN_ID);	

	err = saHpiFinalize();

	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiFinalize: %s\n", get_error_string(err));
		return 0;
	}

	return 0;
}

int discover_resources(SaHpiDomainIdT domainId)
{
	SaErrorT 	err;
	SaHpiSessionIdT sessionId;
	SaHpiRptInfoT	rptInfo;
	SaHpiRptInfoT	rptInfoAfter;
	SaHpiEntryIdT	entryId;
	SaHpiEntryIdT	nextEntryId;
	SaHpiRptEntryT	rptEntry;

  	err = saHpiSessionOpen(domainId, &sessionId, NULL);

	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiSessionOpen: %s\n", get_error_string(err));
		return -1;
	}

	/* force regeneration of the RPT */
 	err = saHpiResourcesDiscover(sessionId);
	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiResourcesDiscover: %s\n", get_error_string(err));
		return -1;
	}

	/* grab copy of the update counter before traversing RPT */
	err = saHpiRptInfoGet(sessionId, &rptInfo);
	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiRptInfoGet: %s\n", get_error_string(err));
		return -1;
	}

	/* Get first entry */
	err = saHpiRptEntryGet(sessionId, SAHPI_FIRST_ENTRY,
			       	&nextEntryId, &rptEntry);
	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiRptEntryGet: %s\n", get_error_string(err));
		return -1;
	}

	/* iterate through all entries */
	while (nextEntryId != SAHPI_LAST_ENTRY)
	{
		entryId = nextEntryId;
		err = saHpiRptEntryGet(sessionId, entryId,
			       	&nextEntryId, &rptEntry);
		if (err != SA_OK) 
		{
			fprintf(stderr, "saHpiRptEntryGet: %s\n", get_error_string(err));
			return -1;
		}

		display_resource_entry(rptEntry);

		/* if the resource is also a domain, then 
		 * traverse its RPT */	
		if (rptEntry.ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN)
		{
			discover_resources(rptEntry.DomainId);			
		}
	}

	/* get a copy of update counter to see if we missed anything */
	err = saHpiRptInfoGet(sessionId, &rptInfoAfter);
	if (err != SA_OK) 
	{
		fprintf(stderr, "saHpiRptInfoGet: %s\n", get_error_string(err));
		return -1;
	}

	if (rptInfoAfter.UpdateCount != rptInfo.UpdateCount)
	{
		fprintf(stderr, "RPT table changed while obtaining resource info\n");
	}

	return 0;
}

void display_resource_entry(SaHpiRptEntryT rptEntry)
{
	fprintf(stderr, "Finish implementation of display_resource_entry\n");
}

const char * get_error_string(SaErrorT error)
{
	switch(error)
	{
		case SA_ERR_HPI_ERROR:
			return SA_ERR_HPI_ERROR_STR;
		case SA_ERR_HPI_UNSUPPORTED_API:
			return SA_ERR_UNSUPPORTED_API_STR;
		case SA_ERR_HPI_BUSY:
			return SA_ERR_HPI_BUSY_STR;
		case SA_ERR_HPI_INVALID:
			return SA_ERR_HPI_INVALID_STR;
		case SA_ERR_HPI_INVALID_CMD:
			return SA_ERR_HPI_INVALID_CMD_STR;
		case SA_ERR_HPI_TIMEOUT:
			return SA_ERR_HPI_TIMEOUT_STR;
		case SA_ERR_HPI_OUT_OF_SPACE:
			return SA_ERR_HPI_OUT_OF_SPACE_STR;
		case SA_ERR_HPI_DATA_TRUNCATED:
			return SA_ERR_HPI_DATA_TRUNCATED_STR;
		case SA_ERR_HPI_DATA_LEN_INVALID:
			return SA_ERR_HPI_DATA_LEN_INVALID_STR;
		case SA_ERR_HPI_DATA_EX_LIMITS:
			return SA_ERR_HPI_DATA_EX_LIMITS_STR;
		case SA_ERR_HPI_INVALID_PARAMS:
			return SA_ERR_HPI_INVALID_PARAMS_STR;
		case SA_ERR_HPI_INVALID_DATA:
			return SA_ERR_HPI_INVALID_DATA_STR;
		case SA_ERR_HPI_NOT_PRESENT:
			return SA_ERR_HPI_NOT_PRESENT_STR;
		case SA_ERR_HPI_INVALID_DATA_FIELD:
			return SA_ERR_HPI_INVALID_DATA_FIELD_STR;
		case SA_ERR_HPI_INVALID_SENSOR_CMD:
			return SA_ERR_HPI_INVALID_SENSOR_CMD_STR;
		case SA_ERR_HPI_NO_RESPONSE:
			return SA_ERR_HPI_NO_RESPONSE_STR;
		case SA_ERR_HPI_DUPLICATE:
			return SA_ERR_HPI_DUPLICATE_STR;
		case SA_ERR_HPI_INITIALIZING:
			return SA_ERR_HPI_INITIALIZING_STR;
		case SA_ERR_HPI_UNKNOWN:
			return SA_ERR_HPI_UNKNOWN_STR;
		case SA_ERR_HPI_INVALID_SESSION:
			return SA_ERR_HPI_INVALID_SESSION_STR;
		case SA_ERR_HPI_INVALID_RESOURCE:
			return SA_ERR_HPI_INVALID_RESOURCE_STR;
		case SA_ERR_HPI_INVALID_REQUEST:
			return SA_ERR_HPI_INVALID_REQUEST_STR;
		case SA_ERR_HPI_ENTITY_NOT_PRESENT:
			return SA_ERR_HPI_ENTITY_NOT_PRESENT_STR;
		case SA_ERR_HPI_UNINITIALIZED:
			return SA_ERR_HPI_UNINITIALIZED_STR;
		default:
			return "(invalid error code)";
	}
	return "\0";
}


