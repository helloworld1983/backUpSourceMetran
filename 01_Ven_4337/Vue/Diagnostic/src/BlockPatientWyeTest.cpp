//$COMMON.CPP$
//   File Name:  BlockPatientWyeTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To verify the patient wye is blocked and the patient is 
//      disconnected from the instrument.
//
//      If the wye is blocked, the circuit can be pressurized (barring any 
//      leaks in the system). If the wye is unblocked, the circuit is open 
//      to atmosphere and cannot be pressurized. 
//
//      The following systems are checked by this class:
//
//      Air Flow Sensor Calibration
//      O2 Flow Sensor Calibration
//      Exh Flow Sensor Calibration
//          Test Approach - Verify the calibration data integrity. If the data
//              is corrupt or not up-to-data then rebuild the calibration data.
//              Once the new data is rebuilt, verify it is correct.
//      
//      Inhalation Auto Zero Solenoid
//      Exhalation Auto Zero Solenoid
//          Test Approach - Test the auto zero solenoids by pressurizing the 
//              patient circuit then exposing the solenoids to atmosphere by 
//              opening the auto zero solenoids. If the solenoids open, then 
//              the pressure sensors should read the atmospheric pressure, 
//              which should be close to 0cmH2O.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "BlockPatientWyeTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"
#include "DiagnosticTask.h"

//#ifndef NO_HARDWARE
//    #include "Tasks.h"
//    #include "DOutput.h"
//    #include "ExhContr.h"
//    #include "Flow_Cnt.h"
//#endif

