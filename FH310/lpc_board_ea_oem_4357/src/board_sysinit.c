/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

#define SDRAM_POW_PORT 0
#define SDRAM_POW_BIT  1
#define SPI_POW_PORT   2
#define SPI_POW_BIT    0

#define P0 0x0
#define P1 0x1
#define P2 0x2
#define P3 0x3
#define P4 0x4
#define P5 0x5
#define P6 0x6
#define P7 0x7
#define P8 0x8
#define P9 0x9
#define PA 0xA
#define PB 0xB
#define PC 0xC
#define PD 0xD
#define PE 0xE
#define PF 0xF

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Structure for initial base clock states */
struct CLK_BASE_STATES {
	CHIP_CGU_BASE_CLK_T clk;	/* Base clock */
	CHIP_CGU_CLKIN_T clkin;	/* Base clock source, see UM for allowable souorces per base clock */
	bool autoblock_enab;/* Set to true to enable autoblocking on frequency change */
	bool powerdn;		/* Set to true if the base clock is initially powered down */
};

/* Initial base clock states are mostly on */
STATIC const struct CLK_BASE_STATES InitClkStates[] = {
		{CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false},
#if defined(USE_RMII)
		{CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false},
#else
		{CLK_BASE_PHY_RX, CLKIN_ENET_RX, true, false},
#endif
		{CLK_BASE_LCD, CLKIN_MAINPLL, true, false},

		/* Clocks derived from dividers */
		{CLK_BASE_USB1, CLKIN_IDIVD, true, true}
};

/* SPIFI high speed pin mode setup */
//Q_comment Different with Takeuchi san, add SCU_PINIO_FAST |
STATIC const PINMUX_GRP_T spifipinmuxing[] = {
		{P3, 3, (SCU_PINIO_FAST |SCU_MODE_FUNC3)},//SPIFI.SCK
		{P3, 4, (SCU_PINIO_FAST |SCU_MODE_FUNC3 )},//SPIFI.SIO3
		{P3, 5, (SCU_PINIO_FAST |SCU_MODE_FUNC3 )},//SPIFI.SIO2
		{P3, 6, (SCU_PINIO_FAST |SCU_MODE_FUNC3 )},//SPIFI.MISO
		{P3, 7, (SCU_PINIO_FAST |SCU_MODE_FUNC3 )},//SPIFI.MOSI
		{P3, 8, (SCU_PINIO_FAST |SCU_MODE_FUNC3 )},//SPIFI.CS
};

