#ifndef _INC_HPI_CMD_H
#define _INC_HPI_CMD_H
#include <SaHpi.h>

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

extern SaHpiSessionIdT sessionid;
extern struct command commands[];
extern int prt_flag;

void gdb(int level);
int open_session(void);
int close_session(void);
void cmd_shell(void);
int sa_show_rpt(SaHpiResourceIdT resourceid);  //show the rpt of a specific resource
int sa_list_res(void);                         //list all rpt information
int sa_show_rdr(SaHpiResourceIdT resourceid);
#endif

