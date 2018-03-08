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
 *
 * This file implements support functions that are used to perform REST
 * calls and some general calls.
 *
 *      ov_rest_curl_get_request()             - Curl get request.
 *
 *      ov_rest_curl_put_request()             - Curl put request.
 *
 *      ov_rest_login()                        - Logging in to OV.
 *
 *      ov_rest_connection_init()              - Initializing connection.
 *
 *      ov_rest_init_string()                  - Initilizing string.
 *
 *      ov_rest_copy_response_buff()           - Copying response buffer.
 *
 *      ov_rest_print_json_value()             - Printing json value.
 *
 *      ov_rest_prn_json_obj()                 - Printing json object.
 *
 *      ov_rest_wrap_json_object_object_get()  - Getting json object.
 *
 *      rest_enum()                            - Perfomrs the enum string
 *                                               matching.
 *
 *      rest_get_request()                     - Curl get request.
 *
 *      rest_put_request()                     - Curl put request.
 *
 *      rest_patch_request()                   - Curl patch request.
 */

#include "sahpi_wrappers.h"
#include "ov_rest_callsupport.h"
#include "ov_rest_parser_calls.h"
/**
 * curlerr_to_ov_rest_err:
 * 		@curlErr: CURLcode 
 * Purpose:
 * 	Converts the CURLcode error to SaErrorT
 *
 * Detailed Description:
 * 	-NA
 * Return values:
 * 	Converted SaErrorT
 * */
SaErrorT curlerr_to_ov_rest_err(CURLcode curlErr)
{

        switch(curlErr){
       	        case CURLE_OPERATION_TIMEDOUT:
               	        return SA_ERR_HPI_TIMEOUT;
       	        case CURLE_FTP_ACCEPT_TIMEOUT:
                        return SA_ERR_HPI_TIMEOUT;
                default:
                        return SA_ERR_HPI_INTERNAL_ERROR;
	}

}

/**
 * ov_rest_curl_get_request:
 *      @connection: Pointer to connection structure.
 *      @chunk: Pointer to curl_slist structure.
 *      @curl: Pointer to CURL.
 *      @st: Pointer to string structure.
 *
 * Purpose:
 *      Call to CURL GET request.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                         - On Sucess.
 *      SA_ERR_HPI_INTERNAL_ERROR     - On Failure.
 *      SA_ERR_HPI_TIMEOUT            - On Reaching Timeout.
 *      SA_ERR_HPI_INVALID_SESSION    - On Invalid Session.
 **/
SaErrorT ov_rest_curl_get_request(REST_CON *connection, 
	struct curl_slist *chunk, CURL* curl, OV_STRING *st)
{
	char *Auth=NULL, *X_Auth_Token = NULL;
	char *SessionId = NULL;
	char curlErrStr[CURL_ERROR_SIZE+1];
	WRAP_ASPRINTF(&Auth,OV_REST_AUTH,connection->auth);
	WRAP_ASPRINTF(&SessionId,OV_REST_SESSIONID,connection->auth);
	chunk = curl_slist_append(chunk, OV_REST_ACCEPT);
	chunk = curl_slist_append(chunk, OV_REST_CHARSET);
	chunk = curl_slist_append(chunk, OV_REST_CONTENT_TYPE);
	chunk = curl_slist_append(chunk, OV_REST_X_API_VERSION);
	chunk = curl_slist_append(chunk, Auth);
	chunk = curl_slist_append(chunk, SessionId);
	wrap_free(Auth);
	wrap_free(SessionId);
	if(connection->xAuthToken != NULL){
		WRAP_ASPRINTF(&X_Auth_Token,OV_REST_X_AUTH_TOKEN,
				connection->xAuthToken);
		chunk = curl_slist_append(chunk, X_Auth_Token);
	}else {
		err("Sessionkey for server single sign on is invalid/NULL");
		curl_slist_free_all(chunk);
		return SA_ERR_HPI_INVALID_SESSION;
	}
	wrap_free(X_Auth_Token);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, connection->url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
			ov_rest_copy_response_buff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, st);
	curl_easy_setopt(curl, CURLOPT_USERNAME, connection->username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, connection->password);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrStr);
	CURLcode curlErr = curl_easy_perform(curl);
	if(curlErr) {
		err("\nError %s\n", curl_easy_strerror(curlErr));
		err("\nError %s\n",curlErrStr);
		curl_slist_free_all(chunk);
		wrap_free(st->ptr);
		return curlerr_to_ov_rest_err(curlErr);
	} 
	curl_slist_free_all(chunk);
	return SA_OK;
}

