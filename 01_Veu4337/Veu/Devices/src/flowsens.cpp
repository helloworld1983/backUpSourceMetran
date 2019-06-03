//******************************************************************************//
//$COMMON.CPP$
//   File Name:  FLOWSENS.CPP
//   Copyright 2012 MeTranVietNam Co. All Rights Reserved.
//
//   Purpose: 	This file contains the FlowSensor class which
//          	represents a TSI Flow Sensor.
//
//   Interfaces:
//          BinarySearch - utility routine
//          interpolate  - utility routine
//          ipow         - utility routine
//
//   Restrictions:
//          Flow sensor input signals are assumed to have range of 0-5 Volts.
//          A2D output range for the input signals is assumed to be 0 - 4095.
//******************************************************************************
#include "flowsens.h"
#include <math.h>

#include "util.h"
#include "Errorhan.h"

#include "DebugTrace.h"

/* Air flow sensor model number. */
static const USHORT AIR_MODEL_NUM = 101;

/* O2 flow sensor model number. */
static const USHORT O2_MODEL_NUM = 102;

// Number of bytes read in the header portion of the EEPROM for the flow sensor.
// This number includes the checksum up to the coefficient data.
static const UCHAR EEPROM_HDR_BYTES = 18; 

/* Maximum flow reading in LPM, scaled by 2**8; 300 LPM */
static const LONG MAX_READING_IN_SCALED_LPM = 76800;

