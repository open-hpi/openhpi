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
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hpi_cmd.h"

/* functions prototype */
static char *get_input_line(char *buf, int buflen);
static void cmd_parser(void);
static struct command *get_cmd(const char *name);
static char **make_argv(int *pargc, char **parg);
static char *parse_string(void);

/* local variables */
static char line[200];
static char argbuf[200];
char *strbase;
char *argbase;
char *altarg;
int parseflag;

/* main line */
static char *get_input_line(char *buf, int buflen)
{
	printf("OpenHPI> ");
	fflush(stdout);
	return fgets(buf, buflen, stdin);
}

static void cmd_parser(void)
{
	int largc, rv;
	char *larg;
	char **largv;
	register struct command *c;
	register int ch;
	register int l;

	for (;;) {
		if (!get_input_line(line, sizeof(line))) {
			break;
		}
		l = strlen(line);
		if (l == 0)
			break;
		if (line[--l] == '\n') {
			if (l == 0)
				break;
			line[l] = '\0';
		}
		else if (l == sizeof(line) - 2) {
			printf("sorry, input line too long\n");
			while ((ch = getchar()) != '\n' && ch != EOF)
				/* void */;
			break;
		}
		largv = make_argv(&largc, &larg);
		if (largc == 0) {
			continue;
		}
		c = get_cmd(largv[0]);
		if (c == (struct command *)-1) {
			printf("Ambiguous command\n");
			continue;
		}
		if (c == NULL) {
			printf("Invalid command\n");
			continue;
		}
		if (c->fun) {
			rv = c->fun(largc, largv);
			if (rv == HPI_SHELL_PARM_ERROR) {
				printf("%s\n", c->help);
			} 
			else if (rv == -1) {
				printf("Command failed.\n");
			}
		}
	}
}

static struct command *get_cmd(const char *name)
{
	const char *p, *q;
	struct command *c, *found;
	int nmatches, longest;

	longest = 0;
	nmatches = 0;
	found = 0;
	for (c = commands; (p = c->cmd) != NULL; c++) {
		for (q = name; *q == *p++; q++)
			if (*q == 0)        /* exact match? */
 				return (c);
			if (!*q) {          /* the name was a prefix */
				if (q - name > longest) {
					longest = q - name;
					nmatches = 1;
					found = c;
				} else if (q - name == longest)
					nmatches++;
			}
	}
	if (nmatches > 1)
		return ((struct command *)-1);
	return (found);
}

char **make_argv(int *pargc, char **parg)
{
	static char *rargv[20];
	int rargc = 0;
	char **argp;

	argp = rargv;
	strbase = line;      /* scan from first of buffer */
	argbase = argbuf;       /* store from first of buffer */
	parseflag = 0;
	while ((*argp++ = parse_string())!=NULL)
		rargc++;

	*pargc = rargc;
	if (parg) *parg = altarg;
	return rargv;
}

static char *parse_string(void)
{
	int gotted = 0;
	register char *sb = strbase;
	register char *ap = argbase;
	char *tmp = argbase;        /* will return this if token found */

S0:
	switch (*sb) {
	case '\0':
		goto OUT;

	case ' ':
	case '\t':
		sb++; goto S0;
	default:
		switch (parseflag) {
		case 0:
			parseflag++;
			break;
		case 1:
			parseflag++;
			altarg = sb;
			break;
		default:
			break;
		}
		goto S1;
	}

S1:
	switch (*sb) {
	case ' ':
	case '\t':
	case '\0':
		goto OUT;   /* end of token */
	default:
		*ap++ = *sb++;  /* add character to token */
		gotted = 1;
		goto S1;
	}

OUT:
	if (gotted)
		*ap++ = '\0';
	argbase = ap;           /* update storage pointer */
	strbase = sb;        /* update scan pointer */
	if (gotted) {
		return(tmp);
	}
	switch (parseflag) {
	case 0:
		parseflag++;
		break;
	case 1:
		parseflag++;
		altarg = NULL;
		break;
	default:
		break;
	}
	return NULL;
}

void cmd_shell(void)
{
	for (;;) {
		cmd_parser();
	}
};
