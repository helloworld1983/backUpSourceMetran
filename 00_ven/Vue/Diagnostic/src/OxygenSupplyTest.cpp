//$COMMON.CPP$
//   File Name:  OxygenSupplyTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To ensure the oxygen supply sensor correctly detects the existence
//      of an O2 source connected to the instrument. When this test completes
//      successfully the O2 source should be left disconnected to the 
//      instrument. 
//
//      The following systems are checked by this class:
//
//      Oxygen Supply Sensor
//          Test Approach - A transition from the sensor needs to be detected
//              by the sensor by the user connecting/disconnecting the O2
//              source.
//      
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "OxygenSupplyTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"
//
//#ifndef NO_HARDWARE
//    #include "DualKey.h"
//    #include "Flow_Cnt.h"
//#endif

//*****************************************************************************/
//    Operation OxygenSupplyTest()
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
OxygenSupplyTest::OxygenSupplyTest(SelfTest* parent) : 
    ChildTest(parent, eOxygenSupplyTest)
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
void OxygenSupplyTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_GetO2State,                 // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_GetO2State
        eST_CannotHappen,               // ST_DisconnectO2
        eST_CannotHappen,               // ST_VerifyO2Disconnected
        eST_CannotHappen,               // ST_ConnectO2
        eST_CannotHappen                // ST_VerifyO2Connected
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
void OxygenSupplyTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_GetO2State
        eST_Cancel,                     // ST_DisconnectO2
        eST_Cancel,                     // ST_VerifyO2Disconnected
        eST_Cancel,                     // ST_ConnectO2
        eST_Cancel                      // ST_VerifyO2Connected
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
void OxygenSupplyTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_GetO2State
        eST_VerifyO2Disconnected,       // ST_DisconnectO2
        eST_CannotHappen,               // ST_VerifyO2Disconnected
        eST_VerifyO2Connected,          // ST_ConnectO2
        eST_CannotHappen                // ST_VerifyO2Connected
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
//    Operation Name: ST_GetO2State()
//
//    Processing: Based on the initial state of the supply sensor, either 
//      ask the operator to disconnect the supply or connect it. 
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
void OxygenSupplyTest::ST_GetO2State(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    BleedExcessO2();

#ifdef NO_HARDWARE
    // IF O2 supply is connected
    //      goto disconnect state
    // ELSE
    //      goto connect state
    if (O2Pressure->GetStateDirect() == eOff)
        NextState(eST_DisconnectO2);
    else
        NextState(eST_ConnectO2);
#else
    //  Pretend that the O2 supply is connected
    //  goto disconnect state
    NextState(eST_DisconnectO2);
#endif
}

//*****************************************************************************/
//    Operation Name: ST_DisconnectO2()
//
//    Processing: Ask operator to disconnect the oxygen supply.
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
void OxygenSupplyTest::ST_DisconnectO2(void)
{
    // ask operator to disconnect the O2 supply
//    TheGUI().GetResponse(eDisconnectO2Msg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyO2Disconnected()
//
//    Processing: The operator was asked to disconnect the O2 source. Now
//      verify that it is actually disconnected. 
//
//    Input Parameters:
//      pData - the operator response.
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
void OxygenSupplyTest::ST_VerifyO2Disconnected(ResponseData* pData)
{
    CRITERION(O2Disconnected, int, 0, eEqual, eOn, 0, eNoString,
        eFC_O2NotDisconnected)

    // handle the response obtained from user on connecting O2
    CHECK_RESPONSE(pData)

    // verify the user actually disconnected the O2
    BleedExcessO2();
//    VERIFY_RESULT(O2Disconnected, O2Pressure->GetStateDirect())

    NextState(eST_Complete);
}

//*****************************************************************************/
//    Operation Name: ST_ConnectO2()
//
//    Processing: Ask operator to connect the oxygen supply.
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
void OxygenSupplyTest::ST_ConnectO2(void)
{
    // ask operator to connect the O2 supply
//    TheGUI().GetResponse(eConnectO2Msg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyO2Connected()
//
//    Processing: The operator was asked to connect the O2 source. Now
//      verify that it is actually connected. 
//
//    Input Parameters:
//      pData - the operator response.
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
void OxygenSupplyTest::ST_VerifyO2Connected(ResponseData* pData)
{
    CRITERION(O2Connected, int, 0, eEqual, eOff, 0, eNoString,
        eFC_O2NotConnected)

    // handle the response obtained from user on connecting O2
    CHECK_RESPONSE(pData)

    // verify the user actually connected the O2
    BleedExcessO2();
#ifndef NO_HARDWARE
//    VERIFY_RESULT(O2Connected, O2Pressure->GetStateDirect())
#else
//    VERIFY_RESULT(O2Connected, 0)
#endif

    NextState(eST_DisconnectO2);
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
void OxygenSupplyTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (OxygenSupplyTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&OxygenSupplyTest::ST_Idle,
        (pFunc)&OxygenSupplyTest::ST_Complete,
        (pFunc)&OxygenSupplyTest::ST_Cancel,
        (pFunc)&OxygenSupplyTest::ST_GetO2State,
        (pFunc)&OxygenSupplyTest::ST_DisconnectO2,
        (pFunc)&OxygenSupplyTest::ST_VerifyO2Disconnected,
        (pFunc)&OxygenSupplyTest::ST_ConnectO2,
        (pFunc)&OxygenSupplyTest::ST_VerifyO2Connected
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}

//*****************************************************************************/
//    Operation Name: BleedExcessO2()
//
//    Processing: Bleed off trapped residual pressure when O2 is disconnected.
//
//      When the user disconnects the oxygen source from the instrument, 
//      oxygen pressure will be trapped between CV1 and the O2 valve. This 
//      causes the oxygen supply switch to falsely indicate the presence of 
//      an O2 supply when it is actually disconnected. The solution is to 
//      release the residual pressure by opening the O2 valve
//      for a short duration before reading the oxygen supply switch.
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
void OxygenSupplyTest::BleedExcessO2(void)
{
    static const LONG FLOW = 2000 * LPM_FACTOR;  // 200.0 LPM

    // turn on O2 flow for a short duration to bleed off any residual pressure
    // trapped between CV1 and the O2 valve
//    O2FlowController->Reset(FLOW);
//    O2FlowController->SetDesired(FLOW);
//    NU_Sleep(ONE_SECOND);
//    O2FlowController->SetDesired(0);
}



