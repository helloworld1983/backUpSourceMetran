/******************************************************************************/
//$COMMON.H$
//   File Name:  AzScheduler.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the Auto-zero Scheduler 
//      class.  The AzScheduler class is derived from the Nucleus Plus 
//      RTOS class Timer.
/******************************************************************************/
#ifndef AZSCHEDULER_H
#define AZSCHEDULER_H

#include "Standard.h"
#include "Timer.h"

#ifdef AZSCHEDULER_CPP
#define EXTERN 
#else
#define EXTERN extern 
#endif

typedef void (*NotifyPtr) (void);

class AzScheduler : public Timer
{
    public:
        static void S_Init(void);
        void RestartTiming(void);
        virtual void ExpirationRoutine(void);
        void Suspend(void);
        void Resume(void);

    protected:


    private:
        AzScheduler(NotifyPtr ptr);
        AzScheduler(void);
        AzScheduler(const AzScheduler&);

        //$COMMON.ATTRIBUTE$
        //  Name: ScheduleTimeIx
        //  Description:  Index into the ScheduleTime array; used to determine
        //      when the next auto-zero is performed.
        //  Units: none
        //  Range: 0 - (NUM_SCHEDULE_TIMES - 1)
        SHORT ScheduleTimeIx;
        
        //$COMMON.ATTRIBUTE$
        //  Name: NotifyTimeForAz
        //  Description:  Pointer to function invoked when it is time to perform
        //      an auto-zero.
        //  Units: none
        //  Range: n/a
        NotifyPtr NotifyTimeForAz;

        //$COMMON.ATTRIBUTE$
        //  Name: RemainingTime
        //  Description:  Time Remaining in ticks until the Scheduling Timer
        //      expires.  This value is only updated when the scheduler is
        //      suspended.
        //  Units: TICKS
        //  Range: n/a
        UNSIGNED RemainingTime;

        //$COMMON.ATTRIBUTE$
        //  Name: StartTime
        //  Description:  Snapshot of the SystemClock when the timer was last
        //      started.
        //  Units: TICKS
        //  Range: n/a
        UNSIGNED StartTime; 

        //$COMMON.ATTRIBUTE$
        //  Name: IsSuspended
        //  Description:  Indicates if the timer is suspended.
        //  Units: None
        //  Range: n/a
        E_Bool IsSuspended;
        
};      

EXTERN AzScheduler* InhAzSchedulerPtr;
EXTERN AzScheduler* ExhAzSchedulerPtr;

#undef EXTERN

#endif  // AZSCHEDULER_H
