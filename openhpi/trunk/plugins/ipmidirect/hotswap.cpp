/*
 *
 * Copyright (c) 2003,2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ipmi_utils.h"
#include "ipmi.h"
#include "ipmi_sensor_hotswap.h"


static const char *hotswap_states[] =
{
  "inactive",
  "insertion_pending",
  "active_healthy",
  "active_unhealthy",
  "extraction_pending",
  "not_present"
};

static int hotswap_states_num = sizeof( hotswap_states ) / sizeof( char * );


const char *
HotswapStateToString( SaHpiHsStateT state )
{
  if ( state >= hotswap_states_num )
       return "invalid";

  return hotswap_states[state];
}


SaErrorT
cIpmi::IfGetHotswapState( cIpmiResource *res, SaHpiHsStateT &state )
{
  // get hotswap sensor
  cIpmiSensorHotswap *hs = res->GetHotswapSensor();

  if ( !hs )
       return SA_ERR_HPI_INVALID_PARAMS;

  // get hotswap state
  return hs->GetState( state );
}


// state == SAHPI_HS_STATE_ACTIVE_HEALTHY
//    => M2 -> M3
//    => M5 -> M4
// state == SAHPI_HS_STATE_INACTIVE
//    => M5 -> M6

SaErrorT
cIpmi::IfSetHotswapState( cIpmiResource *res, SaHpiHsStateT state )
{
  if ( !m_is_atca )
     {
       stdlog << "ATCA not supported by SI !\n";
       return SA_ERR_HPI_INVALID;
     }

  // get hotswap sensor
  cIpmiSensorHotswap *hs = res->GetHotswapSensor();

  if ( !hs )
       return SA_ERR_HPI_INVALID_PARAMS;

  // read current state
  SaHpiHsStateT current;
  SaErrorT rv = hs->GetState( current );

  if ( rv != SA_OK )
       return rv;

  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdSetFruActivation );
  msg.m_data_len = 3;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = res->FruId();

  switch( state )
     {
       case SAHPI_HS_STATE_ACTIVE_HEALTHY:
            if (    current == SAHPI_HS_STATE_INSERTION_PENDING
                 || current == SAHPI_HS_STATE_EXTRACTION_PENDING )
                 msg.m_data[2] = dIpmiActivateFru;
            else
               {
                 stdlog << "FRU state is not SAHPI_HS_STATE_INSERTION_PENDING or SAHPI_HS_STATE_EXTRACTION_PENDING: "
                        << HotswapStateToString( current ) << " !\n";
                 return SA_ERR_HPI_INVALID;
               }

            break;

       case SAHPI_HS_STATE_INACTIVE:
            if (    current == SAHPI_HS_STATE_ACTIVE_HEALTHY
                 || current == SAHPI_HS_STATE_EXTRACTION_PENDING )
                 msg.m_data[2] = dIpmiDeactivateFru;
            else
              {
                 stdlog << "FRU state is not SAHPI_HS_STATE_INSERTION_PENDING or SAHPI_HS_STATE_EXTRACTION_PENDING: "
                        << HotswapStateToString( current ) << " !\n";
                 return SA_ERR_HPI_INVALID;
               }

            break;

       default:
            stdlog << "IfSetHotSwapState: illegal state " << HotswapStateToString( state ) << " !\n";
            return SA_ERR_HPI_INVALID;
     }

  cIpmiMsg rsp;

  int r = res->SendCommand( msg, rsp );

  if ( r )
     {
       stdlog << "IfSetHotSwapState: could not send set FRU activation: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "IfSetHotSwapState: IPMI error set FRU activation: "
	      << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  return SA_OK;
}


// act == SAHPI_HS_ACTION_INSERTION     => M1->M2
// act == SAHPI_HS_STATE_ACTIVE_HEALTHY => M4->M5

SaErrorT
cIpmi::IfRequestHotswapAction( cIpmiResource *res,
                               SaHpiHsActionT act )
{
  if ( !m_is_atca )
     {
       stdlog << "ATCA not supported by SI !\n";
       return SA_ERR_HPI_INVALID;
     }

  // get hotswap sensor
  cIpmiSensorHotswap *hs = res->GetHotswapSensor();

  if ( !hs )
       return SA_ERR_HPI_INVALID_PARAMS;

  SaHpiHsStateT current;
  SaErrorT rv = hs->GetState( current );

  if ( rv != SA_OK )
       return rv;

  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdSetFruActivationPolicy );
  msg.m_data_len = 4;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = res->FruId();

  if ( act == SAHPI_HS_ACTION_INSERTION )
     {
       if ( current != SAHPI_HS_STATE_INACTIVE )
          {
            stdlog << "FRU not SAHPI_HS_STATE_INACTIVE: "
		   << HotswapStateToString( current ) << " !\n";
            return SA_ERR_HPI_INVALID;
          }

       // m1 -> m2
       msg.m_data[2] = 1; // M1->M2 lock bit
       msg.m_data[3] = 0; // clear locked bit M1->M2
     }
  else
     {
       if ( current != SAHPI_HS_STATE_ACTIVE_HEALTHY )
          {
            stdlog << "FRU not SAHPI_HS_STATE_ACTIVE_HEALTHY: "
		   << current << " !\n";
            return SA_ERR_HPI_INVALID;
          }

       msg.m_data[2]  = 2; // M4->M5 lock bit
       msg.m_data[3]  = 0; // clear lock bit M4->M5
     }

  cIpmiMsg  rsp;

  int r = res->SendCommand( msg, rsp );

  if ( r )
     {
       stdlog << "IfRequestHotswapAction: could not send set FRU activation policy: "
	      << r << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len != 2 
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "IfRequestHotswapAction: set FRU activation: " << rsp.m_data[0] << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetPowerState( cIpmiResource *res, SaHpiHsPowerStateT &state )
{
  // get power level
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetPowerLevel );
  cIpmiMsg rsp;

  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = res->FruId();
  msg.m_data[2] = 0x01; // desired steady power
  msg.m_data_len = 3;

  int rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "cannot send get power level: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 3
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[0] != dIpmiPigMgId )
     {
       stdlog << "cannot get power level: " << rsp.m_data[0] << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  unsigned char power_level = rsp.m_data[2] & 0x1f;

  // get current power level
  msg.m_data[2]  = 0; // steady state power

  rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "SetHotSwapState: could not send get power level: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 6
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "SetHotSwapState: IPMI error get power level: " << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_INVALID_CMD;
     }

  unsigned char current_power_level = rsp.m_data[2] & 0x1f;
  
  if ( current_power_level >= power_level )
       state = SAHPI_HS_POWER_ON;
  else
       state = SAHPI_HS_POWER_OFF;

  return SA_OK;
}


SaErrorT
cIpmi::IfSetPowerState( cIpmiResource *res, SaHpiHsPowerStateT state )
{
  int rv;
  unsigned int power_level = 0;

  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetPowerLevel );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = res->FruId();
  cIpmiMsg rsp;

  if ( state == SAHPI_HS_POWER_CYCLE )
     {
       // power off
       msg.m_cmd = eIpmiCmdSetPowerLevel;

       msg.m_data[2] = power_level;
       msg.m_data[3] = 0x01; // copy desierd level to present level
       msg.m_data_len = 4;

       rv = res->SendCommand( msg, rsp );

       if ( rv )
          {
            stdlog << "cannot send set power level: " << rv << " !\n";
            return SA_ERR_HPI_INVALID_CMD;
          }

       if (    rsp.m_data_len < 2
               || rsp.m_data[0] != eIpmiCcOk 
               || rsp.m_data[1] != dIpmiPigMgId )
          {
            stdlog << "cannot set power level: " <<  rsp.m_data[0] << " !\n";
            return SA_ERR_HPI_INVALID_CMD;
          }

       // power on
       state = SAHPI_HS_POWER_ON;
     }

  if ( state == SAHPI_HS_POWER_ON )
     {
       // get power level
       msg.m_cmd = eIpmiCmdGetPowerLevel;

       msg.m_data[2] = 0x01; // desired steady power
       msg.m_data_len = 3;

       rv = res->SendCommand( msg, rsp );

       if ( rv )
          {
            stdlog << "cannot send get power level: " << rv << " !\n";
            return SA_ERR_HPI_INVALID_CMD;
          }

       if (    rsp.m_data_len < 3
            || rsp.m_data[0] != eIpmiCcOk 
            || rsp.m_data[1] != dIpmiPigMgId )
          {
            stdlog << "cannot get power level: " << rsp.m_data[0] << " !\n";
            return SA_ERR_HPI_INVALID_CMD;
          }

       power_level = rsp.m_data[2] & 0x1f;
     }
  else
       assert( state == SAHPI_HS_POWER_OFF );

  // set power level
  msg.m_cmd = eIpmiCmdSetPowerLevel;

  msg.m_data[2] = power_level;
  msg.m_data[3] = 0x01; // copy desierd level to present level
  msg.m_data_len = 4;

  rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "cannot send set power level: " << rv << "! \n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "cannot set power level: " << rsp.m_data[0] << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetIndicatorState( cIpmiResource *res, SaHpiHsIndicatorStateT &state )
{
  cIpmiMsg  msg;
  cIpmiMsg  rsp;

  msg.m_netfn    = eIpmiNetfnPicmg;
  msg.m_cmd      = eIpmiCmdGetFruLedState;
  msg.m_data_len = 3;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = res->FruId();
  msg.m_data[2]  = 0; // blue led;

  int rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "IfGetIndicatorState: could not send get FRU LED state: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 6
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "IfGetIndicatorState: IPMI error set FRU LED state: "
	      <<  rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  // lamp test
  if ( rsp.m_data[2] & 4 )
     {
       if ( rsp.m_data_len < 10 )
          {
            stdlog << "IfGetIndicatorState: IPMI error (lamp test) message to short: "
		   << rsp.m_data_len << " !\n";

            return SA_ERR_HPI_DATA_LEN_INVALID;
          }

       state = SAHPI_HS_INDICATOR_ON;
       return SA_OK;
     }
  
  // overwrite state
  if ( rsp.m_data[2] & 2 )
     {
       if ( rsp.m_data_len < 9 )
          {
            stdlog << "IfGetIndicatorState: IPMI error (overwrite) message to short: " 
		   << rsp.m_data_len << " !\n";

            return SA_ERR_HPI_DATA_LEN_INVALID;
          }

       if ( rsp.m_data[6] == 0 )
            state = SAHPI_HS_INDICATOR_OFF;
       else
            state = SAHPI_HS_INDICATOR_ON;

       return SA_OK;
     }

  // local control state
  if ( rsp.m_data[3] == 0 )
       state = SAHPI_HS_INDICATOR_OFF;
  else
       state = SAHPI_HS_INDICATOR_ON;

  return SA_OK;
}


SaErrorT
cIpmi::IfSetIndicatorState( cIpmiResource *res, SaHpiHsIndicatorStateT state )
{
  cIpmiMsg  msg( eIpmiNetfnPicmg, eIpmiCmdSetFruLedState );
  msg.m_data_len = 6;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = res->FruId();
  msg.m_data[2]  = 0; // blue led;

  msg.m_data[3]  = (state == SAHPI_HS_INDICATOR_ON) ? 0xff : 0;
  msg.m_data[4]  = 0;
  msg.m_data[5]  = 1; // blue

  cIpmiMsg  rsp;
  int rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "IfGetIndicatorState: could not send get FRU LED state: "
	      << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "IfGetIndicatorState: IPMI error set FRU LED state: " 
	      << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetResetState( cIpmiResource * /*res*/, SaHpiResetActionT &state )
{
  state = SAHPI_RESET_DEASSERT;

  return SA_OK;
}


SaErrorT
cIpmi::IfSetResetState( cIpmiResource *res, SaHpiResetActionT state )
{
  unsigned char reset_state;

  switch( state )
     {
       case SAHPI_COLD_RESET:
            reset_state = 0x00;
            break;

       case SAHPI_WARM_RESET:
            reset_state = 0x01;
            break;

       default:
            stdlog << "IfSetResetState: unsupported state " << state << " !\n";
            return SA_ERR_HPI_INVALID_CMD;
     }

  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdFruControl );
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = res->FruId();
  msg.m_data[2]  = state;
  msg.m_data_len = 3;

  cIpmiMsg rsp;
  int rv = res->SendCommand( msg, rsp );

  if ( rv )
     {
       stdlog << "SetHotSwapState: could not send FRU control: " << rv << " !\n";
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "SetHotSwapState: IPMI error set FRU activation: "
	      << rsp.m_data[0] << " !\n";

       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}
