/*
 * Force specific code
 *
 * Copyright (c) 2004 by FORCE Computers.
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

#include "ipmi_mc_vendor_force.h"
#include "ipmi_utils.h"
#include "ipmi_log.h"
#include "ipmi_mc.h"
#include <errno.h>


cIpmiMcVendorForceShMc::cIpmiMcVendorForceShMc()
  : cIpmiMcVendor( 0x000e48, 0x1011, "Force ShMc" )
{
}


cIpmiMcVendorForceShMc::~cIpmiMcVendorForceShMc()
{
}


bool
cIpmiMcVendorForceShMc::Init( cIpmiMc *mc, const cIpmiMsg &devid )
{
  stdlog << "Force ShMc found.\n";

  // because we are talking to a ShMC and not the ShM,
  // we need to do some setup:
  // 1.) set the MC in ShMc mode
  // 2.) clear repository SDR

  if ( mc->Addr().IsType( eIpmiAddrTypeSystemInterface ) )
     {
       // we want to go into BMC mode
       stdlog << "switch to ShMc mode.\n";

       cIpmiMsg msg( (tIpmiNetfn)0x30, (tIpmiCmd)0x03 );
       msg.m_data[0] = 0;
       msg.m_data_len = 1;

       cIpmiMsg rsp;

       int rv = mc->SendCommand( msg, rsp );

       if ( rv )
          {
            stdlog << "cannot send set BMC mode: " << rv << " !\n";
            return false;
          }

       if ( rsp.m_data_len <= 0 || rsp.m_data[0] != eIpmiCcOk )
          {
            stdlog << "cannot go into BMC mode: " << rsp.m_data[0] << " !\n";
            return false;
          }
     }

  // check if there is a repository SDR
  if ( devid.m_data[6] & 2 )
     {
       stdlog << "clear repository SDR.\n";

       // clear repository SDR

       // get a reservation
       cIpmiMsg msg( eIpmiNetfnStorage, eIpmiCmdReserveSdrRepository );
       msg.m_data_len = 0;
       cIpmiMsg rsp;
       
       int rv = mc->SendCommand( msg, rsp );

       if ( rv )
          {
            stdlog << "cannot send reserve reposotory SDR: " << rv << " !\n";
            return false;
          }

       if ( rsp.m_data_len != 3 || rsp.m_data[0] != eIpmiCcOk )
          {
            stdlog << "cannot reserve repository SDR: " << rsp.m_data[0] << " !\n";

            return false;
          }

       unsigned short reservation = IpmiGetUint16( rsp.m_data + 1 );

       // create repository SDR and wait until erase completed
       bool first = true;

       msg.m_netfn = eIpmiNetfnStorage;
       msg.m_cmd   = eIpmiCmdClearSdrRepository;
       IpmiSetUint16( msg.m_data, reservation );
       msg.m_data[2] = 'C';
       msg.m_data[3] = 'L';
       msg.m_data[4] = 'R';
       msg.m_data_len = 6;

       do
          {
            msg.m_data[5] = first ? 0xaa : 0; // initiate erase/ erase status
            first = false;

            rv = mc->SendCommand( msg, rsp );

            if ( rv )
               {
                 stdlog << "cannot send clear SDR reposotory: " << rv << " !\n";
                 return false;
               }

            if ( rsp.m_data_len != 2 || rsp.m_data[0] != eIpmiCcOk )
               {
                 stdlog << "cannot reserve repository SDR: " << rsp.m_data[0] << " !\n";

                 return false;
               }
          }
       // wait until erase is complete
       while( (rsp.m_data[1] & 0x7) != 0x1 );
     }

  // this is for debugging only
  if ( devid.m_data[6] & 4 )
     {
       stdlog << "clear SEL.\n";

       // get a reservation
       cIpmiMsg msg( eIpmiNetfnStorage, eIpmiCmdReserveSel );
       msg.m_data_len = 0;
       cIpmiMsg rsp;

       int rv = mc->SendCommand( msg, rsp );

       if ( rv )
          {
            stdlog << "cannot send reserve SEL: " << rv << " !\n";
            return false;
          }

       if ( rsp.m_data_len != 3 || rsp.m_data[0] != eIpmiCcOk )
          {
            stdlog << "cannot reserve SEL: " << rsp.m_data[0] << " !\n";
       
            return false;
          }

       unsigned short reservation = IpmiGetUint16( rsp.m_data + 1 );

       // clear SEL
       msg.m_netfn = eIpmiNetfnStorage;
       msg.m_cmd   = eIpmiCmdClearSel;
       IpmiSetUint16( msg.m_data, reservation );
       msg.m_data[2] = 'C';
       msg.m_data[3] = 'L';
       msg.m_data[4] = 'R';
       msg.m_data_len = 6;
       
       bool first = true;

       do
          {
            msg.m_data[5] = first ? 0xaa : 0; // initiate erase/ erase status
            first = false;

            rv = mc->SendCommand( msg, rsp );

            if ( rv )
               {
                 stdlog << "cannot send clear SDR reposotory: " << rv << " !\n";
                 return false;
               }

            if ( rsp.m_data_len != 2 || rsp.m_data[0] != eIpmiCcOk )
               {
                 stdlog << "cannot reserve repository SDR: " << rsp.m_data[0] << " !\n";

                 return false;
               }
          }
       // wait until erase is complete
       while( (rsp.m_data[1] & 0x7) != 0x1 );
     }

  return true;
}
