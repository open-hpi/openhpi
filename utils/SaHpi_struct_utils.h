/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Steve Sherman <stevees@us.ibm.com>
 */

#ifndef __SAHPI_STRUCT_UTILS_H
#define __SAHPI_STRUCT_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif 

#define OPENHPI_MAX_STRING_BUFFER 512

/* Structures to string conversion routines */
const char *SaHpiManufacturerIdT2str(SaHpiManufacturerIdT  value);
SaErrorT SaHpiTimeT2str(SaHpiTimeT time, 
			char *buffer, 
			size_t buffer_size);
SaErrorT SaHpiEventStateT2str(SaHpiEventStateT event_state, 
			      SaHpiEventCategoryT event_cat,
			      char *buffer,
			      int buffer_size);
 
/* Validate structure routines */
SaHpiBoolT valid_SaHpiEventStateT4Cat(SaHpiEventStateT event_state, 
		                      SaHpiEventCategoryT event_cat);


/* Print structure routines */
#if 0
SaHpiBoolT valid_SaHpiTextBufferT(SaHpiTextBufferT value);
SaHpiBoolT valid_SaHpiTimeT(SaHpiTimeT value);
/* EventAdd and EventLogAdd ??? */
SaHpiBoolT valid_SaHpiEventT(SaHpiEventT value);
SaErrorT print_SaHpiTextBufferT(SaHpiTextBufferT text_buffer); 
#endif

#ifdef __cplusplus
}
#endif
 
#endif
