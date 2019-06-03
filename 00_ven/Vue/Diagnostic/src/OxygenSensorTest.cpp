//$COMMON.CPP$
//   File Name:  OxygenSensorTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: To calibrate the O2 sensor using 100% O2. The calibration results
//      are stored in NV RAM for use by the system. The calibration is a one
//      point test, meaning we assume the 0% O2 voltage is fixed (e.g. 0V)
//      so the 100% O2 voltage is the only reading we take. The sensor output
//      is linear so with both a O% and 100% voltage points we can derive 
//      any O2 percentage.
//
//      This class expects the patient circuit to have all air flushed
//      from the patient circuit (meaning saturated with 100% O2) and the O2 
//      flow controller must already be set at 1lpm when these tests start.
//
//      The following systems are checked by this class:
//
//      Oxygen Concentration (Fi) Sensor
//          Test Approach - After the patient circuit is fully saturated with 
//              100% O2, measure the O2 sensor voltage. The 100% O2 voltage 
//              level must be in range. If test passes, the calibration 
//              constants will be saved to NV RAM.
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "OxygenSensorTest.h"
#include "Criterion.h"
#include "FailureCodes.h"
#include "TestServices.h"
//#include "GUI.h"
//
//#ifndef NO_HARDWARE
//    #include "FiO2.h"
//    #include "nvram.h"
//    #include "Flow_Cnt.h"
//#else
//    #include "HardwareStubs.h"
//#endif

