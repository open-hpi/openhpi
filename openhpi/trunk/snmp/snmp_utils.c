/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *      Renier Morales <renierm@users.sf.net>
 *	David Judkovics <djudkovi@us.ibm.com>
 *
 */

#include <glib.h>
#include <string.h>

#include <oh_error.h>
#include <snmp_utils.h>

/**
 * snmp_get
 * @ss: a handle to the snmp session needed to make an
 * snmp transaction.
 * @objid: string containing the OID entry.
 * @value: the value received from snmp will be put in this union.
 *
 * Gets a single value indicated by the objectid using snmp.
 * In the case of multiple values being returned, the type in @value will be
 * ASN_NULL (0x05). Nothing else in @value will be filled in.
 *
 * Returns: 0 if successful, <0 if there was an error.
 **/
SaErrorT snmp_get(struct snmp_session *ss, const char *objid, 
             struct snmp_value *value) 
{
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;
        
        oid anOID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        struct variable_list *vars;
	SaErrorT returncode = SA_OK;
        int status;
        
        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_GET);
        read_objid(objid, anOID, &anOID_len);
        snmp_add_null_var(pdu, anOID, anOID_len);

        /*
         * Send the Request out.
         */
        status = snmp_synch_response(ss, pdu, &response);

        /*
         * Process the response.
         */
        if (status == STAT_SUCCESS) {
		if(response->errstat == SNMP_ERR_NOERROR) {
               	 	vars = response->variables;
                	value->type = vars->type;
                	if (vars->next_variable != NULL) {
                		/* There are more values, set return type to null. */
                        	value->type = ASN_NULL;
			} else if ( !CHECK_END(vars->type)) {
				/* This is one of the exception condition */
				returncode = SA_ERR_HPI_NOT_PRESENT;
				dbg("snmp exception %d \n",vars->type);

                    	} else if ( (vars->type == ASN_INTEGER) || 
				    (vars->type == ASN_COUNTER) || 
			    	    (vars->type == ASN_UNSIGNED) ) {
                        	value->integer = *(vars->val.integer);

                	} else { 
                        	value->str_len = vars->val_len;
                        	if (value->str_len >= MAX_ASN_STR_LEN)
                                		value->str_len = MAX_ASN_STR_LEN;
                        	else value->string[value->str_len] = '\0';

                        	memcpy(value->string, vars->val.string, value->str_len);
                	}

		} else {
                        dbg("Error in packet %s\nReason: %s\n",
                               	 objid, snmp_errstring(response->errstat));
			returncode = errstat2hpi(response->errstat);
		}

        } else {
                value->type = (u_char)0x00; 
                snmp_sess_perror("snmpget", ss);
		returncode = snmpstat2hpi(status);
        }

        /* Clean up: free the response */
        if (response) snmp_free_pdu(response);

        return (returncode);
}

/**
 * snmp_set
 * @ss: a handle to the snmp session needed to make an snmp transaction.
 * @objid: string containing the OID to set.
 * @value: the value to set the oid with.
 *
 * Sets a value where indicated by the objectid
 * using snmp.
 *
 * Returns: 0 if Success, less than 0 if Failure.
 **/
SaErrorT snmp_set(
        struct snmp_session *ss,
        char *objid,
        struct snmp_value value) 
{
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;

