//$COMMON.CPP$
//   File Name:  EST.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Class to execute the Extended Self Tests (EST) for Diagnostic 
//      mode. Controls the high level order of execution for the child tests in 
//      order to complete the entire EST. Each child test will perform its 
//      tests and notify back with either a failure (meaning the child test 
//      failed for some reason) or a complete (the child test passed 
//      successfully).
//
//      EST has a step mode feature which stops after every child test and 
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
#include "EST.h"
//#include "GUI.h"
#include "DiagnosticTask.h"
#include "Result.h"
#include "LogEntry.h"
#include "FailureCodes.h"
#include "TestServices.h"
#ifdef USE_NUCLEUS
#include "ErrorHan.h"
#endif

//ALLOCATOR_CPP_INTERFACE(EST)

//*****************************************************************************/
//    Operation Name: TheEST()
//
//    Processing: Return a reference to a local static instance of this class. 
//      All access to this class should be made through this function. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:
//      EST& - a ref to the only instance of this class. 
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
EST& TheEST(void)
{
    static EST est;
    return est;
}

//*****************************************************************************/
//    Operation Name: EST()
//
//    Processing: Class constructor.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
EST::EST(void) : 
    ParentTest(eESTPassedMsg, eESTFailedMsg, eESTChildCompleteEvent, eEST_Base),
    blockPatientWyeTest(this),
    oxygenSupplyTest(this),
    oxygenDeliveryTest(this),
    oxygenSensorTest(this),
    airDeliveryTest(this),
    crossoverCircuitTest(this),
    blowerTest(this),
    heatedFilterTest(this),
    safetyValveTest(this),
    pressureReliefValveTest(this),
    exhalationValveTest(this),
    audioTest(this),
    keyboardTest(this),
    ledTest(this),
    patientCircuitTest(this)
{
}

