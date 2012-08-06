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
 *     Shuah Khan <shuah.khan@hp.com>
 *     Richard White <richard.white@hp.com>
 */

/*****************************
 * This file implements the iLO2 RIBCL plug-in iLO2 SSL connection
 * management functionality that is not part of the infrastructure.
 * It supports the following function:
 *
 * ilo2_ribcl_ssl_send_command	- Send a command, reading the response
*****************************/

/* TODO: Evaluate whether a shorter timeout value should be used.  This
 * affects calls to oh_ssl_connect(), oh_ssl_read(), and oh_ssl_write().
 */

/* Header files */
#include <oh_ssl.h>
#include <ilo2_ribcl_ssl.h>
#include <ilo2_ribcl_xml.h>
#include <ilo2_ribcl_cmnds.h>


/**
 * ilo2_ribcl_ssl_send_command
 * @ir_handler: Ptr to this instance's private handler.
 * @cmnd_buf: Ptr to buffer containing the RIBCL command(s) to send.
 * @resp_buf: Ptr to buffer into which the response will be written.
 * @resp_buf_size: Size (in bytes) of the response buffer.
 *
 * This routine will send the contents of the RIBCL command buffer cmnd_buf
 * to the iLO2 addressed by the plugin private handler ir_handler. The response
 * from iLO2 will be stored in the null terminated character buffer pointed
 * to by resp_buf.
 *
 * Return values:
 * 0 success.
 * -1 failure.
 **/
int ilo2_ribcl_ssl_send_command( ilo2_ribcl_handler_t *ir_handler,
				 char *cmnd_buf, char *resp_buf, int resp_size)
{
	void *ssl_handler = NULL;
	int in_index;
	int ret;
	int ilo_header_len;
	char *hostname = NULL;
	char cmnd_bufsize[ILO2_RIBCL_CMD_MAX_LEN];

	/* Zero out the response buffer */
	memset( resp_buf, 0, resp_size);
	ssl_handler = oh_ssl_connect( ir_handler->ilo2_hostport,
				ir_handler->ssl_ctx, 0);
	if( ssl_handler == NULL){
		err("ilo2_ribcl_ssl_send_command(): "
		    "oh_ssl_connect returned NULL.");
		return( -1);
	}
	/* Send the XML header. iLO2 requires the header to be sent ahead 
	   separately from the buffer containing the command. */
	/*
	 * the following is added to send different header info
	 * for iLO2 and iLO3. based on the ilo_type, header is 
	 * choosen, if the ilo_type is 0(zero ). then a test header
	 * is sent to identify if that is ilo2 or ilo3.
	 */
	memset(cmnd_bufsize, '\0', ILO2_RIBCL_CMD_MAX_LEN);
	switch(ir_handler->ilo_type)
	{
		case ILO:
		case ILO2:
		        ret = oh_ssl_write(ssl_handler, ILO2_RIBCL_XML_HDR,
						sizeof(ILO2_RIBCL_XML_HDR), 0);
			break;
		case ILO3:
		case ILO4:
			hostname = ir_handler->ir_hostname;
			itoascii(cmnd_bufsize, strlen(cmnd_buf));
			ilo_header_len = strlen( ILO3_RIBCL_XML_HDR)\
					+ strlen(hostname)\
					+ strlen(cmnd_bufsize);
			ir_handler->ribcl_xml_ilo3_hdr = malloc( ilo_header_len);
			if( ir_handler->ribcl_xml_ilo3_hdr == NULL){
				err("ilo2_ribcl_ssl_send_command():"
					"unable to allocate memory");
				return -1;
			}
			memset( ir_handler->ribcl_xml_ilo3_hdr,
				'\0', ilo_header_len);
			ir_xml_insert_headerinfo(ir_handler->ribcl_xml_ilo3_hdr,
				 ilo_header_len, ILO3_RIBCL_XML_HDR,
				ir_handler->ir_hostname, cmnd_bufsize);
			ret = oh_ssl_write(ssl_handler,
					ir_handler->ribcl_xml_ilo3_hdr,
					strlen(ir_handler->ribcl_xml_ilo3_hdr),
					0);
			/*
			 * Free up the allocated memory
			 */
			free( ir_handler->ribcl_xml_ilo3_hdr);
			break;
		case NO_ILO:
			hostname = ir_handler->ir_hostname;
			itoascii(cmnd_bufsize, strlen(ILO_RIBCL_TEST_ILO)-1);
			ilo_header_len = strlen( ILO_RIBCL_XML_TEST_HDR)\
					+ strlen(hostname)\
					+ strlen(cmnd_bufsize);
			ir_handler->ribcl_xml_test_hdr = malloc( ilo_header_len);
			if( ir_handler->ribcl_xml_test_hdr == NULL){
				err("ilo2_ribcl_ssl_send_command():"
					"unable to allocate memory");
				return -1;
			}
			memset( ir_handler->ribcl_xml_test_hdr,
				'\0', ilo_header_len);
			ir_xml_insert_headerinfo(ir_handler->ribcl_xml_test_hdr,
				ilo_header_len, ILO_RIBCL_XML_TEST_HDR,
				ir_handler->ir_hostname, cmnd_bufsize);
			ret = oh_ssl_write(ssl_handler,
					ir_handler->ribcl_xml_test_hdr,
					strlen(ir_handler->ribcl_xml_test_hdr),
					0);
			/*
			 * Free up the allocated memory
			 */
			free( ir_handler->ribcl_xml_test_hdr);
			break;
		default:
			err("ilo2_ribcl_ssl_send_command(): "
				"could not find iLO type.");
			ret = -1;
	}
	if( ret < 0){
		err("ilo2_ribcl_ssl_send_command(): "
		    "write of xml header to socket failed.");
		oh_ssl_disconnect(ssl_handler, OH_SSL_BI);
		return( -1);
	}

	/* Send the command buffer. */
	if( ir_handler->ilo_type != NO_ILO)
		ret = oh_ssl_write(ssl_handler, cmnd_buf, strlen(cmnd_buf), 0);
	else
		ret = oh_ssl_write(ssl_handler, ILO_RIBCL_TEST_ILO,
					 strlen(ILO_RIBCL_TEST_ILO), 0);
	if( ret < 0){
		err("ilo2_ribcl_ssl_send_command(): "
		    "write of xml command to socket failed.");
		oh_ssl_disconnect(ssl_handler, OH_SSL_BI);
		return( -1);
	}

	ret = 0;
	in_index = 0;
	while( 1){
		ret = oh_ssl_read( ssl_handler,
			 &(resp_buf[in_index]), (resp_size - in_index), 0);
	
		if( ret <= 0){
			break;
		}

		in_index = in_index + ret;
	}
	resp_buf[in_index] = '\0';

	/* cleanup */
	oh_ssl_disconnect(ssl_handler, OH_SSL_BI);

	return( 0);

} /* end ilo2_ribcl_ssl_send_command */


