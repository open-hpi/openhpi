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

#include <oh_clients.h>
#define OH_SVN_REV "$Revision: 1.3 $"

/************************************************************
 * Globals here
 ***********************************************************/
static int hpicrypt_usage(char* program_name)
{
        printf("Usage:\n");
        printf("%s [-d] -c config file\n",program_name);
        printf("-d for decrypting\n");
        printf("-c config file to encrypt or decrypt\n");
        printf("backup the file before encrypting\n");
        printf("\n");
        return(0);
}

int main(int argc, char *argv[])
{

	char *file_path = NULL;
	char *file_contents = NULL;
        char *program = argv[0];
	int crypt_type = OHPI_ENCRYPT;

        while ((argc > 1) && (argv[1][0] == '-'))
        {
                switch (argv[1][1])
                {
                        case 'd':
                                crypt_type = OHPI_DECRYPT;
                                break;

                        case 'c':
                                ++argv;
                                --argc;
                                file_path=argv[1]; 
                                break;

                        default:
                                printf("Wrong Argument: %s\n", argv[1]);
                                hpicrypt_usage(program);
                                return(1);
                }

                ++argv;
                --argc;
        }
 
        if ((argc > 1) || (file_path == NULL)) 
        {
                hpicrypt_usage(program);
                return(1);
        }

        /* 
         * Right now all the file handling is done in oh_crypt function.
         * The file_contents could be used to move the file handling here.
         */

	if ( crypt_type ==OHPI_ENCRYPT ) {
	        file_contents=oh_crypt(file_path, crypt_type);
	        if (file_contents == NULL) {
		         CRIT("The encryption of %s failed\n",file_path);
                         return(1);
	        } 
                printf("File was encrypted and overwritten. If you like a backup,\n");
                printf("please decrypt with hpicrypt -d and save the text file securely\n");
	        g_free(file_contents);
	        file_contents=NULL;
	        return(0);
	} else if ( crypt_type == OHPI_DECRYPT ) {
	        file_contents = oh_crypt(file_path, crypt_type);
                if (file_contents == NULL) {
                         CRIT("The decryption of %s failed\n",file_path);
                         return(1);
                }
	        printf("%s",file_contents);
	        printf("# Redirect above contents to a file to add/remove/edit handlers.\n");
	        printf("# Last two comment lines could be removed before saving.\n");
	        g_free(file_contents);
	        file_contents=NULL;
	        return(0);
	}
	else {
		printf("Unknown crypt type %d. %d and %d are only valid\n",crypt_type, OHPI_ENCRYPT, OHPI_DECRYPT);
		return(1);
	}
        return(0);
 
	
}
