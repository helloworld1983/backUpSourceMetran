/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HighMAP_2AutoResetTrigger.cpp
//   Purpose: 
//      This file contains the definition of the High MAP_2 Alarm Auto-Reset
//      Trigger, which is derived from the base Trigger class.  
//
//      This trigger is enabled when the ventilator enters the HFOHighMAP_2 mode.
//      The trigger fires when ...
//   Interfaces:
//      The CheckCondition() operation interfaces with 
//      CheckCondition() also calls SystemClock::Retrieve() to snapshot the
//      current system time.
//
//   Restrictions:
//      None

#include <DeviceInterface.h>
#include "HighMAP_2AutoResetTrigger.h"
#include "DebugTrace.h"

// Define statics
HighMAP_2AutoResetTrigger* HighMAP_2AutoResetTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the HighMAP_2AutoResetTrigger object, it invokes
//      HighMAP_2AutoResetTrigger::S_GetInstance(), which returns a pointer
//      to the HighMAP_2AutoResetTrigger object.
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
//      HighMAP_2AutoResetTrigger* - pointer to the instance of this class
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
HighMAP_2AutoResetTrigger* HighMAP_2AutoResetTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HighMAP_2AutoResetTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end HighMAP_2AutoResetTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HighMAP_2AutoResetTrigger()
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
HighMAP_2AutoResetTrigger::HighMAP_2AutoResetTrigger(void): Trigger(eHighMAP_2AutoResetTrigger)
{
    TimeCounter = 0;
}   // end HighMAP_2AutoResetTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the ventilator is
//      in the HFOHighMAP_2 mode. 
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
E_Bool HighMAP_2AutoResetTrigger::CheckCondition(void)
{
    const UNSIGNED ONE_SEC = 1000;
    TimeCounter++;

    if ( TimeCounter >= ONE_SEC ) // 1 second.
    {
        E_State ValveState = eClose;
        DeviceInterface::S_GetInstance()->RequestDevice(eSafetyValveSetDesiredState, &ValveState);
    }

    LONG InhPressSensor;
    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhPressSensor);


    if (InhPressSensor <= (4 * SCALE) )
    {
        if ( TimeCounter >= 1500)
        {
            TriggerDidFire = eTrue;
        }
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
void HighMAP_2AutoResetTrigger::EnableTrigger(void)
{

    TimeCounter = 0;
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
