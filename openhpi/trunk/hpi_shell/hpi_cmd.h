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
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */
#ifndef _INC_HPI_CMD_H
#define _INC_HPI_CMD_H
#include <SaHpi.h>
#include <hpi_ui.h>

#define HPI_SHELL_MAJOR	0
#define HPI_SHELL_MINOR 2

enum error_type{
	HPI_SHELL_OK = 0,
	HPI_SHELL_PARM_ERROR,
	HPI_SHELL_CMD_ERROR
};

struct command {
    const char *cmd;
    int (*fun)(int, char **);
    const char *help;
};

extern SaHpiSessionIdT	sessionid;
extern struct command	commands[];
extern int		prt_flag;
extern int		show_event_short;
extern Domain_t		*Domain;

extern void	delete_progress(void);
extern void	do_progress(char *mes);
extern int	ui_print(char *Str);
extern int	help(int argc, char *argv[]);

void gdb(int level);
int open_session(void);
int close_session(void);
void cmd_shell(void);
int sa_show_rpt(SaHpiResourceIdT resourceid);  //show the rpt of a specific resource
int sa_list_res(void);                         //list all rpt information
int sa_show_rdr(SaHpiResourceIdT resourceid);
void time2str(SaHpiTimeT time, char * str, size_t size);
#endif