        oid anOID[MAX_OID_LEN];
        size_t anOID_len = MAX_OID_LEN;
        void *dataptr = NULL;
        int status = 0;
	SaErrorT rtncode = 0;

        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_SET);
        read_objid(objid, anOID, &anOID_len);

        rtncode = 0; /* Default - All is OK */

        switch (value.type)
        {
                case ASN_INTEGER:
		case ASN_UNSIGNED:
                case ASN_COUNTER:
			dataptr = &value.integer;
                        break;
                case ASN_OCTET_STR:
			dataptr = value.string;
                        break;
                default:
                        rtncode = SA_ERR_HPI_INVALID_PARAMS;
                        dbg("datatype %c not yet supported by snmp_set()\n", value.type);
                        break;
        }

        if (rtncode == 0) {

		/*
		 * Set the data to send out
		 */
                /* Old code - int rc = snmp_add_var(pdu, objid, objid_len, datatype, dataptr)      */
		/*            was missing checking for rc, so there was no OID and no data was     */
		/*            included in the package.                                             */
		/*            Since snmp_add_var() converts input data to string then call         */
		/*            snmp_pdu_add_variable(), we stick with add_variable() for efficiency */
		snmp_pdu_add_variable(pdu, anOID, anOID_len, value.type, dataptr, value.str_len);

        	/*
         	* Send the Request out.
         	*/
        	status = snmp_synch_response(ss, pdu, &response);

        	/*
         	* Process the response.
         	*/
                if (status == STAT_SUCCESS) 
                                rtncode = errstat2hpi(response->errstat);
                else {
			snmp_sess_perror("snmpset", ss);
			rtncode = snmpstat2hpi(status);;
		}
		
        	/* Clean up: free the response */
        	if (response) snmp_free_pdu(response);
	}

        return rtncode;
}

/**
 * snmp_get2: Gets a single value indicated by the objectid
 * using snmp.
 * @handle: a handle to the snmp session needed to make an
 * snmp transaction.
 * @objid: string containing the OID entry.
 * @value: the value received from snmp will be put in this union.
 *
 * In the case of multiple values being returned, the type in 'value' will be
 * ASN_NULL (0x05). Nothing else in 'value' will be filled in.
 * Use snmp_get_all for doing gets that return multiple values.
 *
 * Return value: Returns 0 if successful, <0 if there was an error.
 **/
SaErrorT snmp_get2(struct snmp_session *ss, 
	           oid *objid,
	           size_t objid_len,
                   struct snmp_value *value) 
{
        struct snmp_pdu *pdu;
        struct snmp_pdu *response;
        
        struct variable_list *vars;
	SaErrorT returncode = SA_OK;
	int i;
        int status;
        
        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_GET);

        snmp_add_null_var(pdu, objid, objid_len);

        /*
         * Send the Request out.
         */
        status = snmp_synch_response(ss, pdu, &response);

        /*
         * Process the response.
         */
        if (status == STAT_SUCCESS) {
		if (response->errstat == SNMP_ERR_NOERROR) {

	               vars = response->variables;
 	               value->type = vars->type;

	                if (vars->next_variable != NULL) {
				/* If there are more values, set return type to null. */
       	                 	value->type = ASN_NULL;
			} else if ( !CHECK_END(vars->type)) {
				/* This is one of the exception condition */
				returncode = SA_ERR_HPI_NOT_PRESENT;
				dbg("snmp exception %d \n",vars->type);

                    	} else if ( (vars->type == ASN_INTEGER) || 
				    (vars->type == ASN_COUNTER) || 
			    	    (vars->type == ASN_UNSIGNED) ) {

				value->integer = *(vars->val.integer);

                	} else {

                        	value->str_len = vars->val_len;

                        	if (value->str_len >= MAX_ASN_STR_LEN)
                                		value->str_len = MAX_ASN_STR_LEN;

                        	else value->string[value->str_len] = '\0';
                        
                        	memcpy(value->string, vars->val.string, value->str_len);
                       
                	}

			/* display data */
#ifdef DEBUG
			if (CHECK_END(vars->type)) { 
				fprintf(stderr, "*** snmp_get2 ******************************************\n");
				fprint_variable(stderr, 
						vars->name, 
						vars->name_length, 
						vars);
				fprintf(stderr, "********************************************************\n");
			}  else {
				dbg("snmp_get2(): No idea.Unknown Type: %X", vars->type);
				fprint_variable(stderr, 
						vars->name, 
						vars->name_length, 
						vars);
			}		
#endif
		} else {
                       dbg("Error, Reason: %s", 
				snmp_errstring(response->errstat));
			fprintf(stderr, "objid: ");
			for(i = 0; i<objid_len; i++ )
				fprintf(stderr, "%d.", (int)objid[i]);
			fprintf(stderr, "\n");
			returncode = errstat2hpi(response->errstat);		}
        } else {
		snmp_sess_perror("snmpget", ss);
		returncode = snmpstat2hpi(status);

        }

        /* Clean up: free the response */
	sc_free_pdu(&response); 

        return (returncode);
}


