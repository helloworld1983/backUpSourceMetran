/********************************************************************/
/*                                                                  */
/* File Name    : common.h                                    		*/
/*                                                                  */
/* General      : Initialization Hardware							*/
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/*                                                                  */
/********************************************************************/

#include "common.h"
#include "UsbMgr.h"
#include "gpio.h"
#include "ff.h"
#include "SPIFMgr.h"
#include "Led.h"
#include "EEPROM.h"
#include "adc.h"
#include "AudioMgr.h"
#include "IRQHandle.h"
#include "LogMgr.h"
#include "Fan.h"
#include "RealTimeClock.h"



FATFS fatFS;
bool g_isSPIFError = false;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	SetupHardware							              				*/
/*                                                                                      */
/* Details:    Init all devices which are used in bootloader							*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void SetupHardware(void)
{
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
	DEBUGINIT();
	Chip_GPIO_Init(LPC_GPIO_PORT);
	led_InitLed();
	gpio_Init();
	gpio_Set();
	fan_Init();
	Board_I2C_Init(I2C1);
	Board_I2C_Init(I2C0);
	g_isSPIFError = spifMgr_unlock();
	EEPROM_Init();
	adc_Init();
	IRQ_timer3Init();
	logMgr_InitLogMemory();
	audioMgr_InitAudio();
}


