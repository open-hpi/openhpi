/*
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
 *     Mohanasundaram Devarajulu (mohanasundaram.devarajulu@hp.com)
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <SaHpi.h>
#include <oHpi.h>
#include <oh_utils.h>



/**
 * openhpi.conf in this directory needs to have 600 or 400 permissions 
 * SVN does not allow to remove read permissions, so set it up now
 **/
 
 
int main(int argc, char **argv)
{
	struct stat fst;
	char filename[30] = "./openhpi.conf";
	char modestr[] = "0600";
	int mode = 0;
	mode = strtol(modestr, 0 ,8);
        if (stat (filename, &fst) == -1) {
                printf("stat of %s failed. Error is %s \n", filename, strerror(errno));
		if (errno == ENOENT) {
#ifdef OPENHPI_CONF
			if(stat (OPENHPI_CONF, &fst) == -1) {	
                		printf("stat of %s failed. Quitingi. \n", OPENHPI_CONF);
				if (errno == ENOENT) 
					return 0;
				else
					return -1;
			}
			if (chmod(filename,mode) < 0)
			{
				printf("chmod (%s, %s) failed as %s\n", filename, modestr, strerror(errno));
				return -1;
			}
#endif
			return 0;
		}
		else
			return -1;
        }
	if (chmod(filename,mode) < 0)
	{
		printf("chmod (%s, %s) failed with %d(%s)\n", filename, modestr, errno, strerror(errno));
		return -1;
	}
	return 0;
}
	
