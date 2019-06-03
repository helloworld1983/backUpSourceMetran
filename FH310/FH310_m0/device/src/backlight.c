/********************************************************************/
/*                                                                  */
/* File Name     : backlight.c			                            */
/*                                                                  */
/* General       : This file contain function for control backlight */
/*                 depend on the brightness condition of the 		*/
/*				   environment and step value of backlight setting  */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/07      Quyen Ngo  			new file  	    */
/*																	*/
/*   1.		   2017/08/25	   Quyen Ngo			Add macros		*/
/*					BACKLIGHT_MAX_STEP, MAX_DUTY for check range of */
/*					Argument of backlight_Setduty and 				*/
/*					backlight_SetStep function                      */
/*	#0002	   2017/09/01	   Quyen Ngo			modify			*/
/*		-remove assert												*/
/********************************************************************/


#include "backlight.h"
#include "IlluminanceSensor.h"
#include "TaskCommon.h"

#ifdef DEBUG
//#define DEBUG_BACKLIGHT_EN
#endif

#ifdef DEBUG_BACKLIGHT_EN
#define DEBUG_BACKLIGHT(...) printf(__VA_ARGS__)
#else
#define DEBUG_BACKLIGHT(...)
#endif

#define BACKLIGH_STEP_NUM 	11
#define BACKLIGHT_MAX_STEP 	10
#define BACKLIGHT_SETP_MIN 	0
#define BACKLIGHT_UPDATE_TIME 200/DEVICE_TASK_DELAY_TIME
#define ERROR_STEP -1
#define MAX_DUTY	100

typedef struct
{
	uint8_t step; //step of brightness 0-10
	uint8_t value; //duty cycle of backlight PWM corresponding with step
} BACLIGHT_STEP_t;

static BACLIGHT_STEP_t s_backLightTableBright[BACKLIGH_STEP_NUM] =
{
		{0, 0},
		{1,50},
		{2,55},
		{3,60},
		{4,65},
		{5,70},
		{6,75},
		{7,80},
		{8,85},
		{9,90},
		{10,95}
};

static BACLIGHT_STEP_t s_backLightTableMedium[BACKLIGH_STEP_NUM] =
{
		{0, 0},
		{1,38},
		{2,41},
		{3,45},
		{4,49},
		{5,52},
		{6,56},
		{7,60},
		{8,64},
		{9,68},
		{10,71}
};

static BACLIGHT_STEP_t s_backLightTableDark[BACKLIGH_STEP_NUM] =
{
		{0, 0},
		{1,13},
		{2,14},
		{3,15},
		{4,16},
		{5,18},
		{6,19},
		{7,20},
		{8,21},
		{9,23},
		{10,24}
};

static uint8_t s_blUpdateTimeCnt = 0;

static uint8_t s_currentStep = 5;

