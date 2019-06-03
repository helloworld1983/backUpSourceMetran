//$COMMON.CPP$
//   File Name:  SST.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Class to execute the Short Self Tests (SST) for Diagnostic mode. 
//      Controls high level order of execution for the child tests in order to 
//      complete the entire SST. Each child test will perform its tests and 
//      notify back with either a failure (meaning the child test failed for 
//      some reason) or a complete (the child test passed successfully).
//
//      SST has a step mode feature which stops after every child test and 
//      prompts the user before continuing. This is useful for software testing
//      where faults need to be injected into the system at certain points
//      during the test sequence. The test mode is enabled by installing JP1 
//      (software jumper) on the CPU board. 
//
//   Interfaces:
//
//   Restrictions:
//      Calls from a child test to the parent should not execute any state 
//      machine calls directly. Instead, a message is posted to be serviced 
//      asynchronously. This prevents a calling structure where the same 
//      child state machine is reentered.
//
//      An incorrect calling sequence is:
//          child test calls parent test
//          parent test calls child test (child test is reentered, bad)
//  
//      The correct calling sequence is:
//          child test calls parent test
//          parent test posts a task message
//          child state machine completes execution
//          intertask message is received and parent test calls child test
/******************************************************************************/

#include "CommonConstants.h"
#include "SST.h"
//#include "GUI.h"
#include "DiagnosticTask.h"
#include "LogEntry.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#ifdef USE_NUCLEUS
//#include "ErrorHan.h"
//#endif

//ALLOCATOR_CPP_INTERFACE(SST)

//*****************************************************************************/
//    Operation Name: TheSST()
//
//    Processing: Return a reference to a local static instance of this class. 
//      All access to this class should be made through this function. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      SST& - a ref to the only instance of this class. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
SST& TheSST(void)
{
    static SST sst;
    return sst;
}

//*****************************************************************************/
//    Operation Name: SST()
//
//    Processing: Class constructor.
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
SST::SST(void) : 
    ParentTest(eSSTPassedMsg, eSSTFailedMsg, eSSTChildCompleteEvent, eSST_Base),
    patientCircuitTest(this),
    blockPatientWyeTest(this),
    safetyValveTest(this)
{
}

//*****************************************************************************/
//    Operation Name: Start()
//
//    Processing: Starts the self test going. Ignore any start events if the 
//      self test is in progress. 
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
void SST::Start(void) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_BlockPatientWyeTest,        // eST_Idle
        eST_EventIgnored,               // eST_Complete
        eST_EventIgnored,               // eST_Failure
        eST_CannotHappen,               // eST_Retest
        eST_CannotHappen,               // eST_Cancel
        eST_CannotHappen,               // eST_StepMode
        eST_EventIgnored,               // eST_BlockPatientWyeTest
        eST_EventIgnored,               // eST_SafetyValveTest
        eST_EventIgnored                // eST_PatientCircuitTest
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState]);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Cancel()
//
//    Processing: Cancels the current self test in progress.   
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
void SST::Cancel(void) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // eST_Idle
        eST_EventIgnored,               // eST_Complete
        eST_EventIgnored,               // eST_Failure
        eST_CannotHappen,               // eST_Retest
        eST_CannotHappen,               // eST_Cancel
        eST_CannotHappen,               // eST_StepMode
        eST_Cancel,                     // eST_BlockPatientWyeTest
        eST_Cancel,                     // eST_SafetyValveTest
        eST_Cancel                      // eST_PatientCircuitTest
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState]);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Response()
//
//    Processing: Event indicating a response from the GUI. 
//
//    Input Parameters:
//      pData - pointer to response data.
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
void SST::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // eST_Idle
        eST_Idle,                       // eST_Complete
        eST_Retest,                     // eST_Failure
        eST_CannotHappen,               // eST_Retest
        eST_CannotHappen,               // eST_Cancel
        eST_CannotHappen,               // eST_StepMode
        eST_StepMode,                   // eST_BlockPatientWyeTest
        eST_StepMode,                   // eST_SafetyValveTest
        eST_StepMode                    // eST_PatientCircuitTest
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState], pData);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Complete()
//
//    Processing: Event indicating the child test is complete.
//
//    Input Parameters: 
//      pData - a pointer to a structure containing the complete data. 
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
void SST::Complete(CompleteData* pData)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // eST_Idle
        eST_EventIgnored,               // eST_Complete
        eST_CannotHappen,               // eST_Failure
        eST_CannotHappen,               // eST_Retest
        eST_CannotHappen,               // eST_Cancel
        eST_CannotHappen,               // eST_StepMode
        eST_SafetyValveTest,            // eST_BlockPatientWyeTest
        eST_PatientCircuitTest,         // eST_SafetyValveTest
        eST_Complete                    // eST_PatientCircuitTest
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState], pData);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: Failure()
//
//    Processing: Event indicating a failure occurred.
//
//    Input Parameters:
//      pData - a struct containing the failure data.
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
void SST::Failure(FailureData* pData)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // eST_Idle
        eST_EventIgnored,               // eST_Complete
        eST_EventIgnored,               // eST_Failure
        eST_CannotHappen,               // eST_Retest
        eST_CannotHappen,               // eST_Cancel
        eST_CannotHappen,               // eST_StepMode
        eST_Failure,                    // eST_BlockPatientWyeTest
        eST_Failure,                    // eST_SafetyValveTest
        eST_Failure                     // eST_PatientCircuitTest
    };
    
    ASSERTION(StateTransitions[CurrentState] != eST_CannotHappen);
    ASSERTION(CurrentState < eST_CannotHappen);
    if (StateTransitions[CurrentState] != eST_EventIgnored)
    {
        NextState(StateTransitions[CurrentState], pData);
        StateEngine();
    }
}

