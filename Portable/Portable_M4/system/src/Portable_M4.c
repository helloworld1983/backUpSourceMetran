/*
===============================================================================
 Name        : Portable_M4.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 //2017/10/05	Quyen Ngo	remove Board_LED_Sets
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

#if defined (__MULTICORE_MASTER_SLAVE_M0APP) | defined (__MULTICORE_MASTER_SLAVE_M0SUB)
#include <cr_start_m0.h>
#endif
#include <DualCoreCommon.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "spifilib_api.h"
#include <RTC.h>
#include <OpTime.h>
#include "Setting.h"
#include <EEPRom.h>
#include "SPIFMgr.h"
#include "LogMgr.h"
#include "SPIFMgr.h"
#include <DevCtrl.h>
#include "LogTask.h"
#include <DevTask.h>
#include "UsbVcom.h"
#include "AlarmTask.h"
#include "USBTask.h"
#include <AudioTask.h>
#include <Gpio.h>
#include "HAL_LPC18xx.h"
#include "app_usbd_cfg.h"
#include <UpdateFlash.h>
#include "Battery.h"
#include "Led.h"
extern SPIFI_HANDLE_T *pSpifi;
extern USBD_HANDLE_T g_hUsb;

static bool gs_isUpdateBootloader;


/****************************************************************************************/
/*                                                                                      */
/* General 	USB0_IRQHandler			                                            		*/
/*                                                                                      */
/* Details  This USB0 interrupt when insert USB memory or Connect Portable to PC	   */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/* Using Global Data : (I) NONE							                              	*/
/*                     			                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*                                                                                      */
/****************************************************************************************/
void USB0_IRQHandler(void)
{
	if (gs_isUpdateBootloader)
	{
		HcdIrqHandler(0);
	}
	else
	{
		USBD_API->hw->ISR(g_hUsb);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	prvSetupHardware						              				*/
/*                                                                                      */
/* Details:  Init all necessary hardware for each Task									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_I2C_Init(I2C0);
	Board_I2C_Init(I2C1);
	gpio_Init();
	gpio_Set();
	realtimeclock_Init();
	WDT_init();
	spifMgr_unlock();
	audioMgr_InitAudio();
	battery_Init();
	devTask_Init();
	logTask_InitLogTask();
	audioTask_Init();
	EEPROM_Init();
	OperationTime_Init();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	updateBootloader								              		*/
/*                                                                                      */
/* Details:  Update bootloader															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void updateBootloader(void)
{
	led_DisplayWord(eOnFlowLed);
	led_DisplayWord(eUSB);
	NVIC_SetPriority(I2S0_IRQn,2);
	bool error = false;
	error = update_startUpdate();
	if (error == false)
	{
		setting_Set(eSetUpdateModeID, 0);//clear update bootloader flag
	}
	while (1);//finish update, wait for restart
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	main_tasks								              				*/
/*                                                                                      */
/* Details:  Create and run different tasks for controlling Portable system				*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void main_tasks(void)
{
	devTask_CreateTask();
	audioTask_CreateTask();
	USBTask_CreateTask();
	logTask_CreateTask();
	alarmTask_CreateTask();
	vTaskStartScheduler();
	/* Control should never come here */
	DEBUGSTR("Schedule Failure\r\n");
	while (1) {}
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
#endif
#endif
//	// Start M0APP slave processor
//#if defined (__MULTICORE_MASTER_SLAVE_M0APP)
//	cr_start_m0(SLAVE_M0APP,&__core_m0app_START__);
//#endif

//	// Start M0SUB slave processor
//#if defined (__MULTICORE_MASTER_SLAVE_M0SUB)
//	cr_start_m0(SLAVE_M0SUB,&__core_m0sub_START__);
//#endif

	// TODO: insert code here
	prvSetupHardware();
	if(setting_Get(eSetUpdateModeID))
	{
		gs_isUpdateBootloader = true;
		updateBootloader();
	}
	DEBUGOUT("system start......\n");
	USBTask_Init();
	Chip_RTC_Enable(LPC_RTC,ENABLE);
	main_tasks();
	return 0 ;
}
