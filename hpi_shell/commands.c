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
 *     Hu Yin     <hu.yin@intel.com>
 *     Racing Guo <racing.guo@intel.com>
 * Changes:
 *	11.30.2004 - Kouzmich < Mikhail.V.Kouzmich@intel.com >:
 *			porting to HPI-B
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <hpi_ui.h>
#include "hpi_cmd.h"

#define	SEV_BUF_SIZE	32

typedef struct {
	char		*name;
	SaHpiSeverityT	val;
} Sev_def_t;

static Sev_def_t Sev_array[] = {
	{"crit", 	SAHPI_CRITICAL},
	{"maj", 	SAHPI_MAJOR},
	{"min", 	SAHPI_MINOR},
	{"inf", 	SAHPI_INFORMATIONAL},
	{"ok", 		SAHPI_OK},
	{"debug", 	SAHPI_DEBUG},
	{"all", 	SAHPI_ALL_SEVERITIES},
	{NULL,	 	0}
};

int ui_print(char *Str)
{
	printf("%s", Str);
	return(0);
}

void help(int as)
//  as = 0  - Available commands
//  as = 1  - help command
{
	command_def_t	*cmd = NULL, *res = (command_def_t *)NULL;
	int		len;
	term_def_t	*term;

	term = get_next_term();
	if ((term == NULL) || (as == 0)) {
		int width = 0;

		printf("Available commands are: \n\n");
		for (cmd = commands; cmd->cmd != NULL; cmd++) {
			if ((cmd->type != MAIN_COM) &&
				(cmd->type != block_type) &&
				(cmd->type != UNDEF_COM))
				continue;
			printf("%-20s", cmd->cmd);
			if ((++width % 4) == 0)
				printf("\n");
		}
		printf("\n");
		return;
	}

	for (;;) {
		register char	*arg;
		int		n;

		arg = term->term;
		len = strlen(arg);
		n = 0;
                for (cmd = commands; cmd->cmd != NULL; cmd++) {
			if ((cmd->type != MAIN_COM) &&
				(cmd->type != block_type) &&
				(cmd->type != UNDEF_COM))
				continue;
                        if (strncmp(cmd->cmd, arg, len) == 0) {
				if (n == 0) res = cmd;
				n++;
			};
			if (strcmp(cmd->cmd, arg) == 0) {
				res = cmd;
				n = 1;
				break;
			}
		}
		if (n != 1)
			printf("Invalid help command %s\n", arg);
		else
			printf("%s\n", res->help);
		term = get_next_term();
		if (term == NULL) break;
	}
}

static ret_code_t help_cmd(void)
{
	help(1);
	return(HPI_SHELL_OK);
}

static ret_code_t add_config(void)
{
//	SaErrorT	rv = SA_OK;
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		printf("no config file\n");
		return HPI_SHELL_CMD_ERROR;
	}
//	rv = oh_process_config_file(term->term);
//	if (rv == SA_ERR_HPI_BUSY) {
//		printf("Hold on. Another configuration changing is"
//		" processing\n");
//	}
//	if (rv == SA_ERR_HPI_NOT_PRESENT) {
//		printf("Hold on. Initialization is processing\n");
//	}
//	return rv;
	return HPI_SHELL_CMD_ERROR;
}
  
static ret_code_t event(void)
{
	term_def_t	*term;

	term = get_next_term();
	if (term == NULL) {
		printf("Event display: %s\n", prt_flag?"Enable":"Disable"); 
		return(HPI_SHELL_OK);
	};
	if (strcmp(term->term, "enable") == 0) {
		prt_flag = 1;
		printf("Event display enable successfully\n"); 
	} else if (strcmp(term->term, "disable") == 0) {
		prt_flag = 0;
		printf("Event display disable successfully\n"); 
	} else if (strcmp(term->term, "short") == 0) {
		show_event_short = 1;
		prt_flag = 1;
		printf("Event short display enable successfully\n"); 
	} else if (strcmp(term->term, "full") == 0) {
		show_event_short = 0;
		prt_flag = 1;
		printf("Event full display enable successfully\n"); 
	} else {
		return HPI_SHELL_PARM_ERROR;
	};
	set_Subscribe((Domain_t *)NULL, prt_flag);

	return HPI_SHELL_OK;
}

static ret_code_t debugset(void)
{
	char		*val;
	term_def_t	*term;

	if (debug_flag) printf("debugset:\n");
	term = get_next_term();
	if (term == NULL) {
		val = getenv("OPENHPI_DEBUG");
		if (val == (char *)NULL) val = "NO";
		printf("OPENHPI_DEBUG=%s\n", val);
		return(HPI_SHELL_OK);
	};
	if (strcmp(term->term, "on") == 0)
		val = "YES";
	else if (strcmp(term->term, "off") == 0)
		val = "NO";
	else
		return HPI_SHELL_PARM_ERROR;
	setenv("OPENHPI_DEBUG", val, 1);

	return HPI_SHELL_OK;
}

