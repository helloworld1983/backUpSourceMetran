/******************************************************************************/
//$COMMON.CPP$
//   File Name:  MonitorOTP.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//            Updates the Global Watch Dog
// 
//            Monitor tasks within the system keeping statistics of
//            execution time (min, max average), and scheduling interval (min,
//            max, average) and CPU percentage.
//
//            In order to start the timing of a task set the attribute StartNow
//            to 1.  This can be done by examining the address of the variable
//            and using the ICE server to modify the address once the system is
//            past it's start up state.  During startup there is a system back
//            log preventing accurate timing.
//
//            To restart the timing sequence, the ClearData attribute is set to
//            1 and the StartNow attrubute reset to 1.
//
//            Set a break point in the task you are interested.  Step into
//            the monitoring functions and type a *this at the debugger to 
//            examine timing data.
//
//
//   Interfaces: All system tasks.
//
//   Restrictions:  Note the statistics do not account for any time being 
//                  swapped out by the OS.
//
//   Revision History:
//
//      Rev:      Date:       Engineer:           Project:
//      04       08/04/98    K. A. McGuigan      Morph
//      Description: SCR-283.  Changed call to MonitorInit() to use jump table.
//
//      Rev:  Date:     Engineer:           Project:
//      01    05/20/98  Dan Seibert         Morph
//      Description: Initial Revision
//
/******************************************************************************/

// THIS MODULE IS LINKED INTO ROM 
// as a performance enhancement for Morph
//
#pragma ghs section text=".romtext"

#include "Monitor.h"
#include "util.h"


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MonitorBegin   
//
//    Processing:
//          Computes the scheduled time of a task.  Inserted at the begin of 
//          main loop on a task to be monitored.
//          Updates the monitored WatchDog heart beat with the current time.
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
void Monitor::MonitorBegin(void)

{
UNSIGNED currentTime;

    currentTime = SystemClock::Retrieve();

    UpdateTime[MonitorId] = currentTime;

    if (ClearData == eTrue)
    {
        J_MonitorInit();  // clear history data
        return;
    }

    if (!StartNow)
        return;

    if (FirstTime == eTrue)
    {
        StartNow1 = eTrue;
        CurrentTime = currentTime;
        InitTime = CurrentTime;
        return;
    }

    NumExecutions++;

    SchedulingInterval = currentTime - CurrentTime;

    CurrentTime = currentTime;

    TotalSchedulingInterval += SchedulingInterval;

    AvgSchedulingInterval = TotalSchedulingInterval / NumExecutions;
        
    if (SchedulingInterval > MaxSchedulingInterval)
       MaxSchedulingInterval = SchedulingInterval;
            
    if (SchedulingInterval < MinSchedulingInterval)
       MinSchedulingInterval = SchedulingInterval;

}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MonitorEnd  
//
//    Processing:   
//          Computes the execution time of a task.  Inserted at the end of main
//          loop on a task to be monitored
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
void Monitor::MonitorEnd(void)

{
    UNSIGNED currentTime;
    
    if (!StartNow1)
        return;

    if (FirstTime == eTrue)
    {
        FirstTime = eFalse;
        return;
    }

    currentTime = SystemClock::Retrieve();

    ExecutionTime = currentTime - CurrentTime;

    TotalExecutionTime += ExecutionTime;

    AvgExecutionTime = TotalExecutionTime / NumExecutions;

    if (ExecutionTime > MaxExecutionTime)
       MaxExecutionTime = ExecutionTime;
            
    if (ExecutionTime < MinExecutionTime)
       MinExecutionTime = ExecutionTime;
       
    // compute percent CPU usage
    CPUpercent = (TotalExecutionTime*100) / (CurrentTime - InitTime);       

}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GlobalMonitor  
//
//    Processing: Monitors all tasks to check their HeartBeat if a task has
//                not checked in within the WatchDogInterval it is probably not
//                operating properly.
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
void Monitor::GlobalWatchDog()

{
    register int i;
    E_Bool  strobe = eTrue;

    if (InDiagnosticMode())
    {
        WDogPtr->StrobeAll();
        return;
    }    

    for (i = 0; i < eLastTask; i++)
    {
        if ( (tasks[i].WatchDogInterval != 0) && (UpdateTime[i] != 0) )
        {
            if (CalcElapsedTime(UpdateTime[i])>=tasks[i].WatchDogInterval)
            {
                strobe = eFalse;
                break;
            }

        }
            
    } // for (i = 0; i < eLastTask; i++) 

    if (strobe)
        WDogPtr->StrobeAll();
    else
    {
        J_Timeout(i);
    }
}




