/******************************************************************************/
//$COMMON.CPP$
//   File Name:  PtInhTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Alarm Reset 
//      Operator Request Trigger.  This class multiply inherits from the
//      Trigger base class and the OpRequest base class and provides the 
//      functionality to manually reset alarm conditions such as Apnea
//      in response to an operator request.
//
//   Interfaces:
//      The CheckCondition() operation interfaces with the PhaseMgr to determine
//      the current breath phase.  Otherwise, operations in this class only
//      invoke methods defined in the classes from which it is derived.
//
//   Restrictions:
//      There is only 1 instance of this class.
/******************************************************************************/
#include "PtInhTrigger.h"
#include "ExhPhase.h"
#include "DebugTrace.h"
PtInhTrigger* PtInhTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the PtInhTrigger object, it invokes
//      PtInhTrigger::S_GetInstance(), which returns a pointer to
//      the PtInhTrigger object.
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
//      PtInhTrigger* - pointer to the instance of this class
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
PtInhTrigger* PtInhTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new PtInhTrigger();
        ASSERTION(S_Instance != NULL);
    }
    return(S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This operation is called whenever a mode is entered where the alarm
//      reset is active.  The key itself is always active, but it only
//      affects ventilation in modes that can be manually reset.  This operation
//      sets the IsArmed attribute to FALSE, indicating that no request has
//      been made and invokes the base class Trigger::EnableTrigger() 
//      operation.
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
void PtInhTrigger::EnableTrigger(void)
{
    IsArmed = eFalse;
    Trigger::EnableTrigger();

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD cycle while this trigger is enabled.
//
//      This operation checks the IsArmed attribute to determine if the operator
//      has made an Alarm Reset Request.  If the operator has made a request
//      and the ventilator is in the unrestricted phase of exhalation or HFO, 
//      TriggerDidFire is set to True.
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
E_Bool PtInhTrigger::CheckCondition(void)
{

    if((eTrue == IsArmed) &&(ExhPhase::S_IsInUnrestrictedPhase() == eTrue))
    {
        TriggerDidFire = eTrue;
    }

    return(TriggerDidFire);

}   // end CheckCondition()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing: 
//      This operation is invoked by the ModeChangeTrigger::CheckCondition()
//      operation when the mode change trigger is armed but is waiting
//      for the unrestricted phase of exhalation.   PreCheckCondition() looks
//      ahead to determine if the patient trigger is going to fire on the
//      current BD cycle.  The ModeChangeTrigger uses this information to
//      determine whether the mode change should be phased in early.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger condition is met
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
E_Bool PtInhTrigger::PreCheckCondition(void)
{
    E_Bool rtnValue = eFalse;       // return value; indicates if trigger fired

    if (eTrue == IsEnabled)
    {

        // See if the ventilator is in the unrestricted phase of exhalation
        // By not passing any parameters, the default minimum exhalation 
        // time is used.
        if (ExhPhase::S_IsInUnrestrictedPhase() == eTrue)
        {

            // If the trigger is armed, the trigger is ready to fire.
            if (eTrue == IsArmed)
            {
                rtnValue = eTrue;
            }
        }
    }

    return(rtnValue);

}   // end PreCheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtInhTrigger()
//
//    Processing: 
//      This operation is the constructor for the Alarm Reset Operator Request
//      Trigger.  This operation invokes the base class Trigger constructor
//      with the TriggerId and the base class OpRequest() constructors.  And,
//      it initializes the IsArmed attribute to FALSE since the trigger cannot
//      be armed at start-up.
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
PtInhTrigger::PtInhTrigger(void) : Trigger(eNoTrigger), IsArmed(eFalse)
//    : Trigger(ePtInhTrigger), IsArmed(eFalse)
{
}   // end PtInhTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReceiveEvent()
//
//    Processing: Recieve Patient Inh event from Rtai
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      IsArmed
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
void PtInhTrigger::ReceiveEvent(void)
{
    IsArmed = eTrue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PtInhTrigger()
//
//    Processing: Get arm
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      IsArmed
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
E_Bool PtInhTrigger::GetIsArmed(void)
{
    return IsArmed;
}
