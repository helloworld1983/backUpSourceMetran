/********************************************************************/
/*                                                                  */
/* File Name    : boot.c		                                 	*/
/*                                                                  */
/* General      : This module contains function to run bootloader  	*/
/*				and get current control software from Flash bank	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include "boot.h"
#include "updateFlash.h"
#include "checksum.h"
#include "EEPROM.h"
#include "Led.h"
#include "Key.h"
#include "gpio.h"
#include "string.h"
#include "common.h"
#include "AudioMgr.h"
#include "board.h"
#include "board_api.h"
#include "IRQHandle.h"
#include "Battery.h"
#include "LogMgr.h"
#define M4_Image_Magic  0xAA55DEAD
#define CONTROL_SW_ADDRESS_OFFSET 8
#define SPIF_COMMUNICATION_ERROR 93
#define START_BYTE_VERSION_H_TENTH 3
#define START_BYTE_VERSION_H 4
#define START_BYTE_VERSION_L_TENTH 7
#define START_BYTE_VERSION_L 8
#define DELAY_TIME_1000_MS  1000
#define DELAY_TIME_2000_MS  2000
extern bool g_isBuzzerSoundPlaying;
extern bool g_isSPIFError;


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_delayMS							              				*/
/*                                                                                      */
/* Details:    delay milliseconds														*/
/*                                                                                      */
/* Arguments:	uint16_t																*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void boot_delayMS(uint16_t Milliseconds)
{
	while (Milliseconds--)
	{
		volatile  uint32_t  i;

		for (i = 0; i < (20 * 1000); i++) {    /* This logic was tested. It gives app. 1 micro sec delay        */
			;
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_isACConnected						              				*/
/*                                                                                      */
/* Details:    Check whether AC charging is connected									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool boot_isACConnected()
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_isBatteryConnected					              				*/
/*                                                                                      */
/* Details:    Check whether Battery is connected										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool boot_isBatteryConnected()
{
	return !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BAT_DETECT_PORT_NUM,BAT_DETECT_BIT_NUM);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_isBatteryLow						              				*/
/*                                                                                      */
/* Details:    Check whether Battery status is low										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool boot_isBatteryLow()
{
	return !Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,VBAT_SEN_PORT_NUM,VBAT_SEN_BIT_NUM);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_resetWatchDogHandle				              				*/
