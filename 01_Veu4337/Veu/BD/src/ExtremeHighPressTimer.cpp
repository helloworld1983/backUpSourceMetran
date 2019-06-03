/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHighPressTimer.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHighPressTimer
/******************************************************************************/
#include "ExtremeHighPressTimer.h"
#include "AlarmResetOpReqTrigger.h"

//#include "BdTask.h"

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
ExtremeHighPressTimer::ExtremeHighPressTimer(int  initialTime, bool enabled)
    :Timer("ExtremeHighPressTimer", initialTime, enabled)
{
    isExpired = false;
    isProcessAutoReset = false;
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
void ExtremeHighPressTimer::ExpirationRoutine()
{
    this->Stop();
    isExpired = true;
    if (isProcessAutoReset)
    {
        AlarmResetOpReqTrigger::S_GetInstance()->SetIsIgnoreAlarmReset(eTrue);
//        BdTask::S_GetInstance()->Send(eAlarmResetKey);
    }
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
void ExtremeHighPressTimer::initialize()
{
    this->Stop();
    isExpired = false;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: setTimerExpired(bool state)
//
//    Processing: Set timer expired
//
//    Input Parameters:
//      bool state - state of timer
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
void ExtremeHighPressTimer::setTimerExpired(bool state)
{
    isExpired = state;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: getTimerExpired()
//
//    Processing: Get timer expired
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      isExpired - timer is expired or not
//
//    Miscellaneous:
//      None
//
//    Requirements:
//      None
//
/******************************************************************************/
bool ExtremeHighPressTimer::getTimerExpired(void)
{
    return isExpired;
}
