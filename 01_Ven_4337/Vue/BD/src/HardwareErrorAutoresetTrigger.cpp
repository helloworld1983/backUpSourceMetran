/******************************************************************************/
//$COMMON.H$
//   File Name:  HardwareErrorAutoresetTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the HardwareErrorAutoresetTrigger
/******************************************************************************/
#include "HardwareErrorAutoresetTrigger.h"
#include "valvestuck.h"

#include "DebugTrace.h"

#define TWO_SECOND 2000
HardwareErrorAutoresetTrigger* HardwareErrorAutoresetTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the HighMAP_2AutoResetTrigger object, it invokes
//      HardwareErrorAutoresetTrigger::S_GetInstance(), which returns a pointer
//      to the HardwareErrorAutoresetTrigger object.
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
//      HardwareErrorAutoresetTrigger* - pointer to the instance of this class
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
HardwareErrorAutoresetTrigger* HardwareErrorAutoresetTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorAutoresetTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end HardwareErrorAutoresetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HardwareErrorAutoresetTrigger()
//
//    Processing:
//      This operation is the constructor for the Patient Exhalation Pressure
//      Trigger.  It invokes the base class Trigger constructor with the
//      Trigger ID.
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
HardwareErrorAutoresetTrigger::HardwareErrorAutoresetTrigger(void)
    : Trigger(eHardwareErrorAutoresetTrigger)
{
    TimeCounter = 0;
    IsArmed = eFalse;
    two_second = 0;
}   // end HardwareErrorAutoresetTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      This operation is invoked every BD task cycle when the vent is in Hardware Error Mode
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
E_Bool HardwareErrorAutoresetTrigger::CheckCondition(void)
{

//    TriggerDidFire =(E_Bool)(!(HardwareChecking::S_GetInstance()->checkHFODoor()
//                               | HardwareChecking::S_GetInstance()->CheckHFOPiston()
//
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(O2Valve,eO2MotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(AirValve,eAirMotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(ExhValve,eExhMotorError,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorError(HfoValve,eHFOMotorError,eNoLogEvent)
//
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(O2Valve,eO2MotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(AirValve,eAirMotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(ExhValve,eExhMotorLostCommunication,eNoLogEvent)
//                               | HardwareChecking::S_GetInstance()->CheckMotorCommunication(HfoValve,eHFOMotorLostCommunication,eNoLogEvent)
//
//                               | ValveStuck::S_GetInstance()->CheckO2ValveStuckOpen()
//                               | ValveStuck::S_GetInstance()->CheckAirValveStuckOpen()
//                               | ValveStuck::S_GetInstance()->CheckO2ValveStuckClosed()
//                               | ValveStuck::S_GetInstance()->CheckAirValveStuckClosed()
//                               | HardwareChecking::S_GetInstance()->CheckFlowSensor()
//                               ));
    two_second = two_second + 2;
    if(two_second >= TWO_SECOND)
    {
        two_second = 0;
//        TriggerDidFire =(E_Bool)(TriggerDidFire & (!HardwareChecking::S_GetInstance()->CheckDACLoopback()));
    }

    return(TriggerDidFire);

}   // end CheckCondition()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing:
//      This method is invoked when the HighMAP_2AutoResetTrigger is enabled.
//      This operation sets the IsConditionMet attribute to FALSE and
//      invokes the base class Trigger::EnableTrigger().
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
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void HardwareErrorAutoresetTrigger::EnableTrigger(void)
{
    IsArmed = eFalse;
    TimeCounter = 0;
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()


