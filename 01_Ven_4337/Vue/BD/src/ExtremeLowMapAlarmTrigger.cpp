/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ExtremeLowMapAlarmTrigger.cpp
//   Purpose: 
//      This file contains the definition of the High MAP_2 Alarm Trigger.
//      The High MAP_2 Alarm Trigger is derived from the Trigger.It is used
//        to detect the High MAP_2 condition during HFO mode.
//
//   Interfaces:
//      The CheckCondition() operation retrieves the current pressure measured 
//        by calling HfoController::GetFilterPressIntegrator() and it retrieves 
//        the operator setting for High MAP_2 Limit by calling 
//        MgrBdSetting::GetCurrentValue().
//   Restrictions:
//      None
//
//   Revision History:
//
//
/******************************************************************************/
#ifdef HFO_SYSTEM
#include "ExtremeLowMapAlarmTrigger.h"
#include "Hfo_cnt.h"
#include "AlarmStat.h"
#include "MgrBdSetting.h"
#include <QDebug>
ExtremeLowMapAlarmTrigger* ExtremeLowMapAlarmTrigger::S_Instance = NULL;
USHORT ExtremeLowMapAlarmTrigger::activeCounter = 0;
USHORT ExtremeLowMapAlarmTrigger::notActiveCounter = 0;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExtremeLowMapAlarmTrigger()
//
//    Processing: 
//      This operation is the constructor for the HighMAP_2AlarmTrigger.
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
ExtremeLowMapAlarmTrigger::ExtremeLowMapAlarmTrigger(void)
    : Trigger(eExtremeLowMapAlarmTrigger)  
{
    InactiveTimer = 350;
    MapSetting = 0;
    MapLimit = 2*SCALE;
}   // end ExtremeLowMapAlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      This operation retrieves the current Map measured by calling 
//        GetFilterPressIntegrator() .GetExhMap() is also called to get the Exh Map
//        measured with exhalation pressure sensor reading.
//    
//      The value of TriggerDidFire is always returned to the caller.
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
E_Bool ExtremeLowMapAlarmTrigger::CheckCondition(void)
{
    static USHORT lowMapCounter = 0;

    if ( InactiveTimer > 0)
    {
        // Check if the current pressure is less than 2 cmH2O and the Exhalation Map
        // is more than 80 cmH2O .That is in the case of ET disconnection...very dangerous
        if((HfoController->GetFilterPressIntegrator() <= 200)&&(HfoController->GetExhMap() > 8000))
        {
            TriggerDidFire = eTrue;
        }
        InactiveTimer--;
    }
    else
    {
        // Check if the current pressure is less than 2 cmH2O ( 2 * Scale) with Scale = 100
        MapSetting =  MgrBdSetting::S_GetInstance()->GetCurrentValue(eHFOMap);
        MapLimit = (2*MapSetting)/3 > 2*SCALE ? (2*MapSetting)/3 : 2*SCALE;

        if (HfoController->GetFilterPressIntegrator() < MapLimit)
        {

            lowMapCounter++;
            // If map is low for 200ms. That is 2 ms servo * 100 = 200ms
            if ( lowMapCounter >= 50 )
            {
                lowMapCounter = 0;
                // If the pressure condition is met, set the TriggerDidFire attribute
                HfoController->SetExhValveLimit(1100);
                TriggerDidFire = eTrue;

            }
        }
        else
        {
            lowMapCounter = 0;
        }
    }

    return (TriggerDidFire);

}   // end CheckCondition()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      
//      Finally, this operation invokes the base class Trigger::DisableTrigger()
//      operation to perform some processing common to disabling all triggers.
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
void ExtremeLowMapAlarmTrigger::EnableTrigger(void)
{
    // Invoke Base Class Disable Method
    Trigger::EnableTrigger();
}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This method is invoked whenever the RR is changed.  This operation
//      calculates a new breath period based upon the respiratory rate and
//      stores the value in the TimePeriod attribute.  If the trigger is
//      enabled, the private PhaseIn() operation is called to update the
//      timer and trigger status based upon the new respiratory rate.
//
//    Input Parameters:
//      newValue - new respiratory rate in BPM * SCALE
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
void ExtremeLowMapAlarmTrigger::PhaseInSettingChange(LONG map)
{
    InactiveTimer = 350;
    MapSetting = map;

    if(map > 1000)
    {
        MapLimit = map/2 ;
    }
    else
    {
        MapLimit = map/4;
    }
}   // end PhaseInSetting()


#endif // HFO_SYSTEM

