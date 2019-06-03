/*
 * DeviceTask.c
 *
 *  Created on: Dec 3, 2018
 *      Author: qsbk0
 */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_uart.h"
#include "fsl_debug_console.h"
#include "DeviceTask.h"
#include "RS485Sensor.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"
#include "DeviceInterface.h"
#include "Delay.h"

#define DEVICE_TASK_STACK_SIZE 1024

#define I2C_BAUDRATE (100000) /* 100K */
//for adc
//#define I2C_MASTER_SLAVE_ADDR_7BIT 0x48
//for I2C- Flow
//#define I2C_MASTER_SLAVE_ADDR_7BIT 0x40
//for board charge
//#define I2C_MASTER_SLAVE_ADDR_7BIT 0x40
#define TEST_CHARGE

#ifdef TEST_CHARGE
#define MANUFACTURE_ID                 (0xFE)
#define DEVICE_ID                      (0xFF)

#define CHARGE_OPTION                  (0x12)
#define CHARGE_CURRENT                 (0x14)
#define CHARGE_VOL                     (0x15)
#define INPUT_CURRENT                  (0x3F)

#define MANUFACTURE_ID_ADD                 (MANUFACTURE_ID >> 1)
#define DEVICE_ID_ADD                      (DEVICE_ID >> 1)

#define I2C_MASTER_SLAVE_ADDR_7BIT         (0x12 >> 1)

#define CHARGE_CURRENT_MASK         (0x1fc0)
#define CHARGE_VOLTAGE_MASK         (0x7ff0)
#define INPUT_CURRENT_MASK          (0x1f80)

//ChargeOption 0x12
#define OPTION_CHARGE_INHIBIT           (1 << 0)
#define OPTION_ACOC_THRESHOLD           (1 << 1)
#define OPTION_BOOST_MODE_STATE         (1 << 2)
#define OPTION_BOOST_MODE_ENABLE        (1 << 3)
#define OPTION_ACDET_STATE              (1 << 4)
#define OPTION_IOUT_SELECTION           (1 << 5)
#define OPTION_LEARN_ENABLE             (1 << 6)
#define OPTION_IFAULT_LOW_THRESHOLD     (1 << 7)
#define OPTION_IFAULT_HI_ENABLE         (1 << 8)
#define OPTION_EMI_FREQ_ENABLE          (1 << 9)
#define OPTION_EMI_FREQ_ADJ             (1 << 10)
#define OPTION_BAT_DEPLETION_THRESHOLD  (3 << 11)
#define OPTION_WATCHDOG_TIMER           (3 << 13)
#define OPTION_ACPRES_DEGLITCH_TIME     (1 << 15)

// OPTION_ACOC_THRESHOLD
#define ACOC_THRESHOLD_DISABLE          (0 << 1)
#define ACOC_THRESHOLD_133X             (1 << 1)

// OPTION_IFAULT_LOW_THRESHOLD
#define IFAULT_LOW_135MV_DEFAULT        (0 << 7)
#define IFAULT_LOW_230MV                (1 << 7)

// OPTION_BAT_DEPLETION_THRESHOLD */
#define FALLING_THRESHOLD_5919          (0 << 11)
#define FALLING_THRESHOLD_6265          (1 << 11)
#define FALLING_THRESHOLD_6655          (2 << 11)
#define FALLING_THRESHOLD_7097_DEFAULT  (3 << 11)

// OPTION_WATCHDOG_TIMER */
#define CHARGE_WATCHDOG_DISABLE         (0 << 13)
#define CHARGE_WATCHDOG_44SEC           (1 << 13)
#define CHARGE_WATCHDOG_88SEC           (2 << 13)
#define CHARGE_WATCHDOG_175SEC_DEFAULT  (3 << 13)

// OPTION_ACPRES_DEGLITCH_TIME */
#define ACPRES_DEGLITCH_150MS           (0 << 15)
#define ACPRES_DEGLITCH_1300MS_DEFAULT  (1 << 15)

#endif

static TaskHandle_t s_devTaskHandle = NULL;

