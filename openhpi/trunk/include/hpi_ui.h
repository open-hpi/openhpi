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
 * Author(s):
 *      Kouzmich	< Mikhail.V.Kouzmich@intel.com >
 *
 */

#ifndef __HPI_UI_H
#define __HPI_UI_H

#include <SaHpi.h>
#include <oh_utils.h>

typedef union {
	int	i;
	double	d;
	char	*s;
	void	*a;
} union_type_t;

/*	value types	*/

#define	NO_TYPE		0	// value doesn't present
#define	INT_TYPE	1	// interger
#define	FLOAT_TYPE	2	// float
#define	STR_TYPE	3	// string
#define	STRUCT_TYPE	4	// structure
#define	ARRAY_TYPE	5	// array

/*	attribute flags		*/

#define	MOD_ENABLE_FLAG		0x01	// modification enable
#define MODIFY_FLAG		0x02	// value was modified

typedef struct {
	char		*name;		// attribute name
	int		type;		// value type
	int		flags;		// attribute flags
	union_type_t	value;		// value
	union_type_t	orig_value;	// original value for recovery
} attr_t;

typedef struct {
	int	n_attrs;
	attr_t	*Attrs;
} Attributes_t;

typedef struct {
	SaHpiRdrT	*Rdr;
	SaHpiEntryIdT	RecordId;
	SaHpiRdrTypeT	RdrType;
	Attributes_t	Attrutes;
	int		is_inited;
} Rdr_t;

typedef struct {
	SaHpiRptEntryT		*Rpt;
	SaHpiEntryIdT		EntryId;
	SaHpiResourceIdT	ResourceId;
	Attributes_t		Attrutes;
	int			is_inited;
	int			n_rdrs;
	Rdr_t			*rdrs;
} Rpt_t;

typedef struct {
	SaHpiSessionIdT		sessionId;
	SaHpiDomainIdT		domainId;
	int			n_rpts;
	Rpt_t			*rpts;
} Domain_t;

typedef int (*hpi_ui_print_cb_t)(char *buf);

extern Domain_t	*init_resources(SaHpiSessionIdT session);	// create resources tree
extern SaErrorT	check_resources(Domain_t *Domain);	// check resources tree
extern SaErrorT	check_rpt(Domain_t *Domain, SaHpiResourceIdT rptId);
			// check resources for RPT rptId
extern SaErrorT	check_rdr(Domain_t *Domain, SaHpiResourceIdT rptId,
				SaHpiEntryIdT rdrId);	// check rdr rdrID for rptId
extern Rdr_t	*get_rdr(Domain_t *Domain, SaHpiResourceIdT rptId, SaHpiSensorNumT num);
extern Rpt_t	*get_rpt(Domain_t *Domain, SaHpiResourceIdT rptId);
			// get resources for RPT rptId
extern SaErrorT	get_rpt_attr(Rpt_t *rpt, char *attr_name, union_type_t *val);
			// get rpt attribute value
extern SaErrorT	get_rpt_attr_as_string(Rpt_t *rpt, char *attr_name, char *val, int len);
			// get rpt attribute value as string (max length: len)
extern SaErrorT	get_rdr_attr(Rdr_t *rdr, char *attr_name, union_type_t *val);
			// get rdr attribute value
extern SaErrorT	get_rdr_attr_as_string(Rdr_t *rdr, char *attr_name, char *val, int len);
			// get rdr attribute value as string (max length: len)
extern SaErrorT	set_rpt_attr(Rpt_t *rpt, char *attr_name, union_type_t val);
			// set rpt attribute value
extern SaErrorT	set_rdr_attr(Rdr_t *rdr, char *attr_name, union_type_t val);
			// set rdr attribute value
extern SaErrorT	get_value(Attributes_t *Attrs, int num, union_type_t *val);
			// get attribute value as string by number (max length: len)
extern SaErrorT	get_value_as_string(Attributes_t *Attrs, int num, char *val, int len);
			// get attribute value as string by number (max length: len)
extern char	*get_attr_name(Attributes_t *Attrs, int num);
			// get attribute name
extern int	get_attr_type(Attributes_t *Attrs, int num);
			// get attribute type


extern SaErrorT	show_event_log(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
			int show_short, hpi_ui_print_cb_t proc);
extern SaErrorT	show_Rdr(Rdr_t *Rdr, hpi_ui_print_cb_t proc);
extern SaErrorT	show_Rpt(Rpt_t *Rpt, hpi_ui_print_cb_t proc);
extern SaErrorT	show_rpt_list(Domain_t *D, hpi_ui_print_cb_t proc);
extern SaErrorT	show_sensor_list(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
			hpi_ui_print_cb_t proc);
extern SaErrorT	show_sensor_status(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
			SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc);
extern SaErrorT	show_sensor(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
			SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc);
extern void	show_short_event(SaHpiEventT *event);
extern SaErrorT	show_threshold(SaHpiSessionIdT sessionid, SaHpiResourceIdT resourceid,
			SaHpiSensorNumT sensornum, hpi_ui_print_cb_t proc);
extern int	print_thres_value(SaHpiSensorReadingT *item, char *mes,
			hpi_ui_print_cb_t proc);
extern SaErrorT	sensor_list(SaHpiSessionIdT sessionid, hpi_ui_print_cb_t proc);
extern void	time2str(SaHpiTimeT time, char * str, size_t size);

#endif /* __HPI_UI_H */

 
