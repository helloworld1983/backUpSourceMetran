/******************************************************************************/
//$COMMON.H$
//   File Name:  RequestAccessDevice.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for access devices from another module
/******************************************************************************/
#include <DeviceInterface.h>
#include "DebugTrace.h"

#include "RealTimeClock.h"
#include "Standard.h"

#define SEC_IN_A_YEAR           (365 * 24 * 60 * 60)
#define SEC_IN_A_DAY            (24 * 60 * 60)
#define SEC_IN_A_HOUR           (60 * 60)
#define SEC_IN_A_MIN            (60)

DeviceInterface *DeviceInterface::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the RequestAccessDevices class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to RequestAccessDevices class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
DeviceInterface* DeviceInterface::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new DeviceInterface();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for RequestAccessDevices
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
void DeviceInterface::Init (void)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RequestAccessDevices
//
//    Processing: constructor for RequestAccessDevices
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
DeviceInterface::DeviceInterface (void)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RequestAccessDevices
//
//    Processing: constructor for RequestAccessDevices
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: true if you can access devices successfully and vice versa
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
bool DeviceInterface::RequestDevice (RequestId ID, void *data)
{
    //initialize status variable of function
    bool bStatus = false;
    switch (ID)
    {
        //get current time of system
        case eGetCurrentTime:
        {
            Real_Time_Clock clock;
            RealTimeClock::S_GetInstance()->GetCurrentTime(&clock);

            // calculate time again with month , year,...
            UNSIGNED Time = clock.wday * SEC_IN_A_DAY + clock.min * SEC_IN_A_MIN + clock.sec;
            data = &Time;
            bStatus = true;
        }
            break;

        //get high O2 alarm limit
        case eGetHighO2AlarmLimit:
        {
            SHORT AlarmLimit = 0;
            //get value from O2 sensor, add later
            data = &AlarmLimit;
            bStatus = true;
        }
        break;

        //get low O2 alarm limit
        case eIsO2SensorConnect:
        {
            E_Bool IsO2SensorConnect = eFalse;
            //check sensor and update status, add later
            data = &IsO2SensorConnect;
            bStatus = true;
        }
        break;

        //get current value of FiO2 sensor
        case eFiO2SensorGetCurrentReading:
        {
            //get current value of FiO2 sensor
            bStatus = true;
        }
        break;

        //get averages pressure in pressure buffer of FiO2 sensor
        case eFiO2SensorGetAvgPress:
        {
            //get averages pressure in pressure buffer of FiO2 sensor, update later
            bStatus = true;
        }
        break;

        //get averages flow in flow buffer of FiO2 sensor
        case eFiO2SensorGetAvgFlow:
        {
            //get averages flow in flow buffer of FiO2 sensor, update later
            bStatus = true;
        }
        break;

        //get last status of proxy sensor
        case eProxySensorLastStatus:
        {
            //get last status of proxy sensor, update later
            bStatus = true;
        }
        break;

        //reading from the
        //actual pressure sensor that is designated as the Primary
        case ePressureSensorGetLastReading:
        {
            //get last status of pressure sensor, update later
            bStatus = true;
        }
        break;

        //Get step position of HFO Valve
        case eHfoValveGetStepPosition:
        {
            //Get step position of HFO Valve
            bStatus = true;

        }
        break;

        //Get step position of O2Valve
        case eO2ValveGetStepPosition:
        {
            //Get step position of O2Valve
            bStatus = true;

        }
        break;

        //Get step position of AirValve
        case eAirValveveGetStepPosition:
        {
            //Get step position of AirValve
            bStatus = true;

        }
        break;

        //Get step position of ExhValve
        case eExhValveveGetStepPosition:
        {
            //Get step position of ExhValve
            bStatus = true;

        }
        break;

        //HFO Valve ramps to position
        case eHfoValveRampToPosition:
        {
            //HFO Valve ramps to position
            bStatus = true;

        }
        break;

        //O2Valve ramps to position
        case eO2ValveRampToPosition:
        {
            //O2Valve ramps to position
            bStatus = true;

        }
        break;

        //Air Valve ramps to position
        case eAirValveveRampToPosition:
        {
            //Air Valve ramps to position
            bStatus = true;

        }
        break;

        //Exh Valve ramps to position
        case eExhValveveRampToPosition:
        {
            //Exh Valve ramps to position
            bStatus = true;

        }
        break;

        //get pressure from inhalation pressure sensor
        case eInhPressSensorGetLastReading:
        {
            //get pressure from inhalation pressure sensor
            bStatus = true;

        }
        break;

        //get pressure from exhalation pressure sensor
        case eExhPressSensorGetLastReading:
        {
            //get pressure from exhalation pressure sensor
            bStatus = true;

        }
        break;

        //get value of active pressure sensor
        case eActivePressureSensorGetLastReading:
        {
            //get value of active pressure sensor
            bStatus = true;

        }
        break;

        //set desired satate for safety valve
        case eSafetyValveSetDesiredState:
        {
            //set desired satate for safety valve
            bStatus = true;

        }
        break;

        //Set bias flow for servo
        case eCMVServoControllerSetBiasFlow:
        {
            //Set bias flow for servo
            bStatus = true;

        }
        break;

        //set inhalation target for servo
        case eCMVServoControllerSetInhalationTarget:
        {
            //set inhalation target for servo
            bStatus = true;

        }
        break;

        case eInpedanceLineSetStateeOn:
        {
            //eInpedanceLine Set State : On
            bStatus = true;
        }
        break;

        case eInpedanceLineSetDesiredStateeOff:
        {
            //InpedanceLine Sets DesiredStat : On
            bStatus = true;
        }
        break;

        case eSafetyValveSetDesiredStateeClose:
        {
            //SafetyValve Sets DesiredStat : On
            bStatus = true;
        }
        break;

        case eCMVServoControllerEnable:
        {
            //Set CMVServo Controller Enable
            bStatus = true;
        }
        break;

        case eCMVServoControllerDisable:
        {
            //Set CMVServo Controller Disable
            bStatus = true;
        }
        break;

        case eVCVServoControllerEnable:
        {
            //set VCVServoController Enable
            bStatus = true;
        }
        break;

        case eVCVServoControllerDisable:
        {
            //set VCVServoController Disable
            bStatus = true;
        }
        break;

        default:
            //default status function is false;
            bStatus = false;
            break;
    }

    return bStatus;
}