static void devTsk_DeInit()
{
#ifdef TEST_SPI
    spi_RtosDeInit();
#endif

}
#ifdef TEST_SPI
void init_motor(void)
{
    PRINTF("init motor\n");
    SendData(0x84, 0x4440006B); //A: 26x-SPI, B: 26x-S/D
    SendData(0xB1, 0x00B71B00); // clk 7A1200/8Mhz, 0x00B71B00/12MHz, 0x00F42400/16Mhz
    SendData(0x80, 0x00006026); // direct-a, direct-bow
    SendData(0x90, 0x00010001); // Steplength

    //Configure the TMC26x via cover datagrams

    SendData(0xEC, 0x00000000); // Using cover datagram to write to DRVCTRL of TMC26x  SendData
    // single edge steps, disable step interpolation, microstep resolution: 256
    SendData(0xEC, 0x00091935); // Using cover datagram to write to CHOPCONF of TMC26x
    // tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
    SendData(0xEC, 0x000A0000); // Using cover datagram to write to SMARTEN of TMC26x
    // Disable Smarten Register
    SendData(0xEC, 0x000D0509); // Using cover datagram to write to SGCSCONF of TMC26x
    // SGT=0,CS=24
    SendData(0xEC, 0x000EF040); // Using cover datagram to write to DRVCONF of TMC26x
    // SLPH = 3, SLPL = 3, DISS2G = off, TS2G = 0-3.2??s, SDOFF = on, VSENSE = 0

    SendData(0xA4, 0x00000000); // v = 0
    SendData(0xA1, 0x00000000); // xactual = 0
    SendData(0xB7, 0x00000000); // xtarget = 0

    SendData(0xA0, 0x00000000); // S-Ramp + POS Mode 6 RAM \ 4 NO RAM \ 0 Velocitiy mode
    SendData(0xA4, 0x03E80000); // VMAX =  [pps] 03E80000 256K
    SendData(0xA8, 0x00001000); // AMAX F42400 4M
    SendData(0xA9, 0x00001000); // DMAX F42400
}
#endif

#ifdef TEST_CHARGE

void ReadCMD(uint8_t add, uint8_t* cmd, uint8_t *recData, uint8_t len){

    i2c_Write(I2C3, add, cmd, 1);
    delay_us(100);
    i2c_Read(I2C3, add, recData, len);
}
void WriteCMD(uint8_t add, uint8_t* cmd, uint8_t *Data, uint8_t len){
    int8_t writeData[3] = {'\0'};
    writeData[0] = *cmd;
    for(int i = 0; i < len; ++i){
        writeData[i + 1] = Data[i];
    }
    PRINTF("WriteData %x %x %x", writeData[0], writeData[1], writeData[2]);

    i2c_Write(I2C3, add, writeData, len + 1);
    delay_us(100);
}
uint16_t GetDeviceID(void){
    uint16_t DeviceID = 0;

    uint8_t GetDeviceIdCMD[1] = {DEVICE_ID};
    uint8_t recData[2] = {'\0'};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, GetDeviceIdCMD, recData, 2);

    DeviceID = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);

    PRINTF("Device ID = 0x%x \n", DeviceID);
    return DeviceID;
}

uint16_t GetManufactureId(void){
    uint16_t ManufactureId = 0;

    uint8_t GetManufactureIdCMD[1] = {MANUFACTURE_ID};
    uint8_t recData[2] = {'\0'};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, GetManufactureIdCMD, recData, 2);

    ManufactureId = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);

    PRINTF("ManufactureId = 0x%x \n", ManufactureId);
    return ManufactureId;
}

uint16_t GetInputCurrent(void){
    uint16_t InPutCurrent = 0;

    uint8_t recData[2] = {'\0'};
    uint8_t InputCMD[1] = {INPUT_CURRENT};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, InputCMD, recData, 2);

    InPutCurrent = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);
    PRINTF("GetChargeVol = 0x%x\n", InPutCurrent);
    return InPutCurrent;
}

void SetInputCurrent(uint16_t input_current){
    uint8_t OptionCMD[1] = {INPUT_CURRENT};
    uint8_t data[2] = {'\0'};

    data[0] = (uint8_t)(input_current);
    data[1] = (uint8_t)(input_current  >> 8);
    PRINTF("SetChargeVol = 0x%x %x\n", data[1], data[0]);

    WriteCMD(I2C_MASTER_SLAVE_ADDR_7BIT, OptionCMD, data, 2);
}

uint16_t GetOption(void){
    uint16_t Option = 0;

    uint8_t recData[2] = {'\0'};
    uint8_t InputCMD[1] = {CHARGE_OPTION};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, InputCMD, recData, 2);

    Option = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);
    PRINTF("GetOption = 0x%x\n", Option);
    return Option;
}
void SetOption(uint16_t Option){
    uint8_t OptionCMD[1] = {CHARGE_OPTION};
    uint8_t data[2] = {'\0'};

    data[0] = (uint8_t)(Option);
    data[1] = (uint8_t)(Option >> 8);
    PRINTF("SetOption = 0x%x %x\n", data[1], data[0]);

    WriteCMD(I2C_MASTER_SLAVE_ADDR_7BIT, OptionCMD, data, 2);
}

