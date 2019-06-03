/********************************************************************/
/*                                                                  */
/* File Name    : AudioMgr.c                                   	    */
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
#include "board.h"
#include "AudioMgr.h"
#include "string.h"
#include "IRQHandle.h"
#define SPEAKER_NOT_DETECT 128

#define DATA_SIZE_VOICE_ALERT_RESTART 254776

#define DATA_SIZE_VOICE_START 248828

#define DATA_SIZE_VOICE_BUZZER_200MS 17730
#define BUFFER_SIZE			4*1048//2*1024
#define BUFFER_FULL 0
#define BUFFER_EMPTY 1
#define BUFFER_AVAILABLE 2
extern unsigned char audioVoiceAlertRestartBoot[];
extern unsigned char audioVoiceStartBoot[];
extern unsigned char audioSquareWave200msBoot[];
extern bool g_isServiceScreen;
uint8_t tx_buffer[BUFFER_SIZE];
uint32_t data_offset, buffer_offset,remain_data;
uint32_t tx_offset = 0;
Bool Tx_EndofBuf_F = FALSE;
static size_t sizeAudio = 0;
static E_ID_Audio idAudio = eNoneAudio;
static bool gs_isPlayPowerLostSound = false;
/****************************************************************************************/
/*                                                                                      */
/* General 	-Memory copy					                                            */
/*                                                                                      */
/* Details  -Memory copy from SPI flash to ram 								            */
/*				 																		*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audio_memcpy(void *des, void *source, size_t size)
{
	__disable_irq();
	memcpy(des, source, size);
	__enable_irq();
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize transmit buffer		                                            */
/*                                                                                      */
/* Details  -Initialize transmit buffer, reset buffer offset, data offset and           */
/*				 remaining data															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/****************************************************************************************/
void audioMgr_InitBuffer(void *audio, size_t size)
{
	sizeAudio = size;
	audio_memcpy(tx_buffer,audio, BUFFER_SIZE);
	buffer_offset = 0;
	data_offset = BUFFER_SIZE;
	remain_data = size - BUFFER_SIZE;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioMgr_PlayPowerLostAlarmAudio 			 						*/
/*                                                                                      */
/* Details:    Play alarm sound															*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/* ReturnValue : None                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_PlayPowerLostAlarmAudio(void)
{
	gs_isPlayPowerLostSound = true;
	IRQ_timer2Enalble();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioMgr_StopPowerLostAlarmAudio 		 							*/
/*                                                                                      */
/* Details:    stop play alarm sound													*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopPowerLostAlarmAudio(void)
{
	gs_isPlayPowerLostSound = false;
	IRQ_timer2Disable();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioMgr_GetStatusPowerLostAlarmAudio 		 						*/
/*                                                                                      */
/* Details:    return status play or no play Alarm Sound								*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/
bool audioMgr_GetStatusPowerLostAlarmAudio()
{
	return gs_isPlayPowerLostSound;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-I2S callback function		                                            	*/
/*                                                                                      */
/* Details  -I2S callback function, will be call when I2S send a half of buffer complete*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_CallbackI2S(void *audio)
{
	if(remain_data >=BUFFER_SIZE/2)
	{
		if(buffer_offset == BUFFER_SIZE)
		{
			// copy audio data into remain half of tx_buffer
			audio_memcpy(tx_buffer + BUFFER_SIZE/2, audio+data_offset, BUFFER_SIZE/2);
			buffer_offset = 0;
		}
		else
			// copy audio data into remain half of tx_buffer
			audio_memcpy(tx_buffer, audio+data_offset, BUFFER_SIZE/2);
		data_offset += BUFFER_SIZE/2;
		remain_data -= BUFFER_SIZE/2;
	}
	else // reach the last copy
	{
		if(buffer_offset == BUFFER_SIZE)
		{
			// copy audio data into remain half of tx_buffer
			audio_memcpy(tx_buffer + BUFFER_SIZE/2, audio+data_offset, remain_data);
			buffer_offset = 0;
		}
		else
			// copy audio data into remain half of tx_buffer
			audio_memcpy(tx_buffer, audio+data_offset, remain_data);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-I2S0 interrupt		                                            			*/
/*                                                                                      */
/* Details  -I2S0 interrupt handler sub-routine											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void I2S0_IRQHandler(void)
{
	uint32_t txlevel,i;
	txlevel = Chip_I2S_GetTxLevel(LPC_I2S0);

	if(txlevel <= 4)
	{
		for(i=0;i<8-txlevel;i++)
		{
			Chip_I2S_Send(LPC_I2S0, *(uint32_t *)(tx_buffer + buffer_offset));
			tx_offset +=4;
			buffer_offset +=4;
			//call audioMgr_CallbackI2S() function when reach each half of tx_buffer
			if((buffer_offset == BUFFER_SIZE/2)||(buffer_offset == BUFFER_SIZE))
			{
				switch(idAudio)
				{
				case eStartAudio:
					audioMgr_CallbackI2S(audioVoiceStartBoot);
					break;
				case eBuzzer200msAudio:
					audioMgr_CallbackI2S(audioSquareWave200msBoot);
					break;
				case eAlertRestartAudio:
					audioMgr_CallbackI2S(audioVoiceAlertRestartBoot);
					break;
				default:
					break;
				}
			}

			if(tx_offset >= sizeAudio)//reach the end of buffer
			{
				tx_offset = 0;
				buffer_offset = 0;
				data_offset = BUFFER_SIZE;
				remain_data = sizeAudio - BUFFER_SIZE;

				NVIC_DisableIRQ(I2S0_IRQn);
				Chip_I2S_TxStop(LPC_I2S0);
			}
		}
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Write data	to I2C	                                            		    */
/*                                                                                      */
/* Details  -Write data to I2C register													*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t reg : address register										*/
/* 			 	   uint16_t val : buffer value register									*/
/* 			 	   uint8_t dataSize : size buffer value register						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_WriteRegI2C(uint8_t reg, uint16_t val, uint8_t dataSize)
{
	uint8_t dat[3];
	dat[0] = reg; dat[1] = val >> 8; dat[2] = val & 0xFF;

	I2CM_XFER_T i2cData;

	i2cData.slaveAddr = (0x30 >> 1);
	i2cData.options = 0;
	i2cData.status = 0;
	i2cData.txBuff = dat;//rx_data;
	i2cData.txSz = dataSize + 1;//2;
	i2cData.rxBuff = NULL;
	i2cData.rxSz = 0;

	if(Chip_I2CM_XferBlocking(LPC_I2C1, &i2cData) == 0)
	{
		DEBUGOUT("\nSend to I2C Error******************");
	}
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Set volume			                                            		    */
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
			audioMgr_WriteRegI2C(0x26,0xA500, 1);// Analog Volume to SPK, set = -18.6dB

			break;
		case 9:
			audioMgr_WriteRegI2C(0x00,0x0100, 1);
			audioMgr_WriteRegI2C(0x26,0xA200, 1);// Analog Volume to SPK, set = -17dB

			break;
		case 10 :
			audioMgr_WriteRegI2C(0x00,0x0100, 1);
			audioMgr_WriteRegI2C(0x26,0x9400, 1);// Analog Volume to SPK, set = -10dB

			break; //Volume 10(MAX)
		default :
			return;
		}
		audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
		audioMgr_WriteRegI2C(0x20,0x8600, 1);//Add 03-10-2016
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Write to I2C			                                            		*/
/*                                                                                      */
/* Details  -Write to I2C 															    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE												 					*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : int		                                                            */
/*                                                                                      */
/****************************************************************************************/
int audioMgr_InitRegI2C()
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

	audioMgr_WriteRegI2C(0x00,0x0000, 1);// Set register page to 0
	audioMgr_WriteRegI2C(0x3F,0xD400, 1);// Power-up DAC left and right channels (soft step enabled)
	audioMgr_WriteRegI2C(0x41,0x0000, 1);// DAC Left gain 0xD4 = -22 dB, 0x02 = 1dB, 0x0A = 5dB
	audioMgr_WriteRegI2C(0x42,0x0000, 1);// DAC Right gain 0xD4 = -22 dB, 0x02 = 1dB, 0x0A = 5dB
	audioMgr_WriteRegI2C(0x40,0x0000, 1);// Unmute DAC left and right channels

	ret = 1;
	return ret;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize audio device		                                            */
