//$COMMON.CPP$
//   File Name:  KeyboardTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To verify the front panel keyboard and rotary knob functions
//      correctly. 
//
//      The following systems are checked by this class:
//
//      Touch Panel
//          Test Approach - Ask the touch panel for a failed beam report. If
//              the touch panel indicates beams have failed, then then test
//              fails. Otherwise the test passes. 
//
//      Keys
//          Test Approach - Check all front panel keys by asking the operation
//              to press each one individually and confirming the correct 
//              key was pressed.
//
//      Rotary Knob
//          Test Approach - Prompt the operator to turn the knob and verify
//              the knob turns.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "KeyboardTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"
//
//#ifndef NO_HARDWARE
//    #include "Audible.h"
//#endif

#define VERIFY_KEYPRESS(name, value) \
//    if (name->Passes(value)) \
//        Audible::S_GetInstance()->SetFeedback(eGoodBeep); \
    else { \
        if (pData->response != eRejectResponse) \
            Audible::S_GetInstance()->SetFeedback(eBadBeep); \
        Result::Info Info; \
        name->GetInfo(&Info); \
        Parent->ChildFailure(this, &Info); \
        NextState(eST_Idle); \
        return; \
    }
    
//*****************************************************************************/
//    Operation KeyboardTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *client - a pointer to the test client.
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
KeyboardTest::KeyboardTest(SelfTest* parent) : 
    ChildTest(parent, eKeyboardTest)
{
}

//*****************************************************************************/
//    Operation Name: Start()
//
//    Processing: Event to start this test. 
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
void KeyboardTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_AskForFailedBeams,          // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_AskForFailedBeams
        eST_CannotHappen,               // ST_VerifyNoFailedBeams
        eST_CannotHappen,               // ST_PressAlarmReset
        eST_CannotHappen,               // ST_PressAllO2
        eST_CannotHappen,               // ST_PressManInsp
        eST_CannotHappen,               // ST_PressScreenLock
        eST_CannotHappen,               // ST_PressExpHold
        eST_CannotHappen,               // ST_PressOptionLeft
        eST_CannotHappen,               // ST_PressOptionRight
        eST_CannotHappen,               // ST_PressRotaryKnobAccept
        eST_CannotHappen,               // ST_TurnRotaryKnob
        eST_CannotHappen                // ST_VerifyRotaryKnobTurned
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
//    Processing: Event to cancel this test. 
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
void KeyboardTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_AskForFailedBeams
        eST_Cancel,                     // ST_VerifyNoFailedBeams
        eST_Cancel,                     // ST_PressAlarmReset
        eST_Cancel,                     // ST_PressAllO2
        eST_Cancel,                     // ST_PressManInsp
        eST_Cancel,                     // ST_PressScreenLock
        eST_Cancel,                     // ST_PressExpHold
        eST_Cancel,                     // ST_PressOptionLeft
        eST_Cancel,                     // ST_PressOptionRight
        eST_Cancel,                     // ST_PressRotaryKnobAccept
        eST_Cancel,                     // ST_TurnRotaryKnob
        eST_Cancel                      // ST_VerifyRotaryKnobTurned
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
//    Processing: Event indicating a response from the client. 
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
void KeyboardTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_VerifyNoFailedBeams,        // ST_AskForFailedBeams
        eST_CannotHappen,               // ST_VerifyNoFailedBeams
        eST_PressAlarmReset,            // ST_PressAlarmSilence
        eST_PressAllO2,                 // ST_PressAlarmReset
        eST_PressManInsp,               // ST_PressAllO2
        eST_PressScreenLock,            // ST_PressManInsp
        eST_PressExpHold,               // ST_PressScreenLock
        eST_PressOptionLeft,            // ST_PressExpHold
        eST_PressOptionRight,           // ST_PressOptionLeft
        eST_PressRotaryKnobAccept,      // ST_PressOptionRight
        eST_TurnRotaryKnob,             // ST_PressRotaryKnobAccept
        eST_VerifyRotaryKnobTurned,     // ST_TurnRotaryKnob
        eST_CannotHappen                // ST_VerifyRotaryKnobTurned
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
void KeyboardTest::ST_Idle(void)
{
//    TheGUI().DisableOffScreenKeys();

    // put the instrument into a known state
    S_Serv->KnownState();
}

//*****************************************************************************/
//    Operation Name: ST_AskForFailedBeams()
//
//    Processing: Ask the Diagnostic GUI subsystem for a touch panel's failed 
//      beam report.
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
void KeyboardTest::ST_AskForFailedBeams(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    // ask GUI for a failed beam report
//    TheGUI().FailedBeamReport();
}

//*****************************************************************************/
//    Operation Name: ST_VerifyNoFailedBeams()
//
//    Processing: Reponse to the failed beam report request routes here. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_VerifyNoFailedBeams(ResponseData* pData)
{
    CRITERION(FailedBeams, int, 0, eEqual, eFailedBeamReportPassResponse, 0, 
        eNoString, eFC_TouchscreenFailure)
       
    VERIFY_RESULT(FailedBeams, pData->response) 

#ifndef NO_HARDWARE
    NextState(eST_PressAlarmSilence);
#else
    NextState(eST_VerifyRotaryKnobTurned);
#endif
}