/**
 * snmp_set2: Gets a single value indicated by the objectid
 * using snmp.
 * @handle: a handle to the snmp session needed to make an
 * snmp transaction.
 * @objid: string containing the OID entry.
 * @value: the value received from snmp will be put in this union.
 *
 * In the case of multiple values being returned, the type in 'value' will be
 * ASN_NULL (0x05). Nothing else in 'value' will be filled in.
 * Use snmp_get_all for doing gets that return multiple values.
 *
 * Return value: Returns 0 if successful, -1 if there was an error.
 **/
SaErrorT snmp_set2(struct snmp_session *ss, 
	           oid *objid,
	           size_t objid_len,
                   struct snmp_value *value) 
{
	struct snmp_pdu *pdu;
	struct snmp_pdu *response;
        struct variable_list *vars;

        void *dataptr = NULL;
        int status = 0;
	SaErrorT rtncode = SA_OK; /* Default - All is OK */

        /*
         * Create the PDU for the data for our request.
         */
        pdu = snmp_pdu_create(SNMP_MSG_SET);

        switch (value->type)
        {
                case ASN_INTEGER:
		case ASN_UNSIGNED:
                case ASN_COUNTER:
			dataptr = &value->integer;
                        break;
                case ASN_OCTET_STR:
			dataptr = value->string;
                        break;
                default:
                        rtncode = SA_ERR_HPI_INVALID_PARAMS;
                        dbg("datatype %c not yet supported by snmp_set2()", 
				value->type);
                        break;
        }

        if (rtncode == SA_OK) {

		/*
		 * Set the data to send out
		 */
                /* Old code - snmp_add_var(pdu, objid, objid_len, datatype, dataptr); */
                //int retcode = snmp_add_var(pdu, objid, objid_len, datatype, dataptr);
		snmp_pdu_add_variable(pdu, objid, objid_len, value->type, dataptr, value->str_len);

        	/*
         	* Send the Request out.
         	*/
        	status = snmp_synch_response(ss, pdu, &response);
        	/*
         	* Process the response.
         	*/
		if (status == STAT_SUCCESS) {
               	 	vars = response->variables;
			if (response->errstat == SNMP_ERR_NOERROR) {
				/* display data */
#ifdef DEBUG
				fprintf(stderr, "*** snmp_set2 ******************************************\n");
					if (CHECK_END(response->variables->type)) {
						fprint_variable(stderr, 
								response->variables->name,
								response->variables->name_length,
								response->variables);  
					}  else 
						fprintf(stderr, "snmp_set2(): No idea.\n");
				fprintf(stderr, "********************************************************\n");
#endif
				if (!CHECK_END(response->variables->type)) {
                                	/* This is one of the exception condition */
				        rtncode = SA_ERR_HPI_NOT_PRESENT;
                                	dbg("snmp exception %d \n",vars->type);
				}
			} else {
                        	dbg("snmp_set2: Error in packet, Reason: %s",
						snmp_errstring(response->errstat));
                                rtncode = errstat2hpi(response->errstat);

			}                
        	} else {
                       	snmp_sess_perror("snmpset", ss);
                       	rtncode = snmpstat2hpi(status);

        	}

        	/* Clean up: free the response */
        	if (response) snmp_free_pdu(response);
	
	}

        return rtncode;

}

