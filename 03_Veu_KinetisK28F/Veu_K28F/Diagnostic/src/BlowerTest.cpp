//$COMMON.CPP$
//   File Name:  BlowerTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Tests whether the blower is capable of being shut off by software.
//
//      The following systems are checked by this class:
//
//      Blower
//          Test Approach - With the blower turned off, the exh valve closed,
//              and patient wye blocked, start a flow going. No air flow 
//              should be measured with the blower off. Test both the DAC
//              and digital switch to see that they can independently turn 
//              the blower off. 
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "BlowerTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"

//#ifndef NO_HARDWARE
//    #include "Blower.h"
//#endif

const LONG BlowerTest::BLOWER_SPINDOWN_FLOW = 500 * LPM_FACTOR;

//*****************************************************************************/
//    Operation BlowerTest()
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
BlowerTest::BlowerTest(SelfTest* parent) : 
    ChildTest(parent, eBlowerTest)
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
void BlowerTest::Start(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_BlowerShutoffSwitch,        // ST_Idle
		eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
		eST_CannotHappen,               // ST_BlowerShutoffSwitch
		eST_CannotHappen                // ST_BlowerShutoffDAC
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
void BlowerTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
		eST_EventIgnored,               // ST_Idle,
        eST_CannotHappen,               // ST_Complete
		eST_CannotHappen,               // ST_Cancel
		eST_Cancel,                     // ST_BlowerShutoffSwitch
		eST_Cancel                      // ST_BlowerShutoffDAC
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
//    Operation Name: ST_BlowerShutoffSwitch()
//
//    Processing: Test to see if the blower can be shut off using the blower
//      digital switch. The DAC, which controls the power applied to the motor,
//      should be set for full speed and the digital switch off. The flow 
//      sensors should not measure any flow with the blower off.
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
void BlowerTest::ST_BlowerShutoffSwitch(void)
{
    CRITERION(InhFlowBlowerOffSwitch, LONG, LPM_SCALE, eLessThan, 
        BLOWER_SPINDOWN_FLOW / LPM_FACTOR, 0, eLPM, eFC_BlowerNotOffSwitch)

    // turn the blower off (the DAC volatge should still be on)
//    TheBlower->SwitchOff();

    // shutdown the blower to at least the spin down flow
    LONG blowerOffFlow;
    if (S_Serv->BlowerSpinDown(BLOWER_SPINDOWN_FLOW, &blowerOffFlow))
        return;

    // if blower is indeed off, then no flow should be measured
    VERIFY_RESULT(InhFlowBlowerOffSwitch, blowerOffFlow)

    NextState(eST_BlowerShutoffDAC);
}

//*****************************************************************************/
//    Operation Name: ST_BlowerShutoffDAC()
//
//    Processing: Test to see if the blower can be shut off using the DAC.
//      The DAC, which controls the voltage applied to the motor, should be set 
//      all the down and the digital switch on. The flow sensors should not 
//      measure any flow with the blower off.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous: Blower must already be spun down and air flow controller
//      on and trying to blow.
//
//    Requirements:  
//
//*****************************************************************************/
void BlowerTest::ST_BlowerShutoffDAC(void)
{
    CRITERION(InhFlowBlowerOffDAC, LONG, LPM_SCALE, eLessThan, 
       BLOWER_SPINDOWN_FLOW / LPM_FACTOR, 0, eLPM, eFC_BlowerNotOffDAC)

    // set the power applied to the motor to 0
    // turn the blower's digital switch on
//    TheBlower->JumpToSpeed(0);
//    TheBlower->SwitchOn();

#ifdef USE_NUCLEUS
    // wait long enough for flow to start if failure occurs and blower kicks on
    NU_Sleep(ONE_SECOND);
#endif

    // shutdown the blower to at least the spin down flow
    LONG blowerOffFlow;
    if (S_Serv->BlowerSpinDown(BLOWER_SPINDOWN_FLOW, &blowerOffFlow))
        return;

    // if blower is indeed off, then no flow should be measured
    VERIFY_RESULT(InhFlowBlowerOffDAC, blowerOffFlow)

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
void BlowerTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
	typedef void (BlowerTest::*pFunc)(EventData *);

    // state table with pointers to each state method
	static pFunc stateMachine[eST_MaxStates] = {
		(pFunc)&BlowerTest::ST_Idle,
		(pFunc)&BlowerTest::ST_Complete,
		(pFunc)&BlowerTest::ST_Cancel,
		(pFunc)&BlowerTest::ST_BlowerShutoffSwitch,
		(pFunc)&BlowerTest::ST_BlowerShutoffDAC
	};

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}



