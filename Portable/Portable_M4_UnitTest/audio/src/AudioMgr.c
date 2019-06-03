/********************************************************************/
/*                                                                  */
/* File Name    : AudioMgr.c                                   	    */
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/


//#include <AudioMgr.h>
//#include "string.h"
//#include "Setting.h"
//#include "I2CInterface.h"
#include "AudioDataSize.h"


#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "string.h"

#define SPEAKER_NOT_DETECT 128

#define DEBUG_AUDIO_EN
#ifdef DEBUG_AUDIO_EN
#define DEBUG_AUDIO(...) printf(__VA_ARGS__)
#else
#define DEBUG_AUDIO(...)
#endif


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



//#define DATA_SIZE_VOICE_ALERT_CANNULA 297902
//#define DATA_SIZE_VOICE_ALERT_CHARGE 198194
//#define DATA_SIZE_VOICE_ALERT_CONTACT 319068
//#define DATA_SIZE_VOICE_ALERT_INSPECT 197908
//#define DATA_SIZE_VOICE_ALERT_RESTART 254776
//#define DATA_SIZE_VOICE_ALERT_SYNC_ONLY 310538
//#define DATA_SIZE_VOICE_MUTE 191970
//#define DATA_SIZE_VOICE_POWER_AC 262210
//#define DATA_SIZE_VOICE_POWER_BATTERY 254832
//#define DATA_SIZE_VOICE_ALERT_BATTERY 271620
//#define DATA_SIZE_VOICE_CONT 149988
//#define DATA_SIZE_VOICE_CONT_FLOW010 315150
//#define DATA_SIZE_VOICE_CONT_FLOW025 351632
//#define DATA_SIZE_VOICE_CONT_FLOW050 318204
//#define DATA_SIZE_VOICE_CONT_FLOW075 357278
//#define DATA_SIZE_VOICE_CONT_FLOW100 288594
//#define DATA_SIZE_VOICE_CONT_MODE 201094
//#define DATA_SIZE_VOICE_FINISH 260990
//#define DATA_SIZE_VOICE_FLOW010 231476
//#define DATA_SIZE_VOICE_MAINTENANCE_MODE 236442
//#define DATA_SIZE_VOICE_POWER_LOST 174974
//#define DATA_SIZE_VOICE_START 248828
//#define DATA_SIZE_VOICE_SYNC 160954
//#define DATA_SIZE_VOICE_SYNC_FLOW025 345968
//#define DATA_SIZE_VOICE_SYNC_FLOW050 322466
//#define DATA_SIZE_VOICE_SYNC_FLOW075 364720
//#define DATA_SIZE_VOICE_SYNC_FLOW100 278306
//#define DATA_SIZE_VOICE_SYNC_FLOW125 344202
//#define DATA_SIZE_VOICE_SYNC_FLOW150 313978
//#define DATA_SIZE_VOICE_SYNC_FLOW175 348432
//#define DATA_SIZE_VOICE_SYNC_FLOW200 266446
//#define DATA_SIZE_VOICE_SYNC_FLOW250 311794
//#define DATA_SIZE_VOICE_SYNC_FLOW300 288250
//#define DATA_SIZE_VOICE_SYNC_MODE 206382
//#define DATA_SIZE_VOICE_UPDATE_MODE 200932
//#define DATA_SIZE_VOICE_VOLUME_LEVEL01 248482
//#define DATA_SIZE_VOICE_VOLUME_LEVEL02 260890
//#define DATA_SIZE_VOICE_VOLUME_LEVEL03 252378
//#define DATA_SIZE_VOICE_VOLUME_LEVEL04 259184
//#define DATA_SIZE_VOICE_VOLUME_LEVEL05 254720
//#define DATA_SIZE_VOICE_VOLUME_LEVEL06 253124
//#define DATA_SIZE_VOICE_VOLUME_LEVEL07 261034
//#define DATA_SIZE_VOICE_VOLUME_LEVEL08 267972
//#define DATA_SIZE_VOICE_VOLUME_LEVEL09 262626
//#define DATA_SIZE_VOICE_VOLUME_LEVEL10 275870
//#define DATA_SIZE_VOICE_BUZZER_100MS 8910
//#define DATA_SIZE_VOICE_BUZZER_200MS 17730




#define BUFFER_SIZE			4*1048

#define BUFFER_FULL 0
#define BUFFER_EMPTY 1
#define BUFFER_AVAILABLE 2

