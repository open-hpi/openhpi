/*
 * (C) Copyright 2016-2017 Hewlett Packard Enterprise Development LP
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
 * Neither the name of the Hewlett Packard Enterprise, nor the names
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
 *      Mohan Devarajulu <mohan.devarajulu@hpe.com>
 *      Hemantha Beecherla <hemantha.beecherla@hpe.com>
 *      Chandrashekhar Nandi <chandrashekhar.nandi@hpe.com>
 *      Shyamala Hirepatt  <shyamala.hirepatt@hpe.com>
 */

#ifndef _OV_REST_UTILS_H
#define _OV_REST_UTILS_H

/* Include files */
#include <math.h>
#include <ctype.h>

#include "ov_rest_parser_calls.h"
#include "ov_rest.h"
#include "ov_rest_inventory.h"

/* Checks the string length that is returned by json. If it is more
 * than 255 characters, raise a CRIT error, but continue
 */
#define OV_REST_CHEK_STR_LEN(key, val) \
	{ \
		if(json_object_get_type(val) == json_type_string) \
			if(strlen(json_object_get_string(val) >= 255)) { \
				CRIT("String too long (255)"); \
				CRIT("key=%s",key); \
				CRIT("val=%s",json_object_get_string(val)); \
			} \
	}

/* Functions */
void ov_rest_update_hs_event(struct oh_handler_state *oh_handler,
                          struct oh_event *event);

struct oh_event *copy_ov_rest_event(struct oh_event *event);

SaErrorT lock_ov_rest_handler(struct ov_rest_handler *ov_handler);

SaErrorT ov_rest_check_config_parameters(GHashTable *handler_config);

SaErrorT ov_rest_delete_all_inv_info(struct oh_handler_state *oh_handler);

void ov_rest_clean_rptable(struct oh_handler_state *oh_handler);

void release_ov_rest_resources(struct enclosureStatus *enclosure);

SaErrorT ov_rest_lower_to_upper(char *src,
                                SaHpiInt32T src_len,
                                char *dest,
                                SaHpiInt32T dest_len);

void ov_rest_update_resource_status(resource_info_t *res_status,
                                    SaHpiInt32T index,
                                    char *serialNumber,
                                    SaHpiResourceIdT resource_id,
                                    resource_presence_t presence,
                                    resourceCategory_t type);

char * ov_rest_trim_whitespace(char *s);
void itostr(int x,char **s);

SaErrorT get_url_from_idr(struct oh_handler_state *, SaHpiResourceIdT, char**);
#endif
