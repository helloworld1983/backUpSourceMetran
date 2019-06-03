//$COMMON.CPP$
//   File Name:  Timer.cpp
//
//   Purpose: This file contains the class declaration for the Timer Class
//                it is based class for all timer class use later
//
/******************************************************************************/
#include "Timer.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Timer
//
//    Processing: constructor for Timer class
//
//    Input Parameters: const char* const pcTimerName - Name of Timer
//                      const TickType_t xTimerPeriodInTicks - The timer period in ticks (must be greater than 0)
//                      const UBaseType_t uxAutoReload - The timers will auto-reload themselves when the expire(pdTRUE)
//                      void* const pvTimerID - The ID is used to store a count of the number of times the timer has expired
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
Timer::Timer (const char* const pcTimerName,
              const TickType_t xTimerPeriodInTicks,
              const UBaseType_t uxAutoReload)
{
    //Create timer using RTOS function
    handle = xTimerCreate (pcTimerName, //Name of Timer
                           xTimerPeriodInTicks,        //The timer period in ticks
                           uxAutoReload,               //The timers will auto-reload themselves when the expire
                           this,                       //The ID is used to store a count of the number of times the timer has expired
                           TimerCallbackFunctionAdapter); //Each timer calls the same callback when it expires
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start
//
//    Processing: Start Timer
//
//    Input Parameters: TickType_t CmdTimeout - Specifies the time, in ticks,
//            that the calling task should be held in the Blocked state
//     to wait for the start command to be successfully sent to the timer command queue
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool Timer::Start (TickType_t CmdTimeout)
{
    bool Status;
    //Start the timer
    if(pdFALSE == xTimerStart(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ~Timer
//
//    Processing: Destructors for EventGroup class - delete timer
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
Timer::~Timer ()
{
    xTimerDelete(handle, portMAX_DELAY);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: IsAcitve
//
//    Processing: Check timer is still active or not
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool Timer::IsAcitve ()
{
    bool Status;
    //Check the timer is active or not
    if(pdFALSE == xTimerIsTimerActive (handle))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Stop
//
//    Processing: Stop timer
//
//    Input Parameters: TickType_t CmdTimeout - Specifies the time, in ticks,
//            that the calling task should be held in the Blocked state
//     to wait for the start command to be successfully sent to the timer command queue
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool Timer::Stop (TickType_t CmdTimeout)
{
    bool Status;
    //Stop timer
    if(pdFALSE == xTimerStop(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Reset
//
//    Processing: Reset timer
//
//    Input Parameters: TickType_t CmdTimeout - Specifies the time, in ticks,
//            that the calling task should be held in the Blocked state
//     to wait for the start command to be successfully sent to the timer command queue
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool Timer::Reset (TickType_t CmdTimeout)
{
    bool Status;
    //Stop timer
    if(pdFALSE == xTimerReset(handle, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPeriod
//
//    Processing: Set new period for timer
//
//    Input Parameters: TickType_t CmdTimeout - Specifies the time, in ticks,
//            that the calling task should be held in the Blocked state
//     to wait for the start command to be successfully sent to the timer command queue
//                      TickType_t NewPeriod - new period for timer
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool Timer::SetPeriod (TickType_t NewPeriod, TickType_t CmdTimeout)
{
    bool Status;
    //Set new period for timer
    if(pdFALSE == xTimerChangePeriod(handle, NewPeriod, CmdTimeout))
    {
        Status = false;
    }
    else
    {
        Status = true;
    }

    return Status;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TimerCallbackFunctionAdapter
//
//    Processing: When timer expires, call back function is run
//
//    Input Parameters: TimerHandle_t xTimer
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void Timer::TimerCallbackFunctionAdapter (TimerHandle_t xTimer)
{
    //call ExpirationRoutine function
    static_cast<Timer *> (pvTimerGetTimerID (xTimer))->ExpirationRoutine ();
}
