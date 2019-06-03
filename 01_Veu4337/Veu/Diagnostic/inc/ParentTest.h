//*****************************************************************************/
//$COMMON.H$
//   File Name:  ParentTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: ParentTest
//      Provide a common base for all parent tests. 
//*****************************************************************************/

#ifndef PARENTTEST_H
#define PARENTTEST_H

#include "SelfTest.h"
#include "DiagnosticEvents.h"

class ParentTest : public SelfTest
{
public:
    ParentTest(E_StringID, E_StringID, E_DiagnosticEvent, int);
    ~ParentTest() { }

    // events taken by this class
    void ChildComplete(SelfTest*);
    void ChildFailure(SelfTest*, E_FailureCodes);
    void ChildFailure(SelfTest*, Result::Info*);
    
    E_Bool IsTesting(void) { return testing; }
    
protected:
    // state machine states
    virtual void ST_Idle(ResponseData*);
    virtual void ST_Complete(CompleteData*);
    virtual void ST_Failure(FailureData*);
    virtual void ST_Retest(ResponseData*);
    virtual void ST_Cancel(EventData*);
    virtual void ST_StepMode(ResponseData*);

#ifndef USE_NUCLEUS
    virtual void ST_Idle(EventData*) {};
    virtual void ST_Complete(EventData*) {};
    virtual void ST_Failure(EventData*) {};
    virtual void ST_Retest(EventData*) {};
    virtual void ST_StepMode(EventData*) {};
#endif
    
    virtual int NextStateInSequence(int) = 0;
    
    enum E_States 
    {
        eST_Idle = 0,
        eST_Complete,
        eST_Failure,
        eST_Retest,
        eST_Cancel,
        eST_StepMode,
        eST_MaxStates
    };
    
    //$COMMON.ATTRIBUTE$
    //  Name: RetestState
    //  Description: The state to goto if user wants to retest the last test.
    //  Units: 
    //  Range: 
    USHORT RetestState;

    //$COMMON.ATTRIBUTE$
    //  Name: testing
    //  Description: True if test is in progress, false otherwise. 
    //  Units: 
    //  Range: 
    E_Bool testing;
    
    //$COMMON.ATTRIBUTE$
    //  Name: PassedMsg
    //  Description: Message to display to the user if test fails. 
    //  Units: 
    //  Range: 
    const E_StringID PassedMsg;
    
    //$COMMON.ATTRIBUTE$
    //  Name: FailedMsg
    //  Description: Message to display to the user if test passes. 
    //  Units: 
    //  Range: 
    const E_StringID FailedMsg;
    
    //$COMMON.ATTRIBUTE$
    //  Name: ErrorCodeBase
    //  Description: The base code to added to the failure code used to make a 
    //      unique entry into the error log.
    //  Units: 
    //  Range: 
    const int ErrorCodeBase;
    
    //$COMMON.ATTRIBUTE$
    //  Name: ChildCompleteEvent
    //  Description: The event to post when a child test completes. 
    //  Units: 
    //  Range: 
    const E_DiagnosticEvent ChildCompleteEvent;

private:
    // prevent copying
    ParentTest(const ParentTest&);
    void operator=(ParentTest&);
};
#endif //PARENTTEST_H


