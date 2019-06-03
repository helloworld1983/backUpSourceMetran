/********************************************************************/
/*                                                                  */
/* File Name     : PressureSensor.cpp                               */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
using ::testing::TestWithParam;

#define VDDA 3.3
#define POLYNOMIAL  0x131

#define DIFF_PRESSURE_CONST 1200.0
#define DIFFSENSOR_I2C_SEND_ADDR 0x80
#define DIFFSENSOR_I2C_READ_ADDR 0x81
#define DIFFSENSOR_GET_VALUE_CMD_LENGTH 1
#define DIFFSENSOR_RESET_CMD_LENGTH 1
#define DIFFSENSOR_GET_VALUE_RESPONSE_LENGTH 3
#define DIFFSENSOR_L_BYTE 1
#define DIFFSENSOR_H_BYTE 0
#define DIFFSENSOR_CRC_BYTE 2
const float a1 = 0.004091;
const float a2 = -0.008182;
const float b1 = 0.09091;
const float b2 = 0.09091;


#define PORTABLE_ADC_ID                 0 /*ADC ID use for Portable*/
#define PROVIDE_PRESSURE_ADC_CH         5 /*Productank pressure chanel*/
#define TANK_PRESSURE_ADC_CH            2 /*Compressor positive pressure*/
#define VACUUM_PRESSURE_ADC_CH          6 /*Cannula pressure sensor */
#define ILLU_SENSOR_ADC_CH              7 /*Illuminance sensor*/
#define DRV_TEMP_ADC_CH                 1
#define KEY1_ADC_CH                     4
#define KEY0_ADC_CH                     3

bool pressure_checkCRC( unsigned char *data, unsigned char numberOfBytes, unsigned char checksum)
{
    unsigned char bit;
    unsigned char crc = 0x00;
    unsigned char byteCounter;
    for(byteCounter = 0; byteCounter < numberOfBytes; byteCounter++)
    {
        crc ^= (data[byteCounter]);
        for(bit = 8; bit > 0; --bit)
        {
            if(crc & 0x80)
                crc = (crc << 1) ^ POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    if(crc != checksum)
        return true;
    else
        return false;
}

class PressureSensorUnitTest : public ::testing::Test
{
};
TEST_F(PressureSensorUnitTest, PressureSensorUnitTestCase)
{
    unsigned char Data[255];
    for(int i = 0; i < 255; ++i)
    {
        Data[i] = i;
    }

    EXPECT_EQ(false, pressure_checkCRC(Data, 1, 0));
    EXPECT_EQ(false, pressure_checkCRC(Data, 3, 150));
    EXPECT_EQ(false, pressure_checkCRC(Data, 255, 144));

    EXPECT_EQ(true, pressure_checkCRC(Data, 1, 1));
    EXPECT_EQ(true, pressure_checkCRC(Data, 3, 151));
    EXPECT_EQ(true, pressure_checkCRC(Data, 255, 145));

}

float pressure_ConvertVoltToKpa(float volt, uint8_t channelIndex)
{
    float result = 0.0;
    switch (channelIndex) {
    case PROVIDE_PRESSURE_ADC_CH:
        result = (1/a1) * ((volt / VDDA) -b1);
        break;
    case TANK_PRESSURE_ADC_CH:
        result = (1/a1) * ((volt / VDDA) -b1);
        break;
    case VACUUM_PRESSURE_ADC_CH:
        result = (1/a2) * ((volt / VDDA) -b2);
        break;
    default:
        break;
    }
    printf(" kq = %f \n", result);
    return result;
}
struct ConvertVoltToKpa
{
        float volt;
        uint8_t channelIndex;
        float result;
};

struct ConvertVoltToKpa ConvertVoltToKpaTestCase[] =
{
        {1, PROVIDE_PRESSURE_ADC_CH, 51.850471},
        {1, TANK_PRESSURE_ADC_CH, 51.850471},
        {1, VACUUM_PRESSURE_ADC_CH, -25.925236}
};

class ConvertVoltToKpaTest : public ::testing::TestWithParam<ConvertVoltToKpa>
{
};

TEST_P(ConvertVoltToKpaTest, ConvertVoltToKpaTestcase)
{
    EXPECT_FLOAT_EQ(GetParam().result, pressure_ConvertVoltToKpa(GetParam().volt, GetParam().channelIndex));
}
INSTANTIATE_TEST_CASE_P(ConvertVoltToKpaTest,
                        ConvertVoltToKpaTest,
                        testing::ValuesIn(ConvertVoltToKpaTestCase)
);
