/******************************************************************************/
//$COMMON.CPP$
//   File Name:  TriggerMgr.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the Trigger Manager definition.  The TriggerMgr
//      coordinates all of the Triggers in the BD subsystem.  Triggers are
//      enabled, disabled and checked via the TriggerMgr.
//
//   Interfaces:
//      The TriggerMgr interfaces with all of the Trigger objects to enable,
//      disable, check and process the triggers.  The TriggerMgr also interfaces
//      with the ModeMgr to determine the currently active mode.  Processing
//      for most triggers is mode dependent.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ModeMgr.h>
#include "BpTimeTrigger.h"
#include "CyclicTimeTrigger.h"
#include "TriggerMgr.h"
#include "ImmediateTrigger.h"
#include "PtInhPressTrigger.h"
#include "PtInhFlowTrigger.h"
#include "PtExhPressTrigger.h"
#include "PtExhFlowTrigger.h"
#include "ApneaAlarmTimeTrigger.h"
#include "ApneaAutoResetTrigger.h"
#include "ItLongAlarmTimeTrigger.h"
#include "HipAlarmTrigger.h"
#include "Hip2AlarmTrigger.h"
#include "AlarmResetOpReqTrigger.h"
#include "ManualInhOpReqTrigger.h"
#include "ModeChangeTrigger.h"
#include "LowBaselineAlarmTrigger.h"
#include "APRV_PtInhPressTrigger_InPLow.h"
#include "APRV_PtInhPressTrigger_InPHigh.h"
#include "PtExhFlowSpontTrigger.h"
#include "PtExhFlowSuppTrigger.h"
#include "BpBackupTimeTrigger.h"
#include "NIVBpTimeTrigger.h"
#include "AutoSIOnTimeTrigger.h"
#include "AutoSIOffTimeTrigger.h"
#include "DisconnectionAlarmTrigger.h"
#include "ReconnectionTrigger.h"
#include "HighMAP_2AutoResetTrigger.h"
#include "occltrigger.h"
#include "HardwareErrorTrigger.h"
#include "HardwareErrorAutoresetTrigger.h"
#include "SI10sTimerTrigger.h"
#include "ExtremeHigh_PHigh_Trigger.h"
#include "ProxyDisconnectTrigger.h"
#include "ProxyConnectTrigger.h"
#include "HPHighAlarmTrigger.h"
#include "ExtremeHighPEEPTrigger.h"
#include "TotalGasLossTrigger.h"

#include "DebugTrace.h"

// define statics
TriggerMgr* TriggerMgr::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the TriggerMgr object, it invokes TriggerMgr::S_GetInstance(),
//      which returns a pointer to the TriggerMgr object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      TriggerMgr* - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
TriggerMgr* TriggerMgr::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new TriggerMgr ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableAllTriggers()
//
//    Processing: 
//      This operation is invoked on entry into a Non-Ventilation Mode to 
//      disable all of the triggers.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void TriggerMgr::DisableAllTriggers (void)
{
    // Disable all triggers.
    for (SHORT ii = 0; ii < eNumTriggers; ii++)
    {
        TriggerList[ii]->DisableTrigger ();
    }
}   // end DisableAllTriggers()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableHFOTrigger()
//
//    Processing:
//      Enable HFO Trigger
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::EnableHFOTrigger(void)
{
    EnableTrigger(eModeChangeTrigger);

    EnableTrigger(eTotalGasLossTrigger);

    //    EnableTrigger(eHighMAP_2AlarmTrigger);

    //    EnableTrigger(eExtremeLowMapAlarmTrigger);

    EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);

    EnableTrigger(eHardwareErrorAlarmTriger);

    EnableTrigger(eOcclusionTrigger);

    EnableTrigger(eAlarmResetOpReqTrigger);

    //    if(eSwitchTypeOn == MgrBdSetting::S_GetInstance()->GetCurrentValue(eAutoSI)
    //       &&(MgrBdSetting::S_GetInstance()->GetCurrentValue(eSITime)!=0))
    //    {
    //        EnableTrigger(eAutoSIOffTimeTrigger);
    //    }
    //    else
    //    {
    //        DisableTrigger(eAutoSIOnTimeTrigger);
    //        DisableTrigger(eAutoSIOffTimeTrigger);
    //    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableIMVTrigger()
