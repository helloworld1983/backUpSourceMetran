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
/*  #0001    April 18,2017     Thao Ha(MV)		  	    new file    */
/*        															*/
/*                                                                  */
/********************************************************************/
#include "board.h"

typedef enum {
	eNoneAudio,
	eStartUpAudio,
	eTerminateAudio,
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

/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_InitAudio                                                         */
/*                                                                                      */
/* Details  -Initialize audio device with                                               */
/*           config Sample rate                                                         */
/*           config channel number                                                      */
/*           config word width                                                          */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_InitAudio();

/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_PlayAudio                                                         */
/*                                                                                      */
/* Details  -Play sound by assigned buffer                                              */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) E_ID_Audio id                                                */
/*                     (O) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayAudio(E_ID_Audio id,uint8_t volume);

/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_StopAudio                                                         */
/*                                                                                      */
/* Details  -Stop audio , reset buffet and clear interrupt                              */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*                                                                                      */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                     (O) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopAudio();

/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_ReadRegI2C                                                        */
/*                                                                                      */
/* Details  -Read data to I2C register                                                  */
/*                                                                                      */
/*                                                                                      */
/* Arguments :  (I)  uint8_t reg : address register                                     */
/*              (I)  uint16_t val : buffer value register                               */
/*              (I)  uint8_t dataSize : size buffer value register                      */
/*                                                                                      */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_ReadRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize);

/****************************************************************************************/
/* General： audioMgr_test                                                                */
/*                                                                                      */
/* Details：  test audio                                                                  */
/*                                                                                      */
/* Arguments : void                                                                     */
/*                                                                                      */
/* ReturnValue : void                                                                   */
//***************************************************************************************/
void audioMgr_test();

/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_IsSpeakerConnected                                                */
/*                                                                                      */
/* Details  -Send comment to check whether Speaker is connected to the mainboard        */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                     (O) NONE                                                         */
/*                     (O) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_IsSpeakerConnected();



/****************************************************************************************/
/*                                                                                      */
/* General  -audioMgr_GetSpeakerStatus                                                  */
/*                                                                                      */
/* Details  -Return whether Speaker is connected to the mainboard                       */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/*                                                                                      */
/* Using Global Data : (I) NONE                                                         */
/*                     (O) NONE                                                         */
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_GetSpeakerStatus();

#endif /* INC_AUDIOMGR_H_ */
