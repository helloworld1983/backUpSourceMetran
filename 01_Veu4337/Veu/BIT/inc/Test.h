/******************************************************************************/
//$COMMON.H$
//   File Name:  Test.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Test class is an abstract base class from which all CBIT tests
//      are derived.
/******************************************************************************/
#ifndef TEST_H
#define TEST_H
#include "TestHandler.h"
#include "Standard.h"

typedef void (*HandlePtr) (E_EventCode eventCode);

// Some tests fail if a condition is met for a specific time period.
// Others fail if a condition is met each time it is checked for a specific
// number of times it is checked.  This enumeration defines the two types.
enum E_FailureType
{
    eTimeBased,
    eCountBased
};


// The E_TestStatus defines a test status.  It contains values for check 
// status as well as overall test status.
//
// These must match values defined in ramcell.s
enum E_TestStatus
{
    eCheckFailed = 0,       
    eCheckPassed = 1,
    eTestFailed = 2,
};

class Test 
{
    public:
        Test(const Test&);
        Test(void);
        Test
            (
                E_EventCode eventCode, 
                E_FailureType type, 
                UNSIGNED limit, 
                HandlePtr handlePtr
            );

        virtual E_TestStatus Run(void) = 0;
        inline void Handle() {Handler(EventCode);}
        inline void SetTestStatus(E_TestStatus status) {TestStatus = status;}

    protected:
        E_Bool IsFailedLimitMet(void);

        //$COMMON.ATTRIBUTE$
        //  Name: TestStatus
        //  Description:  Status of the most recent execution of this test.
        //  Units: n/a
        //  Range: n/a
        E_TestStatus TestStatus;

        //$COMMON.ATTRIBUTE$
        //  Name: EventCode
        //  Description:  Event Code associated with this test.  This value is
        //      logged to NVRAM when the test fails.
        //  Units: n/a
        //  Range: n/a
        E_EventCode EventCode;

        //$COMMON.ATTRIBUTE$
        //  Name: FailureType
        //  Description:  Indicates if this test is time based or count based.
        //  Units: n/a
        //  Range: n/a
        E_FailureType FailureType;

        //$COMMON.ATTRIBUTE$
        //  Name: CountLimit
        //  Description:  Number of times that the condition must exist before
        //      the test fails; used only if the test is count based.
        //  Units: none
        //  Range: n/a
        SHORT CountLimit;

        //$COMMON.ATTRIBUTE$
        //  Name: NumFailures
        //  Description:  Number of times that the condition has been met
        //      so far; used only if the test is count based.
        //  Units: none
        //  Range: n/a
        SHORT NumFailures;

        //$COMMON.ATTRIBUTE$
        //  Name: TimeOfFirstFailure
        //  Description:  Time stamp of the system clock when the condition was
        //      first met; used only if the test is time based.
        //  Units: TICKS
        //  Range: 0 - Max Num Ticks
        UNSIGNED TimeOfFirstFailure;

        //$COMMON.ATTRIBUTE$
        //  Name: TimeLimit
        //  Description:  Time that the condition can exist before the test 
        //      fails; used only if the test is time based.
        //  Units: TICKS
        //  Range: 0 - Max Num Ticks
        UNSIGNED TimeLimit;

        //$COMMON.ATTRIBUTE$
        //  Name: Handler
        //  Description:  Pointer to the routine that handles the test failure.
        //  Units: n/a
        //  Range: n/a
        HandlePtr Handler;

    private:
};

#endif // TEST_H