/**
 * ov_rest_curl_put_request:
 *      @connection: Pointer to connection structure.
 *      @chunk: Pointer to curl_slist structure.
 *      @curl: Pointer to CURL.
 *      @st: Pointer to string structure.
 *
 * Purpose:
 *      Call to CURL PUT request.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                             - On Sucess.
 *      SA_ERR_HPI_INTERNAL_ERROR         - On Failure.
 *      SA_ERR_HPI_TIMEOUT               - On Reaching Timeout value.
 **/
SaErrorT ov_rest_curl_put_request(REST_CON *connection,
			struct curl_slist *chunk, CURL* curl, 
			char*  postfields, OV_STRING *s)
{
	char *Auth=NULL;
        char curlErrStr[CURL_ERROR_SIZE+1];
	WRAP_ASPRINTF(&Auth,OV_REST_AUTH,connection->auth);
	chunk = curl_slist_append(chunk, OV_REST_ACCEPT);
	chunk = curl_slist_append(chunk, OV_REST_CHARSET);
	chunk = curl_slist_append(chunk, OV_REST_CONTENT_TYPE);
	chunk = curl_slist_append(chunk, OV_REST_X_API_VERSION);
	chunk = curl_slist_append(chunk, Auth);
        wrap_free(Auth);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, connection->url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
				ov_rest_copy_response_buff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
	curl_easy_setopt(curl, CURLOPT_USERNAME, connection->username);
	curl_easy_setopt(curl, CURLOPT_PASSWORD, connection->password);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrStr);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
	CURLcode curlErr = curl_easy_perform(curl);
	if(curlErr) {
		err("\nError %s\n", curl_easy_strerror(curlErr));
//		update_connection(connection);
		wrap_free(s->ptr);
		curl_slist_free_all(chunk);
		return curlerr_to_ov_rest_err(curlErr);
	}
	curl_slist_free_all(chunk);
	return SA_OK;
}

/**
 * ov_rest_login:
 *      @connection: Pointer to connection structure.
 *      @url: Pointer to url.
 *      @postfields: Specify data to POST.
 *
 * Purpose:
 *      Logging in to OV.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      0                             - On Sucess.
 *      SA_ERR_HPI_INVALID_SESSION    - On Invalid Session.
 **/
