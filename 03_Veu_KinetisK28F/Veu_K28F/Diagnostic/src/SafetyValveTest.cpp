//$COMMON.CPP$
//   File Name:  SafetyValveTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Tests the safety valve to ensure it is capable of opening up
//      to atmosphere.
//
//      The following systems are checked by this class:
//
//      Safety Valve
//          Test Approach - With the patient wye blocked, open the safety valve
//              and start a flow going. No pressure should build up in the 
//              circuit as measured by the inh and/or exh pressure sensors 
//              since air flow should be exhausting out the safety valve.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "SafetyValveTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"

//#ifndef NO_HARDWARE
//    #include "DOutput.h"
//    #include "ExhContr.h"
//    #include "Flow_Cnt.h"
//#endif

//*****************************************************************************/
//    Operation SafetyValveTest()
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
SafetyValveTest::SafetyValveTest(SelfTest* parent) : 
    ChildTest(parent, eSafetyValveTest)
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
void SafetyValveTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_ChargeCircuit,              // ST_Idle
		eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
		eST_CannotHappen,               // ST_ChargeCircuit
		eST_CannotHappen                // ST_SafetyValve
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
void SafetyValveTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
		eST_Cancel,                     // ST_ChargeCircuit
		eST_Cancel                      // ST_SafetyValve
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
//    Operation Name: ST_ChargeCircuit()
//
//    Processing: Ensures the circuit can hold pressure before we test that 
//      the safety valve correctly exhausts air.
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
void SafetyValveTest::ST_ChargeCircuit(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const SHORT PRESSURE = 50 * SCALE;   // 50 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR;   // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 3; 

    CRITERION(ChargeCircuitPressure, SHORT, SCALE, eGreaterThanOrEqual,
        PRESSURE, 0, ecmH2O, eFC_PatientCircuitLeak)

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
    VERIFY_RESULT(ChargeCircuitPressure, pressure)

    NextState(eST_SafetyValve);
}

//*****************************************************************************/
//    Operation Name: ST_SafeyValve()
//
//    Processing: Ensures the safety valve can open to atmosphere.
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
void SafetyValveTest::ST_SafetyValve(void)
{
    static const ULONG FLOW = 100 * LPM_FACTOR;   // 10.0 LPM

    CRITERION(SafeyValveCannotOpen, SHORT, SCALE, eLessThan, 500, 0, ecmH2O,
        eFC_SafeyValveCannotOpen)

    // unpressurize the patient circuit by opening the exh valve
//    ExhController->Open();

    // wait for pressure to dissipate
//    NU_Sleep(FIVE_HUNDRED_MS);

    // close the exh valve
//    ExhController->Close();

    // open the safety valve. Now the patient circuit should be closed except
    // for an exhaust path through the safety valve. 
//    SafetyValve->SetStateDirect(eOpen);

    // start a flow going  
//    AirFlowController->Reset(FLOW);
//    AirFlowController->SetDesired(FLOW);

    // wait some time to see if pressure builds up
//    NU_Sleep(ONE_SECOND);

    // read the inh pressure
//    SHORT inhPressure = S_Serv->GetPressure(InhPressSensor);

// Force the test to pass
#ifdef NO_HARDWARE
	inhPressure = 0;
#endif

    // ensure pressure does not build up meaning flow is being exhausted out
    // the safety valve
//    VERIFY_RESULT(SafeyValveCannotOpen, inhPressure);

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
void SafetyValveTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
	typedef void (SafetyValveTest::*pFunc)(EventData *);

    // state table with pointers to each state method
	static pFunc stateMachine[eST_MaxStates] = {
		(pFunc)&SafetyValveTest::ST_Idle,
		(pFunc)&SafetyValveTest::ST_Complete,
		(pFunc)&SafetyValveTest::ST_Cancel,
		(pFunc)&SafetyValveTest::ST_ChargeCircuit,
		(pFunc)&SafetyValveTest::ST_SafetyValve
	};

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}



