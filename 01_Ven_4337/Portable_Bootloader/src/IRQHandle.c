/********************************************************************/
/*                                                                  */
/* File Name    : IRQHandler.c                                		*/
/*                                                                  */
/* General      : This block contains function handle for different */
/*					IRQ which are used in bootloader				*/
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
#include "IRQHandle.h"
#include "updateFlash.h"
#include "Led.h"
#include "AudioMgr.h"
#include "Key.h"
#include "gpio.h"
#include "boot.h"
#include "EEPROM.h"
enum
{
	ePanelLEDState_U,
	ePanelLEDState_UP,
	ePanelLEDState_UPD,
	ePanelLEDState_C,
	ePanelLEDState_CH,
	ePanelLEDState_CHE,
	ePanelLEDState_H,
	ePanelLEDState_L
};
#define TIMER0_PERIOD 1
#define TIMER1_PERIOD 1
#define TIMER2_PERIOD 0.2
#define TIMER3_PERIOD 0.1
#define LOOP_2_SECOND_CNT 2/TIMER3_PERIOD
#define LOOP_TIME_TO_OFFSYSTEM_WHEN_ALARM_POWER_CNT 5*60/TIMER3_PERIOD
static uint16_t gs_ttick = 0;
static uint16_t gs_ttickAlarmLED = 0;
static bool gs_isAlarmForPowerLostPlaying = false;
bool g_isBuzzerSoundPlaying = false;
extern bool g_isSPIFError;
/****************************************************************************************/
/*                                                                                      */
/* General 	-Interrupt Handler to handle bootloader display while updating				*/
/*                                                                                      */
/* Details  -Interrupt Handler to handle bootloader display while updating				*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void TIMER0_IRQHandler(void)
{
	static uint8_t currentLEDStateUpdate = ePanelLEDState_U;
	static uint8_t currentLEDStateCheck = ePanelLEDState_C;
	if (Chip_TIMER_MatchPending(LPC_TIMER0, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
		if(update_getCurrentState()==eUpdating)
		{
			switch(currentLEDStateUpdate)
			{
			case ePanelLEDState_U:
				led_DisplayWord(eU);
				currentLEDStateUpdate = ePanelLEDState_UP;
				break;
			case ePanelLEDState_UP:
				led_DisplayWord(eUP);
				currentLEDStateUpdate = ePanelLEDState_UPD;
				break;
			case ePanelLEDState_UPD:
				led_DisplayWord(eUPd);
				currentLEDStateUpdate = ePanelLEDState_U;
				break;
			default:
				break;
			}
		}
		else
			if(update_getCurrentState()==eChecking)
			{
				switch(currentLEDStateCheck)
				{
				case ePanelLEDState_C:
					led_DisplayWord(eC);
					currentLEDStateCheck = ePanelLEDState_CH;
					break;
				case ePanelLEDState_CH:
					led_DisplayWord(eCH);
					currentLEDStateCheck = ePanelLEDState_CHE;
					break;
				case ePanelLEDState_CHE:
					led_DisplayWord(eCHE);
					currentLEDStateCheck = ePanelLEDState_C;
					break;
				default:
					break;
				}
			}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Interrupt Handler to handle bootloader display and audio					*/
