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

enum ProxyActions
{
    eGetProxyStatus = 0xA5,
    eGetProxyVersion,
    eDoProxyCalibration,
    eClearProxyCalibration,
    eComfirmProxyReset
};

typedef struct
{
        //$COMMON.ATTRIBUTE$
        //    Name: Address
        //    Description: Device address of slave which is send
        //    Range: 0 - 255
        uint8_t Address;

        //$COMMON.ATTRIBUTE$
        //    Name: CMD
        //    Description: Commend ID
        //    Range: None
        uint8_t CMD;

        //$COMMON.ATTRIBUTE$
        //    Name: LengTH
        //    Description: Length of data fields
        //    Range: None
        uint8_t LengTH;

        //$COMMON.ATTRIBUTE$
        //    Name: Data
        //    Description: Data
        //    Range: None
        uint8_t Data[255];

}ProxySensorSendFrameData;

typedef struct
{
        //$COMMON.ATTRIBUTE$
        //    Name: Address
        //    Description: Device address of slave which is receive
        //    Range: 0 - 255
        uint8_t Address;

        //$COMMON.ATTRIBUTE$
        //    Name: CMD
        //    Description: Commend ID
        //    Range: None
        uint8_t CMD;

        //$COMMON.ATTRIBUTE$
        //    Name: State
        //    Description: The slave sends a state byte to report execution errors
        //    Range: None
        uint8_t State;

        //$COMMON.ATTRIBUTE$
        //    Name: LengTH
        //    Description: Length of data fields
        //    Range: None
        uint8_t LengTH;

        //$COMMON.ATTRIBUTE$
        //    Name: Data
        //    Description: Data
        //    Range: None
        uint8_t Data[255];

}ProxySensorRecFrameData;

class ProxyFlowSensor
{
public:
    // static instance of WatchDogTimer class
    static ProxyFlowSensor *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);


    //Read data from RS485 - UART
    int ReadDataFromUART(void *pData, int iBytes);

    //send request to proxyflow sensor
    void SendComment(ProxySensorSendFrameData *data);

    //Generate frame data from Comment ID
    void GenerateFrameData(ProxySensorSendFrameData *data, uint8_t *FrameData, uint32_t& SizeOfFrameData);
    uint32_t SendDataToUART(void *data, int bytes);//remove, just test
    bool ConvertReceiveDataToFrameData(uint8_t *pSourceByte, uint32_t uSourceSize, ProxySensorRecFrameData *data); //test

    void ConvertByteArrayToSend(uint8_t *pSourceByte, uint32_t uSourceSize, uint8_t *pDesByte, uint64_t& uDesSizeByte);


    //Get current status of Proxy Sensor
    ProxyStatus getCurrentStatus(void);

    //Get last status of proxy sensor
    ProxyStatus getLastStatus(void);

    //Get current value of sensor
    LONG GetCurrentReading(void);

    //value of CurrentReading attribute
    LONG GetLastReading(void);

    //Communicate with proxy sensor
    void CommunicateProxySensor();

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

    ProxyActions ProxyTitle;

    //ProxyFlowSensor constructor
    ProxyFlowSensor(void);

    //Send Data to UART - RS485
//        uint32_t SendDataToUART(void *pData, int iBytes);

//        //Read data from RS485 - UART
//        int ReadDataFromUART(void *data, int bytes);

    //Calculate CRC of Frame Data
    uint8_t CalcCRC(uint8_t* pBuf, uint32_t uSize);

    //Add special byte to Frame Content if it has
//        void ConvertByteArrayToSend(uint8_t *pSourceByte, uint32_t uSourceSize, uint8_t *pDesByte, uint64_t& uDesSizeByte);

    //Convert receive data from uart to Frame Data
//        bool ConvertReceiveDataToFrameData(uint8_t *pSourceByte, uint32_t uSourceSize, ProxySensorRecFrameData *data);

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
