/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHigh_PHigh_Trigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHigh_PHigh_Trigger
/******************************************************************************/
#include <DeviceInterface.h>
#include "ExtremeHigh_PHigh_Trigger.h"
#include "MgrBdSetting.h"
#include "AlarmTask.h"

ExtremeHigh_PHigh_Trigger* ExtremeHigh_PHigh_Trigger::S_Instance = NULL;

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
ExtremeHigh_PHigh_Trigger* ExtremeHigh_PHigh_Trigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ExtremeHigh_PHigh_Trigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExtremeHigh_PHigh_Trigger()
//
//    Processing:
//      This operation is the constructor for the ExtremeHigh_PHigh_Trigger.  It invokes
//      the base class constructor.
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
//      None
//
/******************************************************************************/
ExtremeHigh_PHigh_Trigger::ExtremeHigh_PHigh_Trigger(void)
: Trigger(eExtremeHigh_PHigh_Trigger)
{
    HighPressAlarm = 0;
    InactiveTimer = 0;

    ExtremeHighPEEP_Offset = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: Enable trigger
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
//      None
//
/******************************************************************************/
void ExtremeHigh_PHigh_Trigger::EnableTrigger(void)
{
    ExtremeHighPEEP_Offset =  MgrBdSetting::S_GetInstance()->GetCurrentValue(eExtremeHighPEEP_Autoset);
    Trigger::EnableTrigger();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: Check condition and process action
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
//      None
//
/******************************************************************************/
E_Bool ExtremeHigh_PHigh_Trigger::CheckCondition(void)
{

    LONG PressureLimit = MgrBdSetting::S_GetInstance()->GetCurrentValue(eAPRVHighPHighLimit) + ExtremeHighPEEP_Offset;

    LONG InhPressSensor;
    LONG ExhPressSensor;

    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &InhPressSensor);
    DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &ExhPressSensor);


    if ((InhPressSensor > PressureLimit)||
            (ExhPressSensor > PressureLimit))

    {

        if(eNotActive == AlarmTask::S_GetInstance()->GetIsActive(eExtremeHigh_PHigh))
        {
            AlarmTask::S_GetInstance()->SetIsActive(eExtremeHigh_PHigh, eActive);
        }

        TriggerDidFire = eTrue;
    }
    else
    {
        if(eActive == AlarmTask::S_GetInstance()->GetIsActive(eExtremeHigh_PHigh))
        {
            AlarmTask::S_GetInstance()->SetIsActive(eExtremeHigh_PHigh, eNotActive);
        }
    }
    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetInactiveTimer(USHORT value)
//
//    Processing: set inactive timer value
//
//    Input Parameters:
//      USHORT value - new value of Timer
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
//      None
//
/******************************************************************************/
void ExtremeHigh_PHigh_Trigger::SetInactiveTimer(USHORT value)
{
    InactiveTimer = value;
}