unsigned char audioVoiceAlertCannula[DATA_SIZE_VOICE_ALERT_CANNULA];
unsigned char audioVoiceAlertCharge[DATA_SIZE_VOICE_ALERT_CHARGE];
unsigned char audioVoiceAlertContact[DATA_SIZE_VOICE_ALERT_CONTACT];
unsigned char audioVoiceAlertInspect[DATA_SIZE_VOICE_ALERT_INSPECT];
unsigned char audioVoiceAlertRestart[DATA_SIZE_VOICE_ALERT_RESTART];
unsigned char audioVoiceAlertSyncOnly[DATA_SIZE_VOICE_ALERT_SYNC_ONLY];
unsigned char audioVoiceMute[DATA_SIZE_VOICE_MUTE];
unsigned char audioVoicePowerAC[DATA_SIZE_VOICE_POWER_AC];
unsigned char audioVoicePowerBattery[DATA_SIZE_VOICE_POWER_BATTERY];
unsigned char audioVoiceAlertBattery[DATA_SIZE_VOICE_ALERT_BATTERY];
unsigned char audioVoiceCont[DATA_SIZE_VOICE_CONT];
unsigned char audioVoiceContFlow010[DATA_SIZE_VOICE_CONT_FLOW010];
unsigned char audioVoiceContFlow025[DATA_SIZE_VOICE_CONT_FLOW025];
unsigned char audioVoiceContFlow050[DATA_SIZE_VOICE_CONT_FLOW050];
unsigned char audioVoiceContFlow075[DATA_SIZE_VOICE_CONT_FLOW075];
unsigned char audioVoiceContFlow100[DATA_SIZE_VOICE_CONT_FLOW100];
unsigned char audioVoiceContMode[DATA_SIZE_VOICE_CONT_MODE];
unsigned char audioVoiceFinish[DATA_SIZE_VOICE_FINISH];
unsigned char audioVoiceFlow010[DATA_SIZE_VOICE_FLOW010];
unsigned char audioVoiceMaintenanceMode[DATA_SIZE_VOICE_MAINTENANCE_MODE];
unsigned char audioVoicePowerLost[DATA_SIZE_VOICE_POWER_LOST];
unsigned char audioVoiceStart[DATA_SIZE_VOICE_START];
unsigned char audioVoiceSync[DATA_SIZE_VOICE_SYNC];
unsigned char audioVoiceSyncFlow025[DATA_SIZE_VOICE_SYNC_FLOW025];
unsigned char audioVoiceSyncFlow050[DATA_SIZE_VOICE_SYNC_FLOW050];
unsigned char audioVoiceSyncFlow075[DATA_SIZE_VOICE_SYNC_FLOW075];
unsigned char audioVoiceSyncFlow100[DATA_SIZE_VOICE_SYNC_FLOW100];
unsigned char audioVoiceSyncFlow125[DATA_SIZE_VOICE_SYNC_FLOW125];
unsigned char audioVoiceSyncFlow150[DATA_SIZE_VOICE_SYNC_FLOW150];
unsigned char audioVoiceSyncFlow175[DATA_SIZE_VOICE_SYNC_FLOW175];
unsigned char audioVoiceSyncFlow200[DATA_SIZE_VOICE_SYNC_FLOW200];
unsigned char audioVoiceSyncFlow250[DATA_SIZE_VOICE_SYNC_FLOW250];
unsigned char audioVoiceSyncFlow300[DATA_SIZE_VOICE_SYNC_FLOW300];
unsigned char audioVoiceSyncMode[DATA_SIZE_VOICE_SYNC_MODE];
unsigned char audioVoiceUpdateMode[DATA_SIZE_VOICE_UPDATE_MODE];
unsigned char audioVoiceVolumeLevel01[DATA_SIZE_VOICE_VOLUME_LEVEL01];
unsigned char audioVoiceVolumeLevel02[DATA_SIZE_VOICE_VOLUME_LEVEL02];
unsigned char audioVoiceVolumeLevel03[DATA_SIZE_VOICE_VOLUME_LEVEL03];
unsigned char audioVoiceVolumeLevel04[DATA_SIZE_VOICE_VOLUME_LEVEL04];
unsigned char audioVoiceVolumeLevel05[DATA_SIZE_VOICE_VOLUME_LEVEL05];
unsigned char audioVoiceVolumeLevel06[DATA_SIZE_VOICE_VOLUME_LEVEL06];
unsigned char audioVoiceVolumeLevel07[DATA_SIZE_VOICE_VOLUME_LEVEL07];
unsigned char audioVoiceVolumeLevel08[DATA_SIZE_VOICE_VOLUME_LEVEL08];
unsigned char audioVoiceVolumeLevel09[DATA_SIZE_VOICE_VOLUME_LEVEL09];
unsigned char audioVoiceVolumeLevel10[DATA_SIZE_VOICE_VOLUME_LEVEL10];
unsigned char audioSquareWave100ms[DATA_SIZE_VOICE_BUZZER_100MS];
unsigned char audioSquareWave200ms[DATA_SIZE_VOICE_BUZZER_200MS];
//extern bool g_isServiceScreen;

