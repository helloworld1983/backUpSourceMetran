/******************************************************************************/
//$COMMON.CPP$
//   File Name:  HighMAP_2AlarmTrigger.cpp
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
#include "HighMAP_2AlarmTrigger.h"
#include "HFOHighMAP_2Mode.h"
#include "Hfo_cnt.h"
#include "MgrBdSetting.h"
#include "mainhandlerdelegate.h"
HighMAP_2AlarmTrigger* HighMAP_2AlarmTrigger::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: HighMAP_2AlarmTrigger()
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
HighMAP_2AlarmTrigger::HighMAP_2AlarmTrigger(void)
    : Trigger(eHighMAP_2AlarmTrigger)
{
    
}   // end HighMAP_2AlarmTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation retrieves the current pressure measured by calling 
//        HfoController::GetFilterPressIntegrator() and it retrieves
//        the operator setting for High MAP_2 Limit by calling HfoController::GetHighMAP_2Limit()
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
E_Bool HighMAP_2AlarmTrigger::CheckCondition(void)
{
    static USHORT highSICounter = 0;
    static USHORT highMapCounter = 0;
    LONG level2PressLimit,level2_SIPressLimit;
    LONG map;
    LONG exhmap;
    MgrBdSetting* mgrBdSettingPtr = MgrBdSetting::S_GetInstance();
    if ( InactiveTimer > 0)
    {
        InactiveTimer--;
        return (eFalse);
    }


    map = HfoController->GetFilterPressIntegrator();
    exhmap = HfoController->GetExhMap();

    level2_SIPressLimit = MgrBdSetting::S_GetInstance()->GetPendingValue(eSI) +
            mgrBdSettingPtr->GetPendingValue(eExtremeHighMap_Autoset);

    level2PressLimit = MgrBdSetting::S_GetInstance()->GetPendingValue(eHFOMap) +
            mgrBdSettingPtr->GetPendingValue(eExtremeHighMap_Autoset);
    
    if((HfoController->GetSIRequest()== eTrue)||(HfoController->GetHIMP()== eTrue))
    {
        // Check if the current pressure is greater than the operator set High SI limit
        if ((map > level2_SIPressLimit)||(exhmap > level2_SIPressLimit))
        {

            highSICounter++;
            if ( highSICounter > 5) // 5 * 2ms servo = 10ms
            {// If the pressure condition is met, set the TriggerDidFire attribute

                TriggerDidFire = eTrue;
            }
        }
        else
        {
            highSICounter = 0;
        }
    }
    else
        ////////////////////////////////////////////////////////////////////////////////////////////
    {
        // Check if the current pressure is greater than the operator set High MAP_2 setting
        if ((map > level2PressLimit)||(exhmap > level2PressLimit))
        {
            highMapCounter++;
            if ( highMapCounter > 5)
            {    // If the pressure condition is met, set the TriggerDidFire attribute
                TriggerDidFire = eTrue;
            }
        }
        else
        {
            highMapCounter = 0;
        }

    }

    return (TriggerDidFire);
    
}   // end CheckCondition()
#endif // HFO_SYSTEM


