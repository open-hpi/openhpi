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
 * management functionality such as open/close SSL connection to iLO2
 * and send/receive command/responses from RIBCL over the SSL connection.
 * Uses OpenSSL and supported the following functions:
 *
 * ilo2_ribcl_ssl_ctx_init()	- Creates a new SSL_CTX object as
 *				  framework for  TLS/SSL enabled functions.
 *				  Adds default CA certificate location.
 * ilo2_ribcl_ssl_connect()	- Create and open new ssl conection.
 * ilo2_ribcl_ssl_read()	- Read from SSL connection.
 * ilo2_ribcl_ssl_write()	- Write to SSL connection.
 * ilo2_ribcl_ssl_disconnect()	- Close/Free SSL connection.
 * ilo2_ribcl_ssl_ctx_free()	- Free SSL_CTX.
*****************************/

/* OpenSSL headers */

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <ilo2_ribcl_ssl.h>
#include <ilo2_ribcl_cmnds.h>

/**
 * ilo2_ribcl_ssl_ctx_init: Creates a new SSL_CTX object as framework for
 * TLS/SSL enabled functions. Adds default CA certificate location.
 * @none:
 *
 * Detailed description:
 * 	- Calls SSL_CTX_new() to initialize the list of ciphers, the session
 * 	  cache setting, the callbacks, the keys and certificates, and the
 * 	  options to its default values.
 *	- Calls SSL_CTX_set_default_verify_paths() to set default locations
 *	  for trusted CA certificates. SSL_CTX_set_default_verify_paths() 
 *	  allows us to add system-wide default certs path to the verify 
 *	  CApath without having to specify a default location. Default 
 *	  location of the trusted root certificates varies from distribution
 *	  to distributuion on Linux. Hence chose to use this undocumented
 *	  feature instead of SSL_CTX_load_verify_locations().
 *
 * Return values:
 * 	void *	pointer to SSL_CTX upon Success.
 * 	NULL	failure.
 **/
void *ilo2_ribcl_ssl_ctx_init()
{
	SSL_CTX *ctx;

	ctx = SSL_CTX_new(SSLv23_client_method());
	if(ctx == NULL) {
		return(NULL);
	}

	SSL_CTX_set_options(ctx, SSL_OP_TLS_ROLLBACK_BUG|SSL_OP_ALL);

	if(! SSL_CTX_set_default_verify_paths(ctx)) {
		return(NULL);
	}

	return((void *) ctx);
}

/**
 * ilo2_ribcl_ssl_connect: Create and open new ssl conection.
 * @hostname: Target name of the host to open connection to. Example:
 *            "hostname:port" or "IPaddress:port".
 * @ctx: void pointer to SSL_CTX returned by ilo2_ribcl_ssl_ctx_init(). 
 *
 *
 * Detailed description:
 * This routine creates and opens new SSL connection to a specified host
 * and return void pointer to connection BIO pointer. Calls to do subsequent
 * operations on this connection require this connection pointer as input
 * parameter.
 *
 * Return values:
 * 	void *	pointer to BIO upon Success.
 * 	NULL	failure.
 **/
void *ilo2_ribcl_ssl_connect(char *hostname, void *ctx)
{
	BIO *bio;
	SSL *ssl;

	if(hostname == NULL) {
		err("ilo2_ribcl_ssl_connect: NULL hostname.");
		return(NULL);
	}
	if(ctx == NULL) {
		err("ilo2_ribcl_ssl_connect: NULL ctx.");
		return(NULL);
	}

	bio = BIO_new_ssl_connect(ctx);
	if(bio == NULL) {
		err("ilo2_ribcl_ssl_connect: BIO_new_ssl_connect returned NULL.");
		return(NULL);
	}
	BIO_get_ssl(bio, &ssl);
	if(ssl == NULL) {
		err("ilo2_ribcl_ssl_connect: BIO_get_ssl returned NULL.");
	}
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(bio, hostname);

	if(BIO_do_connect(bio) <= 0) {
		err("ilo2_ribcl_ssl_connect: BIO_do_connect returned <= 0.");
		err("ilo2_ribcl_ssl_connect: Error: %s.", ERR_reason_error_string(ERR_get_error()) );
		/* free new connection */
		BIO_free_all(bio);
		return(NULL);
	}

	return((void *) bio);
}

