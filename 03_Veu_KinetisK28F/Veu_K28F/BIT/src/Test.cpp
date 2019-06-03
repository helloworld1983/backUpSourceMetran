/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Test.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file defines the base Test class which is used to derive all
//      built-in tests.
//
//   Interfaces:
//      Operations within this class interface with utility functions.  
//      Otherwise, there are no interfaces.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "Test.h"
#include "Util.h"

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Test()
//
//    Processing: 
//      This operation is the constructor for the Test.
//
//    Input Parameters:
//      eventCode - event code error ID logged if this test fails
//      type - type of failure -- count based or time based
//      limit - amount of time or the number of occurances allowed before the
//              test fails
//      handlePtr - pointer to the function to handle a failed test
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
Test::Test
    (
        E_EventCode eventCode, 
        E_FailureType type, 
        UNSIGNED limit, 
        HandlePtr handlePtr
    )
    
{
    // If it is a count based test, set up count limit and default the 
    // time limit to 0.
    if (eCountBased == type)
    {
        CountLimit = (SHORT) limit;
        TimeLimit = 0;
    }
    
    // If it is a time based test, set up time limit and default the 
    // count limit to 0.
    else if (eTimeBased == type)
    {
        CountLimit = 0;
        TimeLimit = limit;
    }
    
    else
    {
        ASSERTION((type == eTimeBased) || (type == eCountBased));
    }

    NumFailures = 0;
    FailureType = type;
    EventCode = eventCode;
    TestStatus = eCheckPassed;
    
    Handler = handlePtr;

    ASSERTION(Handler != NULL);
    
    
}   // end Test()




/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: IsFailedLimitMet()
//
//    Processing: 
//      This operation determines if the test has failed based upon the
//      time limit or count limit established for the test.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      eTrue - the limit has been met.
//      eFalse - the limit has not been met.
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
E_Bool Test::IsFailedLimitMet(void)
{
    E_Bool rtnValue = eFalse;

    // If the failure was already reported, the test is still in a failed
    // state until the test resets it.
    if (eTestFailed == TestStatus)
    {
        rtnValue = eTrue;
    }

    else
    {    
        if (eTimeBased == FailureType)
        {
            // If this is the first time the check failed, snapshot the clock
            if (eCheckPassed == TestStatus)
            {
//                TimeOfFirstFailure = SystemClock::Retrieve();
            }
            else
            {
                // Compare the amount of time the condition has existed with the
                // time limit
                if (CalcElapsedTime(TimeOfFirstFailure) >= TimeLimit)
                {
                    rtnValue = eTrue;   
                }
            }           
        }
        else if (eCountBased == FailureType)
        {
            // First time in a row the check failed.  Reset NumFailures.
            if (eCheckPassed == TestStatus)
            {
                NumFailures = 0;
            }

            // Increment and compare NumFailures to the limit.
            NumFailures++;
            if (NumFailures >= CountLimit)
            {
                rtnValue = eTrue;        
            }
        }
        else
        {
            ASSERTION((eTimeBased == FailureType) || (eCountBased == FailureType));
        }
    }
    
    return(rtnValue);    

}   // end IsFailedLimitMet()
