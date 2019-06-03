/********************************************************************/
/*                                                                  */
/* File Name    : UIMgr.c                        				    */
/*                                                                  */
/* General      : contain function to update UI                     */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       April 18, 2017     Thao Ha(MV)  		new file        	*/
/*                                                                  */
/********************************************************************/

#include <AlarmMsgHandler.h>
#include <AudioMgr.h>
#include <AudioTask.h>
#include <DualCoreCommon.h>
#include "LedBrightnessHandler.h"
#include <EEPRom.h>
#include <FlowCtrl.h>
#include <DevInterface.h>
#include <PanelLed.h>
#include <PsaCtrl.h>
#include <UIMgr.h>
#include "Setting.h"
#include "ModeHandle.h"
#include "LogMgr.h"
#include "string.h"
#include "LogTask.h"
#include "Gpio.h"



#define VERY_LOW_BATTERY_LOW_PERCENTAGE 0
#define VERY_LOW_BATTERY_HIGH_PERCENTAGE 20
#define LOW_BATTERY_PERCENTAGE 50
#define MEDIUM_BATTERY_PERCENTAGE 75
#define HIGH_BATTERY_PERCENTAGE 90
#define FULL_BATTERY_PERCENTAGE 100
#define WAITING_TIME_TO_WRITE_LOG 1000/DEVICE_TASK_DELAY_TIME
#define LOOP_CNT_200MS 200/DEVICE_TASK_DELAY_TIME
static int gs_currentMode = -1;
static int gs_currentFlowRate = -1;
static uint16_t gs_currentBatteryPercent=0;
static bool gs_isACExist= false;
static bool gs_isBatteryCharging = false;
static bool gs_powerOffFlag = false;
static bool gs_isAlarmPowerLostReset = false;
static bool gs_isPowerSwitchReset = false;
static bool gs_isDisableDisplayWhenShutdown = false;
static E_ChargeSource gs_currentChargingState=eBatterySource;



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_GetCurrentMode						              				*/
/*                                                                                      */
/* Details:        Get displaying mode on UI											*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : int			                                                        */
/*                                                                                      */
/****************************************************************************************/
int UIMgr_GetCurrentMode(void)
{
	return gs_currentMode;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_GetCurrentFlowrate						              			*/
/*                                                                                      */
/* Details:        Get displaying flowrate on UI										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : int			                                                        */
/*                                                                                      */
/****************************************************************************************/
int UIMgr_GetCurrentFlowrate(void)
{
	return gs_currentFlowRate;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_isACExist							              				*/
/*                                                                                      */
/* Details:        check whether AC exist												*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool UIMgr_isACExist(void)
{
	return gs_isACExist;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_SetPowerOffFlag						              				*/
/*                                                                                      */
/* Details:       Set Power off flag													*/
/*                                                                                      */
/* Arguments:	(I) bool state															*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_SetPowerOffFlag(bool state)
{
	gs_powerOffFlag = state;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_DisableDisplayBatteryPercentWhenShutdown						    */
/*                                                                                      */
/* Details:       Disable display battery Percent when shutdown	Portable				*/
/*                                                                                      */
/* Arguments:	(I) void																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_DisableDisplayBatteryPercentWhenShutdown()
{
	gs_isDisableDisplayWhenShutdown = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateMode							              				*/
/*                                                                                      */
/* Details:       Update UI base on current mode										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_UpdateMode()
{
	if(setting_Get(eModeSettingID)==eContinuosMode)
	{
		panelLed_LedDisplay( (E_ID_CONDITION)(setting_Get(eContinuousFLowRateSettingID) + eValueZeroOneZero) );
		panelLed_LedDisplay(eContinuousModePattern1);
		gs_currentMode=eContinuosMode;
		gs_currentFlowRate=setting_Get(eContinuousFLowRateSettingID);
	}
	else
	{
		panelLed_LedDisplay( (E_ID_CONDITION)(setting_Get(eSynchronizedFlowRateSettingID) + eValueZeroOneZero) );
		panelLed_LedDisplay(eSynchronizedModeClose);
		gs_currentMode=eSynchronizedMode;
		gs_currentFlowRate=setting_Get(eSynchronizedFlowRateSettingID);
	}
	if((gs_currentMode == eContinuosMode) && (gs_currentFlowRate>=eFlowRateZeroOneZero)
			&& (gs_currentFlowRate<=eFlowRateOneZeroZero))
	{
		psa_select(gs_currentFlowRate,eContinuosMode);
	}
	if((gs_currentMode == eSynchronizedMode) && (gs_currentFlowRate>=eFlowRateZeroTwoFive)
			&& (gs_currentFlowRate<=eFlowRateThreeZeroZero))
	{
		psa_select(gs_currentFlowRate,eSynchronizedMode);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateBattery						              				*/
/*                                                                                      */
/* Details:       Update UI base on current battery status								*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_UpdateBattery()
{
	if(gs_isBatteryCharging == false)
	{
		if(gs_currentBatteryPercent == VERY_LOW_BATTERY_LOW_PERCENTAGE)
		{
			panelLed_LedDisplay(eWithoutChargeLower);

		}
		else
			if((gs_currentBatteryPercent <= VERY_LOW_BATTERY_HIGH_PERCENTAGE)&&(gs_currentBatteryPercent>VERY_LOW_BATTERY_LOW_PERCENTAGE))
			{
				panelLed_LedDisplay(eWithoutChargeLower);
			}
			else
				if((gs_currentBatteryPercent > VERY_LOW_BATTERY_HIGH_PERCENTAGE) && (gs_currentBatteryPercent <= LOW_BATTERY_PERCENTAGE))
				{
					panelLed_LedDisplay(eWithoutChargeLow);
				}
				else
					if((gs_currentBatteryPercent > LOW_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= MEDIUM_BATTERY_PERCENTAGE))
					{
						panelLed_LedDisplay(eWithoutChargeMed);
					}
					else
						if((gs_currentBatteryPercent > MEDIUM_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= FULL_BATTERY_PERCENTAGE))
						{
							panelLed_LedDisplay(eWithoutChargeHigh);
						}

	}
	else
	{
		if(gs_currentBatteryPercent == VERY_LOW_BATTERY_LOW_PERCENTAGE)
		{
			panelLed_LedDisplay(eWithoutChargeLower);
		}
		else
			if((gs_currentBatteryPercent <= VERY_LOW_BATTERY_HIGH_PERCENTAGE)&&(gs_currentBatteryPercent > VERY_LOW_BATTERY_LOW_PERCENTAGE))
			{
				panelLed_LedDisplay(eDuringChargeLower);
			}
			else
				if((gs_currentBatteryPercent > VERY_LOW_BATTERY_HIGH_PERCENTAGE) && (gs_currentBatteryPercent <= LOW_BATTERY_PERCENTAGE))
				{
					panelLed_LedDisplay(eDuringChargeLow);
				}
				else
					if((gs_currentBatteryPercent > LOW_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= HIGH_BATTERY_PERCENTAGE))
					{
						panelLed_LedDisplay(eDuringChargeMed);
					}
					else
						if((gs_currentBatteryPercent > HIGH_BATTERY_PERCENTAGE) && (gs_currentBatteryPercent <= FULL_BATTERY_PERCENTAGE))
						{
							panelLed_LedDisplay(eDuringChargeHigh);
						}

	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateACDetect						              				*/
/*                                                                                      */
/* Details:       Detect and handle when AC source change								*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_UpdateACDetect()
{
	AUDIO_T eventAudio;
	eventAudio.volume=setting_Get(eAudioVolumeSettingID);
	if(devInterface_GetChargeSource()!=eBatterySource)
	{
		DEBUGOUT("AC connect\n");
		panelLed_LedDisplay(eACConnect);
		eventAudio.ID = ePowerACAudio;
		gs_isACExist = true;
	}
	else
	{
		DEBUGOUT("No connect\n");
		panelLed_LedDisplay(eACDisconnect);
		eventAudio.ID = ePowerBatteryAudio;
		gs_isACExist = false;
	}
	if(!audioTask_SendToAudioQueue(eventAudio, 10)){
		DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_StartSequence								              		*/
/*                                                                                      */
/* Details:        Start sequence														*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_StartSequence()
{
	AUDIO_T eventAudio;
	eventAudio.volume =setting_Get(eAudioVolumeSettingID);
	eventAudio.ID = eBuzzer100msAudio;

	if(!audioTask_SendToAudioQueue(eventAudio, 10)){
		DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
	}
	vTaskDelay(100);
	eventAudio.ID = eStartUpAudio;
    if(!audioTask_SendToAudioQueue(eventAudio, 10)){
        DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
    }
	if(devInterface_GetChargeSource()!=eBatterySource)
	{
		panelLed_LedDisplay(eACConnect);
		gs_isACExist = true;
	}
	else
	{
		panelLed_LedDisplay(eACDisconnect);
		gs_isACExist = false;
	}
	UIMgr_UpdateMode();
	mode_StartSequence();
	gs_currentChargingState = devInterface_GetChargeSource();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateLEDPanel								              		*/
/*                                                                                      */
/* Details:        Update LED panel base on current setting and battery status			*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_UpdateLEDPanel(uint16_t remainingPercentage,bool isBatteryCharging)
{
	bool isACExist = false;
	if(devInterface_GetChargeSource()!=eBatterySource)
	{
		isACExist = true;
	}
	else
	{
		isACExist = false;
	}
	if(gs_isACExist != isACExist)
	{
		UIMgr_UpdateACDetect();
	}
	if(gs_currentMode != setting_Get(eModeSettingID))
	{
		UIMgr_UpdateMode();
	}
	if(gs_currentMode == eContinuosMode)
	{
		if(gs_currentFlowRate != setting_Get(eContinuousFLowRateSettingID))
		{
			UIMgr_UpdateMode();
		}
	}
	else
	{
		if(gs_currentFlowRate != setting_Get(eSynchronizedFlowRateSettingID))
		{
			UIMgr_UpdateMode();
		}
	}
	uint16_t tempBattPercent = remainingPercentage;
	bool tempIsBattCharging = isBatteryCharging;
	if(gs_isDisableDisplayWhenShutdown == false)
	{
		if(gs_currentBatteryPercent!=tempBattPercent)
		{
			gs_currentBatteryPercent = tempBattPercent;
			gs_isBatteryCharging = tempIsBattCharging;
			UIMgr_UpdateBattery();

		}
		if(gs_isBatteryCharging!=tempIsBattCharging)
		{
			gs_currentBatteryPercent = tempBattPercent;
			gs_isBatteryCharging = tempIsBattCharging;
			UIMgr_UpdateBattery();
		}
	}
	else
	{
		panelLed_LedDisplay(eWithoutChargeLower);
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateCurrentPowerSupply								           	*/
/*                                                                                      */
/* Details:   Update current Power supply display										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_UpdateCurrentPowerSupply()
{
	static bool s_isChargingStateChange = true;
	static uint8_t s_cnt = WAITING_TIME_TO_WRITE_LOG;

	if(gs_currentChargingState != devInterface_GetChargeSource())
	{
		gs_currentChargingState = devInterface_GetChargeSource();
		s_isChargingStateChange = true;
	}
	if(s_isChargingStateChange == true)
	{
		s_cnt++;
		if(s_cnt>= WAITING_TIME_TO_WRITE_LOG)
		{
			POR_LOG_T log ={'\0'};
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			switch(gs_currentChargingState)
			{
			case eBatterySource:
				log.logCode=eBatteryOperationLogId;
				break;
			case eCarSource:
				log.logCode=eDCOperationLogId;
				break;
			case eACSource:
				log.logCode=eACOperationLogId;
				break;
			default:
				break;
			}
			setting_GetCurrentSetting(&log.newValue);
			E_logQueueReceive logQueue;
			logQueue.id = eRequestWriteLog;
			memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from UI_UpdateCurrentPowerSupply");
			}
			else
			{
				s_isChargingStateChange =false;
				s_cnt=0;
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateCurrentPowerSupply								           	*/
/*                                                                                      */
/* Details:   Update current Power supply display										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_AlarmPowerLostReset()
{
	static uint16_t s_cnt = 0;
	s_cnt++;
	if(s_cnt >= LOOP_CNT_200MS)
	{
		WDT_SystemReset();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateCurrentPowerSupply								           	*/
/*                                                                                      */
/* Details:   Update current Power supply display										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_PowerSwitchReset()
{
	static uint16_t s_cnt = 0;
	s_cnt++;
	if(s_cnt >= LOOP_CNT_200MS)
	{
		WDT_SystemReset();
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateCurrentPowerSupply								           	*/
/*                                                                                      */
/* Details:   Update current Power supply display										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_PowerLostHanlde(void)
{
	alarmMsgHandler_DisableAllAlarm();
	UIMgr_SetPowerOffFlag(true);
	E_logQueueReceive logQueue;
	POR_LOG_T log ={'\0'};
	log.logCode=ePowerLost;
	log.EventOrAlarm = ePowerLost;
	setting_GetCurrentSetting(&log.oldValue);
	setting_GetCurrentSetting(&log.newValue);
	logQueue.id = eRequestWriteLog;
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from UI_Handle");
	}
	DEBUG_TASK("Power Lost Alarm \n");
	EEPROM_SetMode(ePowerLostMode);
	gs_isAlarmPowerLostReset = true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_UpdateCurrentPowerSupply								           	*/
/*                                                                                      */
/* Details:   Update current Power supply display										*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void UIMgr_PowerSwitchHandle(void)
{
	alarmMsgHandler_DisableAllAlarm();
	UIMgr_SetPowerOffFlag(true);
	E_logQueueReceive logQueue;
	POR_LOG_T log ={'\0'};
	log.logCode=eResetPowerSwitching;
	log.EventOrAlarm = eAlarmPowerLost;
	setting_GetCurrentSetting(&log.oldValue);
	setting_GetCurrentSetting(&log.newValue);
	logQueue.id = eRequestWriteLog;
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from UI_Handle");
	}
	DEBUG_TASK("Power Switch Reset \n");
	EEPROM_SetMode(ePowerLostMode);
	gs_isPowerSwitchReset = true;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	UI_Handle								              				*/
/*                                                                                      */
/* Details:        UI handle function is call for checking and updating UI every 20ms	*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void UIMgr_Handle(uint16_t remainingPercentage,bool isBatteryCharging)
{
	LedBrightness_Handle();
	if(gs_powerOffFlag == false)
	{
		static bool s_isStartUI = true;
		if(s_isStartUI==true)
		{
			UIMgr_StartSequence();
			s_isStartUI = false;
		}
		UIMgr_UpdateLEDPanel(remainingPercentage,isBatteryCharging);
		if(s_isStartUI == false)
			UIMgr_UpdateCurrentPowerSupply();
		if((devInterface_GetChargeSource()==eBatterySource)&&(!Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BAT_DETECT_PORT_NUM,BAT_DETECT_BIT_NUM) == false))
		{
			UIMgr_PowerLostHanlde();
		}
		if((devInterface_GetChargeSource()!=eBatterySource)&&(Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,BAT_DETECT_PORT_NUM,BAT_DETECT_BIT_NUM)==true))
		{
			audioMgr_StopAudio();//stop audio for decrease Cd-Ni battery power consume
			if (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,AC_DETECT_PORT_NUM,AC_DETECT_BIT_NUM)==true)
				UIMgr_PowerSwitchHandle();
			else
				UIMgr_PowerLostHanlde();
		}
	}
	if(gs_isAlarmPowerLostReset == true)
	{
		UIMgr_AlarmPowerLostReset();
	}
	if(gs_isPowerSwitchReset == true)
	{
		UIMgr_PowerSwitchReset();
	}
}
