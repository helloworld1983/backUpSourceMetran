/******************************************************************************/
//$COMMON.H$
//   File Name:  DeviceInterface.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for access devices from another module
/******************************************************************************/
#ifndef INC_DEVICEINTERFACE_H_
#define INC_DEVICEINTERFACE_H_

#include "stdint.h"
enum RequestId
{
    eGetCurrentTime,    //get current time of system




    eGetHighO2AlarmLimit,   //get high O2 alarm limit
    eGetLowO2AlarmLimit,    //get low O2 alarm limit

    eIsO2SensorConnect,      //get status of O2 sensor(connect or not)

    eFiO2SensorGetCurrentReading, //get current value of FiO2 sensor
    eFiO2SensorGetAvgPress,       //get averages pressure in pressure buffer of FiO2 sensor
    eFiO2SensorGetAvgFlow,        //get averages flow in flow buffer of FiO2 sensor

    eProxySensorLastStatus, //get last status of proxy sensor
    eActivePressureSensorGetLastReading,    //get value of active pressure sensor

    ePressureSensorGetLastReading, //reading from the
                                   //actual pressure sensor that is designated as the Primary

    eHfoValveGetStepPosition,   //Get step position of HFO Valve
    eO2ValveGetStepPosition,   //Get step position of O2Valve
    eAirValveveGetStepPosition,   //Get step position of AirValve
    eExhValveveGetStepPosition,   //Get step position of ExhValve


    eHfoValveRampToPosition,   //HFO Valve ramps to position
    eO2ValveRampToPosition,   //O2Valve ramps to position
    eAirValveveRampToPosition,   //Air Valve ramps to position
    eExhValveveRampToPosition,   //Exh Valve ramps to position

    eInhPressSensorGetLastReading,  //get pressure from inhalation pressure sensor
    eExhPressSensorGetLastReading,  //get pressure from exhalation pressure sensor

    eSafetyValveSetDesiredState,  //set desired satate for safety valve

    eCMVServoControllerSetBiasFlow, //Set bias flow for servo
    eCMVServoControllerSetInhalationTarget, //set inhalation target for servo
    eCMVServoControllerEnable,    //Set CMVServo Controller Enable
    eCMVServoControllerDisable,    //Set CMVServo Controller Disable

    eInpedanceLineSetStateeOn,  //InpedanceLine Sets State: (eOn)
    eInpedanceLineSetDesiredStateeOff,  //InpedanceLine Set DesiredState: (eOff);

    eSafetyValveSetDesiredStateeClose,   //SafetyValve Set DesiredState: (eClose)


    eVCVServoControllerEnable,  //set VCVServoController Enable
    eVCVServoControllerDisable,  //set VCVServoController Disable

    eActivePressureSensorSetPrimaryInhPressSensor,


};

enum ProxyStatus
{
    eNotCalibrated = 0x55,
    eCalibrated,
    eResetAndRecovered,
    eSensorError
};

class DeviceInterface
{
public:
    // static instance of RequestAccessDevices class
    static DeviceInterface *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //request access devices throw Device ID
    bool RequestDevice(RequestId ID, void *data);

#ifndef UNIT_TEST
protected:

private:
#endif
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of RequestAccessDevices class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static DeviceInterface *S_Instance;

    //RequestAccessDevices constructor
    DeviceInterface(void);

};

#endif /* INC_DEVICEINTERFACE_H_ */
