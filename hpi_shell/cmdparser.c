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
 *	   Hu Yin     <hu.yin@intel.com>
 * Changes:
 *	11.30.2004 - Kouzmich: porting to HPI-B
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hpi_cmd.h"

term_def_t	*terms;
int		read_stdin = 1;
int		read_file = 0;
com_enum_t	block_type = MAIN_COM;
FILE		*input_file = (FILE *)NULL;
ret_code_t	shell_error = HPI_SHELL_OK;

/* local variables */
static char	input_buffer[READ_BUF_SIZE];	// command line buffer
static char	*input_buf_ptr = input_buffer;	// current pointer in input_buffer
static char	input_line[READ_BUF_SIZE];	// current input line
static int	max_term_count = 0;
static int	term_count = 0;
static int	current_term = 0;

static void new_command(void)
{
	term_count = 0;
	current_term = 0;
}

static void go_to_dialog(void)
{
	if (read_stdin) return;
	read_stdin = 1;
	read_file = 0;
	fclose(input_file);
	new_command();
}

static void add_term(char *term, term_t type)
{
	term_def_t	*tmp_terms;

	if (term_count >= max_term_count) {
		max_term_count += 5;
		tmp_terms = (term_def_t *)malloc(max_term_count * sizeof(term_def_t));
		memset(tmp_terms, 0, max_term_count * sizeof(term_def_t));
		if (term_count > 0) {
			memcpy(tmp_terms, terms, term_count * sizeof(term_def_t));
			free(terms);
		};
		terms = tmp_terms;
	};
	tmp_terms = terms + term_count;
	tmp_terms->term_type = type;
	tmp_terms->term = term;
	if (debug_flag) printf("add_term: term = %s  type = %d\n", term, type);
	term_count++;
}

static char *find_cmd_end(char *str)
{
	while (*str != 0) {
		if (*str == '\"') {
			str++;
			while ((*str != 0) && (*str != '\"')) str++;
			if (*str == 0) return(str);
		};
		if (*str == ';') return(str + 1);
		str++;
	};
	return(str);
}

static char *get_input_line(char *mes)
{
	FILE	*f = stdin;
	char	*res;
	int	len;

	if (strlen(input_buf_ptr) > 0) {
		res = input_buf_ptr;
		input_buf_ptr = find_cmd_end(input_buf_ptr);
		return(res);
	};
	input_buf_ptr = input_buffer;
	fflush(stdout);
	if (read_stdin) {
		if (mes != (char *)NULL) printf("%s", mes);
		else printf("OpenHPI> ");
		f = stdin;
	} else if (read_file) {
		f = input_file;
	} else {
		printf("Internal error: get_input_line:\n"
			" No input file\n");
		exit(1);
	};
	memset(input_buffer, 0, READ_BUF_SIZE);
	res = fgets(input_buffer, READ_BUF_SIZE - 1, f);
	if (res != (char *)NULL) {
		len = strlen(res);
		if ((len > 0) && (res[len - 1] == '\n'))
			res[len - 1] = 0;
		input_buf_ptr = find_cmd_end(input_buf_ptr);
	};
	return(res);
}

static command_def_t *get_cmd(char *name)
{
	const char	*p;
	command_def_t	*c, *res = (command_def_t *)NULL;
	int		len = strlen(name);
	int		n = 0;

	if (debug_flag) printf("get_cmd: block_type = %d\n", block_type);
	for (c = commands; (p = c->cmd) != NULL; c++) {
		if ((c->type != MAIN_COM) && (c->type != block_type) &&
			(c->type != UNDEF_COM))
			continue;
		if (strncmp(p, name, len) == 0) {
			if (n == 0) res = c;
			n++;
		};
		if (strcmp(p, name) == 0) {
			return(c);
		}
	};
	if (n == 1) return(res);
	return((command_def_t *)NULL);
}

void cmd_parser(char *mes, int as)
// as = 0  - get command
// as = 1  - may be exit with empty command
{
	char		*cmd, *str, *beg;

	for (;;) {
		new_command();
		cmd = get_input_line(mes);
		if (cmd == (char *)NULL) {
			go_to_dialog();
			continue;
		};
		strcpy(input_line, cmd);
		str = cmd;
		while (isspace(*str)) str++;
		if (strlen(str) == 0) {
			if (as) return;
			continue;
		};
		beg = str;
		if (*beg == '#') continue;
		while (*str != 0) {
			if (isspace(*str)) {
				*str++ = 0;
				if (strlen(beg) > 0)
					add_term(beg, ITEM_TERM);
				while (isspace(*str)) str++;
				beg = str;
				continue;
			};
			if (*str == '\"') {
				str++;
				while ((*str != 0) && (*str != '\"')) str ++;
				if (*str == 0) {
					add_term(beg, ITEM_TERM);
					return;
				};
				if (*beg == '\"') {
					beg++;
					*str = 0;
					add_term(beg, ITEM_TERM);
					beg = str + 1;
				};
				str++;
				continue;
			};
			if (*str == 0) {
				if (strlen(beg) > 0)
					add_term(beg, ITEM_TERM);
				return;
			};
			if (*str == ';') {
				*str++ = 0;
				if (strlen(beg) > 0)
					add_term(beg, ITEM_TERM);
				add_term(";", CMD_END_TERM);
				return;
			};
			str++;
		};
		if (strlen(beg) > 0)
			add_term(beg, ITEM_TERM);
		return;
	}
}

