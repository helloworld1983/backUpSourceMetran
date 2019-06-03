#line 2 "utilotp.cpp"           // force __FILE__ to drop path name
/******************************************************************************/
//$COMMON.CPP$
//   File Name:  UTIL.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains utility routines
//
//          Binary_Search()
//          Wait() - a routine to waste some time
//          ltoa() - a routine to convert a long number into a string
//
//   Interfaces: None
//
//   Restrictions:
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//
//   Revision History:
//
//      Rev:  Date:     Engineer:            Project:
//      03    08/19/98  K. A. McGuigan       Morph
//      Description: Added #line preprocessor directive to the top of the file.
//      This command sets the line number of the following line and allows
//      the __FILE__ filename to be overwritten.  By specifying a file name, 
//      the ASSERTION macro called from BinarySearch will use just the file name
//      instead of the entire path and file name.  Variations in the file path
//      were causing the .fixro area of the OTP to change from build to build.
//
//      02    07/09/98  R. L. Stephenson     Morph
//      Description: Replaced #pragma that placed this code in the .romtext
//      section with the ROM.h header file.  This file contains common #pragma
//      statements for the text, data, and const section used with operations
//      placed in the OTP memory space.
//
//      Rev:  Date:     Engineer:           Project:
//      01    04/10/98  D. Seibert          Morph
//      Description: Moved frequently executed routines into v851 on-chip ROM
//          as a performance enhancement for Morph.
//
/******************************************************************************/

// THIS MODULE IS LINKED INTO ROM 
// as a performance enhancement for Morph
//
#include "ROM.h"

#define UTIL_CPP
#include "util.h"
#include "CommonTypes.h"
#include "poststatus.h"


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: BinarySearch()
//
//    Processing: 
//      This routine performs a binary search into an array. 
//      If Value is less than Array[Start], Start is returned.  If Value is 
//      greater than Array[End], End is returned.  Otherwise, this routine
//      returns the index into the array whose entry is closest to Value without
//      exceeding Value.   In other words, if Array[3] = 10, Array[4] = 15 and
//      Value = 14, 3 will be returned.
//                                                         
//    Input Parameters:
//      Array[] - pointer to the array to search
//      Start - starting index into the array
//      End - last index into the array
//      Value - value searching for in the array
//
//    Output Parameters:
//      None
//
//    Return Values:
//      index into the array corresponding to the entry that most closely 
//      matches without exceeding Value
//
//    Pre-Conditions:
//      Start <= End
//      Assumes that the elements in Array[] are sorted in ascending order
//
//    Miscellaneous:
//      Assertion if Start > End
//      This code is mapped to the v851 on-chip ROM.
//      This code cannot be modified and still run on existing CPU boards.
//
//    Requirements:  
//
//******************************************************************************
long BinarySearch(const long Array[], long Start, long End, long Value)
{
    long Mid;
    
    ASSERTION(Start <= End);
    
    // Initialize Mid to Start in case the Value is less that the smallest
    // value, in which case the routine will return right away.
    Mid = Start;
    
    while ((Start <= End) && (Array[Start] <= Value))
    {
        Mid = (Start + End) >> 1;

        if (Array[Mid] == Value)
        {
            break;    // Found the exact value...
        }
        else if (Array[Mid] < Value)
        {
            Start = Mid + 1;
        }
        else
        {
            End = Mid - 1;
        }
    }

    return(Mid);

}



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
UNSIGNED CalcElapsedTime(UNSIGNED startTime)
{
    
    const UNSIGNED MAX_UINT = ~0L;
    UNSIGNED currentTime;
    UNSIGNED elapsedTime;

    currentTime = SystemClock::Retrieve();
    if (currentTime >= startTime)
    {
        elapsedTime = currentTime - startTime;
    }

    // Deal with rollover
    else
    {
        elapsedTime = (MAX_UINT - startTime + currentTime);
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
    if ((BootKeys | DIAGNOSTIC_MODE_KEYS) == DIAGNOSTIC_MODE_KEYS)
        return(eTrue);
    else
        return(eFalse);
}

#undef UTIL_CPP
