//$COMMON.CPP$
//   File Name:  ParentTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Provide a common base for all parent tests. This class provides
//      common event and state fuctions used by parent tests.
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

#include "ParentTest.h"
//#include "GUI.h"
#include "TestServices.h"
#include "DiagnosticTask.h"
//
//#ifdef NO_HARDWARE
//    #include "HardwareStubs.h"
//#else
//    #include "DualKey.h"
//    #include "ErrorHan.h"
//#endif

//*****************************************************************************/
//    Operation Name: ParentTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      passed - the string ID for the passed message.
//      failed - the string ID for the failed message.
//      event - the event to post to the Diagnostic task when child test is 
//          complete.
//      base - the base number to add to the failure codes when displaying 
//          the value to the user or when placed into the error log.
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
ParentTest::ParentTest
    (
        E_StringID passed, 
        E_StringID failed, 
        E_DiagnosticEvent event, 
        int base
    ) : 
    SelfTest(NULL),
    PassedMsg(passed),
    FailedMsg(failed),
    ErrorCodeBase(base),
    ChildCompleteEvent(event)
{
    testing = eFalse;
}

//*****************************************************************************/
//    Operation Name: ChildComplete()
//
//    Processing: Called by the child test when the testing is complete. If
//      the software jumper JP1 is installed, the step mode is enabled. This
//      mode prompts the user to press continue after each test. This allow
//      faults to be injected between test during validation that might 
//      otherwise be occurring too quickly. 
//
//    Input Parameters:
//      test - a pointer to a Test class.
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
void ParentTest::ChildComplete(SelfTest* test)
{
    // a child test is complete, so make this test active once again
    S_SetActiveTest(this);

    // let GUI know a child test has successfully completed
//    TheGUI().TestCompleted(0);

    // if software jumper JP1 is not installed (meaning step mode is disabled)
//    if (SoftwareJumper->GetStateDirect())
//    {
//        TheDiagnosticTask().Send(ChildCompleteEvent);
//    }
//    else
//    {
//        // prompt user to press OK before continuing onto next test
//        TheGUI().GetResponse(eStepModeMsg);
//    }
}

//*****************************************************************************/
//    Operation Name: ChildFailure()
//
//    Processing: Called by the child test when a test fails.
//
//    Input Parameters:
//      test - a pointer to a Test class.
//      failure - a test failure code.
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
void ParentTest::ChildFailure(SelfTest* test, E_FailureCodes code)
{
    // a child test has failed, so make this test active once again
    S_SetActiveTest(this);

    // tell GUI the test failed (add base number to failure code)
//    TheGUI().TestCompleted(code + ErrorCodeBase);

    // create and fill failure struct
    FailureData* pData = new FailureData;
    pData->code = code;
    pData->test = test;

#ifdef USE_NUCLEUS
    // post message to the DiagnosticTask
    TheDiagnosticTask().Send(eFailureEvent, pData);
#endif
}

//*****************************************************************************/
//    Operation Name: ChildFailure()
//
//    Processing: Called by the child test when a test fails.
//
//    Input Parameters:
//      test - a pointer to a Test class.
//      info - information regarding failure.
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
void ParentTest::ChildFailure(SelfTest* test, Result::Info* info)
{
    // a child test has failed, so make this test active once again
    S_SetActiveTest(this);

    // let GUI know the test failed (add base number to failure code)
//    TheGUI().TestCompleted(
//        info->code + ErrorCodeBase,
//        info->units,
//        info->constraint,
//        info->result,
//        info->lower,
//        info->upper
//    );

    // create and fill failure struct
    FailureData* pData = new FailureData;
    pData->code = info->code;
    pData->test = test;

#ifdef USE_NUCLEUS
    // post message to the DiagnosticTask
    TheDiagnosticTask().Send(eFailureEvent, pData);
#endif
}

//*****************************************************************************/
//    Operation Name: ST_Idle()
//
//    Processing: State machine sits here when not testing. 
//
//    Input Parameters:
//      not used. 
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
void ParentTest::ST_Idle(ResponseData*)
{
    // parent test is complete, no test should be active
    S_SetActiveTest(NULL);

    // put the instrument into an idle state
    S_Serv->Conclude();

    testing = eFalse;
}