/*                                                                                      */
/* Details  -Initialize audio device with 												*/
/*           config Sample rate															*/
/*           config channel number														*/
/*           config word width											                */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_InitAudio()
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,1, 13);
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,1, 13);
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
	IRQ_timer2Init();
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Play audio		                                            				*/
/*                                                                                      */
/* Details  -Play sound by assigned buffer  											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) E_ID_Audio id		 					  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void audioMgr_PlayAudio(E_ID_Audio id)
{
	DEBUGOUT("Test ID Audio %d\n",id);
	audioMgr_StopAudio();
	switch(id)
	{
	case eStartAudio:
		audioMgr_InitBuffer(audioVoiceStartBoot, DATA_SIZE_VOICE_START);
		break;
	case eBuzzer200msAudio:
		audioMgr_SetVolumeVoice(7);
		audioMgr_InitBuffer(audioSquareWave200msBoot, DATA_SIZE_VOICE_BUZZER_200MS);
		break;
	case eAlertRestartAudio:
		audioMgr_SetVolumeVoice(10);
		audioMgr_InitBuffer(audioVoiceAlertRestartBoot, DATA_SIZE_VOICE_ALERT_RESTART);
		break;
	default:
		break;
	}

	audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
	audioMgr_WriteRegI2C(0x20,0x8600, 1);//Add 03-10-2016

	idAudio = id;

	Chip_I2S_TxStop(LPC_I2S0);//Add 03-10-2016
	Chip_I2S_DisableMute(LPC_I2S0);//Add 03-10-2016
	Chip_I2S_TxStart(LPC_I2S0);

	Chip_I2S_Int_TxCmd(LPC_I2S0, ENABLE, 4);
	NVIC_EnableIRQ(I2S0_IRQn);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Stop audio		                                            				*/
/*                                                                                      */
/* Details  -Stop audio , reset buffet and clear interrupt   							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*                                                                                      */
/****************************************************************************************/
void audioMgr_StopAudio()
{
	tx_offset = 0;
	buffer_offset = 0;
	data_offset = BUFFER_SIZE;
	remain_data = sizeAudio - BUFFER_SIZE;

	audioMgr_WriteRegI2C(0x00,0x0100, 1);//Add 03-10-2016
	audioMgr_WriteRegI2C(0x20,0x0600, 1);//Add 03-10-2016

	NVIC_DisableIRQ(I2S0_IRQn);
	NVIC_ClearPendingIRQ(I2S0_IRQn);//Add 03-10-2016
	Chip_I2S_EnableMute(LPC_I2S0);//Add 03-10-2016
	Chip_I2S_TxStop(LPC_I2S0);
}