//*****************************************************************************/
//    Operation Name: ST_BlockPatientWyeTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
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
void SST::ST_BlockPatientWyeTest(CompleteData* pData)
{
    testing = eTrue;

    // ensure no other test is active or that the active test is this class
    // we are starting the SST, make it the active test
    ASSERTION(!S_GetActiveTest() || S_GetActiveTest() == this);
    S_SetActiveTest(this);

    // reset the test results, a new test is starting
//    TheGUI().ResetTestResults();

    // get the instrument ready to start the SST
    S_Serv->Initiate();

    // let GUI know a child test of SST is starting
//    TheGUI().TestStarted(blockPatientWyeTest.GetTestNameID());

    // start the test
    blockPatientWyeTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_SafetValveTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
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
void SST::ST_SafetyValveTest(CompleteData* pData)
{
    // let GUI know test is starting
    // start the test
//    TheGUI().TestStarted(safetyValveTest.GetTestNameID());
    safetyValveTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_PatientCircuitTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
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
void SST::ST_PatientCircuitTest(CompleteData* pData)
{
    // let GUI know test is starting
    // start the test
//    TheGUI().TestStarted(patientCircuitTest.GetTestNameID());
    patientCircuitTest.Start();
}

//*****************************************************************************/
//    Operation Name: DoStateAction()
//
//    Processing: Perform the required state processing. Depending on the state
//      machine's current state, call the appropriate method.
//
//    Input Parameters:
//      pData - event data to send to the state.
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
void SST::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
	typedef void (SST::*pFunc)(EventData* pData);

    // state table with pointers to each state method
	static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&SST::ST_Idle,
        (pFunc)&SST::ST_Complete,
        (pFunc)&SST::ST_Failure,
        (pFunc)&SST::ST_Retest,
        (pFunc)&SST::ST_Cancel,
        (pFunc)&SST::ST_StepMode,
        (pFunc)&SST::ST_BlockPatientWyeTest,
        (pFunc)&SST::ST_SafetyValveTest,
        (pFunc)&SST::ST_PatientCircuitTest
	};

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}

//*****************************************************************************/
//    Operation Name: NextStateInSequence()
//
//    Processing: Returns the next test state in the sequence. All of the 
//      tests follow a linear sequence. So given a current state, the next
//      one can be determined. 
//
//    Input Parameters:
//      state - the current state. 
//
//    Output Parameters:
//
//    Return Values:
//      int - the next state in the test sequence. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
int SST::NextStateInSequence(int state)
{
    // based upon the current state, return the next state in test sequence
    switch (state)
    {
        case eST_BlockPatientWyeTest: 
            return eST_SafetyValveTest;
        case eST_SafetyValveTest:
            return eST_PatientCircuitTest;
        case eST_PatientCircuitTest:
            return eST_Complete;
        default:
            ASSERTION(false);
    	    return 0;
    }
}










