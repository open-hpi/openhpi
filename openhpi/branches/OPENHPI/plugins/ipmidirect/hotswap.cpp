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
HostswapStateToString( SaHpiHsStateT state )
{
  if ( state >= hotswap_states_num )
       return "invalid";

  return hotswap_states[state];
}


SaErrorT
cIpmi::IfGetHotswapState( cIpmiEntity *ent, SaHpiHsStateT &state )
{
  // get hotswap sensor
  cIpmiSensorHotswap *hs = ent->GetHotswapSensor();

  if ( !hs )
       return SA_ERR_HPI_INVALID_PARAMS;

  // get hotswap state
  return hs->GetState( state );
}


SaErrorT
cIpmi::IfSetHotswapState( cIpmiEntity *ent, SaHpiHsStateT state )
{
  if ( !m_is_atca )
     {
       IpmiLog( "ATCA not supported by SI !\n" );
       return SA_ERR_HPI_INVALID;
     }

  unsigned char fru_state;

  switch( state )
     {
       case SAHPI_HS_STATE_ACTIVE_HEALTHY:
            fru_state = dIpmiActivateFru;
            break;

       case SAHPI_HS_STATE_INACTIVE:
            fru_state = dIpmiDeactivateFru;
            break;

       default:
            IpmiLog( "IfSetHotSwapState: illegal state %d !\n", state );
            return SA_ERR_HPI_INVALID;
     }

  cIpmiMsg  cmd_msg;
  cIpmiMsg  rsp;
  cIpmiAddr addr;

  addr.m_type       = eIpmiAddrTypeIpmb;
  addr.m_channel    = ent->Channel();
  addr.m_slave_addr = ent->AccessAddress();
  addr.m_lun        = 0; //ent->lun;

  cmd_msg.m_netfn    = eIpmiNetfnPicmg;
  cmd_msg.m_cmd      = eIpmiCmdSetFruActivation;
  cmd_msg.m_data_len = 3;
  cmd_msg.m_data[0]  = dIpmiPigMgId;
  cmd_msg.m_data[1]  = ent->FruId();
  cmd_msg.m_data[2]  = fru_state;

  int rv = SendCommand( addr, cmd_msg, rsp );

  if ( rv )
     {
       IpmiLog( "IfSetHotSwapState: could not send set FRU activation: 0x%02x !\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "IfSetHotSwapState: IPMI error set FRU activation: %x !\n",
                rsp.m_data[0]);

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfRequestHotswapAction( cIpmiEntity *ent,
                               SaHpiHsActionT act )
{
  if ( !m_is_atca )
     {
       IpmiLog( "ATCA not supported by SI !\n" );
       return SA_ERR_HPI_INVALID;
     }

  // get hotswap sensor
  cIpmiSensorHotswap *hs = ent->GetHotswapSensor();

  if ( !hs )
       return SA_ERR_HPI_INVALID_PARAMS;

  SaHpiHsStateT current;
  SaErrorT rv = hs->GetState( current );
  
  if ( rv != SA_OK )
       return rv;

  cIpmiAddr addr;

  addr.m_type       = eIpmiAddrTypeIpmb;
  addr.m_channel    = ent->Channel();
  addr.m_slave_addr = ent->AccessAddress();
  addr.m_lun        = 0; //ent->lun;

  cIpmiMsg msg;
  msg.m_netfn = eIpmiNetfnPicmg;

  if ( act == SAHPI_HS_ACTION_INSERTION )
     {
       if ( current != SAHPI_HS_STATE_INACTIVE )
          {
            IpmiLog( "FRU not SAHPI_HS_STATE_INACTIVE: %d !\n",
                     current );
            return SA_ERR_HPI_INVALID;
          }

       msg.m_cmd      = eIpmiCmdSetFruActivationPolicy;
       msg.m_data_len = 4;
       msg.m_data[0]  = dIpmiPigMgId;
       msg.m_data[1]  = ent->FruId();
       msg.m_data[2]  = 1;
       msg.m_data[3]  = 0; // clear the activation/deactivation locked
     }
  else
     {
       if ( current != SAHPI_HS_STATE_ACTIVE_HEALTHY )
          {
            IpmiLog( "FRU not SAHPI_HS_STATE_ACTIVE_HEALTHY: %d !\n",
                     current );
            return SA_ERR_HPI_INVALID;
          }

       msg.m_cmd      = eIpmiCmdSetFruActivation;
       msg.m_data_len = 3;
       msg.m_data[0]  = dIpmiPigMgId;
       msg.m_data[1]  = ent->FruId();
       msg.m_data[2]  = 0; // deactivate
     }

  cIpmiMsg  rsp;

  int r = SendCommand( addr, msg, rsp );

  if ( r )
     {
       IpmiLog( "IfRequestHotswapAction: could not send set FRU activation policy: 0x%02x !\n", r );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len != 2 
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "IfRequestHotswapAction: set FRU activation: 0x%02x !\n", rsp.m_data[0] );
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetPowerState( cIpmiEntity *ent, SaHpiHsPowerStateT &state )
{
  cIpmiAddr addr( eIpmiAddrTypeIpmb, ent->Channel(), 
                  0, ent->AccessAddress() );

  // get power level
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetPowerLevel );
  cIpmiMsg rsp;

  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = ent->FruId();
  msg.m_data[2] = 0x01; // desired steady power
  msg.m_data_len = 3;

  int rv = SendCommand( addr, msg, rsp );

  if ( rv )
     {
       IpmiLog( "cannot send get power level: %d\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 3
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[0] != dIpmiPigMgId )
     {
       IpmiLog( "cannot get power level: 0x%02x !\n", rsp.m_data[0] );
       return SA_ERR_HPI_INVALID_CMD;
     }

  unsigned char power_level = rsp.m_data[2] & 0x1f;

  // get current power level
  msg.m_data[2]  = 0; // steady state power

  rv = SendCommand( addr, msg, rsp );

  if ( rv )
     {
       IpmiLog( "SetHotSwapState: could not send get power level: 0x%02x !\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 6
       || rsp.m_data[0] != eIpmiCcOk
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "SetHotSwapState: IPMI error get power level: 0x%02x !\n",
                rsp.m_data[0]);

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
cIpmi::IfSetPowerState( cIpmiEntity *ent, SaHpiHsPowerStateT state )
{
  int rv;
  unsigned int power_level = 0;

  cIpmiAddr addr( eIpmiAddrTypeIpmb, ent->Channel(), 
                  0, ent->AccessAddress() );
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetPowerLevel );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = ent->FruId();
  cIpmiMsg rsp;

  if ( state == SAHPI_HS_POWER_CYCLE )
     {
       // power off
       msg.m_cmd = eIpmiCmdSetPowerLevel;

       msg.m_data[2] = power_level;
       msg.m_data[3] = 0x01; // copy desierd level to present level
       msg.m_data_len = 4;

       rv = SendCommand( addr, msg, rsp );

       if ( rv )
          {
            IpmiLog( "cannot send set power level: %d\n", rv );
            return SA_ERR_HPI_INVALID_CMD;
          }

       if (    rsp.m_data_len < 2
               || rsp.m_data[0] != eIpmiCcOk 
               || rsp.m_data[1] != dIpmiPigMgId )
          {
            IpmiLog( "cannot set power level: 0x%02x !\n", rsp.m_data[0] );
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

       rv = SendCommand( addr, msg, rsp );

       if ( rv )
          {
            IpmiLog( "cannot send get power level: %d\n", rv );
            return SA_ERR_HPI_INVALID_CMD;
          }

       if (    rsp.m_data_len < 3
            || rsp.m_data[0] != eIpmiCcOk 
            || rsp.m_data[1] != dIpmiPigMgId )
          {
            IpmiLog( "cannot get power level: 0x%02x !\n", rsp.m_data[0] );
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

  rv = SendCommand( addr, msg, rsp );

  if ( rv )
     {
       IpmiLog( "cannot send set power level: %d\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != eIpmiCcOk 
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "cannot set power level: 0x%02x !\n", rsp.m_data[0] );
       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetIndicatorState( cIpmiEntity *ent, SaHpiHsIndicatorStateT &state )
{
  cIpmiMsg  msg;
  cIpmiMsg  rsp;
  cIpmiAddr addr;

  addr.m_type       = eIpmiAddrTypeIpmb;
  addr.m_channel    = ent->Channel();
  addr.m_slave_addr = ent->AccessAddress();
  addr.m_lun        = 0; //ent->lun;

  msg.m_netfn    = eIpmiNetfnPicmg;
  msg.m_cmd      = eIpmiCmdGetFruLedState;
  msg.m_data_len = 3;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = ent->FruId();
  msg.m_data[2]  = 0; // blue led;

  int rv = SendCommand( addr, msg, rsp );

  if ( rv )
     {
       IpmiLog( "IfGetIndicatorState: could not send get FRU LED state: 0x%02x !\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 6
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "IfGetIndicatorState: IPMI error set FRU LED state: %x !\n",
                rsp.m_data[0]);

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  // lamp test
  if ( rsp.m_data[2] & 4 )
     {     
       if ( rsp.m_data_len < 10 )
          {
            IpmiLog( "IfGetIndicatorState: IPMI error (lamp test) message to short: %d !\n",
                     rsp.m_data_len);

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
            IpmiLog( "IfGetIndicatorState: IPMI error (overwrite) message to short: %d !\n",
                     rsp.m_data_len );

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
cIpmi::IfSetIndicatorState( cIpmiEntity *ent, SaHpiHsIndicatorStateT state )
{
  cIpmiMsg  msg;
  cIpmiMsg  rsp;
  cIpmiAddr addr;

  addr.m_type       = eIpmiAddrTypeIpmb;
  addr.m_channel    = ent->Channel();
  addr.m_slave_addr = ent->AccessAddress();
  addr.m_lun        = 0; //ent->lun;

  msg.m_netfn    = eIpmiNetfnPicmg;
  msg.m_cmd      = eIpmiCmdSetFruLedState;
  msg.m_data_len = 6;
  msg.m_data[0]  = dIpmiPigMgId;
  msg.m_data[1]  = ent->FruId();
  msg.m_data[2]  = 0; // blue led;

  msg.m_data[3]  = (state == SAHPI_HS_INDICATOR_ON) ? 0xff : 0;
  msg.m_data[4]  = 0;
  msg.m_data[5]  = 1; // blue

  int rv = SendCommand( addr, msg, rsp );

  if ( rv )
     {
       IpmiLog( "IfGetIndicatorState: could not send get FRU LED state: 0x%02x !\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "IfGetIndicatorState: IPMI error set FRU LED state: %x !\n",
                rsp.m_data[0] );

       return SA_ERR_HPI_DATA_LEN_INVALID;
     }

  return SA_OK;
}


SaErrorT
cIpmi::IfGetResetState( cIpmiEntity * /*ent*/, SaHpiResetActionT &state )
{
  state = SAHPI_RESET_DEASSERT;

  return SA_OK;
}


SaErrorT
cIpmi::IfSetResetState( cIpmiEntity *ent, SaHpiResetActionT state )
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
            IpmiLog( "IfSetResetState: unsupported state 0x%02x !\n", state );
            return SA_ERR_HPI_INVALID_CMD;
     }

  cIpmiMsg  cmd_msg;
  cIpmiMsg  rsp;
  cIpmiAddr addr;

  addr.m_type       = eIpmiAddrTypeIpmb;
  addr.m_channel    = ent->Channel();
  addr.m_slave_addr = ent->AccessAddress();
  addr.m_lun        = ent->Lun();

  cmd_msg.m_netfn    = eIpmiNetfnPicmg;
  cmd_msg.m_cmd      = eIpmiCmdFruControl;
  cmd_msg.m_data[0]  = dIpmiPigMgId;
  cmd_msg.m_data[1]  = ent->FruId();
  cmd_msg.m_data[2]  = state;
  cmd_msg.m_data_len = 3;

  int rv = SendCommand( addr, cmd_msg, rsp );

  if ( rv )
     {
       IpmiLog( "SetHotSwapState: could not send FRU control: 0x%02x !\n", rv );
       return SA_ERR_HPI_INVALID_CMD;
     }

  if (    rsp.m_data_len < 2
       || rsp.m_data[0] != 0
       || rsp.m_data[1] != dIpmiPigMgId )
     {
       IpmiLog( "SetHotSwapState: IPMI error set FRU activation: %x !\n",
                rsp.m_data[0]);

       return SA_ERR_HPI_INVALID_CMD;
     }

  return SA_OK;
}
