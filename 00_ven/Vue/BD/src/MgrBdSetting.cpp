/******************************************************************************/
//$COMMON.CPP$
//   File Name:  MgrBdSetting.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  
//      This file contains the definition of the MgrBdSetting class.
//      MgrBdSetting contains a setting list with pointers to all Settings
//      used by the Breath Delivery and Patient Data software.   This class 
//      contains methods to process settings changes requested by the user.  
//      It also contains a method to phase in the pending setting change values.
//
//   Interfaces:
//      The BD Task entry point invokes the ReceiveRequest() operation
//      whenever a settings change request is received on the BD queue.  
//      ReceiveRequest() then interfaces with all affected BdSetting
//      objects.
//
//      ReceiveRequest() also invokes operations in the PhaseMgr object
//      to notify that object of changes in ventilation type, support pressure
//      or trigger type.  These settings are used to determine which breath
//      phases are active.
//
//   Restrictions:
//      Only 1 instance of this class is allowed.
/******************************************************************************/
#include <PhaseMgr.h>
#include "MgrBdSetting.h"
#include "TriggerMgr.h"
#include "O2Setting.h"
#include "CommonTypes.h"
#include "VA_AC_Mode.h"
#include "VA_SIMV_Mode.h"
#include "ApneaVAMode.h"

#include "DebugTrace.h"

// Define statics
MgrBdSetting* MgrBdSetting::S_Instance = NULL;

