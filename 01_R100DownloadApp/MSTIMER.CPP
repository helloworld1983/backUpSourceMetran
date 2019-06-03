//******************************************************************************//
//$COMMON.CPP$
//   File Name:  MSTIMER.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: The file contains the routines for the MsTimer class.
//      A MsTimer object is a timer and an event flag. 
//      The timer sets the event flag at the interval specified.
//      This is used to synchronize task execution.
//
//   Interfaces: None
//
//   Restrictions: Can not have more than 32 mstimer objects
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      05    10/16/98  K.A. McGuigan       Morph
//      Description: SCR 359 - Updated comments.
//
//      Rev:  Date:     Engineer:           Project:
//      04    08/13/98  K.A. McGuigan       Morph
//      Description: SCR 283 - Moved the expiration routine back into this file.  
//          The compiler generated destructor was being placed in OTP.  
//          It made reference to the Timer destructor which was in text space, 
//          preventing the OTP from being independent.
//
//      Rev:  Date:     Engineer:           Project:
//      03    04/10/98  D. Seibert          Morph
//      Description: Moved the expiration routine into v851 on-chip ROM
//          as a performance enhancement for Morph.
//          (see MsTimerOTP.cpp)
//
//      Rev:  Date:     Engineer:           Project:
//      02    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      01    01/24/97  Dan Seibert         Morph
//      Description: Initial Revision
//
//******************************************************************************
#define MSTIMER_CPP
#include "mstimer.h"
#include "nucpp.h"


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: MsTimer()
//
//    Processing: This is the constructor for the MsTimer class.
//          The constructor creates a timer and an Event.
//          The timer sets the event at the interval specified by the
//          input parameter.
//
//    Input Parameters:
//          name: (char *) contains ASCII name of object. (for debug)
//          ticks: (SHORT) the timer interval in ticks.
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions: None
//
//    Miscellaneous:
//          Timers are set for auto rescheduling
//
//    Requirements:  
//
//******************************************************************************
MsTimer::MsTimer( char * name, ULONG ticks )        
    : Timer
        (
            name,       // Name of timer.
            ticks,	// Initial expiration time.
            ticks,	// Reschedule expiration time.
            FALSE       // Clock is initially disabled.
        )
    {
        // Create and Initialize the Event flag
        TimerEvent = new Event();
        TimerEvent->Clear();
    }

// Default constructor
MsTimer::MsTimer()
    : Timer
        (
            "DFLT",      // Name of timer.
            10,	        // Initial expiration time.
            10,     	// Reschedule expiration time.
            FALSE       // Clock is initially disabled.
        )
{
        // Create and Initialize the Event flag
        TimerEvent = new Event();
        TimerEvent->Clear();
}




//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing: Set event flag 
//
//    Input Parameters:    None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//******************************************************************************
void MsTimer::ExpirationRoutine()
{
    // Wake up the task by setting the event flag            
    TimerEvent->Set();
}
