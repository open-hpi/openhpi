/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Louis Zhuang <louis.zhuang@linux.intel.com>
 */

#include "ipmi.h"
void
posix_vlog(char *format, enum ipmi_log_type_e log_type, va_list ap)
{
     int do_nl = 1;
     char *dstr = getenv("OPENHPI_DEBUG");
     
     if (((log_type <= IPMI_LOG_WARNING) || (log_type == IPMI_LOG_ERR_INFO))
		     && (!dstr || !strcmp("YES",getenv("OPENHPI_DEBUG"))))
	     return;
    switch(log_type)
    {
	case IPMI_LOG_INFO:
	    printf("INFO: ");
	    do_nl = 0;
	    break;

	case IPMI_LOG_WARNING:
	    printf("WARN: ");
	    do_nl = 0;
	    break;

	case IPMI_LOG_SEVERE:
	    printf("SEVR: ");
	    do_nl = 0;
	    break;

	case IPMI_LOG_FATAL:
	    printf("FATL: ");
	    do_nl = 0;
	    break;

	case IPMI_LOG_ERR_INFO:
	    printf("EINF: ");
	    do_nl = 1;
	    break;

	case IPMI_LOG_DEBUG_START:
	    /* FALLTHROUGH */
	case IPMI_LOG_DEBUG:
	    printf("DEBG: ");
	    break;

	case IPMI_LOG_DEBUG_CONT:
	    /* FALLTHROUGH */
	case IPMI_LOG_DEBUG_END:
	    break;
    }

    vprintf(format, ap);

    if (do_nl)
	printf("\n");
}
