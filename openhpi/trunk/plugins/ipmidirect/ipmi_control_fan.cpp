/*
 * ipmi_control_fan.cpp
 *
 * Copyright (c) 2004 by FORCE Computers.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 */


#include "ipmi_control_fan.h"
#include "ipmi_mc.h"
#include "ipmi_log.h"


cIpmiControlFan::cIpmiControlFan( cIpmiMc *mc,
                                  unsigned int num,
                                  unsigned int minium_speed_level,
                                  unsigned int maximum_speed_level,
                                  unsigned int default_speed_level,
                                  bool local_control_mode )
  : cIpmiControl( mc, num, SAHPI_CTRL_FAN_SPEED,
                  SAHPI_CTRL_TYPE_ANALOG ),
    m_minimum_speed_level( minium_speed_level ),
    m_maximum_speed_level( maximum_speed_level ),
    m_default_speed_level( default_speed_level ),
    m_local_control_mode( local_control_mode )
{
}


cIpmiControlFan::~cIpmiControlFan()
{
}


bool
cIpmiControlFan::CreateRdr( SaHpiRptEntryT &resource, SaHpiRdrT &rdr )
{
  if ( cIpmiControl::CreateRdr( resource, rdr ) == false )
       return false;

  SaHpiCtrlRecT &rec = rdr.RdrTypeUnion.CtrlRec;
  SaHpiCtrlRecAnalogT &ana_rec = rec.TypeUnion.Analog;

  ana_rec.Min     = (SaHpiCtrlStateAnalogT)m_minimum_speed_level;
  ana_rec.Max     = (SaHpiCtrlStateAnalogT)m_maximum_speed_level;
  ana_rec.Default = (SaHpiCtrlStateAnalogT)m_default_speed_level;

  return true;
}


SaErrorT
cIpmiControlFan::SetState( const SaHpiCtrlStateT &state )
{
  if ( state.Type != SAHPI_CTRL_TYPE_ANALOG )
       return SA_ERR_HPI_INVALID_DATA;

  if (    (unsigned int)state.StateUnion.Analog < m_minimum_speed_level
       || (unsigned int)state.StateUnion.Analog > m_maximum_speed_level )
       return SA_ERR_HPI_INVALID_DATA;

  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdSetFanLevel );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = 0;
  msg.m_data[2] = (unsigned char)state.StateUnion.Analog;
  msg.m_data_len = 3;

  cIpmiMsg rsp;

  int rv = m_mc->SendCommand( msg, rsp );

  if (    rv
       || rsp.m_data_len < 2
       || rsp.m_data[0] != eIpmiCcOk
          || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "cannot send set fan speed !\n";
       return SA_ERR_HPI_INVALID_REQUEST;
     }

  return SA_OK;
}


SaErrorT
cIpmiControlFan::GetState( SaHpiCtrlStateT &state )
{
  cIpmiMsg msg( eIpmiNetfnPicmg, eIpmiCmdGetFanLevel );
  msg.m_data[0] = dIpmiPigMgId;
  msg.m_data[1] = 0;
  msg.m_data_len = 2;

  cIpmiMsg rsp;

  int rv = m_mc->SendCommand( msg, rsp );

  if (    rv
       || rsp.m_data_len < 3
       || rsp.m_data[0] != eIpmiCcOk
          || rsp.m_data[1] != dIpmiPigMgId )
     {
       stdlog << "cannot send get fan speed !\n";
       return SA_ERR_HPI_INVALID_REQUEST;
     }

  state.Type = SAHPI_CTRL_TYPE_ANALOG;
  state.StateUnion.Analog = (SaHpiCtrlStateAnalogT)rsp.m_data[2];

  return SA_OK;
}


void
cIpmiControlFan::Dump( cIpmiLog &dump, const char *name ) const
{
  dump.Begin( "FanControl", name );

  dump.Entry( "ControlNum" ) << m_num << ";\n";
  dump.Entry( "Oem" ) << m_oem << ";\n";
  dump.Entry( "Ignore" ) << m_ignore << ";\n";
  dump.Entry( "MinimumSpeedLevel" ) << m_minimum_speed_level << ";\n";
  dump.Entry( "MaximumSpeedLevel" ) << m_maximum_speed_level << ";\n";
  dump.Entry( "DefaultSpeedLevel" ) << m_default_speed_level << ";\n";
  dump.Entry( "LocalControlMode" ) << m_local_control_mode << ";\n";

  dump.End();
}