//*****************************************************************************/
//    Operation OxygenSensorTest()
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
OxygenSensorTest::OxygenSensorTest(SelfTest* parent) : 
    ChildTest(parent, eOxygenSensorTest)
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
void OxygenSensorTest::Start(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_O2SensorPresent,            // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_CannotHappen,               // ST_O2SensorPresent
        eST_CannotHappen,               // ST_O2SensorCalibration
        eST_CannotHappen                // ST_TestFailure
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
void OxygenSensorTest::Cancel(void)
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_EventIgnored,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Cancel,                     // ST_O2SensorPresent
        eST_Cancel,                     // ST_O2SensorCalibration
        eST_Cancel                      // ST_TestFailure
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
void OxygenSensorTest::Response(ResponseData* pData) 
{
    static const UCHAR StateTransitions[eST_MaxStates] = {
        // -New State-                    -Current State-
        eST_CannotHappen,               // ST_Idle
        eST_CannotHappen,               // ST_Complete
        eST_CannotHappen,               // ST_Cancel
        eST_Complete,                   // ST_O2SensorPresent
        eST_TestFailure,                // ST_O2SensorCalibration
        eST_CannotHappen                // ST_TestFailure
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
//    Operation Name: ST_O2SensorPresent()
//
//    Processing: Detects whether the O2 sensor is connected to the instrument.
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
void OxygenSensorTest::ST_O2SensorPresent(void)
{
    // set this test as active so all client events will route here
    S_SetActiveTest(this);
    
#ifndef NO_HARDWARE
    // if O2 sensor is connected
    //      go cal the sensor
    //  else
    //      prompt user that O2 sensor is missing
//    if (FiO2Sensor->GetIsPresent())
//        NextState(eST_O2SensorCalibration);
//    else
//	{
//        // save in NV RAM the fact that the O2 sensor cal failed
//        // update the FiO2 conversion factor to reflect a bad cal
//		systemConfigNvram->SetValue(eFiO2Calibration, O2_INVALID);
//		FiO2Sensor->SetConversionFactor(O2_INVALID);
//
//        TheGUI().GetResponse(eO2SensorMissingMsg);
//	}


#else
    //  prompt user that O2 sensor is missing
    TheGUI().GetResponse(eO2SensorMissingMsg);
#endif
}

//*****************************************************************************/
//    Operation Name: ST_O2SensorCalibration()
//
//    Processing: Calibrate the O2 sensor using 100% O2. The response time of
//      the sensor is approximately 12 seconds, so we need to delay at least 
//      that long to ensure the 100% O2 measurement is accurate.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions: The patient circuit must already have all air flushed 
//      from the patient circuit (meaning saturated with 100% O2) and the O2 
//      flow controller must already be set at 1lpm.
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void OxygenSensorTest::ST_O2SensorCalibration(void)
{
//    CRITERION(O2Sensor100PercentSampleMin, USHORT, 0, eGreaterThanOrEqual,
//        1500, 0, eA2DCnts, eFC_O2Sensor100PercentSampleRange)
//    CRITERION(O2Sensor100PercentSampleMax, USHORT, 0, eLessThanOrEqual,
//        O2_SENSOR_LIMIT, 0, eA2DCnts, eFC_O2Sensor100PercentSampleRange)
//    CRITERION(O2Sensor100PercentAvg, USHORT, 0, eRange,
//        1045, O2_SENSOR_LIMIT, eA2DCnts, eFC_O2Sensor100PercentAvgRange)
//
//    USHORT min;
//    USHORT max;
//
//    // delay long enough for the sensor to fully respond to the 100% O2
//    if (S_Serv->DelayWithCancel(ONE_SECOND * 15))
//        return;
//
//    // read the oxygen sensor while 100% O2 is in the system
//    USHORT O2CalVoltage = S_Serv->GetO2V(5, &min, &max);
//
//    // IF O2 sensor tested good
//    if (O2Sensor100PercentSampleMin->Passes(min) &&
//        O2Sensor100PercentSampleMax->Passes(max) &&
//        O2Sensor100PercentAvg->Passes(O2CalVoltage))
//    {
//#ifndef NO_HARDWARE
//        // save 100% O2 voltage in NV RAM
//        // update the FiO2 conversion factor to reflect the new cal reading
//        systemConfigNvram->SetValue(eFiO2Calibration, O2CalVoltage);
//        FiO2Sensor->SetConversionFactor(O2CalVoltage);
//#endif
//        NextState(eST_Complete);
//    }
//    else
//    {
//#ifndef NO_HARDWARE
//        // save in NV RAM the fact that the O2 sensor cal failed
//        // update the FiO2 conversion factor to reflect a bad cal
//        systemConfigNvram->SetValue(eFiO2Calibration, O2_INVALID);
//        FiO2Sensor->SetConversionFactor(O2_INVALID);
//
//        // prompt user that O2 sensor is bad
//        TheGUI().GetResponse(eO2SensorBadMsg);
//
//        // save failure data for after user acknowledges sensor is bad prompt
//        if (!O2Sensor100PercentSampleMin->Passes(min))
//            O2Sensor100PercentSampleMin->GetInfo(&FailureInfo);
//        else if (!O2Sensor100PercentSampleMax->Passes(max))
//            O2Sensor100PercentSampleMax->GetInfo(&FailureInfo);
//        else if (!O2Sensor100PercentAvg->Passes(O2CalVoltage))
//            O2Sensor100PercentAvg->GetInfo(&FailureInfo);
//#else
//        NextState(eST_Complete);
//#endif
//    }
}

//*****************************************************************************/
//    Operation Name: ST_TestFailure()
//
//    Processing: Test has failed. Send failure data back to the parent test.
//
//    Input Parameters:
//      pData - the operator response.
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
void OxygenSensorTest::ST_TestFailure(ResponseData* pData)
{
    // let parent test know test failed
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
void OxygenSensorTest::DoStateAction(EventData* pData)
{
    // function prototype each state method must conform to
    typedef void (OxygenSensorTest::*pFunc)(EventData *);

    // state table with pointers to each state method
    static pFunc stateMachine[eST_MaxStates] = {
        (pFunc)&OxygenSensorTest::ST_Idle,
        (pFunc)&OxygenSensorTest::ST_Complete,
        (pFunc)&OxygenSensorTest::ST_Cancel,
        (pFunc)&OxygenSensorTest::ST_O2SensorPresent,
        (pFunc)&OxygenSensorTest::ST_O2SensorCalibration,
        (pFunc)&OxygenSensorTest::ST_TestFailure
    };

    // ensure current state is within bounds
    // execute the state method
    ASSERTION(CurrentState < eST_MaxStates);  
    (this->*stateMachine[CurrentState])(pData);
}

//*****************************************************************************/
//    Operation Name: RetestInit()
//
//    Processing: Gets this test ready to test after being interrupted by a 
//      a failure. 
//
//      Normally if no failure occurs the previous test, OxygenDeliveryTest, is 
//      responsible for flushing all the air from the system so 100% O2 is 
//      left by the time this test is run. However, if this test fails and the
//      operator wants a retest, then this test needs to reflush the air from 
//      the system to ensure 100% O2 is present when the calibration is 
//      performed.
//      
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous: If this test or the previous test fails this method must 
//      be called before this test is executed.
//
//    Requirements:  
//
//*****************************************************************************/
void OxygenSensorTest::RetestInit(void)
{
    static const LONG HIGH_FLOW = 2000 * LPM_FACTOR;   // 200.0 LPM
    static const LONG LOW_FLOW = 10 * LPM_FACTOR;      // 1.0 LPM

    // start a high flow going to quickly fill system with 100% O2
//    O2FlowController->SetDesired(HIGH_FLOW);

    // wait for O2 to saturate the system
//    NU_Sleep(TWO_SECONDS);

    // start a low residual flow going to maintain 100% O2 in the system
//    O2FlowController->SetDesired(LOW_FLOW);
}


