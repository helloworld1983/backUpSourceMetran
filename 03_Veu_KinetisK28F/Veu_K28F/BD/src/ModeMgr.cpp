/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ModeMgr.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the Mode Manager class.   The ModeMgr coordinates
//      entry and exit into the different ventilation modes.  Modes include
//      both user selectable modes such as Assist/Control and emergency
//      modes such as Apena Ventilation Mode.
//
//   Interfaces:
//      The ModeMgr interfaces with all of the the ventilator mode objects.
//      It also interfaces with the Mode Setting object.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include <ModeMgr.h>

#include "MgrBdSetting.h"

#include <OcclMode.h>
#include "AcMode.h"
#include "AvMode.h"
#include "MixedMode.h"
#include "SpontMode.h"
#include "NoGasMode.h"

#include "SelfTestMode.h"
#include "MEMode.h"
#include "StandbyMode.h"
#include "PCV_SIMV_Mode.h"
#include "PCV_AC_Mode.h"
#include "VCV_SIMV_Mode.h"
#include "VCV_AC_Mode.h"
#include "VCV_CPAP_Mode.h"
#include "PCV_CPAP_Mode.h"
#include "N_CPAPMode.h"
#include "DisconnectionMode.h"
#include "HighPressMode.h"
#include "ApneaBackupMode.h"
#include "APRVMode.h"
#include "VA_AC_Mode.h"
#include "VA_SIMV_Mode.h"
#include "ApneaVAMode.h"
#include "HardwareErrorMode.h"
#include "NIV_Mode.h"

#ifdef HFO_SYSTEM
#include "HFOMode.h"
#include "HFOHighMAP_2Mode.h"
#include "HFOExtremeLowMapMode.h"
#endif

#include "DebugTrace.h"