SaErrorT ov_rest_login(REST_CON *connection, char* postfields)
{
	OV_STRING s = {0};
	const char *temp = NULL;
	SaErrorT rv = SA_OK;
	struct curl_slist *chunk = NULL;
	/* Base URL for the service */
	curl_global_init(CURL_GLOBAL_ALL);
	/* Get a curl handle */
	CURL* curlHandle = curl_easy_init();
	json_object *jobj = NULL;

	rv = ov_rest_curl_put_request(connection, chunk, curlHandle, 
			postfields, &s);
	if(rv != SA_OK){
		CRIT("Failed to login to OV");
		return rv;
	}
	jobj = ov_rest_wrap_json_object_object_get(s.jobj, "sessionID");
	temp = json_object_get_string(jobj);
	if(temp){
		memcpy(connection->auth,temp, strlen(temp)+1);
	}else{
		ov_rest_wrap_json_object_put(s.jobj);
		wrap_free(s.ptr);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	if(connection->auth == NULL){
		err("Reply from %s contains no session ID, please check the"
				"configuration file", connection->hostname);
		ov_rest_wrap_json_object_put(s.jobj);
		wrap_free(s.ptr);
		curl_easy_cleanup(curlHandle);
		curl_global_cleanup();
		return SA_ERR_HPI_INVALID_SESSION;
	}
	ov_rest_wrap_json_object_put(s.jobj);
	/* Clean-up libcurl */
	wrap_free(s.ptr);
	curl_easy_cleanup(curlHandle);
	curl_global_cleanup();
	return SA_OK;
}

/**
 * ov_rest_connection_init:
 *      @handler: Pointer to oh_handler_state structure.
 *      @ov_handler: Pointer to ov_rest_handler structure.
 *
 * Purpose:
 *      Initializing OV connection.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK    - On Sucess.
 **/
SaErrorT ov_rest_connection_init(struct oh_handler_state *handler)
{
	SaErrorT rv = SA_OK;
	char * postfields = NULL;
	REST_CON *con = NULL;
	struct ov_rest_handler *ov_handler =
		(struct ov_rest_handler *) handler->data;

	ov_handler->discover_called_count = 0;
	con = (REST_CON *) ov_handler->connection;

	/* Get the Active OV hostname/IP address and check whether it's NULL */
	con->hostname = (char *) g_hash_table_lookup(handler->config,
			"ACTIVE_OV");

	/* Get the user_name and password from config file */
	con->username = (char *) g_hash_table_lookup(handler->config,
			"OV_User_Name");
	con->password = (char *) g_hash_table_lookup(handler->config,
			"OV_Password");
	WRAP_ASPRINTF(&con->url, OV_REST_LOGIN_URI, con->hostname);
	WRAP_ASPRINTF(&postfields, OV_REST_LOGIN_POST ,con->username,con->password, 
			"true");

	rv =  ov_rest_login(con, postfields);
	if(rv != SA_OK)
		err("Login failed. Please check the Composer connection"
				" and openhpi.conf file parameters.");
	wrap_free(con->url);
	wrap_free(postfields);
	return rv;
}

 /**
 * ov_rest_copy_response_buff:
 *      @ptr: Void pointer.
 *      @size: Unsigned integer type.
 *      @nmemb: Unsigned integer type
 *      @s: Pointer to string structure.
 *
 * Purpose:
 *      Copying response buffer.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      size*nmemb    - On Success.
 **/
size_t ov_rest_copy_response_buff(void *ptr, size_t size, size_t nmemb, 
		OV_STRING *s)
{
	size_t new_len = s->len + size * nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if(s->ptr == NULL){
		CRIT("Out of Memory");
		return 0;
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	dbg("RAW Resposonse \n%s",s->ptr);
	s->ptr[new_len] = '\0';
	s->len = new_len;
	s->jobj = json_tokener_parse(s->ptr);
        return size*nmemb;
}


/**
 * ov_rest_prn_json_obj:
 *      @key: Character pointer.
 *      @val: Pointer to json_object structure.
 *
 * Purpose:
 *      Printing the json object based on object type.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      None.
 **/
void ov_rest_prn_json_obj(char *key, struct json_object *val)
{
        enum json_type type;
        type = json_object_get_type(val);
        switch (type) {
                   case json_type_null:
                       dbg("%s = (null)",key);
                       break;
                   case json_type_boolean:
                       dbg("%s = (boolean) %s",
			key, json_object_get_boolean(val)? "true": "false");
                       break;
                   case json_type_double:
                       dbg("%s = (double) %f",key, json_object_get_double(val));
                       break;
                   case json_type_int:
                       dbg("%s = (int) %d",key, json_object_get_int(val));
                       break;
                   case json_type_string:
                       dbg("%s = (string) %s",key, json_object_get_string(val));
                       break;
                   case json_type_object:
                       dbg("Hmmm, not expecting an object. Printing and");
                       dbg("%s = (object as string) %s",key, json_object_get_string(val));
                       break;
                   case json_type_array:
                       dbg("Hmmm, not expecting array. Printing and ");
                       dbg("%s = (array as string) %s",key, json_object_get_string(val));
                       break;
                   default:
                       dbg("ERROR, not expecting %d. What is this?",type);
                       break;
       }
}

/**
 * ov_rest_wrap_json_object_object_get:
 *      @obj: Pointer to json_object.
 *      @key: Constant character pointer.
 *
 * Purpose:
 *      Extract json object key, value pair.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      json_object    - On Success.
 *      NULL           - On Failure.
 **/
json_object *ov_rest_wrap_json_object_object_get(json_object * obj, 
		const char * key)
{
	json_object * jobj_ret = NULL;
	return json_object_object_get_ex(obj,key, &jobj_ret) ? jobj_ret : NULL;
}

/**
 * ov_rest_wrap_json_object_put:
 *      @obj: Pointer to json_object.
 *
 * Purpose:
 *      decrement the reference count by one and when the reference count
 *      reaches 0 the json_object_put free the memory associated with 
 *      json_object (jobj) and ov_rest_wrap_json_object_put returns SA_OK when
 *      reference count reaches 0 and the memory is freed,if the reference 
 *      count is not 0 then return SA_ERR_HPI_ERROR.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK    	   - On refrence count is zero.
 *      SA_ERR_HPI_ERROR   - On refrence count is not zero.
 **/
SaErrorT ov_rest_wrap_json_object_put(json_object* jobj)
{	int rv = 0;
	rv = json_object_put(jobj);
	if(rv != 1){
		CRIT("Reference count not reached down to zero");
		return SA_ERR_HPI_ERROR;
	}
	return SA_OK;
}

/**
 * rest_enum
 *      @enums: Combined enum value string, as generated by the OV_REST_ENUM()
 *              macro.  Generally, this is the type name of the enum followed
 *              by "_S".
 *      @value: The string that should match one of the enum value names
 *
 * Description: Performs enum string matching, which would otherwise require a
 *              large amount of parameter parsing code.
 *
 * The general problem is that the OV returnes a large number of enum values.
 * These come across as strings, and need to be matched back to enum values.
 * There would be a large amount of string matching code to do this job.
 *
 * Instead, during the definition of each enum, a single string is also created
 * containing the string values that need to be matched.  A single call to
 * strstr() will locate the enum value in the string, and then it's position in
 * the combined enum string determines the enum value to be returned.
 *
 * Return value: 
 *      The enum value (a non-negative integer) if a match is found.
 *      -1 If there is no successful match.
 **/
int rest_enum(const char *enums, const char *value)
{
        char            *found = NULL;
        const char      *start = enums;
        int             n = 0;
        int             len = 0;

        if (! value) {                  /* Can't proceed without a string */
                err("Could not find enum (NULL value) in \"%s\"", enums);
                return(-1);
        }

        len = strlen(value);
	if (len == 0)
		return(-1);

        /* We have to search repeatedly, in case the match is just a substring
         * of an enum value.
         */
        while (start) {
                found = strstr(start, value);
                /* To match, a value must be found, it must either be at the
                 * start of the string or be preceeded by a space, and must
                 * be at the end of the string or be followed by a ','
                 */
                if ((found) &&
                    ((found == start) || (*(found - 1) == ' ')) &&
                    ((*(found + len) == ',') || (*(found + len) == '\0'))) {
                        /* Having found a match, count backwards to see which
                         * enum value should be returned.
                         */
                        while (--found >= enums) {
                                if (*found == ',')
                                        n++;
                        }
                        return(n);
                }
                if (found) {
                        /* We found something but it was a substring.  We need
                         * to search again, but starting further along in the
                         * enums string.
                         */
                        start = found + len;
                }
                else {
                        start = NULL;   /* We won't search any more */
                }
        }

        dbg("could not find enum value \"%s\" in \"%s\"", value, enums);
        return(-1);
}

/**
 * rest_get_request:
 *      @conn: Pointer to REST_CON.
 *      @response: Pointer to OV_STRING.
 *
 * Purpose:
 *      Call to CURL GET request.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                             - On Sucess.
 *      SA_ERR_HPI_INTERNAL_ERROR         - On Failure.
 **/
int  rest_get_request(REST_CON *conn, OV_STRING *response)
{
	char *auth = NULL;
	char curlErrStr[CURL_ERROR_SIZE+1];
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();

	chunk = curl_slist_append(chunk, OV_REST_ACCEPT);
	chunk = curl_slist_append(chunk, OV_REST_CHARSET);
	chunk = curl_slist_append(chunk, OV_REST_CONTENT_TYPE);
	chunk = curl_slist_append(chunk, OV_REST_X_API_VERSION);

	WRAP_ASPRINTF(&auth,"Auth: %s",conn->auth);
	chunk = curl_slist_append(chunk, auth);
	wrap_free(auth);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, conn->url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
			ov_rest_copy_response_buff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrStr);

	CURLcode curlErr = curl_easy_perform(curl);
	if (curlErr) {
		err("\nCURLcode : %s\n", curl_easy_strerror(curlErr));
		curl_slist_free_all(chunk);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}
	wrap_free(response->ptr);
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
	return SA_OK;
}

/**
 * rest_put_request:
 *      @conn: Pointer to REST_CON.
 *      @response: Pointer to OV_STRING.
 *      @postFields: Specify data to POST.
 *
 * Purpose:
 *      Call to CURL PUT request.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                             - On Sucess.
 *      SA_ERR_HPI_INTERNAL_ERROR         - On Failure.
 **/
int  rest_put_request(REST_CON *conn, OV_STRING *response, char *postFields)
{
        char *auth = NULL;
        char curlErrStr[CURL_ERROR_SIZE+1];
        struct curl_slist *chunk = NULL;
        curl_global_init(CURL_GLOBAL_ALL);
        CURL* curl = curl_easy_init();

        chunk = curl_slist_append(chunk, OV_REST_ACCEPT);
        chunk = curl_slist_append(chunk, OV_REST_CHARSET);
        chunk = curl_slist_append(chunk, OV_REST_CONTENT_TYPE);
        chunk = curl_slist_append(chunk, OV_REST_X_API_VERSION);

        WRAP_ASPRINTF(&auth,"Auth: %s",conn->auth);
        chunk = curl_slist_append(chunk, auth);
        wrap_free(auth);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        curl_easy_setopt(curl, CURLOPT_URL, conn->url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
				ov_rest_copy_response_buff);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrStr);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");

        CURLcode curlErr = curl_easy_perform(curl);
        if (curlErr) {
                err("\nCURLcode: %s\n", curl_easy_strerror(curlErr));
		curl_slist_free_all(chunk);
                return SA_ERR_HPI_INTERNAL_ERROR;
        }

	wrap_free(response->ptr);
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
        return SA_OK;
}

