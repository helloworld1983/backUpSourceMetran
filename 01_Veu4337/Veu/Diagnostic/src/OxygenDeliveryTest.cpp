//$COMMON.CPP$
//   File Name:  OxygenDeliveryTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To ensure the oxygen delivery system is operating correctly. 
//
//      The following systems are checked by this class:
//
//      Oxygen (O2) Flow Sensor 
//          Test Approach - With the patient wye blocked, compare the O2  
//              and exh flow sensor measurements and they should be equal at
//              each different flow rate.
//          
//      Oxygen (O2) Flow Valve 
//          Test Approach - At each different flow rate, the stepper position
//              should be within a certain range.
//          
//      Exhalation (Exh) Flow Sensor 
//          Test Approach - With the patient wye blocked, compare the inh  
//              and exh flow sensor measurements and they should be equal at
//              each different flow rate.
// 
//      Oxygen Concentration (Fi) Sensor
//          Test Approach - After the patient circuit is fully saturated with 
//              100% O2, measure the O2 sensor voltage. The 100% O2 voltage 
//              level must be in range. If test passes, the calibration 
//              constants will be saved to NV RAM.
//
//      These tests cannot ensure that the gauge accuracy of each flow sensor 
//      is correct, but it can ensure symmetry between the sensors. Meaning, 
//      by verifying both the O2 and exh flow measurements match a good
//      probability exists for accurate readings. In addition, verifying the 
//      O2 stepper position is within an acceptable range, we can further 
//      narrow the probability of accurate readings.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "OxygenDeliveryTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"

//#ifndef NO_HARDWARE
//    #include "DInput.h"
//    #include "Flow_Cnt.h"
//#else
//    #include "HardwareStubs.h"
//#endif

//*****************************************************************************/
//    Operation OxygenDeliveryTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *client - a pointer to the test client.
//
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
OxygenDeliveryTest::OxygenDeliveryTest(SelfTest* parent) : 
    ChildTest(parent, eOxygenDeliveryTest)
{
    Passed = eFalse;
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
void OxygenDeliveryTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_StepperRangeOfMotion,       // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_StepperRangeOfMotion
        eST_CannotHappen                // ST_OxygenFlowSymmetry
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
void OxygenDeliveryTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_StepperRangeOfMotion
        eST_Cancel                      // ST_OxygenFlowSymmetry
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
//    Operation Name: ST_Idle()
//
//    Processing: State machine sits here when not testing. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous: If tests pass, do not put the instrument into a known 
//      state (with KnownState()) because we need the O2 flow controller left 
//      at 1lpm when this test finishes.
//    Requirements:      None
//
//*****************************************************************************/
void OxygenDeliveryTest::ST_Idle(void)
{
    // IF tests did not pass
    //      put the instrument into a known state
    if (!Passed)
        S_Serv->KnownState();

    Passed = eFalse;
}

//*****************************************************************************/
//    Operation Name: ST_StepperRangeOfMotion()
//
//    Processing: Command the oxygen stepper valve to various positions from 
//      full closed to full open and ensure the motor can achieve the full range
//      of motion.
//
//      The position must be commanded 4 times because the motor controller 
//      boards are latent in returning the current position by three commands. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void OxygenDeliveryTest::ST_StepperRangeOfMotion(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
//    static const USHORT STEP_TOLERANCE = 5;
//    static const USHORT LOW_STEP_POSITION =
//        (O2Valve->Liftoff < STEP_TOLERANCE) ? STEP_TOLERANCE : O2Valve->Liftoff;
//    static const USHORT HI_STEP_POSITION = O2Valve->MaxStep;
//    static const USHORT MID_STEP_POSITION =
//        (LOW_STEP_POSITION + HI_STEP_POSITION)/2;
//
//    CRITERION(O2StepperPos0Steps, USHORT, 0, eRange,
//        (LOW_STEP_POSITION - STEP_TOLERANCE), (LOW_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_O2StepperPositionCloseOutsideRange)
//    CRITERION(O2StepperPos1000Steps, USHORT, 0, eRange,
//        (MID_STEP_POSITION - STEP_TOLERANCE), (MID_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_O2StepperPositionMidOutsideRange)
//    CRITERION(O2StepperPos2000Steps, USHORT, 0, eRange,
//        (HI_STEP_POSITION - STEP_TOLERANCE), (HI_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_O2StepperPositionOpenOutsideRange)

    // disable flow control, just want to command motor to a specific position
//    O2FlowController->Disable();

    // command a new step position
    // wait for the motor to arrive at the new position
    // The step position must be commanded three additional times to receive 
    //  ALL of the new position information.  
    // verify the motor arrived at the position we commanded
//    O2Valve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    O2Valve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(O2StepperPos0Steps, O2Valve->GetStepPosition())
//#else
//    VERIFY_RESULT(O2StepperPos0Steps, LOW_STEP_POSITION)
//#endif
//
//    O2Valve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    O2Valve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(O2StepperPos1000Steps, O2Valve->GetStepPosition())
//#else
//    VERIFY_RESULT(O2StepperPos1000Steps, MID_STEP_POSITION)
//#endif
//
//    O2Valve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    O2Valve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    O2Valve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(O2StepperPos2000Steps, O2Valve->GetStepPosition())
//#else
//    VERIFY_RESULT(O2StepperPos2000Steps, HI_STEP_POSITION)
//#endif
//
//    // turn valve off
//    O2Valve->SetStepPosition(0);
//
//    // re-enable the flow controller
//    O2FlowController->Enable();
//
//    NextState(eST_OxygenFlowSymmetry);
}