/*  			function after update has finished										*/
/*                                                                                      */
/* Details 	-Interrupt Handler to handle bootloader display and audio					*/
/*  			function after update has finished										*/
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void TIMER1_IRQHandler(void)
{
	static uint8_t s_cnt = 0;
	static uint8_t currentLEDState = ePanelLEDState_H;
	if (Chip_TIMER_MatchPending(LPC_TIMER1, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER1, 1);
		if(g_isSPIFError == false)
		{
			if(s_cnt == 0)
			{
				audioMgr_PlayAudio(eAlertRestartAudio);
			}
			switch(currentLEDState)
			{
			case ePanelLEDState_H:
				led_DisplayHValue(update_getUpdatedVersionH());
				currentLEDState = ePanelLEDState_L;
				break;
			case ePanelLEDState_L:
				led_DisplayLValue(update_getUpdatedVersionL());
				currentLEDState = ePanelLEDState_H;
				break;
			default:
				break;
			}
		}
		s_cnt++;
		if(s_cnt == 5)
		{
			s_cnt=0;
			if(g_isSPIFError == true)
			{
				DEBUGOUT("Reset by TIMER 1 IRQ \n");
				NVIC_SystemReset();
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Interrupt Handler to counter whether Power button is pushed for 2 second	*/
/*                                                                                      */
/* Details  -Interrupt Handler to counter whether Power button is pushed for 2 second	*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void TIMER2_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER2, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER2, 1);
		E_ID_Audio event = eNoneAudio;
		gs_ttick++;
		gs_ttickAlarmLED++;
		if(audioMgr_GetStatusPowerLostAlarmAudio())
		{
			if(gs_ttick == 2)
			{
				event = eBuzzer200msAudio;
				audioMgr_PlayAudio(event);
				g_isBuzzerSoundPlaying= true;
			}
			else if(gs_ttick == 4)
			{
				event = eBuzzer200msAudio;
				audioMgr_PlayAudio(event);
				g_isBuzzerSoundPlaying= true;
			}
			else if(gs_ttick == 6)
			{
				event = eBuzzer200msAudio;
				audioMgr_PlayAudio(event);
				g_isBuzzerSoundPlaying= true;
			}
			else if (gs_ttick == 18)
			{
				gs_ttick = 0;
				g_isBuzzerSoundPlaying= false;
			}
			else
			{
				g_isBuzzerSoundPlaying= true;
			}
		}

		if(led_getStatusDisplayContactAlarm())
		{
			if(gs_ttickAlarmLED == 1)
			{
				led_TurnOnContactLed();
			}
			else if(gs_ttickAlarmLED == 2)
			{
				gs_ttickAlarmLED = 0;
				led_TurnOffContactLed();
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Interrupt Handler to counter whether Power button is pushed for 2 second	*/
/*                                                                                      */
/* Details  -Interrupt Handler to counter whether Power button is pushed for 2 second	*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void TIMER3_IRQHandler(void)
{
	static uint32_t s_cntAlarmPowerLost = 0;
	static uint8_t s_cnt = 0;
	if (Chip_TIMER_MatchPending(LPC_TIMER3, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER3, 1);
		if(gs_isAlarmForPowerLostPlaying == true)
		{
			s_cntAlarmPowerLost++;
			if(s_cntAlarmPowerLost>=LOOP_TIME_TO_OFFSYSTEM_WHEN_ALARM_POWER_CNT)
			{
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,POW_ON_PORT_NUM, POW_ON_BIT_NUM,false);
				Chip_GPIO_WritePortBit(LPC_GPIO_PORT,BAT_ON_PORT_NUM, BAT_ON_BIT_NUM,false);
				__disable_irq();
//				if(EEPROM_ReadMode()==eVeryLowBatteryMode)
//				{
					EEPROM_SetMode(eOperationMode);
//				}
				while(1)
				{
					if(boot_isACConnected()==true||boot_isBatteryConnected()==true)
					{
						DEBUGOUT("Reset by TIMER 3 IRQ 1\n");
						NVIC_SystemReset();
					}
				}
			}
		}
		else
		{
			s_cntAlarmPowerLost = 0;
		}
		if(Key_isPowerButtonPressed()==true)
		{
			s_cnt++;
		}
		else
		{
			s_cnt=0;
		}
		if(s_cnt>=LOOP_2_SECOND_CNT)
		{
			if(update_getCurrentState() == eNoneUpdate)
			{
				DEBUGOUT("Set key on\n");
				Key_setPowerOn();
			}
			else
			{
				DEBUGOUT("Turn off\n");
				led_DisplayWord(eOFF);
				IRQ_timer0Disable();
				IRQ_timer1Disable();
				while(Key_isPowerButtonPressed()==true)
				{
					led_DisplayWord(eOffAllLed);
				}
				DEBUGOUT("Reset by TIMER 3 IRQ 2\n");
				NVIC_SystemReset();
			}
			s_cnt=0;
			IRQ_timer3Disable();
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize Timer 0							                   				*/
/*                                                                                      */
/* Details  -Initialize Timer 0															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer0Init(void)
{
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_RGU_TriggerReset(RGU_TIMER0_RST);
	while (Chip_RGU_InReset(RGU_TIMER0_RST)) {}
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
	Chip_TIMER_SetMatch(LPC_TIMER0, 1, Chip_Clock_GetRate(CLK_MX_MXCORE)*TIMER0_PERIOD);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
	Chip_TIMER_Enable(LPC_TIMER0);
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize Timer 1							                   				*/
/*                                                                                      */
/* Details  -Initialize Timer 1															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer1Init(void)
{
	Chip_TIMER_Init(LPC_TIMER1);
	Chip_RGU_TriggerReset(RGU_TIMER1_RST);
	while (Chip_RGU_InReset(RGU_TIMER1_RST)) {}
	Chip_TIMER_Reset(LPC_TIMER1);
	Chip_TIMER_MatchEnableInt(LPC_TIMER1, 1);
	Chip_TIMER_SetMatch(LPC_TIMER1, 1, Chip_Clock_GetRate(CLK_MX_MXCORE)*TIMER1_PERIOD);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER1, 1);
	Chip_TIMER_Enable(LPC_TIMER1);
	NVIC_EnableIRQ(TIMER1_IRQn);
	NVIC_ClearPendingIRQ(TIMER1_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize Timer 3							                   				*/
/*                                                                                      */
/* Details  -Initialize Timer 3															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer3Init(void)
{
	Chip_TIMER_Init(LPC_TIMER3);
	Chip_RGU_TriggerReset(RGU_TIMER3_RST);
	while (Chip_RGU_InReset(RGU_TIMER3_RST)) {}
	Chip_TIMER_Reset(LPC_TIMER3);
	Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
	Chip_TIMER_SetMatch(LPC_TIMER3, 1, Chip_Clock_GetRate(CLK_MX_MXCORE)*TIMER3_PERIOD);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
	Chip_TIMER_Enable(LPC_TIMER3);
	NVIC_EnableIRQ(TIMER3_IRQn);
	NVIC_ClearPendingIRQ(TIMER3_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize Timer 2							                   				*/
/*                                                                                      */
/* Details  -Initialize Timer 2															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer2Init(void)
{
	Chip_TIMER_Init(LPC_TIMER2);
	Chip_RGU_TriggerReset(RGU_TIMER2_RST);
	while (Chip_RGU_InReset(RGU_TIMER2_RST)) {}
	Chip_TIMER_Reset(LPC_TIMER2);
	Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
	Chip_TIMER_SetMatch(LPC_TIMER2, 1, Chip_Clock_GetRate(CLK_MX_MXCORE)*TIMER2_PERIOD);
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
	Chip_TIMER_Enable(LPC_TIMER2);
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Enable Timer 2								                   				*/
/*                                                                                      */
/* Details  -Enable Timer 2																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer2Enalble(void)
{
	gs_isAlarmForPowerLostPlaying=true;
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_ClearPendingIRQ(TIMER2_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Disable Timer 3							                   				*/
/*                                                                                      */
/* Details  -Disable Timer 3															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer3Disable(void)
{
	NVIC_DisableIRQ(TIMER3_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Disable Timer 2							                   				*/
/*                                                                                      */
/* Details  -Disable Timer 2															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer2Disable(void)
{
	NVIC_DisableIRQ(TIMER2_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Disable Timer 1							                   				*/
/*                                                                                      */
/* Details  -Disable Timer 1															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer1Disable(void)
{
	NVIC_DisableIRQ(TIMER1_IRQn);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Disable Timer 0							                   				*/
/*                                                                                      */
/* Details  -Disable Timer 0															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void IRQ_timer0Disable(void)
{
	NVIC_DisableIRQ(TIMER0_IRQn);
}



