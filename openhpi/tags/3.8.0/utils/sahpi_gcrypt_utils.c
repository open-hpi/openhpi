/*      
 *
 * Copyright (C) 2013, Hewlett-Packard Development Company, LLP
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
 *      Mohan Devarajulu <mohan@fc.hp.com>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <glib.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <oh_error.h>

#ifdef HAVE_ENCRYPT

SaErrorT oh_initialize_gcrypt(gcry_cipher_hd_t *h1, gcry_cipher_hd_t *h2, gchar *key)
{
        static int initialized = 0;

        if (initialized == 1)
               return(0);
      
	if ((!gcry_check_version ( NULL )) || 
            (gcry_control( GCRYCTL_DISABLE_SECMEM_WARN )) || 
	    (gcry_control( GCRYCTL_INIT_SECMEM, 16384, 0 )) ||
	    (gcry_cipher_open( h1, GCRY_CIPHER_ARCFOUR,GCRY_CIPHER_MODE_STREAM,0 )) ||
	    (gcry_cipher_open( h2, GCRY_CIPHER_ARCFOUR,GCRY_CIPHER_MODE_STREAM,0 )) ||
	    (gcry_cipher_setkey ( *h1, key, strlen(key) )) ||
            (gcry_cipher_setkey ( *h2, key, strlen(key) ))) {
                CRIT("Something went wrong with gcrypt calls");
                return(1);
        } else {
                initialized = 1;
                return(0);
        }
}


	

/******************************************************************************
 * gchar *oh_crypt(gchar *file_path, SaHpiUint32T type)
 *
 * Function to encrypt or decrypt given file. The encrypted
 * or decrypted text is returned to the caller. The caller needs to free the
 * memory. 
 * While encrypting the file, encrypted file is saved to the original file 
 * location after a check and the encrypted text is sent to the caller. While 
 * decrypting, the decrypted text is sent to the caller, but the original file 
 * is left intact.
 *
 * file_path - File name to be encrypted or decrypted. When the file is
 * encrypted, the original file is overwritten, when it is decrypted it is left 
 * as it is
 * type - Encrypt or Decrypt.
 *
 * Return: gchar *
 *         Encrypted or Decrypted string
 *
 *****************************************************************************/
