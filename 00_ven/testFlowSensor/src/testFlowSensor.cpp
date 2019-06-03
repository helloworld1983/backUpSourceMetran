#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include "cr_start_m0.h"
#endif


#include "FreeRTOS.h"
#include "task.h"

static void I2C_init()
{
    Board_I2C_Init(I2C0);
}

bool I2C1_Read(uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = NULL;
    i2cData.txSz = 0;
    i2cData.rxBuff = buf;
    i2cData.rxSz = len;

    if (Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0) {
        return false;
    }
    return true;
}

bool I2C1_Write(uint32_t addr, uint8_t* buf, uint32_t len)
{
    I2CM_XFER_T i2cData;

    i2cData.slaveAddr = addr;
    i2cData.options = 0;
    i2cData.status = 0;
    i2cData.txBuff = buf;
    i2cData.txSz = len;
    i2cData.rxBuff = NULL;
    i2cData.rxSz = 0;

    if (Chip_I2CM_XferBlocking(LPC_I2C0, &i2cData) == 0) {
        return false;
    }
    return true;
}

#define SENSOR_I2C_SEND_ADDR 0x80
#define SENSOR_I2C_READ_ADDR 0x81
#define MSB_CRC8 0x31

uint8_t GetCRC8( const void *buff, size_t size )
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

static void goiHam(void*p)
{
    Board_UARTDebug("goiHam\n");
    static bool staus = true;

    Chip_GPIO_Init(LPC_GPIO_PORT);

    Chip_SCU_PinMuxSet(0xF, 8, (SCU_MODE_FUNC4));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 22);
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 22,true);

    static bool status = true;
    //  SDA GND  =====> VDD(5V)  SCL
    //  HEADER J5: SDA = pin 25
    //  HEADER J5: SCL = pin 26
    //if the supply drops below 4.75V and back, the sensor will perform a Power on Reset and will stop measuring.
    // Make sure VDD - GND >= 5 or measure incorrectly


    while(1)
    {
        Board_UARTDebug("GOI HAM\n");
        Board_LED_Set(0, staus);
        staus = !staus;

        Chip_GPIO_WritePortBit(LPC_GPIO_PORT,7, 22,status);

        //soft reset device
        uint8_t ResetDevice[2] = {0x20, 0x00};
//  The I2C address is represented by a 7-digit binary value. By default, the I2C address is set to 64 (1000000b) . The address is
//  always followed by a write bit (0) or read bit (1). The default hexadecimal I2C header for read access to the sensor is therefore
//  0x81
        I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, ResetDevice, 2);
        vTaskDelay(100/portTICK_PERIOD_MS);

        //Get serial add
        uint8_t sendCmd[2] = {0x31, 0xAE};
        uint8_t SerialData[6] = {};



        I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, sendCmd, 2);
        vTaskDelay(1/portTICK_PERIOD_MS);
        I2C1_Read(SENSOR_I2C_READ_ADDR >> 1, SerialData, 6);
        for(int i = 0; i < 6; ++i)
        {
            Board_UARTDebug("%x  ",SerialData[i]);

        }

        Board_UARTDebug(" ==> Serial = %x %x %x %x ", SerialData[0], SerialData[1], SerialData[3], SerialData[4]);
        Board_UARTDebug("\n");



        uint8_t TempData[3] = {};
        uint8_t GetDataFlow[2] = {0x10, 0x00};
        I2C1_Write(SENSOR_I2C_SEND_ADDR >> 1, GetDataFlow, 2);
        vTaskDelay(100/portTICK_PERIOD_MS);
        I2C1_Read(SENSOR_I2C_READ_ADDR >> 1, TempData, 3);

        for(int i = 0; i < 3; ++i)
        {
            Board_UARTDebug("%x ", TempData[i]);
        }
        int16_t flow = ((uint16_t)TempData[0] << 8) + (uint16_t)TempData[1];
        Board_UARTDebug("%x %d ", flow, flow);

        // flow = (measure - offset) / scale
        // offset = 32 000
        // scale (air, N2) = 140
        // scale (02) = 142.8
        flow = (flow - 32000) * 100 / 140;
        Board_UARTDebug("100 * flow = %d ", flow);


        Board_UARTDebug("\n");







        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}
void TestTask(void)
{
    xTaskCreate(goiHam, "goiHam", 5*configMINIMAL_STACK_SIZE,NULL, (tskIDLE_PRIORITY + 5UL), (TaskHandle_t *) NULL);

}


static void main_tasks (void)
{
    const int FOREVER = 1;
    TestTask();
    vTaskStartScheduler ();
    //Control should never come here
    while (FOREVER)
    {
    }
}


int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
#if defined (__MULTICORE_MASTER) || defined (__MULTICORE_NONE)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();



#endif
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    // Start M0APP slave processor
#if defined (__MULTICORE_MASTER_SLAVE_M0APP)
    cr_start_m0(SLAVE_M0APP,&__core_m0app_START__);
#endif

    // Start M0SUB slave processor
#if defined (__MULTICORE_MASTER_SLAVE_M0SUB)
    cr_start_m0(SLAVE_M0SUB,&__core_m0sub_START__);
#endif


    I2C_init();

    main_tasks ();
    return 0 ;
}