static uint8_t gs_tx_buffer[BUFFER_SIZE];
static uint32_t gs_data_offset, gs_buffer_offset,gs_remain_data;
static uint32_t gs_tx_offset = 0;
static size_t gs_sizeAudio = 0;
static E_ID_Audio gs_idAudio = eNoneAudio;
static bool gs_speakerStatus = true;


void audioMgr_StopAudio();
/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_GetVolume				                                            */
/*                                                                                      */
/* Details  -Convert from E_ID_AudioVolume to value to send to Audio Chip	            */
/*                                                                                      */
/* Arguments : (I) E_ID_AudioVolume volume 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t	                                                            */
/*                                                                                      */
/****************************************************************************************/
uint16_t audioMgr_GetVolume(uint8_t volume)
{
	switch(volume)
	{
	case eAudioVolumeLevel1:
		return 6;
	case eAudioVolumeLevel2:
		return 7;
	case eAudioVolumeLevel3:
		return 8;
	case eAudioVolumeLevel4:
		return 9;
	case eAudioVolumeLevel5:
		return 10;
	default:
		return 10;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audio_memcpy					                                            */
/*                                                                                      */
/* Details  -Memory copy from SPI flash to ram 								            */
/*				 																		*/
/*                                                                                      */
/* Arguments : (I) void *des			 					  							*/
/*             (I) void *source															*/
/*             (O) size_t size                                                          */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audio_memcpy(void *des, void *source, size_t size)
{
//	__disable_irq();
	memcpy(des, source, size);
//	__enable_irq();
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitBuffer			                                            */
/*                                                                                      */
/* Details  -Initialize transmit buffer, reset buffer offset, data offset and           */
/*				 remaining data															*/
/*                                                                                      */
/* Arguments : (I) void *audio			 					  							*/
/*			   (I) size_t size															*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
uint32_t audioMgr_InitBuffer(void *audio, size_t size)
{
	gs_sizeAudio = size;
	audio_memcpy(gs_tx_buffer,audio, BUFFER_SIZE);
	gs_buffer_offset = 0;
	gs_data_offset = BUFFER_SIZE;
	gs_remain_data = size - BUFFER_SIZE;
	return gs_remain_data;
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_CallbackI2S		                                            	*/
/*                                                                                      */
/* Details  -I2S callback function, will be call when I2S send a half of buffer complete*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) void *audio			 					  							*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_CallbackI2S(void *audio)
{
	if(gs_remain_data >=BUFFER_SIZE/2)
	{
		if(gs_buffer_offset == BUFFER_SIZE)
		{
			// copy audio data into remain half of gs_tx_buffer
			audio_memcpy(gs_tx_buffer + BUFFER_SIZE/2, audio+gs_data_offset, BUFFER_SIZE/2);
			gs_buffer_offset = 0;
		}
		else
			// copy audio data into remain half of gs_tx_buffer
			audio_memcpy(gs_tx_buffer, audio+gs_data_offset, BUFFER_SIZE/2);
		gs_data_offset += BUFFER_SIZE/2;
		gs_remain_data -= BUFFER_SIZE/2;
	}
	else // reach the last copy
	{
		if(gs_buffer_offset == BUFFER_SIZE)
		{
			// copy audio data into remain half of gs_tx_buffer
			audio_memcpy(gs_tx_buffer + BUFFER_SIZE/2, audio+gs_data_offset, gs_remain_data);
			gs_buffer_offset = 0;
		}
		else
			// copy audio data into remain half of gs_tx_buffer
			audio_memcpy(gs_tx_buffer, audio+gs_data_offset, gs_remain_data);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-I2S0_IRQHandler	                                            			*/
/*                                                                                      */
/* Details  -I2S0 interrupt handler sub-routine											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/****************************************************************************************/

void I2S0_IRQHandler(void)
{
	uint32_t txlevel,i;
//	txlevel = Chip_I2S_GetTxLevel(LPC_I2S0);

	if(txlevel <= 4)
	{
		for(i=0;i<8-txlevel;i++)
		{
//			Chip_I2S_Send(LPC_I2S0, *(uint32_t *)(gs_tx_buffer + gs_buffer_offset));
			gs_tx_offset +=4;
			gs_buffer_offset +=4;
			if((gs_buffer_offset == BUFFER_SIZE/2)||(gs_buffer_offset == BUFFER_SIZE))
			{
				switch(gs_idAudio)
				{
				case eStartUpAudio:
					audioMgr_CallbackI2S(audioVoiceStart);
					break;
				case eTerminateAudio:
					audioMgr_CallbackI2S(audioVoiceFinish);
					break;
				case eZeroOneZeroContAudio:
					audioMgr_CallbackI2S(audioVoiceContFlow010);
					break;
				case eZeroTwoFiveContAudio:
					audioMgr_CallbackI2S(audioVoiceContFlow025);
					break;
				case eZeroFiveZeroContAudio:
					audioMgr_CallbackI2S(audioVoiceContFlow050);
					break;
				case eZeroSevenFiveContAudio:
					audioMgr_CallbackI2S(audioVoiceContFlow075);
					break;
				case eOneZeroZeroContAudio:
					audioMgr_CallbackI2S(audioVoiceContFlow100);
					break;
				case eZeroTwoFiveSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow025);
					break;
				case eZeroFiveZeroSycAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow050);
					break;
				case eZeroSevenFiveSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow075);
					break;
				case eOneZeroZeroSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow100);
					break;
				case eOneTwoFiveSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow125);
					break;
				case eOneFiveZeroSycAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow150);
					break;
				case eOneSevenFiveSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow175);
					break;
				case eTwoZeroZeroSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow200);
					break;
				case eTwoFiveZeroSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow250);
					break;
				case eThreeZeroZeroSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSyncFlow300);
					break;
				case eBuzzer100msAudio:
					audioMgr_CallbackI2S(audioSquareWave100ms);
					break;
				case eBuzzer200msAudio:
					audioMgr_CallbackI2S(audioSquareWave200ms);
					break;
				case eAlertContactAudio:
					audioMgr_CallbackI2S(audioVoiceAlertContact);
					break;
				case eAlertCannulaAudio:
					audioMgr_CallbackI2S(audioVoiceAlertCannula);
					break;
				case eAlertChargetAudio:
					audioMgr_CallbackI2S(audioVoiceAlertCharge);
					break;
				case eAlertInspectAudio:
					audioMgr_CallbackI2S(audioVoiceAlertInspect);
					break;
				case eAlertRestartAudio:
					audioMgr_CallbackI2S(audioVoiceAlertRestart);
					break;
				case eAlertSyncOnlyAudio:
					audioMgr_CallbackI2S(audioVoiceAlertSyncOnly);
					break;
				case eAlertBattertyAudio:
					audioMgr_CallbackI2S(audioVoiceAlertBattery);
					break;
				case eMuteAudio:
					audioMgr_CallbackI2S(audioVoiceMute);
					break;
				case ePowerACAudio:
					audioMgr_CallbackI2S(audioVoicePowerAC);
					break;
				case ePowerBatteryAudio:
					audioMgr_CallbackI2S(audioVoicePowerBattery);
					break;
				case ePowerLostAudio:
					audioMgr_CallbackI2S(audioVoicePowerLost);
					break;
				case eContModeAudio:
					audioMgr_CallbackI2S(audioVoiceContMode);
					break;
				case eSyncModeAudio:
					audioMgr_CallbackI2S(audioVoiceSyncMode);
					break;
				case eMaintenanceModeAudio:
					audioMgr_CallbackI2S(audioVoiceMaintenanceMode);
					break;
				case eUpdateModeAudio:
					audioMgr_CallbackI2S(audioVoiceUpdateMode);
					break;
				case eVolumeLevel01:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel01);
					break;
				case eVolumeLevel02:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel02);
					break;
				case eVolumeLevel03:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel03);
					break;
				case eVolumeLevel04:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel04);
					break;
				case eVolumeLevel05:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel05);
					break;
				case eVolumeLevel06:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel06);
					break;
				case eVolumeLevel07:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel07);
					break;
				case eVolumeLevel08:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel08);
					break;
				case eVolumeLevel09:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel09);
					break;
				case eVolumeLevel10:
					audioMgr_CallbackI2S(audioVoiceVolumeLevel10);
					break;
				case eContAudio:
					audioMgr_CallbackI2S(audioVoiceCont);
					break;
				case eSyncAudio:
					audioMgr_CallbackI2S(audioVoiceSync);
					break;
				case eZeroOneZeroAudio:
					audioMgr_CallbackI2S(audioVoiceFlow010);
					break;
				default:
					break;
				}
			}
			if(gs_tx_offset >= gs_sizeAudio)
			{
				gs_tx_offset = 0;
				gs_buffer_offset = 0;
				gs_data_offset = BUFFER_SIZE;
				gs_remain_data = gs_sizeAudio - BUFFER_SIZE;
//				NVIC_DisableIRQ(I2S0_IRQn);
//				Chip_I2S_TxStop(LPC_I2S0);
			}
		}
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_WriteRegI2C                                            		    */
/*                                                                                      */
/* Details  -Write data to I2C register													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t reg : address register										*/
/* 			   (I) uint16_t val : buffer value register									*/
/* 			   (I) uint8_t dataSize : size buffer value register						*/
/*																						*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_WriteRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize)
{
	uint8_t dat[3];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;

//	I2CM_XFER_T i2cData;
//
//	i2cData.slaveAddr = (0x30 >> 1);
//	i2cData.options = 0;
//	i2cData.status = 0;
//	i2cData.txBuff = dat;
//	i2cData.txSz = dataSize + 1;
//	i2cData.rxBuff = NULL;
//	i2cData.rxSz = 0;
//
//	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
//	{
//		DEBUG_AUDIO("\nSend to I2C Error******************");
//	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_ReadRegI2C                                            		    */
/*                                                                                      */
/* Details  -Read data to I2C register													*/
/*																						*/
/*                                                                                      */
/* Arguments : 	(I)  uint8_t reg : address register										*/
/* 			 	(I)  uint16_t val : buffer value register								*/
/* 			 	(I)  uint8_t dataSize : size buffer value register						*/
/*																						*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_ReadRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize)
{
	uint8_t dat[3];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;

//	I2CM_XFER_T i2cData;
//
//	i2cData.slaveAddr = (0x30 >> 1);
//	i2cData.options = 0;
//	i2cData.status = 0;
//	i2cData.txBuff = NULL;//rx_data;
//	i2cData.txSz = 0;
//	i2cData.rxBuff = dat;
//	i2cData.rxSz = dataSize + 1;
//
//	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
//	{
//		DEBUG_AUDIO("\nSend to I2C Error******************");
//	}
//
//	for(int i = 0; i < 3; i++)
//	{
//		DEBUG_AUDIO("\n audioMgr_ReadRegI2C[%d] = %d", i,dat[i]);
//	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_SetVolumeVoice                                           		    */
/*                                                                                      */
/* Details  -Set volume	audio														    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint16_t value : value volume					 					*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_SetVolumeVoice(uint16_t value)
{
	//	audioMgr_WriteRegI2C(0x20,0x8600, 1);
	switch(value)
	{
	case 0 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xE500, 1);// Analog Volume to SPK, set = -51dB
		break; //Volume 0(MIN)
	case 1 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xE200, 1);// Analog Volume to SPK, set = -49.3dB
		break;
	case 2 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xD400, 1);// Analog Volume to SPK, set = -42.1dB
		break;
	case 3:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xC600, 1);// Analog Volume to SPK, set = -35.2dB
		break;
	case 4 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xBE00, 1);// Analog Volume to SPK, set = -31.1dB
		break;
	case 5:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xB800, 1);// Analog Volume to SPK, set = -28.1dB
		break;
	case 6 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xB200, 1);// Analog Volume to SPK, set = -25.1dB
		break;
	case 7:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xAA00, 1);// Analog Volume to SPK, set = -21.1dB
		break;
	case 8:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		//		audioMgr_WriteRegI2C(0x26,0x9C00, 1);// Analog Volume to SPK, set = -14dB
		audioMgr_WriteRegI2C(0x26,0xA500, 1);// Analog Volume to SPK, set = -18.6dB
		break;
	case 9:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		//		audioMgr_WriteRegI2C(0x26,0x8E00, 1);// Analog Volume to SPK, set = -7dB
		//		audioMgr_WriteRegI2C(0x26,0x9C00, 1);// Analog Volume to SPK, set = -14dB
		audioMgr_WriteRegI2C(0x26,0xA200, 1);// Analog Volume to SPK, set = -17dB
		break;
	case 10 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		//				audioMgr_WriteRegI2C(0x26,0x8000, 1);// Analog Volume to SPK, set = 0dB
		//		audioMgr_WriteRegI2C(0x26,0x9400, 1);// Analog Volume to SPK, set = -10dB
		audioMgr_WriteRegI2C(0x26,0x8E00, 1);// Analog Volume to SPK, set = -7dB
		break; //Volume 10(MAX)
	default :
		return;
	}

	audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
	audioMgr_WriteRegI2C(0x20,0x8600, 1);//Add 03-10-2016
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitRegI2C	                                            		*/
/*                                                                                      */
/* Details  -Write to I2C 															    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint16_t value : value volume					 					*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
static void audioMgr_InitRegI2C()
{
	audioMgr_WriteRegI2C(0x00,0x0000, 1);// Set register page to 0

	audioMgr_WriteRegI2C(0x01,0x0100, 1);// Software reset resistor
	audioMgr_WriteRegI2C(0x04,0x0700, 1);// Clock-Gen_Muxing PLL_CLKIN = BCLK, CODEC_CLKIN = PLL_CLK
	audioMgr_WriteRegI2C(0x05,0x9100, 1);// PLL P & R values: PLL powered up, P = 1, R = 1
	audioMgr_WriteRegI2C(0x06,0x3C00, 1);// PLL J value: J = 60
	audioMgr_WriteRegI2C(0x07,0x0000, 1);// PLL D value MBS: D = 00
	audioMgr_WriteRegI2C(0x08,0x0000, 1);// PLL D value LBS: D = 00
	audioMgr_WriteRegI2C(0x0B,0x8300, 1);// NDAC is powered up and set to 3
	audioMgr_WriteRegI2C(0x0C,0x8500, 1);// MDAC is powered up and set to 5
	audioMgr_WriteRegI2C(0x0D,0x0000, 1);// DOSR value MSB = 0
	audioMgr_WriteRegI2C(0x0E,0x8000, 1);// DOSR value MSB = 128
	audioMgr_WriteRegI2C(0x1B,0x0000, 1);// Mode is I2S, wordlength is 16, slave mode

	audioMgr_WriteRegI2C(0x00,0x0100, 1);// Set register page to 1
	audioMgr_WriteRegI2C(0x1F,0x0400, 1);// Headphone drivers powered down
	audioMgr_WriteRegI2C(0x20,0x8600, 1);// Power-down Class-D driver, 0x86 is powered-up, 0x06 is powered-down
	audioMgr_WriteRegI2C(0x21,0x7F00, 1);// HP_Output_Drivers_POP_Removal_Settings
	audioMgr_WriteRegI2C(0x23,0x4400, 1);// DAC_Output_Routing

	audioMgr_WriteRegI2C(0x2A,0x1C00, 1);// Unmute Class-D, set gain = 24 dB, 0x14 = 18dB
	audioMgr_WriteRegI2C(0x26,0x8000, 1);// Analog Volume to SPK, set 0x92= -9dB, 0x80 = 0dB

	audioMgr_WriteRegI2C(0x2E,0x0B00, 1);//Test detected speaker

	audioMgr_WriteRegI2C(0x00,0x0000, 1);// Set register page to 0
	audioMgr_WriteRegI2C(0x3F,0xD400, 1);// Power-up DAC left and right channels (soft step enabled)
	audioMgr_WriteRegI2C(0x41,0x0000, 1);// DAC Left gain 0xD4 = -22 dB, 0x02 = 1dB, 0x0A = 5dB
	audioMgr_WriteRegI2C(0x42,0x0000, 1);// DAC Right gain 0xD4 = -22 dB, 0x02 = 1dB, 0x0A = 5dB
	audioMgr_WriteRegI2C(0x40,0x0000, 1);// Unmute DAC left and right channels

	audioMgr_WriteRegI2C(0x43,0x8000, 1);//Test detected speaker

	//Test detected speaker
	audioMgr_WriteRegI2C(0x00,0x0300, 1);//Test detected speaker
	audioMgr_WriteRegI2C(0x10,0x0100, 1);//Test detected speaker

	//	audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
	//	audioMgr_WriteRegI2C(0x20,0x8600, 1);//Add 03-10-2016

}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitAudio			                                            */
/*                                                                                      */
/* Details  -Initialize audio device with 												*/
/*           config Sample rate															*/
/*           config channel number														*/
/*           config word width											                */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_InitAudio()
{
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,1, 13);
//	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,1, 13);
	audioMgr_InitRegI2C();
