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

#define READ_BUF_SIZE	1024

typedef enum {
	HPI_SHELL_OK = 0,
	HPI_SHELL_CMD_ERROR = -1,
	HPI_SHELL_PARM_ERROR = -2
} ret_code_t;

typedef enum {
	UNDEF_COM,
	MAIN_COM,
	SEN_COM,
	CTRL_COM,
	INV_COM
} com_enum_t;

typedef struct {
    char	*cmd;
    ret_code_t	(*fun)(void);
    const char	*help;
    com_enum_t	type;
} command_def_t;

typedef enum {
	ITEM_TERM,
	CMD_END_TERM,
	EMPTY_TERM
} term_t;

typedef struct {
	term_t	term_type;
	char	*term;
} term_def_t;

extern command_def_t	commands[];
extern int		prt_flag;
extern int		show_event_short;
extern Domain_t		*Domain;
extern GSList		*domainlist;
extern term_def_t	*terms;
extern int		read_stdin;
extern int		read_file;
extern FILE		*input_file;
extern com_enum_t	block_type;
extern ret_code_t	shell_error;
extern int		debug_flag;

extern int		add_domain(Domain_t *domain);
extern ret_code_t	ask_rdr(SaHpiResourceIdT rptid, SaHpiRdrTypeT type, SaHpiInstrumentIdT *ret);
extern ret_code_t	ask_rpt(SaHpiResourceIdT *ret);
extern int		close_session(void);
extern void		cmd_shell(void);
extern ret_code_t	ctrl_block(void);
extern void		delete_progress(void);
extern int		do_discover(Domain_t *domain);
extern void		do_progress(char *mes);
extern int		get_hex_int_param(char *mes, int *val);
extern int		get_int_param(char *mes, int *val);
extern int		get_new_command(char *mes);
extern int		get_string_param(char *mes, char *string, int len);
extern term_def_t	*get_next_term(void);
extern int		get_sessionId(Domain_t *domain);
extern ret_code_t	help(int as);
extern ret_code_t	inv_block(void);
extern ret_code_t	list_sensor(void);
extern ret_code_t	open_file(char *path);
extern int		open_session(int eflag);
extern void		cmd_parser(char *mes, int as);
extern int		run_command(void);
extern ret_code_t	sen_block(void);
extern void		set_Subscribe(Domain_t *domain, int as);
extern ret_code_t	show_inv(void);
extern int		ui_print(char *Str);
extern ret_code_t	unget_term(void);

#endif

