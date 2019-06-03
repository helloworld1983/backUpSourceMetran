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
#ifndef INC_PROXYFLOWSENSOR_H_
#define INC_PROXYFLOWSENSOR_H_

#include "Standard.h"
#include "DeviceInterface.h"
//enum ProxyStatus
//{
//    eNotCalibrated = 0x55,
//    eCalibrated,
//    eResetAndRecovered,
//    eSensorError
//};

enum ProxyActions
{
    eGetProxyStatus = 0xA5,
    eGetProxyVersion,
    eDoProxyCalibration,
    eClearProxyCalibration,
    eComfirmProxyReset
};

class ProxyFlowSensor
{
public:
    // static instance of WatchDogTimer class
    static ProxyFlowSensor *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Get current status of Proxy Sensor
    ProxyStatus getCurrentStatus(void);

    //Get last status of proxy sensor
    ProxyStatus getLastStatus(void);

    //Get current value of sensor
    LONG GetCurrentReading(void);

    //value of CurrentReading attribute
    LONG GetLastReading(void);

    //Communicate with proxy sensor
    void CommunicateProxySensor(ProxyActions ProxyTitle);

    //Check sensor is ready to shutdown
    E_Bool okToShutdown(void);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of ProxyFlowSensor class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static ProxyFlowSensor *S_Instance;

    //$COMMON.ATTRIBUTE$
    //    Name: status
    //
    //    Description: Status of proxy sensor
    //
    //    Units: None
    //
    //    Range: N/A
    //
    ProxyStatus status;

    //$COMMON.ATTRIBUTE$
    //    Name: isOkToShutdown
    //
    //    Description: status to check if sensor is ready to shutdown
    //
    //    Units: None
    //
    //    Range: N/A
    //
    E_Bool isOkToShutdown;

    //$COMMON.ATTRIBUTE$
    //    Name:        CurrentReading
    //    Description: Holds current reading in engineering units.
    //    Units: ml/sec * SCALE
    //    Range: 0 - 333333
    LONG CurrentReading;

    //ProxyFlowSensor constructor
    ProxyFlowSensor(void);

    //handle calibration procedure.
    void doCalibration(void);

    //Clear calibration
    void clearCalibration(void);

    //confirm proxy reset
    void comfirmProxyReset(void);

    //Get offset
    float getOffset(void);
};

#endif /* INC_PROXYFLOWSENSOR_H_ */
