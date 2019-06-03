/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_ecspi.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ECSPI_TRANSFER_SIZE 64
#define ECSPI_TRANSFER_BAUDRATE 1000000
// #define ECSPI_TRANSFER_BAUDRATE 500000U
#define ECSPI_MASTER_BASEADDR ECSPI1
#define ECSPI_MASTER_CLK_FREQ                                                                \
    CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootEcspi1)) / \
        (CLOCK_GetRootPostDivider(kCLOCK_RootEcspi1))
#define ECSPI_MASTER_TRANSFER_CHANNEL kECSPI_Channel0

#define CS_GPIO GPIO3
#define CS_GPIO_PIN 25U

#define CONVST_GPIO GPIO3
#define CONVST_PIN 4U

#define RST_GPIO GPIO3
#define RST_PIN 8U

#define BUSY_INT_GPIO GPIO3
#define BUSY_INT_PIN 9U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void SendData(uint32_t data);

void delay(uint32_t time)
{
    for (uint32_t i = 0; i < time; ++i)
    {
        __NOP();
    }
}
/*******************************************************************************
 * Code
 ******************************************************************************/
#define READ_REG_COMMAND (0x4000)
#define WRITE_REG_COMMAND (0x8000)
void ReadReg(uint32_t Reg)
{
    //trigger CONVST
    // GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 0U);
    // delay(1);
    // GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 1U);

    //enable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);

    //write data
    uint32_t Tx_Buf = (uint32_t)(Reg << 10) | (uint32_t)(READ_REG_COMMAND);
    uint32_t Rx_Buf = 0;
    ecspi_transfer_t masterXfer;
    masterXfer.txData = &Tx_Buf;
    masterXfer.rxData = &Rx_Buf;
    masterXfer.dataSize = 1;
    masterXfer.channel = ECSPI_MASTER_TRANSFER_CHANNEL;

    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);
    PRINTF("READ (%x) = %x \n", Tx_Buf, Rx_Buf);

    //delay
    delay(1000);

    //disable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

void WriteReg(uint32_t Reg, uint8_t data)
{
    //trigger CONVST
    // GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 0U);
    // delay(1);
    // GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 1U);

    //enable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);

    //write data
    uint32_t Tx_Buf = (uint32_t)(Reg << 10) | (uint32_t)(WRITE_REG_COMMAND) | (uint32_t)(data);
    uint32_t Rx_Buf = 0;
    ecspi_transfer_t masterXfer;
    masterXfer.txData = &Tx_Buf;
    masterXfer.rxData = &Rx_Buf;
    masterXfer.dataSize = 1;
    masterXfer.channel = ECSPI_MASTER_TRANSFER_CHANNEL;

    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);
    PRINTF("WRITE (%x) = %x \n", Tx_Buf, Rx_Buf);

    //delay
    delay(1000);

    //disable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

void readADC(void)
{
    //trigger CONVST
    GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 0U);
    delay(1);
    GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 1U);

    //enable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);

    //write data

    uint32_t Tx_Buf[2] = {0x0000, 0x0000};
    uint32_t Rx_Buf[2] = 0;
    ecspi_transfer_t masterXfer;
    masterXfer.txData = &Tx_Buf[0];
    masterXfer.rxData = &Rx_Buf[0];
    masterXfer.dataSize = 2;
    masterXfer.channel = ECSPI_MASTER_TRANSFER_CHANNEL;

    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    PRINTF("Read ADC = %x(%d) %x(%d)\n", Rx_Buf[0], Rx_Buf[0], Rx_Buf[1], Rx_Buf[1]);

    int adc = (Rx_Buf[0] >> 4) * 8; //3.3 /4096/10 000

    PRINTF("\tadc = %d %f\n", adc);

    //delay
    delay(100);

    //disable CS
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

int main(void)
{
    ecspi_transfer_t masterXfer;
    ecspi_master_config_t masterConfig;
    uint32_t masterRxData[ECSPI_TRANSFER_SIZE] = {0};
    uint32_t masterTxData[ECSPI_TRANSFER_SIZE] = {0};

    gpio_pin_config_t cs_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    gpio_pin_config_t convst_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    gpio_pin_config_t rst_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

    gpio_pin_config_t busy_config = {kGPIO_DigitalInput, 0, kGPIO_NoIntmode};

    /* Init board hardware. */
    /* Board specific RDC settings */
    BOARD_RdcInit();

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitMemory();

    CLOCK_SetRootMux(kCLOCK_RootEcspi1, kCLOCK_EcspiRootmuxSysPll1); /* Set ECSPI1 source to SYSTEM PLL1 800MHZ */
    CLOCK_SetRootDivider(kCLOCK_RootEcspi1, 2U, 5U);                 /* Set root clock to 800MHZ / 10 = 80MHZ */

    //enable css PIN
    GPIO_PinInit(CS_GPIO, CS_GPIO_PIN, &cs_config);
    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);

    GPIO_PinInit(CONVST_GPIO, CONVST_PIN, &convst_config);
    GPIO_PinWrite(CONVST_GPIO, CONVST_PIN, 1U);

    GPIO_PinInit(RST_GPIO, RST_PIN, &rst_config);
    GPIO_PinWrite(RST_GPIO, RST_PIN, 1U);

    GPIO_PinInit(BUSY_INT_GPIO, BUSY_INT_PIN, &busy_config);

    bool status = true;

    /* Master config:
     * masterConfig.channel = kECSPI_Channel0;
     * masterConfig.burstLength = 8;
     * masterConfig.samplePeriodClock = kECSPI_spiClock;
     * masterConfig.baudRate_Bps = TRANSFER_BAUDRATE;
     * masterConfig.chipSelectDelay = 0;
     * masterConfig.samplePeriod = 0;
     * masterConfig.txFifoThreshold = 1;
     * masterConfig.rxFifoThreshold = 0;
     * masterConfig.enableLoopback = true;
     */
    ECSPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = ECSPI_TRANSFER_BAUDRATE;
    masterConfig.enableLoopback = false;
    masterConfig.burstLength = 16;
    ECSPI_MasterInit(ECSPI_MASTER_BASEADDR, &masterConfig, ECSPI_MASTER_CLK_FREQ);

    //==========================================================
    //hardware reset
    GPIO_PinWrite(RST_GPIO, RST_PIN, 0U);
    delay(10);
    GPIO_PinWrite(RST_GPIO, RST_PIN, 1U);
    delay(10);

    //set to Idle mode
    WriteReg(0x08, 0x00);
    ReadReg(0x08);

    //select channel
    WriteReg(0x04, 0x06);
    ReadReg(0x04);

    ReadReg(1);
    ReadReg(2);
    ReadReg(3);
    ReadReg(4);
    ReadReg(5);
    ReadReg(6);
    ReadReg(7);
    ReadReg(8);
    ReadReg(9);
    ReadReg(10);
    readADC();
    
    ReadReg(1);
    ReadReg(2);
    ReadReg(3);
    ReadReg(4);
    ReadReg(5);
    ReadReg(6);
    ReadReg(7);
    ReadReg(8);
    ReadReg(9);
    ReadReg(10);


    while (1)
    {
        // SendData(0x9005);
        // delay(8000);

        // SendData(0x5000);
        // delay(8000);

        // SendData(0x0000);
        // WriteReg(0x09, 0xAA);
        delay(16000000);
        readADC();
    }

    /* Deinit the ECSPI. */
    ECSPI_Deinit(ECSPI_MASTER_BASEADDR);
}