//*****************************************************************************/
//    Operation Name: ST_OxygenFlowSymmetry()
//
//    Processing: Check the O2 flow sensors at various different rates. 
//      When this test is done, leave the O2 flow at 1lpm for the 
//      OxygenSensorTest class to use. This state must output enough O2 during
//      the high flow tests so that all the air if flushed from the system and 
//      the 1lpm is only there to maintain the 100% O2. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void OxygenDeliveryTest::ST_OxygenFlowSymmetry(void)
{
    static const LONG FLOW_TOLERANCE = 80;     // 8.0 LPM
    static const LONG DIFF_TOLERANCE = 200;    // 20.0 LPM
    static const USHORT STEP_TOLERANCE = 300;  // steps
    static const UCHAR TOTAL_FLOWS = 6;
    static const LONG FLOW[TOTAL_FLOWS] = {
        0,                             //   0.0 LPM
        2000 * LPM_FACTOR,             // 200.0 LPM
        1500 * LPM_FACTOR,             // 150.0 LPM
        1000 * LPM_FACTOR,             // 100.0 LPM
        500  * LPM_FACTOR,             //  50.0 LPM
        10   * LPM_FACTOR,             //   1.0 LPM
    };

    // criterion for checking flow accuracy at each flow
    CRITERION(O2Flow0lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[0] / LPM_FACTOR) - 10, 
        (FLOW[0] / LPM_FACTOR) + 10, 
        eLPM, eFC_O2FlowOutsideRange)
    CRITERION(O2Flow200lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[1] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[1] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_O2FlowOutsideRange)
    CRITERION(O2Flow150lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[2] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[2] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_O2FlowOutsideRange)
    CRITERION(O2Flow100lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[3] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[3] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_O2FlowOutsideRange)
    CRITERION(O2Flow50lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[4] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[4] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_O2FlowOutsideRange)
    CRITERION(O2Flow1lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[5] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[5] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_O2FlowOutsideRange)

    static Criterion<LONG>* FLOW_CRITERION[TOTAL_FLOWS] = {
        O2Flow0lpm,
        O2Flow200lpm,
        O2Flow150lpm,
        O2Flow100lpm,
        O2Flow50lpm,
        O2Flow1lpm                                   };

    // criterion for checking the stepper position at each flow
    CRITERION(O2StepperPos0lpm, USHORT, 0, eRange, 
        145, 155,
        eSteps, eFC_O2StepperOutsideRange)
    CRITERION(O2StepperPos1lpm, USHORT, 0, eRange, 
        325 - STEP_TOLERANCE, 325 + STEP_TOLERANCE, 
        eSteps, eFC_O2StepperOutsideRange)
    CRITERION(O2StepperPos50lpm, USHORT, 0, eRange, 
        850 - STEP_TOLERANCE, 850 + STEP_TOLERANCE, 
        eSteps, eFC_O2StepperOutsideRange)
    CRITERION(O2StepperPos100lpm, USHORT, 0, eRange,
        1150 - STEP_TOLERANCE, 1150 + STEP_TOLERANCE, 
        eSteps, eFC_O2StepperOutsideRange)
    CRITERION(O2StepperPos150lpm, USHORT, 0, eRange,
        1400 - STEP_TOLERANCE, 1400 + STEP_TOLERANCE, 
        eSteps, eFC_O2StepperOutsideRange)
    CRITERION(O2StepperPos200lpm, USHORT, 0, eRange,
        1650 - STEP_TOLERANCE, 1650 + STEP_TOLERANCE, 
        eSteps, eFC_O2StepperOutsideRange)

    static Criterion<USHORT>* STEP_CRITERION[TOTAL_FLOWS] = {
        O2StepperPos0lpm,
        O2StepperPos200lpm,
        O2StepperPos150lpm,
        O2StepperPos100lpm,
        O2StepperPos50lpm,
        O2StepperPos1lpm
    };

    // criterion for checking the difference between the O2 and exh sensor
    // at each flow
    CRITERION(DifferenceO2VsExh0lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)
    CRITERION(DifferenceO2VsExh1lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)
    CRITERION(DifferenceO2VsExh50lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)
    CRITERION(DifferenceO2VsExh100lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)
    CRITERION(DifferenceO2VsExh150lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)
    CRITERION(DifferenceO2VsExh200lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceO2VsExh)

    static Criterion<LONG>* DIFF_CRITERION[TOTAL_FLOWS] = {
        DifferenceO2VsExh0lpm,
        DifferenceO2VsExh200lpm,
        DifferenceO2VsExh150lpm,
        DifferenceO2VsExh100lpm,
        DifferenceO2VsExh50lpm,
        DifferenceO2VsExh1lpm
    };

    // FOR each flow 
    for (UCHAR i=0; i<TOTAL_FLOWS; i++)
    {
        // start a flow going  
//        O2FlowController->Reset(FLOW[i]);
//        O2FlowController->SetDesired(FLOW[i]);

#ifndef NO_HARDWARE
        // if software jumper is not installed (meaning step mode is disabled)
//        if (SoftwareJumper->GetStateDirect())
//        {
//            // wait for oxygen flow to stabilize
//            if (S_Serv->DelayWithCancel(ONE_HUNDRED_MS * 12))
//                return;
//        }
//        else
//        {
//            // wait longer in step mode
//            if (S_Serv->DelayWithCancel(ONE_SECOND * 10))
//                return;
//        }
#endif

        // read O2 flow sensor
        // read exh flow sensor
        // read step position
//        LONG O2Flow = S_Serv->GetFlow(O2FlowSensor);
//        LONG exhFlow = S_Serv->GetFlow(ExhFlowSensor);
//        USHORT O2StepPos = O2Valve->GetStepPosition();

#ifndef NO_HARDWARE
        // ensure flow is within spec
//        VERIFY_RESULT(FLOW_CRITERION[i], O2Flow)
#endif

        // O2 and exh measurements must agree with one another
//        DIFF_CRITERION[i]->SetLimits
//        (
//            O2Flow - DIFF_TOLERANCE,
//            O2Flow + DIFF_TOLERANCE
//        );
#ifndef NO_HARDWARE
//        VERIFY_RESULT(DIFF_CRITERION[i], exhFlow)
#endif

#ifndef NO_HARDWARE
        // ensure step position is within spec
//        VERIFY_RESULT(STEP_CRITERION[i], O2StepPos)
#endif
    }

    NextState(eST_Complete);
}

//*****************************************************************************/
//    Operation Name: ST_Complete()
//
//    Processing: Test has completed successfully. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void OxygenDeliveryTest::ST_Complete(void)
{
    Passed = eTrue;

    // let parent test know we successfully completed the test
    // goto idle state
    Parent->ChildComplete(this);
    NextState(eST_Idle);
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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void OxygenDeliveryTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (OxygenDeliveryTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&OxygenDeliveryTest::ST_Idle,
        (pFunc)&OxygenDeliveryTest::ST_Complete,
        (pFunc)&OxygenDeliveryTest::ST_Cancel,
        (pFunc)&OxygenDeliveryTest::ST_StepperRangeOfMotion,
        (pFunc)&OxygenDeliveryTest::ST_OxygenFlowSymmetry
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}