// During Non-Invasive ventilation, HIP limit is not operator settable.  
// Instead it is fixed at 10 cmH2O above the operator set IPAP.
static const SHORT NPPV_HIP_OFFSET = 10 * SCALE;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the MgrBdSetting object, it calls MgrBdSetting::S_GetInstance(),
//      which returns a pointer to the MgrBdSetting object.
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
//      MgrBdSetting* - pointer to the object instantiated from this class
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
MgrBdSetting* MgrBdSetting::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new MgrBdSetting ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReceiveRequest()
//
//    Processing: 
//      This operation is invoked from BdTask::Entry() whenever a BD Settings
//      type of event is placed on the BD Queue.  Based on the event ID,
//      this operation determines whether a single setting or batch ventilation
//      type setting change is being made.  It then calls operations in 
//      BdSetting objects to phase in the changes.  
//
//      This operation also notifies the PhaseMgr object whenever a ventilation 
//      type batch change has been received via a call to ProcessNewVentType().
//      And, it notifies the PhaseMgr when support pressure or trigger type
//      changes via a call to ProcessNewSupportPress() or 
//      ProcessNewTriggerType().
//
//    Input Parameters:
//      bdEvent - constant reference to the settings change event received
//                  on the BD Queue
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
//      If the event ID in the BdEvent passed to this operation is not valid,
//      error handling is performed.
//
//    Requirements:  
//
/******************************************************************************/
void MgrBdSetting::ReceiveRequest (const BdEvent& bdEvent)
{
    E_BdEventId id = bdEvent.BasicEvent.Id;

    // Check if this is a single setting change event.
    if ((id >= eFirstSingleSettingChangeId) && (id <= eLastSingleSettingChangeId))
    {
        // If this is a single setting change event, invoke the
        // SetPendingValue() method for the setting.
        SettingList[id - eFirstSingleSettingChangeId]->
        SetPendingValue(bdEvent.SingleSettingChangeEvent.Value);
        HandlephaseInImmediatelyFlag(eFalse, SettingList[id - eFirstSingleSettingChangeId], NULL, bdEvent.SingleSettingChangeEvent.Value);

        switch(id)
        {
            case eTriggerTypeChangeId:
                if(((E_TriggerType) bdEvent.SingleSettingChangeEvent.Value)==ePressureTrigger)
                {
                    FlowTrigOn = eFalse;
                    PressTriggOn = (MgrBdSetting::S_GetInstance()->GetCurrentValue(ePressSens)>1000)? eFalse:eTrue;
                    if(PressTriggOn)
                        (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(ePressureTrigger);
                    else
                        (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eNoTriggerType);
                }
                else if(((E_TriggerType) bdEvent.SingleSettingChangeEvent.Value)==eFlowTrigger)
                {
                    FlowTrigOn = (MgrBdSetting::S_GetInstance()->GetCurrentValue(eFlowSens)>1000)? eFalse:eTrue;

                    if(FlowTrigOn==eFalse)
                        PressTriggOn = eFalse;
                    else
                        PressTriggOn = (MgrBdSetting::S_GetInstance()->GetCurrentValue(ePressSens)>1000)? eFalse:eTrue;

                    if(FlowTrigOn)
                        (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eFlowTrigger);
                    else
                        (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eNoTriggerType);
                }
                else
                {
                }

                break;
            case ePressSensChangeId:
                PressTriggOn = (bdEvent.SingleSettingChangeEvent.Value>1000)? eFalse:eTrue;
                if(PressTriggOn)
                    (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(ePressureTrigger);
                else
                    (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eNoTriggerType);
                break;
            case eFlowSensChangeId:
                FlowTrigOn = (bdEvent.SingleSettingChangeEvent.Value>1000)? eFalse:eTrue;

                if(FlowTrigOn==eFalse)
                    PressTriggOn = eFalse;
                else
                    PressTriggOn = (MgrBdSetting::S_GetInstance()->GetCurrentValue(ePressSens)>1000)? eFalse:eTrue;

                if(FlowTrigOn)
                    (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eFlowTrigger);
                else
                    (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eNoTriggerType);
                break;
            case eSupportPressChangeId:
                //            PSVOn = (bdEvent.SingleSettingChangeEvent.Value > PSV_OFF_VALUE)?eTrue:eFalse;
                break;
            case ePcvInhPressChangeId:
            case eAPRVHighInhPressChangeId:
            case eAPRVLowInhPressChangeId:
            case eBaselinePressChangeId:
                //            CMVServoController->SetIMVAlarmInactiveTimer(20);//2 s
                break;

#ifdef HFO_SYSTEM
            case eHFOMapChangeId:
            case eHFOStrokeVolumeChangeId:
            case eHFOFrequencyChangeId:
                ExtremeLowMapAlarmTrigger::S_GetInstance()->SetInactiveTimer(2500);
                break;
#endif
            default:
                break;
        }
    }
    else if (ePressureBatchChangeId == id)
    {

        // If this is a Pressure Batch Setting Event, update the common settings 
        // and then update the pressure specific settings by invoking local 
        // operations.
        HandlePressureSettings(bdEvent.BatchSettingsChangeEvent.VentTypeSettings.PressureSettings);
    }
    else if (eCommonBatchChangeId == id)
    {
        // If this is a HFO Batch Setting Event, update the common
        // settings and then update the non-invasive specific settings by 
        // invoking local operations.
        HandleCommonSettings(bdEvent);

    }
    else
    {
        // Unknown event ID.
        ASSERTION(eFalse);
    }

    //    Send(bdEvent);

}       // end ReceiveRequest()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AddSettingToChangeList()
//
//    Processing: 
//      This method is invoked by BdSetting::SetPendingValue() whenever a 
//      settings value is changing.
//
//    Input Parameters:
//      bdSettingPtr - Pointer to the BdSetting that has changed.
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
//
//    Requirements:  
//
/******************************************************************************/
void MgrBdSetting::AddSettingToChangeList (BdSetting* bdSettingPtr)
{
    // The change list only holds the total number of BdSettings.  As long as
    // the ChangeListIx is less than that number, add the BdSetting to the
    // ChangedList and increment the ChangeListIx.

    E_BdSettingId id = eMode ;  // edit

    if(NULL != bdSettingPtr)
    {
        id = bdSettingPtr->GetID();
    }
    if(id < eNumBdSettings)
    {
        ChangedList[id] = bdSettingPtr;
    }

}   // end AddSettingToChangeList()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessNewPhase()
//
//    Processing: 
//      This operation is invoked at the start of each inhalation and at
//      the start of each exhalation to phase in pending setting changes.
//      This method invokes the PhaseInSetting method of all BdSetting objects
//      that have changed since the start of the last breath phase.
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
void MgrBdSetting::ProcessNewPhase (void)
{
    SHORT ii;                   // loop control
    E_Bool done = eFalse;       // indicates when end of changed list is reached

    // If so many settings changed that the ChangedList is full, the 
    // ChangeListOverflow flag would have been set.  If the flag is set,
    // all settings are phased in because the list may be incomplete.    
    if(eTrue == ChangeListOverflow)
    {
        // Reset the ChangedList
        for (ii = 0; ii < eNumBdSettings; ii++)
        {
            ChangedList[ii] = NULL;
        }

        // Phase in all settings; even if a setting hadn't changed, invoking
        // PhaseInSetting just causes the old value to be phased in.
        for (ii = 0; ii < eNumBdSettings; ii++)
        {
            SettingList[ii]->PhaseInSetting ();
        }

        // Reset the overflow flag 
        ChangeListOverflow = eFalse;
    }

    else
    {
        // If the overflow flag was not set, just phase in settings that have
        // changed
        ii = 0;

        while ((ii < eNumBdSettings) && !done)
        {
            // The first NULL in the change list indicates the end of the
            // pending changes.
            if(NULL == ChangedList[ii])
            {
                done = eTrue;
            }
            else
            {
                // Phase in the setting change and reset the pointer in the
                // ChangedList
                ChangedList[ii]->PhaseInSetting ();
                ChangedList[ii] = NULL;
            }

            ii++;
        }
    }

    // Reset the ChangeListIx; always start at the top of the list. 
    ChangeListIx = 0;

    // At the start of each breath phase, update the status of the air and
    // oxygen alarms.
    (O2Setting::S_GetInstance ())->UpdateAirO2Alarms ();

}   // end ProcessNewPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:PhaseInAllSettings()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void MgrBdSetting::PhaseInAllSettings()
{

    SHORT ii;                   // loop control
    ii = 0;

    while (ii < eNumBdSettings)
    {
        // The first NULL in the change list indicates the end of the
        // pending changes.
        if (NULL != ChangedList[ii])
        {
            // Phase in the setting change and reset the pointer in the
            // ChangedList

            ChangedList[ii]->PhaseInSetting();

            ChangedList[ii] = NULL;
        }

        ii++;

    }

    // Reset the ChangeListIx; always start at the top of the list.
    ChangeListIx = 0;

    // At the start of each breath phase, update the status of the air and
    // oxygen alarms.
    (O2Setting::S_GetInstance())->UpdateAirO2Alarms();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:PhaseInNCPAPSettings()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void MgrBdSetting::PhaseInNCPAPSettings()
{
    SettingList[eNasalCPAPFlow]->PhaseInSetting();
    SettingList[ePctO2]->PhaseInSetting();
    SettingList[eHighNCPAPLimit]->PhaseInSetting();
    SettingList[eLowNCPAPLimit]->PhaseInSetting();
    SettingList[eFlushPctO2]->PhaseInSetting();
    SettingList[eExtremeHighPEEP_Autoset]->PhaseInSetting();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:PhaseInPIPSetting()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void MgrBdSetting::PhaseInPIPSetting()
{
    if(SettingList[ePcvInhPress]->GetCurrentValue() != SettingList[ePcvInhPress]->GetPendingValue())
    {
        VA_AC_Mode::S_GetInstance()->StartTestBreath();
        VA_SIMV_Mode::S_GetInstance()->StartTestBreath();
        ApneaVAMode::S_GetInstance()->StartTestBreath();
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:PhaseInPIPSetting()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
void MgrBdSetting::DoNotStartAutoSetAmp(void)
{
    startAutoSetAmp = eFalse;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInHFOSettings()
//
//    Processing:Phase in settings for all HFO specific settings
//    Input Parameters:
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
#ifdef HFO_SYSTEM
void MgrBdSetting::PhaseInHFOSettings()
{
    if(SettingList[eHFOMap]->GetCurrentValue() != SettingList[eHFOMap]->GetPendingValue())
    {
        HFOAlarmTrigger::S_GetInstance()->SetDelayMapAlarmCheck();
    }
    //Phase in settings for all HFO specific settings
    SettingList[eHFOMap]->PhaseInSetting();
    SettingList[eHFOFrequency]->PhaseInSetting();
    SettingList[eHFOStrokeVolume]->PhaseInSetting();
    SettingList[eSI]->PhaseInSetting();
    SettingList[eFreshGas]->PhaseInSetting();
    SettingList[ePctO2]->PhaseInSetting();
    SettingList[eFlushPctO2]->PhaseInSetting();

    SettingList[eHiMAP_1Limit]->PhaseInSetting();
    SettingList[eHiMAP_2Limit]->PhaseInSetting();
    SettingList[eLowMAPLimit]->PhaseInSetting();
    SettingList[eExtremeHighMap_Autoset]->PhaseInSetting();
    SettingList[eHighAmplitudeLimit]->PhaseInSetting();
    SettingList[eLowAmplitudeLimit]->PhaseInSetting();
    SettingList[eSITime]->PhaseInSetting();
    SettingList[eAutoSI]->PhaseInSetting();
    SettingList[eSIRate]->PhaseInSetting();
    SettingList[eAmplitudeControl]->PhaseInSetting();
    SettingList[eAmplitude]->PhaseInSetting();

}
#endif

//******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessHFOSettings()
//
//    Processing:
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************/
#ifdef HFO_SYSTEM
void MgrBdSetting::ProcessHFOSettings()
{

}
#endif

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrBdSetting()
//
//    Processing: 
//      This is the constructor for the MgrBdSetting class.  This method
//      initializes the attributes of the class and instantiates all BdSetting
//      objects.
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
MgrBdSetting::MgrBdSetting (void)
{
    startAutoSetAmp = eTrue;
    PressTriggOn = eFalse;
    FlowTrigOn = eFalse;
    //    PSVOn = eFalse; //not used in old source
    SHORT ii;       // loop control

    // Set the index to 0.  This index is used to maintain a list of settings
    // that have changed.
    ChangeListIx = 0;

    // No overflow has occurred yet.
    ChangeListOverflow = eFalse;

    // Initialize the ChangedList to NULL.  No settings changes are pending
    // at this point.
    for (ii = 0; ii < eNumBdSettings; ii++)
    {
        ChangedList[ii] = NULL;
    }

    // Get the pointer to the TriggerMgr object
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Instantiate all BdSetting objects and set up the SettingList[]
    SettingList[eMode] = new BdSetting(eMode,triggerMgrPtr->GetTriggerPtr(eModeChangeTrigger), eTrue);

    SettingList[eTriggerType] = new BdSetting(eTriggerType);

    SettingList[ePressSens] = new BdSetting(ePressSens);

    SettingList[eFlowSens] = new BdSetting(eFlowSens);

    SettingList[eBaseFlow] = new BdSetting(eBaseFlow);

    SettingList[ePctO2] = O2Setting::S_GetInstance();

    SettingList[eFlushPctO2] = new BdSetting(eFlushPctO2);//Should be defined like ePctO2

    SettingList[eRr] = new BdSetting(eRr,triggerMgrPtr->GetTriggerPtr(eBpTimeTrigger), eTrue);

    SettingList[eNIVRr] = new BdSetting(eNIVRr,triggerMgrPtr->GetTriggerPtr(eNIVBpTimeTrigger), eTrue);

    SettingList[eSupportPress] = new BdSetting(eSupportPress);

    SettingList[eRiseTime] = new BdSetting(eRiseTime);

    SettingList[eExhSens] = new BdSetting(eExhSens);

    SettingList[eBaselinePress] = new BdSetting(eBaselinePress);

    SettingList[eInhTime] = new BdSetting(eInhTime,triggerMgrPtr->GetTriggerPtr(eMandPressInhTimeTrigger), eFalse);

    SettingList[ePcvInhPress] = new BdSetting(ePcvInhPress);

    SettingList[eAPRVLowInhPress] = new BdSetting(eAPRVLowInhPress);

    SettingList[eAPRVHighInhPress] = new BdSetting(eAPRVHighInhPress);

    SettingList[eAPRVTimeHigh] = new BdSetting(eAPRVTimeHigh,triggerMgrPtr->GetTriggerPtr(eAPRVHighPressTimeTrigger), eFalse);

    SettingList[eAPRVTimeLow] = new BdSetting(eAPRVTimeLow,triggerMgrPtr->GetTriggerPtr(eAPRVLowPressTimeTrigger), eFalse);

    SettingList[eHipLimit] = new BdSetting(eHipLimit);

    SettingList[eLipLimit] = new BdSetting(eLipLimit);

    SettingList[eLowBaselineLimit] = new BdSetting(eLowBaselineLimit);

    SettingList[eHrrLimit] = new BdSetting(eHrrLimit);

    SettingList[eApneaLimit] = new BdSetting(eApneaLimit,triggerMgrPtr->GetTriggerPtr(eApneaAlarmTimeTrigger), eFalse);

    SettingList[eExtremeHighPEEP_Autoset] = new BdSetting(eExtremeHighPEEP_Autoset);

    SettingList[eExtremeHighInsp_Autoset] = new BdSetting(eExtremeHighInsp_Autoset);

    //#ifdef HFO_SYSTEM
    SettingList[eHFOFrequency] = new BdSetting(eHFOFrequency);
    SettingList[eHFOStrokeVolume] = new BdSetting(eHFOStrokeVolume);
    SettingList[eHFOMap] = new BdSetting(eHFOMap);

    SettingList[eFreshGas] = new BdSetting(eFreshGas);
    SettingList[eLowMAPLimit] = new BdSetting(eLowMAPLimit);
    SettingList[eHiMAP_1Limit] = new BdSetting(eHiMAP_1Limit);
    SettingList[eHiMAP_2Limit] = new BdSetting(eHiMAP_2Limit);
    SettingList[eExtremeHighMap_Autoset] = new BdSetting(eExtremeHighMap_Autoset);

    SettingList[eLowAmplitudeLimit] = new BdSetting(eLowAmplitudeLimit);

    SettingList[eHighAmplitudeLimit] = new BdSetting(eHighAmplitudeLimit);

    SettingList[eLowAmplitude_Autoset] = new BdSetting(eLowAmplitude_Autoset);
    SettingList[eHighAmplitude_Autoset] = new BdSetting(eHighAmplitude_Autoset);

    SettingList[eSI] = new BdSetting(eSI);

    SettingList[eAmplitude] = new BdSetting(eAmplitude);
    SettingList[eSITime] = new BdSetting(eSITime);
    SettingList[eSIRate] = new BdSetting(eSIRate);
    SettingList[eAmplitudeControl] = new BdSetting(eAmplitudeControl);
    SettingList[eAutoSI] = new BdSetting(eAutoSI);

    SettingList[eTidalVolume] = new BdSetting(eTidalVolume);
    SettingList[eInspFlow] = new BdSetting(eInspFlow);
    SettingList[eWaveForm] = new BdSetting(eWaveForm);

    SettingList[ePlateauTime] = new BdSetting(ePlateauTime,triggerMgrPtr->GetTriggerPtr(ePlateauTimeTrigger), eFalse);

    SettingList[eBackupBR] = new BdSetting(eBackupBR,triggerMgrPtr->GetTriggerPtr(eBpBackupTimeTrigger), eTrue);

    SettingList[eSyncPlus] = new BdSetting(eSyncPlus);
    SettingList[eTubeDiameter] = new BdSetting(eTubeDiameter);
    SettingList[eTubeLength] = new BdSetting(eTubeLength);
    SettingList[eCompRate] = new BdSetting(eCompRate);

    SettingList[eNasalCPAPFlow] = new BdSetting(eNasalCPAPFlow);
    SettingList[eTiLimit] = new BdSetting(eTiLimit,triggerMgrPtr->GetTriggerPtr(eItLongAlarmTimeTrigger),eFalse);

    SettingList[eHighNCPAPLimit] = new BdSetting(eHighNCPAPLimit);
    SettingList[eLowNCPAPLimit] = new BdSetting(eLowNCPAPLimit);

    SettingList[eAPRVHighPHighLimit] = new BdSetting(eAPRVHighPHighLimit);

    SettingList[eAPRVLowPHighLimit] = new BdSetting(eAPRVLowPHighLimit);
    SettingList[eAPRVHighPLowLimit] = new BdSetting(eAPRVHighPLowLimit);
    SettingList[eAPRVLowPLowLimit] = new BdSetting(eAPRVLowPLowLimit);

    SettingList[eHighTidalVolLimit] = new BdSetting(eHighTidalVolLimit);
    SettingList[eLowTidalVolLimit] = new BdSetting(eLowTidalVolLimit);

    SettingList[eProximalLeakLimit] = new BdSetting(eProximalLeakLimit);

    SettingList[eHighTidalVolAutoSet] = new BdSetting(eHighTidalVolAutoSet);
    SettingList[eLowTidalVolAutoSet] = new BdSetting(eLowTidalVolAutoSet);

    SettingList[eAPRVExtremeHighPHighAutoSet] = new BdSetting(eAPRVExtremeHighPHighAutoSet);
    //#endif

    SettingList[eHighBaselineLimit] = new BdSetting(eHighBaselineLimit);
    SettingList[eHpsvLimit] = new BdSetting(eHpsvLimit);
    SettingList[eLpsvLimit] = new BdSetting(eLpsvLimit);

    SettingList[eLemtvLimit] = new BdSetting(eLemtvLimit);
    SettingList[eLestvLimit] = new BdSetting(eLestvLimit);
    SettingList[eHemvLimit] = new BdSetting(eHemvLimit);
    SettingList[eLemvLimit] = new BdSetting(eLemvLimit);

    // NVRAM settings
    SettingList[eComplFactor] = new BdSetting(eCircuitCompliance);
    SettingList[eComplOnOff] = new BdSetting(eComplOnOff);
    SettingList[eDummyBaselinePress] = new BdSetting(eDummyBaselinePress);

    // Make sure that all setting pointers were properly initialized
    DEBUG_MGR_BDSETTING("initialize MgrBdSetting list(%d) \n\t", eNumBdSettings);
    for (ii = 0; ii < eNumBdSettings; ii++)
    {
        DEBUG_MGR_BDSETTING("%d ", ii);
        ASSERTION(SettingList[ii] != NULL);
    }
    DEBUG_MGR_BDSETTING("Finish - \n");

}   // end MgrBdSetting()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HandleVolumeSettings()
//
//    Processing: 
//      This private method is invoked from ReceiveRequest() whenever
//      a Volume Vent Type batch setting change request is received.  This
//      operation invokes the SetPendingValue() method for all Volume specific
//      BdSetting objects.
//
//    Input Parameters:
//      volSettings - constant reference to the volume settings received in
//          the BD Event.
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
void MgrBdSetting::HandleVolumeSettings (
        const VolumeSpecificSettings& volSettings)
{

    // Set pending values for all Volume Specific Settings
    //    SettingList[eTv]->SetPendingValue (volSettings.TidalVol);
    //    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTv], NULL, volSettings.TidalVol);
    //
    //    SettingList[ePf]->SetPendingValue (volSettings.PeakFlow);
    //    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePf], NULL, volSettings.PeakFlow);
    //
    //    SettingList[eFlowPattern]->SetPendingValue (volSettings.FlowPattern);
    //    HandlephaseInImmediatelyFlag(eFalse, SettingList[eFlowPattern], NULL, volSettings.FlowPattern);

    SettingList[ePlateauTime]->SetPendingValue (volSettings.Plateau);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePlateauTime], NULL, volSettings.Plateau);

    SettingList[eSupportPress]->SetPendingValue (volSettings.SupportPressure);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSupportPress], NULL, volSettings.SupportPressure);

    SettingList[ePressSens]->SetPendingValue (volSettings.PressureSens);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePressSens], NULL, volSettings.PressureSens);

    SettingList[eHipLimit]->SetPendingValue (volSettings.HipLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHipLimit], NULL, volSettings.HipLimit);

    SettingList[eLemtvLimit]->SetPendingValue (volSettings.LemtvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLemtvLimit], NULL, volSettings.LemtvLimit);

    SettingList[eLestvLimit]->SetPendingValue (volSettings.LestvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLestvLimit], NULL, volSettings.LestvLimit);

}   // end HandleVolumeSettings()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HandlePressureSettings()
//
//    Processing:
//      This private method is invoked from ReceiveRequest() whenever
//      a Pressure Vent Type batch setting change request is received.  This
//      operation invokes the SetPendingValue() method for all Pressure specific
//      BdSetting objects.
//
//    Input Parameters:
//      pressSettings - constant reference to the Pressure settings received in
//          the BD Event.
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
void MgrBdSetting::HandlePressureSettings (
        const PressureSpecificSettings& pressSettings)
{
    SettingList[eTriggerType]->SetPendingValue(pressSettings.TriggerType);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTriggerType], NULL, pressSettings.TriggerType);

    // Set pending values for all Pressure specific settings
    SettingList[eInhTime]->SetPendingValue(pressSettings.InhTime);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eInhTime], NULL, pressSettings.InhTime);

    SettingList[ePcvInhPress]->SetPendingValue(pressSettings.InhPress);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePcvInhPress], NULL, pressSettings.InhPress);

    SettingList[eSupportPress]->SetPendingValue(pressSettings.SupportPressure);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSupportPress], NULL, pressSettings.SupportPressure);

    SettingList[ePressSens]->SetPendingValue(pressSettings.PressureSens);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePressSens], NULL, pressSettings.PressureSens);

    SettingList[eFlowSens]->SetPendingValue(pressSettings.FlowSens);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eFlowSens], NULL, pressSettings.FlowSens);

    SettingList[eHipLimit]->SetPendingValue(pressSettings.HipLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHipLimit], NULL, pressSettings.HipLimit);

    SettingList[eHpsvLimit]->SetPendingValue(pressSettings.HpsvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHpsvLimit], NULL, pressSettings.HpsvLimit);

    SettingList[eLemvLimit]->SetPendingValue(pressSettings.LemvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLemvLimit], NULL, pressSettings.LemvLimit);

    SettingList[eHemvLimit]->SetPendingValue(pressSettings.HemvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHemvLimit], NULL, pressSettings.HemvLimit);

    SettingList[eHipLimit]->SetPendingValue(pressSettings.HipLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHipLimit], NULL, pressSettings.HipLimit);

    SettingList[eLemtvLimit]->SetPendingValue(pressSettings.LemtvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLemtvLimit], NULL, pressSettings.LemtvLimit);

    SettingList[eLestvLimit]->SetPendingValue(pressSettings.LestvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLestvLimit], NULL, pressSettings.LestvLimit);

    if(((E_TriggerType)SettingList[eTriggerType]->GetPendingValue() == ePressureTrigger))
    {
        FlowTrigOn = eFalse;
        PressTriggOn = (SettingList[ePressSens]->GetPendingValue() > 1000) ? eFalse:eTrue;
        if(PressTriggOn)
            (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(ePressureTrigger);
    }
    else if(((E_TriggerType)SettingList[eTriggerType]->GetPendingValue() == eFlowTrigger))
    {
        FlowTrigOn = (SettingList[eFlowSens]->GetPendingValue() > 1000) ? eFalse:eTrue;
        if(FlowTrigOn==eFalse)
            PressTriggOn = eFalse;
        else
        {
            (PhaseMgr::S_GetInstance())->ProcessNewTriggerType(eFlowTrigger);
            PressTriggOn = (MgrBdSetting::S_GetInstance()->GetCurrentValue(ePressSens)>1000)? eFalse:eTrue;
        }
    }

    SettingList[eBaseFlow]->SetPendingValue(pressSettings.ExhFlow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eBaseFlow], NULL, pressSettings.ExhFlow);

    SettingList[eBaselinePress]->SetPendingValue(pressSettings.BaselinePress);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eBaselinePress], NULL, pressSettings.BaselinePress);

    SettingList[eRr]->SetPendingValue(pressSettings.RespRate);
    HandlephaseInImmediatelyFlag(eTrue, SettingList[eRr],
                                 TriggerMgr::S_GetInstance()->GetTriggerPtr(eBpTimeTrigger), pressSettings.RespRate);

    SettingList[eNIVRr]->SetPendingValue(pressSettings.NIVRespRate);
    HandlephaseInImmediatelyFlag(eTrue, SettingList[eNIVRr],
                                 TriggerMgr::S_GetInstance()->GetTriggerPtr(eNIVBpTimeTrigger), pressSettings.NIVRespRate);

    SettingList[eRiseTime]->SetPendingValue(pressSettings.RiseTime);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eRiseTime], NULL, pressSettings.RiseTime);

    SettingList[eExhSens]->SetPendingValue(pressSettings.ExhSens);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eExhSens], NULL, pressSettings.ExhSens);

    // And Common Alarms...
    SettingList[eLipLimit]->SetPendingValue(pressSettings.LipLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLipLimit], NULL, pressSettings.LipLimit);

    SettingList[eHrrLimit]->SetPendingValue(pressSettings.HrrLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHrrLimit], NULL, pressSettings.HrrLimit);

    SettingList[eApneaLimit]->SetPendingValue(pressSettings.eApneaLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eApneaLimit], NULL, pressSettings.eApneaLimit);

    SettingList[eLowBaselineLimit]->SetPendingValue(pressSettings.LowBaselineLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowBaselineLimit], NULL, pressSettings.LowBaselineLimit);

    SettingList[eHighBaselineLimit]->SetPendingValue(pressSettings.HighBaselineLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighBaselineLimit], NULL, pressSettings.HighBaselineLimit);

    SettingList[eLpsvLimit]->SetPendingValue(pressSettings.LpsvLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLpsvLimit], NULL, pressSettings.LpsvLimit);

    SettingList[eExtremeHighPEEP_Autoset]->SetPendingValue(pressSettings.ExtremeHighPEEPAutoset);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eExtremeHighPEEP_Autoset], NULL, pressSettings.ExtremeHighPEEPAutoset);

    SettingList[eExtremeHighInsp_Autoset]->SetPendingValue(pressSettings.ExtremeHighInspAutoset);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eExtremeHighInsp_Autoset], NULL, pressSettings.ExtremeHighInspAutoset);