//*****************************************************************************/
//    Operation Name: ST_Complete()
//
//    Processing: Parent test is complete! Now determine whether parent test 
//      passed or failed and prompt the user with a dialog indicating success 
//      or failure.
//
//    Input Parameters:
//      pData - a pointer to the complete data structure. 
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
void ParentTest::ST_Complete(CompleteData* pData)
{
    // get pass/fail status for the self test
    E_Bool Passes = S_Serv->DoTestsPass();
 
    // send all test results out RS-232 port
    S_Serv->SendResultsOutRS232();

    // prompt user that test was either successful or unsuccessful
    if (Passes && !pData->failed)
    {
        // log passed code (which is just the base code) into error log
        // prompt user that test passed
//        ErrorHandler::S_GetInstance()->LogEvent((E_EventCode)ErrorCodeBase);
//        TheGUI().GetResponse(PassedMsg);
    }
    else
    {
        Result::Info info;
        S_Serv->GetFirstFailure(info);

        // create a log entry by added the failure code to the event code 
        // base to create a unique diagnostic event code for test
        if (info.code == eFC_None)
        {
            info.code = eFC_TestCanceledByUser;
            info.units = eNoString;
        }
//        E_EventCode logEntry = E_EventCode(ErrorCodeBase + (int)info.code);

        // redisplay the first failure encountered to the user
//        TheGUI().ClearFailureLine();
//        TheGUI().DisplayFailureLine(
//            info.code + ErrorCodeBase,
//            info.units,
//            info.constraint,
//            info.result,
//            info.lower,
//            info.upper
//        );

        // log failure into error log
        // prompt user that test failed
//        ErrorHandler::S_GetInstance()->LogEvent(logEntry);
//        TheGUI().GetResponse(FailedMsg);
    }
}

//*****************************************************************************/
//    Operation Name: ST_Failure()
//
//    Processing: Handle a test failure here. 
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
void ParentTest::ST_Failure(FailureData* pData)
{
    RetestState = LastState;

    // if user did not cancel test
    //      ask operator whether to Retest, Continue, or Cancel        
    // else
    //      goto complete state
    if (pData->code != eFC_TestCanceledByUser)
    {
//        TheGUI().GetRetestResponse(pData->test->GetTestNameID());
    }
    else
    {
        CompleteData* pComplete = new CompleteData;
        pComplete->failed = eTrue;
        NextState(eST_Complete, pComplete);
    }
}

//*****************************************************************************/
//    Operation Name: ST_Retest()
//
//    Processing: A failure has occurred, and the operator was asked whether to
//      Retest, Continue, or Cancel the test in progress. If Retest is selected,
//      then go back and do the last test again. If Continue is selected, then
//      continue on with the next test in the squence. If Cancel is selected,
//      then abort all self tests.
//
//    Input Parameters:
//      pData - a pointer to the response data structure. 
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
void ParentTest::ST_Retest(ResponseData* pData)
{
    DiagnosticEventStruct eventStruct;

    CompleteData* pComplete = NULL;

    // based upon the user response, goto a new state
    switch (pData->response)
    {
        case eRejectResponse:
            pComplete = new CompleteData;
            pComplete->failed = eTrue;
            NextState(eST_Complete, pComplete);
            break;
        case eRetestResponse:
//            TheGUI().ClearLastTestLine();
//            TheGUI().ClearFailureLine();
            
            pComplete = new CompleteData;
            pComplete->retest = eTrue;
            NextState(RetestState, pComplete);
            break;
        case eContinueResponse:
//            TheGUI().ClearFailureLine();
            
            if (NextStateInSequence(RetestState) == eST_Complete)
            {
                pComplete = new CompleteData;
                pComplete->failed = eFalse;
                NextState(NextStateInSequence(RetestState), pComplete);
            }
            else
                NextState(NextStateInSequence(RetestState));
            break;
        default:
            ASSERTION(false);
    }
}

//*****************************************************************************/
//    Operation Name: ST_Cancel()
//
//    Processing: Cancel all testing. 
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
void ParentTest::ST_Cancel(EventData *)
{
    CompleteData* pComplete = new CompleteData;
    pComplete->failed = eTrue;
    NextState(eST_Complete, pComplete);
}

//*****************************************************************************/
//    Operation Name: ST_StepMode()
//
//    Processing: User was prompted to continue before proceeding with the 
//      next test. Now go perform the next test in sequence. 
//
//    Input Parameters:
//      pData - a pointer to the response data structure. 
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
void ParentTest::ST_StepMode(ResponseData* pData)
{
    CompleteData* pComplete = new CompleteData;
    
    switch (pData->response)
    {
        case eRejectResponse:
            // user wants to cancel testing
            pComplete->failed = eTrue;
            pComplete->retest = eFalse;
            NextState(eST_Complete, pComplete);
            break;
        case eAcceptResponse:
            // user wants to continue with testing
            pComplete->failed = eFalse;
            pComplete->retest = eFalse;
            NextState(NextStateInSequence(LastState), pComplete);
            break;
        default:
            ASSERTION(false);
    }
}
