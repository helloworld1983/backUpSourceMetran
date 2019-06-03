/********************************************************************/
/*                                                                  */
/* File Name    : AlarmControl.c                                   	*/
/*                                                                  */
/* General      : AlarmControl module receive queue from Alarm Task	*/
/*                to handle and send to UI					     	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/09     Truong Nguyen(MV)  	 new file       */
/*                                                                  */
/********************************************************************/

#include <AlarmMsgHandler.h>
#include <AudioMgr.h>
#include <AudioTask.h>
#include <EEPRom.h>
#include <FlowCtrl.h>
#include <PanelKey.h>
#include <PanelLed.h>
#include <TaskCommon.h>
#include <UIMgr.h>
#include "timers.h"
#include "LogMgr.h"
#include "USBControl.h"
#include "string.h"
#include "BreathTrigger.h"
#include "Compressor.h"
#include "USBControl.h"
#include "LogTask.h"
#include "USBTask.h"
#include "DevInterface.h"


//#define DEBUG_ALARM_MGR_EN
#ifdef DEBUG_ALARM_CTRL_EN
#define DEBUG_ALARM_CTRL(...) printf(__VA_ARGS__)
#else
#define DEBUG_ALARM_CTRL(...)
#endif

/* constants, macro definition */
#define LIMIT_COUNT_250MS	1
#define LIMIT_COUNT_500MS	2
#define LIMIT_COUNT_750MS	3
#define LIMIT_COUNT_1250MS	5
#define LIMIT_COUNT_1500MS	6
#define LIMIT_COUNT_2250MS	9
#define LIMIT_COUNT_2500MS	10
#define LIMIT_COUNT_2750MS	11
#define LIMIT_COUNT_3250MS	13
#define LIMIT_COUNT_3500MS	14
#define LIMIT_COUNT_4250MS	17
#define LIMIT_COUNT_10S		40

#define LIMIT_COUNT_400MS	1
#define LIMIT_COUNT_800MS	2
#define LIMIT_COUNT_1200MS	3
#define LIMIT_COUNT_2400MS	6
#define LIMIT_COUNT_4S		10
#define LIMIT_COUNT_25S		62

#define LIMIT_END_PLAY_HIGH_ALM_SOUND	17
#define LIMIT_END_PLAY_MED_ALM_SOUND 	6

#define ALARM_LED_5S_TICKS_TIME			5000
#define ALARM_LED_2000MS_TICKS_TIME		2000
#define ALARM_AUDIO_400MS_TICKS_TIME	400
#define ALARM_AUDIO_250MS_TICKS_TIME	250

#define ALARM_LED_TIMER_5S_ID			1
#define ALARM_LED_TIMER_2000MS_ID		2
#define ALARM_AUDIO_TIMER_400MS_ID		3
#define ALARM_AUDIO_TIMER_250MS_ID		4
#define NUMBER_ALARM_ACTIVE	   10

#ifndef paMS_TO_TICKS
#define paMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif


static TimerHandle_t gs_Start5STimer;
static TimerHandle_t gs_Start2000MSTimer;
static TimerHandle_t gs_Start400MSTimer;
static TimerHandle_t gs_Start250MSTimer;


static ALARM_STAT_T gs_alarmList[NumberOfAlarm];
static uint8_t gs_listAlarmActive[NUMBER_ALARM_ACTIVE];	// Buffer of Active Alarms
static E_PriorityOfAlarm gs_listPriorityAlarm[NUMBER_ALARM_ACTIVE];
static E_PriorityOfAlarm gs_highestPriority = eNoneAlarm;
static E_PriorityOfAlarm gs_highestPriorityEndSound = eNoneAlarm;
static E_VoiceGuidace gs_voiceGuidance;