gchar * oh_crypt(gchar *file_path, SaHpiUint32T type)
{
        gcry_cipher_hd_t handle1, handle2;
        int ret = 0, err=0, cwd_fd = 0;
        struct stat buf, keybuf;
        char * plain_text = NULL;
        char * ptr_substr = NULL;
        char * out = NULL;
        char * key = NULL;
        char * deout = NULL;
        long int file_size = 0, en_file_size=0;
        char  file_path_ck[PATH_MAX];
        char  file_path_en[PATH_MAX];
        char cwd[PATH_MAX];
        FILE *fconf = NULL, *fpuuid = NULL;

        /* If you can not stat then you can not access the file */
	
        if (getcwd(cwd, sizeof(cwd)) != NULL)
                cwd_fd = open(cwd, O_DIRECTORY, S_IWUSR);

        ret = faccessat(cwd_fd, file_path, R_OK, AT_EACCESS );
        if (ret != 0 )
        {
                CRIT("Can not access file %s",file_path);
                return(NULL);
        } else {
                ret = stat(file_path, &buf);
                /* Really stat call should not fail here */
                if (ret != 0 )
                {
                        CRIT("Can not stat file %s",file_path);
                        return(NULL);
                }
        }

        file_size = buf.st_size;
        if ((file_size > 1000000000) || 
            (strlen(file_path)+10 > PATH_MAX)) {
                CRIT("%s path is long or too big",file_path);
                return(NULL);
        }

        plain_text = ( char * ) g_malloc0 
			( sizeof ( char ) * ( file_size + 1) );
        fconf = fopen(file_path,"r");
        if (fconf == NULL) {
                CRIT("Unable to open %s",file_path);
                return(NULL);
        }
        fread(plain_text, 1, file_size, fconf);
        fclose(fconf);

        ret = faccessat(0, PUUID_PATH, R_OK, AT_EACCESS );
        if ( ret != 0 ) {
                if (geteuid() == 0) {
                        WARN("Could not find %s",PUUID_PATH);
                        WARN("Using the SAHPI_DEFKEY. If the above file is created");
                        WARN("ever, move the file away to decrypt");
                }
                key = ( char * ) g_malloc0 ( sizeof ( char ) * ( strlen(SAHPI_DEFKEY)+1) );
                strcpy(key,SAHPI_DEFKEY);
        } else {
                ret = stat(PUUID_PATH, &keybuf);
                /* Really stat call should not fail here */
                if (ret != 0 )
                {
                        CRIT("Can not stat file %s",PUUID_PATH);
                        return(NULL);
                }
                fpuuid=fopen(PUUID_PATH, "r");
                if (fpuuid == NULL) {
                      if (geteuid() == 0) 
                              CRIT("Unable to open %s",PUUID_PATH);
                      else
                              CRIT("Unable to open %s, Use sudo", PUUID_PATH);
                      return(NULL);
                }
                key = ( char * ) g_malloc0 ( sizeof ( char ) * ( keybuf.st_size + 1) );
                ret=fread(key, 1, keybuf.st_size, fpuuid);
                fclose(fpuuid);
        }

        if(oh_initialize_gcrypt(&handle1, &handle2, key)) {
                CRIT("Could not initialize gcrypt libraries");
                g_free(key);
                return(NULL);
        } 

        if ( type == OHPI_ENCRYPT ) {
                ptr_substr=strstr(plain_text,"handler");
                if (!ptr_substr) {
                        CRIT("Looks like the %s does not have a handler",file_path);
                        CRIT("It is either already encrypted or no plugins defined");
                        g_free(plain_text);
                        g_free(key);
                        return(NULL);
                }
                out = ( char * ) g_malloc0 ( sizeof ( char ) * ( file_size + 1 ) );
                err =  gcry_cipher_encrypt ( handle1, ( unsigned char * ) out, file_size,
                        ( const unsigned char * ) plain_text, file_size );
                if ( err )
                {
                        CRIT("Failure: %s/%s", gcry_strsource ( err ), gcry_strerror ( err ) );
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);

                }

                strcpy(file_path_en, file_path);
                strcat(file_path_en,".temp_en");
                if (access (file_path_en, F_OK) != -1 ) {
                        CRIT("Trying to use %s for temporary_file",file_path_en);
                        CRIT("That file already exists. Remove it and restart again");
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                }
                fconf = fopen(file_path_en,"w");
                if (fconf == NULL) {
                        CRIT("Unable to open the file for writing");
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                }
                err = fwrite(out, 1, file_size, fconf);
                if (err != file_size ) {
                        CRIT("Wrote %d, but expected to write %ld",err, file_size);
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                }
                err = 0;
                fclose(fconf);
		ret = chmod(file_path_en, buf.st_mode); /* Setting mode to original file mode */ 
                ret = stat(file_path_en, &buf);
                en_file_size = buf.st_size;
                if (file_size != en_file_size) {
                        CRIT("original file size %ld, encrypted %ld", file_size, en_file_size);
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                }
                memset(out,0,file_size);
                fconf = fopen(file_path_en,"r");
                fread(out,1,file_size,fconf);
                fclose(fconf);
                deout = ( char * ) g_malloc0 ( sizeof ( char ) * ( file_size + 1 ) );
                err =  gcry_cipher_decrypt ( handle2, ( unsigned char * ) deout, file_size,
                        ( const unsigned char * ) out, file_size );
                if (strcmp(deout, plain_text) != 0 || (err)) {
                        CRIT("Unable to encrypt and decrypt");
                        if(err)
                                CRIT("decrypt call failed");
                        remove(file_path_en);
                        if (strlen(file_path)+10 < 1024) {
                                strcpy(file_path_ck, file_path);
                                strcat(file_path_ck,".check");
                        }
                        fconf = fopen(file_path_ck,"w");
                        err = fwrite(deout, 1, file_size, fconf);
                        if (err != file_size ) {
                                CRIT("Wrote %d, but expected to write %ld",err, file_size);
                        }
                        CRIT("compare %s and %s. Encryption failed",file_path,file_path_ck);
                        g_free(plain_text);
                        g_free(out);
                        g_free(deout);
                        g_free(key);
                        return(NULL);
                }
                /* At this point everything is ok. Able to encrypt and save into a file.
                * the same file is read and the contents decrypted. Original and decryption
                * match. So move the encrypted file to the original location */
                g_free(plain_text);
                g_free(deout);
                err = 0;
                err = rename(file_path_en, file_path);
                if ( err != 0 ) {
                        CRIT("%s is not moved to %s",file_path_en,file_path);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                } 
                g_free(key);
                return(out);
        } else if(type == OHPI_DECRYPT) {
                ptr_substr=strstr(plain_text,"handler");
                if (ptr_substr) {
                        CRIT("Looks like the %s is a plain file",file_path);
                        CRIT("It was not encrypted?. Please check");
                        g_free(plain_text);
                        g_free(key);
                        return(NULL);
                }

                out = ( char * ) g_malloc0 ( sizeof ( char ) * ( file_size + 1 ) );
                err =  gcry_cipher_decrypt ( handle2, ( unsigned char * ) out, file_size,
                        ( const unsigned char * ) plain_text, file_size );
                if ( err )
                {
                        CRIT("Failure: %s/%s", gcry_strsource ( err ), gcry_strerror ( err ) );
                        g_free(plain_text);
                        g_free(out);
                        g_free(key);
                        return(NULL);
                } else {
                        g_free(plain_text);
                        ptr_substr = NULL;
                        ptr_substr=strstr(out,"handler");
                        if ((!ptr_substr) && (geteuid() == 0)) {
                                CRIT("Looks like the decrypted texts does not have a handler");
                                CRIT("Decryption might have failed.");
                                if(!strcmp(key,SAHPI_DEFKEY))
                                       INFO("Check if %s exists",PUUID_PATH);
                                else {
                                       INFO("Try to decrypt it as a normal user/no sudo");
                                       INFO("if that fails recover from backup. Sorry");
                                }
                        } else if (!ptr_substr) {
                                CRIT("Looks like the decrypted text does not have a handler");
                                CRIT("Decryption might have failed.");
                                if(!strcmp(key,SAHPI_DEFKEY))
                                       INFO("If file is bad, Recover from backup or try sudo.");
                                else 
                                       INFO("If file is bad, Recover from backup.");
                        }  
                        g_free(key);
                        return(out);
                }
        } else {
                CRIT("Unknown Type %d",type);
                g_free(plain_text);
                g_free(key);
                return(NULL);
        }
        g_free(key);
        return(NULL);
}

#endif
