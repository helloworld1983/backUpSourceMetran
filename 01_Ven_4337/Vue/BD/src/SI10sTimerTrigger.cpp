/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SI10sTimerTrigger.cpp
//   Purpose:
//
//   Interfaces:
//
//   Restrictions:
//      None
//
//   Revision History:
//
//
/******************************************************************************/
#include "SI10sTimerTrigger.h"
#include "AlarmTask.h"

const UNSIGNED SI_TIME_IN_TICKS = 30000/MS_PER_TICK; // 30 sec

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIOffTimeTrigger()
//
//    Processing: Constructor for class
//
//    Input Parameters:
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Requirements:
//
/******************************************************************************/
SI10sTimerTrigger::SI10sTimerTrigger(void)
    : TimeTrigger(eSI10sTimeTrigger,(CHAR*) "SITENSEC")
{
//    SIMsg.Value = 0;

    one_hundred_ms = 0;
    currentSITime = 0;
    // Store the Time Period in System Clock Ticks.
    SetTimePeriod(SI_TIME_IN_TICKS);

}   // end SI10sTimerTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIOffTimeTrigger()
//
//    Processing: Constructor for class
//
//    Input Parameters:
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Requirements:
//
/******************************************************************************/
void SI10sTimerTrigger::EnableTrigger(void)
{
    one_hundred_ms = 0;
    currentSITime = 0;
    this->Start();
    TimeTrigger::EnableTrigger();
}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//      Check if the trigger fired or not
//      and Update Current SI Time in Patient Data Monitor
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_Bool SI10sTimerTrigger::CheckCondition(void)
{
    UpdateSITime();
    return(TriggerDidFire);
}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateSITime()
//
//    Processing:
//      Update Current SI Time in Patient Data Monitor
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void SI10sTimerTrigger::UpdateSITime()
{
    one_hundred_ms = one_hundred_ms + 2;
    if(one_hundred_ms >= 100)
    {
        one_hundred_ms = 0;
        currentSITime = currentSITime + 10;
//        SIMsg.Value = currentSITime;
//        MainHandlerDelegate::S_GetInstance()->postCustomEvent<PatientDataGUIEvent>(eManInspHoldTimePtDataID, SIMsg);
    }
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: DisableTrigger()
//
//    Processing:
//      DisableTrigger
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//     None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
void SI10sTimerTrigger::DisableTrigger(void)
{
    one_hundred_ms = 0;
    currentSITime = 0;
//    Disable();
    this->Stop();
    Trigger::DisableTrigger();

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AutoSIOffTimeTrigger()
//
//    Processing: Constructor for class
//
//    Input Parameters:
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Requirements:
//
/******************************************************************************/
void SI10sTimerTrigger::ProcessTrigger(Mode* mode)
{
    mode->ProcessTriggerID(eSI10sTimeTrigger);
}