static uint8_t timeTickHighAlarm = 0;
static uint8_t timeTickMedAlarm = 0;
static uint8_t timeTickLowAlarm = 0;
static bool is_displayErrorCodeAgain = true;
static bool gs_isMuteButtonTOGGLE2S = false;
static bool gs_isPowerButtonTOGGLE2S = false;
static bool gs_isNoAlarm = true;
static bool	gs_isVolumeButtonOncePress = false;
static bool gs_isPowerLostOccur = false;
static bool gs_checkPrecedenceVoice = false ;

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_PlayHighPriorityAlarmAudio								*/
/*                                                                                      */
/* Details：	 	Play high priority Alarm audio 										    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/* Using Global Data :	is_highPriorityAlarm 					                       	*/
/*                     	timeTickHighAlarm							                 	*/
/*                     								                                 	*/
/* local variables   : 	uint8_t event;			      							        */
/*【Note】                                                                            										    		*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_PlayHighPriorityAlarmAudio()
{
	AUDIO_T eventAudio;
	eventAudio.volume = setting_Get(eAudioVolumeSettingID);
	if((timeTickHighAlarm == 1)
			||(timeTickHighAlarm == LIMIT_COUNT_250MS)
			||(timeTickHighAlarm == LIMIT_COUNT_500MS)
			||(timeTickHighAlarm == LIMIT_COUNT_750MS)
			||(timeTickHighAlarm == LIMIT_COUNT_1250MS)
			||(timeTickHighAlarm == LIMIT_COUNT_1500MS)
			||(timeTickHighAlarm == LIMIT_COUNT_2250MS)
			||(timeTickHighAlarm == LIMIT_COUNT_2500MS)
			||(timeTickHighAlarm == LIMIT_COUNT_2750MS)
			||(timeTickHighAlarm == LIMIT_COUNT_3250MS)
			||(timeTickHighAlarm == LIMIT_COUNT_3500MS)
	)
	{
		eventAudio.ID = eBuzzer200msAudio;
		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
	}
	else if (timeTickHighAlarm == LIMIT_COUNT_4250MS )
	{
		if (gs_checkPrecedenceVoice)
		{
			eventAudio.ID = eAlertContactAudio;
			if(!audioTask_SendToAudioQueue(eventAudio, 20)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayLowPriorityAlarmAudio");
			}
		}
		else
		{
			eventAudio.ID = eAlertBattertyAudio;

			if(!audioTask_SendToAudioQueue(eventAudio, 20)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
		}
		if(gs_highestPriority == eNoneAlarm)
		{
			gs_highestPriorityEndSound = eNoneAlarm;
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_PlayMediumPriorityAlarmAudio								*/
/*                                                                                      */
/* Details：	 	Play medium priority alarm audio 									    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/* Using Global Data :	is_mediumPriorityAlarm 					                       	*/
/*                     	timeTickMedAlarm							                    */
/*                     								                                 	*/
/* local variables   : 	uint8_t event		 								            */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_PlayMediumPriorityAlarmAudio()
{
	AUDIO_T eventAudio;
	eventAudio.volume = setting_Get(eAudioVolumeSettingID);
	if ((timeTickMedAlarm == LIMIT_COUNT_400MS)
			||(timeTickMedAlarm == LIMIT_COUNT_800MS)
			||(timeTickMedAlarm == LIMIT_COUNT_1200MS))
	{
		eventAudio.ID =  eBuzzer200msAudio;

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
	}
	else if(timeTickMedAlarm == LIMIT_COUNT_2400MS)
	{
		if (gs_voiceGuidance == eAlertCannula)
		{
			eventAudio.ID = eAlertCannulaAudio;

			if(audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			if(gs_highestPriority == eNoneAlarm)
			{
				gs_highestPriorityEndSound = eNoneAlarm;
			}
		}
		else if (gs_voiceGuidance == eAlertCharge)
		{
			eventAudio.ID = eAlertChargetAudio;

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			if(gs_highestPriority == eNoneAlarm)
			{
				gs_highestPriorityEndSound = eNoneAlarm;
			}
		}
		else
		{
			eventAudio.ID = eAlertInspectAudio;

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			if(gs_highestPriority == eNoneAlarm)
			{
				gs_highestPriorityEndSound = eNoneAlarm;
			}
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_PlayLowPriorityAlarmAudio									*/
/*                                                                                      */
/* Details：	 	Play low priority Alarm Audio 										    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/* Using Global Data :	is_lowPriorityAlarm 					                       	*/
/*                     	timeTickLowAlarm							                    */
/*                     								                                 	*/
/* local variables   : 	uint8_t event			      						            */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_PlayLowPriorityAlarmAudio()
{
	AUDIO_T eventAudio;
	eventAudio.volume = setting_Get(eAudioVolumeSettingID);
	if (timeTickLowAlarm == LIMIT_COUNT_400MS)
	{
		eventAudio.ID = eBuzzer200msAudio;

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
	}
	else if(timeTickLowAlarm == LIMIT_COUNT_800MS)
	{
		eventAudio.ID = eBuzzer200msAudio;

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayLowPriorityAlarmAudio");
		}
	}
	else if(timeTickLowAlarm == LIMIT_COUNT_1200MS)
	{
		eventAudio.ID = eBuzzer200msAudio;

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayLowPriorityAlarmAudio");
		}
		if(gs_highestPriority == eNoneAlarm)
		{
			gs_highestPriorityEndSound = eNoneAlarm;
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_CheckPermitPlayAlarmAudio									*/
/*                                                                                      */
/* Details：	 	Play Alarm Audio 							 		   					*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/* Using Global Data :	gs_highestPriority 					                        	*/
/*                    	timeTickHighAlarm			                                	*/
/*                    	timeTickMedAlarm			                                	*/
/*                    	timeTickLowAlarm			                                	*/
/*                     								                                 	*/
/* local variables   : 	bool muteStatus				      					            */
/*                      bool shutdownStatus                                             */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_PlayAlarmAudio()
{
	if (gs_isMuteButtonTOGGLE2S == true)
	{
		gs_highestPriorityEndSound = eNoneAlarm;
		timeTickHighAlarm = 0;
		timeTickMedAlarm = 0;
		timeTickLowAlarm = 0;
	}
	if ((gs_isPowerButtonTOGGLE2S == false)&&(is_displayErrorCodeAgain == true))
	{
		switch (gs_highestPriorityEndSound)
		{
		case eLowPriority:
			alarmMsgHandler_PlayLowPriorityAlarmAudio();
			break;
		case eMediumPriority:
			alarmMsgHandler_PlayMediumPriorityAlarmAudio();
			break;
		case eHighPriority:
			alarmMsgHandler_PlayHighPriorityAlarmAudio();
			break;
		default:
			break;
		}
	}
}


/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_DisplayFlowRateSettingAgain								*/
/*                                                                                      */
/* Details：	 	display Flow Rate Setting Again (when Alarms auto clear)				*/
/*                                                            							*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/* Using Global Data :	gs_highestPriority 					                        	*/
/*                     								                                 	*/
/* local variables   : 	unsigned char event				      					        */
/*                      bool shutdownStatus                                             */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_DisplayFlowRateSettingAgain()
{
	panelLed_LedDisplay( (E_ID_CONDITION)(UIMgr_GetCurrentFlowrate() + eValueZeroOneZero) );
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	devCtrl_SendErrorCodeToUI											*/
/*                                                                                      */
/* Details：	 	send Alarm Error Code to display in UI								    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :	gs_listAlarmActive				                             	*/
/*                     								                                 	*/
/* local variables   : 	static uint8_t countNumAlarm 			          		        */
/*                     	static bool isDisplayFlowRateSetting							*/
/*                     	unsigned char event							   					*/
/*                     	shutdownStatus													*/
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_SendErrorCodeToUI()
{
	static uint8_t countNumAlarm = 0;
	static bool isDisplayFlowRateSetting = false;
	unsigned char event = 0;
	if(gs_isPowerButtonTOGGLE2S)
	{
		panelLed_LedDisplay(eBreathErrorNoTrigger);
		panelLed_LedDisplay(eContactErrorNoTrigger);
		panelLed_LedDisplay(eLowerBatteryNoTrigger);
		panelLed_LedDisplay(eServiceRequireNoTrigger);
	}
	else
	{
		if (countNumAlarm == 0)
		{
			if(gs_listAlarmActive[0] != 0)
			{
				panelLed_DisplayErrorCode( (E_ID_ERRORCODE)gs_listAlarmActive[0] );
				event = gs_listAlarmActive[0];
				E_USBQUEUE usbQueue;
				usbQueue.ID = eErrorCode;
				usbQueue.data = event;

				if(!USBTask_SendToUSBQueue(usbQueue, 10)){
					DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
				}
				gs_isNoAlarm = false;
				isDisplayFlowRateSetting = true;
			}
			else if (isDisplayFlowRateSetting == true)// Display Flow rate setting again after all Alarm are cleared
			{
				gs_isNoAlarm = true;
				alarmMsgHandler_DisplayFlowRateSettingAgain();
				isDisplayFlowRateSetting = false;
			}
		}
		else
		{
			panelLed_DisplayErrorCode((E_ID_ERRORCODE)gs_listAlarmActive[countNumAlarm]);
			event = gs_listAlarmActive[countNumAlarm];
			E_USBQUEUE usbQueue;
			usbQueue.ID = eErrorCode;
			usbQueue.data = event;
			if(!USBTask_SendToUSBQueue(usbQueue, 10)){
				DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
			}
		}
		countNumAlarm++;
		if (gs_listAlarmActive[countNumAlarm] == 0)	countNumAlarm = 0;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_SendAlarmLog												*/
/*                                                                                      */
/* Details：	 	send Alarm Log Code 												    */
/*                                                                                      */
/* Arguments : ALARM_STAT_T alarm_Receive		 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :									                             	*/
/*                     								                                 	*/
/* local variables   : 	POR_LOG_T log 							          		        */
/*                     																	*/
/* 【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_SendAlarmLog(ALARM_STAT_T alarm_Receive)
{
	POR_LOG_T log ={'\0'};
	switch (alarm_Receive.ID)
	{
	case eAlarmBreathErr:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eBreathErrorLogId;
			log.EventOrAlarm = eAlarmBreath;
		}
		break;
	case eAlarmAbnormalOxygenConcentration:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eAbnormalO2ConcentrationErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmLowOxygenConcentration:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eLowO2ConcentrationLogId;
			log.EventOrAlarm = eAlarmInspect;
		}
		break;
	case eAlarmOxygenTankPressure:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eOxygenTankPressureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmBatteryTemperature:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eBatteryTemperatureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmBatteryCharge:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eBatteryChargeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmBatteryRemainingCapacityLower:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eBatteryRemaininingCapacityLowerLogId;
			log.EventOrAlarm = eAlarmCharge;
		}
		break;
	case eAlarmPowerLostErr:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = ePowerLost;
			log.EventOrAlarm = eAlarmPowerLost;
		}
		else
		{
			log.logCode = ePowerLostClear;
			log.EventOrAlarm = eAlarmClear;
		}
		break;
	case eAlarmCompressorSupplyPressure:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorSupplyPressureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorSuctionPressure:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorVacuumPressureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowcontrol:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmSupplyPressureSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eSupplyPressureSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmVacuumPressureSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eVacuumPressureSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmTankPressureSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eTankPressureSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmDifferentialPressureSensorCommunication:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eDifferentialPressureSensorCommunicationErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmO2Sensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerO2SensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerCommunicationNoResponse:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationNoResponsemErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerPressureSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerPressureSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerFlowSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerFlowSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerTemperatureSensor:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerTemperatureSensorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerCommunication:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationCOMErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerCommunicationCheckSum:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationCommandCheckSumErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerCommunicationCommand:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationCommandCodeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerCommunicationParameter:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationCommandParameterErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerResponseCheckSumError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationResponseCheckSumErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerResponseCodeError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationResponseCodeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmFlowControllerResponseDataLengthError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eFlowControllerCommunicationResponseDataLengthErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorCommunicationNoResponse:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationNoResponseErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorDCOverVoltage:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverDCOverVoltageErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorDCLowerVoltage:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverDCLowerVoltageErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorSoftwareOverCurrent:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverSoftwareOverCurrentErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorHardwareOverCurrent:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverHardwareOverCurrentErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorBoardTemperature:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverBoardTemperatureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorMotorTemperature:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverMotorTemperatureErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorRotationSpeed:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverRotationSpeedErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorStepOut:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverStepOutErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorCommunication:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationCommunicationErrorErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorCommunicationCheckSum:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationCommandCheckSumErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorCommunicationCommand:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationCommandCodeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorCommunicationParameter:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationCommandParameterErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorResponseCheckSumError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationResponseCheckSumErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorResponseCodeError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationResponseCodeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmCompressorResponseDataLenghtError:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eCompressorDriverCommunicationResponseDataLengthErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmSpeaker:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eSpeakerErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmBatteryType:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eBatteryTypeErrorLogId;
			log.EventOrAlarm = eAlarmContact;
		}
		break;
	case eAlarmTachypnea:
		if(alarm_Receive.currentStatus == eActive)
		{
			log.logCode = eTachypneaError;
			log.EventOrAlarm = eAlarmBreath;
		}
		break;
	default:
		break;
	}
	setting_GetCurrentSetting(&log.oldValue);
	setting_GetCurrentSetting(&log.newValue);
	E_logQueueReceive logQueue;
	logQueue.id = eRequestWriteLog;
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from alarmCtrl_SendAlarmLog");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmCtrl_SendBlinkSignalToUI										*/
/*                                                                                      */
/* Details：	 send Blinking signal to  UI											    */
/*                                                                                      */
/* Arguments : ALARM_STAT_T alarm_Send			 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :	None							                             	*/
/*                     								                                 	*/
/* local variables   : 	unsigned char event			               					    */
/*                     								   									*/
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_PlayBlinkingSignal()
{
//	for (int i=0; i<NUMBER_ALARM_ACTIVE; i++)
//	{
//		if (gs_listAlarmActive[i] == eAlarmLowOxygenConcentration)
//		{
//			panelLed_LedDisplay(eServiceRequireTrigger);
//			break;
//		}
//		else
//		{
//			panelLed_LedDisplay(eServiceRequireNoTrigger);
//		}
//	}
	if(gs_alarmList[eAlarmLowOxygenConcentration].currentStatus == eActive )
	{
		panelLed_LedDisplay(eServiceRequireTrigger);
		E_USBQUEUE usbQueue;
		usbQueue.ID = eErrorCode;
		usbQueue.data = eAlarmLowOxygenConcentration;

		if(!USBTask_SendToUSBQueue(usbQueue, 10)){
			DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_SendErrorCodeToUI");
		}
	}
	else
	{
		panelLed_LedDisplay(eServiceRequireNoTrigger);
	}
	for (int i=0; i<NUMBER_ALARM_ACTIVE; i++)
	{
		if ((gs_listAlarmActive[i] == eAlarmBreathErr)||(gs_listAlarmActive[i] == eAlarmTachypnea))
		{
			panelLed_LedDisplay(eBreathErrorTrigger);
			break;
		}
		else
		{
			panelLed_LedDisplay(eBreathErrorNoTrigger);
		}
	}

//	for (int i=0; i<NUMBER_ALARM_ACTIVE; i++)
//	{
//		if (gs_listAlarmActive[i]== eAlarmBatteryRemainingCapacityLower)
//		{
//			panelLed_LedDisplay(eLowerBatteryTrigger);
//			break;
//		}
//		else
//		{
//			panelLed_LedDisplay(eLowerBatteryNoTrigger);
//		}
//	}

	if(gs_alarmList[eAlarmBatteryRemainingCapacityLower].currentStatus == eActive )
	{
		panelLed_LedDisplay(eLowerBatteryTrigger);
	}
	else
	{
		panelLed_LedDisplay(eLowerBatteryNoTrigger);
	}

	for (int i=0; i<NUMBER_ALARM_ACTIVE; i++)
	{
		if ((gs_listAlarmActive[i]!= 0)
				&&(gs_listAlarmActive[i]!= eAlarmBreathErr)
				&&(gs_listAlarmActive[i]!= eAlarmTachypnea)
				&&(gs_listAlarmActive[i]!= eAlarmBatteryRemainingCapacityLower)
				&&(gs_listAlarmActive[i]!= eAlarmLowOxygenConcentration))
		{
			panelLed_LedDisplay(eContactErrorTrigger);
			break;
		}
		else
		{
			panelLed_LedDisplay(eContactErrorNoTrigger);
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_timerCallback										*/
/*                                                                                      */
/* Details：	 	This Function is called when Timers overflow						    */
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :	gs_alarmList 					                             	*/
/*                    	g_checkMuteButton			                                	*/
/*                     								                                 	*/
/* local variables   : 								      					            */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_timerCallback( TimerHandle_t xTimer )
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId)
	{
	case ALARM_LED_TIMER_5S_ID:
		is_displayErrorCodeAgain = true;
		//Key_ResetVolumeButtonStatusOncePress();
		gs_isVolumeButtonOncePress = false;
		timeTickHighAlarm = 0;
		timeTickMedAlarm = 0;
		timeTickLowAlarm = 0;
		break;
	case ALARM_LED_TIMER_2000MS_ID:
		if (xTimerStart(gs_Start2000MSTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start Alarm LED start 1000MS timer \n");
		}
		if(is_displayErrorCodeAgain == true)
			alarmMsgHandler_SendErrorCodeToUI();
		break;
	case ALARM_AUDIO_TIMER_250MS_ID:
		if (xTimerStart(gs_Start250MSTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start Alarm Audio start 400MS timer \n");
		}
		if(gs_highestPriorityEndSound == eHighPriority)
		{
			alarmMsgHandler_PlayAlarmAudio();
			timeTickHighAlarm++;
			if (timeTickHighAlarm == 40)
				timeTickHighAlarm = 0;
		}
		break;
	case ALARM_AUDIO_TIMER_400MS_ID:
		if (xTimerStart(gs_Start400MSTimer,10) != pdPASS)
		{
			DEBUG_TASK("ERR: Failed to start Alarm Audio start 400MS timer \n");
		}
		if(gs_highestPriorityEndSound == eMediumPriority)
		{
			alarmMsgHandler_PlayAlarmAudio();
			timeTickMedAlarm++;
			if (timeTickMedAlarm == 62)
				timeTickMedAlarm = 0;
		}
		if(gs_highestPriorityEndSound == eLowPriority)
		{
			alarmMsgHandler_PlayAlarmAudio();
			timeTickLowAlarm++;
			if (timeTickLowAlarm == 10)
				timeTickLowAlarm = 0;
		}
		break;
	default:
		break;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMsgHandler_CreateTimer		 									*/
/*                                                                                      */
/* Details:         Create FreeRTOS Timer1000MS for displaying Alarm Error Code			*/
/*                  Timer 250MS and 400MS for calling Alarm Sound				        */
/*                                                                                      */
/* Arguments : None      																*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_CreateTimer()
{
	gs_Start5STimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Alarm Led Start 5S timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					paMS_TO_TICKS(ALARM_LED_5S_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) ALARM_LED_TIMER_5S_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					alarmMsgHandler_timerCallback
			);
	gs_Start2000MSTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Alarm Led Start 2000MS timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					paMS_TO_TICKS(ALARM_LED_2000MS_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) ALARM_LED_TIMER_2000MS_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					alarmMsgHandler_timerCallback
			);
	gs_Start400MSTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Alarm Audio Start 400MS timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					paMS_TO_TICKS(ALARM_AUDIO_400MS_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) ALARM_AUDIO_TIMER_400MS_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					alarmMsgHandler_timerCallback
			);
	gs_Start250MSTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Alarm Audio Start 250MS timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					paMS_TO_TICKS(ALARM_AUDIO_250MS_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) ALARM_AUDIO_TIMER_250MS_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					alarmMsgHandler_timerCallback
			);
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMsgHandler_StartTimer 											*/
/*                                                                                      */
/* Details:         Start Timer 1000MS, Timer 250MS  and Timer 400MS					*/
/*                                                                                      */
/* Arguments : (I)  None      															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_StartTimer()
{
	if (xTimerStart(gs_Start2000MSTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start  Alarm LED start 2000MS timer \n");
	}
	if (xTimerStart(gs_Start400MSTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR:  Failed to start Alarm Audio start 400MS \n");
	}
	if (xTimerStart(gs_Start250MSTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR:  Failed to start Alarm Audio start 400MS \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmMsgHandler_Init 												*/
/*                                                                                      */
/* Details:         Initial Alarm Control												*/
/*                                                                                      */
/* Arguments : (I)  None      															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_Init()
{
	alarmMsgHandler_CreateTimer();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_CreateAListOfActiveAlarm						 	*/
/*                                                                                      */
/* Details：  	Create a list of active alarm from List of all alarms        		    */
/*              to Display on LED 7Segs                                                 */
/*                                                                                      */
/* Arguments : 		 					  												*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :	gs_alarmList 									               	*/
/*                    	gs_listAlarmActive					                           	*/
/*                     	gs_listPriorityAlarm						                   	*/
/*                                                                                      */
/* local variables   : 	ALARM_STAT_T alarmListTempo			      						*/
/*                      uint8_t levelPriority                                           */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_CreateAListOfActiveAlarm()
{
	ALARM_STAT_T alarmListTempo [NumberOfAlarm];	// Create a temporary List of all alarms
	for (int alarmIndex = eFirsAlarmId; alarmIndex < eALarmVeryLowBattery; alarmIndex++)
	{
		alarmListTempo[alarmIndex].currentStatus = gs_alarmList[alarmIndex].currentStatus;
	}
	for (uint8_t i = 0; i < NUMBER_ALARM_ACTIVE; i++ )// Create a list of active alarm from List of all alarms to Display on LED 7Segs
	{
		for (uint8_t j = eFirsAlarmId; j < eALarmVeryLowBattery ; j++)
		{
			if (alarmListTempo[j].currentStatus == eActive)
			{

				if (j == eAlarmPowerLostErr)
				{
					gs_listPriorityAlarm[i] = eLowPriority;// low priority
				}
				else if ((j == eAlarmBreathErr)||(j == eAlarmTachypnea))
				{
					gs_listPriorityAlarm[i] = eMediumPriority;// medium priority
					gs_voiceGuidance = eAlertCannula;
				}
				else if (j == eAlarmBatteryRemainingCapacityLower)
				{
					gs_listPriorityAlarm[i] = eMediumPriority;// medium priority
					gs_voiceGuidance = eAlertCharge;
				}
				else if (j == eAlarmLowOxygenConcentration)
				{
					alarmListTempo[j].currentStatus = eInactive;
					break;
					//gs_listPriorityAlarm[i] = eNoneAlarm;// no alarm sound
				}
				else
				{
					gs_listPriorityAlarm[i] = eHighPriority;// high priority
					if (j != eAlarmBatteryType)
						gs_checkPrecedenceVoice = true;	// Alert Battery voice
				}
				gs_listAlarmActive[i] = j;
				alarmListTempo[j].currentStatus = eInactive;
				break;
			}
			else
			{
				gs_listAlarmActive[i] = 0;
				gs_listPriorityAlarm[i] = eNoneAlarm;
			}
		}
	}
	// Choose highest Priority Alarm to play Alarm Audio
	gs_highestPriority = eNoneAlarm;
	for (uint8_t i = 0; i < NUMBER_ALARM_ACTIVE; i++)
	{
		if (gs_highestPriority < gs_listPriorityAlarm[i])
			gs_highestPriority = gs_listPriorityAlarm[i];
	}
	if(gs_highestPriority != eNoneAlarm)
	{
		gs_highestPriorityEndSound = gs_highestPriority;
	}
	else if ((timeTickHighAlarm > LIMIT_END_PLAY_HIGH_ALM_SOUND)||(timeTickMedAlarm > LIMIT_END_PLAY_MED_ALM_SOUND))
	{
		gs_highestPriorityEndSound = eNoneAlarm;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	alarmCtrl_DisableAllAlarm 											*/
/*                                                                                      */
/* Details:         Initial Alarm Control												*/
/*                                                                                      */
/* Arguments : (I)  None      															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_DisableAllAlarm()
{
	gs_isPowerButtonTOGGLE2S = true;
}



/****************************************************************************************/
/*                                                                                      */
/* Function name：   alarmMsgHandler_BatteryVeryLowHandle                                  */
/*                                                                                      */
/* Details：     Handle battery capacity < 8%                                               */
/*                                                                                      */
/* Arguments :  None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Using Global Data :  gs_isPowerLostOccur                                             */
/*                                                                                      */
/* local variables   :                                                                  */
/*                                                                                      */
/*【Note】                                                                                                                                                                */
/*                                                                                      */
/****************************************************************************************/
static void alarmMsgHandler_BatteryVeryLowHandle(void)
{
	gs_isPowerLostOccur = true;
    E_logQueueReceive logQueue;
    POR_LOG_T log ={'\0'};
    log.logCode=ePowerLost;
    log.EventOrAlarm = eAlarmPowerLost;
    setting_GetCurrentSetting(&log.oldValue);
    setting_GetCurrentSetting(&log.newValue);
    logQueue.id = eRequestWriteLog;
    memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
    if(!logTask_SendToLogQueueReceive(logQueue, 10))
    {
    	DEBUG_TASK("Failed to send Item to logQueueRecive from alarmMsgHandler_BatteryInvalidHandle");
    }
    else
    {
    	keyPanel_PowerButtonToggle2S();
    }
    DEBUG_TASK("Power Lost Alarm \n");

}

/****************************************************************************************/
/*                                                                                      */
/* Function name：   alarmMsgHandler_BatteryInvalidHandle                                 */
/*                                                                                      */
/* Details：     Handle when battery invalid                                               */
/*                                                                                      */
/* Arguments :  None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Using Global Data :  gs_isPowerLostOccur                                             */
/*                                                                                      */
/* local variables   :                                                                  */
/*                                                                                      */
/*【Note】                                                                                                                                                                */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_BatteryInvalidHandle(void)
{
	static bool s_isSendSuccess = false;
	if((gs_alarmList[eAlarmBatteryType].currentStatus == eActive)&&(devInterface_GetChargeSource()==eBatterySource)&&(s_isSendSuccess==false))
	{
		gs_isPowerLostOccur = true;
		E_logQueueReceive logQueue;
		POR_LOG_T log ={'\0'};
		log.logCode=ePowerLost;
		log.EventOrAlarm = eAlarmPowerLost;
		setting_GetCurrentSetting(&log.oldValue);
		setting_GetCurrentSetting(&log.newValue);
		logQueue.id = eRequestWriteLog;
		memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));
		if(!logTask_SendToLogQueueReceive(logQueue, 10))
		{
			DEBUG_TASK("Failed to send Item to logQueueRecive from alarmMsgHandler_BatteryInvalidHandle");
		}
		else
		{
			s_isSendSuccess = true;
			keyPanel_PowerButtonToggle2S();
		}
		DEBUG_TASK("Power Lost Alarm \n");
	}
}


/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_HandleAlarm											*/
/*                                                                                      */
/* Details：  	Handling Alarm Queue to Update Alarm        						    */
/*                                                                                      */
/* Arguments :  ALARM_STAT_T alarm_Receive		 					  					*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data :	timeTickHighAlarm 								               	*/
/*                    	timeTickMedAlarm					                           	*/
/*                     	timeTickLowAlarm						   	                	*/
/*                                                                                      */
/* local variables   : 										      						*/
/*                      					                                            */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_HandleAlarm(ALARM_STAT_T alarmReceive)
{
	static bool s_isStartSequence = true;
	if(s_isStartSequence==true)
	{
		alarmMsgHandler_StartTimer();
		s_isStartSequence=false;
	}
	unsigned char event = eLastAlarmId;
	if(alarmReceive.currentStatus == eInactive)
	{
	    E_USBQUEUE usbQueue;
	    usbQueue.ID = eErrorCode;
	    usbQueue.data = event;
		if(!USBTask_SendToUSBQueue(usbQueue, 10)){
			DEBUG_TASK("Failed to send Item to USBQueue from alarmCtrl_HandlingQueueForAlarm");
		}
	}
	else if (alarmReceive.ID != eAlarmLowOxygenConcentration )
	{
		gs_isMuteButtonTOGGLE2S= false;
	}
	if(gs_isPowerButtonTOGGLE2S == false)
	{
		alarmMsgHandler_SendAlarmLog(alarmReceive);
	}
	if(alarmReceive.ID != eNoneAlarmId)
	{
		gs_alarmList[alarmReceive.ID].currentStatus = alarmReceive.currentStatus;
	}

    if((alarmReceive.ID == eALarmVeryLowBattery)&&(alarmReceive.currentStatus == eActive))
    {
    	alarmMsgHandler_BatteryVeryLowHandle();
    }

    else if((alarmReceive.ID == eAlarmBreathErr)&&(alarmReceive.currentStatus == eActive))
    {
    	breathTrigger_ResetDetectBreathMode();
    }
    else if((alarmReceive.ID == eAlarmDifferentialPressureSensorCommunication)&&(alarmReceive.currentStatus == eActive))
    {
        Board_I2C_Init(I2C1);
    }

	if(gs_highestPriority == eNoneAlarm)
	{
		timeTickHighAlarm = 0;
		timeTickMedAlarm = 0;
		timeTickLowAlarm = 0;
	}
    alarmMsgHandler_CreateAListOfActiveAlarm();
	alarmMsgHandler_PlayBlinkingSignal();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_StopDisplayErrorCode								*/
/*                                                                                      */
/* Details：  	Stop display Error Code to display Flow Rate Setting when The value		*/
/*              of Flow Rate Setting is changed by press volume button                  */
/*                                                                                      */
/*【Note】                                                                            										    */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_StopDisplayErrorCode()
{
	is_displayErrorCodeAgain = false;
	if (xTimerStart(gs_Start5STimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start  Alarm LED start 5S timer \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_GetStatusAlarm										*/
/*                                                                                      */
/* Details：  	Return Status of Alarm ( return True if it has not any Alarm)			*/
/*              														                */
/*                                                                                      */
/*【Note】                                                                            										   	*/
/*                                                                                      */
/****************************************************************************************/
bool alarmMsgHandler_GetStatusAlarm()
{
	return gs_isNoAlarm;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_GetStatusVolumeButtonOncePress						*/
/*                                                                                      */
/* Details：  	Return Status of Volume Button Once Press 								*/
/*              ( return True if it has not any Alarm )					                */
/*                                                                                      */
/*【Note】                                                                            										   	*/
/*                                                                                      */
/****************************************************************************************/
bool alarmMsgHandler_GetStatusVolumeButtonOncePress()
{
	return gs_isVolumeButtonOncePress;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name：  	alarmMsgHandler_IsGoingToPlayPowerLostAlarm							*/
/*                                                                                      */
/* Details：  	Return Status of Battery (when under 8%	 )								*/
/*              														                */
/*                                                                                      */
/*【Note】                                                                            										   	*/
/*                                                                                      */
/****************************************************************************************/
bool alarmMsgHandler_IsGoingToPlayPowerLostAlarm()
{
	return gs_isPowerLostOccur;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:   alarmMsgHandler_HandleKeyWhenAlarmOccur                             */
/*                                                                                      */
/* Details: Handle alarm when buttons is pressed                                        */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void alarmMsgHandler_HandleKeyWhenAlarmOccur()
{
    KEY_EVENT keyStatus[5];
    keyPanel_GetButtonStatus(keyStatus);
    if(keyStatus[ePowerButtonStatus] == TOGGLE2S)
        gs_isPowerButtonTOGGLE2S = true;
    if(keyStatus[eMuteButtonStatus] == TOGGLE2S)
        gs_isMuteButtonTOGGLE2S = true;
    if(keyStatus[eModeChangeButtonStatus] == TOGGLE)
    {
    	alarmMsgHandler_StopDisplayErrorCode();
    	alarmMsgHandler_DisplayFlowRateSettingAgain();
    }
    if((keyStatus[eDecreaseButtonStatus] == TOGGLE)||(keyStatus[eIncreaseButtonStatus]== TOGGLE))
    {
    	alarmMsgHandler_StopDisplayErrorCode();
        if((gs_isVolumeButtonOncePress == false)&&(gs_isNoAlarm == false))
        {
            gs_isVolumeButtonOncePress = true;
            alarmMsgHandler_DisplayFlowRateSettingAgain();
        }
    }
}


/* end of files */
