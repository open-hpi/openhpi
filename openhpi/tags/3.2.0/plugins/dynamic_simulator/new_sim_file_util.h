/** 
 * @file    new_sim_file_util.h
 *
 * The file includes some helper classes for parsing the simulation file:\n
 * SimulatorTokens\n
 * NewSimulatorFileUtil
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.2
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *     
 */
 
#ifndef __NEW_SIM_FILE_UTIL_H__
#define __NEW_SIM_FILE_UTIL_H__

#include <glib.h>

#ifndef __NEW_SIM_TEXT_BUFFER_H__
#include "new_sim_text_buffer.h"
#endif

#ifndef __NEW_SIM_ENTITY_H__
#include "new_sim_entity.h"
#endif

extern "C" {
#include "SaHpi.h"
}

/**
 * @enum SimTokenType
 * Enhancemeent of standard tokens from GScanner to have it a little bit easier
 * when scanning the simulation file.
 **/
enum SimTokenType {
        CONFIG_TOKEN_HANDLER = G_TOKEN_LAST,
        RPT_TOKEN_HANDLER,
        RDR_TOKEN_HANDLER,
        RDR_DETAIL_TOKEN_HANDLER,
        SENSOR_TOKEN_HANDLER,
        CONTROL_TOKEN_HANDLER,
        INVENTORY_TOKEN_HANDLER,
        WATCHDOG_TOKEN_HANDLER,
        ANNUNCIATOR_TOKEN_HANDLER,
        DIMI_TOKEN_HANDLER,
        FUMI_TOKEN_HANDLER,
        CONTROL_GET_TOKEN_HANDLER,
        SENSOR_DATA_TOKEN_HANDLER,
        INVENTORY_DATA_TOKEN_HANDLER,
        INV_AREA_TOKEN_HANDLER,
        INV_FIELD_TOKEN_HANDLER,
        WDT_GET_TOKEN_HANDLER,
        ANNUNCIATOR_DATA_TOKEN_HANDLER,
        ANNOUNCEMENT_TOKEN_HANDLER,
        DIMI_DATA_TOKEN_HANDLER,
        DIMI_TESTCASE_TOKEN_HANDLER,
        DIMI_TEST_DATA_TOKEN_HANDLER,
        FUMI_DATA_TOKEN_HANDLER,
        FUMI_SOURCE_DATA_TOKEN_HANDLER,
        FUMI_TARGET_DATA_TOKEN_HANDLER,
        FUMI_LOG_TARGET_DATA_TOKEN_HANDLER
};

/** 
 * @class SimulatorToken 
 * 
 * Holds one token which is needed for parsing
 * 
 * In order to use the glib lexical parser we need to define token
 * types which we want to switch on.
 **/
class SimulatorToken {
	private:
	gchar              *m_name;      //!< Name of one token
	guint              m_token;      //!< Value of the token
	
	public:

	SimulatorToken(const gchar *name, const guint token);
	~SimulatorToken();
	
	gchar *Name() { return m_name; } //!< Read/Write function to the private m_name value 
	guint &Token() { return m_token; } //!< Read/Write function fo the private m_token value
};



/**
 * @class NewSimulatorFileUtil
 * 
 * Provides some helper functions for parsing.
 **/
class NewSimulatorFileUtil {
   
   protected:
   GScanner               *m_scanner; //!< Holding the scanner pointer
   /// Root entity path comes from configuration file
   NewSimulatorEntityPath m_root_ep;
   
   public:
   NewSimulatorFileUtil( NewSimulatorEntityPath root );
   NewSimulatorFileUtil( GScanner *scanner );
   ~NewSimulatorFileUtil();

   bool process_textbuffer(NewSimulatorTextBuffer &buffer);
   bool process_textbuffer(SaHpiTextBufferT &text);
   bool process_entity( SaHpiEntityPathT &path );
   
   bool process_hexstring(guint max_len, gchar *str, SaHpiUint8T *hexlist);
};


#endif /*NEW_SIM_FILE_H_*/