//
//    Processing:
//      Enable IMV Trigger
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::EnableIMVTrigger(void)
{
    EnableTrigger(eModeChangeTrigger);
    EnableTrigger(eHip2AlarmTrigger);
    EnableTrigger(eHipAlarmTrigger);
    EnableTrigger(eTotalGasLossTrigger);
    EnableTrigger(eUpdateStatusTimeTrigger, INIT_UPDATE_STAT_TIME);
    EnableTrigger(eApneaAlarmTimeTrigger);
    EnableTrigger(eHardwareErrorAlarmTriger);
    EnableTrigger(eExtremeHighPEEPTrigger);
    EnableTrigger(eAlarmResetOpReqTrigger);

    //    ProxyStatus Status = proxySensor->getLastStatus();
    //    if((Status == eCalibrated)||(Status == eResetAndRecovered))
    //    {
    //        EnableTrigger(eProxyDisconnectTrigger);
    //    }
    //    else
    //    {
    //        EnableTrigger(eProxyConnectTrigger);
    //    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableExhaleTrigger()
//
//    Processing:
//      Enable Exhale Trigger
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::EnableExhaleTrigger(void)
{
    EnableTrigger(eExtremeHighPEEPTrigger);
    EnableTrigger(eApneaAlarmTimeTrigger);
    EnableTrigger(eManualInhOpReqTrigger);

    DisableTrigger(eHipAlarmTrigger);
    DisableTrigger(eHip2AlarmTrigger);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked from the BD Task Entry Point each time 
//      the BD Task awakens.  This operation goes through its list of triggers.
//      For each trigger that is enabled, this operation invokes the 
//      CheckCondition() operation of the trigger. 
//
//      If the trigger has fired, the Trigger::ProcessTrigger() operation is
//      called to process the trigger based upon the currently active mode.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void TriggerMgr::ProcessBdCycle (void)
{
    SHORT ii = 0;                       // loop control
    E_Bool triggerFired = eFalse;       // indicates if a trigger did fire
    Mode* modePtr;                      // pointer to the current mode
    DEBUG_MGR_TRIGGER("Process TriggerMgr \n");

    // Loop through the TriggerList
    while (ii < eNumTriggers)
    {
        DEBUG_MGR_TRIGGER("%d ", ii);
        // Check if the trigger is enabled
        if(eTrue == TriggerList[ii]->GetIsEnabled ())
        {
            DEBUG_MGR_TRIGGER("Trigger (%d) is ENABLE\n", ii);
            // Check if the trigger fired.
            triggerFired = TriggerList[ii]->CheckCondition ();

            // If the trigger has fired, update the last trigger ID in the
            // Mode object and process the trigger.
            if(eTrue == triggerFired)
            {
                //                Mode::S_SetLastTriggerId (E_TriggerId (ii));

                // Process the trigger based on the current mode.
                modePtr = (ModeMgr::S_GetInstance ())->GetCurrentModePtr (); //current mode is standardby
                ASSERTION(modePtr != NULL);
                DEBUG_MGR_TRIGGER("ProcessTrigger - %d \n", ii);
                TriggerList[ii]->ProcessTrigger (modePtr);

            }
        }
        ii++;
    }
    DEBUG_MGR_TRIGGER(" - \n");

}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessIMVBdCycle()
//
//    Processing:
//      This operation is invoked from the BD Task Entry Point each time
//      the BD Task awakens.  This operation goes through its list of triggers.
//      For each trigger that is enabled, this operation invokes the
//      CheckCondition() operation of the trigger.
//
//      If the trigger has fired, the Trigger::ProcessTrigger() operation is
//      called to process the trigger based upon the currently active mode.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::ProcessIMVBdCycle(void)
{
    E_Bool triggerFired = eFalse;       // indicates if a trigger did fire
    SHORT ii = eFirstIMVTrigger;
    Mode* modePtr;

    // Loop through the TriggerList
    while (ii <= eLastIMVTrigger)
    {
        if (eTrue == TriggerList[ii]->GetIsEnabled())
        {

            // Check if the trigger fired.
            triggerFired = TriggerList[ii]->CheckCondition();

            // If the trigger has fired, update the last trigger ID in the
            // Mode object and process the trigger.
            if (eTrue == triggerFired)
            {
                //                Mode::S_SetLastTriggerId(E_TriggerId (ii));

                // Process the trigger based on the current mode.
                modePtr = (ModeMgr::S_GetInstance())->GetCurrentModePtr();
                ASSERTION(modePtr != NULL);
                TriggerList[ii]->ProcessTrigger(modePtr);
            }
        }
        ii++;
    }

}   // end ProcessIMVBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHFOBdCycle()
//
//    Processing:
//      This operation is invoked from the BD Task Entry Point each time
//      the BD Task awakens.  This operation goes through its list of triggers.
//      For each trigger that is enabled, this operation invokes the
//      CheckCondition() operation of the trigger.
//
//      If the trigger has fired, the Trigger::ProcessTrigger() operation is
//      called to process the trigger based upon the currently active mode.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::ProcessHFOBdCycle(void)
{
#ifdef  HFO_SYSTEM
    E_Bool triggerFired = eFalse;       // indicates if a trigger did fire
    SHORT ii = eFirstHFOTrigger;
    Mode* modePtr;

    // Loop through the TriggerList
    while (ii <= eLastHFOTrigger)
    {
        if (eTrue == TriggerList[ii]->GetIsEnabled())
        {

            // Check if the trigger fired.
            triggerFired = TriggerList[ii]->CheckCondition();

            // If the trigger has fired, update the last trigger ID in the
            // Mode object and process the trigger.
            if (eTrue == triggerFired)
            {
                Mode::S_SetLastTriggerId(E_TriggerId (ii));

                // Process the trigger based on the current mode.
                modePtr = (ModeMgr::S_GetInstance())->GetCurrentModePtr();
                ASSERTION(modePtr != NULL);
                TriggerList[ii]->ProcessTrigger(modePtr);
            }
        }
        ii++;
    }
#endif
}   // end ProcessHFOBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TriggerMgr()
//
//    Processing: 
//      This operation is the constructor for the TriggerMgr object.  This
//      operation constructs all of the triggers and initializes its TriggerList
//      with pointers to each of the triggers.  
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      If any element of the TriggerList is not properly initialized, error
//      handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
TriggerMgr::TriggerMgr (void)
{
    SHORT ii;   // loop control

    // All triggers are disabled at initialization; also initialize all pointers
    // so that it can be checked that all pointers are properly initialized.
    for (ii = 0; ii < eNumTriggers; ii++)
    {
        TriggerList[ii] = NULL;
    }

    // Instantiate all of the triggers
#ifdef HFO_SYSTEM
    TriggerList[eHighMAP_2AlarmTrigger] = HFOAlarmTrigger::S_GetInstance();
    TriggerList[eExtremeLowMapAlarmTrigger] = ExtremeLowMapAlarmTrigger::S_GetInstance();
    TriggerList[eExtremeLowMapAutoResetTrigger] = ExtremeLowMapAutoResetTrigger::S_GetInstance();

#endif
    TriggerList[eHighMAP_2AutoResetTrigger] = HighMAP_2AutoResetTrigger::S_GetInstance();

    TriggerList[eAutoSIOnTimeTrigger] = AutoSIOnTimeTrigger::S_GetInstance();;

    TriggerList[eAutoSIOffTimeTrigger] = AutoSIOffTimeTrigger::S_GetInstance();

    TriggerList[eSI10sTimeTrigger] = new SI10sTimerTrigger();

    TriggerList[eResetSITimeTrigger] = new TimeTrigger(eResetSITimeTrigger,(CHAR*)"SIONESEC");

    TriggerList[eTotalGasLossTrigger] = TotalGasLossTrigger::S_GetInstance();

    TriggerList[eOcclusionTrigger] = OcclTrigger::S_GetInstance();

    TriggerList[eResetGasLossTrigger] = new Trigger(eResetGasLossTrigger);

    TriggerList[eHardwareErrorAlarmTriger] = HardwareErrorTrigger::S_GetInstance();

    TriggerList[eHardwareErrorAutoresetTrigger] = HardwareErrorAutoresetTrigger::S_GetInstance();

    TriggerList[eAlarmResetOpReqTrigger] = AlarmResetOpReqTrigger::S_GetInstance();

    TriggerList[eModeChangeTrigger] = new ModeChangeTrigger();
    EnableTrigger(eModeChangeTrigger);

    TriggerList[eUpdateStatusTimeTrigger] = new CyclicTimeTrigger(eUpdateStatusTimeTrigger,
                                                                  (CHAR*)"STATTMR", (5 * TICKS_PER_SEC));

    TriggerList[eAPRVUpdateBRTrigger] = new TimeTrigger(eAPRVUpdateBRTrigger,(CHAR*)"ONEMINUTE");

    TriggerList[eHip2AlarmTrigger] = Hip2AlarmTrigger::S_GetInstance();

    TriggerList[eHipAlarmTrigger] = HipAlarmTrigger::S_GetInstance();

    TriggerList[eExtremeHighPEEPTrigger] = ExtremeHighPEEPTrigger::S_GetInstance();

    TriggerList[eExtremeHigh_PHigh_Trigger] = ExtremeHigh_PHigh_Trigger::S_GetInstance();

    TriggerList[eApneaAlarmTimeTrigger] = ApneaAlarmTimeTrigger::S_GetInstance();

    TriggerList[eItLongAlarmTimeTrigger] = new TimeTrigger(eItLongAlarmTimeTrigger,(CHAR*)"ITLONG");

    TriggerList[eDisconnectionAlarmTrigger] = DisconnectionAlarmTrigger::S_GetInstance();

    TriggerList[eReconnectionTrigger] = ReconnectionTrigger::S_GetInstance();

    TriggerList[eApneaAutoResetTrigger] = ApneaAutoResetTrigger::S_GetInstance();

    TriggerList[eMandPressInhTimeTrigger] = new TimeTrigger(eMandPressInhTimeTrigger,(CHAR*)"PCVITMR");

    TriggerList[eManualInhOpReqTrigger] = ManualInhOpReqTrigger::S_GetInstance();

    TriggerList[ePtInhPressTrigger] = new PtInhPressTrigger();

    TriggerList[ePtInhFlowTrigger] = new PtInhFlowTrigger();

    TriggerList[eBpTimeTrigger] = new BpTimeTrigger(eBpTimeTrigger, (CHAR*)"BPTMR");

    TriggerList[eBpBackupTimeTrigger] = new BpBackupTimeTrigger(eBpBackupTimeTrigger,(CHAR*) "BPBKTMR");

    TriggerList[eNIVBpTimeTrigger] = new NIVBpTimeTrigger(eNIVBpTimeTrigger, (CHAR*)"NIVBPTMR");

    TriggerList[eAvBpTimeTrigger] = new BpTimeTrigger(eAvBpTimeTrigger,(CHAR*) "AVBPTMR");

    TriggerList[eImmediateTrigger] = new ImmediateTrigger();

    TriggerList[ePtExhFlowSpontTrigger] = new PtExhFlowSpontTrigger();

    TriggerList[eVcvInhTimeTrigger] = new TimeTrigger(eVcvInhTimeTrigger,(CHAR*)"VCVITMR");

    TriggerList[ePlateauTimeTrigger] = new TimeTrigger(ePlateauTimeTrigger,(CHAR*) "PLATTMR");

    TriggerList[eAPRVLowPressTimeTrigger] = new TimeTrigger(eAPRVLowPressTimeTrigger,(CHAR*)"APRVLOWTMR");

    TriggerList[eAPRVHighPressTimeTrigger] = new TimeTrigger(eAPRVHighPressTimeTrigger,(CHAR*)"APRVLOWTMR");

    TriggerList[eAPRV_PtInhPressTrigger_InPLow] = new APRV_PtInhPressTrigger_InPLow();

    TriggerList[eAPRV_PtInhPressTrigger_InPHigh] = new APRV_PtInhPressTrigger_InPHigh();

    TriggerList[eHPHighAlarmTrigger] = HPHighAlarmTrigger::S_GetInstance();

    TriggerList[eProxyDisconnectTrigger] = new ProxyDisconnectTrigger();

    TriggerList[eProxyConnectTrigger] = new ProxyConnectTrigger();

    TriggerList[ePtExhFlowSuppTrigger] = new PtExhFlowSuppTrigger();

    // Make sure that all trigger pointers were properly initialized
    DEBUG_MGR_TRIGGER("Init TRrigger List in TriggerMGR (%d) \n\t", eNumTriggers);
    for (ii = 0; ii < eNumTriggers; ii++)
    {
        ASSERTION(TriggerList[ii] != NULL);
        DEBUG_MGR_TRIGGER("%d ", ii);
    }
    DEBUG_MGR_TRIGGER("Finish - \n");

}   // end TriggerMgr()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableInhaleTrigger()
//
//    Processing:
//      Enable Inhale Trigger
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void TriggerMgr::EnableInhaleTrigger(void)
{
    EnableTrigger(eHipAlarmTrigger);
    EnableTrigger(eHip2AlarmTrigger);

    DisableTrigger(eExtremeHighPEEPTrigger);
    DisableTrigger(eApneaAlarmTimeTrigger);

    DisableTrigger(eManualInhOpReqTrigger);
}
