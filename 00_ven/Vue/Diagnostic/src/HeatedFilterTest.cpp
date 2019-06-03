//$COMMON.CPP$
//   File Name:  HeatedFilterTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Test the heated filter backpressure and verify the pressure is
//      within acceptable range.
//
//      The following systems are checked by this class:
//
//      Heated Filter Backpressure
//          Test Approach - With air flowing through the circuit, measure the 
//              backpressure generated both with and without the heated filter 
//              connected to the system. The calculated filter backpressure 
//              must be within specification.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "HeatedFilterTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"

//#ifndef NO_HARDWARE
//    #include "Flow_Cnt.h"
//    #include "PresSens.h"
//#endif

// the backpressure of the system components downstream of the heated filter
const SHORT HeatedFilterTest::PRESSURE_SYS = 100;  

//*****************************************************************************/
//    Operation HeatedFilterTest()
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
HeatedFilterTest::HeatedFilterTest(SelfTest* parent) : 
    ChildTest(parent, eHeatedFilterTest)
{
    PressureHF = 0;
    PressureConn = 0;
    PressureDisc = 0;
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
void HeatedFilterTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_MeasureFilterBackpressure,  // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_MeasureFilterBackpressure
        eST_CannotHappen,               // ST_DisconnectCircuitFromFilter
        eST_CannotHappen                // ST_MeasureNoFilterBackpressure
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
void HeatedFilterTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_MeasureFilterBackpressure
        eST_Cancel,                     // ST_DisconnectCircuitFromFilter
        eST_Cancel                      // ST_MeasureNoFilterBackpressure
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
void HeatedFilterTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_MeasureFilterBackpressure
        eST_MeasureNoFilterBackpressure,// ST_DisconnectCircuitFromFilter
        eST_CannotHappen                // ST_MeasureNoFilterBackpressure
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
//    Operation Name: ST_MeasureFilterBackpressure()
//
//    Processing: Measure the filter backpressure with the heated filter 
//      connected.
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
void HeatedFilterTest::ST_MeasureFilterBackpressure(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const LONG FLOW = 1000 * LPM_FACTOR;   // 100.0LPM

    CRITERION(ExhFlow100lpm, LONG, LPM_SCALE, eRange, 900, 1100, eLPM,
        eFC_ExhFlowOutsideRange)

    // start inh flow going  
//    AirFlowController->Reset(FLOW);
//    AirFlowController->SetDesired(FLOW);

    // wait for flow to stabilize
//    NU_Sleep(ONE_SECOND);

    // read the exh flow sensor
//    LONG exhFlow = S_Serv->GetFlow(ExhFlowSensor);

#ifdef NO_HARDWARE
    //  Force the test to pass
    exhFlow = 1100;
#endif

    // ensure the flow is within range
//    VERIFY_RESULT(ExhFlow100lpm, exhFlow)

    // read inh pressure sensor and save result
//    PressureConn = S_Serv->GetPressure(InhPressSensor, 10);

    // stop air flow 
//    AirFlowController->SetDesired(0);

    NextState(eST_DisconnectCircuitFromFilter);
}

//*****************************************************************************/
//    Operation Name: ST_DisconnectCircuitFromFilter()
//
//    Processing: Ask operator to disconnect the patient circuit, including the
//      condenser vial, from the heated filter. 
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
void HeatedFilterTest::ST_DisconnectCircuitFromFilter(void)
{
//    TheGUI().GetResponse(eDisconnectCircuitFromFilterMsg);
}

//*****************************************************************************/
//    Operation Name: ST_MeasureNoFilterBackpressure()
//
//    Processing: Measure the backpressure with the no filter connected. The 
//      exhalation side of the tubing will not be connected to the instrument
//      at this point.
//
//    Input Parameters:
//      *pData - a pointer the a structure containing the response from the 
//          user.
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
void HeatedFilterTest::ST_MeasureNoFilterBackpressure(ResponseData* pData)
{
    static const LONG FLOW = 1000 * LPM_FACTOR;   // 100.0LPM

    CRITERION(InhFlow100lpm, LONG, LPM_SCALE, eRange, 900, 1100, eLPM,
        eFC_InhFlowOutsideRange)
    CRITERION(HeatedFilterBackpressure, SHORT, SCALE, eRange, 500, 1500, 
        ecmH2O, eFC_HeatedFilterBackpressureRange)

    // handle the response obtained from user on disconnecting circuit 
    CHECK_RESPONSE(pData)

    // start inh flow going  
//    AirFlowController->Reset(FLOW);
//    AirFlowController->SetDesired(FLOW);

    // wait for flow to stabilize
//    NU_Sleep(ONE_SECOND);

    // read the inh flow sensor
//    LONG inhFlow = S_Serv->GetFlow(AirFlowSensor);

#ifdef NO_HARDWARE
    //  Force the test to pass
    inhFlow = 1100;
#endif

    // ensure the flow is within range
//    VERIFY_RESULT(InhFlow100lpm, inhFlow)

    // read inh pressure sensor and save result
//    PressureDisc = S_Serv->GetPressure(InhPressSensor, 10);

    // stop air flow 
//    AirFlowController->SetDesired(0);

    // calculate the heated filter backpressure
    PressureHF = PressureConn - PressureDisc - PRESSURE_SYS;

#ifdef NO_HARDWARE
    //  Force the test to pass
    PressureHF = 1500;
#endif

    // ensure the backpressure is within spec
    VERIFY_RESULT(HeatedFilterBackpressure, PressureHF)

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
void HeatedFilterTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (HeatedFilterTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&HeatedFilterTest::ST_Idle,
        (pFunc)&HeatedFilterTest::ST_Complete,
        (pFunc)&HeatedFilterTest::ST_Cancel,
        (pFunc)&HeatedFilterTest::ST_MeasureFilterBackpressure,
        (pFunc)&HeatedFilterTest::ST_DisconnectCircuitFromFilter,
        (pFunc)&HeatedFilterTest::ST_MeasureNoFilterBackpressure
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}


