/******************************************************************************/
//$COMMON.CPP$
//   File Name:  CbitTimer.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      The CbitTimer is derived from the RTOS Timer class and is used to
//      implement CBIT Tests that are run on a fixed time interval.
//
//   Interfaces:
//      The ExpirationRoutine() interfaces with the CBIT Task to notify it
//      when it is time to run a specific CBIT Test.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "CbitTimer.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CbitTimer()
//
//    Processing: This method constructs a CbitTimer object.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
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

CbitTimer::CbitTimer(CHAR* name, UNSIGNED time, E_CbitTestId id): Timer(name, time, pdTRUE)
//    : Timer(name, time, time, TRUE),
//      TestId(id)
{
    
}   // end CbitTimer()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ExpirationRoutine()
//
//    Processing: 
//      This operation is invoked whenever the timer expires.  It calls
//      CbitTask::SetIsTimeToRun() for the test associated with this timer.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
/******************************************************************************/
void CbitTimer::ExpirationRoutine(void)
{

    // Notify the CBIT Task that it is time to run the test associated with 
    // this timer.
    (CbitTask::S_GetInstance())->SetIsTimeToRun(TestId);
    
}   // end ExpirationRoutine()




