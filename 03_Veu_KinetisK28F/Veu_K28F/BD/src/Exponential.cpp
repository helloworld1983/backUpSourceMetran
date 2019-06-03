/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Exponential.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The Exponential class is declared in this file.  The exponential is
//      used for determining rise to pressure during PCV, Pressure support, and
//      NIPPV.  It is also used for defining the pressure decline to PEEP 
//      at the start of exhalation and it is used for defining the rise
//      to base flow during flow triggered exhalations.
//
//      One operation is defined here, GetExpTerm() which retrieves the the 
//      value that corresponds to
//
//       -3*time/RT
//      e          , where time is the time elapsed since the start of a phase
//      and RT is the time to rise (or fall) to the target.
//
//   Interfaces:
//      None
//
//   Restrictions:
//      This class is not instantiated.  It contains only static operations.
/******************************************************************************/

#include "Exponential.h"

const SHORT NUM_EXP_DATA_PTS = 165;

//   -3(time)/RT
//  e
//
//  time/RT * 100 is used to index into the array;
//  the value in the array at time/RT * 100 is e**-3(time)/RT
static LONG ExpData[NUM_EXP_DATA_PTS] =
{
//  0    1   2   3   4   5   6   7   8   9

        64, 62, 60, 58, 57, 55, 53, 52, 50, 49, /*  0 -  9 */
        47, 46, 45, 43, 42, 41, 40, 38, 37, 36, /* 10 - 19 */
        35, 34, 33, 32, 31, 30, 29, 28, 28, 27, /* 20 - 29 */
        26, 25, 25, 24, 23, 22, 22, 21, 20, 20, /* 30 - 39 */
        19, 19, 18, 18, 17, 17, 16, 16, 15, 15, /* 40 - 49 */
        14, 14, 13, 13, 13, 12, 12, 12, 11, 11, /* 50 - 59 */
        11, 10, 10, 10, 9, 9, 9, 9, 8, 8, /* 60 - 69 */
        8, 8, 7, 7, 7, 7, 7, 6, 6, 6, /* 70 - 79 */
        6, 6, 5, 5, 5, 5, 5, 5, 5, 4, /* 80 - 89 */
        4, 4, 4, 4, 4, 4, 4, 3, 3, 3, /* 90 - 99 */
        3, 3, 3, 3, 3, 3, 3, 3, 3, 2, /* 100 - 109 */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* 110 - 119 */
        2, 2, 2, 2, 2, 2, 1, 1, 1, 1, /* 120 - 129 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 130 - 139 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 140 - 149 */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 150 - 159 */
        1, 1, 0, 0, 0 /* 160 - 165 */
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetExpTerm()
//
//    Processing: 
//      This operation returns the exponential term of the pressure or flow
//      profile.
//
//    Input Parameters:
//      riseTime - time to rise to the target pressure or flow (system TICKS)
//      elapsedTime - time elapsed since the start of the phase (system TICKS)
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - exponential term
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
LONG Exponential::GetExpTerm (UNSIGNED riseTime, UNSIGNED elapsedTime)
{
    LONG ix;    // index into ExpData

    // Determine the index into the ExpData array based on the elapsed
    // time and the rise time.  Rise time can actually be a rise or fall time.
    ix = (elapsedTime * 100) / riseTime;

    // Limit the index to the maximum element of the array.     
    if(ix >= NUM_EXP_DATA_PTS)
    {
        ix = NUM_EXP_DATA_PTS - 1;
    }

    return (ExpData[ix]);

}   // end GetExpTerm()