//*****************************************************************************/
//    Operation Name: Start()
//
//    Processing: Starts the self test going. Ignore any start events if the 
//      self test is in progress. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::Start(void) 
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
        eST_EventIgnored,               // eST_BlowerTest
        eST_EventIgnored,               // eST_OxygenSupplyTest
        eST_EventIgnored,               // eST_CrossoverCircuitTest
        eST_EventIgnored,               // eST_OxygenDeliveryTest
        eST_EventIgnored,               // eST_OxygenSensorTest
        eST_EventIgnored,               // eST_AirDeliveryTest
        eST_EventIgnored,               // eST_PressureReliefValveTest
        eST_EventIgnored,               // eST_ExhalationValveTest
        eST_EventIgnored,               // eST_PatientCircuitTest
        eST_EventIgnored,               // eST_HeatedFilterTest
        eST_EventIgnored,               // eST_AudioTest
        eST_EventIgnored,               // eST_LEDTest
        eST_EventIgnored                // eST_KeyboardTest
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
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::Cancel(void) 
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
        eST_Cancel,                     // eST_BlowerTest
        eST_Cancel,                     // eST_OxygenSupplyTest
        eST_Cancel,                     // eST_CrossoverCircuitTest
        eST_Cancel,                     // eST_OxygenDeliveryTest
        eST_Cancel,                     // eST_OxygenSensorTest
        eST_Cancel,                     // eST_AirDeliveryTest
        eST_Cancel,                     // eST_PressureReliefValveTest
        eST_Cancel,                     // eST_ExhalationValveTest
        eST_Cancel,                     // eST_PatientCircuitTest
        eST_Cancel,                     // eST_HeatedFilterTest
        eST_Cancel,                     // eST_AudioTest
        eST_Cancel,                     // eST_LEDTest
        eST_Cancel                      // eST_KeyboardTest
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
//    Processing: Event indicating a response from the user. 
//
//    Input Parameters:
//      pData - pointer to response data.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::Response(ResponseData* pData) 
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
        eST_StepMode,                   // eST_BlowerTest
        eST_StepMode,                   // eST_OxygenSupplyTest
        eST_StepMode,                   // eST_CrossoverCircuitTest
        eST_StepMode,                   // eST_OxygenDeliveryTest
        eST_StepMode,                   // eST_OxygenSensorTest
        eST_StepMode,                   // eST_AirDeliveryTest
        eST_StepMode,                   // eST_PressureReliefValveTest
        eST_StepMode,                   // eST_ExhalationValveTest
        eST_StepMode,                   // eST_PatientCircuitTest
        eST_StepMode,                   // eST_HeatedFilterTest
        eST_StepMode,                   // eST_AudioTest
        eST_StepMode,                   // eST_LEDTest
        eST_StepMode                    // eST_KeyboardTest
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
//    Processing: Communication from a child test that the test is complete. 
//
//    Input Parameters: 
//      pData - pointer to a data structure with the complete data.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::Complete(CompleteData* pData)
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
        eST_BlowerTest,                 // eST_SafetyValveTest
        eST_OxygenSupplyTest,           // eST_BlowerTest
        eST_CrossoverCircuitTest,       // eST_OxygenSupplyTest
        eST_OxygenDeliveryTest,         // eST_CrossoverCircuitTest
        eST_OxygenSensorTest,           // eST_OxygenDeliveryTest
        eST_AirDeliveryTest,            // eST_OxygenSensorTest
        eST_PressureReliefValveTest,    // eST_AirDeliveryTest
        eST_ExhalationValveTest,        // eST_PressureReliefValveTest
        eST_PatientCircuitTest,         // eST_ExhalationValveTest
        eST_HeatedFilterTest,           // eST_PatientCircuitTest
        eST_AudioTest,                  // eST_HeatedFilterTest
        eST_LEDTest,                    // eST_AudioTest
        eST_KeyboardTest,               // eST_LEDTest
        eST_Complete                    // eST_KeyboardTest
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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::Failure(FailureData* pData)
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
        eST_Failure,                    // eST_BlowerTest
        eST_Failure,                    // eST_OxygenSupplyTest
        eST_Failure,                    // eST_CrossoverCircuitTest
        eST_Failure,                    // eST_OxygenDeliveryTest
        eST_Failure,                    // eST_OxygenSensorTest
        eST_Failure,                    // eST_AirDeliveryTest
        eST_Failure,                    // eST_PressureReliefValveTest
        eST_Failure,                    // eST_ExhalationValveTest
        eST_Failure,                    // eST_PatientCircuitTest
        eST_Failure,                    // eST_HeatedFilterTest
        eST_Failure,                    // eST_AudioTest
        eST_Failure,                    // eST_LEDTest
        eST_Failure                     // eST_KeyboardTest
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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_BlockPatientWyeTest(CompleteData* pData)
{
    testing = eTrue;

    // ensure no other test is active or that the active test is this class
    // we are starting the EST, make it the active test
    ASSERTION(!S_GetActiveTest() || S_GetActiveTest() == this);
    S_SetActiveTest(this);

    // reset the test results, a new test is starting
//    TheGUI().ResetTestResults();

    // get the instrument ready to start the EST
    S_Serv->Initiate();

    // let GUI know a child test is starting
//    TheGUI().TestStarted(blockPatientWyeTest.GetTestNameID());

    // start the test
    blockPatientWyeTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_SafetyValveTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_SafetyValveTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(safetyValveTest.GetTestNameID());

    // start the test
    safetyValveTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_BlowerTest()
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
void EST::ST_BlowerTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(blowerTest.GetTestNameID());

    // start the test
    blowerTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_OxygenSupplyTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_OxygenSupplyTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(oxygenSupplyTest.GetTestNameID());

    // start the test
    oxygenSupplyTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_CrossoverCircuitTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_CrossoverCircuitTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(crossoverCircuitTest.GetTestNameID());

    // start the test
    crossoverCircuitTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_OxygenDeliveryTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_OxygenDeliveryTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(oxygenDeliveryTest.GetTestNameID());

    // start the test
    oxygenDeliveryTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_OxygenSensorTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_OxygenSensorTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(oxygenSensorTest.GetTestNameID());

    // if this is a retest, then call retest initialize
    if (pData && pData->retest)
        oxygenSensorTest.RetestInit();

    // start the test
    oxygenSensorTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_AirDeliveryTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_AirDeliveryTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(airDeliveryTest.GetTestNameID());

    // start the test
    airDeliveryTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_PressureReliefValveTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_PressureReliefValveTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(pressureReliefValveTest.GetTestNameID());

    // start the test
    pressureReliefValveTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_ExhalationValveTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_ExhalationValveTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(exhalationValveTest.GetTestNameID());

    // start the test
    exhalationValveTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_PatientCircuitTest()   
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_PatientCircuitTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(patientCircuitTest.GetTestNameID());

    // start the test
    patientCircuitTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_HeatedFilterTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_HeatedFilterTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(heatedFilterTest.GetTestNameID());

    // start the test
    heatedFilterTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_AudioTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_AudioTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(audioTest.GetTestNameID());

    // start the test
    audioTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_KeyboardTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_KeyboardTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(keyboardTest.GetTestNameID());

    // start the test
    keyboardTest.Start();
}

