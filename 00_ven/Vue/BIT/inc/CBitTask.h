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
        inline void SetIsTimeToRun(E_CbitTestId id) {TestList[id].isTimeToRun = eTrue;}
        static inline CbitTask* S_GetInstance(void);
        inline STATUS Send(E_EventCode code, UNSIGNED suspend = NU_NO_SUSPEND);

    protected:
        virtual void Entry(void);

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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the CbitTask object, it invokes 
//      CbitTask::S_GetInstance(), which returns a pointer to the 
//      CbitTask object.
//
//      If the object has not already been created, this method instantiates 
//      it and returns a pointer to the object.  If the object has already been 
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      CbitTask* - pointer to the object instantiated from this class
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
inline CbitTask* CbitTask::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new CbitTask();
        ASSERTION(S_Instance != NULL);
    }
   
    return(S_Instance);

}   // end S_GetInstance()



/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Send()
//
//    Processing: 
//      This operation invokes the Nucleus Plus RTOS kernel servis Q::Send()
//      to place a Batch Setting Event on the CBIT Queue.  This operation is
//      not invoked in the CBIT Task thread of control; it is called by other
//      tasks wanting to communicate with the CBIT Task via its queue.
//
//    Input Parameters:
//      batchEvent - reference to a Batch Settings Change Event message
//      suspend - indicates if the calling task should suspend if the queue
//          is full
//
//    Output Parameters:
//      None
//
//    Return Values:
//      STATUS - completion status for the Nucleus Plus kernel service Send()
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
inline STATUS CbitTask::Send(E_EventCode code, UNSIGNED suspend)
{
//    return(CbitTask::QueuePtr->Send(&code, CBIT_Q_MSG_SIZE, suspend));

}   // end Send()

#endif // CBITTASK_H



