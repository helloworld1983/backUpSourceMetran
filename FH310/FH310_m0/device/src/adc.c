/********************************************************************/
/*                                                                  */
/* File Name     : Adc.c			                                */
/*                                                                  */
/* General       : This module contain function for init and 		*/
/* 					read adc										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/28      Quyen Ngo  			new file  	    */
/*																	*/
/* #0001	   2017/11/02	   Quyen Ngo			modify			*/
/*      Change adc_Read function to return ADC_VAL_t (Bug 2012)		*/
/*                                                                  */
/********************************************************************/
#include <adc.h>

#define ADC_10BIT_MASK 1023
#define ADC_MAX_VOLTAGE 3.3
#define ADC_BIT_RATE	400000//500000

/****************************************************************************************/
/*                                                                                      */
/* General：  adc_Init																		*/
/*                                                                                      */
/* Details：  This function init and config adc for FH310									*/
/*                                                                                      */
/* Arguments : None								 					  					*/
/*																						*/
/* Macros:		ADC_BIT_RATE															*/
/*				FH310_ADC_ID															*/
/*                                                                                      */
/* ReturnValue : void																	*/
/*                                                                                      */
/* Using Global Data : 									                            	*/
/*                    					                                            	*/
/*                     								                                 	*/
/* local valiables   :																	*/
/* 					 				                                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void adc_Init(void){
	ADC_CLOCK_SETUP_T ADCSetup;
	uint32_t bitRate = ADC_BIT_RATE;
	ADC_RESOLUTION_T bitAccuracy = ADC_10BITS;
	Board_ADC_Init();
	/*ADC Init */
	Chip_ADC_Init(FH310_ADC_ID, &ADCSetup);
	/* Disable Burst mode */
	Chip_ADC_SetBurstCmd(FH310_ADC_ID, DISABLE);
	/*Set sample rate */
	Chip_ADC_SetSampleRate(FH310_ADC_ID, &ADCSetup, bitRate);
	/* Set resolution */
	Chip_ADC_SetResolution(FH310_ADC_ID, &ADCSetup, bitAccuracy);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General：  adc_Read																		*/
/*                                                                                      */
/* Details：  Read adc value of channel pass in argument   								*/
/*                                                                                      */
/* Arguments : ADC_CHANNEL_T channel					 					  			*/
/*                                                                                      */
/* Macros:	   FH310_ADC_ID																*/
/*			   ADC_MAX_VOLTAGE															*/
/*			   ADC_10BIT_MASK															*/
/*																						*/
/* ReturnValue : 	ADC_VAL_T adcVal													*/
/*																						*/
/*																						*/
/*                                                                                      */
/* Using Global Data : 									                             	*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											 	    */
/*                                                                                      */
/****************************************************************************************/
ADC_VAL_t adc_Read(ADC_CHANNEL_T channel) {
	ADC_VAL_t stAdcVal;
	float voltVal;
	uint16_t dataADC;
	adc_Init();
	Chip_ADC_EnableChannel(FH310_ADC_ID, channel, ENABLE);
	/* Start A/D conversion if not using burst mode */
	Chip_ADC_SetStartMode(FH310_ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
	/* Waiting for A/D conversion complete */
	while (Chip_ADC_ReadStatus(FH310_ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}
	/* Read ADC value */
	Chip_ADC_ReadValue(FH310_ADC_ID, channel, &dataADC);
	// convert binary val to real voltage val
	voltVal = dataADC*ADC_MAX_VOLTAGE/ADC_10BIT_MASK;
	stAdcVal.adVal = dataADC;
	stAdcVal.voltVal = voltVal;
	return stAdcVal;
}