SaErrorT snmp_getn_bulk( struct snmp_session *ss, 
		    oid *bulk_objid, 
		    size_t bulk_objid_len,
		    struct snmp_pdu *bulk_pdu, 
		    struct snmp_pdu **bulk_response,
		    int num_repetitions )
{
	int status;
	SaErrorT rtncode = SA_OK;
//        struct variable_list *vars;

	bulk_pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
 	
	bulk_pdu->non_repeaters = 0; 
	
	bulk_pdu->max_repetitions = num_repetitions;

	snmp_add_null_var(bulk_pdu, bulk_objid, bulk_objid_len);
	
	/* Send the Request out.*/
	status = snmp_synch_response(ss, bulk_pdu, bulk_response);

	/*
	 * Process the response.
	*/
#if 0
	if (status == STAT_SUCCESS) {
		vars = (*bulk_response)->variables;
		if ((*bulk_response)->errstat == SNMP_ERR_NOERROR) {
			if (!CHECH_END(vars->type)) {
                                /* This is one of the exception condition */
				rtncode = SA_ERR_HPI_NOT_PRESENT;
				dbg("snmp exception %d \n",vars->type);
			}
		} else {
			fprintf(stderr, "Error in packet %s\nReason: %s\n",
					(char *)bulk_objid, snmp_errstring((*bulk_response)->errstat));
			if ((*bulk_response)->errstat == SNMP_ERR_NOSUCHNAME)
					(*bulk_response)->errstat = SNMP_NOSUCHOBJECT;
			rtncode = (SaErrorT) (SA_ERR_SNMP_BASE - (*bulk_response)->errstat);
		}
	} else {
		snmp_sess_perror("snmpset", ss);
		rtncode = (SaErrorT) (SA_ERR_SNMP_BASE - status);
	}
#endif
	return(rtncode);

}

void sc_free_pdu(struct snmp_pdu **p) 
{
	if (*p) {
		snmp_free_pdu(*p);
		*p = NULL;
	}
}

SaErrorT errstat2hpi(long pdu_errstat) 
{
	SaErrorT hpicode = SA_OK;
	
	switch(pdu_errstat) {
		case SNMP_ERR_NOERROR:
			hpicode = SA_OK;
			break;
		case SNMP_ERR_TOOBIG:
		case SNMP_ERR_BADVALUE:
		case SNMP_ERR_WRONGTYPE:
		case SNMP_ERR_WRONGLENGTH:
		case SNMP_ERR_WRONGENCODING:
		case SNMP_ERR_WRONGVALUE:
		case SNMP_ERR_COMMITFAILED:
		case SNMP_ERR_UNDOFAILED:
		case SNMP_ERR_INCONSISTENTVALUE:
			hpicode = SA_ERR_HPI_INVALID_DATA;
			break;
		case SNMP_ERR_READONLY:
		case SNMP_ERR_NOTWRITABLE:
			hpicode = SA_ERR_HPI_READ_ONLY;
			break;		
		case SNMP_ERR_NOACCESS:
		case SNMP_ERR_AUTHORIZATIONERROR:
		case SNMP_ERR_INCONSISTENTNAME:
		case SNMP_ERR_NOSUCHNAME:
		case SNMP_ERR_NOCREATION:
			hpicode = SA_ERR_HPI_INVALID_PARAMS;
			break;
		case SNMP_ERR_RESOURCEUNAVAILABLE:
			hpicode = SA_ERR_HPI_OUT_OF_SPACE;
			break;
		case SNMP_ERR_GENERR:
		default:
			hpicode = SA_ERR_HPI_UNKNOWN;
			break;		
	}
		
	return(hpicode);
}

SaErrorT snmpstat2hpi(int snmpstat)
{
	SaErrorT hpicode = SA_OK;
	
	switch(snmpstat)
	{
		case STAT_SUCCESS:
			hpicode = SA_OK;
			break;
		case STAT_TIMEOUT:
			hpicode = SA_ERR_HPI_TIMEOUT;
			break;
		case STAT_ERROR:
		default:
			hpicode = SA_ERR_HPI_UNKNOWN;
			break;
	}
	
	return(hpicode);

}
