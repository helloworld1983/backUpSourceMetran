//$COMMON.CPP$
//   File Name:  LEDTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To verify that the front panel LEDs illuminate and extinguish 
//      correctly. 
//
//      Test Approach - Light each of the front panel LEDs and prompt the 
//          operator to verify each LED's operation.  Most of the LEDs have a 
//          "blinking" state that will be used to verify that they light
//          and extinguish.  The "External Battery" and "Mains" LEDs are not
//          controllable by software, so they are excluded from this test.  
//
//          The Safety Valve LED is tied to the safety valve, so it will not
//          be tested in a blinking state.  Similarly, the Internal Battery 
//          Charging LED is not available in a blinking state.  
//
//          The state of each LED, prior to its test, will be preserved and 
//          restored following the test.  
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/
#define LEDTEST_CPP

#include "LEDTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"

//#ifndef NO_HARDWARE
//    #include "TRILED.h"
//#endif

E_State previousLEDState;

//*****************************************************************************/
//    Operation LEDTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *client - a pointer to the test client.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
LEDTest::LEDTest(SelfTest* parent) : 
    ChildTest(parent, eLEDTest)
{
}

//*****************************************************************************/
//    Operation Name: Start()
//
//    Processing: Event to start this test. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = 
    {
        // -New State-                    -Current State-
        eST_TurnOnNormalLED,            // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_TurnOnNormalLED
        eST_CannotHappen,               // ST_TurnOnAlarmHighLED
        eST_CannotHappen,               // ST_TurnOnAlarmMedLED
        eST_CannotHappen,               // ST_TurnOnVentInopLED
        eST_CannotHappen,               // ST_TurnOnSafetyValveLED
        eST_CannotHappen,               // ST_TurnOnAlarmSilenceLED
        eST_CannotHappen,               // ST_TurnOnAllO2LED
        eST_CannotHappen,               // ST_TurnOnScreenLockLED
        eST_CannotHappen,               // ST_TurnOnIntBattInUseLED
        eST_CannotHappen,               // ST_TurnOnIntBattChargingLED
        eST_CannotHappen,               // ST_TurnOnIntBattLowLED
        eST_CannotHappen                // ST_VerifyIntBattLowLEDOn
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
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_TurnOnNormalLED
        eST_Cancel,                     // ST_TurnOnAlarmHighLED
        eST_Cancel,                     // ST_TurnOnAlarmMedLED
        eST_Cancel,                     // ST_TurnOnVentInopLED
        eST_Cancel,                     // ST_TurnOnSafetyValveLED
        eST_Cancel,                     // ST_TurnOnAlarmSilenceLED
        eST_Cancel,                     // ST_TurnOnAllO2LED
        eST_Cancel,                     // ST_TurnOnScreenLockLED
        eST_Cancel,                     // ST_TurnOnIntBattInUseLED
        eST_Cancel,                     // ST_TurnOnIntBattChargingLED
        eST_Cancel,                     // ST_TurnOnIntBattLowLED
        eST_Cancel                      // ST_VerifyIntBattLowLEDOn
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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_TurnOnAlarmHighLED,         // ST_TurnOnNormalLED
        eST_TurnOnAlarmMedLED,          // ST_TurnOnAlarmHighLED
        eST_TurnOnVentInopLED,          // ST_TurnOnAlarmMedLED
        eST_TurnOnSafetyValveLED,       // ST_TurnOnVentInopLED
        eST_TurnOnAlarmSilenceLED,      // ST_TurnOnSafetyValveLED
        eST_TurnOnAllO2LED,             // ST_TurnOnAlarmSilenceLED
        eST_TurnOnScreenLockLED,        // ST_TurnOnAllO2LED
        eST_TurnOnIntBattInUseLED,      // ST_TurnOnScreenLockLED
        eST_TurnOnIntBattChargingLED,   // ST_TurnOnIntBattInUseLED
        eST_TurnOnIntBattLowLED,        // ST_TurnOnIntBattChargingLED
        eST_VerifyIntBattLowLEDOn,      // ST_TurnOnIntBattLowLED
        eST_CannotHappen                // ST_VerifyIntBattLowLEDOn
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
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_Idle(void)
{
    // put the instrument into a known state
    S_Serv->KnownState();
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnNormalLED()
//
//    Processing: Turn on the Normal LED.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnNormalLED(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    // Get the current state of the Normal LED
//    previousLEDState = NormalLED->GetState();

    // Turn on the LED.
//    NormalLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eNormalLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnAlarmHighLED()
//
//    Processing: Restore the previous Normal LED setting, verify that the 
//      Normal LED is illuminated, then test the Alarm High LED.  
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnAlarmHighLED(ResponseData* pData)
{
    // Restore the previous Normal LED setting
//    NormalLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(NormalLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(NormalLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(NormalLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the High Alarm LED
//    previousLEDState = HighAlarmLED->GetState();

    // Turn on the LED.
//    HighAlarmLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eAlarmHighLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnAlarmMedLED()
//
//    Processing: Restore the previous High Alarm LED setting. Verify the High
//      Alarm LED Illumination. Turn on the Med/Low Alarm LED.
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnAlarmMedLED(ResponseData* pData)
{
    // Restore the previous High Alarm LED setting
//    HighAlarmLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(HighAlarmLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(HighAlarmLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(HighAlarmLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Med Alarm LED
//    previousLEDState = MedAlarmLED->GetState();

    // Turn on the LED.
//    MedAlarmLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eAlarmMedLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnVentInopLED()
//
//    Processing: Restore the previous Med/Low Alarm LED setting. Verify the 
//      Med/Low Alarm LED Illumination. Turn on the Vent Inop LED.
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnVentInopLED(ResponseData* pData)
{
    // Restore the previous Med Alarm LED setting
//    MedAlarmLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(MedAlarmLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(MedAlarmLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(MedAlarmLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Vent Inop LED
//    previousLEDState = VentInopLED->GetState();

    // Turn on the LED.
//    VentInopLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eVentInopLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnSafetyValveLED()
//
//    Processing: Restore the previous Vent Inop LED setting. Verify the Vent
//      Inop LED Illumination. Turn on the Safety Valve LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous: 
//      The Safety Valve LED is tied to the safety valve, so, for this test, 
//      the safety valve in opened, causing the LED to turn on.  
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnSafetyValveLED(ResponseData* pData)
{
    // Restore the previous Vent Inop LED setting
//    VentInopLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(VentInopLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(VentInopLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(VentInopLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Safety Valve LED
//    previousLEDState = SafetyValve->GetState();

    // Turn on the LED (==> Open the Safety Valve)
//    SafetyValve->SetStateDirect(eOpen);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eSafetyValveLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnAlarmSilenceLED()
//
//    Processing: Restore the previous Safety Valve LED setting. Verify the 
//      Safety Valve LED Illumination. Turn on the Alarm Silence LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnAlarmSilenceLED(ResponseData* pData)
{
    // Restore the previous Safety Valve LED setting
//    SafetyValve->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(SafetyValveLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(SafetyValveLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(SafetyValveLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Alarm Silence LED
//    previousLEDState = AlarmSilenceLED->GetState();

    // Turn on the LED.
//    AlarmSilenceLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eAlarmSilenceLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnAllO2LED()
//
//    Processing: Restore the previous Alarm Silence LED setting. Verify the 
//      Alarm Silence LED Illumination. Turn on the 100% O2 LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnAllO2LED(ResponseData* pData)
{
    // Restore the previous Alarm Silence LED setting
//    AlarmSilenceLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(AlarmSilenceLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(AlarmSilenceLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(AlarmSilenceLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the 100% O2 LED
//    previousLEDState = AllO2LED->GetState();

    // Turn on the LED.
//    AllO2LED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eAllO2LEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnScreenLockLED()
//
//    Processing: Restore the previous 100% O2 LED setting. Verify the 
//      100% O2 LED Illumination. Turn on the Screen Lock LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnScreenLockLED(ResponseData* pData)
{
    // Restore the previous 100% O2 LED setting
//    AllO2LED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(AllO2LEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(AllO2LEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(AllO2LEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Screen Lock LED
//    previousLEDState = ScreenLockLED->GetState();

    // Turn on the LED.
//    ScreenLockLED->SetStateDirect(eBlinking);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eScreenLockLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnIntBattInUseLED()
//
//    Processing: Restore the previous Screen Lock LED setting. Verify the 
//      Screen Lock LED Illumination. Turn on the Internal Battery In Use LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnIntBattInUseLED(ResponseData* pData)
{
    // Restore the previous Screen Lock LED setting
//    ScreenLockLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(ScreenLockLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(ScreenLockLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(ScreenLockLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Get the current state of the Internal Battery LED
//    previousLEDState = InternalBatteryLED->GetState();

    // Turn on the LED.
//    InternalBatteryLED->SetStateDirect(eInUse);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eIntBattInUseLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnIntBattChargingLED()
//
//    Processing: Restore the previous Internal Battery In Use LED setting. 
//      Verify the Internal Battery In Use LED Illumination. Turn on the 
//      Internal Battery Charging LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous: 
//      The Internal Battery Charging LED is not available in a "blinking" state.  
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnIntBattChargingLED(ResponseData* pData)
{
    // Verify the LED Illumination
    CRITERION(IntBattInUseLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(IntBattInUseLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(IntBattInUseLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Turn on the Internal Battery Charging LED.
//    InternalBatteryLED->SetStateDirect(eCharging);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eIntBattChargingLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_TurnOnIntBattLowLED()
//
//    Processing: Restore the previous Internal Battery Charging LED setting. 
//      Verify the Internal Battery Charging LED Illumination. Turn on the 
//      Internal Battery Low LED. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_TurnOnIntBattLowLED(ResponseData* pData)
{
    // Verify the Internal Battery Charging LED Illumination
    CRITERION(IntBattChargingLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(IntBattChargingLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(IntBattChargingLEDOn, eTrue)
            break;
        default:
            break; 
    }
    
    // Turn on the Internal Battery Low LED.
//    InternalBatteryLED->SetStateDirect(eVeryLowChg);

    // Prompt the operator to verify that the LED is illuminated. 
//    TheGUI().GetResponse(eIntBattLowLEDMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyIntBattLowLEDOn()
//
//    Processing: Restore the previous Internal Battery Low LED setting. 
//      Verify the Internal Battery Low LED Illumination. 
//
//    Input Parameters:
//      *pData - pointer to response data. 
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::ST_VerifyIntBattLowLEDOn(ResponseData* pData)
{
    // Restore the previous Internal Battery LED setting
//    InternalBatteryLED->SetStateDirect(previousLEDState);

    // Verify the LED Illumination
    CRITERION(InternalBatteryLEDOn, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_LEDIlluminationFailure)
        
    switch (pData->response) 
    { 
        case eRejectResponse: 
            VERIFY_RESULT(InternalBatteryLEDOn, eFalse)
            break; 
        case eAcceptResponse: 
            VERIFY_RESULT(InternalBatteryLEDOn, eTrue)
            break;
        default:
            break; 
    }

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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void LEDTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (LEDTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&LEDTest::ST_Idle,
        (pFunc)&LEDTest::ST_Complete,
        (pFunc)&LEDTest::ST_Cancel,
        (pFunc)&LEDTest::ST_TurnOnNormalLED,
        (pFunc)&LEDTest::ST_TurnOnAlarmHighLED,
        (pFunc)&LEDTest::ST_TurnOnAlarmMedLED,
        (pFunc)&LEDTest::ST_TurnOnVentInopLED,
        (pFunc)&LEDTest::ST_TurnOnSafetyValveLED,
        (pFunc)&LEDTest::ST_TurnOnAlarmSilenceLED,
        (pFunc)&LEDTest::ST_TurnOnAllO2LED,
        (pFunc)&LEDTest::ST_TurnOnScreenLockLED,
        (pFunc)&LEDTest::ST_TurnOnIntBattInUseLED,
        (pFunc)&LEDTest::ST_TurnOnIntBattChargingLED,
        (pFunc)&LEDTest::ST_TurnOnIntBattLowLED,
        (pFunc)&LEDTest::ST_VerifyIntBattLowLEDOn
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}
