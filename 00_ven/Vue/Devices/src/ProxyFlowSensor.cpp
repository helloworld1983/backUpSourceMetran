/******************************************************************************/
//
//$COMMON.H$
//    File Name: ProxyFlowSensor.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: ProxyFlowSensor
//
//    ProxyFlowSensor allow user init, set ProxyFlowSensor of system
//
/******************************************************************************/
#include "ProxyFlowSensor.h"
#include "DebugTrace.h"

ProxyFlowSensor *ProxyFlowSensor::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the ProxyFlowSensor class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to ProxyFlowSensor class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
ProxyFlowSensor* ProxyFlowSensor::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new ProxyFlowSensor();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Init()
//
//    Processing: initialize proxysensor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void ProxyFlowSensor::Init (void)
{
    //Initialize proxy sensor
    //add code
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: getCurrentStatus()
//
//    Processing: Handle getting status of proxy board procedure
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     status - status of proxy sensor
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
ProxyStatus ProxyFlowSensor::getCurrentStatus(void)
{
    //Get status of proxy sensor and update status
    //add code
    return status;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: getLastStatus()
//
//    Processing: Get last status of proxy sensor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     status - status of proxy sensor
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
ProxyStatus ProxyFlowSensor::getLastStatus (void)
{
    //Get last status of proxy sensor
    return status;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: doCalibration()
//
//    Processing: handle calibration procedure.
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void ProxyFlowSensor::doCalibration()
{
    //add code here
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentReading()
//
//    Processing: get current flow then scale up 100
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     flow value after scale up
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
LONG ProxyFlowSensor::GetCurrentReading(void)
{
    //add code here
    return(CurrentReading);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetLastReading()
//
//    Processing:  Returns value of CurrentReading attribute
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Current flow reading in ml/sec * SCALE
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
LONG ProxyFlowSensor::GetLastReading(void)
{
    // Copy current reading to address specified
    return (CurrentReading);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CommunicateProxySensor()
//
//    Processing: depend on current job, do the correspoding thing:
//                  1. get version of proxy board
//                  2. calibrate proxy board
//                  3. clear calibration effect
//                  4. confirm proxy board reset
//                  5. request status of proxy board
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void ProxyFlowSensor::CommunicateProxySensor (ProxyActions ProxyTitle)
{
    switch (ProxyTitle)
    {
        case eDoProxyCalibration:
            doCalibration();
            break;

        case eClearProxyCalibration:
            clearCalibration();
            break;

        case eComfirmProxyReset:
            comfirmProxyReset();
            break;

        default:
            getCurrentStatus();
            break;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: okToShutdown()
//
//    Processing: Check sensor is ready to shutdown
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     isOkToShutdown - status of sensor
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
E_Bool ProxyFlowSensor::okToShutdown (void)
{
    return isOkToShutdown;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: clearCalibration()
//
//    Processing: Handle clear calibration procedure
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void ProxyFlowSensor::clearCalibration(void)
{

}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: comfirmProxyReset()
//
//    Processing: confirm proxy reset
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void ProxyFlowSensor::comfirmProxyReset(void)
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProxyFlowSensor
//
//    Processing: Constructor
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
ProxyFlowSensor::ProxyFlowSensor (void)
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: getOffset()
//
//    Processing: calculate offset voltage at clibration time
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     VoltageOffset
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
float ProxyFlowSensor::getOffset(void)
{
    float VoltageOffset = 0.0;


    return VoltageOffset;
}
