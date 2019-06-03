//$COMMON.CPP$
//   File Name:  PatientCircuitTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Verifies the patient circuit integrity following the change
//      of a patient circuit. This includes testing for leaks and a measurement 
//      of patient circuit compliance. 
//
//      The following systems are checked by this class:
//
//      Inhalation (Inh) Pressure Sensor
//          Test Approach - At each pressure measurement compare the inh and
//              exh pressure sensor measurements and they should be equal.
//
//      Exhalation (Exh) Pressure Sensor
//          Test Approach - At each pressure measurement compare the inh and
//              exh pressure sensor measurements and they should be equal.
//
//      Check Valve 3 (CV3)
//          Test Approach - With the system pressurized, open the safety valve
//              which relieves the pressure upstream of CV3. The one way check 
//              valve should keep the system pressurized as measured by the inh 
//              and/or exh pressure sensor(s).
//
//      These tests cannot ensure that the gauge accuracy of each pressure 
//      sensor is correct, but it can ensure symmetry between the sensors. 
//      Meaning, by verifying both the inh and exh pressure measurements match 
//      a good probability exists for accurate readings. 
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "PatientCircuitTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"
#include "DiagnosticTask.h"
//#include "mathutil.h"
//
//#ifndef DIAG_PC
//#ifndef NO_HARDWARE
//    #include "Tasks.h"
//    #include "NVRAM.h"
//    #include "DOutput.h"
//    #include "Flow_Cnt.h"
//    #include "PresSens.h"
//#else
//    #include "Uart.h"
//#endif
//#endif
                                             
//*****************************************************************************/
//    Operation PatientCircuitTest()
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
PatientCircuitTest::PatientCircuitTest(SelfTest* parent) : 
    ChildTest(parent, ePatientCircuitTest)
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
void PatientCircuitTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_ChargePatientCircuit,       // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_ChargePatientCircuit
        eST_CannotHappen,               // ST_TestCircuitCompliance
        eST_CannotHappen                // ST_CheckValve3
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
void PatientCircuitTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_ChargePatientCircuit
        eST_Cancel,                     // ST_TestCircuitCompliance
        eST_Cancel                      // ST_CheckValve3
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
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::ST_Idle(void)
{
#ifdef USE_NUCLEUS
    // put task priority back to the original value
    TheDiagnosticTask().ChangePriority(tasks[eDiagnostics].Pri); 
#endif

    // put the instrument into a known state
    S_Serv->KnownState();
}

//*****************************************************************************/
//    Operation Name: ST_ChargePatientCircuit()
//
//    Processing: Pressurize the patient circuit.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::ST_ChargePatientCircuit(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    static const SHORT PRESSURE = 50 * SCALE;   // 50 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR;   // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 10; 

    CRITERION(ChargeCircuitPressure, SHORT, SCALE, eGreaterThanOrEqual,
        PRESSURE, 0, ecmH2O, eFC_PatientCircuitLeak)

    // wait for the opened exh valve to fully dissipate any pressure
//    NU_Sleep(FIVE_HUNDRED_MS);

#ifdef USE_NUCLEUS
    // bump up the DiagnosticTask priority above GUITask temporarily while 
    // we do the timing for circuit compliance. This prevents a large GUI 
    // processing load from potentially throwing off the timing
    TheDiagnosticTask().ChangePriority(tasks[eGui].Pri-1); 
#endif

    // charge the circuit using the air controller
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            AirFlowController,
//            FLOW,
//            TIME_LIMIT,
//            PRESSURE,
//            &pressure,
//            &Volume_Compl
//        );

    // time stamp the start time
//    StartTime = SystemClock::Retrieve();

    // verify the correct pressure was attained
    VERIFY_RESULT(ChargeCircuitPressure, pressure)

    NextState(eST_TestCircuitCompliance);
}

