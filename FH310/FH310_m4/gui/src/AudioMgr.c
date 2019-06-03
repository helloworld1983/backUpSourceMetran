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
/* #0001       2017/08/25		Viet Le				modify			*/
/*        Add wait buffer FIFO in I2S0_IRQHandler()                 */
/* #0002       2017/09/01		Quyen Ngo		    modify			*/
/*        Cast to argument of (audio_memcpy())						*/
/* #0003	   2017/09/01		Quyen Ngo			modify			*/
/*		  Check how much data sent before call I2S send function	*/
/* #0004	   2017/09/05		Viet Le				modify			*/
/*        remove #include "assert.h"								*/
/* #0005	   2017/09/19		Viet Le				modify			*/
/*		  remove check previous mode in audioMgr_CheckAndPlayAudio()*/
/*				function 											*/
/* #0006  	   2017/09/28		Quyen Ngo			modify			*/
/*		  Add sequence to handle the case that I2C bus error when 	*/
/*		  detect alarm speaker										*/
/* #0007        2017/09/28		Viet Le				mofify			*/
/*      -Edit define timer play high alarm sound (bug 1922)         */
/*		-Change uint8_t to E_ID_Audio in audioMgr_Request() function*/
/*			(bug 1920)												*/
/* #0008        2017/10/18		Viet Le				mofify			*/
/*	    -Handler case : when play test buzzer is finish, 			*/
/*		when buzzer test not finish yet + voice test on,			*/
/*		when voice test not finish yet + buzzer test on (bug 1981)  */
/* #0009       2017/10/27	   Viet Le				modify			*/
/*	-Add flag s_isEndStopAudio is flase and 						*/
/*	s_playAnypalAfterAudioOther is false,  when audio stop finish set*/
/*	s_isEndStopAudio is true, turn off system set s_isEndStopAudio is*/
/*	false.When anypal connect check s_isEndStopAudio is true play	*/
/*	buzzer anypal, if s_isEndStopAudio is false set 				*/
/*	s_playAnypalAfterAudioOther is true, wait audio stop finish play */
/*	buzzer anypal set s_playAnypalAfterAudioOther is false(bug 1997)*/
/* #0010       2017/10/27	   Viet Le				modify			*/
/*	-Add flag s_isEndFRAudio is false, when audio flowrate finish set*/
/*	s_isEndFRAudio is true, turn off system or change flow rate     */
/*  set s_isEndFRAudio is false.When anypal connect check 			*/
/*	s_isEndFRAudio is true play	buzzer anypal, if s_isEndFRAudio 	*/
/*	is false set s_playAnypalAfterAudioOther is true, wait audio    */
/*	 flowrate finish play buzzer anypal set s_playAnypalAfterAudioOther*/
/*	 is false(bug 1997)												*/
/* #0011       2017/10/27	   Viet Le				modify			*/
/*	Copy audio to external RAM when play audio.						*/
/*     Normal case play audio with s_txBuffer.						*/
/*	Remove : audioMgr_CallbackI2S() function,s_dataOffset variable,	*/
/*	s_remainData variable (bug 1997)								*/
/* #0012       2017/10/30	   Viet Le				modify			*/
/*	Edit Copy audio volume level to RAM when play voice alarm volume*/
/*	(bug 2009)														*/
/* #0013       2017/11/16	   Viet Le				modify			*/
/*	- Add flag s_isEndAnypalAudio is flase and 						*/
/*	s_playChildlockAfterAnypal is false,when audio anypal finish	*/
/*  set s_isEndAnypalAudio is true.When child lock buzzer play 		*/
/*	during anypal buzzer, set s_playChildlockAfterAnypal = true,wait*/
/*   finish anypal buzzer finish, play child lock buzzer(bug 1997)	*/
/* #0014       2017/11/16	   Viet Le				modify			*/
/*	- Add s_counterChildlock = 0, when child lock finish 			*/
/* s_counterChildlock = 2. When anypal connect during play 			*/
/*	child lock  buzzer, wait child lock buzzer finish,				*/
/*	play buzzer anypal(bug 1997)									*/
/* #0015       2017/11/16	   Viet Le				modify			*/
/*  - When alarm with audio not play child lock buzzer and			*/
/*		 anypal buzzer(bug 1997)									*/
/* #0016       2017/11/16	   Viet Le				modify			*/
/*  - Change s_isPlayAudio to s_isPendingAudio.  When change 		*/
/*	flow rate in alarm cannula more 27s, alarm cannula inactive		*/
/*	set s_isPendingAudio = true and play flow rate setting.			*/
/*		(bug 1997 + 2024)											*/
/* #0017       2017/11/17		Viet Le				modify			*/
/*		-When alarm cannula active during alarm O2 					*/
/*	concentration lowering, stop audio high alarm(bug 2033) 		*/
/* #0018       2017/11/20		Viet Le				modify			*/
/*	 - Remove s_playChildlockAfterAnypal = false in case play child */
/*    lock buzzer during play buzzer anypal(bug 1997)				*/
/* #0019       2017/11/22		Quyen Ngo			modify			*/
/*	 - Add function to reset audio 									*/
/*    																*/
/********************************************************************/
#include "string.h"
#include "board.h"
#include "AudioMgr.h"
#include "setting.h"
#include "GuiTask.h"
#include "OperationScreen.h"
#include "ModeMgr.h"
#include "GuiHandler.h"
#include "StandbyScreen.h"
#include "AudioFile.h"
#include "SettingOnOffButton.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "I2CInterface.h"
#include "AnyPalDisplayHandler.h"
#include "AlarmScreenMgr.h"
#include "LowAlarmScreen.h"

