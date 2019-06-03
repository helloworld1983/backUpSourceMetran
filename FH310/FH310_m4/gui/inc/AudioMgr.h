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
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001        2017/08/25		Viet Le				mofify			*/
/*      Add define WAIT_BUFFER_FIFO                                 */
/* #0002        2017/09/28		Viet Le				mofify			*/
/*      -Edit define timer play high alarm sound (bug 1922)         */
/*		-Change uint8_t to E_ID_Audio in audioMgr_Request() function*/
/*			(bug 1920)												*/
/* #0003        2017/12/13		Viet Le				mofify			*/
/*    -Add argument void for coding rule							*/
/********************************************************************/

#include "DualCoreCommon.h"

#define I2CDEV_TLV320_ADDR     			(0x30 >> 1)
#define MAX_LEVEL_TRANSMIT_FIFO 		8//Level of the Transmit FIFO
#define LEVEL_TRANSMIT_FIFO 			4//Level of the Transmit FIFO
#define AUDIO_RESET_PORT       			GPIO_PORT(7)
#define AUDIO_RESET_BIT        			GPIO_BIT(20)
#define SPEAKER_DETECT 					224
#define SPEAKER_NOT_DETECT 				128
#define DATA_SIZE_VOICE_START 			253128
#define DATA_SIZE_VOICE_FINISH 			257210
#define DATA_SIZE_VOICE_TEST 			298030
#define DATA_SIZE_VOICE_ZERO_TWO_FIVE 	265374
#define DATA_SIZE_VOICE_ZERO_FIVE 		236220
#define DATA_SIZE_VOICE_ZERO_SEVEN_FIVE 277620
#define DATA_SIZE_VOICE_ONE 			212308
#define DATA_SIZE_VOICE_ONE_TWO_FIVE 	265374
#define DATA_SIZE_VOICE_ONE_FIVE 		244964
#define DATA_SIZE_VOICE_ONE_SEVEN_FIVE 	277620
#define DATA_SIZE_VOICE_TWO 			191898
#define DATA_SIZE_VOICE_TWO_FIVE 		240882
#define DATA_SIZE_VOICE_THREE 			212308
#define DATA_SIZE_VOICE_BUZZER_3S 		264644
#define DATA_SIZE_VOICE_BUZZER_100MS 	8864
#define DATA_SIZE_VOICE_BUZZER_200MS 	17684
#define DATA_SIZE_VOICE_ALERT_CONTACT 	359260
#define DATA_SIZE_VOICE_ALERT_TUBE 		285784
#define DATA_SIZE_VOICE_LEVEL01 		277620
#define DATA_SIZE_VOICE_LEVEL02 		265374
#define DATA_SIZE_VOICE_LEVEL03 		285784
#define DATA_SIZE_VOICE_LEVEL04 		293948
#define DATA_SIZE_VOICE_LEVEL05 		273538
#define DATA_SIZE_VOICE_LEVEL06 		269456
#define DATA_SIZE_VOICE_LEVEL07 		273538
#define DATA_SIZE_VOICE_LEVEL08 		277620
#define DATA_SIZE_VOICE_LEVEL09 		281702
#define DATA_SIZE_VOICE_LEVEL10 		281702
#define BUFFER_SIZE						4*1048
#define BUFFER_SIZE_I2C					3
#define BUFFER_SIZE_CHECK_SPEAKER		2
#define MAX_VOLUME 						10
#define VOLUME_1						1
#define VOLUME_2						2
#define VOLUME_3						3
#define VOLUME_4						4
#define VOLUME_5						5
#define VOLUME_6						6
#define VOLUME_7						7
#define VOLUME_8						8
#define VOLUME_9						9
//High alarm
#define HIGH_ALARM_BUZZER_ON_1MS		1
#define HIGH_ALARM_BUZZER_ON_250MS		25
#define HIGH_ALARM_BUZZER_ON_500MS		50
#define HIGH_ALARM_BUZZER_ON_1000MS		100
#define HIGH_ALARM_BUZZER_ON_1250MS		125