/**
 * ilo2_ribcl_ssl_read: Read from SSL connection.
 * @bio: void pointer to BIO returned by ilo2_ribcl_ssl_connect. 
 * @buf: buffer to read from the socket.
 * @size: number of bytes to be read.
 *
 * Detailed description:
 * This routine opens the SSL connection which has previously created using
 * the input BIO pointer. Reads from the socket and returns the buffer and
 * the number bytes read.
 *
 * Return values:
 * Number of bytes read.
 * 0 nothing more to read.
 * -1 in case of error.
 *
**/
int ilo2_ribcl_ssl_read(void *bio, char *buf, int size)
{
	int bytes = 0;

	if((bio == NULL) || (buf == NULL) || (size <=0)) {
		return(-1);
	}

	bytes = BIO_read((BIO *) bio, buf, size);
	/* A 0 or -1 return is not necessarily an indication of an error.
	   In particular when the source/sink is non-blocking or of a certain
	   type it may merely be an indication that no data is currently 
	   available and that the application should retry the operation
	   later.
	   Call BIO_should_read() to check the real status of the socket.
	   Call BIO_flush() if BIO_should_read() retrurns false. 
	*/
	if(bytes <= 0) {
		if(! BIO_should_read((BIO *) bio)) {
			if(BIO_flush((BIO *) bio) <= 0) {
				/* Error */
				return(-1);
			}
			return(0);
		} else {
			return(-1);
		}
	}

	return(bytes);
}

/**
 * ilo2_ribcl_ssl_write: Write to SSL connection.
 * @bio: void pointer to BIO returned by ilo2_ribcl_ssl_connect.
 * @buf: buffer to write to the socket.
 * @size: number of bytes to be written.
 * Detailed description:
 * This routine opens the SSL connection which has previously created using
 * the input BIO pointer. Writes to the socket.
 *
 * Return values:
 * 0 success.
 * -1 in case of error.
 *
**/
int ilo2_ribcl_ssl_write(void *bio, char *buf, int size)
{
	int ret;
	if((bio == NULL) || (buf == NULL) || size <=0) {
		return(-1);
	}

	if((ret = BIO_write((BIO *) bio, buf, size) <= 0)) {
		if(! BIO_should_retry((BIO *)bio)) {
			/* Error */
			return(-1);
		}
	}

	/* 
	 * The iLO web server processes the network-based requests, 
	 * dispatching them to the XML handler following the initial tag. 
	 * The socket flush causes the hand-off to happen at a "clean" 
	 * transition, otherwise there can be symptoms from buffering.
	 */
	if(BIO_flush((BIO *) bio) <= 0) {
		/* Error */
		return(-1);
	}

	return(0);
}

/**
 * ilo2_ribcl_ssl_disconnect: Close/Free SSL connection.
 * @bio: void pointer to BIO returned by ilo2_ribcl_ssl_connect.
 *
 * Detailed description:
 * This routine closes and frees memory associated with an open SSL connection
 * referenced by the input BIO pointer.
 *
 * Return values:
 * 0 success.
 * -1 failure.
 **/
int ilo2_ribcl_ssl_disconnect(void *bio)
{
	SSL *ssl;
	int ret;

	if(bio == NULL) {
		return(0);
	}

	BIO_get_ssl(bio, & ssl);
	ret = SSL_shutdown(ssl);

	/* free connection */
	BIO_free_all((BIO *) bio);

	return(0);
}

/**
 * ilo2_ribcl_ssl_ctx_free: Free SSL_CTX.
 * @ctx: void pointer to SSL_CTX returned by ilo2_ssl_connect.
 *
 * Detailed description:
 * Frees memory associated with SSL_CTX pointer.
 *
 * Return values:
 * 0 success.
 * -1 failure.
 **/
int ilo2_ribcl_ssl_ctx_free(void *ctx)
{
	if(ctx == NULL) {
		return(0);
	}

	SSL_CTX_free((SSL_CTX *) ctx);
	
	return(0);
}

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

	/* Zero out the response buffer */
	memset( resp_buf, 0, resp_size);

	ssl_handler = ilo2_ribcl_ssl_connect( ir_handler->ilo2_hostport,
				ir_handler->ssl_ctx);

	if( ssl_handler == NULL){
		err("ilo2_ribcl_ssl_send_command(): ilo2_ribcl_ssl_connect returned NULL.");
		return( -1);
	}

	/* Send the XML header. iLO2 requires the header to be sent ahead 
	   separately from the buffer containing the command. */

	ret = ilo2_ribcl_ssl_write(ssl_handler, ILO2_RIBCL_XML_HDR,
				sizeof(ILO2_RIBCL_XML_HDR));
	if( ret < 0){
		err("ilo2_ribcl_ssl_send_command(): write of xml header to socket failed.");
		ilo2_ribcl_ssl_disconnect(ssl_handler);
		return( -1);
	}

	/* Send the command buffer. */
	ret = ilo2_ribcl_ssl_write(ssl_handler, cmnd_buf, strlen(cmnd_buf));
	if( ret < 0){
		err("ilo2_ribcl_ssl_send_command(): write of xml command to socket failed.");
		ilo2_ribcl_ssl_disconnect(ssl_handler);
		return( -1);
	}

	ret = 0;
	in_index = 0;
	while( 1){
		ret = ilo2_ribcl_ssl_read( ssl_handler,
			 &(resp_buf[in_index]), (resp_size - in_index));
	
		if( ret <= 0){
			break;
		}

		in_index = in_index + ret;
	}
	resp_buf[in_index] = '\0';

	/* cleanup */
	ilo2_ribcl_ssl_disconnect(ssl_handler);

	return( 0);

} /* end ilo2_ribcl_ssl_send_command */
