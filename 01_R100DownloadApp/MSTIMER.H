//******************************************************************************
//$COMMON.H$
//   File Name:  MSTIMER.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: The MsTimer class is used to periodically schedule a task.
//
//          An MsTimer object is created with a desired scheduling interval
//          in ticks. The task invokes one of the public MsTimer::Suspend
//          methods to sleep. When the time interval expires, the task is
//          re-scheduled for execution.
//
//          The MsTimer class inherits from the Timer class and 
//          has an Event object attribute. These are Nucleus C++ classes.
//          
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      02    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      01    02/12/97  Dan Seibert         Morph
//      Description: Initial Revision
//
//******************************************************************************
#ifndef    MSTIMER_H
#define	   MSTIMER_H

#include "nucpp.h"
#include "commontypes.h"

//
//  MsTimer class definition
//
class MsTimer : public Timer
{
    public:                                 
        // Constructor
        MsTimer( char * name, ULONG ticks);

        // Timer expiration routine
        void ExpirationRoutine();

        // Public interfaces
        inline STATUS SuspendAndConsume();
        inline STATUS SuspendWithTimeout(ULONG timeout);
	
    private:
        //$COMMON.ATTRIBUTE$
        //    Name:         TimerEvent
        //    Description:  This is the event flag 
        //        that is set when the timer expires.
        //    Units: None
        //    Range: N/A
        Event * TimerEvent;

        MsTimer();  // not to be called
};


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SuspendAndConsume()
//
//    Processing: This routine suspends a task until the timer expires.
//          The Nucleus method Event::Retrieve() is invoked to do the work.
//
//          When the Timer expires, it sets the event flag which satisfies
//          the Retrieve() request and the task resumes execution.
//
//    Input Parameters: None
//    Output Parameters: None
//    Return Values: STATUS - Nucleus return code.
//
//    Pre-Conditions: Timer must be enabled or task will hang indefinitely.
//
//    Miscellaneous: None
//    Requirements:  
//
//*****************************************************************************
inline STATUS MsTimer::SuspendAndConsume()
{
    // Retrieve Event Flag  (Consume Flag, Suspend Task until flag is set)     
    return (TimerEvent->Retrieve( eTrue ));
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SuspendWithTimeout()
//
//    Processing: This routine suspends a task until either the timer expires
//          or the number of ticks specified expires.
//          The Nucleus method Event::Retrieve() is invoked to do the work.
//
//          When the Timer expires, it sets the event flag which satisfies
//          the Retrieve() request and the task resumes execution.
//          If the timeout period given expires before the timer object, the
//          routine returns with an error status code.
//
//    Input Parameters: None
//    Output Parameters: None
//    Return Values: STATUS - Nucleus error code.
//
//    Pre-Conditions: Timer must be enabled or task will hang indefinitely.
//
//    Miscellaneous: None
//    Requirements:  
//
//*****************************************************************************
inline STATUS MsTimer::SuspendWithTimeout(ULONG ticks)
{
    // Retrieve Event Flag  (Consume Flag, Suspend Task until flag is set)     
    return (TimerEvent->Retrieve( eTrue, ticks ));
}
	

#endif	//MS_TMR_H
