/******************************************************************************/
//$COMMON.CPP$
//   File Name:  AzScheduler.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:   The AzScheduler class is responsible for scheduling
//      the Pressure Transducer Auto-zero CBIT tests.
//
//   Interfaces:
//      Nucleus Timer class
//
//   Restrictions:
//      None
/******************************************************************************/
#define AZSCHEDULER_CPP
#include "AzScheduler.h"
#undef AZSCHEDULER_CPP
#include "CommonConstants.h"
#include "ExhPhase.h"

// Number of different scheduling times for auto-zero tests.
static const SHORT NUM_SCHEDULE_TIMES = 5;

// The following array defines the frequency of the Pressure Transducer auto-
// zeroing.
static const UNSIGNED ScheduleTime[NUM_SCHEDULE_TIMES] =
{
    TICKS_PER_MIN,              // 1 minute after start-up
    5 * TICKS_PER_MIN,          // 5 minutes after previous
    10 * TICKS_PER_MIN,         // 10 minutes after previous
    44 * TICKS_PER_MIN,         // 1 hour after start-up
    60 * TICKS_PER_MIN          // 1 hour after previous
};


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AzScheduler()
//
//    Processing: 
//      This operation is the constructor for objects of the AzScheduler class.
//      This operation invokes the constructors for the Timer class from which
//      this class is derived.  
//
//    Input Parameters:
//      timerName - name of the timer
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
AzScheduler::AzScheduler(NotifyPtr ptr) : Timer("AZ_SCHED", 1, pdTRUE)  //Add to build
//    : Timer("AZ_SCHED", 0, 0, FALSE),
//      NotifyTimeForAz(ptr),
//      IsSuspended(eFalse)
{
    
}   // end AzScheduler()



void AzScheduler::S_Init(void)
{
    // Instantiate the schedulers.
//    InhAzSchedulerPtr = new AzScheduler(ExhPhase::S_SetIsTimeForInhAz);

//    ExhAzSchedulerPtr = new AzScheduler(ExhPhase::S_SetIsTimeForExhAz);

}   // end S_Init()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RestartTiming()
//
//    Processing: 
//      This operation is invoked at start-up to initiate the first timing
//      interval.  It is also invoked whenever the auto-zero timing is 
//      restarted.
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
void AzScheduler::RestartTiming(void)
{
    STATUS status;
    
    // First disable the timer in case it is running.
//    Disable();

    // Reset the index    
    ScheduleTimeIx = 0;

    // Snapshot the system clock
//    StartTime = SystemClock::Retrieve();
    
    // Reset the timer
//    status = Reset(ScheduleTime[ScheduleTimeIx], 0, TRUE);

    // It is an error if the return status is not successful.    
//    ASSERTION(status == NU_SUCCESS);
  
    
}   // end RestartTiming()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing: 
//      This operation is the definition of the ExpirationRoutine() declared
//      as a pure virtual method in the RTOS Timer class.  This operation
//      is invoked by the RTOS whenever the associated timer expires.
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
//      Since this operation is invoked via the OS, it cannot make any 
//      function calls that could potentially cause suspension.
//
//    Requirements:  
//
/******************************************************************************/
void AzScheduler::ExpirationRoutine(void)
{
    STATUS status;

    // It is time for an auto-zero
    NotifyTimeForAz();

    // Update index into the ScheduleTime array.        
    if (ScheduleTimeIx < (NUM_SCHEDULE_TIMES-1))
    {
        ScheduleTimeIx++;
    }
    
    // Snapshot the system clock
//    StartTime = SystemClock::Retrieve();

    // Reset the timer as a one-shot timer for the next schedule time
//    status = Reset(ScheduleTime[ScheduleTimeIx], 0, TRUE);

    // It is an error if the return status is not successful.    
//    ASSERTION(status == NU_SUCCESS);

}   // end ExpirationRoutine()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Suspend()
//
//    Processing: 
//      This operation is invoked when the ventilator enters a non-breathing
//      mode.  It disables the auto-zero scheduling timer.  It hen determines
//      the time remaining before it would expire and it updates the IsSuspended
//      attribute to TRUE.
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
//      Timer cannot already be suspended; otherwise error handling is performed.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void AzScheduler::Suspend(void)
{
    STATUS status;      // RTOS return status

    // Disable the timer.   
//    status = Disable();
//    ASSERTION(NU_SUCCESS == status);

    // Calculate the time remaining before the timer expires.  This is done
    // at the time of suspension rather than resumption to effectively treat
    // the non-ventilation period as if it had not happened.  Typically, the
    // vent will not remain in the non-ventilation period for a long time
    // and if an auto-zero was scheduled but not performed, it would be 
    // performed on exit from the Non-Breathing mode and it is preferable not
    // to schedule another one right away.
    RemainingTime = ScheduleTime[ScheduleTimeIx] - CalcElapsedTime(StartTime);

    // Update the IsSuspended attribute.    
//    ASSERTION(eFalse == IsSuspended);
    IsSuspended = eTrue;

}   // end Suspend()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Resume()
//
//    Processing: 
//      This operation is invoked on exit from a Non-Ventilation mode to 
//      resume the auto-zero schedule timer. 
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
//      IsSuspended must be TRUE.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void AzScheduler::Resume(void)
{
    STATUS status;      // RTOS return status
    
    // Minimum time used to start a timer.  
    const UNSIGNED MIN_TIMER_START = 10;        

    // Resume should only be called if the timer was already suspended.        
//    ASSERTION(eTrue == IsSuspended);
    IsSuspended = eFalse;
    
    if (RemainingTime <= MIN_TIMER_START)
    {
        RemainingTime = MIN_TIMER_START;
    }

    // Reset the timer as a one-shot timer for the remaining time
//    status = Reset(RemainingTime, 0, TRUE);

    // It is an error if the return status is not successful.    
//    ASSERTION(status == NU_SUCCESS);
    
}   // Resume()
