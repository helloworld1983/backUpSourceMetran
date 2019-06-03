#ifndef INC_ALARMLED_H_
	#define INC_ALARMLED_H_

/********************************************************************/
/*                                                                  */
/* File Name     : AlarmLed.h			                            */
/*                                                                  */
/* General       : This module contain function for init and 		*/
/* 					control alarm led								*/
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
/* #0001	   2017/08/25	   Quyen Ngo			Remove enum and */
/*							   function not use						*/
/*                                                                  */
/********************************************************************/


//init alarm led
void aled_Init(void);

//create timer
void aled_CreateTimer(void);

void aled_PlayHigh(void);

void aled_PlayMedium(void);

void aled_StopMedium(void);

void aled_StopHigh(void);

void aled_RedOn(void);

void aled_RedOff(void);

void aled_YellowOn(void);

void aled_YellowOff(void);

void aled_PowerOn(void);

void aled_PowerOff(void);

//start timer
void aled_StartTimer(void);
#endif /* INC_ALARMLED_H_ */
/* end of file */
