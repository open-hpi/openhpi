#include <stdlib.h>
#include <stdio.h>
#include <SaHpi.h>


#define warn(str) fprintf(stderr,"%s: " str "\n", __FUNCTION__)
#define error(str, e) fprintf(stderr,str ": %s\n", get_error_string(e))

SaErrorT discover_domain(SaHpiDomainIdT);
const char * get_error_string(SaErrorT);
void display_entity_path(SaHpiEntityPathT *);
void display_entity_capabilities(SaHpiCapabilitiesT);
const char * severity2str(SaHpiSeverityT);

int main(int argc, char *argv[])
{
	SaErrorT err;
	SaHpiVersionT version;

	err = saHpiInitialize(&version);
	if (SA_OK != err) {
		error("saHpiInitialize", err);
		exit(-1);
	}
	
	err = discover_domain(SAHPI_DEFAULT_DOMAIN_ID);
	if (SA_OK != err) {
		warn("an error was encountered, results may be incomplete");
	}

	err = saHpiFinalize();
	if (SA_OK != err) {
		error("saHpiFinalize", err);
		exit(-1);
	}

	return 0;
}

SaErrorT discover_domain(SaHpiDomainIdT domain_id)
{
	SaErrorT 	err;
	SaHpiSessionIdT session_id;
	SaHpiRptInfoT	rpt_info_before;
	SaHpiRptInfoT	rpt_info_after;
	SaHpiEntryIdT	current;
	SaHpiEntryIdT	next = SAHPI_FIRST_ENTRY;
	SaHpiRptEntryT	entry;
	
	if (next == SAHPI_LAST_ENTRY)
		return SA_OK;

	/* every domain requires a new session */
  	err = saHpiSessionOpen(domain_id, &session_id, NULL);
	if (SA_OK != err) {
		error("saHpiSessionOpen", err);
		return err;
	}

	/* force regeneration of the RPT */
 	err = saHpiResourcesDiscover(session_id);
	if (SA_OK != err) {
		error("saHpiResourcesDiscover", err);
		return err;
	}

	/* grab copy of the update counter before traversing RPT */
	err = saHpiRptInfoGet(session_id, &rpt_info_before);
	if (SA_OK != err) {
		error("saHpiRptInfoGet", err);
		return err;
	}

	do {
		current = next;
		err = saHpiRptEntryGet(session_id, current, &next, &entry);
		if (SA_OK != err) {
			error("saHpiRptEntryGet", err);
			return err;
		}

		printf("%s\n", (char *)entry.ResourceTag.Data);
		printf("Entry ID: %x\n", (int) entry.EntryId);
		printf("Resource ID: %x\n", (int) entry.ResourceId);
		printf("Domain ID: %x\n", (int) entry.DomainId);
		printf("Revision: %c\n", entry.ResourceInfo.ResourceRev);
		printf("Version: %c\n", entry.ResourceInfo.SpecificVer);
		printf("Severity: %s\n",severity2str(entry.ResourceSeverity));
		display_entity_path(&entry.ResourceEntity);
		display_entity_capabilities(entry.ResourceCapabilities);
		printf("\n");

		/* if the resource is also a domain, then 
		 * traverse its RPT */	
		if (entry.ResourceCapabilities & SAHPI_CAPABILITY_DOMAIN) {
			err = discover_domain(entry.DomainId);
			if (SA_OK != err)
				return err;
		}
	} while (next != SAHPI_LAST_ENTRY);

	/* get a copy of update counter to see if we missed anything */
	err = saHpiRptInfoGet(session_id, &rpt_info_after);
	if (SA_OK != err) {
		error("saHpiRptInfoGet", err);
		return err;
	}

	if (rpt_info_after.UpdateCount != rpt_info_before.UpdateCount) {
		err = SA_ERR_HPI_ERROR;
		error("RPT table changed while obtaining resource info", err);
		return err;
	}

	return SA_OK;
}

void display_entity_path(SaHpiEntityPathT *path)
{

}

void display_entity_capabilities(SaHpiCapabilitiesT caps)
{

}

const char * severity2str(SaHpiSeverityT severity)
{
	switch (severity) {
	case SAHPI_CRITICAL:
		return "CRITICAL";
	case SAHPI_MAJOR:
		return "MAJOR";
	case SAHPI_MINOR:
		return "MINOR";
	case SAHPI_INFORMATIONAL:
		return "INFORMATIONAL";	       
	case SAHPI_OK:
		return "OK";
	case SAHPI_DEBUG:
		return "DEBUG";
	default:
		return "UNKNOWN SEVERITY";
	}
}

const char * get_error_string(SaErrorT error)
{
	switch(error) {
		case SA_ERR_HPI_ERROR:
			return "SA_ERR_HPI_ERROR";
		case SA_ERR_HPI_UNSUPPORTED_API:
			return "SA_ERR_UNSUPPORTED_API";
		case SA_ERR_HPI_BUSY:
			return "SA_ERR_HPI_BUSY";
		case SA_ERR_HPI_INVALID:
			return "SA_ERR_HPI_INVALID";
		case SA_ERR_HPI_INVALID_CMD:
			return "SA_ERR_HPI_INVALID_CMD";
		case SA_ERR_HPI_TIMEOUT:
			return "SA_ERR_HPI_TIMEOUT";
		case SA_ERR_HPI_OUT_OF_SPACE:
			return "SA_ERR_HPI_OUT_OF_SPACE";
		case SA_ERR_HPI_DATA_TRUNCATED:
			return "SA_ERR_HPI_DATA_TRUNCATED";
		case SA_ERR_HPI_DATA_LEN_INVALID:
			return "SA_ERR_HPI_DATA_LEN_INVALID";
		case SA_ERR_HPI_DATA_EX_LIMITS:
			return "SA_ERR_HPI_DATA_EX_LIMITS";
		case SA_ERR_HPI_INVALID_PARAMS:
			return "SA_ERR_HPI_INVALID_PARAMS";
		case SA_ERR_HPI_INVALID_DATA:
			return "SA_ERR_HPI_INVALID_DATA";
		case SA_ERR_HPI_NOT_PRESENT:
			return "SA_ERR_HPI_NOT_PRESENT";
		case SA_ERR_HPI_INVALID_DATA_FIELD:
			return "SA_ERR_HPI_INVALID_DATA_FIELD";
		case SA_ERR_HPI_INVALID_SENSOR_CMD:
			return "SA_ERR_HPI_INVALID_SENSOR_CMD";
		case SA_ERR_HPI_NO_RESPONSE:
			return "SA_ERR_HPI_NO_RESPONSE";
		case SA_ERR_HPI_DUPLICATE:
			return "SA_ERR_HPI_DUPLICATE";
		case SA_ERR_HPI_INITIALIZING:
			return "SA_ERR_HPI_INITIALIZING";
		case SA_ERR_HPI_UNKNOWN:
			return "SA_ERR_HPI_UNKNOWN";
		case SA_ERR_HPI_INVALID_SESSION:
			return "SA_ERR_HPI_INVALID_SESSION";
		case SA_ERR_HPI_INVALID_RESOURCE:
			return "SA_ERR_HPI_INVALID_RESOURCE";
		case SA_ERR_HPI_INVALID_REQUEST:
			return "SA_ERR_HPI_INVALID_REQUEST";
		case SA_ERR_HPI_ENTITY_NOT_PRESENT:
			return "SA_ERR_HPI_ENTITY_NOT_PRESENT";
		case SA_ERR_HPI_UNINITIALIZED:
			return "SA_ERR_HPI_UNINITIALIZED";
		default:
			return "(invalid error code)";
	}
	return "\0";
}
