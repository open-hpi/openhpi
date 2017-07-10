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

#ifndef _OV_REST_CALLSUPPORT_H_
#define _OV_REST_CALLSUPPORT_H_
#include "ov_rest.h"
#include <curl/curl.h>
#include <json-c/json.h>

#ifdef OV_REST_CALLS_FILE
#define OV_REST_ENUM_STRING(name, ...) \
        const char name##_S[] = #__VA_ARGS__;
#else
#define OV_REST_ENUM_STRING(name, ...)
#endif
#define OV_REST_CALLS_FILE              /* Defines ENUM strings in this file */
#define OV_REST_ENUM(name, ...) \
        enum name { __VA_ARGS__ }; \
        OV_REST_ENUM_STRING(name, __VA_ARGS__)
#define WRAP_ASPRINTF(...)              \
    if (asprintf( __VA_ARGS__ ) == -1) {  \
        err("Faild to allocate memory, %s",strerror(errno));\
	abort();	\
    }

struct ovString {
  char *ptr;
  int len;
  json_object* jobj;
};
typedef struct ovString OV_STRING;

struct ovConnection {
        char* hostname;
        char* username;
        char* password;
        char auth[255];
	char serverIlo[16];
	char xAuthToken[255]; /* SessionKey for Server-Hardware iLO */
        char* url;
};

typedef struct ovConnection REST_CON;

int rest_get_request   (REST_CON *conn, OV_STRING *response);
int rest_put_request   (REST_CON *conn, OV_STRING *response, char *postField);
int rest_patch_request (REST_CON *conn, OV_STRING *response, char *postField);


SaErrorT curlerr_to_ov_rest_err(CURLcode curlErr);
int ov_rest_curl_getallevents_request(REST_CON *connection, 
		struct curl_slist *chunk, CURL* curl, OV_STRING *st);
int ov_rest_curl_get_request(REST_CON *connection, struct curl_slist *chunk,
                             CURL* curl, OV_STRING *st);
int ov_rest_curl_put_request(REST_CON * connection,struct curl_slist * chunk,
                             CURL* curl, char*  postfields, OV_STRING* s);
int ov_rest_login(REST_CON *connection, char* postfields);
char* ov_rest_get(struct oh_handler_state *oh_handler, REST_CON *connection);
SaErrorT ov_rest_connection_init(struct oh_handler_state *handler);
void ov_rest_init_string(OV_STRING *s);
size_t ov_rest_copy_response_buff(void *ptr, size_t size, size_t nmemb, 
	OV_STRING *s);
void ov_rest_print_json_value(json_object *jobj);
void ov_rest_prn_json_obj(char *key, struct json_object *val);
json_object *ov_rest_wrap_json_object_object_get(json_object *obj, 
		const char *key);
SaErrorT ov_rest_wrap_json_object_put(json_object *jobj);
int  rest_enum(const char *enums, const char *value);

#endif