//*****************************************************************************/
//    Operation Name: ST_PressAlarmSilence()
//
//    Processing: Ask the the user to press this key. 
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
void KeyboardTest::ST_PressAlarmSilence(void)
{
//    TheGUI().EnableOffScreenKeys();
//    TheGUI().GetResponse(ePressAlarmSilenceKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressAlarmReset()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressAlarmReset(ResponseData* pData)
{
    CRITERION(AlarmSilenceKey, int, 0, eEqual, eAlarmSilenceKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(AlarmSilenceKey, pData->response)
    
//    TheGUI().GetResponse(ePressAlarmResetKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressAllO2()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressAllO2(ResponseData* pData)
{
    CRITERION(AlarmResetKey, int, 0, eEqual, eAlarmResetKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(AlarmResetKey, pData->response)

//    TheGUI().GetResponse(ePressAllO2KeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressManInsp()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressManInsp(ResponseData* pData)
{
    CRITERION(AllO2Key, int, 0, eEqual, eAllO2KeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(AllO2Key, pData->response)

//    TheGUI().GetResponse(ePressManInspKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressScreenLock()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressScreenLock(ResponseData* pData)
{
//    CRITERION(ManInspKey, int, 0, eEqual, eManInspKeyResponse, 0,
//        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(ManInspKey, pData->response)
    
//    TheGUI().GetResponse(ePressScreenLockKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressExpHold()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressExpHold(ResponseData* pData)
{
    CRITERION(ScreenLockKey, int, 0, eEqual, eScreenLockKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(ScreenLockKey, pData->response)

//    TheGUI().GetResponse(ePressExpHoldKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressOptionsLeft()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressOptionLeft(ResponseData* pData)
{
    CRITERION(ExpHoldKey, int, 0, eEqual, eExpHoldKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(ExpHoldKey, pData->response)

//    TheGUI().GetResponse(ePressOptionLeftKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressOptionRight()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressOptionRight(ResponseData* pData)
{
    CRITERION(OptionLeftKey, int, 0, eEqual, eOptionLeftKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(OptionLeftKey, pData->response)

//    TheGUI().GetResponse(ePressOptionRightKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_PressRotaryKnobAccept()
//
//    Processing: Check the response data from the previous user keypress 
//      query. Then ask the the user to press another key. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_PressRotaryKnobAccept(ResponseData* pData)
{
    CRITERION(OptionRightKey, int, 0, eEqual, eOptionRightKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(OptionRightKey, pData->response)

//    TheGUI().GetResponse(ePressRotaryKnobAcceptKeyMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnRotaryKnob()
//
//    Processing: Check the response data from the previous keypress user 
//      query. Then ask the the user to turn the rotary knob.
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_TurnRotaryKnob(ResponseData* pData)
{
    CRITERION(RotaryKnobAcceptKey, int, 0, eEqual, eRotaryKnobAcceptKeyResponse, 0, 
        eNoString, eFC_KeyBroke)
        
    VERIFY_KEYPRESS(RotaryKnobAcceptKey, pData->response)
    
//    TheGUI().DisableOffScreenKeys();
//    TheGUI().GetResponse(eTurnRotaryKnobMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyRotaryKnobTurned()
//
//    Processing: Verify the rotary knob was turned. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
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
void KeyboardTest::ST_VerifyRotaryKnobTurned(ResponseData* pData)
{
    CRITERION(RotaryKnob, int, 0, eEqual, eRotaryKnobTurnedResponse, 0, 
        eNoString, eFC_RotaryKnobBroke)
        
#ifndef NO_HARDWARE
    VERIFY_RESULT(RotaryKnob, pData->response)
#else
    // Force the test to pass
    VERIFY_RESULT(RotaryKnob, eRotaryKnobTurnedResponse)
#endif

    NextState(eST_Complete);
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
void KeyboardTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (KeyboardTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&KeyboardTest::ST_Idle,
        (pFunc)&KeyboardTest::ST_Complete,
        (pFunc)&KeyboardTest::ST_Cancel,
        (pFunc)&KeyboardTest::ST_AskForFailedBeams,
        (pFunc)&KeyboardTest::ST_VerifyNoFailedBeams,
        (pFunc)&KeyboardTest::ST_PressAlarmSilence,
        (pFunc)&KeyboardTest::ST_PressAlarmReset,
        (pFunc)&KeyboardTest::ST_PressAllO2,
        (pFunc)&KeyboardTest::ST_PressManInsp,
        (pFunc)&KeyboardTest::ST_PressScreenLock,
        (pFunc)&KeyboardTest::ST_PressExpHold,
        (pFunc)&KeyboardTest::ST_PressOptionLeft,
        (pFunc)&KeyboardTest::ST_PressOptionRight,
        (pFunc)&KeyboardTest::ST_PressRotaryKnobAccept,
        (pFunc)&KeyboardTest::ST_TurnRotaryKnob,
        (pFunc)&KeyboardTest::ST_VerifyRotaryKnobTurned
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}







