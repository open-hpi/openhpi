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
#include <glib.h>

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

extern struct command	commands[];
extern int		prt_flag;
extern int		show_event_short;
extern Domain_t		*Domain;
extern GSList		*domainlist;

extern int	add_domain(Domain_t *domain);
extern void	delete_progress(void);
extern int	do_discover(Domain_t *domain);
extern void	do_progress(char *mes);
extern int	get_sessionId(Domain_t *domain);
extern int	ui_print(char *Str);
extern int	help(int argc, char *argv[]);
extern int	open_session(int eflag);
extern int	close_session(void);
extern void	cmd_shell(void);
extern void	set_Subscribe(Domain_t *domain, int as);
#endif

