//$COMMON.CPP$
//   File Name:  PressureReliefValveTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Check the pressure relief valve to ensure it opens within the 
//      correct pressure range.
//
//      The following systems are checked by this class:
//
//      Pressure Relief Valve
//          Test Approach - Check the cracking pressure to ensure the valve 
//              opens at the correct pressure level. Use the O2 source to
//              provide the high pressure required to open the relief valve.
//              Take the stabilized pressure measurements and verify they
//              are within acceptable ranges.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "PressureReliefValveTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "mathutil.h"
//
//#ifndef NO_HARDWARE
//#include "ExhContr.h"
//#include "Flow_Cnt.h"
//#endif

#include "DiagnosticTask.h"

//*****************************************************************************/
//    Operation PressureReliefValveTest()
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
PressureReliefValveTest::PressureReliefValveTest(SelfTest* parent) : 
    ChildTest(parent, ePressureReliefValveTest)
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
void PressureReliefValveTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CrackFlowPressure,          // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen                // ST_CrackFlowPressure
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
void PressureReliefValveTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel                      // ST_CrackFlowPressure
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
//    Operation Name: ST_CrackFlowPressure()
//
//    Processing: Check the pressure relief valve's pressure when just enough
//      flow is produced to just crack the valve open.
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
void PressureReliefValveTest::ST_CrackFlowPressure(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const LONG FLOW = 10 * LPM_FACTOR;   // 1.0 lpm
    static const SHORT STABILITY = 200;         // 2.00 cm H2O
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 60;
    SHORT pressure = 0;
    E_Bool stable = eFalse;
    E_Bool exceeded = eTrue;

    CRITERION(CrackFlowPressureExceeded, int, 0, eEqual, eFalse, 0, eNoString, 
        eFC_ReliefValveCrackFlowPressureTooHigh)
    CRITERION(CrackFlowPressureStability, int, 0, eEqual, eTrue, 0, eNoString, 
        eFC_ReliefValveCrackFlowPressureNotStable)
    CRITERION(CrackFlowPressure, SHORT, SCALE, eRange, 12000, 16000, ecmH2O,
        eFC_ReliefValveCrackFlowPressureOutsideRange)
    CRITERION(ReliefValveCrackFlow, LONG, LPM_SCALE, eRange, 7, 13, eLPM,
        eFC_O2FlowCrackFlowOutsideRange)

    // get a stable pressure reading which will be the pressure required to
    // just crack open the relief valve
    E_Bool cancelDetected = GetStabilizedPressure
        (
            FLOW, 
            STABILITY, 
            TIME_LIMIT, 
            ONE_SECOND,
            &pressure,
            &stable,
            &exceeded
        );

#ifndef NO_HARDWARE
    // if a cancel event was detected, then just abort this test
    if (cancelDetected)
        return;
#endif

    // read current O2 flow
//    LONG O2Flow = S_Serv->GetFlow(O2FlowSensor);

    // open the exh valve release pressure
//    ExhController->Open();

#ifdef USE_NUCLEUS
    // wait for pressure to dissipate
    NU_Sleep(FIVE_HUNDRED_MS);
#endif
    
#ifdef NO_HARDWARE
    //  Force the tests to pass
    exceeded = eFalse;
    O2Flow = 13;
    stable = eTrue;
    pressure = 16000;
#endif
    // ensure pressure was not exceeded during the get stable pressure
    // ensure flow is exhausting out the relief valve
    // ensure the pressure reading was stable
    // ensure the cracking pressure is within acceptable limits
    VERIFY_RESULT(CrackFlowPressureExceeded, exceeded)
//    VERIFY_RESULT(ReliefValveCrackFlow, O2Flow)
    VERIFY_RESULT(CrackFlowPressureStability, stable)
    VERIFY_RESULT(CrackFlowPressure, pressure)

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
void PressureReliefValveTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (PressureReliefValveTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&PressureReliefValveTest::ST_Idle,
        (pFunc)&PressureReliefValveTest::ST_Complete,
        (pFunc)&PressureReliefValveTest::ST_Cancel,
        (pFunc)&PressureReliefValveTest::ST_CrackFlowPressure
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}

//*****************************************************************************/
//    Operation Name: GetStabilizedPressure()
//
//    Processing: Start a flow and take pressure samples until the pressure
//      readings stabilize.
//
//    Input Parameters:
//      flow - the flow rate to set the O2 controller at.
//      stability - the pressure range the pressure must stabilize within.
//      timeLimit - the maximum time for allowing the pressure to stabilize.
//      sampleInterval - the time between pressure reads.
//
//    Output Parameters:
//      *pressure - the stabilized pressure is returned via this pointer.
//      *stable - true if pressure stabilized, false if a stable pressure could
//          not be attained.
//      *exceeded - true if pressure exceeded the max, false otherwise.
//
//    Return Values:
//      E_Bool - true if a cancel was detected during routine, false otherwise.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
E_Bool PressureReliefValveTest::GetStabilizedPressure
    (
        LONG flow,
        SHORT stability,
        UNSIGNED timeLimit,
        UNSIGNED sampleInterval,
        SHORT *pressure,
        E_Bool *stable,
        E_Bool *exceeded
    )
{
    static const UCHAR SAMPLES = 3;
    static const SHORT PRESSURE_LIMIT = 17000;  // 170.00 cmH2O
    SHORT pressureSamples[SAMPLES] = {0};       // init all elements to 0

    E_Bool rollOver = eFalse;
    *stable = eFalse;
    *exceeded = eFalse;

    // close the exh valve to trap O2 within patient circuit
//    ExhController->Close();

    // start O2 flow going to build up pressure
//    O2FlowController->Reset(flow);
//    O2FlowController->SetDesired(flow);

    // time stamp the starting time
//    UNSIGNED startTime = SystemClock::Retrieve();

    UCHAR sampleCnt = 0;
    // WHILE time limit has not expired
//    while (S_Serv->ElapsedTime(startTime) < timeLimit)
//    {
//        SHORT min = 0;
//        SHORT max = 0;

//#ifdef USE_NUCLEUS
        // delay a little before each sample. if read too fast could get a 
        // false indication of a series of stable readings
//        NU_Sleep(sampleInterval);
//#endif

        // get the exh pressure sensor reading
//        pressureSamples[sampleCnt] = S_Serv->GetPressure(ExhPressSensor);

        // counter increment and roll over code
//        if (++sampleCnt >= SAMPLES)
//        {
//            sampleCnt = 0;
//            rollOver = eTrue;
//        }
//
//        // calc the minimum and maximum readings
//        for (UCHAR i=0; i<SAMPLES; i++)
//        {
//            if (i == 0)
//                min = max = pressureSamples[i];
//            else
//            {
//                min = pressureSamples[i] < min ? pressureSamples[i] : min;
//                max = pressureSamples[i] > max ? pressureSamples[i] : max;
//            }
//        }
//
//        // IF the min and max readings are within the stability range AND
//        // enough samples have been taken to fill the array
//        //      we have attained a stable pressure (good)
//        if (USHORT(max - min) <= stability &&
//            rollOver)
//        {
//            *stable = eTrue;
//            break;
//        }
//
//        // IF pressure exceeds the maximum
//        //      we exceeded the max allowable pressure (bad)
//        if (max > PRESSURE_LIMIT)
//        {
//            *exceeded = eTrue;
//            break;
//        }
        
//        // check for a cancel event being set
//#ifdef USE_NUCLEUS
//        STATUS status = TheDiagnosticTask().RetrieveCancelEvent();
//        if (status == NU_NOT_PRESENT) {}
//        else if (status == NU_SUCCESS)
//            return eTrue;
//        else
//            ASSERTION(0);
//#else
//        return eTrue;
//#endif
//    }
//
//    // compute the average pressure
//    UCHAR samplesTaken = rollOver ? SAMPLES : sampleCnt;
////    for (UCHAR i=0; i<samplesTaken; i++)
////        *pressure = avg(*pressure, pressureSamples[i], i+1);
//
//    return eFalse;
}