uint16_t GetChargeCurrent(void){
    uint16_t ChargeCurrent = 0;

    uint8_t recData[2] = {'\0'};
    uint8_t InputCMD[1] = {CHARGE_CURRENT};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, InputCMD, recData, 2);

    ChargeCurrent = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);
    PRINTF("Get ChargeCurrent = 0x%x\n", ChargeCurrent);
    return ChargeCurrent;
}
void SetChargeCurrent(uint16_t iCurrent){
    uint8_t OptionCMD[1] = {CHARGE_CURRENT};
    uint8_t data[2] = {'\0'};

    data[0] = (uint8_t)(iCurrent);
    data[1] = (uint8_t)(iCurrent  >> 8);
    PRINTF("SetChargeCurrent = 0x%x %x\n", data[1], data[0]);

    WriteCMD(I2C_MASTER_SLAVE_ADDR_7BIT, OptionCMD, data, 2);
}
void SetChargeVol(uint16_t iVol){
    uint8_t OptionCMD[1] = {CHARGE_VOL};
    uint8_t data[2] = {'\0'};

    data[0] = (uint8_t)(iVol);
    data[1] = (uint8_t)(iVol  >> 8);
    PRINTF("SetChargeVol = 0x%x %x\n", data[1], data[0]);

    WriteCMD(I2C_MASTER_SLAVE_ADDR_7BIT, OptionCMD, data, 2);
}
uint16_t GetChargeVol(void){
    uint16_t ChargeVol = 0;

    uint8_t recData[2] = {'\0'};
    uint8_t InputCMD[1] = {CHARGE_VOL};

    ReadCMD(I2C_MASTER_SLAVE_ADDR_7BIT, InputCMD, recData, 2);

    ChargeVol = (uint16_t)((uint16_t)(recData[1] << 8) + (uint16_t)recData[0]);
    PRINTF("GetChargeVol = 0x%x\n", ChargeVol);
    return ChargeVol;
}

#endif

