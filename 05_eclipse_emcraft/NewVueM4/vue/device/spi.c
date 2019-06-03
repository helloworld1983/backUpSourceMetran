/*
 * spi.c
 *
 *  Created on: Dec 17, 2018
 *      Author: qsbk0
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_ecspi.h"
#include "fsl_ecspi_freertos.h"

#include "fsl_gpio.h"

#define ECSPI_MAX_TRANSFER_SIZE 64
#define ECSPI_TRANSFER_BAUDRATE 1500000
#define ECSPI_MASTER_BASEADDR ECSPI1
#define ECSPI_MASTER_CLK_FREQ  \
        CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootEcspi1)) / \
        (CLOCK_GetRootPostDivider(kCLOCK_RootEcspi1))
#define EXAMPLE_ECSPI_MASTER_IRQN ECSPI1_IRQn

/* Task priorities. */
#define ecspi_task_PRIORITY (configMAX_PRIORITIES - 1)

#define CS_GPIO GPIO3 //P3.63 _ GPIO 1- PIN 13
#define CS_GPIO_PIN 25U

ecspi_master_config_t masterConfig;
ecspi_transfer_t masterXfer;
ecspi_rtos_handle_t master_rtos_handle;

ecspi_master_handle_t *g_m_handle;

void spi_HwInit(void)
{
    CLOCK_SetRootMux(kCLOCK_RootEcspi1, kCLOCK_EcspiRootmuxSysPll1); /* Set ECSPI1 source to SYSTEM PLL1 800MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootEcspi1, 2U, 5U);                  /* Set root clock to 800MHZ / 10 = 80MHZ */
    /* Set IRQ priority for freertos_ecspi */
    NVIC_SetPriority(EXAMPLE_ECSPI_MASTER_IRQN, 2);

    gpio_pin_config_t cs_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    //disable css PIN
    GPIO_PinInit(CS_GPIO, CS_GPIO_PIN, &cs_config);
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

void spi_RtosInit(void)
{
    status_t status = kStatus_Fail;

    ECSPI_MasterGetDefaultConfig(&masterConfig);

    masterConfig.baudRate_Bps = ECSPI_TRANSFER_BAUDRATE;

    masterConfig.enableLoopback = false;
    masterConfig.burstLength = 8;

//    masterConfig.channelConfig.chipSlectActiveState = kECSPI_ClockInactiveStateHigh;
//    masterConfig.channelConfig.polarity = kECSPI_PolarityActiveLow;
//    masterConfig.channelConfig.phase = kECSPI_ClockPhaseSecondEdge;

    status = ECSPI_RTOS_Init(&master_rtos_handle, ECSPI_MASTER_BASEADDR, &masterConfig, ECSPI_MASTER_CLK_FREQ);

    if (status != kStatus_Success)
    {
        PRINTF("ECSPI meets error during initialization. \r\n");
        vTaskSuspend(NULL);
    }

    g_m_handle = &master_rtos_handle.drv_handle;
    memset(&masterXfer, 0, sizeof(masterXfer));
}

void spi_RtosDeInit(void)
{
     ECSPI_RTOS_Deinit(&master_rtos_handle);
}

int16_t spi_Transfer(uint32_t* tx, uint32_t* rx, uint32_t size)
{
    if(size > ECSPI_MAX_TRANSFER_SIZE)
    {
        assert(false);
        return -1;
    }

    status_t status;

    g_m_handle = &master_rtos_handle.drv_handle;
    memset(&masterXfer, 0, sizeof(masterXfer));

    masterXfer.txData = tx;
    masterXfer.rxData = rx;
    masterXfer.dataSize = size;
    masterXfer.channel = kECSPI_Channel0;

    status = ECSPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);

    if (status != kStatus_Success)
    {
        PRINTF("ECSPI transfer completed with error. \r\n\r\n");
        vTaskSuspend(NULL);
    }
    return 0;
}

void SendData(uint8_t address, uint32_t datagram)
{
    uint32_t Tx_Buf[5] = {'\0'};
    uint32_t Rx_Buf[5] = {'\0'};

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);
    delay_us(10);

    Tx_Buf[0] = address & 0x000000FF;

    Tx_Buf[1] = (uint32_t)((datagram >> 24) & 0x000000FF);
    Tx_Buf[2] = (uint32_t)((datagram >> 16) & 0x000000FF);
    Tx_Buf[3] = (uint32_t)((datagram >> 8) & 0x000000FF);
    Tx_Buf[4] = (uint32_t)((datagram)&0x000000FF);

    spi_Transfer(Tx_Buf, Rx_Buf, 5);

    delay_us(130);

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

void ReadData(uint8_t address, uint32_t *datagram)
{
    PRINTF("ReadData: add =  %x datagram = %x \n", address, *datagram);

    uint32_t Tx_Buf[5];
    Tx_Buf[0] = address;
    Tx_Buf[1] = 0;
    Tx_Buf[2] = 0;
    Tx_Buf[3] = 0;
    Tx_Buf[4] = 0;
    uint32_t Rx_Buf[5] = {'\0'};

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);
    delay_us(10);

    spi_Transfer(&Tx_Buf[0], &Rx_Buf[0], 1);

    spi_Transfer(&Tx_Buf[1], &Rx_Buf[1], 1);
    spi_Transfer(&Tx_Buf[2], &Rx_Buf[2], 1);
    spi_Transfer(&Tx_Buf[3], &Rx_Buf[3], 1);
    spi_Transfer(&Tx_Buf[4], &Rx_Buf[4], 1);


    PRINTF("read =  (%x) %x %x %x %x \n", Rx_Buf[0], Rx_Buf[1], Rx_Buf[2], Rx_Buf[3], Rx_Buf[4], Rx_Buf[5]);

    Rx_Buf[1] = (uint32_t)((Rx_Buf[1] << 24));
    Rx_Buf[2] = (uint32_t)((Rx_Buf[2] << 16));
    Rx_Buf[3] = (uint32_t)((Rx_Buf[3] << 8));
    Rx_Buf[4] = (uint32_t)((Rx_Buf[4]));

    *datagram = Rx_Buf[1] + Rx_Buf[2] + Rx_Buf[3] + Rx_Buf[4];

     PRINTF("F = %x(%d) \n", *datagram, *datagram);

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}