/*                                                                                      */
/* Details:    Handle the case when system reset by watchdog timer						*/
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void boot_resetWatchDogHandle()
{
	led_DisplayWord(eOnFlowLed);
	Chip_RTC_Init(LPC_RTC);
	bool error = false;
	POR_LOG_T log ={'\0'};
	log.logCode=eResetWatchdogLogId;
	log.EventOrAlarm = eEvent;
	logMgr_GetCurrentSetting(&log.oldValue);
	logMgr_GetCurrentSetting(&log.newValue);
	logMgr_RecordLog(log);
	EEPROM_SetMode(eOperationMode);
	if((EEPROM_ReadMode() == eUpdateMode)||(checksum_Check()!=false))
	{
		error = update_startUpdate();
		if(error == false)
		{
			EEPROM_ClearUpdateFlag();
		}
		while(1)
		{
			if(gpio_InputVoltageCheck()==eNoCharge && boot_isBatteryConnected()==false)
			{
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
				__disable_irq();
				while(1)
				{
					if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
					{
						NVIC_SystemReset();
					}
				}
			}
		}
	}
	else
	{
		update_runCtrlSW();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_noPowerHandle						              				*/
/*                                                                                      */
/* Details:    Handle the case when there is no charger and battery connected to 		*/
/*             Portable system	                                                        */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void boot_noPowerHandle()
{
	DEBUGOUT("No AC No Battery\n");

	audioMgr_PlayPowerLostAlarmAudio();

	while(!Key_isPowerOn())
	{
		if((boot_isBatteryLow()==true) && (g_isBuzzerSoundPlaying==false))
		{
			DEBUGOUT("shutdown By low voltage\n");
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
			__disable_irq();
			while(1)
			{
				if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
				{
					NVIC_SystemReset();
				}
			}
		}
		if(gpio_InputVoltageCheck()!=eNoCharge)
		{
			DEBUGOUT("Reset by input voltage not no charge\n");
			EEPROM_SetMode(ePowerLostMode);
			boot_delayMS(DELAY_TIME_1000_MS);
			NVIC_SystemReset();
		}
		if(boot_isBatteryConnected()==true)
		{
			DEBUGOUT("Reset by battery connected\n");
			EEPROM_SetMode(ePowerLostMode);
			NVIC_SystemReset();
		}
	}
	audioMgr_StopPowerLostAlarmAudio();
	while(Key_isPowerButtonPressed());
	EEPROM_SetMode(eOperationMode);//set operation mode when power lost alarm is cleared
	Chip_RTC_Init(LPC_RTC);
	POR_LOG_T log ={'\0'};
	log.logCode=ePowerLostClear;
	log.EventOrAlarm = eEvent;
	logMgr_GetCurrentSetting(&log.oldValue);
	logMgr_GetCurrentSetting(&log.newValue);
	logMgr_RecordLog(log);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
	while(1)
	{
		__disable_irq();
		if(boot_isACConnected()==true)
		{
			NVIC_SystemReset();
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_chargerConnectHandle				              				*/
/*                                                                                      */
/* Details:    Handle the case when only battery is connected to Portable system		*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void boot_batteryConnectHandle()
{
	bool error = false;
	DEBUGOUT("No AC Battery exist\n");
	bool isPowerButtonPress2Sec = false;
	if(EEPROM_ReadMode()!=ePowerLostMode && EEPROM_ReadMode()!=eVeryLowBatteryMode)
	{
		while(Key_isPowerButtonPressed())
		{
			if(Key_isPowerOn())
			{
				isPowerButtonPress2Sec=true;
				break;
			}
		}
	}
	else
		if(EEPROM_ReadMode()==eVeryLowBatteryMode)
		{
			DEBUGOUT("Enter VeryLowBatteryMode\n");
			audioMgr_PlayPowerLostAlarmAudio();
			while(Key_isPowerOn()==false)
			{
				if(boot_isACConnected()==true)
				{
					NVIC_SystemReset();
				}
				if((boot_isBatteryLow()==true) && (g_isBuzzerSoundPlaying==false))
				{
					DEBUGOUT("shutdown By low voltage \n");
					EEPROM_SetMode(ePowerLostMode);
					Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
					Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
					__disable_irq();
					while(1)
					{
						if(boot_isACConnected()||Key_isPowerButtonPressed()==true)
						{
							NVIC_SystemReset();
						}
					}
				}
			}
			EEPROM_SetMode(eOperationMode);
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
			while(1)
			{
				__disable_irq();
				if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
				{
					NVIC_SystemReset();
				}
			}
		}
		else
		{
			EEPROM_SetMode(eOperationMode);
			isPowerButtonPress2Sec = true;
		}
	if(isPowerButtonPress2Sec==true)
	{
		led_DisplayWord(eOnFlowLed);
		if((EEPROM_ReadMode() == eUpdateMode)||(checksum_Check()!=false))
		{
			error = update_startUpdate();
			if(error == false)
			{
				EEPROM_ClearUpdateFlag();
			}
			while(1);
		}
		else
		{
			update_runCtrlSW();
		}
	}
	else
	{
		DEBUGOUT("Shutdown\n");
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
		while(1){
			__disable_irq();
			if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
			{
				NVIC_SystemReset();
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_chargerConnectHandle				              				*/
/*                                                                                      */
/* Details:    Handle the case when charger is connected to Portable system				*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void boot_chargerConnectHandle()
{
	bool error = false;
	DEBUGOUT("AC exist\n");
	if(EEPROM_ReadMode()!=ePowerLostMode && EEPROM_ReadMode()!=eVeryLowBatteryMode)
	{
		E_ChargingState currentStateCharge = gpio_InputVoltageCheck();
		battery_UpdateChargeInputCurrentLimit();
		while(!Key_isPowerOn())
		{
			if(gpio_InputVoltageCheck()==eNoCharge)
			{
				DEBUGOUT("Turn off\n");
				boot_delayMS(DELAY_TIME_2000_MS);
				DEBUGOUT("Turn off OK\n");
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
				__disable_irq();
				while(1)
				{
					if(Key_isPowerButtonPressed()==true)
					{
						NVIC_SystemReset();
					}
				}
			}
			if(currentStateCharge!=gpio_InputVoltageCheck())
			{
				currentStateCharge=gpio_InputVoltageCheck();
				battery_UpdateChargeInputCurrentLimit();
			}
		}
	}
	else
	{
		while(boot_isBatteryConnected()==false);
		boot_delayMS(DELAY_TIME_1000_MS);
		if(gpio_InputVoltageCheck()==eNoCharge)
		{
			DEBUGOUT("Reset\n");
			NVIC_SystemReset();
		}
		EEPROM_SetMode(eOperationMode);
	}
	led_DisplayWord(eOnFlowLed);
	if((EEPROM_ReadMode() == eUpdateMode)||(checksum_Check()!=false))
	{
		error = update_startUpdate();
		if(error == false)
		{
			EEPROM_ClearUpdateFlag();
		}
		while(1)
		{
			if(gpio_InputVoltageCheck()==eNoCharge && boot_isBatteryConnected()==false)
			{
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
				__disable_irq();
				while(1)
				{
					if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
					{
						NVIC_SystemReset();
					}
				}
			}
		}
	}
	else
	{
		update_runCtrlSW();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_startBootloader					              				*/
/*                                                                                      */
/* Details:    Start running the bootloader												*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void boot_startBootloader()
{
	DEBUGOUT("Test mode %d \n",EEPROM_ReadMode());
	if(g_isSPIFError==false)
	{
		if(EEPROM_ReadMode()==eResetWatchdogTimer)
		{
			boot_resetWatchDogHandle();
		}
		if(gpio_InputVoltageCheck()!=eNoCharge)
		{
			boot_chargerConnectHandle();
		}
		else
		{
			if(boot_isBatteryConnected())
			{
				boot_batteryConnectHandle();
			}
			else
			{
				boot_noPowerHandle();
			}
		}
	}
	else
	{
		DEBUGOUT("SPIF unlock Error \n");
		led_DisplayErrorCode(SPIF_COMMUNICATION_ERROR);
		led_StartDisplayContactAlarm();
		IRQ_timer1Init();
		while(1)
		{
			if(gpio_InputVoltageCheck()==eNoCharge && boot_isBatteryConnected()==false)
			{
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
				__disable_irq();
				while(1)
				{
					if(boot_isACConnected()==true||Key_isPowerButtonPressed()==true)
					{
						NVIC_SystemReset();
					}
				}
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	boot_getControlSWVersion				              				*/
/*                                                                                      */
/* Details:    get Control software version from flash bank								*/
/*                                                                                      */
/* Arguments:	(I) image_addr															*/
/*              (O) uint8_t* versionH													*/
/*              (O) uint8_t* versionL                                                   */
/* ReturnValue : int			                                                        */
/*                                                                                      */
/****************************************************************************************/

int boot_getControlSWVersion(uint32_t image_addr,uint8_t* versionH,uint8_t* versionL)
{
	const uint32_t *addr = (uint32_t *) image_addr + CONTROL_SW_ADDRESS_OFFSET;
	const struct image_sig *img;
	if (*addr != M4_Image_Magic) {
		return -1;
	}
	addr++;
	img = (const struct image_sig *) *addr;
	if (img->signature != SIGNATURE_MAGIC) {
		return -1;
	}
	*versionH=(img->version[START_BYTE_VERSION_H_TENTH]-'0')*10+(img->version[START_BYTE_VERSION_H]-'0');
	*versionL=(img->version[START_BYTE_VERSION_L_TENTH]-'0')*10+(img->version[START_BYTE_VERSION_L]-'0');
	return 0;
}

