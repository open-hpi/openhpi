/*      -*- linux-c -*-
 *
 * Copyright (c) 2003 by International Business Machines
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors: David Judkovics
 *     
 *     
 *     
 */

#include "openhpid.h"
#include <dlfcn.h>

char *pname;

int counter = 0;

/* used to shutdown mainloop */
int done = 0;

/* prototypes */
extern void answer(int newssockfd);

int main (int argc, char *argv[])
{
        dbg("SERVER RUNNING\n");

        int     sockfd,
                newsockfd,
                nfds,
                clilen;
 
        void *dl_handle;
    

        struct sockaddr_in
                cli_addr,
                serv_addr;

        fd_set readfds;

        pname = argv[0];
    
        dl_handle = dlopen("/usr/local/lib/libopenhpi.so", RTLD_LAZY);

        /* initialize libopenhpi and load pluggins */
        init_openhpid();


        /* Open a TCP socket (An Internet stream socket). */
        if ( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 ) {
                dbg("socket error\n");
        }

        /* bind our local address so that the client can send to us. */
        bzero((char *)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family =      AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port =        htons(SERV_TCP_PORT);

        if ( bind (sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0 ) {
                dbg("bind error");
        }
        if ( listen (sockfd, 5) < 0 ) {
                dbg("listen error");
        }
    
        /* select setup */
        nfds = sockfd + 1;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        /* accept setup*/
        clilen = sizeof(cli_addr);
        /* main loop */
        while (!done) {
        
                select(nfds, &readfds, NULL, NULL, NULL);

                newsockfd = accept (sockfd, (struct sockaddr *)&cli_addr, &clilen );

                if (newsockfd < 0 ) {
                        perror("accept error");
                        done = TRUE;
                        break;
                }

                answer(newsockfd);
                /* make sure fd is released */
                if (shutdown(newsockfd, 0) < 0)
                        dbg("failure closing client connection");
        }

        close_openhpid();

        return(0); /* server shutdown */
}

