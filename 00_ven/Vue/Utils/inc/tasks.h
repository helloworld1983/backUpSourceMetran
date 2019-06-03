/******************************************************************************/
//$COMMON.H$
//   File Name:  Tasks.h 
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the parmaters for constructing the class 
//            declarations for the system's tasks
/******************************************************************************/
#ifndef TASKS_H
#define TASKS_H

#include "Standard.h"
#include "CommonConstants.h"

static const UNSIGNED BD_CYCLE_TIME_IN_MS = 20;     // 20 ms

static const UNSIGNED BD_CYCLE_TIME_IN_TICKS = (BD_CYCLE_TIME_IN_MS
        + MS_PER_TICK / 2) / MS_PER_TICK;

enum domainIDs
// note: changing the number of domainIDs will have an effect on the 
//		 code located in v851 on-chip OTP.
{
    eFlowTask,
    eExhController,
    eInhController,
    eBd,
    eErrHand,
    eDataIO,
    eAlarms,
    ePd,
    eGui,
    eDiagnostics,
    eRS232,
    eBatteryTask,
    eCbit,
    eSpare1,
    eSpare2,
    eSpare3,
    eSpare4,
    eLastTask = (eSpare4 + 1)
};

class Tasks
{
    public:

        //$COMMON.ATTRIBUTE$
        //    Name: Name  
        //
        //    Description: The task's name (max of 7 char).
        //
        //    Units: None
        //
        //    Range: N/A
        //
        CHAR * Name;

        //$COMMON.ATTRIBUTE$
        //    Name: StackSize  
        //
        //    Description: Number of bytes for the task stack.
        //
        //    Units: bytes
        //
        //    Range: N/A
        //        
        UNSIGNED StackSize;

        //$COMMON.ATTRIBUTE$
        //    Name: Pri  
        //
        //    Description: Priority between 0 and 255 (0 high).
        //
        //    Units: None
        //
        //    Range: N/A
        //                
        //        OPTION Pri;
        UNSIGNED Pri;

        //$COMMON.ATTRIBUTE$
        //    Name: TimeSlice  
        //
        //    Description:
        //             The maximum number of clock ticks that 
        //             can expire while executing this task
        //             before other tasks of equal priority
        //             are allowed to execute. A value of zero 
        //             for timeslice disables time slicing for 
        //             this task.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //        
        UNSIGNED TimeSlice;

        //$COMMON.ATTRIBUTE$
        //    Name: PreEmption  
        //
        //    Description:
        //             TRUE indicates that this task is 
        //             preemptable and FALSE indicates 
        //             that it is not. Time slicing is 
        //             disabled if the task is not
        //
        //    Units: None
        //
        //    Range: N/A
        //                            
        bool PreEmption;

        //$COMMON.ATTRIBUTE$
        //    Name: SchedulingInterval  
        //
        //    Description:
        //             The number of clock ticks that expire before executing this 
        //            task again.
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //                
        UNSIGNED SchedulingInterval;

        //$COMMON.ATTRIBUTE$
        //    Name: WatchDogInterval  
        //
        //    Description:
        //             The number of clock ticks that expire before the watch dog
        //            is not strobed causing a system reset.  
        //
        //    Units: Clock Ticks
        //
        //    Range: N/A
        //                
        UNSIGNED WatchDogInterval;

    protected:
    private:
};

extern Tasks tasks[eLastTask];

#endif  //TASKS_H
