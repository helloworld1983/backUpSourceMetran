//*****************************************************************************/
//$COMMON.H$
//   File Name:  SelfTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: SelfTest
//      A class for which all self tests are derived. Provides state
//      transition and event routing services. 
//*****************************************************************************/

#ifndef SELFTEST_H
#define SELFTEST_H

#include "DebugTrace.h"
#include "Result.h"
#include "Statemachine.h"

// forward declaration eliminates include files thus speeding compile times
class TestServices;

// Macro used to verify a test Criterion. 'name' is the name of a Criterion 
// class and 'value' is the number to check against the limits. No action taken
// if test passes. If fails the parent test is notified of the failure, the
// current state does an early return, and the state machine transitions to the
// eST_Idle state.
#define VERIFY_RESULT(name, value) \
    if (!name->Passes(value)) { \
        Result::Info Info; \
        name->GetInfo(&Info); \
        Parent->ChildFailure(this, &Info); \
        NextState(eST_Idle); \
        return; \
    } else {}

// Macro used to verify a Accept/Reject response from the user
#define CHECK_RESPONSE(pData) \
    switch (pData->response) \
    { \
        case eRejectResponse: \
            Parent->ChildFailure(this, eFC_TestCanceledByUser); \
            NextState(eST_Idle); \
            return; \
        case eAcceptResponse: \
            break; \
        default: \
            ASSERTION(0); \
    }

class SelfTest : public StateMachine
{
public:
    SelfTest(SelfTest*, E_StringID = eNoString);
    virtual ~SelfTest() { }
    // Structure to carry response event data. Inherits from EventData to allow 
    // new/delete of structure.
    struct ResponseData : public EventData
    {
        E_ResponseType response;
    };
    // Structure to carry failure event data. Inherits from EventData to allow 
    // new/delete of structure.
    struct FailureData : public EventData
    {
        E_FailureCodes code;
        SelfTest* test;
    };
    // Structure to carry complete event data. Inherits from EventData to allow 
    // new/delete of structure.
    struct CompleteData : public EventData
    {
        CompleteData() 
        {
            failed = eFalse;
            retest = eFalse;
        }
        E_Bool failed;
        E_Bool retest;
    };

    // Methods for controlling SelfTest. If a particular derived class of 
    // SelfTest will receive these events, then override these methods. Every 
    // test must have at least the Start event defined. 
    virtual void Start(void) = 0;
    virtual void Cancel(void) { ASSERTION(0); }
    virtual void Response(ResponseData*) { ASSERTION(0); }
    virtual void Failure(FailureData*) { ASSERTION(0); }
    virtual void RetestInit(void) { ASSERTION(0); }

    // Methods to receive a status report from another SelfTest. If a particular 
    // derived class of SelfTest will receive these events, then override these 
    // methods.
    virtual void ChildComplete(SelfTest*) { ASSERTION(0); }
    virtual void ChildFailure(SelfTest*, E_FailureCodes) {ASSERTION(0);}
    virtual void ChildFailure(SelfTest*, Result::Info*) {ASSERTION(0);}

    static SelfTest* S_GetActiveTest(void) { return S_ActiveTest; }
    static void S_SetActiveTest(SelfTest* test) { S_ActiveTest = test; }

    E_StringID GetTestNameID(void) { return testNameID; }
protected:
    virtual void DoStateAction(EventData* pData) = 0;
    
    //$COMMON.ATTRIBUTE$
    //  Name: Parent
    //  Description: A pointer to the parent test.
    //  Units: 
    //  Range: 
    SelfTest* const Parent;
    
    //$COMMON.ATTRIBUTE$
    //  Name: S_ActiveTest
    //  Description: The test currently active. 
    //  Units: 
    //  Range: 
    static SelfTest* S_ActiveTest;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Serv
    //  Description: A pointer to TestServices where common test utilities
    //      are found. 
    //  Units: 
    //  Range: 
    static TestServices* S_Serv;

    //$COMMON.ATTRIBUTE$
    //  Name: testNameID
    //  Description: A string ID that is the name of the test.
    //  Units: 
    //  Range: 
    const E_StringID testNameID;

private:
    // prevent copying
    SelfTest(const SelfTest&);
    void operator=(SelfTest&);
};
#endif //SELFTEST_H