static ret_code_t hotswap_stat(void)
{
	SaHpiResourceIdT	resourceid;
	SaErrorT		rv;
	SaHpiHsStateT		state;
	ret_code_t		ret;

	ret = ask_rpt(&resourceid);
	if (ret != HPI_SHELL_OK) return(ret);

	rv = saHpiHotSwapStateGet(Domain->sessionId, resourceid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapStateGet error %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	}

	printf("Current hot swap state of resource %d is:", resourceid);
	switch (state) {
	case SAHPI_HS_STATE_INACTIVE:
		printf("  Inactive.\n");
		break;
	case SAHPI_HS_STATE_INSERTION_PENDING:
		printf("  Insertion Pending.\n");
		break;
	case SAHPI_HS_STATE_ACTIVE:
		printf("  Active.\n");
		break;
	case SAHPI_HS_STATE_EXTRACTION_PENDING:
		printf("  Extraction Pending.\n");
		break;
	case SAHPI_HS_STATE_NOT_PRESENT:
		printf("  Not Present.\n");
		break;
	default:
		printf("  Unknown.\n");
	}

	return HPI_SHELL_OK;
}

static ret_code_t power(void)
{
	SaErrorT		rv;
	SaHpiResourceIdT	resourceid;
	SaHpiPowerStateT	state;
	int			do_set = 1;
	term_def_t		*term;
	ret_code_t		ret;

	ret = ask_rpt(&resourceid);
	if (ret != HPI_SHELL_OK) return(ret);

	term = get_next_term();
	if (term == NULL) do_set = 0;
	else if (!strcmp(term->term, "on")) {
		state = SAHPI_POWER_ON;
	} else if (!strcmp(term->term, "off")) {
		state = SAHPI_POWER_OFF;
	} else if (!strcmp(term->term, "cycle")) {
		state = SAHPI_POWER_CYCLE;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

	if (do_set) {
	        rv = saHpiResourcePowerStateSet(Domain->sessionId, resourceid, state);
       		if (rv != SA_OK) {
			printf("saHpiResourcePowerStateSet error %s\n",
				oh_lookup_error(rv));
			return HPI_SHELL_CMD_ERROR;
		};
		return HPI_SHELL_OK;
	}

        rv = saHpiResourcePowerStateGet(Domain->sessionId, resourceid, &state);
        if (rv != SA_OK) {
                printf("saHpiResourcePowerStateGet error %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	}
	if (state == SAHPI_POWER_ON) {
                printf("Resource %d is power on now.\n",resourceid);
	} else if (state == SAHPI_POWER_OFF) {
                printf("Resource %d is power off now.\n",resourceid);
	}

	return HPI_SHELL_OK;
}

static ret_code_t reset(void)
{
	SaErrorT		rv;
	SaHpiResourceIdT	resourceid;
	SaHpiResetActionT	state;
	int			do_set = 1;
	term_def_t		*term;
	ret_code_t		ret;

	ret = ask_rpt(&resourceid);
	if (ret != HPI_SHELL_OK) return(ret);

	term = get_next_term();
	if (term == NULL) do_set = 0;
	else if (!strcmp(term->term, "cold")) {
		state = SAHPI_COLD_RESET;
	} else if (!strcmp(term->term, "warm")) {
		state = SAHPI_WARM_RESET;
	} else if (!strcmp(term->term, "assert")) {
		state = SAHPI_RESET_ASSERT;
	} else if (!strcmp(term->term, "deassert")) {
		state = SAHPI_RESET_DEASSERT;
	} else {
	 	return HPI_SHELL_PARM_ERROR;
	}

	if (do_set) {
		rv = saHpiResourceResetStateSet(Domain->sessionId, resourceid, state);
		if (rv != SA_OK) {
			printf("saHpiResourceResetStateSet error %s\n",
				oh_lookup_error(rv));
			return HPI_SHELL_CMD_ERROR;
		}
	}

	rv = saHpiResourceResetStateGet(Domain->sessionId, resourceid, &state);
	if (rv != SA_OK) {
		printf("saHpiResourceResetStateGet error %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	}
	if (state == SAHPI_RESET_ASSERT) {
		printf("Entity's reset of %d is asserted now.\n",resourceid);
	} else if (state == SAHPI_RESET_DEASSERT) {
		printf("Entity's reset of %d is not asserted now.\n",resourceid);
	} else {
		printf("Entity's reset of %d is not setted now.\n",resourceid);
	}

	return HPI_SHELL_OK;
}

static ret_code_t clear_evtlog(void)
{
	SaHpiResourceIdT	resourceid;
	term_def_t		*term;
	SaErrorT		rv;

	term = get_next_term();
	if (term == NULL)
		resourceid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	else
		resourceid = (SaHpiResourceIdT)atoi(term->term);

	rv = saHpiEventLogClear(Domain->sessionId, resourceid);
	if (rv != SA_OK) {
		printf("EventLog clear, error = %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	}

	printf("EventLog successfully cleared\n");
	return HPI_SHELL_OK;
}

static ret_code_t show_hs_ind(void)
{
	SaErrorT		rv;
	SaHpiHsIndicatorStateT	state = 0;
	SaHpiResourceIdT	rid;
	int			do_set = 0;
	term_def_t		*term;
	ret_code_t		ret;

	ret = ask_rpt(&rid);
	if (ret != HPI_SHELL_OK) return(ret);
	term = get_next_term();
	if (term != NULL) {
		do_set = 1;
		if (strcmp(term->term, "on") == 0) state = SAHPI_HS_INDICATOR_ON;
		else if (strcmp(term->term, "off") == 0)
			state = SAHPI_HS_INDICATOR_OFF;
		else return HPI_SHELL_PARM_ERROR;
	};
	if (do_set) {
		rv = saHpiHotSwapIndicatorStateSet(Domain->sessionId, rid, state);
		if (rv != SA_OK) { 
			printf("saHpiHotSwapIndicatorStateSet error %s\n",
				oh_lookup_error(rv));
			return HPI_SHELL_CMD_ERROR;
		};
		return(HPI_SHELL_OK);
	};

	rv = saHpiHotSwapIndicatorStateGet(Domain->sessionId, rid, &state);
	if (rv != SA_OK) { 
		printf("saHpiHotSwapIndicatorStateGet error %s\n",
			oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	}

	printf("Current HS Indicator for resource %d is:", rid);
	switch(state) {
		case SAHPI_HS_INDICATOR_OFF:
			printf(" OFF.\n");
			break;
		case SAHPI_HS_INDICATOR_ON:
			printf(" ON.\n");
			break;
		default:
			printf(" Unknown.\n");
	}
	return HPI_SHELL_OK;
}

static ret_code_t set_tag(void)
{
	SaHpiResourceIdT	resid = 0;
	SaHpiTextBufferT	tbuf;
	int			i;
	char			buf[SAHPI_MAX_TEXT_BUFFER_LENGTH + 1];
	SaErrorT		rv;
	SaHpiRptEntryT		rpt_entry;
	Rpt_t			tmp_rpt;
	ret_code_t		ret;

	ret = ask_rpt(&resid);
	if (ret != HPI_SHELL_OK) return(ret);
	i = get_string_param("New tag: ", buf, SAHPI_MAX_TEXT_BUFFER_LENGTH);
	if (i != 0) {
		printf("Invalid tag: %s\n", buf);
		return(HPI_SHELL_PARM_ERROR);
	};
	strcpy(tbuf.Data, buf);
	tbuf.DataType = SAHPI_TL_TYPE_TEXT;
	tbuf.Language = SAHPI_LANG_ENGLISH;
	tbuf.DataLength = i;
	rv = saHpiResourceTagSet(Domain->sessionId, resid, &tbuf);
	if (rv != SA_OK) {
		printf("saHpiResourceTagSet error = %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	};
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (HPI_SHELL_OK);
}

static ret_code_t set_sever(void)
{
	SaHpiResourceIdT	resid;
	SaHpiSeverityT		sev = SAHPI_OK;
	int			i;
	char			buf[SEV_BUF_SIZE + 1];
	SaErrorT		rv;
	SaHpiRptEntryT		rpt_entry;
	Rpt_t			tmp_rpt;
	ret_code_t		ret;

	ret = ask_rpt(&resid);
	if (ret != HPI_SHELL_OK) return(ret);
	i = get_string_param("New severity (crit, maj, min, inf, ok, debug, all): ",
		buf, SEV_BUF_SIZE);
	if (i != 0) {
		printf("Invalid sevetity: %s\n", buf);
		return(HPI_SHELL_PARM_ERROR);
	};
	for (i = 0; Sev_array[i].name != (char *)NULL; i++)
		if (strcmp(buf, Sev_array[i].name) == 0) {
			sev = Sev_array[i].val;
			break;
		};
	if (Sev_array[i].name == (char *)NULL) {
		printf("Invalid sevetity type: %s\n", buf);
		return(HPI_SHELL_PARM_ERROR);
	};
	rv = saHpiResourceSeveritySet(Domain->sessionId, resid, sev);
	if (rv != SA_OK) {
		printf("saHpiResourceSeveritySet error = %s\n", oh_lookup_error(rv));
		return HPI_SHELL_CMD_ERROR;
	};
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (HPI_SHELL_OK);
}

static ret_code_t discovery(void)
{
	SaErrorT        ret;

	do_progress("Discover");
        ret = saHpiDiscover(Domain->sessionId);
        if (SA_OK != ret) {
                printf("saHpiResourcesDiscover error = %s\n",
			oh_lookup_error(ret));
		delete_progress();
        	return HPI_SHELL_CMD_ERROR;
	};
	delete_progress();
        return HPI_SHELL_OK;
}

static ret_code_t dat_list(void)
{
	return show_dat(Domain, ui_print);
}

static ret_code_t listres(void)
{
	show_rpt_list(Domain, SHOW_ALL_RPT, 0, ui_print);
	return(HPI_SHELL_OK);
}

static ret_code_t show_evtlog(void)
{
	SaHpiResourceIdT	rptid = 0;
	term_def_t		*term;

	term = get_next_term();
	if (term == NULL)
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	else
		rptid = (SaHpiResourceIdT)atoi(term->term);

	return show_event_log(Domain->sessionId, rptid, show_event_short, ui_print);
}

static ret_code_t evtlog_time(void)
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiTimeT		logtime;
	SaHpiTextBufferT	buffer;
	term_def_t		*term;

	term = get_next_term();
	if (term == NULL)
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	else
		rptid = (SaHpiResourceIdT)atoi(term->term);

	rv = saHpiEventLogTimeGet(Domain->sessionId, rptid, &logtime);
	if (rv != SA_OK) 
	{
		printf("saHpiEventLogTimeGet %s\n", oh_lookup_error(rv));
		return (HPI_SHELL_CMD_ERROR);
	}

	oh_decode_time(logtime, &buffer);
	printf ("Current event log time: %s\n", buffer.Data);
	return HPI_SHELL_OK;
}

static ret_code_t evtlog_state(void)
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiBoolT		state = SAHPI_TRUE;
	int			do_set = 0;
	char			*str;
	term_def_t		*term;

	rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	term = get_next_term();
	if ((term != NULL) && (isdigit(term->term[0])))
		rptid = (SaHpiResourceIdT)atoi(term->term);

	term = get_next_term();
	if (term != NULL) {
		do_set = 1;
		if (strcmp(term->term, "enable") == 0) state = SAHPI_TRUE;
		else if (strcmp(term->term, "disable") == 0) state = SAHPI_FALSE;
		else return(HPI_SHELL_PARM_ERROR);
	};
	if (do_set) {
		rv = saHpiEventLogStateSet(Domain->sessionId, rptid, state);
		if (rv != SA_OK) {
			printf("saHpiEventLogStateSet %s\n", oh_lookup_error(rv));
			return(HPI_SHELL_CMD_ERROR);
		};
		return(HPI_SHELL_OK);
	};
	rv = saHpiEventLogStateGet(Domain->sessionId, rptid, &state);
	if (rv != SA_OK) {
		printf("saHpiEventLogStateGet %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	if (state == SAHPI_TRUE) str = "Enable";
	else str = "Disable";
	printf("Event Log State: %s\n", str);
	return HPI_SHELL_OK;
}

static ret_code_t evtlog_reset(void)
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	term_def_t		*term;

	rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	term = get_next_term();
	if ((term != NULL) && (isdigit(term->term[0])))
		rptid = (SaHpiResourceIdT)atoi(term->term);

	rv = saHpiEventLogOverflowReset(Domain->sessionId, rptid);
	if (rv != SA_OK) {
		printf("saHpiEventLogOverflowReset %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	return HPI_SHELL_OK;
}

static ret_code_t settime_evtlog(void)
{
	SaHpiResourceIdT	rptid = 0;
	SaErrorT		rv;
	SaHpiTimeT		newtime;
	struct tm		new_tm_time;
	char			buf[READ_BUF_SIZE];
	int			day_array[] = { 31, 28, 31, 30, 31, 30, 31,
						31, 30, 31, 30, 31 };
	term_def_t		*term;
	int			i;

	rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
	term = get_next_term();
	if ((term != NULL) && (isdigit(term->term[0]))) {
		if (strchr(term->term, ':') != (char *)NULL) {
			unget_term();
			rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;
		} else 
			rptid = (SaHpiResourceIdT)atoi(term->term);
	} else
		rptid = SAHPI_UNSPECIFIED_RESOURCE_ID;

	if (rptid == SAHPI_UNSPECIFIED_RESOURCE_ID)
		printf("Set date and time for Domain Event Log!\n");
	else
		printf("Set date and time for Resource %d!\n", rptid);

	memset(&new_tm_time, 0, sizeof(struct tm));
	i = get_string_param("format: MM:DD:YYYY:hh:mm:ss ==> ", buf, READ_BUF_SIZE);
	if (i != 0) return(HPI_SHELL_PARM_ERROR);
	sscanf(buf, "%d:%d:%d:%d:%d:%d", &new_tm_time.tm_mon, &new_tm_time.tm_mday,
		&new_tm_time.tm_year, &new_tm_time.tm_hour, &new_tm_time.tm_min,
		&new_tm_time.tm_sec);
	if ((new_tm_time.tm_mon < 1) || (new_tm_time.tm_mon > 12)) {
		printf("Month out of range: (%d)\n", new_tm_time.tm_mon);
		return(HPI_SHELL_PARM_ERROR);
	};
	new_tm_time.tm_mon--;
	if (new_tm_time.tm_year < 1900) {
		printf("Year out of range: (%d)\n", new_tm_time.tm_year);
		return(HPI_SHELL_PARM_ERROR);
	};
	if (new_tm_time.tm_mon == 1) {
	/* if the given year is a leap year */
		if ((((new_tm_time.tm_year % 4) == 0)
			&& ((new_tm_time.tm_year % 100) != 0))
			|| ((new_tm_time.tm_year % 400) == 0))
			day_array[1] = 29;
	};

	if ((new_tm_time.tm_mday < 1) ||
		(new_tm_time.tm_mday > day_array[new_tm_time.tm_mon])) {
		printf("Day out of range: (%d)\n", new_tm_time.tm_mday);
		return(HPI_SHELL_PARM_ERROR);
	};

	new_tm_time.tm_year -= 1900;
	
	if ((new_tm_time.tm_hour < 0) || (new_tm_time.tm_hour > 24)) {
		printf("Hours out of range: (%d)\n", new_tm_time.tm_hour);
		return(HPI_SHELL_PARM_ERROR);
	};
	if ((new_tm_time.tm_min < 0) || (new_tm_time.tm_min > 60)) {
		printf("Minutes out of range: (%d)\n", new_tm_time.tm_min);
		return(HPI_SHELL_PARM_ERROR);
	};
	if ((new_tm_time.tm_sec < 0) || (new_tm_time.tm_sec > 60)) {
		printf("Seconds out of range: (%d)\n", new_tm_time.tm_sec);
		return(HPI_SHELL_PARM_ERROR);
	};

	newtime = (SaHpiTimeT) mktime(&new_tm_time) * 1000000000;
 	rv = saHpiEventLogTimeSet(Domain->sessionId, rptid, newtime);
	if (rv != SA_OK) 
	{
		printf("saHpiEventLogTimeSet %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	}

	return (HPI_SHELL_OK);
}

static ret_code_t show_rpt(void)
{
	Rpt_t			tmp_rpt;
	SaHpiRptEntryT		rpt_entry;
	SaErrorT		rv;
	SaHpiResourceIdT	resid;
	ret_code_t		ret;

	ret = ask_rpt(&resid);
	if (ret != HPI_SHELL_OK) return(ret);
	rv = saHpiRptEntryGetByResourceId(Domain->sessionId, resid, &rpt_entry);
	if (rv != SA_OK) {
		printf("NO rpt: %d\n", resid);
		return(HPI_SHELL_CMD_ERROR);
	};
	make_attrs_rpt(&tmp_rpt, &rpt_entry);
	show_Rpt(&tmp_rpt, ui_print);
	free_attrs(&(tmp_rpt.Attrutes));
	return (HPI_SHELL_OK);
}

static ret_code_t show_rdr(void)
{
	Rdr_t			tmp_rdr;
	SaHpiRdrT		rdr_entry;
	SaHpiResourceIdT	rptid = 0;
	SaHpiInstrumentIdT	rdrnum;
	SaHpiRdrTypeT		type;
	SaErrorT		rv;
	int			i;
	char			buf[10], t;
	term_def_t		*term;
	ret_code_t		ret;

	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_CMD_ERROR);
		i = show_rpt_list(Domain, SHOW_ALL_RPT, rptid, ui_print);
		if (i == 0) {
			printf("NO rpt!\n");
			return(HPI_SHELL_CMD_ERROR);
		};
		i = get_string_param("RPT (ID | all) ==> ", buf, 9);
		if (i != 0) return HPI_SHELL_CMD_ERROR;
		if (strncmp(buf, "all", 3) == 0) {
			show_rpt_list(Domain, SHOW_ALL_RDR, rptid, ui_print);
			return(HPI_SHELL_OK);
		};
		rptid = (SaHpiResourceIdT)atoi(buf);
	} else {
		if (strcmp(term->term, "all") == 0) {
			show_rpt_list(Domain, SHOW_ALL_RDR, rptid, ui_print);
			return(HPI_SHELL_OK);
		};
		if (isdigit(term->term[0]))
			rptid = (SaHpiResourceIdT)atoi(term->term);
		else
			return HPI_SHELL_PARM_ERROR;
	};
	term = get_next_term();
	if (term == NULL) {
		if (read_file) return(HPI_SHELL_CMD_ERROR);
		i = get_string_param("RDR Type (s|a|c|w|i|all) ==> ", buf, 9);
		if (i != 0) return HPI_SHELL_PARM_ERROR;
	} else {
		memset(buf, 0, 10);
		strncpy(buf, term->term, 3);
	};
	if (strncmp(buf, "all", 3) == 0) t = 'n';
	else t = *buf;
	if (t == 'c') type = SAHPI_CTRL_RDR;
	else if (t == 's') type = SAHPI_SENSOR_RDR;
	else if (t == 'i') type = SAHPI_INVENTORY_RDR;
	else if (t == 'w') type = SAHPI_WATCHDOG_RDR;
	else if (t == 'a') type = SAHPI_ANNUNCIATOR_RDR;
	else type = SAHPI_NO_RECORD;
	ret = ask_rdr(rptid, type, &rdrnum);
	if (ret != HPI_SHELL_OK) return(ret);
	if (type == SAHPI_NO_RECORD)
		rv = find_rdr_by_num(Domain->sessionId, rptid, rdrnum, type, 0,
			&rdr_entry);
	else
		rv = saHpiRdrGetByInstrumentId(Domain->sessionId, rptid, type, rdrnum,
			&rdr_entry);
	if (rv != SA_OK) {
		printf("ERROR!!! Get rdr: ResourceId=%d RdrType=%d RdrNum=%d: %s\n",
			rptid, type, rdrnum, oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	make_attrs_rdr(&tmp_rdr, &rdr_entry);
	show_Rdr(&tmp_rdr, ui_print);
	free_attrs(&(tmp_rdr.Attrutes));
	return HPI_SHELL_OK;
}

static ret_code_t quit(void)
{
	if (block_type != MAIN_COM) {
		unget_term();
		return(HPI_SHELL_OK);
	};
        printf("quit\n");
        close_session();
        exit(0);
}

static ret_code_t run(void)
{
	term_def_t	*term;
	char		*path;

	if (read_file) return(HPI_SHELL_CMD_ERROR);
	term = get_next_term();
	if (term == NULL) return(HPI_SHELL_PARM_ERROR);
	path = term->term;
	return(open_file(path));
}

static ret_code_t echo(void)
{
	term_def_t	*term;

	term = get_next_term();
	if (term != NULL)
		printf("%s\n", term->term);
	return(HPI_SHELL_OK);
}

static ret_code_t domain_info(void)
{
	SaHpiDomainInfoT	info;
	SaHpiTextBufferT	*buf;
	SaErrorT		rv;
	char			date[30];

	rv = saHpiDomainInfoGet(Domain->sessionId, &info);
	if (rv != SA_OK) {
		printf("ERROR!!! saHpiDomainInfoGet: %s\n", oh_lookup_error(rv));
		return(HPI_SHELL_CMD_ERROR);
	};
	printf("Domain: %d   Capabil: 0x%x   IsPeer: %d   Guid: %s\n",
		info.DomainId, info.DomainCapabilities,
		info.IsPeer, info.Guid);
	buf = &(info.DomainTag);
	if (buf->DataLength > 0)
		printf("    Tag: %s\n", buf->Data);
	time2str(info.DrtUpdateTimestamp, date, 30);
	printf("    DRT update count: %d   DRT Timestamp : %s\n",
		info.DrtUpdateCount, date);
	time2str(info.RptUpdateTimestamp, date, 30);
	printf("    RPT update count: %d   RPT Timestamp : %s\n",
		info.RptUpdateCount, date);
	time2str(info.DatUpdateTimestamp, date, 30);
	printf("    DAT update count: %d   DAT Timestamp : %s\n",
		info.DatUpdateCount, date);
	printf("        ActiveAlarms: %d   CriticalAlarms: %d   Major: %d"
		"Minor: %d   Limit: %d\n",
		info.ActiveAlarms, info.CriticalAlarms, info.MajorAlarms,
		info.MinorAlarms, info.DatUserAlarmLimit);
	printf("        DatOverflow : %d\n", info.DatOverflow);
	return(HPI_SHELL_OK);
}

static ret_code_t domain_proc(void)
{
	SaHpiDomainInfoT	info;
	SaHpiTextBufferT	*buf;
	SaHpiEntryIdT		entryid, nextentryid;
	SaHpiDrtEntryT		drtentry;
	SaErrorT		rv;
	SaHpiDomainIdT		id;
	SaHpiSessionIdT		sessionId;
	int			i, n, first;
	gpointer		ptr;
	Domain_t		*domain = (Domain_t *)NULL;
	Domain_t		*new_domain;
	term_def_t		*term;

	term = get_next_term();
	if (term == NULL) {
		printf("Domain list:\n");
		printf("    ID: %d   SessionId: %d", Domain->domainId,
			Domain->sessionId);
		rv = saHpiDomainInfoGet(Domain->sessionId, &info);
		if (rv == SA_OK) {
			buf = &(info.DomainTag);
			if (buf->DataLength > 0)
				printf("    Tag: %s", buf->Data);
		};
		printf("\n");
		entryid = SAHPI_FIRST_ENTRY;
		first = 1;
		while (entryid != SAHPI_LAST_ENTRY) {
			rv = saHpiDrtEntryGet(Domain->sessionId, entryid,
				&nextentryid, &drtentry);
			if (rv != SA_OK) break;
			if (first) {
				first = 0;
				printf("        Domain Reference Table:\n");
			};
			printf("            ID: %d", drtentry.DomainId);
			entryid = nextentryid;
			rv = saHpiSessionOpen(drtentry.DomainId,
						&sessionId, NULL);
			if (rv != SA_OK) {
				printf("\n");
				continue;
			};
			rv = saHpiDomainInfoGet(sessionId, &info);
			if (rv == SA_OK) {
				buf = &(info.DomainTag);
				if (buf->DataLength > 0)
					printf("    Tag: %s", buf->Data);
			};
			saHpiSessionClose(sessionId);
			printf("\n");
		}
		return(HPI_SHELL_OK);
	};

	if (isdigit(term->term[0]))
		id = (int)atoi(term->term);
	else
		return HPI_SHELL_PARM_ERROR;
	n = g_slist_length(domainlist);
	for (i = 0; i < n; i++) {
		ptr = g_slist_nth_data(domainlist, i);
		if (ptr == (gpointer)NULL) break;
		domain = (Domain_t *)ptr;
		if (domain->domainId == id) break;
	};
	if (i >= n) {
		new_domain = (Domain_t *)malloc(sizeof(Domain_t));
		memset(new_domain, 0, sizeof(Domain_t));
		new_domain->domainId = id;
		if (add_domain(new_domain) < 0) {
			free(new_domain);
			printf("Can not open domain: %d\n", id);
			return HPI_SHELL_PARM_ERROR;
		};
		domain = new_domain;
	};
	Domain = domain;
	set_Subscribe(Domain, prt_flag);
	add_domain(Domain);
	return(HPI_SHELL_OK);
}

/* command table */
const char addcfghelp[] = "addcfg: add plugins, domains, handlers from"
			" config file\nUsage: addcfg <config file>\n";
const char clevtloghelp[] = "clearevtlog: clear system event logs\n"
			"Usage: clearevtlog [<resource id>]";
const char ctrlhelp[] =	"ctrl: control command block\n"
			"Usage: ctrl [<ctrlId>]\n"
			"	ctrlId:: <resourceId> <num>\n";
const char dathelp[] = "dat: domain alarm table list\n"
			"Usage: dat";
const char debughelp[] = "debug: set or unset OPENHPI_DEBUG environment\n"
			"Usage: debug [ on | off ]";
const char domainhelp[] = "domain: show domain list and set current domain\n"
			"Usage: domain [<domain id>]";
const char domaininfohelp[] = "domaininfo: show current domain info\n"
			"Usage: domaininfo";
const char dscvhelp[] = "dscv: discovery resources\n"
			"Usage: dscv ";
const char echohelp[] = "echo: pass string to the stdout\n"
			"Usage: echo <string>";
const char eventhelp[] = "event: enable or disable event display on screen\n"
			"Usage: event [enable|disable|short|full] ";
const char evtlresethelp[] = "evtlogreset: reset the OverflowFlag in the event log\n"
			"Usage: evtlogreset [<resource id>]";
const char evtlstatehelp[] = "evtlogstate: show and set the event log state\n"
			"Usage: evtlogstate [<resource id>] [enable|disable]";
const char evtlogtimehelp[] = "evtlogtime: show the event log's clock\n"
			"Usage: evtlogtime [<resource id>]";
const char helphelp[] = "help: help information for OpenHPI commands\n"
			"Usage: help [optional commands]";
const char hsindhelp[] = "hotswap_ind: show hot swap indicator state\n"
			"Usage: hotswap_ind <resource id>";
const char hsstathelp[] = "hotswapstat: retrieve hot swap state of a resource\n"
			"Usage: hotswapstat <resource id>";
const char invhelp[] =	"inv: inventory command block\n"
			"Usage: inv [<InvId>]\n"
			"	InvId:: <resourceId> <IdrId>\n";
const char lreshelp[] = "lsres: list resources\n"
			"Usage: lsres";
const char lsorhelp[] = "lsensor: list sensors\n"
			"Usage: lsensor";
const char powerhelp[] = "power: power the resource on, off or cycle\n"
			"Usage: power <resource id> [on|off|cycle]";
const char quithelp[] = "quit: close session and quit console\n"
			"Usage: quit";
const char resethelp[] = "reset: perform specified reset on the entity\n"
			"Usage: reset <resource id> [cold|warm|assert|deassert]";
const char runhelp[] = "run: execute command file\n"
			"Usage: run <file name>";
const char senhelp[] =	"sen: sensor command block\n"
			"Usage: sen [<sensorId>]\n"
			"	sensorId:: <resourceId> <num>\n";
const char setseverhelp[] = "setsever: set severity for a resource\n"
			"Usage: setsever [<resource id>]";
const char settaghelp[] = "settag: set tag for a particular resource\n"
			"Usage: settag [<resource id>]";
const char settmevtlhelp[] = "settimeevtlog: sets the event log's clock\n"
			"Usage: settimeevtlog [<resource id>]";
const char showevtloghelp[] = "showevtlog: show system event logs\n"
			"Usage: showevtlog [<resource id>]";
const char showinvhelp[] = "showinv: show inventory data of a resource\n"
			"Usage: showinv [<resource id>]";
const char showrdrhelp[] = "showrdr: show resource data record\n"
			"Usage: showrdr [<resource id> [type [<rdr num>]]]\n"
			"   or  rdr [<resource id> [type [<rdr num>]]]\n"
			"	type =	c - control rdr, s - sensor, i - inventory rdr\n"
			"		w - watchdog, a - annunciator, all - all rdr";
const char showrpthelp[] = "showrpt: show resource information\n"
			"Usage: showrpt [<resource id>]\n"
			"   or  rpt [<resource id>]";
//  sensor command block
const char sen_dishelp[] = "disable: set sensor disable\n"
			"Usage: disable";
const char sen_enbhelp[] = "enable: set sensor enable\n"
			"Usage: enable";
const char sen_evtenbhelp[] = "evtenb: set sensor event enable\n"
			"Usage: evtenb";
const char sen_evtdishelp[] = "evtdis: set sensor event disable\n"
			"Usage: evtdis";
const char sen_mskaddhelp[] = "maskadd: add sensor event masks\n"
			"Usage: maskadd";
const char sen_mskrmhelp[] = "maskrm: remove sensor event masks\n"
			"Usage: maskrm";
const char sen_setthhelp[] = "setthres: set sensor thresholds\n"
			"Usage: setthres";
const char sen_showhelp[] = "show: show sensor state\n"
			"Usage: show";
//  inventory command block
const char inv_addahelp[] = "addarea: add inventoty area\n"
			"Usage: addarea";
const char inv_addfhelp[] = "addfield: add inventoty field\n"
			"Usage: addfield";
const char inv_delahelp[] = "delarea: delete inventoty area\n"
			"Usage: delarea";
const char inv_delfhelp[] = "delfield: delete inventoty field\n"
			"Usage: delfield";
const char inv_setfhelp[] = "setfield: set inventoty field\n"
			"Usage: setfield";
const char inv_showhelp[] = "show: show inventoty\n"
			"Usage: show";
//  control command block
const char ctrl_sthelp[] = "setstate: set control state\n"
			"Usage: setstate";
const char ctrl_showhelp[] = "show: show control\n"
			"Usage: show";
const char ctrl_setsthelp[] = "state: show control state\n"
			"Usage: state";

command_def_t commands[] = {
    { "addcfg",		add_config,	addcfghelp,	MAIN_COM },
    { "clearevtlog",	clear_evtlog,	clevtloghelp,	MAIN_COM },
    { "ctrl",		ctrl_block,	ctrlhelp,	MAIN_COM },
    { "dat",		dat_list,	dathelp,	MAIN_COM },
    { "debug",		debugset,	debughelp,	UNDEF_COM },
    { "domain",		domain_proc,	domainhelp,	MAIN_COM },
    { "domaininfo",	domain_info,	domaininfohelp,	MAIN_COM },
    { "dscv",		discovery,	dscvhelp,	MAIN_COM },
    { "echo",		echo,		echohelp,	UNDEF_COM },
    { "event",		event,		eventhelp,	MAIN_COM },
    { "evtlogtime",	evtlog_time,	evtlogtimehelp,	MAIN_COM },
    { "evtlogreset",	evtlog_reset,	evtlresethelp,	MAIN_COM },
    { "evtlogstate",	evtlog_state,	evtlstatehelp,	MAIN_COM },
    { "help",		help_cmd,	helphelp,	UNDEF_COM },
    { "hotswap_ind",	show_hs_ind,	hsindhelp,	MAIN_COM },
    { "hotswapstat",	hotswap_stat,	hsstathelp,	MAIN_COM },
    { "inv",		inv_block,	invhelp,	MAIN_COM },
    { "lsres",		listres,	lreshelp,	MAIN_COM },
    { "lsensor",	list_sensor,	lsorhelp,	MAIN_COM },
    { "power",		power,		powerhelp,	MAIN_COM },
    { "quit",		quit,		quithelp,	UNDEF_COM },
    { "rdr",		show_rdr,	showrdrhelp,	MAIN_COM },
    { "reset",		reset,		resethelp,	MAIN_COM },
    { "rpt",		show_rpt,	showrpthelp,	MAIN_COM },
    { "run",		run,		runhelp,	MAIN_COM },
    { "sen",		sen_block,	senhelp,	MAIN_COM },
    { "settag",		set_tag,	settaghelp,	MAIN_COM },
    { "setsever",	set_sever,	setseverhelp,	MAIN_COM },
    { "settimeevtlog",	settime_evtlog,	settmevtlhelp,	MAIN_COM },
    { "showevtlog",	show_evtlog,	showevtloghelp,	MAIN_COM },
    { "showinv",	show_inv,	showinvhelp,	MAIN_COM },
    { "showrdr",	show_rdr,	showrdrhelp,	MAIN_COM },
    { "showrpt",	show_rpt,	showrpthelp,	MAIN_COM },
    { "?",		help_cmd,	helphelp,	UNDEF_COM },
//  sensor command block
    { "enable",		unget_term,	sen_enbhelp,	SEN_COM },
    { "evtdis",		unget_term,	sen_evtdishelp,	SEN_COM },
    { "evtenb",		unget_term,	sen_evtenbhelp,	SEN_COM },
    { "disable",	unget_term,	sen_dishelp,	SEN_COM },
    { "maskadd",	unget_term,	sen_mskaddhelp,	SEN_COM },
    { "maskrm",		unget_term,	sen_mskrmhelp,	SEN_COM },
    { "setthres",	unget_term,	sen_setthhelp,	SEN_COM },
    { "show",		unget_term,	sen_showhelp,	SEN_COM },
//  inventory command block
    { "addarea",	unget_term,	inv_addahelp,	INV_COM },
    { "addfield",	unget_term,	inv_addfhelp,	INV_COM },
    { "delarea",	unget_term,	inv_delahelp,	INV_COM },
    { "delfield",	unget_term,	inv_delfhelp,	INV_COM },
    { "setfield",	unget_term,	inv_setfhelp,	INV_COM },
    { "show",		unget_term,	inv_showhelp,	INV_COM },
//  control command block
    { "setstate",	unget_term,	ctrl_setsthelp,	CTRL_COM },
    { "show",		unget_term,	ctrl_showhelp,	CTRL_COM },
    { "state",		unget_term,	ctrl_sthelp,	CTRL_COM },
    { NULL,		NULL,		NULL,		MAIN_COM }
};