//*****************************************************************************/
//    Operation Name: ST_TestCircuitCompliance()
//
//    Processing: Test the patient circuit compliance. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions: 
//      Patient circuit must already be pressurized.
//    Miscellaneous: 
//      Ideally the pressure at .1S will always be greater than the 1.1S reading.
//      Since the pressure is trapped within the circuit it can only leak out, 
//      meaning pressure falls over time. However, if the system is very leak 
//      tight then the measurments might be very close together after only 1S 
//      of time. Therefore, noise from sensor or pneumatics may make the 1.1S 
//      sightly greater than the .1S. To account for this, the Pressure_Leak 
//      lower tolerance is set below 0.
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::ST_TestCircuitCompliance(void)
{
    static const USHORT MIN_INH_PRESSURE = 4500; // < Minimum acceptable (cmH2O)
    static const USHORT MIN_COMPLIANCE = 50;     // Minimum acceptable (ml/cmH2O)
    static const USHORT MAX_COMPLIANCE = 900;    // Maximum acceptable (ml/cmH2O)
    static const USHORT PRESSURE_VARIANCE = 300; // Allowed variance (cmH2O)

    CRITERION(InhPressureTooLow, SHORT, SCALE, eGreaterThan, MIN_INH_PRESSURE, 
        0, ecmH2O, eFC_InhPressureTooLow)
    CRITERION(DifferenceInhVsExh01, SHORT, SCALE, eRange, 0, 0, ecmH2O,
        eFC_DifferenceInhVsExh)
    CRITERION(DifferenceInhVsExh11, SHORT, SCALE, eRange, 0, 0, ecmH2O,
        eFC_DifferenceInhVsExh)
    CRITERION(PressureLeak, LONG, SCALE, eRange, 0, 0, ecmH2O, 
        eFC_PressureLeakRange)
    CRITERION(CircuitCompliance, LONG, SCALE, eRange, MIN_COMPLIANCE, 
        MAX_COMPLIANCE, emlcmH2O, eFC_CircuitComplianceRange)

    // delay using the StartTime as a reference point
    S_Serv->DelayNoSleep(ONE_HUNDRED_MS, StartTime);
    
    // read inh flow sensor
    // read exh flow sensor
    SHORT min;
    SHORT max;
//    SHORT inhPressure = S_Serv->GetPressure(InhPressSensor, 5, &min, &max);
//    SHORT exhPressure = S_Serv->GetPressure(ExhPressSensor);

    // minimum measurement must be above tolerance, otherwise leak in system
    // or sensor problem
    VERIFY_RESULT(InhPressureTooLow, min);

    // inh and exh measurements must agree with one another (+/-3cm H2O)
//    DifferenceInhVsExh01->SetLimits(inhPressure - PRESSURE_VARIANCE,
//        inhPressure + PRESSURE_VARIANCE);
//    VERIFY_RESULT(DifferenceInhVsExh01, exhPressure);

    // save the average of the two sensors readings at 0.1 sec
//    Pressure50_01 = avg(inhPressure, exhPressure);

    // delay using the StartTime as a reference point
    S_Serv->DelayNoSleep(ONE_SECOND + ONE_HUNDRED_MS, StartTime);

    // read inh flow sensor
    // read exh flow sensor
//    inhPressure = S_Serv->GetPressure(InhPressSensor);
//    exhPressure = S_Serv->GetPressure(ExhPressSensor);

#ifdef USE_NUCLEUS
    // timing critical part over, put the task priority back to original value
    TheDiagnosticTask().ChangePriority(tasks[eDiagnostics].Pri); 
#endif
    
    // inh and exh measurements must agree with one another (+/- 3cm H2O)
//    DifferenceInhVsExh11->SetLimits(inhPressure - PRESSURE_VARIANCE,
//        inhPressure + PRESSURE_VARIANCE);
//    VERIFY_RESULT(DifferenceInhVsExh11, exhPressure);

    // save the average of the two sensors readings at 1.1 sec
//    Pressure50_11 = avg(inhPressure, exhPressure);

#ifdef NO_HARDWARE
    // Force the circuit compliance to Pass or Fail
    LONG compliance = 149;
#else
    // calculate circuit compliance
    LONG compliance = (Volume_Compl * SCALE) / Pressure50_01;
#endif
    
    // calculate the leak pressure and set limits for leak based upon compliance
//    Pressure_Leak = Pressure50_01 - Pressure50_11;
//    PressureLeak->SetLimits(-PRESSURE_VARIANCE, (1670 * SCALE) / compliance);

    // verify the results are within spec
    VERIFY_RESULT(PressureLeak, Pressure_Leak)
    VERIFY_RESULT(CircuitCompliance, compliance)

#ifndef NO_HARDWARE
    // save the compliance in NVRAM
//    systemConfigNvram->SetValue(eCircuitCompliance, compliance);
#else
    // Go ahead and display the compliance now 
    TheGUI().Compliance(compliance);
#endif

    NextState(eST_CheckValve3);
}

//*****************************************************************************/
//    Operation Name: ST_CheckValve3()
//
//    Processing: Tests CV3 for leakage.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::ST_CheckValve3(void)
{
    CRITERION(CheckValve3Leaky, SHORT, SCALE, eGreaterThan, 3500, 0, ecmH2O,
        eFC_CheckValve3Leaky)

    // open the safety valve to relieve pressure behind CV3
//    SafetyValve->SetStateDirect(eOpen);

    // wait for pressure to dissipate behind CV3. Also give enough time to 
    // expose a slow leak in CV3
//    NU_Sleep(ONE_SECOND * 2);
                                                            
    // read the inh pressure
//    SHORT inhPressure = S_Serv->GetPressure(InhPressSensor);

    // verify that CV3 can hold the circuit pressurized
//    VERIFY_RESULT(CheckValve3Leaky, inhPressure);

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
//    Miscellaneous: 
//      Only display the compliance value after all the tests within this class 
//      complete. Typically users will block the patient wye with their thumb 
//      and once the compliance is displayed they will release their finger. 
//      Waiting until all the tests have completed before displaying the value 
//      ensures the user does not lift his/her finger too soon. 
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::ST_Complete(void)
{
    // display the patient circuit compliance to the user
#ifndef NO_HARDWARE
//    TheGUI().Compliance(systemConfigNvram->GetValue(eCircuitCompliance));
#endif

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
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:      None
//
//*****************************************************************************/
void PatientCircuitTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (PatientCircuitTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&PatientCircuitTest::ST_Idle,
        (pFunc)&PatientCircuitTest::ST_Complete,
        (pFunc)&PatientCircuitTest::ST_Cancel,
        (pFunc)&PatientCircuitTest::ST_ChargePatientCircuit,
        (pFunc)&PatientCircuitTest::ST_TestCircuitCompliance,
        (pFunc)&PatientCircuitTest::ST_CheckValve3
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}

