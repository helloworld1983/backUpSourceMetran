//$COMMON.CPP$
//   File Name:  AirDeliveryTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To ensure the air delivery system is operating correctly. 
//
//      The following systems are checked by this class:
//
//      Air (Inh) Flow Sensor 
//          Test Approach - With the patient wye blocked, compare the inh  
//              and exh flow sensor measurements and they should be equal at
//              each different flow rate.
//          
//      Air (Inh) Flow Valve 
//          Test Approach - At each different flow rate, the stepper position
//              should be within a certain range. Also test the range of 
//              positions from closed to open to ensure the motor can achieve
//              the full range of motion.
//          
//      Exhalation (Exh) Flow Sensor 
//          Test Approach - With the patient wye blocked, compare the inh  
//              and exh flow sensor measurements and they should be equal at
//              each different flow rate.
//
//      These tests cannot ensure that the gauge accuracy of each flow sensor 
//      is correct, but it can ensure symmetry between the sensors. Meaning, 
//      by verifying both the inh and exh flow measurements match a good
//      probability exists for accurate readings. In addition, verifying the 
//      inh stepper position is within an acceptable range, we can further 
//      narrow the probability of accurate readings.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "AirDeliveryTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"

//#ifndef NO_HARDWARE
//    #include "DualKey.h"
//    #include "Flow_Cnt.h"
//#endif

//*****************************************************************************/
//    Operation AirDeliveryTest()
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
AirDeliveryTest::AirDeliveryTest(SelfTest* parent) : 
    ChildTest(parent, eAirDeliveryTest)
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
void AirDeliveryTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_StepperRangeOfMotion,       // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_StepperRangeOfMotion
        eST_CannotHappen                // ST_AirFlowSymmetry
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
void AirDeliveryTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_StepperRangeOfMotion
        eST_Cancel                      // ST_AirFlowSymmetry
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
//    Operation Name: ST_StepperRangeOfMotion()
//
//    Processing: Command the air stepper valve to various positions from full 
//      closed to full open and ensure the motor can achieve the full range
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
void AirDeliveryTest::ST_StepperRangeOfMotion(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
//    static const USHORT STEP_TOLERANCE = 5;
//    static const USHORT LOW_STEP_POSITION =
//        (AirValve->Liftoff < STEP_TOLERANCE) ? STEP_TOLERANCE : AirValve->Liftoff;
//    static const USHORT HI_STEP_POSITION = AirValve->MaxStep;
//    static const USHORT MID_STEP_POSITION =
//        (LOW_STEP_POSITION + HI_STEP_POSITION)/2;

//    CRITERION(InhStepperPos0Steps, USHORT, 0, eRange,
//        (LOW_STEP_POSITION - STEP_TOLERANCE), (LOW_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_InhStepperPositionCloseOutsideRange)
//    CRITERION(InhStepperPos1000Steps, USHORT, 0, eRange,
//        (MID_STEP_POSITION - STEP_TOLERANCE), (MID_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_InhStepperPositionMidOutsideRange)
//    CRITERION(InhStepperPos2000Steps, USHORT, 0, eRange,
//        (HI_STEP_POSITION - STEP_TOLERANCE), (HI_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_InhStepperPositionOpenOutsideRange)

    // disable flow control, just want to command motor to a specific position
//    AirFlowController->Disable();

    // command a new step position
    // wait for the motor to arrive at the new position
    // The step position must be commanded three additional times to receive 
    //  ALL of the new position information.  
    // verify the motor arrived at the position we commanded
//    AirValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    AirValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
#ifndef NO_HARDWARE
//    VERIFY_RESULT(InhStepperPos0Steps, AirValve->GetStepPosition())
#else
//    VERIFY_RESULT(InhStepperPos0Steps, LOW_STEP_POSITION)
#endif
//    AirValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    AirValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
#ifndef NO_HARDWARE
//    VERIFY_RESULT(InhStepperPos1000Steps, AirValve->GetStepPosition())
#else
//    VERIFY_RESULT(InhStepperPos1000Steps, MID_STEP_POSITION)
#endif

//    AirValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    AirValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    AirValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
#ifndef NO_HARDWARE
//    VERIFY_RESULT(InhStepperPos2000Steps, AirValve->GetStepPosition())
#else
//    VERIFY_RESULT(InhStepperPos2000Steps, HI_STEP_POSITION)
#endif

    // close valve
//    AirValve->SetStepPosition(0);

    // re-enable the flow controller 
//    AirFlowController->Enable();

//    NextState(eST_AirFlowSymmetry);
}