//*****************************************************************************/
//    Operation Name: ST_LEDTest()
//
//    Processing: Execute the child test.
//
//    Input Parameters:
//      pData - a pointer to a structure containing the complete data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::ST_LEDTest(CompleteData* pData)
{
    // let GUI know test is starting
//    TheGUI().TestStarted(ledTest.GetTestNameID());

    // start the test
    ledTest.Start();
}

//*****************************************************************************/
//    Operation Name: DoStateAction()
//
//    Processing: Perform the required state processing. Depending on the state
//      machine's current state, call the appropriate method.
//
//    Input Parameters:
//      pData - event data to send to the state.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void EST::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
	typedef void (EST::*pFunc)(EventData* pData);

    // state table with pointers to each state method
	static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&EST::ST_Idle,
        (pFunc)&EST::ST_Complete,
        (pFunc)&EST::ST_Failure,
        (pFunc)&EST::ST_Retest,
        (pFunc)&EST::ST_Cancel,
        (pFunc)&EST::ST_StepMode,
        (pFunc)&EST::ST_BlockPatientWyeTest,
        (pFunc)&EST::ST_SafetyValveTest,
        (pFunc)&EST::ST_BlowerTest,
        (pFunc)&EST::ST_OxygenSupplyTest,
        (pFunc)&EST::ST_CrossoverCircuitTest,
        (pFunc)&EST::ST_OxygenDeliveryTest,
        (pFunc)&EST::ST_OxygenSensorTest,
        (pFunc)&EST::ST_AirDeliveryTest,
        (pFunc)&EST::ST_PressureReliefValveTest,
        (pFunc)&EST::ST_ExhalationValveTest,
        (pFunc)&EST::ST_PatientCircuitTest,
        (pFunc)&EST::ST_HeatedFilterTest,
        (pFunc)&EST::ST_AudioTest,
        (pFunc)&EST::ST_LEDTest,
        (pFunc)&EST::ST_KeyboardTest
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
//    Output Parameters: None
//    Return Values:
//      int - the next state in the test sequence. 
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
int EST::NextStateInSequence(int state)
{
    // based upon the current state, return the next state in test sequence
    switch (state)
    {
        case eST_BlockPatientWyeTest:
            return eST_SafetyValveTest;
        case eST_SafetyValveTest:
            return eST_BlowerTest;
        case eST_BlowerTest:
            return eST_OxygenSupplyTest;
        case eST_OxygenSupplyTest:
            return eST_CrossoverCircuitTest;
        case eST_CrossoverCircuitTest:
            return eST_OxygenDeliveryTest;
        case eST_OxygenDeliveryTest:
            return eST_OxygenSensorTest;
        case eST_OxygenSensorTest:
            return eST_AirDeliveryTest;
        case eST_AirDeliveryTest:
            return eST_PressureReliefValveTest;
        case eST_PressureReliefValveTest:
            return eST_ExhalationValveTest;
        case eST_ExhalationValveTest:
            return eST_PatientCircuitTest;
        case eST_PatientCircuitTest:
            return eST_HeatedFilterTest;
        case eST_HeatedFilterTest:
            return eST_AudioTest;
        case eST_AudioTest:
            return eST_LEDTest;
        case eST_LEDTest:
            return eST_KeyboardTest;
        case eST_KeyboardTest:
            return eST_Complete;
        default:
            ASSERTION(false);
	        return 0;
    }
}









