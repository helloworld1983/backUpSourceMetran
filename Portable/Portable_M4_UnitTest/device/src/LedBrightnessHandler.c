/********************************************************************/
/*                                                                  */
/* File Name    : LedBrightnessHandler.c	                        */
/*                                                                  */
/* General      : brightness handle function						*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#include <PanelLed.h>
#include <TaskCommon.h>
#include "LedBrightnessHandler.h"
#include "IlluminanceSensor.h"

//#define DEBUG_BACKLIGHT_EN
#ifdef DEBUG_BACKLIGHT_EN
#define DEBUG_BACKLIGHT(...) printf(__VA_ARGS__)
#else
#define DEBUG_BACKLIGHT(...)
#endif
#define BACKLIGHT_UPDATE_TIME 1000/DEVICE_TASK_DELAY_TIME
#define ILLUMINANCE_THRESHOLD_VALUE 0.05

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	backlight_Handle						              				*/
/*                                                                                      */
/* Details:        backlight handle function is call for update backlight value 		*/
/* 				base on illuminance sensor value every 200ms							*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void LedBrightness_Handle()
{
	static float s_IlluminanceSensorValue[BACKLIGHT_UPDATE_TIME];
	static uint8_t s_backlightUpdateTimeCnt=0;
	s_IlluminanceSensorValue[s_backlightUpdateTimeCnt]=illuminance_ReadVoltValue();
	s_backlightUpdateTimeCnt++;
	if(s_backlightUpdateTimeCnt >= BACKLIGHT_UPDATE_TIME)
	{
		s_backlightUpdateTimeCnt=0;
		float avrIlluminanceSensorValue = 0;
		for(int i=0;i<BACKLIGHT_UPDATE_TIME;i++)
		{
			avrIlluminanceSensorValue+=s_IlluminanceSensorValue[i];
		}
		avrIlluminanceSensorValue /=  BACKLIGHT_UPDATE_TIME;
		if(avrIlluminanceSensorValue>ILLUMINANCE_THRESHOLD_VALUE)
		{
			panelLed_SetBrightness(eLevelBrightnessHigh);
			DEBUG_BACKLIGHT("Set High Brightness \n");
		}
		else
		{
			panelLed_SetBrightness(eLevelBrightnessLow);
			DEBUG_BACKLIGHT("Set Low Brightness \n");
		}
	}

}
