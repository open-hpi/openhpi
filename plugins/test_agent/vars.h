/*      -*- c++ -*-
 *
 * (C) Copyright Pigeon Point Systems. 2011
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTVARLITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Author(s):
 *        Anton Pak <anton.pak@pigeonpoint.com>
 */

#ifndef VARS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010
#define VARS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

#include <list>
#include <string>


namespace TA {


/**************************************************************
 * Data Types
 *************************************************************/
enum eDataType
{
    dtInvalid,
    dtSaHpiUint8T,
    dtSaHpiUint16T,
    dtSaHpiUint32T,
    dtSaHpiUint64T,
    dtSaHpiInt8T,
    dtSaHpiInt16T,
    dtSaHpiInt32T,
    dtSaHpiInt64T,
    dtSaHpiFloat64T,
    dtSaHpiBoolT,
    dtSaHpiManufacturerIdT,
    dtSaHpiDomainIdT,
    dtSaHpiResourceIdT,
    dtSaHpiEntryIdT,
    dtSaHpiTimeT,
    dtSaHpiTimeoutT,
    dtSaHpiLanguageT,
    dtSaHpiTextTypeT,
    dtSaHpiTextBufferT,
    dtSaHpiInstrumentIdT,
    dtSaHpiEntityPathT,
    dtSaHpiEventCategoryT,
    dtSaHpiEventStateT,
    dtSaHpiEventStateTThreshold,
    dtSaHpiSensorNumT,
    dtSaHpiSensorTypeT,
    dtSaHpiSensorReadingTypeT,
    dtSaHpiSensorRangeFlagsT,
    dtSaHpiSensorUnitsT,
    dtSaHpiSensorModUnitUseT,
    dtSaHpiSensorThdMaskT,
    dtSaHpiSensorEventCtrlT,
    dtSaHpiCtrlNumT,
    dtSaHpiCtrlTypeT,
    dtSaHpiCtrlStateDigitalT,
    dtSaHpiCtrlStateDiscreteT,
    dtSaHpiCtrlStateAnalogT,
    dtSaHpiTxtLineNumT,
    dtSaHpiCtrlModeT,
    dtSaHpiCtrlOutputTypeT,
    dtSaHpiIdrIdT,
    dtSaHpiIdrAreaTypeT,
    dtSaHpiIdrFieldTypeT,
    dtSaHpiWatchdogNumT,
    dtSaHpiWatchdogActionT,
    dtSaHpiWatchdogActionEventT,
    dtSaHpiWatchdogPretimerInterruptT,
    dtSaHpiWatchdogTimerUseT,
    dtSaHpiWatchdogExpFlagsT,
    dtSaHpiDimiNumT,
    dtSaHpiDimiTestServiceImpactT,
    dtSaHpiDimiTestRunStatusT,
    dtSaHpiDimiTestErrCodeT,
    dtSaHpiDimiTestParamTypeT,
    dtSaHpiDimiTestCapabilityT,
    dtSaHpiDimiTestNumT,
    dtSaHpiDimiTestPercentCompletedT,
    dtSaHpiDimiReadyT,
    dtSaHpiFumiNumT,
    dtSaHpiBankNumT,
    dtSaHpiFumiSpecInfoTypeT,
    dtSaHpiFumiSafDefinedSpecIdT,
    dtSaHpiFumiServiceImpactT,
    dtSaHpiFumiSourceStatusT,
    dtSaHpiFumiBankStateT,
    dtSaHpiFumiUpgradeStatusT,
    dtSaHpiFumiLogicalBankStateFlagsT,
    dtSaHpiFumiProtocolT,
    dtSaHpiFumiCapabilityT,
    dtSaHpiHsIndicatorStateT,
    dtSaHpiHsStateT,
    dtSaHpiHsCauseOfStateChangeT,
    dtSaHpiSeverityT,
    dtSaHpiResourceEventTypeT,
    dtSaHpiDomainEventTypeT,
    dtSaHpiSensorOptionalDataT,
    dtSaHpiSensorEnableOptDataT,
    dtSaHpiSwEventTypeT,
    dtSaHpiEventTypeT,
    dtSaHpiAnnunciatorNumT,
    dtSaHpiNameT,
    dtSaHpiStatusCondTypeT,
    dtSaHpiAnnunciatorModeT,
    dtSaHpiAnnunciatorTypeT,
    dtSaHpiRdrTypeT,
    dtSaHpiParmActionT,
    dtSaHpiResetActionT,
    dtSaHpiPowerStateT,
    dtSaHpiLoadNumberT,
    dtSaHpiGuidT,
    dtSaHpiCapabilitiesT,
    dtSaHpiHsCapabilitiesT,
    dtSaHpiEventLogOverflowActionT,
    dtSaHpiEventLogCapabilitiesT,
    dtSaHpiEventLogEntryIdT,

    // Non-trivial cases
    dtSaHpiCtrlStateStreamTWithoutRepeat,
    dtSaHpiCtrlStateOemTWithoutMId,
    dtControlOemConfigData,
    dtSensorReadingBuffer,
    dtDimiTestParamName,
    dtSaHpiFumiOemDefinedSpecInfoTWithoutMid,
};


/**************************************************************
 * Var
 *************************************************************/
struct Var
{
    explicit Var()
        : type( dtInvalid ), rdata( 0 ), wdata( 0 )
    {
        // empty
    }

    explicit Var( eDataType          _type,
                  const std::string& _name,
                  const void *       _rdata,
                  void *             _wdata )
        : type( _type ), name( _name ), rdata( _rdata ), wdata( _wdata )
    {
        // empty
    }


    // data
    eDataType    type;
    std::string  name;
    const void * rdata;
    void *       wdata;
};


/**************************************************************
 * cVars components
 *************************************************************/
struct IF
{
    explicit IF( bool _cond )
        : cond( _cond )
    {
        // empty
    }

    bool cond;
};

struct READONLY
{
};

struct READONLY_IF
{
    explicit READONLY_IF( bool _cond )
        : cond( _cond )
    {
        // empty
    }

    bool cond;
};

struct DATA
{
    template<class X>
    explicit DATA( X& rwx )
        : rdata( &rwx ), wdata( &rwx )
    {
        // empty
    }

    template<class X>
    explicit DATA( const X& rx, X& wx )
        : rdata( &rx ), wdata( &wx )
    {
        // empty
    }

    const void * rdata;
    void       * wdata;
};

struct VAR_END
{
};

typedef std::list<Var> VarList;
typedef VarList::const_iterator VarIter;


/**************************************************************
 * cVars
 *************************************************************/
class cVars : private VarList
{
public:

    explicit cVars();
    ~cVars();

    VarIter begin() const
    {
        return VarList::begin();
    }

    VarIter end() const
    {
        return VarList::end();
    }

    cVars& operator <<( const IF& i_f );
    cVars& operator <<( const std::string& name );
    cVars& operator <<( eDataType type );
    cVars& operator <<( const READONLY& );
    cVars& operator <<( const READONLY_IF& );
    cVars& operator <<( const DATA& d );
    cVars& operator <<( const VAR_END& );

private:

    cVars( const cVars& );
    cVars& operator =( const cVars& );

private: // data

    bool m_cond;
    bool m_ro_cond;
    Var  m_pending;
};

}; // namespace TA


#endif // VARS_H_FB2B5DD5_4E7D_49F5_9397_C2FEC21B4010

