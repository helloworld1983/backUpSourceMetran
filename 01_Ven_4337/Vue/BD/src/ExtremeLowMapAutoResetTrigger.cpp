/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ExtremeLowMapAutoResetTrigger.cpp
//   Purpose: 
//      This file contains the definition of the ExtremeLowMapAutoResetTrigger
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
//
//   Revision History:

/******************************************************************************/
#ifdef HFO_SYSTEM
#include "ExtremeLowMapAutoResetTrigger.h"
#include "Hfo_cnt.h"
#include "Stepmoto.h"
#include "sensor.h"
#include "HfoServo.h"
// Define statics
ExtremeLowMapAutoResetTrigger* ExtremeLowMapAutoResetTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExtremeLowMapAutoResetTrigger()
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
ExtremeLowMapAutoResetTrigger::ExtremeLowMapAutoResetTrigger(void): 
    Trigger(eExtremeLowMapAutoResetTrigger)
{
    OneSecTimeCounter = 0;
    TimeCounter = 0;
    IsArmed = eFalse;

}   // end ExtremeLowMapAutoResetTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the ventilator is
//      in the ExtremeLowMap mode. 
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
E_Bool ExtremeLowMapAutoResetTrigger::CheckCondition(void)
{
    HfoValve->RampToPosition(0);

    if(OneSecTimeCounter < 1000)
    {
        OneSecTimeCounter +=2;
        return eFalse;
    }

    // If pressure come back above  2* SCALE for 20ms.

    if ( (LONG) InhPressSensor->GetLastReading() > (2 * SCALE) )
    {
        // Since the condition has been met before, see how long the
        // condition has been met.  If this value exceeds the minimum
        // time criteria, set the IsArmed attribute.
        TimeCounter = TimeCounter + 2;
        if ( TimeCounter >= 20 ) // 250 ms.
        {
            IsArmed = eTrue;
            TimeCounter = 0;
            TriggerDidFire = eTrue;
            HfoServo::S_GetInstance()->Disable();
            HfoController->Disable();
        }
    }
    else
    {
        TimeCounter = 0;
    }

    if(TriggerDidFire)
    {
        //wait until Exh motor ramp to 0, then trigger
        if(ExhValve->GetStepPosition()!=(USHORT)0)
        {
            ExhValve->RampToPosition(0);
            return eFalse;
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
void ExtremeLowMapAutoResetTrigger::EnableTrigger(void)
{
    IsArmed = eFalse;
    TimeCounter = 0;
    OneSecTimeCounter = 0;
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();

}   // end EnableTrigger()
#endif // HFO_SYSTEM