#ifdef HFO_SYSTEM
    // Set pending values for all HFO specific settings
    SettingList[eHFOMap]->SetPendingValue(pressSettings.Map);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHFOMap], NULL, pressSettings.Map);

    SettingList[eHFOFrequency]->SetPendingValue(pressSettings.Frequency);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHFOFrequency], NULL, pressSettings.Frequency);

    SettingList[eHFOStrokeVolume]->SetPendingValue(pressSettings.StrokeVolume);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHFOStrokeVolume], NULL, pressSettings.StrokeVolume);

    SettingList[eFreshGas]->SetPendingValue(pressSettings.FreshGas);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eFreshGas], NULL, pressSettings.FreshGas);

    SettingList[eExtremeHighMap_Autoset]->SetPendingValue(pressSettings.ExtremeHighMapAutoset);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eExtremeHighMap_Autoset], NULL, pressSettings.ExtremeHighMapAutoset);

    SettingList[eHiMAP_1Limit]->SetPendingValue(pressSettings.HiMAP_1Alarm);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHiMAP_1Limit], NULL, pressSettings.HiMAP_1Alarm);

    SettingList[eHiMAP_2Limit]->SetPendingValue(pressSettings.HiMAP_2Alarm);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHiMAP_2Limit], NULL, pressSettings.HiMAP_2Alarm);

    SettingList[eLowMAPLimit]->SetPendingValue(pressSettings.LowMAPAlarm);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowMAPLimit], NULL, pressSettings.LowMAPAlarm);

    SettingList[eHighAmplitudeLimit]->SetPendingValue(pressSettings.HighAmplitudeLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighAmplitudeLimit], NULL, pressSettings.HighAmplitudeLimit);

    SettingList[eLowAmplitudeLimit]->SetPendingValue(pressSettings. LowAmplitudeLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowAmplitudeLimit], NULL, pressSettings.LowAmplitudeLimit);

    SettingList[eLowAmplitude_Autoset]->SetPendingValue(pressSettings.LowAmplitudeAutoset);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowAmplitude_Autoset], NULL, pressSettings.LowAmplitudeAutoset);

    SettingList[eHighAmplitude_Autoset]->SetPendingValue(pressSettings.HighAmplitudeAutoset);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighAmplitude_Autoset], NULL, pressSettings.HighAmplitudeAutoset);

    SettingList[eSI]->SetPendingValue(pressSettings.SI);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSI], NULL, pressSettings.SI);

    SettingList[eAmplitude]->SetPendingValue(pressSettings.Amplitude);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAmplitude], NULL, pressSettings.Amplitude);

    SettingList[eSITime]->SetPendingValue(pressSettings.SITime);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSITime], NULL, pressSettings.SITime);

    SettingList[eSIRate]->SetPendingValue(pressSettings.SIRate);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSIRate], NULL, pressSettings.SIRate);

    SettingList[eAmplitudeControl]->SetPendingValue(pressSettings.AMPControlType);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAmplitudeControl], NULL, pressSettings.AMPControlType);

    SettingList[eAutoSI]->SetPendingValue(pressSettings.AutoSIType);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAutoSI], NULL, pressSettings.AutoSIType);