/**
 * ilo_ribcl_detect_ilo_type
 * @ir_handler: Pointer to the ilo handler.
 *
 * Detects if the current ilo is ilo2/ilo3
 *
 * Return value: returns either ILO2 or ILO3
 *
 **/
int ilo_ribcl_detect_ilo_type(ilo2_ribcl_handler_t *ir_handler)
{
	char *detect_cmd = NULL;
	char *d_response = NULL;
	char firstline[ILO2_RIBCL_HTTP_LINE_MAX];
	int index;
	int ret;

	d_response = malloc(ILO_RIBCL_TEST_ILO_RESPONSE_MAX);
	if(!d_response){
		err("ilo_ribcl_detect_ilo_type():"
			"unable to allocate memory");
		return( -1);
	}
	detect_cmd = ir_handler->ribcl_xml_test_hdr;
	ret = ilo2_ribcl_ssl_send_command( ir_handler, detect_cmd, 
				d_response, ILO_RIBCL_TEST_ILO_RESPONSE_MAX);
	if( ret < 0){
                err("ilo2_ribcl_ssl_send_command(): "
                    "write of xml header to socket failed.");
		free( d_response);
                return( -1);
        }
	index = 0;
	while(d_response[index]!='\n'){
		firstline[index]=d_response[index];
		index++;
	}
	firstline[index++] = '\n';
	firstline[index] = '\0';
	/*
	 *We can now free up the d_response
	 *pointer
	 */
	free( d_response);
	if(strcmp(ILO_RIBCL_TEST_RESPONSE, firstline)==0){
		dbg("Found iLO3/iLO4 MP");
		return ILO3;
	} else {
		dbg("Found iLO2 MP");
		return ILO2;
	}
} /* end ilo_ribcl_detect_ilo_type() */

/**
 * itoascii
 * @cmnd_size: Pointer to the converted string.
 * @decimal: integer value.
 *
 * Converts a integer value to ascii string.
 *
 * Return value: none.
 *
 **/
void itoascii(char cmnd_size[], int decimal)
{
	int i;
	int j;
	int temp;
	i = 0;
	do{
		cmnd_size[i++] = decimal % 10 + '0';
	}while((decimal /= 10) > 0);
	for(i = 0, j = strlen(cmnd_size)-1; i < j; i++, j--){
		temp = cmnd_size[i];
		cmnd_size[i] = cmnd_size[j];
		cmnd_size[j] = temp;
	}
} /* end itoascii() */
