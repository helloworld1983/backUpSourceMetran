/******************************************************************************/
//$COMMON.CPP$
//   File Name:  DisconnectionAlarmTrigger.cpp
//   Purpose: 
//      This file contains the definition of the Disconected Alarm Trigger.
//      The Disconected Alarm Trigger is derived from the Trigger.It is used
//		to detect the ET Disconected condition during NormBreathing mode.
//
//   Interfaces:
//
//   Restrictions:
//      None
//
//   Revision History:
//
/******************************************************************************/
#include <InhPhase.h>
#include "DisconnectionAlarmTrigger.h"
#include "tasks.h"
#include "MgrBreathData.h"
#include "HipAlarmTrigger.h"
#include "PressureSensor.h"
#include "DebugTrace.h"

DisconnectionAlarmTrigger* DisconnectionAlarmTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the DisconnectionAlarmTrigger object, it invokes
//      DisconnectionAlarmTrigger::S_GetInstance(), which returns a pointer
//      to the DisconnectionAlarmTrigger object.
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
//      DisconnectionAlarmTrigger* - pointer to the instance of this class
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
DisconnectionAlarmTrigger* DisconnectionAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new DisconnectionAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end DisconnectionAlarmTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisconnectionAlarmTrigger()
//
//    Processing: 
//      This operation is the constructor for the DisconnectionAlarmTrigger.
//      This operation invokes the Trigger base class constructor.
//
//    Input Parameters:
//      triggerId - Trigger ID 
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
DisconnectionAlarmTrigger::DisconnectionAlarmTrigger(void)
: Trigger(eDisconnectionAlarmTrigger)
{
    InactiveTimer = 0;
    LowInhPressLimit = 0;
}   // end DisconnectionAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time while the trigger is 
//      enabled.  If the trigger has not already fired ,check air pressure switch
//      and O2 pressure switch in turn.
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
E_Bool DisconnectionAlarmTrigger::CheckCondition(void)
{

    if(InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }

    if(InhPhase::S_IsInUnrestrictedPhase(100) == eTrue)
    {
        //Check Inh Volume > 100*SCALE ml
        if(MgrBreathData::S_GetInstance()->GetPdBreathDataPtr()->GetInhVolume() > 10000)
        {
            if((MgrBreathData::S_GetInstance()->GetMonitoredPressure() < LowInhPressLimit)&&
                    (PressureSensor::ExhPressSensor->GetLastReading() < 2000)) //20 cmH2O
            {
                TriggerDidFire = eTrue;
            }
        }
    }

    return (TriggerDidFire);
}   // end CheckCondition()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      
//    Input Parameters:
//      value - 
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
void DisconnectionAlarmTrigger::PhaseInSettingChange(LONG value)
{
    InactiveTimer = 50;
    LowInhPressLimit = value/3;

    if(LowInhPressLimit < 100)
    {
        LowInhPressLimit = 100;
    }

    HipAlarmTrigger::S_GetInstance()->SetInactiveTimer(100);

}// end PhaseInSettingChange()

