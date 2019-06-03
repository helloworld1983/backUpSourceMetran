/********************************************************************/
/*                                                                  */
/* File Name    : ModeHandle.c	                                    */
/*                                                                  */
/* General       : This module contain function for manage Mode		*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          April 18, 2017      Thao Ha(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/
#include <AudioMgr.h>
#include <AudioTask.h>
#include <DualCoreCommon.h>
#include <FlowCtrl.h>
#include <DevInterface.h>
#include "ModeHandle.h"
#include "Setting.h"
#include "LogMgr.h"
#include "string.h"
#include "BreathTrigger.h"
#include "LogTask.h"

//#define DEBUG_LOG_EN
#ifdef DEBUG_MODE_EN
#define DEBUG_MODE(...) printf(__VA_ARGS__)
#else
#define DEBUG_MODE(...)
#endif
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	mode_ChangeMode						              				    */
/*                                                                                      */
/* Details:        Change current mode including play audio, change UI and record log	*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_ChangeMode()
{
	POR_LOG_T log ={'\0'};
	log.logCode=eChangeModeLogId;
	log.EventOrAlarm = eEvent;
	setting_GetCurrentSetting(&log.oldValue);
	uint8_t currentFlowRate=0;
	AUDIO_T eventAudio;
	eventAudio.volume = setting_Get(eAudioVolumeSettingID);
	if(setting_Get(eModeSettingID)==eSynchronizedMode)
	{
		currentFlowRate=setting_Get(eContinuousFLowRateSettingID);
		setting_Set(eModeSettingID,eContinuosMode);
		eventAudio.ID = (E_ID_Audio)(currentFlowRate+eZeroOneZeroContAudio);

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
	}
	else
	{
		currentFlowRate=setting_Get(eSynchronizedFlowRateSettingID);
		setting_Set(eModeSettingID,eSynchronizedMode);
		eventAudio.ID = (E_ID_Audio)(currentFlowRate + eZeroTwoFiveSyncAudio - 1);
		if(currentFlowRate == eFlowRateTwoFiveZero)
		{
			eventAudio.ID = eTwoFiveZeroSyncAudio;
		}
		if(currentFlowRate == eFlowRateThreeZeroZero)
		{
			eventAudio.ID = eThreeZeroZeroSyncAudio;
		}

		if(!audioTask_SendToAudioQueue(eventAudio, 10)){
			DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
		}
		breathTrigger_ResetDetectBreathMode();
		breathTrigger_ResetTestSynchronizeMode();
	}
	setting_GetCurrentSetting(&log.newValue);
	E_logQueueReceive logQueue;
	logQueue.id = eRequestWriteLog;
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from mode_ChangeMode");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	mode_IncreaseFlowRate				              				    */
/*                                                                                      */
/* Details:        Increase flowrate including play audio, change UI and record log		*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_IncreaseFlowRate()
{
	uint8_t currentFlowRate = 0;
	AUDIO_T eventAudio;
	eventAudio.volume=setting_Get(eAudioVolumeSettingID);
	if(setting_Get(eModeSettingID)==eSynchronizedMode)
	{
		currentFlowRate = setting_Get(eSynchronizedFlowRateSettingID);
		if(currentFlowRate < eFlowRateThreeZeroZero)
		{
			POR_LOG_T log ={'\0'};
			log.logCode=eChangeSynchronizeFlowrateLogId;
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			currentFlowRate++;
			if(currentFlowRate==eFlowRateTwoTowFive)
			{
				currentFlowRate = eFlowRateTwoFiveZero;
			}
			if(currentFlowRate==eFlowRateTwoSevenFive)
			{
				currentFlowRate = eFlowRateThreeZeroZero;
			}
			setting_Set(eSynchronizedFlowRateSettingID,currentFlowRate);
			eventAudio.ID = (E_ID_Audio)(currentFlowRate + eZeroTwoFiveSyncAudio - 1);
			if(currentFlowRate == eFlowRateTwoFiveZero)
			{
				eventAudio.ID = eTwoFiveZeroSyncAudio;
			}
			if(currentFlowRate == eFlowRateThreeZeroZero)
			{
				eventAudio.ID = eThreeZeroZeroSyncAudio;
			}

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			setting_GetCurrentSetting(&log.newValue);
			E_logQueueReceive logQueue;
						logQueue.id = eRequestWriteLog;
						memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

						if(!logTask_SendToLogQueueReceive(logQueue, 10))
						{
							DEBUG_TASK("Failed to send Item to logQueueRecive from mode_IncreaseFlowRate");
						}
		}
	}
	else
	{
		currentFlowRate = setting_Get(eContinuousFLowRateSettingID);
		if(currentFlowRate < eFlowRateOneZeroZero)
		{
			POR_LOG_T log ={'\0'};
			log.logCode=eChangeContinuousFlowrateLogId;
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			currentFlowRate++;
			setting_Set(eContinuousFLowRateSettingID,currentFlowRate);
			eventAudio.ID = (E_ID_Audio)(currentFlowRate + eZeroOneZeroContAudio);

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			setting_GetCurrentSetting(&log.newValue);
			E_logQueueReceive logQueue;
			logQueue.id = eRequestWriteLog;
			memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from mode_IncreaseFlowRate");
			}
		}
	}

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	mode_DecreaseFlowRate				              				    */
/*                                                                                      */
/* Details:        Decrease flowrate including play audio, change UI and record log		*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_DecreaseFlowRate()
{
	uint8_t currentFlowRate = 0;
	AUDIO_T eventAudio;
	eventAudio.volume = setting_Get(eAudioVolumeSettingID);
	if(setting_Get(eModeSettingID)==eSynchronizedMode)
	{
		currentFlowRate = setting_Get(eSynchronizedFlowRateSettingID);
		if(currentFlowRate > eFlowRateOneTwoFive)
		{
			POR_LOG_T log ={'\0'};
			log.logCode=eChangeSynchronizeFlowrateLogId;
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			currentFlowRate--;
			if(currentFlowRate==eFlowRateTwoTowFive)
			{
				currentFlowRate = eFlowRateTwoZeroZero;
			}
			if(currentFlowRate==eFlowRateTwoSevenFive)
			{
				currentFlowRate = eFlowRateTwoFiveZero;
			}
			setting_Set(eSynchronizedFlowRateSettingID,currentFlowRate);
			eventAudio.ID = (E_ID_Audio)(currentFlowRate+eZeroTwoFiveSyncAudio-1);
			if(currentFlowRate == eFlowRateTwoZeroZero)
				eventAudio.ID = eTwoZeroZeroSyncAudio;
			if(currentFlowRate == eFlowRateTwoFiveZero)
				eventAudio.ID = eTwoFiveZeroSyncAudio;

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			setting_GetCurrentSetting(&log.newValue);
			E_logQueueReceive logQueue;
						logQueue.id = eRequestWriteLog;
						memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

						if(!logTask_SendToLogQueueReceive(logQueue, 10)){
							DEBUG_TASK("Failed to send Item to logQueueRecive from mmode_DecreaseFlowRate");
						}

		}
	}
	else
	{
		currentFlowRate = setting_Get(eContinuousFLowRateSettingID);
		if(currentFlowRate > eFlowRateZeroOneZero)
		{
			POR_LOG_T log ={'\0'};
			log.logCode=eChangeContinuousFlowrateLogId;
			log.EventOrAlarm = eEvent;
			setting_GetCurrentSetting(&log.oldValue);
			currentFlowRate--;
			setting_Set(eContinuousFLowRateSettingID,currentFlowRate);
			eventAudio.ID = (E_ID_Audio)(currentFlowRate+eZeroOneZeroContAudio);

			if(!audioTask_SendToAudioQueue(eventAudio, 10)){
				DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
			}
			setting_GetCurrentSetting(&log.newValue);
			E_logQueueReceive logQueue;
			logQueue.id = eRequestWriteLog;
			memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

			if(!logTask_SendToLogQueueReceive(logQueue, 10)){
				DEBUG_TASK("Failed to send Item to logQueueRecive from mode_DecreaseFlowRate");
			}
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	mode_StartSequence					              				    */
/*                                                                                      */
/* Details:        Mode function handle when portable start up							*/
/*                                                                                      */
/* Arguments:	(I)	void																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void mode_StartSequence(void)
{
	uint8_t currentFlowRate=0;
	AUDIO_T eventAudio;
	eventAudio.volume=setting_Get(eAudioVolumeSettingID);
	POR_LOG_T log ={'\0'};
	log.EventOrAlarm = eEvent;
	setting_GetCurrentSetting(&log.oldValue);

	if(setting_Get(eModeSettingID)==eSynchronizedMode)
	{
		currentFlowRate=setting_Get(eSynchronizedFlowRateSettingID);
		eventAudio.ID = (E_ID_Audio)(currentFlowRate+eZeroTwoFiveSyncAudio-1);
		if(currentFlowRate == eFlowRateTwoFiveZero)
			eventAudio.ID = eTwoFiveZeroSyncAudio;
		if(currentFlowRate == eFlowRateThreeZeroZero)
			eventAudio.ID = eThreeZeroZeroSyncAudio;
	}
	else
	{
		currentFlowRate=setting_Get(eContinuousFLowRateSettingID);
		eventAudio.ID = (E_ID_Audio)(currentFlowRate+eZeroOneZeroContAudio);
	}

	if(!audioTask_SendToAudioQueue(eventAudio, 10)){
		DEBUG_TASK("Failed to send Item to audio Queue from alarmCtrl_PlayHighPriorityAlarmAudio");
	}
	setting_GetCurrentSetting(&log.newValue);

	log.logCode = eOperationStartLogId;
	E_logQueueReceive logQueue;
	logQueue.id = eRequestWriteLog;
	memcpy(logQueue.data,&log.year,sizeof(POR_LOG_T));

	if(!logTask_SendToLogQueueReceive(logQueue, 10)){
		DEBUG_TASK("Failed to send Item to logQueueRecive from mode_StartSequence");
	}
}
