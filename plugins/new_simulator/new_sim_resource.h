/** 
 * @file    new_sim_resource.h
 *
 * The file includes a definition of a class for resource handling:\n
 * NewSimulatorResource
 * 
 * @author  Lars Wetzel <larswetzel@users.sourceforge.net>
 * @version 0.1
 * @date    2010
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * The new Simulator plugin is adapted from the ipmidirect plugin.
 * Thanks to 
 *     Thomas Kanngieser <thomas.kanngieser@fci.com>
 *     Pierre Sangouard  <psangouard@eso-tech.com>
 *    
 */
  
#ifndef __NEW_SIM_RESOURCE_H__
#define __NEW_SIM_RESOURCE_H__


#ifndef __NEW_SIM_CONTROL_H__
#include "new_sim_control.h"
#endif

#ifndef __ARRAY_H__
#include "array.h"
#endif

/**
 * @class NewSimulatorResource
 * 
 * Class for simualting resources
 * 
 **/
class NewSimulatorResource : cArray<NewSimulatorRdr> {

public:
   // find a specific rdr
   NewSimulatorRdr *FindRdr( SaHpiRdrTypeT type, unsigned int num );
   // NewSimulatorRdr *FindRdr( NewSimulatorDomain *domain, SaHpiRdrTypeT type, NewSimulatorRdr *rdr );

   bool AddRdr( NewSimulatorRdr *rdr );
   bool RemRdr( NewSimulatorRdr *rdr );
   /// Find a rdr object
   int FindRdr( NewSimulatorRdr *rdr ) { return Find( rdr ); }
   /// Return the number of rdr entries
   int NumRdr() { return Num(); }
   /// Get a rdr object by index
   NewSimulatorRdr *GetRdr( int idx ) { return operator[]( idx ); }
   
   bool CheckWatchdogTimer();

protected:
   /// flag if active or not
   bool                 m_active;
   /// pointer on the domain
   NewSimulatorDomain   *m_domain;
   /// entity path of the resource   
   NewSimulatorEntityPath    m_entity_path;
   /// is the resource a FRU or not
   bool                     m_is_fru;
   /// hotswap state of the FRU
   SaHpiHsStateT             m_hotswap_state;

   // state only to create state change Mx -> M0
   // where Mx is m_picmg_fru_state
   /// Flag if policy is canceled
   bool                     m_policy_canceled;
   /// Extraction timeout
   SaHpiTimeoutT             m_extract_timeout;
   /// oem value
   unsigned int            m_oem;

   /// mapping of sensor numbers - obsolet?
   int                      m_sensor_num[256];

   /// rpt entry values
   SaHpiRptEntryT           m_rpt_entry;
   /// resource tag
   NewSimulatorTextBuffer   m_resource_tag;
   /// highest control id
   unsigned int           m_current_control_id;
   /// power state
   SaHpiPowerStateT         m_power_state;
   /// indicator state
   SaHpiHsIndicatorStateT   m_hotswap_indicator;
   /// reset action
   SaHpiResetActionT        m_reset_state;
      
public:
   int                    CreateSensorNum( SaHpiSensorNumT num );
   /// get a unique control num for this resource
   unsigned int          GetControlNum() { return ++m_current_control_id; }
   
public:
   /// set/get PolicyCanceled
   bool                  &PolicyCanceled() { return m_policy_canceled; }
   /// set/get Extraction timeout value
   SaHpiTimeoutT          &ExtractTimeout() { return m_extract_timeout; }
   NewSimulatorDomain     *Domain() const;
   /// set/get oem value
   unsigned int         &Oem() { return m_oem; }
   /// set/get EntityPath
   NewSimulatorEntityPath &EntityPath() { return m_entity_path; }
   /// set/get FRU flag
   bool                  &IsFru() { return m_is_fru; }
   /// set/get resource tag
   NewSimulatorTextBuffer &ResourceTag() { return m_resource_tag; }
   /// set/get hotswap state
   SaHpiHsStateT          &HotSwapState() { return m_hotswap_state; }
   /// set/get power state
   SaHpiPowerStateT       &PowerState() { return m_power_state; }
   /// set/get hotswap indicator state
   SaHpiHsIndicatorStateT &HSIndicator() { return m_hotswap_indicator; }
   /// set/get reset state
   SaHpiResetActionT      &ResetState() { return m_reset_state; }
   /// set/get entry id
   SaHpiEntryIdT          &EntryId() { return m_rpt_entry.EntryId; }
   /// set/get resource capabilities
   SaHpiCapabilitiesT     &ResourceCapabilities() { return m_rpt_entry.ResourceCapabilities; }
   /// set/get hotswap capabilities
   SaHpiHsCapabilitiesT   &HotSwapCapabilities() { return m_rpt_entry.HotSwapCapabilities; }
   /// set/get resource id
   SaHpiResourceIdT       ResourceId() { return m_rpt_entry.ResourceId; }
   /// set/get resource severity
   SaHpiSeverityT         &ResourceSeverity() { return m_rpt_entry.ResourceSeverity; }
   /// set/get resource failed flag
   SaHpiBoolT             &ResourceFailed() { return m_rpt_entry.ResourceFailed; }

public:
   NewSimulatorResource( NewSimulatorDomain *domain );
   virtual ~NewSimulatorResource();

public:
   virtual bool Destroy();
   void          Dump( NewSimulatorLog &dump ) const;

   bool Activate();
   void Deactivate();
   /// Get HPI hotswap state
   SaHpiHsStateT GetHpiState() { return m_hotswap_state; }
   SaHpiHsStateT SetHpiState( SaHpiHsStateT new_hs_state );
   void SetResourceInfo( SaHpiResourceInfoT resinfo );
   // bool CreateResourceInfo( SaHpiResourceInfoT &resinfo );

private:
   /// flag if resource is populated
   bool m_populate;

public:
   // create and populate hpi resource
   virtual bool Create( SaHpiRptEntryT &entry );
   virtual bool Populate();
};


#endif
