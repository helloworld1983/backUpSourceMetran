/******************************************************************************/
//$COMMON.CPP$
//   File Name:  UTIL.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:
//      This file contains utility routines
//
//   Interfaces: None
//
//   Restrictions:
//      None
/******************************************************************************/
#include <DeviceInterface.h>
#include <Util.h>
#include "DebugTrace.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CalcElapsedTime()
//
//    Processing: calculates the elasped time from startTime accounts for clock
//                  roll over
//
//    Input Parameters: startTime
//    Output Parameters: None
//    Return Values: time in clock ticks
//    Pre-Conditions: None
//    Miscellaneous:
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//    Requirements:
//
//******************************************************************************
unsigned long CalcElapsedTime (unsigned long startTime)
{
    const UNSIGNED MAX_UINT = ~0L;
    UNSIGNED currentTime;
    UNSIGNED elapsedTime;

    if(DeviceInterface::S_GetInstance()->RequestDevice(eGetCurrentTime, &currentTime))
    {
        if (currentTime >= startTime)
        {
            elapsedTime = currentTime - startTime;
        }

        // Deal with rollover
        else
        {
            elapsedTime = (MAX_UINT - startTime + currentTime);
        }
        DEBUG_UTIL("Current time is %Ds \n", elapsedTime);
    }
    else
    {
        DEBUG_UTIL("Can Not Get Current Time From RTC\n");
        elapsedTime = 0;
    }


    return(elapsedTime);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: InDiagnosticMode()
//
//    Processing: This function returns whether or not the ventilator
//          is operating in "Diagnostic Mode"
//
//    Input Parameters:  None
//
//    Output Parameters: None
//
//    Return Values: eTrue  = Diagnostic Mode
//                   eFalse = Operating Mode
//
//    Pre-Conditions: None
//    Miscellaneous:
//        The key inputs are assumed to be active low
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//    Requirements:
//
//******************************************************************************
E_Bool InDiagnosticMode()
{
    return(eFalse);
}
