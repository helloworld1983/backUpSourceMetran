//******************************************************************************
//$COMMON.H$
//   File Name:  FLOWSENS.H
//   Copyright 2012 MeTranVietNam Co. All Rights Reserved.
//
//    Class:  	FlowSensor
//	  According to 840101/840102 Mass Flow Transducers 1980350SP_F.doc: DOCUMENT
//    This class provides an interface to the TSI Mass Flow Sensor type: 840101/840102
//
//   Revision History:
//
//******************************************************************************
#ifndef    FLOWSENS_H
#define    FLOWSENS_H

#include "sensor.h"
#include "Tsi.h"
#include "util.h"
#include "stdint.h"

// Sensor Type field
enum E_FlowType
{
    eAir,
    eOxygen
};

class FlowSensor
{
public:

    FlowSensor(E_DeviceId  id,          // Sensor Id #
               uint8_t I2C,             // which i2c
               E_FlowType type       // Oxygen/Air
    );

    void Init(void);

    //Read data from add
    bool I2C_Read(uint32_t addr, uint8_t* buf, uint32_t len);

    //Write data from add
    bool I2C_Write(uint32_t addr, uint8_t* buf, uint32_t len);

    //calculate CRC
    uint8_t GetCRC8( const void *buff, uint16_t size );

    E_Bool IsFlowSensorError(void){return FlowSensorError;}
    bool CheckModel(void);

    LONG   GetCurrentReading(void);         // Obtains reading from sensor
    LONG   GetLastReading(void);            // Returns last valid reading

    void   SetTemperatureFactor(void);      // Reads temp AI signal
    LONG   GetCompensatedReading(void);     // Adjusts current reading for altitude

private:
    //$COMMON.ATTRIBUTE$
    //    Name:        i2cOrder
    //    Description: choose i2c0 or i2c1 for sensor
    //    Units: None
    //    Range: N/A
    uint8_t i2cOrder;

    E_Bool FlowSensorError;

    //$COMMON.ATTRIBUTE$
    //    Name:        CurrentReading
    //    Description: Holds current reading in engineering units.
    //    Units: ml/sec * SCALE
    //    Range: 0 - 333333
    LONG   CurrentReading;

    //$COMMON.ATTRIBUTE$
    //    Name:         CalibrationData
    //    Description:  Structure holding TSI calibration data
    //    Units: None
    //    Range: N/A
    struct TSI_Calibration_Data_Struct CalibrationData;

    //$COMMON.ATTRIBUTE$
    //    Name:        Type
    //    Description: Denotes whether this flow sensor is used for
    //          air or oxygen.
    //    Units: None
    //    Range: N/A
    E_FlowType Type;
};

#endif  //FLOWSENS_H