#endif

    SettingList[eTidalVolume]->SetPendingValue(pressSettings.TidalVolume);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTidalVolume], NULL, pressSettings.TidalVolume);

    SettingList[eInspFlow]->SetPendingValue(pressSettings.InhFlow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eInspFlow], NULL, pressSettings.InhFlow);

    SettingList[eWaveForm]->SetPendingValue(pressSettings.WaveForm);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eWaveForm], NULL, pressSettings.WaveForm);

    SettingList[ePlateauTime]->SetPendingValue(pressSettings.PlateauTime);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePlateauTime], NULL, pressSettings.PlateauTime);

    SettingList[eBackupBR]->SetPendingValue(pressSettings.BackupBR);
    HandlephaseInImmediatelyFlag(eTrue, SettingList[eBackupBR],
                                 TriggerMgr::S_GetInstance()->GetTriggerPtr(eBpBackupTimeTrigger), pressSettings.BackupBR);

    SettingList[eSyncPlus]->SetPendingValue(pressSettings.SyncPlusType);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eSyncPlus], NULL, pressSettings.SyncPlusType);

    SettingList[eTubeDiameter]->SetPendingValue(pressSettings.TubeDiameter);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTubeDiameter], NULL, pressSettings.TubeDiameter);

    SettingList[eTubeLength]->SetPendingValue(pressSettings.TubeLength);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTubeLength], NULL, pressSettings.TubeLength);

    SettingList[eCompRate]->SetPendingValue(pressSettings.CompRate);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eCompRate], NULL, pressSettings.CompRate);

    SettingList[eAPRVLowInhPress]->SetPendingValue(pressSettings.APRVLowInhPress);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVLowInhPress], NULL, pressSettings.APRVLowInhPress);

    SettingList[eAPRVHighInhPress]->SetPendingValue(pressSettings.APRVHighInhPress);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVHighInhPress], NULL, pressSettings.APRVHighInhPress);

    SettingList[eAPRVTimeLow]->SetPendingValue(pressSettings.APRVTimeLow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVTimeLow], NULL, pressSettings.APRVTimeLow);

    SettingList[eAPRVTimeHigh]->SetPendingValue(pressSettings.APRVTimeHigh);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVTimeHigh], NULL, pressSettings.APRVTimeHigh);

    SettingList[eNasalCPAPFlow]->SetPendingValue(pressSettings.NCPAPFlow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eNasalCPAPFlow], NULL, pressSettings.NCPAPFlow);

    SettingList[eTiLimit]->SetPendingValue(pressSettings.TiLimit);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eTiLimit], NULL, pressSettings.TiLimit);

    SettingList[eHighNCPAPLimit]->SetPendingValue(pressSettings.HighNCPAP);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighNCPAPLimit], NULL, pressSettings.HighNCPAP);

    SettingList[eLowNCPAPLimit]->SetPendingValue(pressSettings.LowNCPAP);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowNCPAPLimit], NULL, pressSettings.LowNCPAP);

    SettingList[eAPRVHighPHighLimit]->SetPendingValue(pressSettings.APRVHighPHigh);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVHighPHighLimit], NULL, pressSettings.APRVHighPHigh);

    SettingList[eAPRVLowPHighLimit]->SetPendingValue(pressSettings.APRVLowPHigh);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVLowPHighLimit], NULL, pressSettings.APRVLowPHigh);

    SettingList[eAPRVHighPLowLimit]->SetPendingValue(pressSettings.APRVHighPLow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVHighPLowLimit], NULL, pressSettings.APRVHighPLow);

    SettingList[eAPRVLowPLowLimit]->SetPendingValue(pressSettings.APRVLowPLow);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVLowPLowLimit], NULL, pressSettings.APRVLowPLow);

    SettingList[eHighTidalVolLimit]->SetPendingValue(pressSettings.HighTidalVolume);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighTidalVolLimit], NULL, pressSettings.HighTidalVolume);

    SettingList[eLowTidalVolLimit]->SetPendingValue(pressSettings.LowTidalVolume);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowTidalVolLimit], NULL, pressSettings.LowTidalVolume);

    SettingList[eHighTidalVolAutoSet]->SetPendingValue(pressSettings.HighTidalVolAutoSet);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHighTidalVolAutoSet], NULL, pressSettings.HighTidalVolAutoSet);

    SettingList[eLowTidalVolAutoSet]->SetPendingValue(pressSettings.LowTidalVolAutoSet);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLowTidalVolAutoSet], NULL, pressSettings.LowTidalVolAutoSet);

    SettingList[eAPRVExtremeHighPHighAutoSet]->SetPendingValue(pressSettings.APRVExHighPHigh);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eAPRVExtremeHighPHighAutoSet], NULL, pressSettings.APRVExHighPHigh);

    SettingList[eProximalLeakLimit]->SetPendingValue(pressSettings.ProximalLeak);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eProximalLeakLimit], NULL, pressSettings.ProximalLeak);

}   // end HandlePressureSettings()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HandleNippvSettings()
//
//    Processing: 
//      This private method is invoked from ReceiveRequest() whenever
//      a NIPPV Vent Type batch setting change request is received.  This
//      operation invokes the SetPendingValue() method for all NIPPV specific
//      BdSetting objects.
//
//    Input Parameters:
//      nippvSettings - constant reference to the NIPPV settings received in
//          the BD Event.
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
void MgrBdSetting::HandleNippvSettings (
        const NippvSpecificSettings& nippvSettings)
{
    // Set pending values for all NIPPV specific settings
    //    SettingList[eNippvIpap]->SetPendingValue (nippvSettings.Ipap);
    //    HandlephaseInImmediatelyFlag(eFalse, SettingList[eNippvIpap], NULL, nippvSettings.Ipap);

    SettingList[eInhTime]->SetPendingValue (nippvSettings.InhTime);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eInhTime], NULL, nippvSettings.InhTime);

    //    SettingList[eLetvLimit]->SetPendingValue (nippvSettings.LetvLimit);
    //    HandlephaseInImmediatelyFlag(eFalse, SettingList[eLetvLimit], NULL, nippvSettings.LetvLimit);

    SettingList[eHipLimit]->SetPendingValue (
            nippvSettings.Ipap + NPPV_HIP_OFFSET);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eHipLimit], NULL, nippvSettings.Ipap + NPPV_HIP_OFFSET);

    SettingList[eDummyBaselinePress]->SetPendingValue (0);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eDummyBaselinePress], NULL, 0);

}   // end HandleNippvSettings()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPressTriggOn()
//
//    Processing:  get status of trigger
//
//    Input Parameters:
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PressTriggOn
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
E_Bool MgrBdSetting::GetPressTriggOn (void)
{
    return PressTriggOn;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetFlowTriggOn()
//
//    Processing: get status of trigger
//
//    Input Parameters:
//
//    Output Parameters:
//      None
//
//    Return Values:
//      PressTriggOn
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
E_Bool MgrBdSetting::GetFlowTriggOn (void)
{
    return FlowTrigOn;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HandleCommonSettings()
//
//    Processing: 
//      This private method is invoked from ReceiveRequest() whenever
//      any batch setting change request is received.  This operation
//      invokes the SetPendingValue() method for all BdSetting objects that
//      are common for Pressure, Volume and NIPPV Ventilation Types.
//
//    Input Parameters:
//      bdEvent - constant reference to the settings change event received
//                  on the BD Queue
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
void MgrBdSetting::HandleCommonSettings (const BdEvent& bdEvent)
{
    // Set pending values for all common settings
    SettingList[ePctO2]->SetPendingValue(bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.PctO2);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[ePctO2], NULL, bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.PctO2);

    SettingList[eFlushPctO2]->SetPendingValue(bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.FlushPctO2);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eFlushPctO2], NULL, bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.FlushPctO2);

    //Set ComplianceOnOff value
    SettingList[eComplOnOff]->SetPendingValue(bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.ComplianceOnOff);
    HandlephaseInImmediatelyFlag(eFalse, SettingList[eComplOnOff], NULL, bdEvent.BatchSettingsChangeEvent.VentTypeSettings.CommonSettings.ComplianceOnOff);

}   // end HandleCommonSettings()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCurrentValue()
//
//    Processing:
//      This operation invokes the GetCurrentValue() operation of the setting
//      whose id is passed to this operation.
//
//    Input Parameters:
//      id - Setting Id of the setting to be retrieved
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - setting value
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
LONG MgrBdSetting::GetCurrentValue (E_BdSettingId id)
{
    ASSERTION(id < eNumBdSettings);
    return (SettingList[id]->GetCurrentValue ());

}   // end GetCurrentValue()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPendingValue()
//
//    Processing:
//      When a setting change comes, if this is a immediately setting,
//      do phaseIn now and don't add to setting list and vice versus
//
//    Input Parameters:
//      E_Bool phaseInImmediately - Process immediately action or not
//      BdSetting* bdSettingPtr - bdSetting pointer to add to list
//      Trigger* triggerPtr - trigger pointer to phaseIN
//      LONG value  - LONG value to phaseIN
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - setting value
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
void MgrBdSetting::HandlephaseInImmediatelyFlag (
        E_Bool phaseInImmediately, BdSetting* bdSettingPtr, Trigger* TriggerPtr,
        LONG value)
{
    if((NULL != TriggerPtr) && (eTrue == phaseInImmediately))
    {
        // Notify the associated trigger that the value has changed.
        TriggerPtr->PhaseInSettingChange (value);
    }
    else
    {
        // Notify MgrBdSetting to add this setting to a list of settings to be
        // phased in on the next breath phase boundary.
        AddSettingToChangeList(bdSettingPtr);
    }
}

