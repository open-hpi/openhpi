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
#include <unistd.h>
#include "hpi_cmd.h"

#define MAX_IN_FILES		10
#define MAX_REDIRECTIONS	10


term_def_t	*terms;
int		read_stdin = 1;
int		read_file = 0;
com_enum_t	block_type = MAIN_COM;
FILE		*input_file = (FILE *)NULL;
ret_code_t	shell_error = HPI_SHELL_OK;
int		in_files_count = 0;
FILE		*input_files[MAX_IN_FILES];
int		out_files_count = 0;
FILE		*output_files[MAX_REDIRECTIONS];

/* local variables */
static char	input_buffer[READ_BUF_SIZE];	// command line buffer
static char	*input_buf_ptr = input_buffer;	// current pointer in input_buffer
static char	cmd_line[LINE_BUF_SIZE];	// current command line
static int	max_term_count = 0;
static int	term_count = 0;
static int	current_term = 0;

static int set_redirection(char *name, int redir)
{
	char	*flags;
	FILE	*out;

	if (out_files_count >= MAX_REDIRECTIONS) {
		printf("Too many redirections\n");
		return(-1);
	};
	if (redir == 1) flags = "w";
	else flags = "a";
	out = fopen(name, flags);
	if (out == (FILE *)NULL) {
		printf("Can not open/create file: %s\n", name);
		return(-1);
	};
	output_files[out_files_count] = stdout;
	stdout = out;
	out_files_count++;
	return(0);
}

static void remove_reditection(void)
{
	if (out_files_count == 0) return;
	out_files_count--;
	fclose(stdout);
	stdout = output_files[out_files_count];
}

static int delete_input_file(void)
{
	FILE		*f;

	if (in_files_count < 1) return(-1);
	in_files_count--;
	f = input_files[in_files_count];
	fclose(f);
	if (in_files_count == 0) return(-1);
	input_file = input_files[in_files_count - 1];
	return(0);
}

static void new_command(void)
{
	term_count = 0;
	current_term = 0;
	*cmd_line = 0;
}

