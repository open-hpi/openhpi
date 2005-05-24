/*      -*- linux-c -*-
 *
 * (C) Copyright IBM Corp. 2003, 2004
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *      Christina Hernandez <hernanc@us.ibm.com>
 *
 */

#include <sim_controls.h>
#include <sim_init.h>
#include <sim_resources.h>
#include <rpt_utils.h>

SaErrorT sim_discover_controls(RPTable *rpt)
{
	SaHpiRptEntryT *res;
	
	/* add to first resource */
	res = oh_get_resource_next(rpt, SAHPI_FIRST_ENTRY);
	if (!res){
		dbg("resource not fond");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1); /* add #1 ... */
	new_control(rpt, res->ResourceId, 5); /* add #5 ... */

	/* add to second resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1);
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 3);

	/* add to third resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 6);

	/* add to fourth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 3);
	new_control(rpt, res->ResourceId, 5);

	/* add to fifth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 2);

	/* add to sixth resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 1);
	new_control(rpt, res->ResourceId, 2);
	new_control(rpt, res->ResourceId, 3);
	new_control(rpt, res->ResourceId, 4);

	/* add to seventh resource */
	res = oh_get_resource_next(rpt, res->ResourceId);
	if (!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 4);
	new_control(rpt, res->ResourceId, 2);

	/*add to eighth resource*/
	res = oh_get_resource_next(rpt, res->ResourceId);
	if(!res){
		dbg("entity_root is needed and not present");
		return 1;
	}
	new_control(rpt, res->ResourceId, 5);

        return 0;

}

SaErrorT new_control(RPTable *rptcache, SaHpiResourceIdT ResId, int Index){
	SaHpiRdrT res_rdr;
	SaHpiRptEntryT *RptEntry;

	// Copy information from rdr array to res_rdr
	res_rdr.RdrType = SAHPI_CONTROL_RDR;
	memcpy(&res_rdr.RdrTypeUnion.CtrlRec, &sim_control_rdrs[Index].RdrTypeUnion.CtrlRec, sizeof(SaHpiCtrlRecT));
	
	oh_init_textbuffer(&res_rdr.IdString);
	oh_append_textbuffer(&res_rdr.IdString, sim_control_rdrs[Index].comment);
	res_rdr.RdrTypeUnion.CtrlRec.Num = sim_get_next_control_num(rptcache, ResId, res_rdr.RdrTypeUnion.CtrlRec.Type);
	res_rdr.RecordId = get_rdr_uid(res_rdr.RdrType, res_rdr.RdrTypeUnion.CtrlRec.Num);
	
	RptEntry = oh_get_resource_by_id(rptcache, ResId);
	if(!RptEntry){
		dbg("NULL rpt pointer\n");
	}
	else{
		res_rdr.Entity = RptEntry->ResourceEntity;
	}
	
	oh_add_rdr(rptcache, ResId, &res_rdr, NULL, 0);
	
	return 0;
}
	

int sim_get_next_control_num(RPTable *rptcache, SaHpiResourceIdT ResId, SaHpiRdrTypeT type)
{
	int i=0;
	SaHpiRdrT *RdrEntry;
	RdrEntry = oh_get_rdr_next(rptcache, ResId, SAHPI_FIRST_ENTRY);
	while(RdrEntry){
		if (RdrEntry->RdrType == type){
			i++;
		}
		if (RdrEntry->RecordId != 0){
			RdrEntry = oh_get_rdr_next(rptcache, ResId, RdrEntry->RecordId);
		}
	}
	return i;

	if(!(oh_get_rdr_by_type(rptcache, ResId, type, i))){
		printf("I hit sim_get_next_control_num\n");
		return i;
	}
	else{
		while(oh_get_rdr_by_type(rptcache, ResId, type, i)){
			i++;
		}
		return i;
	}
}



struct sim_controls sim_control_rdrs[] = {

        /* First control rdr */
	/* This is an RDR representing a digital control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_LED,
                                .Type = SAHPI_CTRL_TYPE_DIGITAL,
                                .TypeUnion = {
                                        .Digital = {
                                                .Default = SAHPI_CTRL_STATE_OFF,
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Digital-Control-1")
        },
        
	/* Second control rdr */
        /* This is an RDR representing a discrete control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_FAN_SPEED,
                                .Type = SAHPI_CTRL_TYPE_DISCRETE,
                                .TypeUnion = {
                                        .Discrete = {
                                                .Default = 5005,
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Discrete-Control-1")
        },
        /* Third control rdr */
        /* This is an RDR representing an analog control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_FAN_SPEED,
                                .Type = SAHPI_CTRL_TYPE_ANALOG,
                                .TypeUnion = {
                                        .Analog = {
						.Min = 2000,
						.Max = 4000,
                                                .Default = 3000
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Analog-Control-1")
        },
        /* Fourth control rdr */
        /* This is an RDR representing a stream control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_LCD_DISPLAY,
                                .Type = SAHPI_CTRL_TYPE_STREAM,
                                .TypeUnion = {
                                        .Stream = {
                                                .Default = {
							.Repeat = 0,
							.StreamLength = 3,
							.Stream = "OK."
						},
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Stream-Control-1")
        },
        /* Fifth control rdr*/
        /* This is an RDR representing a text control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_LCD_DISPLAY,
                                .Type = SAHPI_CTRL_TYPE_TEXT,
                                .TypeUnion = {
                                        .Text = {
						.Language = SAHPI_LANG_ENGLISH,
						.DataType = SAHPI_TL_TYPE_TEXT,
                                                .Default = {
							.Line = 0,
							.Text = def_text_buffer("Text-Control-1")
						},
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Text-Control-1")
        },
        /* Sixth control rdr */
        /* This is an RDR representing a oem control */
        /* on the first system board */
        {
                .RdrType  = SAHPI_CTRL_RDR,
                .Entity   = {
                        .Entry = {
                                {SAHPI_ENT_SYSTEM_BOARD, 1},
                                {SAHPI_ENT_ROOT, 0}
                        },
                },
                .RdrTypeUnion = {
                        .CtrlRec = {
                                .Num = 1,
                                .OutputType = SAHPI_CTRL_OEM,
                                .Type = SAHPI_CTRL_TYPE_OEM,
                                .TypeUnion = {
                                        .Oem = {
						.MId = 287,
						.ConfigData = "Oem-Cont",
                                                .Default = {
							.MId = 287,
							.BodyLength = 3,
							.Body = "Oem"
						},
                                        },
                                },
                        },
                },
                .IdString = def_text_buffer("Control Rdr: Oem-Control-1")
        },
};
