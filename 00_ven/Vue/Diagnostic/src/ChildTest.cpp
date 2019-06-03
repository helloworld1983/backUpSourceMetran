//$COMMON.CPP$
//   File Name:  ChildTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Provide a common base for all child tests. This class provides
//      common event and state fuctions used by child tests.
//
//      A parent test is the high level state machine that controls when 
//      each child test is run. A child test has a narrow testing focus and
//      is responsible for testing a single system or component. A child
//      test has no visiblity to when it executes in relation to other child
//      tests. The parent test has the responsiblity to sequence the child
//      tests in the correct order. 
//
//   Interfaces: 
//
//   Restrictions:
/******************************************************************************/

#include "ChildTest.h"
#include "TestServices.h"

//*****************************************************************************/
//    Operation: ChildTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *client - a pointer to the test client.
//      stringID - the test name string ID of the client.
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
ChildTest::ChildTest(SelfTest* parent, E_StringID stringID) : 
    SelfTest(parent, stringID)
{
}

//*****************************************************************************/
//    Operation Name: ST_Idle()
//
//    Processing: State machine sits here when not testing. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void ChildTest::ST_Idle(EventData *t)
{
    // put the instrument into a known state
    S_Serv->KnownState();
}

//*****************************************************************************/
//    Operation Name: ST_Complete()
//
//    Processing: Test has completed successfully. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void ChildTest::ST_Complete(EventData *t)
{
    // let parent test know we successfully completed the test
    // goto idle state
    Parent->ChildComplete(this);
    NextState(eST_Idle);
}

//*****************************************************************************/
//    Operation Name: ST_Cancel()
//
//    Processing: Cancel the test in progress.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void ChildTest::ST_Cancel(EventData *)
{
    // let parent test know test has failed
    // goto idle state
    Parent->ChildFailure(this, eFC_TestCanceledByUser);
    NextState(eST_Idle);
}