/**
 * rest_patch_request:
 *      @conn: Pointer to REST_CON.
 *      @response: Pointer to OV_STRING.
 *      @postFields: Specify data to POST.
 *
 * Purpose:
 *      Call to CURL PATCH request.
 *
 * Detailed Description:
 *      - NA
 *
 * Return values:
 *      SA_OK                             - On Sucess.
 *      SA_ERR_HPI_INTERNAL_ERROR         - On Failure.
 **/
int  rest_patch_request(REST_CON *conn, OV_STRING *response, char *postFields)
{
	char *auth = NULL;
	char curlErrStr[CURL_ERROR_SIZE+1];
	struct curl_slist *chunk = NULL;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();

	chunk = curl_slist_append(chunk, OV_REST_ACCEPT);
	chunk = curl_slist_append(chunk, OV_REST_CHARSET);
	chunk = curl_slist_append(chunk, OV_REST_CONTENT_TYPE);
	chunk = curl_slist_append(chunk, OV_REST_X_API_VERSION);

	WRAP_ASPRINTF(&auth,"Auth: %s",conn->auth);
	chunk = curl_slist_append(chunk, auth);
	wrap_free(auth);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(curl, CURLOPT_URL, conn->url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
			ov_rest_copy_response_buff);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrStr);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");

	CURLcode curlErr = curl_easy_perform(curl);
	if (curlErr) {
		err("\nCURLcode: %s\n", curl_easy_strerror(curlErr));
		curl_slist_free_all(chunk);
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	wrap_free(response->ptr);
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
	return SA_OK;
}
