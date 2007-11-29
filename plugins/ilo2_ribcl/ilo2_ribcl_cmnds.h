/*
 * Copyright (C) 2007-2008, Hewlett-Packard Development Company, LLP
 *                     All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the Hewlett-Packard Corporation, nor the names
 * of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author(s)
 *     Shuah Khan <shuah.khan@hp.com>
 *     Richard White <richard.white@hp.com>
 */
#ifndef _INC_ILO2_RIBCL_CMNDS_H_
#define _INC_ILO2_RIBCL_CMNDS_H_

/***************
 * This header file contains all of the iLO2 RIBLC command denfinitions.
***************/
/* 
 * XML header definition. iLO2 requires that this  header be  sent at the
 * start of every RIBCL transation. The XML header ensures the connection
 * is an XML connection, not an HTTP connection.
 */
#define ILO2_RIBCL_XML_HDR      "<?xml version=\"1.0\"?>\r\n"

/* Here are all the RIBCL xml templates for the commands. For each command,
 * we have a format specification string that allows us to insert the
 * login and password strings for individual systems. Also, each command
 * has an assocoated macro value that gives its unique index for use in
 * command arrays. When you add a template, make sure to increment
 * IR_NUM_COMMANDS and add code to ir_xml_build_cmdbufs() in file
 * ilo2_ribcl_xml.c.
 */

#define IR_NUM_COMMANDS	5

/*
 * GET_SERVER_DATA command.
 * This is a combination of the RIBCL GET_HOST_DATA and GET_EMBEDDED_HEALTH
 * commands. These two commands are sent in one single transation to get
 * complete server information including fans, temp sensors, cpus, dimms,
 * power supplies, VRMs, and I/O slots. The following define combines and
 * embedds the GET_HOST_DATA and GET_EMBEDDED_HEALTH commands within a single
 * LOGIN block.
 */
#define IR_CMD_GET_SERVER_DATA 0
#define ILO2_RIBCL_GET_SERVER_DATA	"<LOCFG version=\"2.21\"/> <RIBCL version=\"2.22\"> <LOGIN USER_LOGIN=\"%s\" PASSWORD=\"%s\"> <SERVER_INFO MODE=\"READ\" > <GET_HOST_DATA /> <GET_EMBEDDED_HEALTH /> </SERVER_INFO> </LOGIN> </RIBCL>\r\n"

/*
 * GET_HOST_POWER_STATUS command
 */
#define IR_CMD_GET_HOST_POWER_STATUS 1
#define ILO2_RIBCL_GET_HOST_POWER_STATUS	"<LOCFG version=\"2.21\"/> <RIBCL VERSION=\"2.0\"> <LOGIN USER_LOGIN=\"%s\" PASSWORD=\"%s\"> <SERVER_INFO MODE=\"read\"> <GET_HOST_POWER_STATUS/> </SERVER_INFO> </LOGIN> </RIBCL>\r\n"

/*
 * SET_HOST_POWER commands
*/
#define IR_CMD_SET_HOST_POWER_ON 2
#define ILO2_RIBCL_SET_HOST_POWER_ON	"<LOCFG version=\"2.21\"/> <RIBCL VERSION=\"2.0\"> <LOGIN USER_LOGIN=\"%s\" PASSWORD=\"%s\"> <SERVER_INFO MODE=\"write\"> <SET_HOST_POWER HOST_POWER=\"Yes\"/> </SERVER_INFO> </LOGIN> </RIBCL>\r\n"

#define IR_CMD_SET_HOST_POWER_OFF 3
#define ILO2_RIBCL_SET_HOST_POWER_OFF	"<LOCFG version=\"2.21\"/> <RIBCL VERSION=\"2.0\"> <LOGIN USER_LOGIN=\"%s\" PASSWORD=\"%s\"> <SERVER_INFO MODE=\"write\"> <SET_HOST_POWER HOST_POWER=\"No\"/> </SERVER_INFO> </LOGIN> </RIBCL>\r\n"

/*
 * RESET_SERVER command
 */
#define IR_CMD_RESET_SERVER 4
#define ILO2_RIBCL_RESET_SERVER		"<LOCFG version=\"2.21\"/> <RIBCL VERSION=\"2.0\"> <LOGIN USER_LOGIN=\"%s\" PASSWORD=\"%s\"> <SERVER_INFO MODE=\"write\"> <RESET_SERVER/> </SERVER_INFO> </LOGIN> </RIBCL>\r\n"


#endif /*_INC_ILO2_RIBCL_CMNDS_H_*/