__BSS(RAM9) static uint8_t s_txBuffer[DATA_SIZE_VOICE_ALERT_CONTACT];
static uint32_t s_bufferOffset;
static uint32_t s_txOffset = 0;
static size_t s_sizeAudio = 0;
static E_ID_Audio s_idAudio = eNoneAudio;
static uint16_t s_ttick = 0;
static E_AlarmId s_alarmEndId = eLastAlarmId;
static bool s_isPendingAudio = false;
static bool s_isEndStopAudio = false;
static bool s_isEndFRAudio = false;
static bool s_playAnypalAfterAudioOther = false;
static bool s_isEndAnypalAudio = false;
static bool s_playChildlockAfterAnypal = false;
static uint8_t s_counterChildlock = 0;

/****************************************************************************************/
/*                                                                                      */
/* General 	-audio_memcpy					                                            */
/*                                                                                      */
/* Details  -Memory copy from SPI flash to ram 								            */
/*				 																		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) void *des : destination buffer					 					*/
/*				   void *source : source buffer											*/
/*                 size_t size  : size of buffer                                        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audio_memcpy(void *des, void *source, size_t size)
{
	__disable_irq();
	memcpy(des, source, size);
	__enable_irq();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitBuffer		                                            	*/
/*                                                                                      */
/* Details  -Initialize transmit buffer, reset buffer offset, data offset and           */
/*				 remaining data															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) void *audio : buffer audio					 					  	*/
/*				   size_t size : size of buffer											*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_sizeAudio : size of current audio                          */
/*                     (O) s_bufferOffset : buffer offset						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_InitBuffer(void *audio, size_t size)
{
	s_sizeAudio = size;
	audio_memcpy(s_txBuffer,audio, size);
	s_bufferOffset = 0;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-I2S0_IRQHandler		                                            		*/