//*****************************************************************************/
//    Operation Name: ST_AirFlowSymmetry()
//
//    Processing: Checks the symmetry between the inh and exh flow sensors
//      at 5 different flow rates. 0, 50, 100, 150, and 200LPM. 
//
//      At each flow, the test must pass three test criteria:
//      1) The commanded flow is within specification as read by the inh flow
//         sensor.
//      2) The step position of the inh flow valve is within allowable range.
//      3) The difference between the inh and exh flow sensors must be within
//         specification.
//
//      NOTE: Adjusting the blower speed will cause the nominal step positions
//      to change. Having a higher blower speed makes more pressure behind
//      the air valve meaning the valve needs to open less for a given flow. 
//      Conversely, a slowed blower makes the valve open wider. So, if the 
//      blower speed (i.e. blower voltage) changes, these step tolerances need
//      to be reviewed. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AirDeliveryTest::ST_AirFlowSymmetry(void)
{
    static const LONG FLOW_TOLERANCE = 80;     // 8.0 LPM
    static const LONG DIFF_TOLERANCE = 200;    // 20.0 LPM
    static const USHORT STEP_TOLERANCE = 300;  // steps
    static const UCHAR TOTAL_FLOWS = 5;
    static const ULONG FLOW[TOTAL_FLOWS] = {
        0,                   //   0.0 LPM
        500 * LPM_FACTOR,    //  50.0 LPM
        1000 * LPM_FACTOR,   // 100.0 LPM
        1500 * LPM_FACTOR,   // 150.0 LPM 
        2000 * LPM_FACTOR    // 200.0 LPM  
    };

    // criterion for checking flow accuracy at each flow
    CRITERION(InhFlow0lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[0] / LPM_FACTOR) - 10, 
        (FLOW[0] / LPM_FACTOR) + 10, 
        eLPM, eFC_InhFlowOutsideRange)
    CRITERION(InhFlow50lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[1] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[1] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_InhFlowOutsideRange)
    CRITERION(InhFlow100lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[2] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[2] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_InhFlowOutsideRange)
    CRITERION(InhFlow150lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[3] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[3] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_InhFlowOutsideRange)
    CRITERION(InhFlow200lpm, LONG, LPM_SCALE, eRange, 
        (FLOW[4] / LPM_FACTOR) - FLOW_TOLERANCE, 
        (FLOW[4] / LPM_FACTOR) + FLOW_TOLERANCE, 
        eLPM, eFC_InhFlowOutsideRange)

    static Criterion<LONG>* FLOW_CRITERION[TOTAL_FLOWS] = {
        InhFlow0lpm,
        InhFlow50lpm,
        InhFlow100lpm,
        InhFlow150lpm,
        InhFlow200lpm
    };

    // criterion for checking the stepper position at each flow
    CRITERION(InhStepperPos0lpm, USHORT, 0, eRange, 
        145, 155, 
        eSteps, eFC_InhStepperOutsideRange)
    CRITERION(InhStepperPos50lpm, USHORT, 0, eRange, 
        600 - STEP_TOLERANCE, 600 + STEP_TOLERANCE, 
        eSteps, eFC_InhStepperOutsideRange)
    CRITERION(InhStepperPos100lpm, USHORT, 0, eRange, 
        825 - STEP_TOLERANCE, 825 + STEP_TOLERANCE, 
        eSteps, eFC_InhStepperOutsideRange)
    CRITERION(InhStepperPos150lpm, USHORT, 0, eRange,  
        1025 - STEP_TOLERANCE, 1025 + STEP_TOLERANCE, 
        eSteps, eFC_InhStepperOutsideRange)
    CRITERION(InhStepperPos200lpm, USHORT, 0, eRange,  
        1300 - STEP_TOLERANCE, 1300 + STEP_TOLERANCE, 
        eSteps, eFC_InhStepperOutsideRange)

    static Criterion<USHORT>* STEP_CRITERION[TOTAL_FLOWS] = {
        InhStepperPos0lpm,
        InhStepperPos50lpm,
        InhStepperPos100lpm,
        InhStepperPos150lpm,
        InhStepperPos200lpm
    };

    // criterion for checking the difference between the inh and exh sensor
    // at each flow
    CRITERION(DifferenceInhVsExh0lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceInhVsExh)
    CRITERION(DifferenceInhVsExh50lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceInhVsExh)
    CRITERION(DifferenceInhVsExh100lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceInhVsExh)
    CRITERION(DifferenceInhVsExh150lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceInhVsExh)
    CRITERION(DifferenceInhVsExh200lpm, LONG, LPM_SCALE, eRange, 0, 0, eLPM,
        eFC_DifferenceInhVsExh)

    static Criterion<LONG>* DIFF_CRITERION[TOTAL_FLOWS] = {
        DifferenceInhVsExh0lpm,
        DifferenceInhVsExh50lpm,
        DifferenceInhVsExh100lpm,
        DifferenceInhVsExh150lpm,
        DifferenceInhVsExh200lpm
    };

    // FOR each flow 
    for (UCHAR i=0; i<TOTAL_FLOWS; i++)
    {
        // start a flow going  
//        AirFlowController->Reset(FLOW[i]);
//        AirFlowController->SetDesired(FLOW[i]);

#ifndef NO_HARDWARE
        // if software jumper is not installed (meaning step mode is disabled)
//        if (SoftwareJumper->GetStateDirect())
//        {
//            // wait for air flow to stabilize
//            if (S_Serv->DelayWithCancel(ONE_HUNDRED_MS * 10))
//                return;
//        }
//        else
//        {
//            // wait longer in step mode
//            if (S_Serv->DelayWithCancel(ONE_SECOND * 10))
//                return;
//        }
#endif
            
        // read inh flow sensor
        // read exh flow sensor
        // read step position
//        LONG inhFlow = S_Serv->GetFlow(AirFlowSensor);
//        LONG exhFlow = S_Serv->GetFlow(ExhFlowSensor);
//        USHORT inhStepPos = AirValve->GetStepPosition();

#ifndef NO_HARDWARE
        // ensure flow is within spec
//        VERIFY_RESULT(FLOW_CRITERION[i], inhFlow)
#endif

        // inh and exh measurements must agree with one another
//        DIFF_CRITERION[i]->SetLimits
//        (
//            inhFlow - DIFF_TOLERANCE,
//            inhFlow + DIFF_TOLERANCE
//        );
#ifndef NO_HARDWARE
//        VERIFY_RESULT(DIFF_CRITERION[i], exhFlow)
#endif

#ifndef NO_HARDWARE
        // ensure step position is within spec
//        VERIFY_RESULT(STEP_CRITERION[i], inhStepPos)
#endif
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
//
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void AirDeliveryTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (AirDeliveryTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&AirDeliveryTest::ST_Idle,
        (pFunc)&AirDeliveryTest::ST_Complete,
        (pFunc)&AirDeliveryTest::ST_Cancel,
        (pFunc)&AirDeliveryTest::ST_StepperRangeOfMotion,
        (pFunc)&AirDeliveryTest::ST_AirFlowSymmetry
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}