STATIC const PINMUX_GRP_T pinmuxing[] = {
		{P0, 0, (SCU_MODE_FUNC0 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{P0, 1, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//EMC.PWR.EN
		{P1, 3, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN)},//PW.SW.DETECT
		{P1, 4, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//PW.BACKUP.EN
		{P1, 5, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC0 | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN)},//POWER DETECT
		{P1, 15, (SCU_MODE_FUNC1 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//U2.TXD
		{P1, 16, (SCU_MODE_FUNC1 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN)},//U2.RXD
		{P1, 17, (SCU_MODE_FUNC4 |SCU_MODE_PULLUP|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN|SCU_MODE_4MA_DRIVESTR)},//IR.RECEIVE T0CAP3
		{P1, 18, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//VALVE5
		{P1, 19, (SCU_MODE_FUNC3 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P1, 20, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//VALVE3
		{P2, 3, (SCU_MODE_FUNC4 |SCU_MODE_INACT)},//USB0.PPWR
		{P2, 4, (SCU_MODE_FUNC4 |SCU_MODE_PULLUP|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{P2, 5, (SCU_MODE_FUNC2 )},//USB1.VBUS
		{P3, 0, (SCU_MODE_FUNC2 | SCU_PINIO_FAST)},//I2S0.TX.SCK //Q_modify follow Board lib
		{P3, 1, (SCU_MODE_FUNC0 | SCU_PINIO_FAST)},//I2S0.TX.WS //Q_modify follow Board lib
		{P3, 2, (SCU_MODE_FUNC0 | SCU_PINIO_FAST)},//I2S0.TX.SDA //Q_modify follow Board lib
		{P4, 0, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//QSPI.PWR.EN
		{P4, 1, (SCU_MODE_FUNC1 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//LCD.BL.DIM
		{P4, 2, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P4, 3, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P4, 4, (SCU_MODE_FUNC0 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//FAN.PWM
		{P4, 5, (SCU_MODE_FUNC2 )},//LCD.FP
		{P4, 6, (SCU_MODE_FUNC2 )},//LCD.EN.CMD
		{P4, 7, (SCU_MODE_FUNC0 )},//LCD.DCLK
		{P4, 8, (SCU_MODE_FUNC4 )},//RED.LED
		{P4, 9, (SCU_MODE_FUNC2 )},//LCD.VD11
		{P4, 10, (SCU_MODE_FUNC2 )},//LCD.VD10
		{P6, 0, (SCU_MODE_FUNC0 )},//
		{P6, 1, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P6, 2, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P6, 3, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P6, 6, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P6, 7, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P7, 0, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P7, 1, (SCU_MODE_FUNC3 )},//LCD.VD19
		{P7, 2, (SCU_MODE_FUNC3 )},//LCD.VD18
		{P7, 3, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P7, 4, (SCU_MODE_FUNC0 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//CU.ENA
		{P7, 5, (SCU_MODE_FUNC0 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN)},//VBAT.SEN
		{P7, 6, (SCU_MODE_FUNC3 )},//LCD.LP
		{P7, 7, (SCU_MODE_FUNC3 )},//LCD.PWR
		{P8, 0, (SCU_MODE_FUNC0 | SCU_MODE_PULLDOWN | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN)},//USB0.PWR.FAULT
		{P8, 1, (SCU_MODE_FUNC0 |SCU_MODE_PULLUP|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN|SCU_MODE_4MA_DRIVESTR)},//SSP0.IRQ
		{P8, 2, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//BAT_CHG
		{P8, 3, (SCU_MODE_FUNC3 )},//LCD.VD12
		{P8, 4, (SCU_MODE_FUNC3 )},//LCD.VD7
		{P8, 5, (SCU_MODE_FUNC3 )},//LCD.VD6
		{P8, 6, (SCU_MODE_FUNC3 )},//LCD.VD5
		{P8, 7, (SCU_MODE_FUNC3 )},//LCD.VD4
		{P8, 8, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P9, 0, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P9, 1, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{P9, 2, (SCU_MODE_FUNC0 )},//FC.RESET
		{P9, 3, (SCU_MODE_FUNC7 )},//U3.TXD
		{P9, 4, (SCU_MODE_FUNC7 )},//U3.RXD
		{P9, 5, (SCU_MODE_FUNC4 )},//USB1.PPWR
		{P9, 6, ( SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN )},//USB1.PWR.FAULT
		{PA, 0, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS)},//
		{PA, 1, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PA, 2, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLDOWN |SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0 )},//ENCODER.B
		{PA, 3, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLDOWN |SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0 )},//ENCODER.A
		{PA, 4, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PB, 0, (SCU_MODE_FUNC2 )},//LCD.VD23
		{PB, 1, (SCU_MODE_FUNC2 )},//LCD.VD22
		{PB, 2, (SCU_MODE_FUNC2 )},//LCD.VD21
		{PB, 3, (SCU_MODE_FUNC2 )},//LCD.VD20
		{PB, 4, (SCU_MODE_FUNC2 )},//LCD.VD15
		{PB, 5, (SCU_MODE_FUNC2 )},//LCD.VD14
		{PB, 6, (SCU_MODE_FUNC2 )},//LCD.VD13
		{PC, 0, (SCU_MODE_FUNC0 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PC, 1, (SCU_MODE_FUNC4 )},//BL.RESET
		{PC, 2, (SCU_MODE_FUNC2 )},//U1.CTS
		{PC, 3, (SCU_MODE_FUNC2 )},//U1.RTS
		{PC, 4, (SCU_MODE_FUNC4 )},//YELLOW.LED
		{PC, 5, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PC, 6, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PC, 7, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PC, 8, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//3.3V_IO.PWR.EN
		{PC, 9, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS )},//PW.NGLED
		{PC, 10, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC4 | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN )},//FAN.ERR
		{PC, 11, (SCU_MODE_FUNC4 )},//SW1
		{PC, 12, (SCU_MODE_FUNC4 )},//SW2
		{PC, 13, (SCU_MODE_FUNC2 )},//U1.TXD
		{PC, 14, (SCU_MODE_FUNC2 )},//U1.RXD
		{PD, 1, (SCU_MODE_FUNC4 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN|SCU_MODE_4MA_DRIVESTR)},//PSA.SELECT
		{PD, 10, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//VALVE1
		{PD, 11, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//VALVE4
		{PD, 12, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//VALVE2
		{PD, 13, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//LatchValve2
		{PD, 14, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//LatchValve1
		{PD, 15, (SCU_MODE_FUNC4 |SCU_MODE_PULLUP|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_EN|SCU_MODE_ZIF_EN|SCU_MODE_4MA_DRIVESTR)},//CU.ERR
		{PD, 16, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 0, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 1, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 2, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 3, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 4, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 14, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PE, 15, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//SSP0.SSEL
		{PF, 4, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//I2S0.TX.MCLK
		{PF, 5, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PF, 6, (SCU_MODE_FUNC4 |SCU_MODE_INACT|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//AUDIO.RESET#
		{PF, 7, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PF, 8, (SCU_MODE_FUNC4 |SCU_MODE_PULLDOWN|SCU_MODE_HIGHSPEEDSLEW_DIS|SCU_MODE_INBUFF_DIS|SCU_MODE_ZIF_DIS|SCU_MODE_4MA_DRIVESTR)},//
		{PF, 9, (SCU_MODE_FUNC4 )},//CPB.Reset
		{PF, 10, (SCU_MODE_FUNC1 )},//U0.TXD
		{PF, 11, (SCU_MODE_FUNC1 )},//U0.RXD
		//	/* External data lines D0 .. D31 */
		{0x1,  7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1,  8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1,  9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1, 14, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x5,  4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x5,  3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xD,  9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0xE,  5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE,  6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE,  7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE,  8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE,  9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0xE, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},

		/* Address lines A0 .. A23 */
		{0x2,  9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x1,  0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x1,  1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x1,  2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x2,  8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2,  7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},
		{0x2,  6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x2,  2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x2,  1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x2,  0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},
		{0x6,  8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC1)},
		{0x6,  9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //DYCS0
		{0x1,  6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //WE
		{0x6,  4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //CAS
		{0x6,  5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //RAS
		{0x6, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //CKEOUT0
		{0x6, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //DQMOUT0
		{0x6, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //DQMOUT1
		{0xD,  0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)}, //DQMOUT2
		{0xE, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)}, //DQMOUT3
		{0xF, 0,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)},//SSP0_SCK
		{0xF, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | FUNC4)},
		{0xF, 2,  (SCU_PINIO_FAST | SCU_MODE_FUNC2)},//SSP0_MISO
		{0xF, 3,  (SCU_PINIO_FAST | SCU_MODE_FUNC2)},//SSP0_MOSI
};

/* Pin clock mux values, re-used structure, value in first index is meaningless */
STATIC const PINMUX_GRP_T pinclockmuxing[] = {
		{0, 0,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
		{0, 1,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
		{0, 2,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
		{0, 3,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
};

/* EMC clock delay */
#define CLK0_DELAY 5

/* EA SDRAM timing and chip config */
STATIC const IP_EMC_DYN_CONFIG_T IS42S32800D_config = {
		EMC_NANOSECOND(64000000 / 4096), /* Row refresh time */
		0x01,                            /* Command Delayed */
		EMC_NANOSECOND(20),
		EMC_NANOSECOND(45),
		EMC_NANOSECOND(70),
		EMC_CLOCK(0x05),
		EMC_CLOCK(0x05),
		EMC_NANOSECOND(14),
		EMC_NANOSECOND(68),
		EMC_NANOSECOND(68),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(14),
		EMC_CLOCK(0x02),
		{
				{
						EMC_ADDRESS_DYCS0, /* EA Board uses DYCS0 for SDRAM */
						3,                 /* RAS */

						EMC_DYN_MODE_WBMODE_PROGRAMMED |
						EMC_DYN_MODE_OPMODE_STANDARD |
						EMC_DYN_MODE_CAS_3 |
						EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
						EMC_DYN_MODE_BURST_LEN_4,

						EMC_DYN_CONFIG_DATA_BUS_32 |
						EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS
				},
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}
		}
};

/* EA SDRAM timing and chip config */
STATIC const IP_EMC_DYN_CONFIG_T IS42S32400F_config = {
		EMC_NANOSECOND(64000000 / 4096), /* Row refresh time */
		0x01,                            /* Command Delayed */
		EMC_NANOSECOND(21),
		EMC_NANOSECOND(42),
		EMC_NANOSECOND(70),
		EMC_CLOCK(0x05),
		EMC_CLOCK(0x05),
		EMC_NANOSECOND(14),
		EMC_NANOSECOND(14),
		EMC_NANOSECOND(68),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(14),
		EMC_CLOCK(0x02),
		{
				{
						EMC_ADDRESS_DYCS0, /* EA Board uses DYCS0 for SDRAM */
						3,                 /* RAS */

						EMC_DYN_MODE_WBMODE_PROGRAMMED |
						EMC_DYN_MODE_OPMODE_STANDARD |
						EMC_DYN_MODE_CAS_3 |
						EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
						EMC_DYN_MODE_BURST_LEN_4,

						EMC_DYN_CONFIG_DATA_BUS_32 |
						EMC_DYN_CONFIG_4Mx32_4BANKS_12ROWS_8COLS
				},
				{0, 0, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}
		}
};

/* NorFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T SST39VF320_config = {
		0,
		EMC_STATIC_CONFIG_MEM_WIDTH_16 |
		EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
		EMC_STATIC_CONFIG_BLS_HIGH,

		EMC_NANOSECOND(0),
		EMC_NANOSECOND(35),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(40),
		EMC_CLOCK(4)
};

/* NandFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T K9F1G_config = {
		1,
		EMC_STATIC_CONFIG_MEM_WIDTH_8 |
		EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
		EMC_STATIC_CONFIG_BLS_HIGH,

		EMC_NANOSECOND(0),
		EMC_NANOSECOND(35),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(70),
		EMC_NANOSECOND(40),
		EMC_CLOCK(4)
};

/*
 * Configuration for the memory mapped LEDs on the OEM Base Board
 */
STATIC const IP_EMC_STATIC_CONFIG_T memreg_config = {
		2,
		EMC_STATIC_CONFIG_MEM_WIDTH_16,

		EMC_NANOSECOND(0),
		EMC_NANOSECOND(0),
		EMC_NANOSECOND(0),
		EMC_NANOSECOND(0),
		EMC_NANOSECOND(0),
		EMC_CLOCK(4)
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	int i;
	/* Setup system level pin muxing */
	Chip_SCU_SetPinMuxing(pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));

	/* Clock pins only, group field not used */
	for (i = 0; i < (sizeof(pinclockmuxing) / sizeof(pinclockmuxing[0])); i++) {
		Chip_SCU_ClockPinMuxSet(pinclockmuxing[i].pinnum, pinclockmuxing[i].modefunc);
	}
	/* SPIFI pin setup is done prior to setting up system clocking */
	Chip_SCU_SetPinMuxing(spifipinmuxing, sizeof(spifipinmuxing) / sizeof(PINMUX_GRP_T));

	{
		int j;
		int num = sizeof(pinmuxing) / sizeof(PINMUX_GRP_T);
		// verify that no pins are defined twice
		for (i = 0; i < num; i++) {
			for (j = i+1; j < num; j++) {
				if ((pinmuxing[i].pingrp == pinmuxing[j].pingrp) &&
						(pinmuxing[i].pinnum == pinmuxing[j].pinnum)) {
					// Found conflicting setup
					while(1) {
						num = num; /* Should handle this better, but at least it is a breakpoint */
					}
				}
			}
		}
	}
}

/* Setup external memories */
//0001 08/30/2017 Quyen remove set up nand flah nor flash and LED on Embedder Artist board
void Board_SetupExtMemory(void)
{
	Chip_GPIO_Init(LPC_GPIO_PORT);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,SDRAM_POW_PORT, SDRAM_POW_BIT);		/* output */
	// Enable SDRam Power
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,SDRAM_POW_PORT, SDRAM_POW_BIT);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,SPI_POW_PORT, SPI_POW_BIT);		/* output */
	// Enable SPI Power
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,SPI_POW_PORT, SPI_POW_BIT);


	/* Setup EMC Delays */
	/* Move all clock delays together */
	LPC_SCU->EMCDELAYCLK = ((CLK0_DELAY) | (CLK0_DELAY << 4) | (CLK0_DELAY << 8) | (CLK0_DELAY << 12));

	if (MAX_CLOCK_FREQ >= 120000000) {
		/* Setup EMC Clock Divider for divide by 2 - this is done in both the CCU (clocking)
		   and CREG. For frequencies over 120MHz, a divider of 2 must be used. For frequencies
		   less than 120MHz, a divider of 1 or 2 is ok. */
		Chip_Clock_EnableOpts(CLK_MX_EMC_DIV, true, true, 2);
		LPC_CREG->CREG6 |= (1 << 16);
	} else {
		/* Setup EMC Clock Divider for divide by 1 (i.e. same as core) */
		Chip_Clock_EnableOpts(CLK_MX_EMC_DIV, true, true, 1);
		LPC_CREG->CREG6 &= ~(1 << 16);
	}

	/* Enable EMC clock */
	Chip_Clock_Enable(CLK_MX_EMC);

	/* Init EMC Controller -Enable-LE mode- clock ratio 1:1 */
	Chip_EMC_Init(1, 0, 0);
	/* Init EMC Dynamic Controller */
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &IS42S32400F_config);
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
	int i;
	/*************************************************************************************************************************/
	//Fix SPI Err Adapt V3.01
	Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IRC, true, false);	// change SPIFI to IRC during clock programming
	LPC_SPIFI->CTRL |= SPIFI_CTRL_FBCLK(1);								// and set FBCLK in SPIFI controller
	/*************************************************************************************************************************/
	/* Setup FLASH acceleration to target clock rate prior to clock switch */
	Chip_CREG_SetFlashAcceleration(MAX_CLOCK_FREQ);

	Chip_SetupCoreClock(CLKIN_CRYSTAL, MAX_CLOCK_FREQ, true);

	/* Setup system base clocks and initial states. This won't enable and
	   disable individual clocks, but sets up the base clock sources for
	   each individual peripheral clock. */
	for (i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++) {
		Chip_Clock_SetBaseClock(InitClkStates[i].clk, InitClkStates[i].clkin,
				InitClkStates[i].autoblock_enab, InitClkStates[i].powerdn);
	}

	/* Reset and enable 32Khz oscillator */
	LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));
	LPC_CREG->CREG0 |= (1 << 1) | (1 << 0);

	/* Setup a divider E for main PLL clock switch SPIFI clock to that divider.
	   Divide rate is based on CPU speed and speed of SPI FLASH part. */
#if (MAX_CLOCK_FREQ > 180000000)
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 5);
#else
	Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 4);
#endif
	Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* Setup system clocking and memory. This is done early to allow the
	   application and tools to clear memory and use scatter loading to
	   external memory. */
	Board_SetupMuxing();
#ifdef DEBUG
	Board_SetupClocking();//Release comment out
#endif
	Board_SetupExtMemory();
}
