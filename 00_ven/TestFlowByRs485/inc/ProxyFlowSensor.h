//******************************************************************************
//$COMMON.H$
//   File Name:  ProxyFlowSensor.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  ProxyFlowSensor
//    This class is a derived class of the Sensor base class.
//    This class provides an interface to the TSI Mass Flow Sensor
//******************************************************************************
#ifndef PROXYFLOWSENSOR_H_
#define PROXYFLOWSENSOR_H_

#include "stdint.h" //usefor uint32,... can remove later

#ifdef UNIT_TEST
#include "ProxyFlowSensor_UnitTest.h"
#endif

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
        // static instance of HfoServo class
        static ProxyFlowSensor *S_GetInstance(void);

        //Initialize for Proxy flow sensor
        void Init(void);

        //Read data from RS485 - UART
        int ReadDataFromUART(void *pData, int iBytes);

        //Generate frame data from Comment ID
        void GenerateFrameData(ProxySensorSendFrameData *data, uint8_t *FrameData, uint32_t& SizeOfFrameData);
        uint32_t SendDataToUART(void *data, int bytes);//remove, just test


    private:
        //Constructor for ProxyFlowSensor
        ProxyFlowSensor(void);

        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of ProxyFlowSensor class
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static ProxyFlowSensor* S_Instance;

        //Send Data to UART - RS485
//        uint32_t SendDataToUART(void *pData, int iBytes);

//        //Read data from RS485 - UART
//        int ReadDataFromUART(void *data, int bytes);

        //Calculate CRC of Frame Data
        uint8_t CalcCRC(uint8_t* pBuf, uint32_t uSize);

        //Replace special byte in Frame Data
        int8_t *ConvertByteSruffing(int8_t iByte); //remove

        //Add special byte to Frame Content if it has
        void ConvertByteArrayToSend(uint8_t *pSourceByte, uint32_t uSourceSize, uint8_t *pDesByte, uint32_t& uDesSizeByte);

        //Convert receive data from uart to Frame Data
        bool ConvertReceiveDataToFrameData(uint8_t *pSourceByte, uint32_t uSourceSize, ProxySensorRecFrameData *data);

#ifdef UNIT_TEST
friend class proxyProxyFlowSensor;
#endif

};

#endif /* PROXYFLOWSENSOR_H_ */
