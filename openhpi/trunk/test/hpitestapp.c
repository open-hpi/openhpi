/**
 * TODO: License
 *
 * Copyright (c) 2003 Intel Corporation <andrea.l.brugger@intel.com>
 *
 *
 */

#include <stdio.h>
#include <safhpi.h>

int main(int argc, char *argv[])
{
	SaHpiVersionT 	version;
	SaErrorT 	err;

	err = saHpiInitialize(&version);

	if (err == SA_ERR_HPI_UNSUPPORTED_API)
	{
		printf("Initialize: unsupported API\n");
	}	
	else if (err == SA_OK)
	 	printf("Initialize succeeded\n");

	err = saHpiFinalize();

	if (err == SA_ERR_HPI_UNSUPPORTED_API)
	{
		printf("Finalize: unsupported API\n");
	}	
	else if (err == SA_OK)
	 	printf("Finalize succeeded\n");

	return 0;
}


