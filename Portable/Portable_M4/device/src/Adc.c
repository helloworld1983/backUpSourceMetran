/********************************************************************/
/*                                                                  */
/* File Name     : Adc.c			                                */
/*                                                                  */
/* General       : This module contain function for init and 		*/
/* 					read adc										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/28      Thao Ha(MV)			new file  	    */
/*																	*/
/*                                                                  */
/********************************************************************/
#include <Adc.h>

#define ADC_10BIT_MASK 1023
#define ADC_MAX_VOLTAGE 3.3
#define ADC_BIT_RATE	400000
static ADC_CLOCK_SETUP_T ADCSetup;

/****************************************************************************************/
/*                                                                                      */
/* General：  adc_Init																		*/
/*                                                                                      */
/* Details：  This function init and config adc for FH310									*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*                                                                                      */
/* ReturnValue : None																	*/
/*                                                                                      */
/****************************************************************************************/
void adc_Init(void){
	uint32_t bitRate = ADC_BIT_RATE;
	ADC_RESOLUTION_T bitAccuracy = ADC_10BITS;
	Board_ADC_Init();
	/*ADC Init */
	Chip_ADC_Init(PORTABLE_ADC_ID, &ADCSetup);
	/* Disable Burst mode */
	Chip_ADC_SetBurstCmd(PORTABLE_ADC_ID, DISABLE);
	/*Set sample rate */
	Chip_ADC_SetSampleRate(PORTABLE_ADC_ID, &ADCSetup, bitRate);
	/* Set resolution */
	Chip_ADC_SetResolution(PORTABLE_ADC_ID, &ADCSetup, bitAccuracy);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  adc_Read																		*/
/*                                                                                      */
/* Details：  Read adc value of channel pass in argument   								*/
/*                                                                                      */
/* Arguments : (I) uint8_t channel					 					  				*/
/*                                                                                      */
/* ReturnValue : float																	*/
/*                    					                                            	*/
/****************************************************************************************/
float adc_Read(uint8_t channel) {
	float voltVal;
	uint16_t dataADC;
	adc_Init();
	Chip_ADC_EnableChannel(PORTABLE_ADC_ID, (ADC_CHANNEL_T)channel, ENABLE);
	/* Start A/D conversion if not using burst mode */
	Chip_ADC_SetStartMode(PORTABLE_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	/* Waiting for A/D conversion complete */
	while (Chip_ADC_ReadStatus(PORTABLE_ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}
	/* Read ADC value */
	Chip_ADC_ReadValue(PORTABLE_ADC_ID, channel, &dataADC);
	// convert binary val to real voltage val
	voltVal = dataADC*ADC_MAX_VOLTAGE/ADC_10BIT_MASK;
	return voltVal;
}