#define AIR_FLOWSENSOR           (0)
#define O2_FLOWSENSOR            (1)
#define MSB_CRC8                 (0x31)
#define SENSOR_I2C_SEND_ADDR     (0x80)
#define SENSOR_I2C_READ_ADDR     (0x81)

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: FlowSensor()
//
//    Processing: This is the constructor for the flow sensor class.
//
//    Input Parameters:
//          flowaddr:  A2D channel # of the flow signal.
//          tempaddr:  A2D channel # of the temperature signal.
//          csiaddr :  CSI select # of the calibration data EEPROM
//          type    :  air or oxygen
//
//    Output Parameters: None
//    Return Values:     ptr to FlowSensor object.
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
FlowSensor::FlowSensor(E_DeviceId  id, uint8_t I2C, E_FlowType type)
{
    CalibrationData.Checksum = 0;               // edit
    CalibrationData.ModelNumber = 0;
    CalibrationData.SerialNumber = 0;
    CalibrationData.Revision = 0;
    CalibrationData.Day = 0;
    CalibrationData.Month = 0;
    CalibrationData.Year = 0;
    CalibrationData.Tcorr = 0;
    CalibrationData.K = 0;
    CalibrationData.Z = 0;

    for(int i = 0;i<MAX_COEF;i++ )
    {
        CalibrationData.Coefficients[i].Vf = 0;
        CalibrationData.Coefficients[i].A = 0;
        CalibrationData.Coefficients[i].B = 0;
        CalibrationData.Coefficients[i].C = 0;

    }

    CalibrationData.Tcal = 0;
    CalibrationData.Toper = 0;

    CalibrationData.NumberOfAir = 0;
    CalibrationData.NumberOfO2 = 0;


    FlowSensorError = eTrue;
    // Assign sensor type
    Type = type;

    CurrentReading = 0;

    i2cOrder = I2C;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CheckModel()
//
//    Processing:   Check model
//
//    Input Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
bool FlowSensor::CheckModel(void)
{
    bool rtn = false;
    int model = CalibrationData.SerialNumber/10000000;

    if(Type == eAir)
    {
        if((model==105)||(model==101))    //101 is model number of air flow sensor
        {
            rtn = true;
            FlowSensorError = eFalse;
        }
        else
            FlowSensorError = eTrue;
    }
    else
    {
        if((model==106)||(model==102))    //101 is model number of oxygen flow sensor
        {
            rtn = true;
            FlowSensorError = eFalse;
        }
        else
            FlowSensorError = eTrue;
    }
    return rtn;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Init()
//
//    Processing:   initialize flow sensor device
//
//    Input Parameters: NOne
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void FlowSensor::Init (void)
{
    switch (i2cOrder) {
        case AIR_FLOWSENSOR:
            Board_I2C_Init(I2C0);
            break;

        case O2_FLOWSENSOR:
            Board_I2C_Init(I2C1);
            break;
        default:
            break;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: I2C_Read (uint32_t addr, uint8_t* buf, uint32_t len)
//
//    Processing:   Read data from I2C
//
//    Input Parameters: uint32_t addr, uint8_t* buf, uint32_t len
//
//    Return Values:     data of I2C
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
bool FlowSensor::I2C_Read (uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;
    bool bStatus = true;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = NULL;
    i2cData.txSz = 0;
    i2cData.rxBuff = buf;
    i2cData.rxSz = len;

    if(AIR_FLOWSENSOR == i2cOrder)
    {
        if (Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0) {
            bStatus = false;
        }
    }
    else if (O2_FLOWSENSOR == i2cOrder)
    {
        if (Chip_I2CM_XferBlocking(LPC_I2C1, &i2cData) == 0) {
            bStatus = false;
        }
    }
    else
    {
        ASSERTION(false);
    }

    return bStatus;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: I2C_Write (uint32_t addr, uint8_t* buf, uint32_t len)
//
//    Processing:   Write data from I2C
//
//    Input Parameters: uint32_t addr, uint8_t* buf, uint32_t len
//
//    Return Values:     data of I2C
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
bool FlowSensor::I2C_Write (uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;
    bool bStatus = true;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = buf;
    i2cData.txSz = len;
    i2cData.rxBuff = NULL;
    i2cData.rxSz = 0;

    if(AIR_FLOWSENSOR == i2cOrder)
    {
        if (Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0) {
            bStatus = false;
        }
    }
    else if (O2_FLOWSENSOR == i2cOrder)
    {
        if (Chip_I2CM_XferBlocking(LPC_I2C1, &i2cData) == 0) {
            bStatus = false;
        }
    }
    else
    {
        ASSERTION(false);
    }

    return bStatus;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCRC8 (const void* buff, uint16_t size)
//
//    Processing:   calculate CRC
//
//    Input Parameters: const void* buff, uint16_t size
//
//    Return Values:     CRC of data
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
uint8_t FlowSensor::GetCRC8 (const void* buff, uint16_t size)
{
    uint8_t *p = (uint8_t *)buff;
    uint8_t crc8;

    for ( crc8 = 0x00 ; size != 0 ; size-- ){
        crc8 ^= *p++;

        for ( int i = 0 ; i < 8 ; i++ ){
            if ( crc8 & 0x80 ){
                crc8 <<= 1; crc8 ^= MSB_CRC8;
            }
            else{
                crc8 <<= 1;
            }
        }
    }
    return crc8;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentReading()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
LONG  FlowSensor::GetCurrentReading(void)
{
    uint8_t GetDataFlow[2] = {0x10, 0x00};
    uint8_t TempData[3] = {};

    I2C_Write(SENSOR_I2C_SEND_ADDR >> 1, GetDataFlow, 2);

    I2C_Read(SENSOR_I2C_READ_ADDR >> 1, TempData, 3);

    int16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];

    // flow = (measure - offset) / scale
    // offset = 32 000
    // scale (air, N2) = 140
    // scale (02) = 142.8
    flow = (flow - 32000) * 100 / 140;

    CurrentReading = (LONG)(flow);
    return CurrentReading;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCompensatedReading()
//
//    Processing: 	Get the data at input address
//
//    Input Parameters: the address need to get data
//
//    Return Values:     data at input address
//
//    Pre-Conditions:    None
//    Miscellaneous:     None
//
//    Requirements:  
//
//******************************************************************************
LONG   FlowSensor::GetCompensatedReading(void)
{
    return CurrentReading;
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

LONG FlowSensor::GetLastReading(void)
{
    // Copy current reading to address specified
    return (CurrentReading);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetTemperatureFactor()
//
//    Processing:  This method reads the Temperature input signal.
//      It then uses the Counts value as an index into a table of
//      pre-calculated temperature compensation factors which are used
//      as part of the flow rate calculation in the GetCurrentReading() method.
//
//    Input Parameters:     None
//    Output Parameters:    None
//    Return Values:        None
//    Pre-Conditions:       None
//    Miscellaneous:        None
//    Requirements:
//
//******************************************************************************

void FlowSensor::SetTemperatureFactor(void)
{
    return;
}
