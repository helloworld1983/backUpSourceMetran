#include "HardwareErrorTrigger.h"
#include "HardwareErrorMode.h"
#include "valvestuck.h"

#include "DebugTrace.h"

#define TWO_SECOND 2000

HardwareErrorTrigger* HardwareErrorTrigger::S_Instance = NULL;


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the Hip2AlarmTrigger object, it invokes
//      Hip2AlarmTrigger::S_GetInstance(), which returns a pointer
//      to the Hip2AlarmTrigger object.
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
//      Hip2AlarmTrigger* - pointer to the instance of this class
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
HardwareErrorTrigger* HardwareErrorTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HardwareErrorTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

HardwareErrorTrigger::HardwareErrorTrigger()
    : Trigger(eHardwareErrorAlarmTriger)
{
    two_second = 0;
}

E_Bool HardwareErrorTrigger::CheckCondition(void)
{

//    TriggerDidFire =(E_Bool)(HardwareChecking::S_GetInstance()->checkHFODoor()
//                             | HardwareChecking::S_GetInstance()->CheckHFOPiston()
//                             | HardwareChecking::S_GetInstance()->CheckFan(eFanError)
//
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(AirValve,eAirMotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(O2Valve,eO2MotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(ExhValve,eExhMotorError,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorError(HfoValve,eHFOMotorError,eEventLog)
//
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(AirValve,eAirMotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(O2Valve,eO2MotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(ExhValve,eExhMotorLostCommunication,eEventLog)
//                             | HardwareChecking::S_GetInstance()->CheckMotorCommunication(HfoValve,eHFOMotorLostCommunication,eEventLog)
//
//                             | ValveStuck::S_GetInstance()->CheckO2ValveStuckOpen()
//                             | ValveStuck::S_GetInstance()->CheckAirValveStuckOpen()
//                             | ValveStuck::S_GetInstance()->CheckO2ValveStuckClosed()
//                             | ValveStuck::S_GetInstance()->CheckAirValveStuckClosed()
//
//                             | HardwareChecking::S_GetInstance()->CheckFlowSensor()
//
//
//                             );
    two_second = two_second + 2;
    if(two_second >= TWO_SECOND)
    {
        two_second = 0;
//        TriggerDidFire =(E_Bool)(TriggerDidFire | HardwareChecking::S_GetInstance()->CheckDACLoopback());
    }

    return TriggerDidFire;

}



