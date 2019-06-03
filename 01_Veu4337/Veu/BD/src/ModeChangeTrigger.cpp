/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ModeChangeTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Mode Change Trigger, which
//      is derived from the Trigger base class.   This class provides the
//      functionality to process operator requested mode changes. 
//
//   Interfaces:
//      The CheckCondition() operation interfaces with the ExhPhase object
//      via S_IsInUnrestrictedPhase() to determine if the unrestricted phase
//      of exhalation is active.   It also interfaces with the TriggerMgr
//      via PreCheckCondition() to pre-check the status of the patient initiated
//      inhalation triggers.  Otherwise, operations in this class only
//      invoke methods defined in the classes from which it is derived.
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include <ExhPhase.h>
#include "ModeChangeTrigger.h"
#include "TriggerMgr.h"
#include "AlarmTask.h"
#include "AlarmInterface.h"
#include "MgrController.h"
#include "MgrStemMotor.h"
#include "MgrFlowController.h"
#include "HfoServo.h"
#include "NCPAPController.h"

//#include "BdTask.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ModeChangeTrigger()
//
//    Processing: 
//      This operation is the constructor for the Mode Change Trigger. This
//      operation initializes the CurrentMode attribute to an invalid mode
//      so that the initial mode setting gets phased in as a mode change.  It
//      also initializes the IsArmed attribute to False and calls the base
//      class constructor with the trigger ID.
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
ModeChangeTrigger::ModeChangeTrigger (void) :
        Trigger (eModeChangeTrigger), CurrentMode (eNumberOfModes),
        IsArmed (eFalse)
{
    CurrentMode = eNumberOfModes;       // set to an invalid value so that first
    // setting is phased in

}   // end ModeChangeTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This operation is called whenever a mode is entered where operator
//      requested mode changes can be phased in.  If the ventilator is is
//      an alarm mode, the operator requested mode change (e.g. SIMV->A/C)
//      is not honored until the alarm mode is exited. 
//
//      All Normal Breathing Modes enable this trigger by invoking this 
//      operation.  This operation sets the IsArmed attribute to FALSE, 
//      indicating that no mode change request has been made and invokes the 
//      base class Trigger::EnableTrigger() operation.
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
void ModeChangeTrigger::EnableTrigger (void)
{
    IsArmed = eFalse;
    Trigger::EnableTrigger ();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD Task cycle while the Mode Change
//      Trigger is enabled.
//
//      This operation checks if the IsArmed flag is True, indicating that
//      a mode change request has been received.  If the request has been
//      received and the ventilator is in the unrestricted phase of exhalation,
//      the TriggerDidFire attribute is set to True.  If the request has been
//      made but the ventilator is not in the unrestricted phase of exhalation,
//      TriggerMgr::PreCheckCondtion() is invoked to pre-check if the breath
//      period time trigger or a patient inhalation trigger is
//      ready to fire.  If one of those triggers is ready to fire, 
//      TriggerDidFire is set to True to cause the mode change trigger to 
//      fire early.  The inhalation trigger is then processed in the new mode.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool ModeChangeTrigger::CheckCondition (void)
{
    // Check if a mode change request has been received (indicated by IsArmed) 
    IsArmed = eTrue;


    if (eTrue == IsArmed)
    {
        {
            if((MgrStemMotor::HfoValve->GetStepPosition() != 0)||(MgrStemMotor::O2Valve->GetStepPosition() != 0)
                    ||(MgrStemMotor::AirValve->GetStepPosition() != 0)||(MgrStemMotor::ExhValve->GetStepPosition() != 0))
            {


                MgrStemMotor::HfoValve->RampToPosition(0);
                MgrStemMotor::O2Valve->RampToPosition((USHORT)0);
                MgrStemMotor::AirValve->RampToPosition((USHORT)0);
                MgrStemMotor::ExhValve->RampToPosition((USHORT)0);
//                return eFalse;
            }
        }
        AlarmTask::S_GetInstance()->ClearAllAlarm();
        AlarmTask::S_GetInstance()->ClearHFOAlarm();

//        MgrController::CMVServoController->ResetIMVStaticVariables();
//        MgrController::VCVServoController->ResetIMVStaticVariables();

        IsArmed = eFalse;
        TriggerDidFire = eTrue;

    }
    return(TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation is called whenever the Mode setting value is changed.
//      This operation compares the new mode passed as an input parameter to
//      the CurrentMode attribute.  If they are different, the CurrentMode
//      attribute is updated and the IsArmed flag is set if the the trigger
//      is enabled.  If the trigger is not enabled, the mode change will be
//      phased in when the ventilator returns to the operator set mode.
//
//    Input Parameters:
//      value - new mode value
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
void ModeChangeTrigger::PhaseInSettingChange (LONG value)
{
    if (CurrentMode != (E_VentilationMode) value)
    {
        if(CurrentMode == eHFOMode)
        {
//            BdTask::S_GetInstance()->Send(eStopAutoSetRequest);
        }
        CurrentMode = (E_VentilationMode) value;
//
//        // If the trigger is enabled, it is not armed because the mode change
//        // request came in.
        TriggerMgr::S_GetInstance()->DisableAllTriggers();
        TriggerMgr::S_GetInstance()->EnableTrigger(eModeChangeTrigger);
//
//        HfoServo::S_GetInstance()->Disable();
//        HfoController->Disable();
//        MgrController::CMVServoController->Disable();
//        MgrController::CMVServoController->Disable();

//        NCPAPController::S_GetInstance()->Disable();
//        MgrFlowController::O2FlowController->Disable();
//        MgrFlowController::AirFlowController->Disable();
        PhaseMgr::S_GetInstance()->ProcessNewPhase(eNonVentPhase);
        IsArmed = IsEnabled;
    }

}   // end PhaseInSettingChange()

