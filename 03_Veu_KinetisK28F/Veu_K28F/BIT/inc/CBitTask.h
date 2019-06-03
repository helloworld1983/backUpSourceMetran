//******************************************************************************/
//$COMMON.H$
//   File Name:  CbitTask.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the CBIT Task.  The
//      CbitTask class is derived from the Nucleus Plus RTOS base Task class.
/******************************************************************************/
#ifndef CBITTASK_H
#define CBITTASK_H

#include "Test.h"
#include "CommonConstants.h"

#include "DebugTrace.h"

// Forward reference
class CbitTimer;

static const UNSIGNED HIGH_TEMP_TIME = 1 * TICKS_PER_SEC;
static const UNSIGNED ADC_DAC_TIME = 5 * TICKS_PER_SEC;
static const UNSIGNED CLEAR_STRIKES_TIME = 120 * TICKS_PER_MIN;  // 2 hours

// This value defines the maximum message size for a message on the CBIT Queue.
// The size is specified in the number of UNSIGNED data elements.  
// If the message is not an exact multiple of an UNSIGNED, it is rounded up
// to the next size.  
static const UNSIGNED CBIT_Q_MSG_SIZE = 
    (sizeof(E_EventCode) + sizeof(UNSIGNED) - 1)/sizeof(UNSIGNED);

// The E_CbitTestId enumeration is a list of CBIT Test Id's.  These are the 
// tests that are run from the CBIT Task.
enum E_CbitTestId 
{
    eCbitOnChipCrcTestId,
    eCbitProgramCrcTestId,
    eCbitRamTestId,
    eCbitHighTempTestId,
    eCbitAdcDacTestId,
    eCbitFlowTableCrcTestId,

    // Add new tests above here
    eNumCbitTests
};

// This structure define test information for each of the CBIT Tests. 
struct CbitTestInfo
{
    E_Bool isTimeToRun;         // time to run the test
    Test* testPtr;              // pointer to the CBIT Test object
    CbitTimer* timerPtr;        // pointer to the timer associated with the test
};

//
// Forward Class Reference
//
class Csi;

class CbitTask
{
    friend class Csi;   // CSI ISR() needs access to TaskList[] for motor errors

    public:
        void SetIsTimeToRun(E_CbitTestId id) {TestList[id].isTimeToRun = eTrue;}
        static CbitTask* S_GetInstance(void);
        STATUS Send(E_EventCode code, UNSIGNED suspend = NU_NO_SUSPEND);

        //create alarm task
        void CBitCreateTask (void);

    protected:
        static void Entry(void* pvParameters);

    private:
        CbitTask(void);
        CbitTask(const CbitTask&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static CbitTask* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: TestList
        //  Description:  List of tests run as part of the CBIT Task
        //  Units: None
        //  Range: n/a
        CbitTestInfo TestList[eNumCbitTests];

        //$COMMON.ATTRIBUTE$
        //  Name: QueuePointer
        //  Description:  Pointer to CBIT Task's Queue
        //  Units: None
        //  Range: n/a
//        Q* QueuePtr;

};

#endif // CBITTASK_H