// Define statics
ModeMgr* ModeMgr::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the ModeMgr object, it invokes ModeMgr::S_GetInstance(),
//      which returns a pointer to the ModeMgr object.
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
//      ModeMgr* - pointer to the object instantiated from this class
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
ModeMgr* ModeMgr::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new ModeMgr ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ModeMgr()
//
//    Processing: 
//      This operation is the constructor for the ModeMgr object.  This 
//      operation initializes the ModeList array by instantiating all of the
//      modes and initializing the array with the pointers to the modes.
//      This operation also puts the ventilator in the Start-Up Mode by invoking
//      the Enter() operation of that mode class.  
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
ModeMgr::ModeMgr (void)
{
    // There are no entries on the ActiveModeIdList.  The ActiveIx always 
    // contains the index of the NEXT available location.
    for(int i = 0; i < NUM_ALARM_MODE_IDS; ++i)
    {
        ActiveModeIdList[i] = eStandbyMode;
    }

    ActiveIx = 0;

    ModeList[eSelfTestMode] = SelfTestMode::S_GetInstance();
    ModeList[eMEMode] = MEMode::S_GetInstance();
    ModeList[eStandbyMode] = StandbyMode::S_GetInstance();
    ModeList[ePcvMixedMode] = PCV_SIMV_Mode::S_GetInstance();

    ModeList[ePcvACMode] = PCV_AC_Mode::S_GetInstance();
    ModeList[eVcvMixedMode] = VCV_SIMV_Mode::S_GetInstance();
    ModeList[eVcvACMode] = VCV_AC_Mode::S_GetInstance();
    ModeList[eVcvSpontMode] = VCV_CPAP_Mode::S_GetInstance();
    ModeList[ePcvSpontMode] = PCV_CPAP_Mode::S_GetInstance();

    ModeList[eNasalCPAPMode] = N_CPAPMode::S_GetInstance();

#ifdef HFO_SYSTEM
    ModeList[eHFOMode] = HFOMode::S_GetInstance();
    ModeList[eHFOHighMAP_2Mode] = HFOHighMAP_2Mode::S_GetInstance();
    ModeList[eHFOExtremeLowMAPMode] = HFOExtremeLowMapMode::S_GetInstance();
#endif

    ModeList[eDisconnectionMode] = DisconnectionMode::S_GetInstance();
    ModeList[eHighPressMode] = HighPressMode::S_GetInstance();
    ModeList[eNoGasMode] = NoGasMode::S_GetInstance();
    ModeList[eHardwareErrorMode] = HardwareErrorMode::S_GetInstance();
    ModeList[eApneaBackupMode] = ApneaBackupMode::S_GetInstance();

    ModeList[eAPRVMode] = APRVMode::S_GetInstance();
    ModeList[eNivMode] = NIV_Mode::S_GetInstance();
    ModeList[eVaACMode] = VA_AC_Mode::S_GetInstance();
    ModeList[eVaMixedMode] = VA_SIMV_Mode::S_GetInstance();
    ModeList[eApneaVAMode] = ApneaVAMode::S_GetInstance();

    ModeList[eMixedMode] = MixedMode::S_GetInstance ();
    ModeList[eSpontaneousMode] = SpontMode::S_GetInstance ();
    ModeList[eApneaMode] = AvMode::S_GetInstance ();
    ModeList[eOcclusionMode] = OcclMode::S_GetInstance ();

    // Put the ventilator in the Start-Up Mode.  The argument indicates the 
    // mode transitioning from, but in this case it is not used
    NextModeId = eStandbyMode;
    CurrentModeId = eStandbyMode;

    ModeList[CurrentModeId]->Enter (eStandbyMode);

}   // end ModeMgr()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNextModeId()
//
//    Processing:
//                  return the current ID
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_VentilationMode - CurrentModeId: current mode ID
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
E_VentilationMode ModeMgr::GetCurrentModeId (void)
{
    return CurrentModeId;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetNextModeId()
//
//    Processing:
//                  return the next mode ID
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_VentilationMode - nextMode: return next mode ID
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
E_VentilationMode ModeMgr::GetNextModeId()
{
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    E_VentilationMode nextMode = (E_VentilationMode) mgrBdSettingPtr->GetPendingValue(eMode);
    return(nextMode);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GoToOperatorSetMode()
//
//    Processing: 
//      This operation causes the ventilator to exit its current mode by
//      invoking the Exit() method of the currently active mode.  It then
//      puts the ventilator in the operator set mode by invoking the 
//      Enter() operation of the set mode.  The set mode is determined by
//      invoking the GetPendingValue() operation of the Mode setting object.
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
void ModeMgr::GoToOperatorSetMode (void)
{
    DEBUG_MGR_MODE("GoToOperatorSetMode \n");
    ActiveIx = 0;

    // Get pointer to the MgrBdSetting object.
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance ();

    // Retrieve the current operator setting for Mode    
    NextModeId = (E_VentilationMode) mgrBdSettingPtr->GetPendingValue (eMode);

    // Exit the current mode
    DEBUG_MGR_MODE("Exit mode(%d) \n", CurrentModeId);
    ModeList[CurrentModeId]->Exit ();

    // CurrentMode Id contains the mode ID of the mode being exited.  The
    // Enter() processing for the new mode varies depending upon the mode
    // it is being entered from.
    DEBUG_MGR_MODE("Enter mode(%d) \n", NextModeId);
    ModeList[NextModeId]->Enter (CurrentModeId);

    // Update the current mode ID; this update must occur after the call to 
    // Enter().
    CurrentModeId = NextModeId;

}   // end GoToOperatorSetMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GoToAlarmMode()
//
//    Processing: 
//      This operation causes the ventilator to transition out of the current
//      mode into the requested alarm mode.  
//
//    Input Parameters:
//      nextMode - mode that the ventilator will transition into
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      nextMode is an alarm mode.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void ModeMgr::GoToAlarmMode (E_VentilationMode nextMode)
{
    // Must be going to an alarm mode
    if((nextMode < eFirstAlarmMode) || (nextMode > eLastAlarmMode))
    {
        ASSERTION(eFalse);
    }

    NextModeId = nextMode;

    // Leave the current mode
    ModeList[CurrentModeId]->Exit ();

    // CurrentMode Id contains the mode ID of the mode being exited.  The
    // Enter() processing for the new mode varies depending upon the mode
    // it is being entered from.
    ModeList[nextMode]->Enter (CurrentModeId);

    // Update the current mode ID.   This call must be made AFTER the call to
    // Enter() since Enter needs the last mode ID.
    CurrentModeId = nextMode;

    // Add the mode ID to the active mode list
    if(ActiveIx < NUM_ALARM_MODE_IDS)
    {
        ActiveModeIdList[ActiveIx] = CurrentModeId;
        ActiveIx++;
    }
    else
    {
        ASSERTION(eFalse);
    }

}   // end GoToAlarmMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GoToMEMode()
//
//    Processing:
//      This operation causes the ventilator to transition out of the current
//      mode into the ME mode.
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
void ModeMgr::GoToMEMode(void)
{
    ActiveIx = 0;

    // Retrieve the pending operator setting for Mode
    NextModeId = eMEMode;

    // Exit the current mode
    ModeList[CurrentModeId]->Exit();

    // CurrentMode Id contains the mode ID of the mode being exited.  The
    // Enter() processing for the new mode varies depending upon the mode
    // it is being entered from.
    ModeList[NextModeId]->Enter(CurrentModeId);

    // Update the current mode ID; this update must occur after the call to
    // Enter().
    CurrentModeId = NextModeId;

}   // end GoToStandbyMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GoToSTMode()
//
//    Processing:
//      This operation causes the ventilator to transition out of the current
//      mode into the SelfTest mode.
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
void ModeMgr::GoToSelfTestMode(void)
{
    ActiveIx = 0;

    // Retrieve the pending operator setting for Mode
    NextModeId = eSelfTestMode;

    // Exit the current mode
    ModeList[CurrentModeId]->Exit();

    // CurrentMode Id contains the mode ID of the mode being exited.  The
    // Enter() processing for the new mode varies depending upon the mode
    // it is being entered from.
    ModeList[NextModeId]->Enter(CurrentModeId);

    // Update the current mode ID; this update must occur after the call to
    // Enter().
    CurrentModeId = NextModeId;

}   // end GoToStandbyMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReturnToLastMode()
//
//    Processing: 
//      This operation causes the ventilator to transition out of the current
//      mode into the last mode in the active list.  This mode is invoked from
//      alarm breathing mode objects when an auto-reset occurs but the object
//      does not know what mode to go back to.
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
void ModeMgr::ReturnToLastMode (void)
{

#ifdef HFO_SYSTEM
    if((E_VentilationMode) MgrBdSetting::S_GetInstance()->GetPendingValue(eMode) == eHFOMode)
        ActiveIx = 0;
#endif

    // If the only active mode was the fromMode, return to the operator set
    // mode.
    if(0 == ActiveIx)
    {
        GoToOperatorSetMode ();
    }

    else
    {
        // Leave the current mode
        ModeList[CurrentModeId]->Exit ();

        // Determine next mode ID
        NextModeId = ActiveModeIdList[ActiveIx - 1];

        // CurrentMode Id contains the mode ID of the mode being exited.  
        // Enter() processing for the new mode varies depending upon the 
        // mode it is being entered from.
        ModeList[NextModeId]->Enter (CurrentModeId);

        // Update the current mode ID.  
        CurrentModeId = NextModeId;
    }

}   // end ReturnToLastMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessAutoReset()
//
//    Processing: 
//      This operation is invoked when an auto-reset is detected for any of the
//      alarm modes.  This operation decrements the ActiveIx value, effectively
//      removing the current mode ID from the active list.
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
//      If the current mode is not the last one on the list, perform error
//      handling.
//
//    Requirements:  
//
/******************************************************************************/
void ModeMgr::ProcessAutoReset (void)
{

    // An auto-reset can only occur for the currently active alarm mode, which
    // is the last element on the ActiveModeIdList.  By decrementing the
    // ActiveIx, the current mode is effectively removed from the 
    // ActiveModeIdList.
    ActiveIx--;

    // Verify that the CurrentModeId was really the one that just got removed.
    // Otherwise, perform error handling.
    if(CurrentModeId != ActiveModeIdList[ActiveIx])
    {
        ASSERTION(eFalse);
    }

}   // ProcessAutoReset()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessManualReset()
//
//    Processing: 
//      This operation is invoked from the AlarmResetOpReqTrigger when the
//      trigger is armed.   This operation goes through the ActiveModeIdList
//      and removes any mode IDs for modes that are reset by the alarm reset
//      request.  
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
void ModeMgr::ProcessManualReset (void)
{
    SHORT ii = 0;       // loop control
    SHORT jj;           // loop control

    // The ActiveIx is pointing to the NEXT location in the ActiveModeIdList.
    // So, look at all elements currently on the list.    
    while (ii < ActiveIx)
    {
        // If the mode is affected by the operator requested reset,
        // remove it from the list move other elements in the list up.
        if(ModeList[ActiveModeIdList[ii]]->GetDoesManualReset () == eTrue)
        {
            for (jj = 0; jj < (ActiveIx - 1); jj++)
            {
                ActiveModeIdList[jj] = ActiveModeIdList[jj + 1];
            }

            ActiveIx--;
        }
        else
        {
            ii++;
        }
    }

}   // end ProcessManualReset()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ClearExtremeHighCounter()
//
//    Processing:
//               This function is used to reset extreme high press or map when change mode
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
void ModeMgr::ClearExtremeHighCounter()
{
//    HighPressMode::S_GetInstance()->initializeTimer();
//    HFOHighMAP_2Mode::S_GetInstance()->initializeTimer();
}