#define HIGH_ALARM_BUZZER_ON_2000MS		200
#define HIGH_ALARM_BUZZER_ON_2250MS		225
#define HIGH_ALARM_BUZZER_ON_2500MS		250
#define HIGH_ALARM_BUZZER_ON_3000MS		300
#define HIGH_ALARM_BUZZER_ON_3250MS		325
#define HIGH_ALARM_BUZZER_ON_4000MS		400

#define HIGH_ALARM_BUZZER_ON_10000MS	1000
//Medium alarm
#define MEDIUM_ALARM_BUZZER_ON_1MS		1
#define MEDIUM_ALARM_BUZZER_ON_400MS	40
#define MEDIUM_ALARM_BUZZER_ON_800MS	80
#define MEDIUM_ALARM_BUZZER_ON_2000MS	200
#define MEDIUM_ALARM_BUZZER_ON_25000MS	2500

#define REGISTER_CHECK_ALARM_SPEAKER	0x43
#define WAIT_BUFFER_FIFO				4

//Define audio id for audioMgr
typedef enum {
	eNoneAudio,
	eStartAudio,
	eFinishAudio,
	eTestAudio,
	eZeroTwoFiveAudio,
	eZeroFiveAudio,
	eZeroSevenFiveAudio,
	eOneAudio,
	eOneTwoFiveAudio,
	eOneFiveAudio,
	eOneSevenFiveAudio,
	eTwoAudio,
	eTwoFiveAudio,
	eThreeAudio,
	eBuzzer100msAudio,
	eBuzzer200msAudio,
	eBuzzer3sAudio,
	eAlertContactAudio,
	eAlertTubeAudio,
	eLevel01Audio,
	eLevel02Audio,
	eLevel03Audio,
	eLevel04Audio,
	eLevel05Audio,
	eLevel06Audio,
	eLevel07Audio,
	eLevel08Audio,
	eLevel09Audio,
	eLevel10Audio,
	eLevel01AlarmAudio,
	eLevel02AlarmAudio,
	eLevel03AlarmAudio,
	eLevel04AlarmAudio,
	eLevel05AlarmAudio,
	eLevel06AlarmAudio,
	eLevel07AlarmAudio,
	eLevel08AlarmAudio,
	eLevel09AlarmAudio,
	eLevel10AlarmAudio,
	eChildLockAudio,
	eAnyPalAudio,
	eEndAlarmAudio
}E_ID_Audio;

//Define status play alarm audio
typedef enum{
	eNoAlarmAudio,
	ehighAlarmAudio,
	eMediumAlarmAudio
}E_Alarm_Audio;

//Init audio manager
void audioMgr_InitAudio(void);
//Reset Audio
void audioMgr_ResetAudio(void);
//Play audio
void audioMgr_PlayAudio(E_ID_Audio id);
//Stop audio
void audioMgr_StopAudio(void);
//Stop timer in alarm
void audioMgr_StopTimerAudio(E_AlarmId alarmId);
//Write data to I2C register
void audioMgr_WriteRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize);
//Read data from I2C
void audioMgr_ReadRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize);
//Request from GUI
void audioMgr_Request(E_ID_Audio event, E_Alarm_Audio alarmAudio);
//Check alarm speaker
bool audioMgr_CheckAlarmSpeaker(void);
//Play current flow rate
void audioMgr_PlayFlowRate(uint8_t flowRate);
//Stop high alarm
void audioMgr_StopHighAlarm(void);
//Stop medium alarm
void audioMgr_StopMediumAlarm(void);
//Play high alarm
void audioMgr_PlayHighAlarm(void);
//Play medium alarm
void audioMgr_PlayMediumAlarm(void);
//Check and play audio
void audioMgr_CheckAndPlayAudio(void);

#endif /* INC_AUDIOMGR_H_ */
/* end of file */
