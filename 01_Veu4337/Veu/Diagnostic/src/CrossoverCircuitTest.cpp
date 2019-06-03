//$COMMON.CPP$
//   File Name:  CrossoverCircuitTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Tests the crossover circuit to ensure it can toggle appropriately
//      between using the air or O2 sources to keep the safety valve closed. 
//      The crossover circuit is responsible diverting source pressure to the 
//      safety pilot valve. If the pressure is removed, the safety valve will 
//      open. 
//      
//      This test expects the O2 supply to already be disconnected when this
//      test starts. 
//
//      The following systems are checked by this class:
//
//      Crossover Solenoid
//          Test Approach - Ensure the safety valve remains closed with the 
//              crossover circuit using only one supply, both air and O2.
//              By pressurizing the system and reading the pressure, we
//              can determine if the safety valve is indeed closed.
//
//      Check Valve 2 (CV2)
//          Test Approach - With the patient circuit blocked, the blower off, 
//              and the blower valve open an O2 flow is started. CV2 leaks if 
//              the flow backflushes out past flow sensor 1. 
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "CrossoverCircuitTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"

//#ifndef NO_HARDWARE
//    #include "Blower.h"
//    #include "DualKey.h"
//    #include "ExhContr.h"
//    #include "Flow_Cnt.h"
//#endif

const LONG CrossoverCircuitTest::BLOWER_SPINDOWN_FLOW = 500 * LPM_FACTOR;

