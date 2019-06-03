#include "TestFlowSenI2C1.h"

#define SENSOR_I2C_SEND_ADDR 0x80
#define SENSOR_I2C_READ_ADDR 0x81
#define MSB_CRC8 0x31

TestFlowSenI2C1 *TestFlowSenI2C1::S_Instance = NULL;

TestFlowSenI2C1* TestFlowSenI2C1::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new TestFlowSenI2C1();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

void TestFlowSenI2C1::Init (void)
{
    Board_I2C_Init(I2C1);
}

void TestFlowSenI2C1::TestFlowSenI2C1CreateTask (void)
{
    xTaskCreate(Entry, "TestFlowSenI2C1", 5*configMINIMAL_STACK_SIZE,NULL,
                (tskIDLE_PRIORITY + 3UL), (TaskHandle_t *) NULL);
}

bool TestFlowSenI2C1::I2C1_Read (uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = NULL;
    i2cData.txSz = 0;
    i2cData.rxBuff = buf;
    i2cData.rxSz = len;

    if (Chip_I2CM_XferBlocking(LPC_I2C1, &i2cData) == 0) {
        return false;
    }
    return true;
}

bool TestFlowSenI2C1::I2C1_Write (uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = buf;
    i2cData.txSz = len;
    i2cData.rxBuff = NULL;
    i2cData.rxSz = 0;

    if (Chip_I2CM_XferBlocking(LPC_I2C1, &i2cData) == 0) {
        return false;
    }
    return true;
}

uint8_t TestFlowSenI2C1::GetCRC8 (const void* buff, size_t size)
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

void TestFlowSenI2C1::Entry (void* pvParameters)
{
    const int FOREVER = 1;
    while(FOREVER)
    {
        //  SDA GND  =====> VDD(5V)  SCL
        //  HEADER J5: SDA = pin 25
        //  HEADER J5: SCL = pin 26
        //if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring.
        // Make sure VDD - GND >= 5 or measure incorrectly

        //soft reset device
        uint8_t ResetDevice[2] = {0x20, 0x00};
        //  The I2C address is represented by a 7-digit binary value. By default, the I2C address is set to 64 (1000000b) . The address is
        //  always followed by a write bit (0) or read bit (1). The default hexadecimal I2C header for read access to the sensor is therefore
        //  0x81
        TestFlowSenI2C1::S_GetInstance()->I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, ResetDevice, 2);
        vTaskDelay(100/portTICK_PERIOD_MS);

        //Get serial add
        uint8_t sendCmd[2] = {0x31, 0xAE};
        uint8_t SerialData[6] = {};



        TestFlowSenI2C1::S_GetInstance()->I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, sendCmd, 2);
        vTaskDelay(1/portTICK_PERIOD_MS);
        TestFlowSenI2C1::S_GetInstance()->I2C1_Read(SENSOR_I2C_READ_ADDR >> 1, SerialData, 6);
        for(int i = 0; i < 6; ++i)
        {
            DEBUG_FLOWTASKI2C1("%x  ",SerialData[i]);

        }

        DEBUG_FLOWTASKI2C1(" ==> Serial = %x %x %x %x ", SerialData[0], SerialData[1], SerialData[3], SerialData[4]);
        DEBUG_FLOWTASKI2C1("\n");



        uint8_t TempData[3] = {};
        uint8_t GetDataFlow[2] = {0x10, 0x00};
        TestFlowSenI2C1::S_GetInstance()->I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, GetDataFlow, 2);
        vTaskDelay(100/portTICK_PERIOD_MS);
        TestFlowSenI2C1::S_GetInstance()->I2C1_Read(SENSOR_I2C_READ_ADDR >> 1, TempData, 3);

        for(int i = 0; i < 3; ++i)
        {
            DEBUG_FLOWTASKI2C1("%x ", TempData[i]);
        }
        int16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
        DEBUG_FLOWTASKI2C1("%x %d ", flow, flow);

        // flow = (measure - offset) / scale
        // offset = 32 000
        // scale (air, N2) = 140
        // scale (02) = 142.8
        flow = (flow - 32000) * 100 / 140;
        DEBUG_FLOWTASKI2C1("100 * flow = %d ", flow);


        DEBUG_FLOWTASKI2C1("\n");


        vTaskDelay(2/portTICK_PERIOD_MS);
    }
}

TestFlowSenI2C1::TestFlowSenI2C1 (void)
{
}