static void go_to_dialog(void)
{
	if (read_stdin) return;
	if (delete_input_file() == 0) return;
	read_stdin = 1;
	read_file = 0;
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

ret_code_t cmd_parser(char *mes, int as, int new_cmd, int *redirect)
// as = 0  - get command
// as = 1  - may be exit with empty command
// new_cmd = 1 - new command
// new_cmd = 0 - get added items
//     returned redirect value (for new_cmd = 1):
//   0 - no redirect output
//   1 - redirect to the empty file
//   2 - add output to the existent file
{
	char	*cmd, *str, *beg;
	term_t	type;
	int	len, i;

	*redirect = 0;
	for (;;) {
		if (new_cmd) {
			new_command();
			type = CMD_TERM;
		} else type = ITEM_TERM;
		cmd = get_input_line(mes);
		if (cmd == (char *)NULL) {
			go_to_dialog();
			continue;
		};
		len = strlen(cmd_line);
		snprintf(cmd_line + len, LINE_BUF_SIZE - len, " %s",cmd);
		str = cmd;
		while (isspace(*str)) str++;
		if (strlen(str) == 0) {
			if (as) return HPI_SHELL_OK;
			continue;
		};
		beg = str;
		if (*beg == '#') continue;
		while (*str != 0) {
			if (isspace(*str)) {
				*str++ = 0;
				if (strlen(beg) > 0) {
					add_term(beg, type);
					type = ITEM_TERM;
				};
				while (isspace(*str)) str++;
				beg = str;
				continue;
			};
			if (*str == '\"') {
				str++;
				while ((*str != 0) && (*str != '\"')) str ++;
				if (*str == 0) {
					add_term(beg, type);
					if (read_file)
						add_term(";", CMD_ERROR_TERM);
					break;
				};
				if (*beg == '\"') {
					beg++;
					*str = 0;
					add_term(beg, type);
					type = ITEM_TERM;
					beg = str + 1;
				};
				str++;
				continue;
			};
			if (*str == '>') {
				*str++ = 0;
				if (strlen(beg) > 0)
					add_term(beg, type);
				if (*str == '>') {
					add_term(">>", CMD_REDIR_TERM);
					str++;
				} else add_term(">", CMD_REDIR_TERM);
				type = ITEM_TERM;
				beg = str;
				continue;
			};
			if (*str == ';') {
				*str++ = 0;
				break;
			};
			str++;
		};
		if (strlen(beg) > 0)
			add_term(beg, type);
		if (read_file)
			add_term(";", CMD_END_TERM);
		if (new_cmd == 0)
			return(HPI_SHELL_OK);
		for (i = 0; i < term_count; i++) {
			if (terms[i].term_type == CMD_ERROR_TERM)
				return(HPI_SHELL_SYNTAX_ERROR);
			if (terms[i].term_type == CMD_REDIR_TERM) {
				if (*redirect != 0)
					return(HPI_SHELL_SYNTAX_ERROR);
				if (strcmp(">>", terms[i].term) == 0)
					*redirect = 2;
				else *redirect = 1;
			}
		};
		return(HPI_SHELL_OK);
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
	ret_code_t	rv;

	if (debug_flag) printf("run_command:\n");
	term = get_next_term();
	if (term == NULL) return(1);
	if (term->term_type != CMD_TERM) return(1);
	c = get_cmd(term->term);
	if (c == (command_def_t *)NULL) {
		printf("Invalid command:\n%s\n", cmd_line);
		go_to_dialog();
		help(0);
		shell_error = HPI_SHELL_CMD_ERROR;
		return(-1);
	};
	if (c->fun) {
		if (debug_flag)
			printf("run_command: c->type = %d\n", c->type);
		term->term = c->cmd;
		if ((block_type != c->type) && (c->type != UNDEF_COM)) {
			block_type = c->type;
			if (debug_flag) printf("run_command: ret = 2\n");
			return(2);
		};
		rv = c->fun();
		if (rv == HPI_SHELL_PARM_ERROR) {
			printf("Invalid parameters:\n%s\n", cmd_line);
			printf("%s\n", c->help);
			go_to_dialog();
			shell_error = HPI_SHELL_PARM_ERROR;
			return(3);
		} else if (rv == HPI_SHELL_CMD_ERROR) {
			printf("Command failed:\n%s\n", cmd_line);
			go_to_dialog();
			return(3);
		};
		shell_error = rv;
	} else {
		printf("Unimplemented command:\n%s\n", cmd_line);
		go_to_dialog();
		shell_error = HPI_SHELL_CMD_ERROR;
		return(3);
	};
	return(0);
}

int get_new_command(char *mes)
{
	term_def_t	*term;
	int		redir = 0, i, res;
	char		*name;

	if (debug_flag) printf("get_new_command:\n");
	term = get_next_term();
	if ((term != NULL) && (term->term_type == CMD_TERM))
		unget_term();
	else
		cmd_parser(mes, 0, 1, &redir);
	if (redir != 0) {
		for (i = 0; i < term_count; i++) {
			if (terms[i].term_type == CMD_REDIR_TERM)
				break;
		};
		if (i >= term_count - 1) {
			printf("Syntax error:\n%s\n", cmd_line);
			go_to_dialog();
			return(3);
		};
		if (terms[i + 1].term_type != ITEM_TERM) {
			printf("Syntax error:\n%s\n", cmd_line);
			go_to_dialog();
			return(3);
		};
		name = terms[i + 1].term;
		res = set_redirection(name, redir);
		if (res != 0) {
			printf("Command failed:\n%s\n", cmd_line);
			go_to_dialog();
			return(3);
		};
		res = run_command();
		remove_reditection();
		return(res);
	};
	return(run_command());
}

void cmd_shell(void)
{
	help(0);
	for (;;) {
		if (debug_flag) printf("cmd_shell:\n");
		shell_error = HPI_SHELL_OK;
		get_new_command((char *)NULL);
		if ((shell_error != HPI_SHELL_OK) && read_file) {
			go_to_dialog();
		}
	}
}

int get_int_param(char *mes, int *val)
{
	int		res, redir;
	char		*str;
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		cmd_parser(mes, 1, 0, &redir);
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
	int		redir;
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		cmd_parser(mes, 1, 0, &redir);
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
		cmd_parser(mes, 1, 0, &i);
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
	if (read_file && (res->term_type == CMD_END_TERM))
		return((term_def_t *)NULL);
	current_term++;
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

int add_input_file(char *path)
{
	int		i;
	FILE		*f;

	if (in_files_count >= MAX_IN_FILES) return(-1);
	i = access(path, R_OK | F_OK);
	if (i != 0) {
		printf("Can not access file: %s\n", path);
		return(HPI_SHELL_PARM_ERROR);
	};
	f = fopen(path, "r");
	if (f == (FILE *)NULL) {
		printf("Can not open file: %s\n", path);
		return(HPI_SHELL_PARM_ERROR);
	};
	input_files[in_files_count] = f;
	in_files_count++;
	input_file = f;
	return(0);
}
