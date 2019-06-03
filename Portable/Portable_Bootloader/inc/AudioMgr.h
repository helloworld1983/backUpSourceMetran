#ifndef INC_AUDIOMGR_H_
#define INC_AUDIOMGR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : AudioMgr.h                                    	*/
/*                                                                  */
/* General      : Initialization driver audio, play and stop sound  */
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
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

typedef enum {
	eNoneAudio,
	eStartAudio,
	eFinishAudio,
	eZeroOneZeroContAudio,
	eZeroTwoFiveContAudio,
	eZeroFiveZeroContAudio,
	eZeroSevenFiveContAudio,
	eOneZeroZeroContAudio,
	eZeroTwoFiveSyncAudio,
	eZeroFiveZeroSycAudio,
	eZeroSevenFiveSyncAudio,
	eOneZeroZeroSyncAudio,
	eOneTwoFiveSyncAudio,
	eOneFiveZeroSycAudio,
	eOneSevenFiveSyncAudio,
	eTwoZeroZeroSyncAudio,
	eTwoFiveZeroSyncAudio,
	eThreeZeroZeroSyncAudio,
	eBuzzer100msAudio,
	eBuzzer200msAudio,
	eAlertContactAudio,
	eAlertCannulaAudio,
	eAlertChargetAudio,
	eAlertInspectAudio,
	eAlertRestartAudio,
	eAlertSyncOnlyAudio,
	eAlertBattertyAudio,
	eMuteAudio,
	ePowerACAudio,
	ePowerBatteryAudio,
	ePowerLostAudio,
	eContModeAudio,
	eSyncModeAudio,
	eMaintenanceModeAudio,
	eUpdateModeAudio,
	eVolumeLevel01,
	eVolumeLevel02,
	eVolumeLevel03,
	eVolumeLevel04,
	eVolumeLevel05,
	eVolumeLevel06,
	eVolumeLevel07,
	eVolumeLevel08,
	eVolumeLevel09,
	eVolumeLevel10,
	eContAudio,
	eSyncAudio,
	eZeroOneZeroAudio,
}E_ID_Audio;
typedef enum {
	eAudioVolumeLevel1=1,
	eAudioVolumeLevel2,
	eAudioVolumeLevel3,
	eAudioVolumeLevel4,
	eAudioVolumeLevel5,
}E_ID_AudioVolume;
typedef enum{
	eNoAlarmAudio,
	ehighAlarmAudio,
	eMediumAlarmAudio
}E_Alarm_Audio;

//Init audio device
void audioMgr_InitAudio();

//Play audio
void audioMgr_PlayAudio(E_ID_Audio id);
//Play audio
//void audioMgr_PlayAudio(E_ID_Audio id,uint16_t volume_value);

//Stop audio
void audioMgr_StopAudio();

//Stop medium audio
void audioMgr_StopMediumAudio();

//Stop timer audio
void audioMgr_StopTimerAudio();

void audioMgr_ReadRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize);

//Request audio
void audioMgr_Request(unsigned char event, E_Alarm_Audio alarmAudio);
bool audioMgr_IsSpeakerConnected();

//play power lost audio
void audioMgr_PlayPowerLostAlarmAudio();

//stop power lost audio
void audioMgr_StopPowerLostAlarmAudio();

//get status power lost play audio
bool audioMgr_GetStatusPowerLostAlarmAudio();
#endif /* INC_AUDIOMGR_H_ */
