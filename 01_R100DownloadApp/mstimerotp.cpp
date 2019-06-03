//******************************************************************************//
//$COMMON.CPP$
//   File Name:  MSTIMEROTP.CPP
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
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//
//   Revision History:
//
//      Rev:  Date:     Engineer:            Project:
//      02    07/09/98  R. L. Stephenson     Morph
//      Description: Replaced #pragma that placed this code in the .romtext
//      section with the ROM.h header file.  This file contains common #pragma
//      statements for the text, data, and const section used with operations
//      placed in the OTP memory space.
//
//      Rev:  Date:     Engineer:           Project:
//      01    04/10/98  D. Seibert          Morph
//      Description: Moved the expiration routine into v851 on-chip ROM
//          as a performance enhancement for Morph.
//
//******************************************************************************

// THIS MODULE IS LINKED INTO ROM 
// as a performance enhancement for Morph
//
#include "ROM.h"

#define MSTIMER_CPP
#include "mstimer.h"
#include "nucpp.h"


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
//    Miscellaneous: 
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//
//    Requirements:  
//
//******************************************************************************
void MsTimer::ExpirationRoutine()
{
    // Wake up the task by setting the event flag            
    TimerEvent->Set();
}