/****************************************************************************************/
/*                                                                                      */
/* Function name:  backligh_GetValueOfStep												*/
/*                                                                                      */
/* Details:  Get the value of step in backlight step table								*/
/*                                                                                      */
/* Arguments :	(I)uint8_t step										 					*/
/*				range: 0~10															    */
/*				(I) E_BrightnessType type												*/
/*				range {BRIGHT, MEDIUM, DARK} 											*/
/*                                                                                      */
/* Macros:	   		ERROR_STEP															*/
/*																						*/
/* ReturnValue : 	int8_t : The duty value match with input step and bright ness type, */
/*							 return ERROR_STEP if cannot find matching value			*/
/*					range :  0~100, ERROR_STEP = -1										*/
/*                                                                                      */
/* Using Global Data : (I) 	BACLIGHT_STEP_t s_backLightTableBright						*/
/*											s_backLightTableMedium						*/
/*                    					    s_backLightTableDark                        */
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static int8_t backlight_GetValueOfStep(uint8_t step,
		E_BrightnessType type)
{
	BACLIGHT_STEP_t* currrentBacklightTable=NULL;
	switch (type) {
	case BRIGHT:
		currrentBacklightTable = s_backLightTableBright;
		break;
	case MEDIUM:
		currrentBacklightTable = s_backLightTableMedium;
		break;
	case DARK:
		currrentBacklightTable = s_backLightTableDark;
		break;
	default:
		DEBUGOUT("ERR backlight wrong brightness type \n");
		break;
	}
	for (int i = 0; i< BACKLIGH_STEP_NUM; i++)
	{
		if(currrentBacklightTable != NULL)
		{
			if(step == currrentBacklightTable[i].step)
			{
				return currrentBacklightTable[i].value;
			}
		}
	}
	return ERROR_STEP;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  backlight_Handle														*/
/*                                                                                      */
/* Details:  Handle backlight brightness depend on current step in service mode			*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		BACKLIGHT_UPDATE_TIME												*/
/*					ERROR_STEP														    */
/*																			 			*/
/* ReturnValue : 	void												   				 */
/*                                                                                      */
/* Using Global Data : (I) 	uint8_t s_blUpdateTimeCnt									*/
/*							range:  0~255												*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void backlight_Handle(void)
{
	s_blUpdateTimeCnt++;
	if(s_blUpdateTimeCnt >= BACKLIGHT_UPDATE_TIME)
	{
		s_blUpdateTimeCnt = 0;
		E_BrightnessType brightType = illuminance_ReadValue();
		int8_t duty = backlight_GetValueOfStep(s_currentStep,brightType);
		if(duty == ERROR_STEP)
		{
			DEBUGOUT("Backlihgt Err step \n");
			return;
		}
		DEBUG_BACKLIGHT("duty: %d \n",duty);
		backlight_Setduty(duty);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  backlight_Init														*/
/*                                                                                      */
/* Details:  Set init pwm duty value for back light										*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		SCT_PWM																*/
/*					SCT_PWM_BACKLIGHT													*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 	BACLIGHT_STEP_t s_backLightTableBright						*/
/*											s_backLightTableMedium						*/
/*                    					    s_backLightTableDark                        */
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void backlight_Init(void)
{
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_BACKLIGHT, Chip_SCTPWM_PercentageToTicks(SCT_PWM, 50));//init 50%
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  backlight_Setduty													*/
/*                                                                                      */
/* Details:  Set duty cycle of backlight by call LPC open chip lib, if applicaion want  */
/* 				to stop backlight just set dutycycle = 0								*/
/*                                                                                      */
/* Arguments :		uint8_t dutyCycle								 					*/
/*					range: 0~100														*/
/*                                                                                      */
/* Macros:	   		MAX_DUTY															*/
/*			   		SCT_PWM																*/
/*					SCT_PWM_BACKLIGHT													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : NONE 															*/
/*                     								                                 	*/
/*【Note】                                                                           		*/
/*                                                                                      */
/****************************************************************************************/
void backlight_Setduty(uint8_t dutyCycle)
{
	uint8_t invDuty;
	if(dutyCycle > MAX_DUTY)
	{
		DEBUGOUT("Back light duty out of range \n");
		return;
	}
	invDuty = MAX_DUTY - dutyCycle;
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_BACKLIGHT, Chip_SCTPWM_PercentageToTicks(SCT_PWM, invDuty));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  backlight_SetStep													*/
/*                                                                                      */
/* Details:  Set current backlight  step												*/
/*                                                                                      */
/* Arguments :		Set current backlight  step						 					*/
/*					range: 0~10															*/
/*                                                                                      */
/* Macros:	   		MAX_DUTY															*/
/*			   		SCT_PWM																*/
/*					SCT_PWM_BACKLIGHT													*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) uint8_t s_currentStep  										*/
/*					   range 0~10														*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void backlight_SetStep(uint8_t step)
{
	if((step > BACKLIGHT_MAX_STEP))
	{
		DEBUGOUT("Backlight err step \n");
		return;
	}
	s_currentStep = step;
	return;
}
/* end of file */
