/*      -*- linux-c -*-
 *
 * Copyright (c) 2004 by Intel Corp.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * These routines access the OpenIPMI driver directly, rather
 * than using OpenIPMI library calls. 
 *
 * Authors:
 *     Andy Cress <andrew.r.cress@intel.com>
 * Changes:
 *     12/01/04 ARCress - created from ipmiutil/ipmimv.c
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef SCO_UW
#include <sys/ioccom.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define uchar    unsigned char
#define IPMI_MAX_ADDR_SIZE 32
#define IPMI_SYSTEM_INTERFACE_ADDR_TYPE 0x0c
#define IPMI_BMC_CHANNEL  0xf
#define IPMI_IOC_MAGIC 'i'

#ifdef TV_PORT
/* use this to define timeval if it is a portability issue */
struct timeval {
        long int     tv_sec;         /* (time_t) seconds */
        long int     tv_usec;        /* (suseconds_t) microseconds */
};
#endif

struct ipmi_addr
{
        int   addr_type;
        short channel;
        char  data[IPMI_MAX_ADDR_SIZE];
};

struct ipmi_msg
{
        unsigned char  netfn;
        unsigned char  cmd;
        unsigned short data_len;
        unsigned char  *data;
};

struct ipmi_req
{
        unsigned char *addr; /* Address to send the message to. */
        unsigned int  addr_len;
        long    msgid; /* The sequence number for the message.  */
        struct ipmi_msg msg;
};
 
struct ipmi_recv
{
        int     recv_type;  	/* Is this a command, response, etc. */
        unsigned char *addr;    /* Address the message was from */
	int  addr_len;  	/* The size of the address buffer. */
        long    msgid;  	/* The sequence number from the request */
        struct ipmi_msg msg; 	/* The data field must point to a buffer. */
};

struct ipmi_system_interface_addr
{
        int           addr_type;
        short         channel;
        unsigned char lun;
};

#define IPMICTL_RECEIVE_MSG         _IOWR(IPMI_IOC_MAGIC, 12, struct ipmi_recv)
#define IPMICTL_RECEIVE_MSG_TRUNC   _IOWR(IPMI_IOC_MAGIC, 11, struct ipmi_recv)
#define IPMICTL_SEND_COMMAND        _IOR(IPMI_IOC_MAGIC,  13, struct ipmi_req)
#define IPMICTL_SET_GETS_EVENTS_CMD _IOR(IPMI_IOC_MAGIC,  16, int)

FILE *fperr = NULL;    /* if NULL, no messages */
static int ipmi_timeout_mv = 10;   /* 10 seconds, was 5 sec */
static int ipmi_fd = -1;
static int curr_seq = 0;

int ipmi_open_mv(void);
int ipmi_close_mv(void);
int ipmicmd_mv(uchar cmd, uchar netfn, uchar lun, uchar *pdata, uchar sdata, 
		uchar *presp, int sresp, int *rlen);

int ipmi_open_mv(void)
{
    if (ipmi_fd != -1) return(0);
    ipmi_fd = open("/dev/ipmi/0", O_RDWR);
    if (ipmi_fd == -1) 
        ipmi_fd = open("/dev/ipmi0", O_RDWR);
    if (ipmi_fd == -1)
        ipmi_fd = open("/dev/ipmidev0", O_RDWR);
    if (ipmi_fd == -1) 
        ipmi_fd = open("/dev/ipmidev/0", O_RDWR);
    if (ipmi_fd == -1) return(-1);
    return(0);
}

int ipmi_close_mv(void)
{
    int rc = 0;
    if (ipmi_fd != -1) { 
	rc = close(ipmi_fd);
	ipmi_fd = -1; 
    }
    return(rc);
}

int ipmicmd_mv(uchar cmd, uchar netfn, uchar lun, uchar *pdata, uchar sdata, 
		uchar *presp, int sresp, int *rlen)
{
    fd_set readfds;
    struct timeval tv;
    struct ipmi_recv      rsp;
    struct ipmi_addr      addr;
    struct ipmi_req                   req;
    struct ipmi_system_interface_addr bmc_addr;
    int    i;
    int    rv;

    rv = ipmi_open_mv();
    if (rv != 0) return(rv);

    i = 1;
    rv = ioctl(ipmi_fd, IPMICTL_SET_GETS_EVENTS_CMD, &i);
    if (rv) { return(errno); }

    FD_ZERO(&readfds);
    // FD_SET(0, &readfds);  /* dont watch stdin */
    FD_SET(ipmi_fd, &readfds);  /* only watch ipmi_fd for input */

    /* Send the IPMI command */ 
    bmc_addr.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
    bmc_addr.channel = IPMI_BMC_CHANNEL;
    bmc_addr.lun = lun;       // BMC_LUN = 0
    req.addr = (unsigned char *) &bmc_addr;
    req.addr_len = sizeof(bmc_addr);
    req.msg.cmd = cmd;
    req.msg.netfn = netfn;   
    req.msgid = curr_seq;
    req.msg.data = pdata;
    req.msg.data_len = sdata;
    rv = ioctl(ipmi_fd, IPMICTL_SEND_COMMAND, &req);
    curr_seq++;
    if (rv == -1) { rv = errno; }

    if (rv == 0) {
	tv.tv_sec=ipmi_timeout_mv;
	tv.tv_usec=0;
	rv = select(ipmi_fd+1, &readfds, NULL, NULL, &tv);
	/* expect select rv = 1 here */
	if (rv <= 0) { /* no data within 5 seconds */
	   if (fperr != NULL)
             fprintf(fperr,"drv select timeout, fd = %d, isset = %d, rv = %d, errno = %d\n",
		  ipmi_fd,FD_ISSET(ipmi_fd, &readfds),rv,errno);
	   if (rv == 0) rv = -3;
	   else rv = errno;
	} else {
	   /* receive the IPMI response */
	   rsp.addr = (unsigned char *) &addr;
	   rsp.addr_len = sizeof(addr);
	   rsp.msg.data = presp;
	   rsp.msg.data_len = sresp;
	   rv = ioctl(ipmi_fd, IPMICTL_RECEIVE_MSG_TRUNC, &rsp);
	   if (rv == -1) { 
	      if ((errno == EMSGSIZE) && (rsp.msg.data_len == sresp))
		 rv = 0;   /* errno 90 is ok */
	      else { 
		 rv = errno; 
		 if (fperr != NULL)
                   fprintf(fperr,"drv rcv_trunc errno = %d, len = %d\n",
			errno, rsp.msg.data_len);
	      }
	   } else rv = 0;
	   *rlen = rsp.msg.data_len;
	}
    }

    /* ipmi_close_mv();  * rely on the app calling ipmi_close */
    return(rv);
}

#ifdef TEST
int
main(int argc, char *argv[])
{
    fd_set readfds;
    struct timeval tv;
    char   data[40];
    int    i, j;
    int    err;
    int    rlen;

    err = ipmicmd_mv(0x01, 0x06, 0, NULL, 0, data, sizeof(data), &rlen);
    printf("ipmicmd_mv ret=%d, cc=%02x\n",err,(uchar)data[0]);
    printf(" ** Return Code: %2.2X\n", data[0]);
    printf(" ** Data[%d]: ",rlen);
    for (i=1; i < rlen; i++)
	    printf("%2.2X ", (uchar)data[i]);
    printf("\n");

    printf("\n");
    ipmi_close_mv();
    return 0;
}
#endif
