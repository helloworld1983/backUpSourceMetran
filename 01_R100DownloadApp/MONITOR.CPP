/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Monitor.cpp
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
//      Rev:  Date:     Engineer:           Project:
//      04    06/02/98  D. Seibert          Morph
//      Description: Moved definition of static members into fixeddata.cpp
//      These static members are used by Monitor class methods loaded into 
//		.romtext, therefore the locations of these items must remain fixed.
//
//      Rev:  Date:     Engineer:           Project:
//      03    05/20/98  D. Seibert          Morph
//      Description: Moved constructor and initialization routines back into
//      FLASH.  Also added Timeout() method to handle task timeout condition.
//
//      Rev:  Date:     Engineer:           Project:
//      02    04/10/98  D. Seibert          Morph
//      Description: Moved into v851 on-chip OTP as a performance enhancement.
//
//      Rev:  Date:     Engineer:           Project:
//      01    09/03/97  R. P. Rush          Morph
//      Description: Initial Revision
//
/******************************************************************************/

#include <stdio.h>  // for sprintf

#include "Monitor.h"
#include "util.h"
#include "ErrorHan.h"




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init  
//
//    Processing: Used to start the monitoring process
//                If ClearData is true data base is cleared and monitoring
//                is started.
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
void Monitor::MonitorInit(void)

{

    FirstTime = eTrue;
    CurrentTime = 0;
    SchedulingInterval = 0;
    MaxSchedulingInterval = 0;
    MinSchedulingInterval = 0xffffffff;
    AvgSchedulingInterval = 0;
    ExecutionTime = 0;
    MaxExecutionTime = 0;
    MinExecutionTime = 0xffffffff;
    AvgExecutionTime = 0;
    TotalExecutionTime = 0;
    CPUpercent = 0;
    NumExecutions = 0;
    TotalSchedulingInterval = 0;
    StartNow1 = eFalse;
    ClearData = eFalse;
    StartNow = eFalse;
    InitTime = 0;

}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Monitor Constructor  
//
//    Processing: Initializes data to known state
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
Monitor::Monitor(domainIDs id)

{

    MonitorId = id;
    MonitorInit();
    WDogPtr = WatchDog::S_GetInstance();
    ASSERTION( WDogPtr != NULL);
    
}


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Monitor Constructor  
//
//    Processing: Initializes data to known state
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
Monitor::Monitor(void)
{
}
        

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Timeout
//
//    Processing: A task timeout condition has been detected.  Log the error,
//      and reset the system
//
//    Input Parameters: i - index into task table
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
void Monitor::Timeout(int i)
{
    char buffer[120];

    //format a string msg and call the ErrorHandler to log and reset!
    sprintf(buffer, 
     "Task %s failed Watchdog Test! Updt Tim = %ld WtchDg Intrvl %ld\r",
     tasks[i].Name, UpdateTime[i], tasks[i].WatchDogInterval); 
    ErrorHandler::J_GetInstance()->J_HandleRecoverable(buffer);

}
        