static void devTsk_loop(void *param)
{
#ifdef TEST_SPI
    init_motor();

    uint32_t demo = 0;
    ReadData(0x31, &demo);
#endif

#ifdef TEST_I2C
    i2c_Init(I2C3, I2C_BAUDRATE);

    //test ADC
    uint8_t GetADC[1] = {0x84};
    uint8_t result[2] = {'\0'};

    //test I2C- read flow
    uint8_t GetDataFlow[2] = {0x10, 0x00};
    uint8_t Rawflow[3] = {'\0'};

    //test board char
#ifdef TEST_CHARGE
    PRINTF("Test Charge Board\n");
//    GetDeviceID();
//    GetManufactureId();

    uint16_t Option = 0x8902;
    PRINTF("Option = %x\n", Option);
    SetOption(Option);
    GetOption();

    SetChargeVol(0x4000);
    SetChargeCurrent((1 << 10) & CHARGE_CURRENT_MASK);

    GetChargeVol();
    GetChargeCurrent();

//    uint8_t setOption[3] = {0x12,0x02,0x89};
//    i2c_Write(I2C3,0x12>>1,setOption, 3);
//    delay_us(100);
//
//    uint8_t readOption[1] = {0x12};
//    uint8_t readOptionData[2] = {'\0'};
//    i2c_Write(I2C3,0x12>>1,readOption, 1);
//    delay_us(100);
//    i2c_Read(I2C3, 0x12>>1, readOptionData, 2);
//    PRINTF("readOptionData = %x %x \n", readOptionData[0], readOptionData[1]);

//    uint8_t chargeVoltage[3] = {0x15,0x00,0x40};
//    uint8_t chargeCurrent[3] = {0x14,0x00,0x02};
//    i2c_Write(I2C3,0x12>>1,chargeVoltage,3);
//    delay_us(100);
//    i2c_Write(I2C3,0x12>>1,chargeCurrent,3);
//    delay_us(100);
//
//    uint8_t readChargeVoltage[1] = {0x15};
//    uint8_t readChargeVoltageData[2]={'\0'};
//    i2c_Write(I2C3,0x12>>1,readChargeVoltage,1);
//    delay_us(100);
//    i2c_Read(I2C3, 0x12>>1, readChargeVoltageData, 2);
//    PRINTF("readChargeVoltageData = %x %x \n", readChargeVoltageData[0], readChargeVoltageData[1]);
//
//    uint8_t readChargeCurrent[1] = {0x14};
//    uint8_t readChargeCurrentData[2]={'\0'};
//    i2c_Write(I2C3,0x12>>1,readChargeCurrent,1);
//    delay_us(100);
//    i2c_Read(I2C3, 0x12>>1, readChargeCurrentData,2);
//    PRINTF("readChargeCurrentData = %x %x \n", readChargeCurrentData[0], readChargeCurrentData[1]);


#endif


#endif

#ifdef TEST_UART
    uint8_t recv[4] = {'\0'};
#endif
    //    PRINTF("devTsk_loop\n");

    for (;;)
    {
#ifdef TEST_UART
        int error = uart_Read(TEST_UART, recv, sizeof(recv));

        if (kStatus_Success == error){
            /* send back the received data */
            uart_Write(TEST_UART, (uint8_t *)recv, sizeof(recv));
        }
#endif
#ifdef TEST_SPI
        PRINTF("TEST MOTOR\n");
        ReadData(0x31, &demo);
        //		PRINTF("TEST_SPI LOOP BACK\n");
        //		uint32_t masterTxData[64] = {0};
        //		uint32_t masterRxData[64] = {0};
        //
        //		for (int i = 0; i < 64; i++)
        //		{
        //		    masterTxData[i] = i;
        //		}
        //
        //		status_t status = spi_Transfer(masterTxData, masterRxData, 64);
        //		delay_ms(1);
        //
        //		// Compare Tx and Rx data
        //		int err = 0;
        //		for (int i = 0; i < 64; i++)
        //		{
        //		    if (masterTxData[i] != masterRxData[i])
        //		    {
        //	            PRINTF("masterTxData = %x masterRxData = %x \n", masterTxData[i], masterRxData[i]);
        //		        err++;
        //		    }
        //		}
        //
        //		if (status != kStatus_Success)
        //		{
        //		    PRINTF("ECSPI transfer completed with error. \r\n");
        //		    vTaskSuspend(NULL);
        //		}
        //
        //		if(err != 0){
        //		    PRINTF("Test FAIL \r\n\r\n");
        //		}else{
        //		    PRINTF("Test PASS \r\n\r\n");
        //		}
#endif

#ifdef TEST_I2C
        //        PRINTF("test ADC\n");
        //
        //        i2c_Write(I2C3, I2C_MASTER_SLAVE_ADDR_7BIT, GetADC, 1);
        //
        //        delay_us(10);
        //
        //        i2c_Read(I2C3, I2C_MASTER_SLAVE_ADDR_7BIT, result, 2);
        //
        //        int raw_adc = ((result[0] & 0x0F) * 256 + result[1]);
        //        PRINTF(" RAW ADC(%x %x )  = %d \n", result[0], result[1], raw_adc);
        //        PRINTF(" ADC(x1000)  = %d \n", raw_adc * 3300 / 4096);

//                PRINTF("test I2C ReadFlow \n");
//                i2c_Write(I2C3, I2C_MASTER_SLAVE_ADDR_7BIT, GetDataFlow, 2);
//                delay_us(500);
//
//                i2c_Read(I2C3, I2C_MASTER_SLAVE_ADDR_7BIT, Rawflow, 3);
//
//                int16_t flow = ((uint16_t)Rawflow[0] << 8) + ((uint16_t)Rawflow[1]);
//                flow = (flow - 32000) * 100 / 140;
//
//                PRINTF("Flow = %d ml/s \n", flow);
#ifdef TEST_CHARGE
        PRINTF(".\n");
//        delay_us(10);
//        GetOption();
//
//        SetChargeVol((1 << 8) & CHARGE_VOLTAGE_MASK);
//
//        delay_us(100);
//        GetChargeVol();
//
//        SetChargeCurrent((1 << 9) & CHARGE_CURRENT_MASK);
//        delay_us(100);
//        GetChargeCurrent();
//
//        SetInputCurrent((1 << 10) & INPUT_CURRENT_MASK);
//        GetInputCurrent();
//        delay_us(10);
#endif


#endif
        devIf_HandleMsgQueueRecv();
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    devTsk_DeInit();
    return;
}

/*!
 * @brief ipc_create function
 */
void devTsk_Create(void)
{
    devIf_Init();
    if (xTaskCreate(devTsk_loop, "DEV_TASK", DEVICE_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &s_devTaskHandle) != pdPASS)
    {
        PRINTF("\r\nFailed to create application task\r\n");
        while (1)
            ;
    }
    return;
}

void devTsk_Init(void)
{
#ifdef TEST_UART
    uart_Init(TEST_UART,115200u);
#endif

#ifdef TEST_SPI
    spi_HwInit();
    spi_RtosInit();
#endif

}