/*                                                                                      */
/* Details  -I2S0 interrupt handler sub-routine											*/
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
/*                     (O) s_txOffset : transmit offset						        	*/
/*                     (O) s_bufferOffset : buffer offset						        */
/*                     (O) s_isPendingAudio : flag play audio							*/
/*                     (O) s_isEndAnypalAudio : flag end anypal audio				    */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void I2S0_IRQHandler(void)
{
	uint32_t txlevel,i;
	txlevel = Chip_I2S_GetTxLevel(LPC_I2S0);

	if(txlevel <= LEVEL_TRANSMIT_FIFO)
	{
		for(i = 0; i < MAX_LEVEL_TRANSMIT_FIFO - txlevel; i++)
		{
			if(s_txOffset >= s_sizeAudio)//reach the end of buffer
			{
				static int counter = 0;
				while(Chip_I2S_GetTxLevel(LPC_I2S0) != 0)
				{
					counter++;
					if(counter > WAIT_BUFFER_FIFO)
					{
						break;
					}
				}
				counter = 0;
				s_txOffset = 0;
				s_bufferOffset = 0;

				NVIC_DisableIRQ(I2S0_IRQn);
				Chip_I2S_TxStop(LPC_I2S0);

				switch((uint8_t)s_idAudio)
				{
				case eZeroTwoFiveAudio:
				case eZeroFiveAudio:
				case eZeroSevenFiveAudio:
				case eOneAudio:
				case eOneTwoFiveAudio:
				case eOneFiveAudio:
				case eOneSevenFiveAudio:
				case eTwoAudio:
				case eTwoFiveAudio:
				case eThreeAudio:
					s_isPendingAudio = true;
					s_isEndFRAudio = true;
					break;
				case eStartAudio:
				case eBuzzer100msAudio:
				case eTestAudio:
				case eBuzzer3sAudio:
					s_isPendingAudio = true;
					break;
				case eFinishAudio:
					s_isPendingAudio = true;
					s_isEndStopAudio = true;
					break;
				case eAnyPalAudio:
					s_isPendingAudio = true;
					s_isEndAnypalAudio = true;
					break;
				case eChildLockAudio:
					s_counterChildlock++;
					if(s_counterChildlock >= (2*COUNTER_CHILD_LOCK))
					{
						s_isPendingAudio = true;
						s_counterChildlock = 2*COUNTER_CHILD_LOCK;
					}
					break;
				default:
					break;
				}

			}
			else
			{
				Chip_I2S_Send(LPC_I2S0, *(uint32_t *)(s_txBuffer + s_bufferOffset));

				s_bufferOffset += LEVEL_TRANSMIT_FIFO;
				s_txOffset += LEVEL_TRANSMIT_FIFO;
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_WriteRegI2C	                                            		*/
/*                                                                                      */
/* Details  -Write data to I2C register													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t reg : address register										*/
/* 			 	   uint16_t val : buffer value register									*/
/* 			 	   uint8_t dataSize : size buffer value register						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_WriteRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize)
{
	uint8_t dat[BUFFER_SIZE_I2C];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;

	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = I2CDEV_TLV320_ADDR;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = dat;
	i2cData.txSz = dataSize + 1;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;

	if(I2CInterfaceTransfer(LPC_I2C0, &i2cData) == ERROR)
	{
		DEBUGOUT("\nSend to I2C Error******************");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_ReadRegI2C	                                            		*/
/*                                                                                      */
/* Details  -Read data from I2C															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t reg : address register										*/
/* 			 	   uint16_t val : buffer value register									*/
/* 			 	   uint8_t dataSize : size buffer value register						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_ReadRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize)
{
	uint8_t dat[BUFFER_SIZE_I2C];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;

	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = I2CDEV_TLV320_ADDR;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = dat;
	i2cData.rxSz = dataSize + 1;

	if(I2CInterfaceTransfer(LPC_I2C0, &i2cData) == ERROR)
	{
		DEBUGOUT("\nSend to I2C Error******************");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_SetVolumeVoice			                                        */
/*                                                                                      */
/* Details  -Set volume	audio														    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t value : - volume					 						    */
/*				   -range : 0-10														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_SetVolumeVoice(uint8_t value)
{
	switch(value)
	{
	case 0 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x7F00, 1);//mute
		break;
	case VOLUME_1 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xBC00, 1);//-30.1(dB)
		break;
	case VOLUME_2 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xB600, 1);//-27.1(dB)
		break;
	case VOLUME_3:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xB000, 1);//-24.1(dB)
		break;
	case VOLUME_4 :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xA800, 1);//-20.1(dB)
		break;
	case VOLUME_5:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0xA000, 1);//-16(dB)
		break;
	case VOLUME_6:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x9A00, 1);//-13(dB)
		break;
	case VOLUME_7:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x9400, 1);//-10(dB)
		break;
	case VOLUME_8:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x8E00, 1);//-7(dB)
		break;
	case VOLUME_9:
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x8600, 1);//-3(dB)
		break;
	case MAX_VOLUME :
		audioMgr_WriteRegI2C(0x00,0x0100, 1);
		audioMgr_WriteRegI2C(0x26,0x8000, 1);//0(dB)
		break; //Volume 10(MAX)
	default :
		break;
	}

	audioMgr_WriteRegI2C(0x00,0x0100, 1);
	audioMgr_WriteRegI2C(0x20,0x8600, 1);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitRegI2C			                                            */
/*                                                                                      */
/* Details  -Write to I2C 															    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												 					*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
int audioMgr_InitRegI2C(void)
{
	int ret = 0;

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

	ret = 1;
	return ret;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_ResetAudio		                                            	*/
/*                                                                                      */
/* Details  	Reset audio								         					    */
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
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_ResetAudio(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,AUDIO_RESET_PORT, AUDIO_RESET_BIT);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,AUDIO_RESET_PORT, AUDIO_RESET_BIT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_InitAudio		                                            		*/
/*                                                                                      */
/* Details  -Initialize audio device with 												*/
/*           config Sample rate															*/
/*           config channel number														*/
/*           config word width											                */
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
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_InitAudio(void)
{
	int initREG =  audioMgr_InitRegI2C();
	if(initREG != 1)
	{
		DEBUGOUT("Error Init I2C");
		return;
	}

	I2S_AUDIO_FORMAT_T audio_Confg;
	audio_Confg.SampleRate = 44100;
	audio_Confg.ChannelNumber = 1;
	audio_Confg.WordWidth = 16;

	Chip_I2S_Init(LPC_I2S0);
	if(Chip_I2S_TxConfig(LPC_I2S0, &audio_Confg) != SUCCESS)
	{
		DEBUGOUT("Error config I2S");
		return;
	}

	Chip_I2S_TxStop(LPC_I2S0);

	memset(s_txBuffer,0,DATA_SIZE_VOICE_ALERT_CONTACT);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_PlayAudio		                                            		*/
/*                                                                                      */
/* Details  -Play sound by assigned buffer  											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ID_Audio id :  - id for audio					 					*/
/*				   -range : 0-41														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_idAudio : current audio                                    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayAudio(E_ID_Audio id)
{
	audioMgr_StopAudio();

	memset(s_txBuffer,0,DATA_SIZE_VOICE_ALERT_CONTACT);

	switch(id)
	{
	case eStartAudio:
		audioMgr_InitBuffer(audioStart, DATA_SIZE_VOICE_START);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eFinishAudio:
		audioMgr_InitBuffer(audioFinish, DATA_SIZE_VOICE_FINISH);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eTestAudio:
		audioMgr_InitBuffer(audioTestVice, DATA_SIZE_VOICE_TEST);
		audioMgr_SetVolumeVoice(MAX_VOLUME);
		break;
	case eZeroTwoFiveAudio:
		audioMgr_InitBuffer(audioFlow025, DATA_SIZE_VOICE_ZERO_TWO_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eZeroFiveAudio:
		audioMgr_InitBuffer(audioFlow050, DATA_SIZE_VOICE_ZERO_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eZeroSevenFiveAudio:
		audioMgr_InitBuffer(audioFlow075, DATA_SIZE_VOICE_ZERO_SEVEN_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eOneAudio:
		audioMgr_InitBuffer(audioFlow100, DATA_SIZE_VOICE_ONE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eOneTwoFiveAudio:
		audioMgr_InitBuffer(audioFlow125, DATA_SIZE_VOICE_ONE_TWO_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eOneFiveAudio:
		audioMgr_InitBuffer(audioFlow150, DATA_SIZE_VOICE_ONE_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eOneSevenFiveAudio:
		audioMgr_InitBuffer(audioFlow175, DATA_SIZE_VOICE_ONE_SEVEN_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eTwoAudio:
		audioMgr_InitBuffer(audioFlow200, DATA_SIZE_VOICE_TWO);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eTwoFiveAudio:
		audioMgr_InitBuffer(audioFlow250, DATA_SIZE_VOICE_TWO_FIVE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eThreeAudio:
		audioMgr_InitBuffer(audioFlow300, DATA_SIZE_VOICE_THREE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eBuzzer100msAudio:
		audioMgr_InitBuffer(audioBuzzer100ms, DATA_SIZE_VOICE_BUZZER_100MS);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eBuzzerVolumeSettingId));
		break;
	case eBuzzer3sAudio:
		audioMgr_InitBuffer(audioBuzzer3s, DATA_SIZE_VOICE_BUZZER_3S);
		audioMgr_SetVolumeVoice(MAX_VOLUME);
		break;
	case eBuzzer200msAudio:
		audioMgr_InitBuffer(audioBuzzer200ms, DATA_SIZE_VOICE_BUZZER_200MS);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eBuzzerVolumeSettingId));
		break;
	case eAlertContactAudio:
		audioMgr_InitBuffer(audioAlertContact, DATA_SIZE_VOICE_ALERT_CONTACT);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eAlertTubeAudio:
		audioMgr_InitBuffer(audioAlertTube, DATA_SIZE_VOICE_ALERT_TUBE);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel01Audio:
		audioMgr_InitBuffer(audioLevel01, DATA_SIZE_VOICE_LEVEL01);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel01AlarmAudio:
		audioMgr_InitBuffer(audioLevel01, DATA_SIZE_VOICE_LEVEL01);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel02Audio:
		audioMgr_InitBuffer(audioLevel02, DATA_SIZE_VOICE_LEVEL02);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel02AlarmAudio:
		audioMgr_InitBuffer(audioLevel02, DATA_SIZE_VOICE_LEVEL02);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel03Audio:
		audioMgr_InitBuffer(audioLevel03, DATA_SIZE_VOICE_LEVEL03);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel03AlarmAudio:
		audioMgr_InitBuffer(audioLevel03, DATA_SIZE_VOICE_LEVEL03);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel04Audio:
		audioMgr_InitBuffer(audioLevel04, DATA_SIZE_VOICE_LEVEL04);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel04AlarmAudio:
		audioMgr_InitBuffer(audioLevel04, DATA_SIZE_VOICE_LEVEL04);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel05Audio:
		audioMgr_InitBuffer(audioLevel05, DATA_SIZE_VOICE_LEVEL05);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel05AlarmAudio:
		audioMgr_InitBuffer(audioLevel05, DATA_SIZE_VOICE_LEVEL05);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel06Audio:
		audioMgr_InitBuffer(audioLevel06, DATA_SIZE_VOICE_LEVEL06);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel06AlarmAudio:
		audioMgr_InitBuffer(audioLevel06, DATA_SIZE_VOICE_LEVEL06);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel07Audio:
		audioMgr_InitBuffer(audioLevel07, DATA_SIZE_VOICE_LEVEL07);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel07AlarmAudio:
		audioMgr_InitBuffer(audioLevel07, DATA_SIZE_VOICE_LEVEL07);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel08Audio:
		audioMgr_InitBuffer(audioLevel08, DATA_SIZE_VOICE_LEVEL08);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel08AlarmAudio:
		audioMgr_InitBuffer(audioLevel08, DATA_SIZE_VOICE_LEVEL08);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel09Audio:
		audioMgr_InitBuffer(audioLevel09, DATA_SIZE_VOICE_LEVEL09);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel09AlarmAudio:
		audioMgr_InitBuffer(audioLevel09, DATA_SIZE_VOICE_LEVEL09);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eLevel10Audio:
		audioMgr_InitBuffer(audioLevel10, DATA_SIZE_VOICE_LEVEL10);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eGuidanceVolumeSettingId));
		break;
	case eLevel10AlarmAudio:
		audioMgr_InitBuffer(audioLevel10, DATA_SIZE_VOICE_LEVEL10);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eAlarmVolumeSettingId));
		break;
	case eChildLockAudio:
		audioMgr_InitBuffer(audioBuzzer100ms, DATA_SIZE_VOICE_BUZZER_100MS);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eBuzzerVolumeSettingId));
		break;
	case eAnyPalAudio:
		audioMgr_InitBuffer(audioBuzzer100ms, DATA_SIZE_VOICE_BUZZER_100MS);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eBuzzerVolumeSettingId));
		break;
	case eEndAlarmAudio:
		audioMgr_InitBuffer(audioBuzzer100ms, DATA_SIZE_VOICE_BUZZER_100MS);
		audioMgr_SetVolumeVoice(guiInterface_GetSettingValue(eBuzzerVolumeSettingId));
		break;
	default:
		break;
	}

	s_idAudio = id;
	Chip_I2S_Int_TxCmd(LPC_I2S0, ENABLE, LEVEL_TRANSMIT_FIFO);
	NVIC_ClearPendingIRQ(I2S0_IRQn);
	Chip_I2S_TxStart(LPC_I2S0);

	NVIC_SetPriority(I2S0_IRQn,I2S0_INT_PRIO);
	NVIC_EnableIRQ(I2S0_IRQn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_StopAudio	                                            			*/
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
/*                     (O) s_txOffset : transmit offset			                        */
/*                     (O) s_bufferOffset : buffer offset						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopAudio(void)
{
	s_txOffset = 0;
	s_bufferOffset = 0;

	audioMgr_WriteRegI2C(0x00,0x0100, 1);
	audioMgr_WriteRegI2C(0x20,0x0600, 1);

	NVIC_DisableIRQ(I2S0_IRQn);
	Chip_I2S_TxStop(LPC_I2S0);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_StopTimerAudio		                                            */
/*                                                                                      */
/* Details  -Stop timer audio , reset buffet						   					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_AlarmId alarmId :  - alarm id					 					*/
/*				   -range : 0-31														*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_alarmEndId : alarm is deactive                             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopTimerAudio(E_AlarmId alarmId)
{
	switch(alarmId)
	{
	case eLoweringO2ConcentrationId:
	case eExternalFlowPathpressureId:
	case eRisingCompressorOutletTemperatureId:
	case eCannulaMore27sId:
		s_alarmEndId = alarmId;
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_Request		                                            		*/
/*                                                                                      */
/* Details  -Request audio from GUI	task		  										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ID_Audio event : - event request					 				*/
/*				   -range : 0-41														*/
/*                 -unit : NONE                                                         */
/*																						*/
/*				   E_Alarm_Audio alarmAudio	: status alarm(high, medium)				*/
/*				   -range : 0-2															*/
/*                 -unit : NONE                                                         */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_playAnypalAfterStart : play buzzer after start audio       */
/*                     (O) s_ttick : tick counter						                */
/*                     (O) s_isEndAnypalAudio : flag end anypal audio					*/
/*                     (O) s_counterChildlock : counter child lock						*/
/*                     (O) s_playChildlockAfterAnypal : flag play childlock				*/
/*                     (O) s_isEndFRAudio : flag end flow rate audio					*/
/*                     (O) s_isEndStopAudio : flag end stop audio						*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_Request(E_ID_Audio event, E_Alarm_Audio alarmAudio)
{
	switch(alarmAudio)
	{
	case eNoAlarmAudio:
		if(event != eNoneAudio)
		{
			if(event == eAnyPalAudio)
			{
				switch(standbyScreen_GetStateMachine())
				{
				case eStandby:
					audioMgr_PlayAudio(event);
					s_isEndAnypalAudio = false;
					break;
				case eEndSequence:
					if(s_isEndStopAudio == true)
					{
						audioMgr_PlayAudio(event);
						s_isEndAnypalAudio = false;
					}
					else
					{
						s_playAnypalAfterAudioOther = true;
					}
					break;
				case eOperating:
					if((s_idAudio == eZeroTwoFiveAudio) || (s_idAudio == eZeroFiveAudio)
							|| (s_idAudio == eZeroSevenFiveAudio) || (s_idAudio == eOneAudio)
							|| (s_idAudio == eOneTwoFiveAudio) || (s_idAudio == eOneFiveAudio)
							|| (s_idAudio == eOneSevenFiveAudio) || (s_idAudio == eTwoAudio)
							|| (s_idAudio == eTwoFiveAudio) || (s_idAudio == eThreeAudio)
							|| (s_idAudio == eBuzzer100msAudio) || (s_idAudio == eStartAudio))
					{
						switch(alarmScreenMgr_GetCurrentAlarmId())
						{
						case eLoweringO2ConcentrationId:
						case eO2ConcentrationSensorId:
						case eRisingCompressorOutletTemperatureId:
						case eCannulaMore27sId:
							break;
						default:
							if(s_isEndFRAudio == true)
							{
								audioMgr_PlayAudio(event);
								s_isEndAnypalAudio = false;
							}
							else
							{
								s_playAnypalAfterAudioOther = true;
							}
							break;
						}
					}
					else if (s_idAudio == eChildLockAudio)
					{
						//Play anypal when end childlock
						switch(alarmScreenMgr_GetCurrentAlarmId())
						{
						case eLoweringO2ConcentrationId:
						case eO2ConcentrationSensorId:
						case eRisingCompressorOutletTemperatureId:
						case eCannulaMore27sId:
							break;
						default:
							if(s_counterChildlock >= (2*COUNTER_CHILD_LOCK))
							{
								audioMgr_PlayAudio(event);
								s_isEndAnypalAudio = false;
							}
							else
							{
								s_playAnypalAfterAudioOther = true;
							}
							break;
						}
					}
					else
					{
						switch(alarmScreenMgr_GetCurrentAlarmId())
						{
						case eLoweringO2ConcentrationId:
						case eO2ConcentrationSensorId:
						case eRisingCompressorOutletTemperatureId:
						case eCannulaMore27sId:
							break;
						default:
							audioMgr_PlayAudio(event);
							s_isEndAnypalAudio = false;
							break;
						}
					}
					break;
				case eStartSequence:
					s_playAnypalAfterAudioOther = true;
					break;
				default:
					break;
				}
			}
			else if(event == eChildLockAudio)//Play child lock
			{
				if(alarmScreenMgr_GetCurrentAlarmId() != eCannulaMore27sId)
				{
					//Handle play child lock when anypal buzzer playing
					if(s_idAudio == eAnyPalAudio)
					{
						if(s_isEndAnypalAudio == true)
						{
							if(s_counterChildlock >= (2*COUNTER_CHILD_LOCK))
							{
								s_counterChildlock = 0;
							}
							audioMgr_PlayAudio(event);
						}
						else
						{
							s_playChildlockAfterAnypal = true;

							//Reset flag child lock
							s_counterChildlock = 0;

							//Stop timer play child lock
							guihandler_StopTimerPlayChildLock();
							guihandler_StopTimerPlayChildLockAlarm();
						}
					}
					else
					{
						if(s_counterChildlock >= (2*COUNTER_CHILD_LOCK))
						{
							s_counterChildlock = 0;
						}
						audioMgr_PlayAudio(event);
					}
				}
				else
				{
					//Reset flag child lock
					s_counterChildlock = 0;
					s_playChildlockAfterAnypal = false;

					//Stop timer play child lock
					guihandler_StopTimerPlayChildLock();
					guihandler_StopTimerPlayChildLockAlarm();
				}
			}
			else
			{
				if((event == eZeroTwoFiveAudio) || (event == eZeroFiveAudio)
						|| (event == eZeroSevenFiveAudio) || (event == eOneAudio)
						|| (event == eOneTwoFiveAudio) || (event == eOneFiveAudio)
						|| (event == eOneSevenFiveAudio) || (event == eTwoAudio)
						|| (event == eTwoFiveAudio) || (event == eThreeAudio))
				{
					s_isEndFRAudio = false;

					//Reset flag child lock
					s_counterChildlock = 0;
					s_playChildlockAfterAnypal = false;

					//Stop timer play child lock
					guihandler_StopTimerPlayChildLock();
					guihandler_StopTimerPlayChildLockAlarm();

					if(alarmScreenMgr_GetCurrentAlarmId() == eCannulaMore27sId)
					{
						return;
					}
				}
				else if(event == eTestAudio)
				{
					if(settingOnOffButton_GetValue(eValueBuzzerId) == eOn)
					{
						settingOnOffButton_SetValue(eValueBuzzerId,(int8_t*)"OFF");
					}
				}
				else if(event == eBuzzer3sAudio)
				{
					if(settingOnOffButton_GetValue(eValueVoiceId) == eOn)
					{
						settingOnOffButton_SetValue(eValueVoiceId,(int8_t*)"OFF");
					}
				}

				audioMgr_PlayAudio(event);
			}
		}

		if(event == eFinishAudio)
		{
			//Reset tick count
			s_ttick = 0;

			s_alarmEndId = eLastAlarmId;

			//Reset flag stop audio
			s_isEndStopAudio = false;

			//Rest flag flow rate audio
			s_isEndFRAudio = false;

			//Reset flag child lock
			s_counterChildlock = 0;
			s_playChildlockAfterAnypal = false;

			//Stop timer play child lock
			guihandler_StopTimerPlayChildLock();
			guihandler_StopTimerPlayChildLockAlarm();

			//Stop timer play alarm
			guihandler_StopTimerPlayHighAlarm();
			guihandler_StopTimerPlayMediumAlarm();
		}
		break;
	case ehighAlarmAudio:
	{
		s_ttick = 0;
		s_alarmEndId = eLastAlarmId;

		//Reset flag child lock
		s_counterChildlock = 0;
		s_playChildlockAfterAnypal = false;

		//Stop timer play child lock
		guihandler_StopTimerPlayChildLock();
		guihandler_StopTimerPlayChildLockAlarm();

		guihandler_StopTimerPlayMediumAlarm();
		guihandler_StartTimerPlayHighAlarm();
		break;
	}
	case eMediumAlarmAudio:
	{
		s_ttick = 0;

		s_alarmEndId = eLastAlarmId;

		//Reset flag child lock
		s_counterChildlock = 0;
		s_playChildlockAfterAnypal = false;

		//Stop timer play child lock
		guihandler_StopTimerPlayChildLock();
		guihandler_StopTimerPlayChildLockAlarm();

		guihandler_StartTimerPlayMediumAlarm();
		break;
	}
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_I2CRead						                           		    */
/*                                                                                      */
/* Details  -Read data from I2C															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint32_t addr : address register						 				*/
/*                 uint8_t* buf  : buffer read                                          */
/*                 uint32_t len  : length buffer                                        */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Status		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static Status audioMgr_I2CRead(uint32_t addr, uint8_t* buf, uint32_t len)
{
	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = addr;
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = NULL;
	i2cData.txSz = 0;
	i2cData.rxBuff = buf;
	i2cData.rxSz = len;

	if (I2CInterfaceTransfer(LPC_I2C0, &i2cData) == ERROR) {
		DEBUGOUT("audioMgr_I2CRead Err: \n");
		return ERROR;
	}
	return SUCCESS;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_CheckAlarmSpeaker	                                            	*/
/*                                                                                      */
/* Details  -Check alarm speaker								 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_CheckAlarmSpeaker(void)
{
	if(modeMgr_GetCurrentMode() == ePatientMode)
	{
		audioMgr_WriteRegI2C(0x00,0x0000,1);

		uint16_t ret = 0;
		uint8_t buf[BUFFER_SIZE_CHECK_SPEAKER];
		buf[0] = REGISTER_CHECK_ALARM_SPEAKER;

		//Write to I2C
		I2CM_XFER_T i2cData;

		i2cData.slaveAddr = I2CDEV_TLV320_ADDR;
		i2cData.options = 0;
		i2cData.status = 0;
		i2cData.txBuff = buf;
		i2cData.txSz = 1;
		i2cData.rxBuff = NULL;
		i2cData.rxSz = 0;

		I2CInterfaceTransfer(LPC_I2C0, &i2cData);

		//Read from I2C
		if(audioMgr_I2CRead(I2CDEV_TLV320_ADDR, buf, 1) == SUCCESS)
		{
			ret = buf[0];
			if(ret == SPEAKER_NOT_DETECT)
			{
				DEBUGOUT("\n is alarm speaker");
				return true;
			}
		}
		else
		{
			DEBUGOUT("Audio I2C bus err \n");
			//Err on I2C bus
			return true;
		}
	}

	return false;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_PlayFlowRate	                                            		*/
/*                                                                                      */
/* Details  -Play current flow rate									 					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowRate : - flow rate										*/
/*				   -range : 5-60														*/
/*                 -unit : L/M                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayFlowRate(uint8_t flowRate)
{
	E_ID_Audio event = eNoneAudio;

	switch(flowRate)
	{
	case SETTING_FR_300L:
		event = eThreeAudio;
		break;
	case SETTING_FR_250L:
		event = eTwoFiveAudio;
		break;
	case SETTING_FR_200L:
		event = eTwoAudio;
		break;
	case SETTING_FR_175L:
		event = eOneSevenFiveAudio;
		break;
	case SETTING_FR_150L:
		event = eOneFiveAudio;
		break;
	case SETTING_FR_125L:
		event = eOneTwoFiveAudio;
		break;
	case SETTING_FR_100L:
		event = eOneAudio;
		break;
	case SETTING_FR_075L:
		event = eZeroSevenFiveAudio;
		break;
	case SETTING_FR_050L:
		event = eZeroFiveAudio;
		break;
	case SETTING_FR_025L:
		event = eZeroTwoFiveAudio;
		break;
	default:
		break;
	}

	audioMgr_Request(event, eNoAlarmAudio);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_StopHighAlarm	                                            		*/
/*                                                                                      */
/* Details  -Stop high alarm								 							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_ttick : tick counter                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopHighAlarm(void)
{
	audioMgr_StopAudio();
	s_ttick = 0;
	guihandler_StopTimerPlayHighAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_StopMediumAlarm	                                            	*/
/*                                                                                      */
/* Details  -Stop medium alarm									 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_ttick : tick counter                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopMediumAlarm(void)
{
	audioMgr_StopAudio();
	s_ttick = 0;
	guihandler_StopTimerPlayMediumAlarm();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_PlayHighAlarm	                                            		*/
/*                                                                                      */
/* Details  -Play high alarm								 							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_ttick : tick counter                                       */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayHighAlarm(void)
{
	E_ID_Audio event = eNoneAudio;
	s_ttick++;

	/********************High*****************************/
	if(s_ttick == HIGH_ALARM_BUZZER_ON_1MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == HIGH_ALARM_BUZZER_ON_250MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == HIGH_ALARM_BUZZER_ON_500MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == HIGH_ALARM_BUZZER_ON_1000MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_1250MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_2000MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_2250MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_2500MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_3000MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_3250MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_4000MS)
	{
		event = eAlertContactAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick > HIGH_ALARM_BUZZER_ON_4000MS && s_ttick < HIGH_ALARM_BUZZER_ON_10000MS)
	{
		if(s_alarmEndId != eLastAlarmId)
		{
			guihandler_StopTimerPlayHighAlarm();
			audioMgr_StopAudio();
			s_alarmEndId = eLastAlarmId;
			s_ttick = 0;
		}
	}
	else if (s_ttick == HIGH_ALARM_BUZZER_ON_10000MS)
	{
		s_ttick = 0;
	}

	if(alarmScreenMgr_GetCurrentAlarmId() == eCannulaUnder27sId ||
			alarmScreenMgr_GetCurrentAlarmId() == eCannulaMore27sId)
	{
		guihandler_StopTimerPlayHighAlarm();
		audioMgr_StopAudio();
		s_ttick = 0;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_PlayMediumAlarm	                                            	*/
/*                                                                                      */
/* Details  -Play medium alarm								 							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_ttick : tick counter                                       */
/*                     (O) s_alarmEndId : alarm auto reset						        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayMediumAlarm(void)
{
	E_ID_Audio event = eNoneAudio;
	s_ttick++;
	/********************Medium*****************************/
	if(s_ttick == MEDIUM_ALARM_BUZZER_ON_1MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == MEDIUM_ALARM_BUZZER_ON_400MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == MEDIUM_ALARM_BUZZER_ON_800MS)
	{
		event = eBuzzer200msAudio;
		audioMgr_PlayAudio(event);
	}
	else if(s_ttick == MEDIUM_ALARM_BUZZER_ON_2000MS)
	{
		event = eAlertTubeAudio;
		audioMgr_PlayAudio(event);
	}
	else if (s_ttick > MEDIUM_ALARM_BUZZER_ON_2000MS && s_ttick < MEDIUM_ALARM_BUZZER_ON_25000MS)
	{
		if(s_alarmEndId != eLastAlarmId)
		{
			s_isPendingAudio = true;
			guihandler_StopTimerPlayMediumAlarm();
			audioMgr_StopAudio();
			s_alarmEndId = eLastAlarmId;
			s_ttick = 0;
		}
	}
	else if (s_ttick == MEDIUM_ALARM_BUZZER_ON_25000MS)
	{
		s_ttick = 0;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-audioMgr_CheckAndPlayAudio	                                            	*/
/*                                                                                      */
/* Details  -Check and play audio								 						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE													 				*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_idAudio : current id audio							        */
/*					   (I) s_alarmEndId : alarm id										*/
/*					   (I) s_isEndFRAudio : flag end flow rate audio					*/
/*                     (O) s_isPendingAudio : flag play audio                           */
/*                     (O) s_playAnypalAfterAudioOther : flag play anypal				*/
/*                     (O) s_playChildlockAfterAnypal : flag play child lock			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_CheckAndPlayAudio(void)
{
	if(s_isPendingAudio == true)
	{
		switch((uint8_t)s_idAudio)
		{
		case eBuzzer100msAudio:
			if(guiTask_GetFlagServiceScreen() == false)
			{
				audioMgr_Request(eStartAudio, eNoAlarmAudio);
			}
			break;
		case eStartAudio:
			audioMgr_PlayFlowRate(guiInterface_GetSettingValue(eFlowRateSettingId));
			break;
		case eZeroTwoFiveAudio:
		case eZeroFiveAudio:
		case eZeroSevenFiveAudio:
		case eOneAudio:
		case eOneTwoFiveAudio:
		case eOneFiveAudio:
		case eOneSevenFiveAudio:
		case eTwoAudio:
		case eTwoFiveAudio:
		case eThreeAudio:
		case eFinishAudio:
		case eChildLockAudio:
			//Play buzzer anypal after other auido
			if(s_playAnypalAfterAudioOther == true)
			{
				if(anypalDisplayHandler_GetAnypalConnectStatus() == true)
				{
					DEBUGOUT("\n s_playAnypalAfterAudioOther");
					audioMgr_Request(eAnyPalAudio, eNoAlarmAudio);
				}
				s_playAnypalAfterAudioOther = false;
			}
			break;
		case eTestAudio:
			settingOnOffButton_SetValue(eValueVoiceId,(int8_t*)"OFF");
			break;
		case eBuzzer3sAudio:
			settingOnOffButton_SetValue(eValueBuzzerId,(int8_t*)"OFF");
			break;
		case eAnyPalAudio:
			if(s_playChildlockAfterAnypal == true)
			{
				if(WM_IsVisible(g_operationScreen) == true)
				{
					guihandler_StartTimerPlayChildLock();
				}
				else if(WM_IsVisible(g_lowAlarmScreen) == true)
				{
					guihandler_StartTimerPlayChildLockAlarm();
				}
				s_playChildlockAfterAnypal = false;
			}
			break;
		case eBuzzer200msAudio:
		case eAlertTubeAudio:
			if(s_alarmEndId == eLastAlarmId && s_isEndFRAudio == false)
			{
				audioMgr_PlayFlowRate(guiInterface_GetSettingValue(eFlowRateSettingId));
			}
			else
			{
				DEBUGOUT("\n ***********************");
			}
			break;
		default:
			break;
		}

		s_isPendingAudio = false;
	}

	return;
}

/* end of files */


