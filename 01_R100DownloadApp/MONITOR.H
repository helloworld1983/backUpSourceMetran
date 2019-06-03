//*****************************************************************************/
//$COMMON.H$
//    File Name: Monitor.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//      Class: Monitor
//
//   Purpose:
//            Update the WatchDog monitor.
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
//            Note to disable compiling any monitoring code into each task the 
//            "Monitor" flag in *.bld file can be deleted.  
//
//
//    Revision History:
//
//       Rev:      Date:       Engineer:           Project:
//        04       08/04/98    K. A. McGuigan      Morph
//        Description: SCR-283.  Added jump table vector for MonitorInit().
//
//       Rev:      Date:       Engineer:           Project:
//        03       05/20/98    Dan Seibert         Morph
//        Description: Added routine to handle watchdog timeout condition.
//          Also added jump table vector for Timeout() routine.
//          The jump table provides a fixed location for FLASH routines
//          that are called from ROM.
//
//       Rev:      Date:       Engineer:           Project:
//        02       02/23/98    R.P. Rush           Morph
//        Description: SCR-149. Added Code review comments
//
//       Rev:      Date:       Engineer:         Project:
//        01       08/25/97    R.P. Rush         MORPH
//        Description: Intial Rev
//
/******************************************************************************/ 
#ifndef MONITOR_H
#define MONITOR_H


#ifdef USE_NUCLEUS
#include    "nucpp.h"
#endif
#include    "Standard.h" 
#include    "tasks.h"
#include    "WatchDog.h"
 
/* * * * * * * * * * * * * C O N S T A N T S * * * * * * * * * * * * */
/* * * * * * * * * * T Y P E   D E F I N I T I O N S * * * * * * * * */
/* * * * * * * * * * C L A S S   D E F I N I T I O N S * * * * * * * */

class Monitor
{

    public:

        // constructor
        
        Monitor(void);

        Monitor(domainIDs id);

        // initialize vars

        void MonitorInit(void);
        void J_MonitorInit(void);

        // inserted before main at top of main loop on a task to be monitored

        void MonitorBegin(void);

        // inserted at the end of main loop on a task to be monitored

        void MonitorEnd(void);

        static void GlobalWatchDog(void);


        //$COMMON.ATTRIBUTE$
        //    Name: ClearData  
        //
        //    Description:
        //          Flag used to delay time measurement 
        //
        //    Units: None
        //
        //    Range: N/A
        //              
        E_Bool  ClearData;

        //$COMMON.ATTRIBUTE$
        //    Name: StartNow  
        //
        //    Description:
        //          Flag used to delay time measurement 
        //
        //    Units:  None
        //
        //    Range: N/A
        //              
        E_Bool  StartNow;

        //$COMMON.ATTRIBUTE$
        //    Name: StartNow1  
        //
        //    Description:
        //          True if first time through monitoring 
        //
        //    Units:  None
        //
        //    Range: N/A
        //              
        E_Bool  StartNow1;

        //$COMMON.ATTRIBUTE$
        //    Name: FirstTime  
        //
        //    Description:
        //          True if first time through monitoring 
        //
        //    Units:  None
        //
        //    Range: N/A
        //              
        E_Bool  FirstTime;

        //$COMMON.ATTRIBUTE$
        //    Name: InitTime  
        //
        //    Description:
        //          The time at initialization used to compute CPU usage 
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    InitTime;

        //$COMMON.ATTRIBUTE$
        //    Name: CurrentTime  
        //
        //    Description:
        //          The number of errors that occur if the interval is too large 
        //
        //    Units: None
        //
        //    Range: N/A
        //              
        UNSIGNED    CurrentTime;

        //$COMMON.ATTRIBUTE$
        //    Name: SchedulingInterval  
        //
        //    Description:
        //          The number of clock ticks that expire before executing this 
        //          task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    SchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: MaxSchedulingInterval  
        //
        //    Description:
        //          The maximum number of clock ticks that expire before 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    MaxSchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: MinSchedulingInterval  
        //
        //    Description:
        //          The minum number of clock ticks that expire before 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    MinSchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: TotalSchedulingInterval  
        //
        //    Description:
        //          The total number of clock ticks that expire before 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    TotalSchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: AvgSchedulingInterval  
        //
        //    Description:
        //          The average number of clock ticks that expire before 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    AvgSchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: ExecutionTime  
        //
        //    Description:
        //          The number of clock ticks that expire while executing this 
        //          task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    ExecutionTime;

        //$COMMON.ATTRIBUTE$
        //    Name: MaxExecutionTime  
        //
        //    Description:
        //          The maximum number of clock ticks that expire while 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    MaxExecutionTime;

        //$COMMON.ATTRIBUTE$
        //    Name: MinExecutionTime  
        //
        //    Description:
        //          The minum number of clock ticks that expire while 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    MinExecutionTime;

        //$COMMON.ATTRIBUTE$
        //    Name: AvgExecutionTime  
        //
        //    Description:
        //          The average number of clock ticks that expire while 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    AvgExecutionTime;

        //$COMMON.ATTRIBUTE$
        //    Name: TotalExecutionTime  
        //
        //    Description:
        //          The total number of clock ticks that expire while 
        //          executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    TotalExecutionTime;

        //$COMMON.ATTRIBUTE$
        //    Name: AvgCPUpercent  
        //
        //    Description:
        //          The average of cpu time while executing this task again.
        //
        //    Units: Percent
        //
        //    Range: N/A
        //              
        UNSIGNED    CPUpercent;

        //$COMMON.ATTRIBUTE$
        //    Name: NumExecutions  
        //
        //    Description:
        //          The average of cpu time while executing this task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //              
        UNSIGNED    NumExecutions;

        //$COMMON.ATTRIBUTE$
        //    Name: Id  
        //
        //    Description:
        //          Index used to with Tasks.
        //
        //    Units: None
        //
        //    Range: N/A
        //              
        domainIDs   MonitorId;

        //$COMMON.ATTRIBUTE$
        //    Name: WDogPtr  
        //
        //    Description:
        //          Pointer to WatchDog object
        //
        //    Units: None
        //
        //    Range: N/A
        //              
        static WatchDog *WDogPtr;

        //$COMMON.ATTRIBUTE$
        //    Name: UpdateTime  
        //
        //    Description:
        //          Each task when monitoring is done, the UpdateTime field to 
        //          indicates the last started  execution time.  If the 
        //          WatchDogInterval has elasped the watchdog will not be 
        //          strobed causing a system reset.
        //
        //    Units: None
        //
        //    Range: N/A
        //              
        static UNSIGNED UpdateTime[eLastTask];

    protected:


    private:
        static void Timeout(int i);     // Error handling routine
        static void J_Timeout(int i);   // jump table entry for Timeout()


}; // MonitorTasks




/* * * * * * * * * * I N L I N E   O P E R A T I O N S * * * * * * * */
/* * * * * * * * * F U N C T I O N   P R O T O T Y P E S * * * * * * */


#endif // MONITOR_H
