/********************************************************************/
/*                                                                  */
/* File Name    : EncoderInterface.h                                */
/*                                                                  */
/* General       :This file contains function for interface with    */
/*                encoder module			                        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/02     Quyen Ngo(MV)  	    new file        */
/*                                                                  */
/* #0001	   2017/11/02	  Quyen Ngo(MV)			modify			*/
/*		Chang default s_isEnableEncoder to false for avoid the case */
/*		Encoder send ipc msg in standby mode after wake up			*/
/* #0002	   2017/11/17	  Quyen Ngo(MV)			modify			*/
/*		Remove s_EncodeMutex variable not use for fix Coverity issue*/
/********************************************************************/

#include "EncoderInterface.h"
#include "encoder.h"

#define ENCODER_SEMAPHORE_WAITING	1/portTICK_PERIOD_MS

static bool s_isEnableEncoder = false;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoderInterface_DisableEncoder		                                */
/*                                                                                      */
/* Details		: 	Disable encoder												     	*/
/*                                                                                      */
/* Arguments 	: 	(I)                                  								*/
/*                                                                                      */
/* Using Global Data : (O) s_isEnableEncoder			                              	*/
/*                                                                                      */
/****************************************************************************************/
void encoderInterface_SetEnable(bool isEnable)
{
	s_isEnableEncoder = isEnable;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoderInterface_DisableEncoder		                                */
/*                                                                                      */
/* Details		: 	Get encoder is enable state									     	*/
/*                                                                                      */
/* Arguments 	: 		                                  								*/
/*                                                                                      */
/* ReturnValue  : bool	s_isEnableEncoder		                                        */
/*				  range true/false: Encoder enable/disable								*/
/* Using Global Data : (I) s_isEnableEncoder		                              		*/
/*                                                                                      */
/****************************************************************************************/
bool encoderInterface_GetIsEnable(void)
{
	return s_isEnableEncoder;
}