//*****************************************************************************/
//    Operation BlockPatientWyeTest()
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
BlockPatientWyeTest::BlockPatientWyeTest(SelfTest* parent) : 
    ChildTest(parent, eBlockPatientWyeTest)
{
    blockWyeAttempts = 0;
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
void BlockPatientWyeTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_FlowSensorsCalibration,     // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_FlowSensorsCalibration
        eST_CannotHappen,               // ST_UnblockPatientWye
        eST_CannotHappen,               // ST_VerifyPatientWyeUnblocked
        eST_CannotHappen,               // ST_BlockPatientWye
        eST_CannotHappen,               // ST_VerifyPatientWyeBlocked
        eST_CannotHappen,               // ST_TestAutoZeroSolenoids
        eST_CannotHappen                // ST_PatientIsConnected
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
void BlockPatientWyeTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_FlowSensorsCalibration
        eST_Cancel,                     // ST_UnblockPatientWye
        eST_Cancel,                     // ST_VerifyPatientWyeUnblocked
        eST_Cancel,                     // ST_BlockPatientWye
        eST_Cancel,                     // ST_VerifyPatientWyeBlocked
        eST_Cancel,                     // ST_TestAutoZeroSolenoids
        eST_Cancel                      // ST_PatientIsConnected
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
void BlockPatientWyeTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_FlowSensorsCalibration
        eST_VerifyPatientWyeUnblocked,  // ST_UnblockPatientWye
        eST_PatientIsConnected,         // ST_VerifyPatientWyeUnblocked
        eST_VerifyPatientWyeBlocked,    // ST_BlockPatientWye
        eST_CannotHappen,               // ST_VerifyPatientWyeBlocked
        eST_CannotHappen,               // ST_TestAutoZeroSolenoids
        eST_CannotHappen                // ST_PatientIsConnected
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
//    Operation Name: ST_Idle()
//
//    Processing: State machine sits here when not testing. 
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
void BlockPatientWyeTest::ST_Idle(void)
{
    // put the instrument into a known state
    S_Serv->KnownState();

    blockWyeAttempts = 0;
}

//*****************************************************************************/
//    Operation Name: ST_FlowSensorsCalibration()
//
//    Processing: If the flow sensors calibration data is not current or was 
//      corrupted, then rebuild the calibration data.
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
void BlockPatientWyeTest::ST_FlowSensorsCalibration(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
    CRITERION(FlowSensorsTableErase, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_FlowSensorsCannotEraseTable)

    CRITERION(AirFlowSensorCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_AirFlowSensorCannotCal)
    CRITERION(O2FlowSensorCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_O2FlowSensorCannotCal)
    CRITERION(ExhFlowSensorCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_ExhFlowSensorCannotCal)

    CRITERION(AirFlowSensorVerifyCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_AirFlowSensorVerifyCalFailure)
    CRITERION(O2FlowSensorVerifyCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_O2FlowSensorVerifyCalFailure)
    CRITERION(ExhFlowSensorVerifyCal, int, 0, eEqual, eSuccess, 0, eNoString,
        eFC_ExhFlowSensorVerifyCalFailure)

#ifndef NO_HARDWARE
//    E_ReturnStatus airResult = AirFlowSensor->CheckCalibrationData();
//    E_ReturnStatus O2_Result = O2FlowSensor->CheckCalibrationData();
//    E_ReturnStatus exhResult = ExhFlowSensor->CheckCalibrationData();

    // if any of the flow sensor calibration data is corrupt or not up-to-date
//    if (airResult != eSuccess || O2_Result != eSuccess || exhResult != eSuccess)
//    {
        // erase the old flow table in EEPROM
//        VERIFY_RESULT(FlowSensorsTableErase,
//            FlowSensor::S_EraseRadicalLookupTable())

        // get all flow sensor calibration data
//        VERIFY_RESULT(AirFlowSensorCal,
//            AirFlowSensor->ObtainCalibrationData())
//        VERIFY_RESULT(O2FlowSensorCal,
//            O2FlowSensor->ObtainCalibrationData())
//        VERIFY_RESULT(ExhFlowSensorCal,
//            ExhFlowSensor->ObtainCalibrationData())

        // verify the flow sensor calibration data integrity
//        VERIFY_RESULT(AirFlowSensorVerifyCal,
//            AirFlowSensor->CheckCalibrationData())
//        VERIFY_RESULT(O2FlowSensorVerifyCal,
//            AirFlowSensor->CheckCalibrationData())
//        VERIFY_RESULT(ExhFlowSensorVerifyCal,
//            AirFlowSensor->CheckCalibrationData())
//    }
//    else
//    {
        // verify the flow sensor calibration data integrity
//        VERIFY_RESULT(AirFlowSensorVerifyCal, airResult)
//        VERIFY_RESULT(O2FlowSensorVerifyCal, O2_Result)
//        VERIFY_RESULT(ExhFlowSensorVerifyCal, exhResult)
//    }
#endif
//    NextState(eST_UnblockPatientWye);
}

//*****************************************************************************/
//    Operation Name: ST_UnblockPatientWye()
//
//    Processing: Prompt user to unblock the patient wye. 
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
void BlockPatientWyeTest::ST_UnblockPatientWye(void)
{
//    TheGUI().GetResponse(eUnblockPatientWyeMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyPatientWyeUnblocked()
//
//    Processing: Verify the patient wye is not blocked by trying to pressurize
//      the circuit. If the circuit fails to pressurize then the wye not 
//      blocked. 
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
void BlockPatientWyeTest::ST_VerifyPatientWyeUnblocked(ResponseData* pData)
{
    static const SHORT PRESSURE = 10 * SCALE;  // 10 cmH20
    static const LONG FLOW = 250 * LPM_FACTOR; // 25.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 1; 
    
    CRITERION(UnblockedPatientWyePressure, SHORT, SCALE, eLessThan, PRESSURE, 0, 
        ecmH2O, eFC_PatientWyeNotUnblocked)
        
    // handle the response obtained from user on unplugging the patient wye
    CHECK_RESPONSE(pData)
    
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            AirFlowController,
//            FLOW,
//            TIME_LIMIT,
//            PRESSURE,
//            &pressure
//        );
    
// Force the test to pass
#ifdef NO_HARDWARE
    pressure = 0;
#endif
    // if the circuit could not be pressurized (meaning wye not blocked)
   if (UnblockedPatientWyePressure->Passes(pressure))
   {
        VERIFY_RESULT(UnblockedPatientWyePressure, pressure)
//        NextState(eST_BlockPatientWye);
   }
   else
   {
        // open exh and safety valves just incase patient is connected
//        ExhController->Open();
//        SafetyValve->SetStateDirect(eOpen);
        
        // prompt user that patient wye is blocked or patient connected
//        TheGUI().GetResponse(ePatientIsConnectedMsg);

        // save failure data for after user acknowledges patient is connected
        UnblockedPatientWyePressure->GetInfo(&FailureInfo);
   }
}

//*****************************************************************************/
//    Operation Name: ST_BlockPatientWye()
//
//    Processing: Prompt user to block the patient wye. 
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
void BlockPatientWyeTest::ST_BlockPatientWye(void)
{
//    TheGUI().GetResponse(eBlockPatientWyeMsg);
}

//*****************************************************************************/
//    Operation Name: ST_VerifyPatientWyeBlocked()
//
//    Processing: Verify the operator has disconnected the patient from the
//      instrument and that the patient wye is plugged. If the circuit 
//      pressurizes then the wye is correctly blocked. 
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
void BlockPatientWyeTest::ST_VerifyPatientWyeBlocked(ResponseData* pData)
{
    static const UCHAR MAX_TRIES = 2;
    static const SHORT PRESSURE = 10 * SCALE;  // 10 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR;  // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 3; 
    
    CRITERION(BlockedPatientWyePressure, SHORT, SCALE, eGreaterThanOrEqual, 
        PRESSURE, 0, ecmH2O, eFC_PatientWyeNotBlocked)
        
    // handle the response obtained from user on plugging the patient wye
    CHECK_RESPONSE(pData)
    
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            AirFlowController,
//            FLOW,
//            TIME_LIMIT,
//            PRESSURE,
//            &pressure
//        );

// Force the test to pass
#ifdef NO_HARDWARE
    pressure = PRESSURE+1;
#endif

    // IF operator has tried to plug wye too many times OR 
    // pressure is too low (meaning wye is not plugged or large leak)
    if (++blockWyeAttempts >= MAX_TRIES || 
        BlockedPatientWyePressure->Passes(pressure))
    {
        VERIFY_RESULT(BlockedPatientWyePressure, pressure)
        NextState(eST_TestAutoZeroSolenoids);
    }
    else
    {
        // let operator try again to plug the wye
        NextState(eST_BlockPatientWye);
    }

    // open exh valve to relieve pressure
//    ExhController->Open();
}

//*****************************************************************************/
//    Operation Name: ST_TestAutoZeroSolenoids()
//
//    Processing: Test the auto zero solenoids by pressurizing the patient 
//      circuit then exposing the solenoids to atmosphere by opening the 
//      auto zero solenoids. If the solenoids open, then the pressure sensors
//      should read the atmospheric pressure, which should be close to 0cmH2O.
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
void BlockPatientWyeTest::ST_TestAutoZeroSolenoids(void)
{
    static const SHORT PRESSURE = 50 * SCALE; // 50 cmH20
    static const LONG FLOW = 50 * LPM_FACTOR; // 5.0 LPM
    static const UNSIGNED TIME_LIMIT = ONE_SECOND * 3; 

    CRITERION(InhAutoZeroSolenoid, SHORT, SCALE, eRange, -300, 300, ecmH2O,
        eFC_InhAutoZeroSolenoidCannotOpen)
    CRITERION(ExhAutoZeroSolenoid, SHORT, SCALE, eRange, -300, 300, ecmH2O,
        eFC_ExhAutoZeroSolenoidCannotOpen)

    // charge the circuit using the air controller
    SHORT pressure;
//    S_Serv->ChargeCircuit
//        (
//            AirFlowController,
//            PRESSURE,
//            FLOW,
//            TIME_LIMIT,
//            &pressure
//        );

    // expose the inhalation and exhalation pressure sensors to atmosphere using
    // the auto zero solenoids
//    ExhPressureZero->SetStateDirect(eOn);
//    InhPressureZero->SetStateDirect(eOn);

    // wait for the sensor pressures to stabilize
//    NU_Sleep(FIFTY_MS);

    // read the exhalation pressure sensor
    // verify the measurement is close to zero 
//    SHORT exhPressure = S_Serv->GetPressure(ExhPressSensor);

// Force the test to pass
#ifdef NO_HARDWARE
    exhPressure = 0;
#endif

//    VERIFY_RESULT(ExhAutoZeroSolenoid, exhPressure);

    // read the inhalation pressure sensor
    // verify the measurement is close to zero 
//    SHORT inhPressure = S_Serv->GetPressure(InhPressSensor);

// Force the test to pass
#ifdef NO_HARDWARE
    inhPressure = 0;
#endif

//    VERIFY_RESULT(InhAutoZeroSolenoid, inhPressure);

    // turn the solenoids back off
//    ExhPressureZero->SetStateDirect(eOff);
//    InhPressureZero->SetStateDirect(eOff);

    NextState(eST_Complete);
}

//*****************************************************************************/
//    Operation Name: ST_PatientIsConnected()
//
//    Processing: Patient is still connected to the instrument. Let parent test
//      know about failure. 
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
void BlockPatientWyeTest::ST_PatientIsConnected(ResponseData* pData)
{
    // let parent test know patient is connected
    // goto idle state
    Parent->ChildFailure(this, &FailureInfo);
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
void BlockPatientWyeTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (BlockPatientWyeTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&BlockPatientWyeTest::ST_Idle,
        (pFunc)&BlockPatientWyeTest::ST_Complete,
        (pFunc)&BlockPatientWyeTest::ST_Cancel,
        (pFunc)&BlockPatientWyeTest::ST_FlowSensorsCalibration,
        (pFunc)&BlockPatientWyeTest::ST_UnblockPatientWye,
        (pFunc)&BlockPatientWyeTest::ST_VerifyPatientWyeUnblocked,
        (pFunc)&BlockPatientWyeTest::ST_BlockPatientWye,
        (pFunc)&BlockPatientWyeTest::ST_VerifyPatientWyeBlocked,
        (pFunc)&BlockPatientWyeTest::ST_TestAutoZeroSolenoids,
        (pFunc)&BlockPatientWyeTest::ST_PatientIsConnected
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}


