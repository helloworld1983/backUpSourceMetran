//$COMMON.CPP$
//   File Name:  ExhalationValveTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Tests for correct operation of the exhalation system by commanding
//      the system to control backpressure at various levels.
//
//      The following systems are checked by this class:
//
//      Exhalation (Exh) Valve 
//          Test Approach - With a constant air flow, command the exhalation 
//              flow controller to maintain a constant backpressure. 
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "ExhalationValveTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"

//#ifndef NO_HARDWARE
//    #include "ExhContr.h"
//    #include "DInput.h"
//    #include "Flow_Cnt.h"
//#endif

//*****************************************************************************/
//    Operation ExhalationValveTest()
//
//    Processing: Class constructor.
//
//    Input Parameters: 
//      *parent - a pointer to the parent test.
//
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
ExhalationValveTest::ExhalationValveTest(SelfTest* parent) : 
    ChildTest(parent, eExhalationValveTest)
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
void ExhalationValveTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_StepperRangeOfMotion,       // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_StepperRangeOfMotion
        eST_CannotHappen                // ST_ControlBackpressure
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
void ExhalationValveTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_StepperRangeOfMotion
        eST_Cancel                      // ST_ControlBackpressure
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
//    Processing: Command the exhalation stepper valve to various positions from 
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
void ExhalationValveTest::ST_StepperRangeOfMotion(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
//    static const USHORT STEP_TOLERANCE = 5;
//    static const USHORT LOW_STEP_POSITION =
//        (ExhValve->Liftoff < STEP_TOLERANCE) ? STEP_TOLERANCE : ExhValve->Liftoff;
//    static const USHORT HI_STEP_POSITION = ExhValve->MaxStep;
//    static const USHORT MID_STEP_POSITION =
//        (LOW_STEP_POSITION + HI_STEP_POSITION)/2;
//
//    CRITERION(ExhStepperPos0Steps, USHORT, 0, eRange,
//        (LOW_STEP_POSITION - STEP_TOLERANCE), (LOW_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_ExhStepperPositionOpenOutsideRange)
//    CRITERION(ExhStepperPos1000Steps, USHORT, 0, eRange,
//        (MID_STEP_POSITION - STEP_TOLERANCE), (MID_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_ExhStepperPositionMidOutsideRange)
//    CRITERION(ExhStepperPos2000Steps, USHORT, 0, eRange,
//        (HI_STEP_POSITION - STEP_TOLERANCE), (HI_STEP_POSITION + STEP_TOLERANCE),
//        eSteps, eFC_ExhStepperPositionCloseOutsideRange)

    // command a new step position
    // wait for the motor to arrive at the new position
    // The step position must be commanded three additional times to receive 
    //  ALL of the new position information.  
    // verify the motor arrived at the position we commanded
//    ExhValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    ExhValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(LOW_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(ExhStepperPos0Steps, ExhValve->GetStepPosition())
//#else
//    VERIFY_RESULT(ExhStepperPos0Steps, LOW_STEP_POSITION)
//#endif
//
//    ExhValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    ExhValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(MID_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(ExhStepperPos1000Steps, ExhValve->GetStepPosition())
//#else
//    VERIFY_RESULT(ExhStepperPos1000Steps, MID_STEP_POSITION)
//#endif
//
//    ExhValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(ONE_HUNDRED_MS);
//    ExhValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//    ExhValve->SetStepPosition(HI_STEP_POSITION);
//    NU_Sleep(TWENTYFIVE_MS);
//#ifndef NO_HARDWARE
//    VERIFY_RESULT(ExhStepperPos2000Steps, ExhValve->GetStepPosition())
//#else
//    VERIFY_RESULT(ExhStepperPos2000Steps, HI_STEP_POSITION)
//#endif
//
//    NextState(eST_ControlBackpressure);
}

//*****************************************************************************/
//    Operation Name: ST_ControlBackpressure()
//
//    Processing: Control the exhalation controller to maintain different 
//      backpressure levels. Read the exh sensor to ensure the system 
//      accurately produced the desired pressure levels.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    Air flow must already be established.
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void ExhalationValveTest::ST_ControlBackpressure(void)
{
    static const LONG BACKPRESSURE_TOLERANCE = 200;   // 2.0 cmH2O
    static const LONG FLOW = 10 * LPM_FACTOR;         // 1.0 LPM
    static const UCHAR TOTAL_BACKPRESSURES = 6;
    static const SHORT BACKPRESSURE[TOTAL_BACKPRESSURES] = {
        0 * SCALE,    //  0.0 cmH2O
        5 * SCALE,    //  5.0 cmH2O
        10 * SCALE,   // 10.0 cmH2O
        20 * SCALE,   // 20.0 cmH2O
        30 * SCALE,   // 30.0 cmH2O
        35 * SCALE    // 35.0 cmH2O
    };

    // criterion for checking backpressure accuracies 
    CRITERION(ExhPressure0cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[0] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[0] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)
    CRITERION(ExhPressure5cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[1] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[1] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)
    CRITERION(ExhPressure10cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[2] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[2] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)
    CRITERION(ExhPressure20cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[3] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[3] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)
    CRITERION(ExhPressure30cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[4] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[4] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)
    CRITERION(ExhPressure35cmH2O, SHORT, SCALE, eRange, 
        BACKPRESSURE[5] - BACKPRESSURE_TOLERANCE, 
        BACKPRESSURE[5] + BACKPRESSURE_TOLERANCE, 
        ecmH2O, eFC_ExhPressureOutsideRange)

    static Criterion<SHORT>* BACKPRESSURE_CRITERION[TOTAL_BACKPRESSURES] = {
        ExhPressure0cmH2O,
        ExhPressure5cmH2O,
        ExhPressure10cmH2O,
        ExhPressure20cmH2O,
        ExhPressure30cmH2O,
        ExhPressure35cmH2O,
    };

    // start a flow going  
//    AirFlowController->Reset(FLOW);
//    AirFlowController->SetDesired(FLOW);

    // enable the exh controller
//    ExhController->Enable();

    // FOR each backpressure 
//    for (UCHAR i=0; i<TOTAL_BACKPRESSURES; i++)
//    {
        // set a desired backpressure  
//        ExhController->SetDesired(BACKPRESSURE[i]);

#ifndef NO_HARDWARE
        // if software jumper is not installed (meaning step mode is disabled)
//        if (SoftwareJumper->GetStateDirect())
//        {
//            // wait for pressure to stabilize
//            if (S_Serv->DelayWithCancel(ONE_SECOND * 3))
//                return;
//        }
//        else
//        {
//            // wait longer in step mode
//            if (S_Serv->DelayWithCancel(ONE_SECOND * 10))
//                return;
//        }
#endif

        // read exh pressure sensor
//        SHORT exhPressure = S_Serv->GetPressure(ExhPressSensor);

//#ifndef NO_HARDWARE
        // ensure backpressure is within spec
//        VERIFY_RESULT(BACKPRESSURE_CRITERION[i], exhPressure)
//#endif
//    }

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
void ExhalationValveTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (ExhalationValveTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&ExhalationValveTest::ST_Idle,
        (pFunc)&ExhalationValveTest::ST_Complete,
        (pFunc)&ExhalationValveTest::ST_Cancel,
        (pFunc)&ExhalationValveTest::ST_StepperRangeOfMotion,
        (pFunc)&ExhalationValveTest::ST_ControlBackpressure
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}




