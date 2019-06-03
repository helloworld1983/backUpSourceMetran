/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SelfTestMode.cpp
//   Purpose: 
//      This file contains the definition of the SelfTestMode mode.
//      The SelfTestMode mode is entered in response to the system self test.
//
//   Interfaces:
//
//   Restrictions:
//      There is only 1 instance of this class.
//
//   Revision History:
//
/******************************************************************************/
#include "SelfTestMode.h"
#include "TriggerMgr.h"
#include "PhaseMgr.h"
//#include "VirtualPressure.h"
#include "AlarmStat.h"
//#include "flow_cnt.h"
#include "ModeMgr.h"
#include "AzTest.h"

#include "DebugTrace.h"

// Initialize statics
SelfTestMode* SelfTestMode::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the SelfTestMode object, it invokes SelfTestMode::S_GetInstance(),
//      which returns a pointer to the SelfTestMode object.
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
//      SelfTestMode* - pointer to the object instantiated from this class
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
SelfTestMode* SelfTestMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new SelfTestMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SelfTestMode()
//
//    Processing: 
//      This operation is the constructor for the Self Test mode.  It invokes 
//      the base class AlarmBreathingMode constructor.
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
SelfTestMode::SelfTestMode(void):Mode()
{

    CurrentTestId = eNoBdEventId;

}  // end SelfTestMode()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Enter()
//
//    Processing: 
//
//    Input Parameters:
//      fromModeId - ID of the Mode that is being exited 
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
void SelfTestMode::Enter(E_VentilationMode fromModeId)
{
    // Initialize pointer to the PhaseMgr
    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();
    // Put the ventilator into the SelfTest phase.
    phaseMgrPtr->ProcessNewPhase(eSelfTestPhase);

    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
    // Disable all triggers.  This call must be made after forcing the vent
    // into the oxygen calibration phase above because depending on the phase
    // transitioned from, there may be some triggers left enabled.
    triggerMgrPtr->DisableAllTriggers();

//    InhPressureZero->SetDesiredState(eOff);
//    ExhPressureZero->SetDesiredState(eOff);
//
//    SafetyValve->SetDesiredState(eOpen);

    // Suspend auto-zero scheduling.
//    PInhAzTestPtr->SuspendTest();
//    PExhAzTestPtr->SuspendTest();

}   // end Enter()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Exit()
//
//    Processing: 
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
void SelfTestMode::Exit(void)
{
    // Initialize pointer to the TriggerMgr
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers.
    triggerMgrPtr->DisableTrigger(eUpdateStatusTimeTrigger);
    triggerMgrPtr->DisableTrigger(eAlarmResetOpReqTrigger);

    CurrentTestId = eNoBdEventId;
    // Resume auto-zero scheduling
//    PInhAzTestPtr->ResumeTest();
//    PExhAzTestPtr->ResumeTest();

}   // end Exit()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateAlarmStatus()
//
//    Processing: 
//      This operation simply performs error handling.  It should never be
//      invoked but is defined here since it is declared as a pure virtual
//      operation in the base class.
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
//      This operation should not be invoked.
//
//    Requirements:
//
/******************************************************************************/
void SelfTestMode::UpdateAlarmStatus(void)
{
    // This operation should never be called.
    ASSERTION(eFalse);

}  // end UpdateAlarmStatus()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessModeChangeTrigger()
//
//    Processing:
//      This operation simply performs error handling.  It should never be
//      invoked but is defined here since it is declared as a pure virtual
//      operation in the base class.
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
//      This operation should not be invoked.
//
//    Requirements:
//
/******************************************************************************/
void SelfTestMode::ProcessModeChangeTrigger(void)
{
    (ModeMgr::S_GetInstance())->GoToOperatorSetMode();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTriggerID(E_TriggerId id)
//
//    Processing: Process action for each trigger ID of Self Test Mode
//
//    Input Parameters:
//      E_TriggerId id
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
void SelfTestMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case eModeChangeTrigger:
            ProcessModeChangeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}