//*****************************************************************************/
//    Operation CrossoverCircuitTest()
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
CrossoverCircuitTest::CrossoverCircuitTest(SelfTest* parent) : 
    ChildTest(parent, eCrossoverCircuitTest)
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
void CrossoverCircuitTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CrossoverCircuitBlower,     // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_CrossoverCircuitBlower
        eST_CannotHappen,               // ST_ConnectO2
        eST_CannotHappen,               // ST_VerifyO2Connected
        eST_CannotHappen,               // ST_CrossoverCircuitOxygen
        eST_CannotHappen                // ST_CheckValve2
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
void CrossoverCircuitTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_CrossoverCircuitBlower
        eST_Cancel,                     // ST_ConnectO2
        eST_Cancel,                     // ST_VerifyO2Connected
        eST_Cancel,                     // ST_CrossoverCircuitOxygen
        eST_Cancel                      // ST_CheckValve2
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
void CrossoverCircuitTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_CrossoverCircuitBlower
        eST_VerifyO2Connected,          // ST_ConnectO2
        eST_CannotHappen,               // ST_VerifyO2Connected
        eST_CannotHappen,               // ST_CrossoverCircuitOxygen
        eST_CannotHappen                // ST_CheckValve2
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
//    Operation Name: ST_CrossoverCircuitBlower()
//
//    Processing: Set the crossover solenoid valve to use the air source. If
//      the solenoid correctly, then the blower pressure should keep the
//      safety valve closed allowing pressure to build up in the system. 
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
void CrossoverCircuitTest::ST_CrossoverCircuitBlower(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const SHORT PRESSURE = 50 * SCALE;   // 50 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR;   // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 3; 

    CRITERION(O2Disconnected, int, 0, eEqual, eOn, 0, eNoString,
        eFC_O2NotDisconnected)
    CRITERION(ChargeCrossoverCircuitBlower, SHORT, SCALE, eGreaterThanOrEqual,
        PRESSURE, 0, ecmH2O, eFC_CrossoverCircuitFault)
        
    // verify O2 is disconnected the O2
//    VERIFY_RESULT(O2Disconnected, O2Pressure->GetStateDirect())

    // release built up pressure in the patient circuit
//    ExhController->Open();

    // wait for pressure to disapate
//    NU_Sleep(ONE_HUNDRED_MS * 5);

    // set crossover to use the blower (air) source to keep the safety pilot 
    // valve closed
//    CrossoverPilot->SetState(eOpen);

    // charge the circuit using the air controller
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            AirFlowController,
//            FLOW,
//            TIME_LIMIT,
//            PRESSURE,
//            &pressure
//        );

    // verify the correct pressure was attained
    VERIFY_RESULT(ChargeCrossoverCircuitBlower, pressure)

    // release built up pressure
//    ExhController->Open();

    // turn the blower off
//    TheBlower->RampToSpeed(0);

    NextState(eST_ConnectO2);
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
void CrossoverCircuitTest::ST_ConnectO2(void)
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
//      pData - pointer to user's response data.
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
void CrossoverCircuitTest::ST_VerifyO2Connected(ResponseData* pData)
{
    CRITERION(O2Connected, int, 0, eEqual, eOff, 0, eNoString,
        eFC_O2NotConnected)

    // handle the response obtained from user on connecting O2
    CHECK_RESPONSE(pData)
                                                             
    // verify the user actually connected the O2
#ifndef NO_HARDWARE
//    VERIFY_RESULT(O2Connected, O2Pressure->GetStateDirect())
#else
//    VERIFY_RESULT(O2Connected, 0)
#endif
    NextState(eST_CrossoverCircuitOxygen);
}

//*****************************************************************************/
//    Operation Name: ST_CrossoverCircuitOxygen()
//
//    Processing: Set the crossover solenoid valve to use the O2 source. If
//      the solenoid correctly, then the O2 pressure should keep the
//      safety valve closed allowing pressure to build up in the system. 
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
void CrossoverCircuitTest::ST_CrossoverCircuitOxygen(void)
{
    static const SHORT PRESSURE = 50 * SCALE;   // 50 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR;   // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 3; 

    CRITERION(InhFlowBlowerOff, LONG, LPM_SCALE, eLessThan, 
        BLOWER_SPINDOWN_FLOW / LPM_FACTOR, 0, eLPM, eFC_BlowerNotOffDAC)
    CRITERION(ChargeCrossoverCircuitOxygen, SHORT, SCALE, eGreaterThanOrEqual,
        PRESSURE, 0, ecmH2O, eFC_CrossoverCircuitFault)

    // spindown the blower to at least the spindown flow
    LONG blowerOffFlow;
    if (S_Serv->BlowerSpinDown(BLOWER_SPINDOWN_FLOW, &blowerOffFlow))
        return;

    // if blower is indeed off, then no flow should be measured
    VERIFY_RESULT(InhFlowBlowerOff, blowerOffFlow)

    // set crossover to use the blower (oxygen) source to keep the safety pilot 
    // valve closed
//    CrossoverPilot->SetState(eClose);

    // charge the circuit using the O2 controller
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            O2FlowController,
//            FLOW,
//            TIME_LIMIT,
//            PRESSURE,
//            &pressure
//        );

    // verify the correct pressure was attained
    VERIFY_RESULT(ChargeCrossoverCircuitOxygen, pressure)

    // release built up pressure
//    ExhController->Open();

    NextState(eST_CheckValve2);
}

//*****************************************************************************/
//    Operation Name: ST_CheckValve2()
//
//    Processing: Ensure the check valve 2 (CV2) does not leak when pressure
//      is applied. 
//
//      With the patient circuit blocked, the blower off, and the blower valve
//      open an O2 flow is started. The CV2 upstream path is now open to 
//      atmosphere and the downstream has the flow pressure being vented out
//      the pressure relief valve. CV2 leaks if the flow backflushes out
//      past flow sensor 1. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions: Blower must already be turned off and come to a complete
//      stop or at least slowed to provide marginal flow.
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void CrossoverCircuitTest::ST_CheckValve2(void)
{
    static const LONG FLOW = 1000 * LPM_FACTOR;   // 100.0 LPM 

    CRITERION(CheckValve2Leaky, LONG, LPM_SCALE, eLessThan, 30, 0, eLPM, 
        eFC_CheckValve2Leaky)

    // set crossover to use the O2 source to keep the safety pilot 
    // valve closed
//    CrossoverPilot->SetState(eClose);

    // disable the air controller
    // completely open air valve
//    AirFlowController->Disable();
//    AirValve->SetStepPosition(2000);

    // start an O2 flow going  
//    O2FlowController->Reset(FLOW);
//    O2FlowController->SetDesired(FLOW);

    // wait for flow to start
//    NU_Sleep(ONE_HUNDRED_MS * 2);

    // read air flow sensor
//    LONG inhFlow = S_Serv->GetFlow(AirFlowSensor);

#ifdef NO_HARDWARE
    inhFlow = 0;
#endif

    // verify CV2 does not leak, no flow should be measured
//    VERIFY_RESULT(CheckValve2Leaky, inhFlow)
    
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
void CrossoverCircuitTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (CrossoverCircuitTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&CrossoverCircuitTest::ST_Idle,
        (pFunc)&CrossoverCircuitTest::ST_Complete,
        (pFunc)&CrossoverCircuitTest::ST_Cancel,
        (pFunc)&CrossoverCircuitTest::ST_CrossoverCircuitBlower,
        (pFunc)&CrossoverCircuitTest::ST_ConnectO2,
        (pFunc)&CrossoverCircuitTest::ST_VerifyO2Connected,
        (pFunc)&CrossoverCircuitTest::ST_CrossoverCircuitOxygen,
        (pFunc)&CrossoverCircuitTest::ST_CheckValve2
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}