int run_command(void)
//  returned:	-1  - command not found
//		0   - command found and ran
//		1   - no command
//		2   - other block command (do not run command)
//		3   - get new command
{
	term_def_t	*term;
	command_def_t	*c;
	int		rv;

	if (debug_flag) printf("run_command:\n");
	term = get_next_term();
	if (term == NULL) return(1);
	if (term->term_type != ITEM_TERM) return(1);
	c = get_cmd(term->term);
	if (c == (command_def_t *)NULL) {
		printf("Invalid command:\n%s\n", input_line);
		go_to_dialog();
		help(0);
		shell_error = HPI_SHELL_CMD_ERROR;
		return(-1);
	};
	if (c->fun) {
		term->term = c->cmd;
		if ((block_type != c->type) && (c->type != UNDEF_COM)) {
			block_type = c->type;
			return(2);
		};
		rv = c->fun();
		if (rv == HPI_SHELL_PARM_ERROR) {
			printf("Invalid parameters:\n%s\n", input_line);
			printf("%s\n", c->help);
			go_to_dialog();
			shell_error = HPI_SHELL_PARM_ERROR;
			return(3);
		} else if (rv == HPI_SHELL_CMD_ERROR) {
			printf("Command failed:\n%s\n", input_line);
			go_to_dialog();
			shell_error = HPI_SHELL_CMD_ERROR;
			return(3);
		} else if (rv == -1) {
			printf("Command failed:\n%s\n", input_line);
			go_to_dialog();
			shell_error = HPI_SHELL_CMD_ERROR;
			return(3);
		}
	} else {
		printf("Unimplemented command:\n%s\n", input_line);
		go_to_dialog();
		shell_error = HPI_SHELL_CMD_ERROR;
		return(3);
	};
	shell_error = HPI_SHELL_OK;
	return(0);
}

int get_new_command(char *mes)
{
	cmd_parser(mes, 0);
	return(run_command());
}

void cmd_shell(void)
{
	int	i;

	help(0);
	for (;;) {
		shell_error = HPI_SHELL_OK;
		i = get_new_command((char *)NULL);
		if ((shell_error != HPI_SHELL_OK) && read_file) {
			go_to_dialog();
		}
	}
}

int get_int_param(char *mes, int *val)
{
	int		res;
	char		*str;
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		cmd_parser(mes, 1);
		term = get_next_term();
	};
	if (term == NULL) {
		go_to_dialog();
		return(HPI_SHELL_CMD_ERROR);
	};
	str = term->term;
	if (isdigit(*str)) {
		res = sscanf(str, "%d", val);
		return(res);
	};
	return(0);
}

int get_hex_int_param(char *mes, int *val)
{
	char		*str, buf[32];
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		cmd_parser(mes, 1);
		term = get_next_term();
	};
	if (term == NULL) {
		go_to_dialog();
		return(HPI_SHELL_CMD_ERROR);
	};
	str = term->term;
	if (strncmp(str, "0x", 2) == 0)
		snprintf(buf, 31, "%s", str);
	else
		snprintf(buf, 31, "0x%s", str);
	*val = strtol(buf, (char **)NULL, 16);
	return(1);
}

int get_string_param(char *mes, char *val, int len)
{
	term_def_t	*term;
	int		i;

	term = get_next_term();
	if (term == NULL) {
		cmd_parser(mes, 1);
		term = get_next_term();
	};
	if (term == NULL) {
		return(HPI_SHELL_CMD_ERROR);
	};
	memset(val, 0, len);
	strncpy(val, term->term, len);
	for (i = 0; i < len; i++)
		if (val[i] == '\n') val[i] = 0;
	return(0);
}

term_def_t *get_next_term(void)
{
	term_def_t	*res;

	if (current_term >= term_count){
		if (debug_flag) printf("get_next_term: term = (NULL)\n");
		return((term_def_t *)NULL);
	};
	res = terms + current_term;
	current_term++;
	if (res->term_type != ITEM_TERM) {
		if (debug_flag) printf("get_next_term: type != ITEM_TERM\n");
		return((term_def_t *)NULL);
	};
	if (debug_flag) printf("get_next_term: term = %s\n", res->term);
	return(res);
}

ret_code_t unget_term(void)
{
	if (debug_flag) printf("unget_term:\n");
	if (current_term > 0)
		current_term--;
	return (current_term);
}