//	I2S_AUDIO_FORMAT_T audio_Confg;
//	audio_Confg.SampleRate = 44100;
//	audio_Confg.ChannelNumber = 1;
//	audio_Confg.WordWidth = 16;

//	Chip_I2S_Init(LPC_I2S0);
//	if(Chip_I2S_TxConfig(LPC_I2S0, &audio_Confg) != SUCCESS)
//	{
//		DEBUG_AUDIO("Error config I2S");
//		return;
//	}
//
//	Chip_I2S_TxStop(LPC_I2S0);
//	NVIC_SetPriority(I2S0_IRQn,3);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_PlayAudio                                            				*/
/*                                                                                      */
/* Details  -Play sound by assigned buffer  											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) E_ID_Audio id				                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayAudio(E_ID_Audio id, uint8_t volume)
{
	audioMgr_StopAudio();
	audioMgr_SetVolumeVoice(audioMgr_GetVolume(volume));
	switch(id)
	{
	case eStartUpAudio:
		audioMgr_InitBuffer(audioVoiceStart, DATA_SIZE_VOICE_START);
		break;
	case eTerminateAudio:
		audioMgr_InitBuffer(audioVoiceFinish, DATA_SIZE_VOICE_FINISH);
		break;
	case eZeroOneZeroContAudio:
		audioMgr_InitBuffer(audioVoiceContFlow010, DATA_SIZE_VOICE_CONT_FLOW010);
		break;
	case eZeroTwoFiveContAudio:
		audioMgr_InitBuffer(audioVoiceContFlow025, DATA_SIZE_VOICE_CONT_FLOW025);
		break;
	case eZeroFiveZeroContAudio:
		audioMgr_InitBuffer(audioVoiceContFlow050, DATA_SIZE_VOICE_CONT_FLOW050);
		break;
	case eZeroSevenFiveContAudio:
		audioMgr_InitBuffer(audioVoiceContFlow075, DATA_SIZE_VOICE_CONT_FLOW075);
		break;
	case eOneZeroZeroContAudio:
		audioMgr_InitBuffer(audioVoiceContFlow100, DATA_SIZE_VOICE_CONT_FLOW100);
		break;
	case eZeroTwoFiveSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow025, DATA_SIZE_VOICE_SYNC_FLOW025);
		break;
	case eZeroFiveZeroSycAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow050, DATA_SIZE_VOICE_SYNC_FLOW050);
		break;
	case eZeroSevenFiveSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow075, DATA_SIZE_VOICE_SYNC_FLOW075);
		break;
	case eOneZeroZeroSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow100, DATA_SIZE_VOICE_SYNC_FLOW100);
		break;
	case eOneTwoFiveSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow125, DATA_SIZE_VOICE_SYNC_FLOW125);
		break;
	case eOneFiveZeroSycAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow150, DATA_SIZE_VOICE_SYNC_FLOW150);
		break;
	case eOneSevenFiveSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow175, DATA_SIZE_VOICE_SYNC_FLOW175);
		break;
	case eTwoZeroZeroSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow200, DATA_SIZE_VOICE_SYNC_FLOW200);
		break;
	case eTwoFiveZeroSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow250, DATA_SIZE_VOICE_SYNC_FLOW250);
		break;
	case eThreeZeroZeroSyncAudio:
		audioMgr_InitBuffer(audioVoiceSyncFlow300, DATA_SIZE_VOICE_SYNC_FLOW300);
		break;
	case eBuzzer100msAudio:
		audioMgr_InitBuffer(audioSquareWave100ms, DATA_SIZE_VOICE_BUZZER_100MS);
		break;
	case eBuzzer200msAudio:
		audioMgr_InitBuffer(audioSquareWave200ms, DATA_SIZE_VOICE_BUZZER_200MS);
		break;
	case eAlertContactAudio:
		audioMgr_InitBuffer(audioVoiceAlertContact, DATA_SIZE_VOICE_ALERT_CONTACT);
		break;
	case eAlertCannulaAudio:
		audioMgr_InitBuffer(audioVoiceAlertCannula, DATA_SIZE_VOICE_ALERT_CANNULA);
		break;
	case eAlertChargetAudio:
		audioMgr_InitBuffer(audioVoiceAlertCharge, DATA_SIZE_VOICE_ALERT_CHARGE);
		break;
	case eAlertInspectAudio:
		audioMgr_InitBuffer(audioVoiceAlertInspect, DATA_SIZE_VOICE_ALERT_INSPECT);
		break;
	case eAlertRestartAudio:
		audioMgr_InitBuffer(audioVoiceAlertRestart, DATA_SIZE_VOICE_ALERT_RESTART);
		break;
	case eAlertSyncOnlyAudio:
		audioMgr_InitBuffer(audioVoiceAlertSyncOnly, DATA_SIZE_VOICE_ALERT_SYNC_ONLY);
		break;
	case eAlertBattertyAudio:
		audioMgr_InitBuffer(audioVoiceAlertBattery, DATA_SIZE_VOICE_ALERT_BATTERY);
		break;
	case eMuteAudio:
		audioMgr_InitBuffer(audioVoiceMute, DATA_SIZE_VOICE_MUTE);
		break;
	case ePowerACAudio:
		audioMgr_InitBuffer(audioVoicePowerAC, DATA_SIZE_VOICE_POWER_AC);
		break;
	case ePowerBatteryAudio:
		audioMgr_InitBuffer(audioVoicePowerBattery, DATA_SIZE_VOICE_POWER_BATTERY);
		break;
	case ePowerLostAudio:
		audioMgr_InitBuffer(audioVoicePowerLost, DATA_SIZE_VOICE_POWER_LOST);
		break;
	case eContModeAudio:
		audioMgr_InitBuffer(audioVoiceContMode, DATA_SIZE_VOICE_CONT_MODE);
		break;
	case eSyncModeAudio:
		audioMgr_InitBuffer(audioVoiceSyncMode, DATA_SIZE_VOICE_SYNC_MODE);
		break;
	case eMaintenanceModeAudio:
		audioMgr_InitBuffer(audioVoiceMaintenanceMode, DATA_SIZE_VOICE_MAINTENANCE_MODE);
		break;
	case eUpdateModeAudio:
		audioMgr_InitBuffer(audioVoiceUpdateMode, DATA_SIZE_VOICE_UPDATE_MODE);
		break;
	case eVolumeLevel01:
		audioMgr_InitBuffer(audioVoiceVolumeLevel01, DATA_SIZE_VOICE_VOLUME_LEVEL01);
		break;
	case eVolumeLevel02:
		audioMgr_InitBuffer(audioVoiceVolumeLevel02, DATA_SIZE_VOICE_VOLUME_LEVEL02);
		break;
	case eVolumeLevel03:
		audioMgr_InitBuffer(audioVoiceVolumeLevel03, DATA_SIZE_VOICE_VOLUME_LEVEL03);
		break;
	case eVolumeLevel04:
		audioMgr_InitBuffer(audioVoiceVolumeLevel04, DATA_SIZE_VOICE_VOLUME_LEVEL04);
		break;
	case eVolumeLevel05:
		audioMgr_InitBuffer(audioVoiceVolumeLevel05, DATA_SIZE_VOICE_VOLUME_LEVEL05);
		break;
	case eVolumeLevel06:
		audioMgr_InitBuffer(audioVoiceVolumeLevel06, DATA_SIZE_VOICE_VOLUME_LEVEL06);
		break;
	case eVolumeLevel07:
		audioMgr_InitBuffer(audioVoiceVolumeLevel07, DATA_SIZE_VOICE_VOLUME_LEVEL07);
		break;
	case eVolumeLevel08:
		audioMgr_InitBuffer(audioVoiceVolumeLevel08, DATA_SIZE_VOICE_VOLUME_LEVEL08);
		break;
	case eVolumeLevel09:
		audioMgr_InitBuffer(audioVoiceVolumeLevel09, DATA_SIZE_VOICE_VOLUME_LEVEL09);
		break;
	case eVolumeLevel10:
		audioMgr_InitBuffer(audioVoiceVolumeLevel10, DATA_SIZE_VOICE_VOLUME_LEVEL10);
		break;
	case eContAudio:
		audioMgr_InitBuffer(audioVoiceCont, DATA_SIZE_VOICE_CONT);
		break;
	case eSyncAudio:
		audioMgr_InitBuffer(audioVoiceSync, DATA_SIZE_VOICE_SYNC);
		break;
	case eZeroOneZeroAudio:
		audioMgr_InitBuffer(audioVoiceFlow010, DATA_SIZE_VOICE_FLOW010);
		break;
	default:
		break;
	}

	gs_idAudio = id;

//	Chip_I2S_TxStop(LPC_I2S0);//Add 03-10-2016
//	Chip_I2S_DisableMute(LPC_I2S0);//Add 03-10-2016
//	Chip_I2S_TxStart(LPC_I2S0);
//
//	Chip_I2S_Int_TxCmd(LPC_I2S0, ENABLE, 4);
//	NVIC_EnableIRQ(I2S0_IRQn);

}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_StopAudio                                            				*/
/*                                                                                      */
/* Details  -Stop audio , reset buffet and clear interrupt   							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopAudio()
{
	gs_tx_offset = 0;
	gs_buffer_offset = 0;
	gs_data_offset = BUFFER_SIZE;
	gs_remain_data = gs_sizeAudio - BUFFER_SIZE;

	//	audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
	//	audioMgr_WriteRegI2C(0x20,0x0600, 1);//Add 03-10-2016

//	NVIC_DisableIRQ(I2S0_IRQn);
//	NVIC_ClearPendingIRQ(I2S0_IRQn);//Add 03-10-2016
//	Chip_I2S_EnableMute(LPC_I2S0);//Add 03-10-2016
//	Chip_I2S_TxStop(LPC_I2S0);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_IsSpeakerConnected                                   				*/
/*                                                                                      */
/* Details  -Send comment to check whether Speaker is connected to the mainboard		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_IsSpeakerConnected()
{
//	audioMgr_WriteRegI2C(0x00,0x0000,1);
//	uint8_t writeCmd=0;
//	writeCmd=0x43;
//	I2CM_XFER_T i2cData;
//	i2cData.slaveAddr = (0x30 >> 1);
//	i2cData.options = 0;
//	i2cData.status = 0;
//	i2cData.txBuff = &writeCmd;
//	i2cData.txSz = 1;
//	i2cData.rxBuff = NULL;
//	i2cData.rxSz = 0;
//	uint8_t data=0;
//	if(i2CInterface_XferBlocking(LPC_I2C1, &i2cData) == 0)
//	{
//		DEBUG_AUDIO("\nSend to I2C Error******************");
//	}
//	else
//	{
//		I2C1_Read((0x30 >> 1)  ,&data,1);
//	}
//
//	if(data == SPEAKER_NOT_DETECT)
//	{
//		gs_speakerStatus = false;
//	}
//	else
//	{
//		gs_speakerStatus = true;
//	}
	return gs_speakerStatus;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_GetSpeakerStatus                                   				*/
/*                                                                                      */
/* Details  -Return whether Speaker is connected to the mainboard						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_GetSpeakerStatus()
{
	return gs_speakerStatus;
}
