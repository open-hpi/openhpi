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
 * Authors:
 *     Racing Guo <racing.guo@intel.com>
 *     Aaron  Chen <yukun.chen@intel.com>
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <SaHpi.h>
#include <oh_utils.h>
#include "hpi_cmd.h"
#include "resource.h"

SaHpiSessionIdT		sessionid;
static pthread_t	ge_thread;
static pthread_t	prog_thread;
int			prt_flag = 0;
int			show_event_short = 0;
static int		in_progress = 0;
static GCond		*thread_wait = NULL;
static GMutex		*thread_mutex = NULL;
static char		*progress_mes;
Domain_t		*Domain;


#define PROGRESS_BUF_SIZE	80

/* Progress bar implementation */
static void* progress_bar(void *unused)
{
	GTimeVal	time;
	char		buf[PROGRESS_BUF_SIZE], A[20];
	int		i = 0, t = 0, len, mes_len;

	memset(buf, 0, PROGRESS_BUF_SIZE);
	mes_len = strlen(progress_mes);
	while (in_progress) {
		snprintf(A, 10, " %d.%d s ", t / 10, t % 10);
		len = strlen(A);
		memset(buf + mes_len, '.', i);
		strncpy(buf, progress_mes, mes_len);
		if (i > 8)
			strncpy(buf + mes_len + (i - len) / 2, A, len);
		printf("%s\r", buf);
		fflush(stdout);
		g_get_current_time(&time);
		g_time_val_add(&time, G_USEC_PER_SEC / 10);
		g_cond_timed_wait(thread_wait, thread_mutex, &time);
		if (i < (PROGRESS_BUF_SIZE - mes_len - 1)) i++;
		t++;
	};
        g_thread_exit(0);
	return (void *)1;
}

/* This function creates thread for progress bar.
 *	mes - progress bar title.
 */
void do_progress(char *mes)
{
	progress_mes = mes;
	in_progress = 1;
	pthread_create(&prog_thread, NULL, progress_bar, NULL);
}

/* This function deletes thread for progress bar. */
void delete_progress()
{
	char	buf[PROGRESS_BUF_SIZE];
	
	in_progress = 0;
	memset(buf, ' ', PROGRESS_BUF_SIZE);
	buf[PROGRESS_BUF_SIZE - 1] = 0;
	printf("%s\n", buf);
}

static void* get_event(void *unused)
{
	SaHpiEventT	event;
	SaErrorT	rv;        

	rv = saHpiSubscribe(sessionid);
	if (rv != SA_OK) {
		printf("OpenHPI>Fail to Subscribe event\n");
		return (void *)0;
	}	
	
	for(;;) {
		memset(&event, 0xF, sizeof(event));

		rv = saHpiEventGet(sessionid, SAHPI_TIMEOUT_BLOCK, &event, NULL, NULL, NULL);		
		if (rv != SA_OK ) {
			saHpiUnsubscribe(sessionid);
			return (void *)1;
		}
		if (prt_flag == 1) {
			if (show_event_short)
				show_short_event(&event);
			else
				oh_print_event(&event, 1);
		}
	} /*the loop for retrieving event*/
	return (void *)1;
}

int open_session()
{
	SaErrorT rv;

        if (!g_thread_supported()) {
                g_thread_init(NULL);
	};
	thread_wait = g_cond_new();
	thread_mutex = g_mutex_new();
	do_progress("Discover");
	rv = saHpiSessionOpen(SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL);
	if (rv != SA_OK) {
     		printf("saHpiSessionOpen error %s\n", oh_lookup_error(rv));
		return -1;
	}
	rv = saHpiDiscover(sessionid);
	if (rv != SA_OK) 
		printf("saHpiDiscover rv = %s\n", oh_lookup_error(rv));
	delete_progress();

	printf("Initial discovery done\n");

	Domain = init_resources(sessionid);
	if (Domain == (Domain_t *)NULL) {
     		printf("init_resources error\n");
		return -1;
	}
	printf("\tEnter a command or \"help\" for list of commands\n");

	pthread_create(&ge_thread, NULL, get_event, NULL);
	return 0;
}

int close_session()
{
	SaErrorT rv;

	pthread_kill(ge_thread, SIGKILL);
	
	rv = saHpiSessionClose(sessionid);
	if (rv != SA_OK) {
                printf("saHpiSessionClose error %s\n", oh_lookup_error(rv));
                return -1;
        }
	return 0;
}
