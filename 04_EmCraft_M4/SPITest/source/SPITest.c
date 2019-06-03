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
#define ECSPI_TRANSFER_BAUDRATE 1500000
// #define ECSPI_TRANSFER_BAUDRATE 500000U
#define ECSPI_MASTER_BASEADDR ECSPI1
#define ECSPI_MASTER_CLK_FREQ                                                                \
    CLOCK_GetPllFreq(kCLOCK_SystemPll1Ctrl) / (CLOCK_GetRootPreDivider(kCLOCK_RootEcspi1)) / \
        (CLOCK_GetRootPostDivider(kCLOCK_RootEcspi1))
#define ECSPI_MASTER_TRANSFER_CHANNEL kECSPI_Channel0

#define CS_GPIO GPIO3 //P3.63 _ GPIO 1- PIN 13
#define CS_GPIO_PIN 25U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void SendData(uint8_t address, uint32_t datagram);
void ReadData(uint8_t address, uint32_t *datagram);

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
void SendData(uint8_t address, uint32_t datagram)
{
    uint32_t Tx_Buf[5] = {'\0'};
    uint32_t Rx_Buf[5] = {'\0'};

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);
    delay(40); //10us

    Tx_Buf[0] = address & 0x000000FF;

    Tx_Buf[1] = (uint32_t)((datagram >> 24) & 0x000000FF);
    Tx_Buf[2] = (uint32_t)((datagram >> 16) & 0x000000FF);
    Tx_Buf[3] = (uint32_t)((datagram >> 8) & 0x000000FF);
    Tx_Buf[4] = (uint32_t)((datagram)&0x000000FF);

    ecspi_transfer_t masterXfer;
    masterXfer.txData = &Tx_Buf[0];
    masterXfer.rxData = &Rx_Buf[0];
    masterXfer.dataSize = 5;
    masterXfer.channel = ECSPI_MASTER_TRANSFER_CHANNEL;

    PRINTF("send data =  %x %x %x %x %x \n", Tx_Buf[0], Tx_Buf[1], Tx_Buf[2], Tx_Buf[3], Tx_Buf[4]);
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    // ECSPI_WriteBlocking(ECSPI_MASTER_BASEADDR, &Tx_Buf[0], 1);
    // ECSPI_WriteBlocking(ECSPI_MASTER_BASEADDR, &Tx_Buf[1], 1);
    // ECSPI_WriteBlocking(ECSPI_MASTER_BASEADDR, &Tx_Buf[2], 1);
    // ECSPI_WriteBlocking(ECSPI_MASTER_BASEADDR, &Tx_Buf[3], 1);
    // ECSPI_WriteBlocking(ECSPI_MASTER_BASEADDR, &Tx_Buf[4], 1);

    delay(500);

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}

void ReadData(uint8_t address, uint32_t *datagram)
{
    uint32_t Tx_Buf[5];
    Tx_Buf[0] = address;
    Tx_Buf[1] = 0;
    Tx_Buf[2] = 0;
    Tx_Buf[3] = 0;
    Tx_Buf[4] = 0;
    uint32_t Rx_Buf[5] = {'\0'};

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 0U);
    delay(40); //10us

    *datagram = 0;
    ecspi_transfer_t masterXfer;
    masterXfer.channel = ECSPI_MASTER_TRANSFER_CHANNEL;
    masterXfer.dataSize = 5;

    masterXfer.txData = &Tx_Buf[0];
    masterXfer.rxData = &Rx_Buf[0];
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    masterXfer.txData = &Tx_Buf[1];
    masterXfer.rxData = &Rx_Buf[1];
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    masterXfer.txData = &Tx_Buf[2];
    masterXfer.rxData = &Rx_Buf[2];
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    masterXfer.txData = &Tx_Buf[3];
    masterXfer.rxData = &Rx_Buf[3];
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    masterXfer.txData = &Tx_Buf[4];
    masterXfer.rxData = &Rx_Buf[4];
    ECSPI_MasterTransferBlocking(ECSPI_MASTER_BASEADDR, &masterXfer);

    PRINTF("read =  %x %x %x %x %x \n", Rx_Buf[0], Rx_Buf[1], Rx_Buf[2], Rx_Buf[3], Rx_Buf[4], Rx_Buf[5]);

    Rx_Buf[1] = (uint32_t)((Rx_Buf[1] << 24));
    Rx_Buf[2] = (uint32_t)((Rx_Buf[2] << 16));
    Rx_Buf[3] = (uint32_t)((Rx_Buf[3] << 8));
    Rx_Buf[4] = (uint32_t)((Rx_Buf[4]));

    *datagram = Rx_Buf[1] + Rx_Buf[2] + Rx_Buf[3] + Rx_Buf[4];

    // PRINTF("F = %x \n", *datagram);

    GPIO_PinWrite(CS_GPIO, CS_GPIO_PIN, 1U);
}
void init_motor(void)
{
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
    SendData(0xA4, 0x03E80000); // VMAX =  [pps] 03E80000 200K
    SendData(0xA8, 0x00001000); // AMAX F42400 4M
    SendData(0xA9, 0x00001000); // DMAX F42400
}

/*!
 * @brief Main function
 */
int main(void)
{
    ecspi_transfer_t masterXfer;
    ecspi_master_config_t masterConfig;
    uint32_t masterRxData[ECSPI_TRANSFER_SIZE] = {0};
    uint32_t masterTxData[ECSPI_TRANSFER_SIZE] = {0};

    gpio_pin_config_t cs_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

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
    masterConfig.burstLength = 8;

    ECSPI_MasterInit(ECSPI_MASTER_BASEADDR, &masterConfig, ECSPI_MASTER_CLK_FREQ);

    //==========================================================
    init_motor();
    while (1)
    {
        // SendData(0xB7, 0x00000200);

        // delay(400);

        // SendData(0xB7, 0x00000400);

        // uint32_t rec = 0;

        // ReadData(0x37, &rec);

        // PRINTF("rec = %x \n", rec);
        // delay(10);
    }

    /* Deinit the ECSPI. */
    ECSPI_Deinit(ECSPI_MASTER_BASEADDR);
}
