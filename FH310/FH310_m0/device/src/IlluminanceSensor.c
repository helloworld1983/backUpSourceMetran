/********************************************************************/
/*                                                                  */
/* File Name     : IlluminanceSensor.c		                        */
/*                                                                  */
/* General       : This file contains the functions for calculate   */
/*				  the brightness type based on illu valu            */
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
/*             2016/10/15      Quyen Ngo  	new file       		    */
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*					-remove assert									*/
/* #0002	   2017/11/02	   Quyen Ngo	modify		 	        */
/*		-Modify to adapt with new design of adc_Read function		*/
/*                                                                  */
/********************************************************************/

/* include system files */
/* include user files */
#include "IlluminanceSensor.h"
#include "adc.h"

/* constants, macro definition */

#ifdef DEBUG
//#define DEBUG_ILLU_EN
#endif


#ifdef DEBUG_ILLU_EN
#define DEBUG_ILLU(...) printf(__VA_ARGS__)
#else
#define DEBUG_ILLU(...)
#endif

#define BRIGHT_INIT_THRESHOLD 		0.5
#define DARK_INIT_THRESHOLD 	    0.2
#define MIDDLE_TO_BRIGHT_THRESHOLD  0.5
#define BRIGHT_TO_MIDDLE_THRESHOLD  0.4
#define DARK_TO_MIDDLE_THRESHOLD	0.3
#define MIDDLE_TO_DARK_THRESHOLD	0.2

/* definitions of structures */

/*global variable*/
static float s_illuVoltVal = 0.0;
static E_BrightnessType s_currentBrighnessType = MEDIUM;
/* external declaration */

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: illuminance_ChooseInitBrightType
//
//    Detail: Choose Init bright Type
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void illuminance_ChooseInitBrightType(void)
{
	if(s_illuVoltVal > BRIGHT_INIT_THRESHOLD)
	{
		s_currentBrighnessType = BRIGHT;
	}
	else if (s_illuVoltVal < DARK_INIT_THRESHOLD)
	{
		s_currentBrighnessType = DARK;
	}
	else
	{
		s_currentBrighnessType = MEDIUM;
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: illuminace_SwitchBrightType
//
//    Detail: Switch Brightness type depend on s_illuVoltVal
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
static void illuminace_SwitchBrightType(void)
{
	switch (s_currentBrighnessType) {
	case BRIGHT:
		if(s_illuVoltVal <= MIDDLE_TO_DARK_THRESHOLD)
		{
			s_currentBrighnessType = DARK;
		}
		else if(s_illuVoltVal <= BRIGHT_TO_MIDDLE_THRESHOLD)
		{
			s_currentBrighnessType = MEDIUM;
		}
		break;
	case MEDIUM:
		if(s_illuVoltVal >= MIDDLE_TO_BRIGHT_THRESHOLD)
		{
			s_currentBrighnessType = BRIGHT;
		}
		else if(s_illuVoltVal <= MIDDLE_TO_DARK_THRESHOLD)
		{
			s_currentBrighnessType = DARK;
		}
		break;
	case DARK:
		if(s_illuVoltVal >= MIDDLE_TO_BRIGHT_THRESHOLD)
		{
			s_currentBrighnessType = BRIGHT;
		}
		else if(s_illuVoltVal >= DARK_TO_MIDDLE_THRESHOLD)
		{
			s_currentBrighnessType = MEDIUM;
		}
		break;
	default:
		DEBUGOUT("invalid brightness type\n");
		break;
	}
	return;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: illuminance_ReadValue
//
//    Detail: read value of illuminance sensor
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
E_BrightnessType illuminance_ReadValue(void)
{
	ADC_VAL_t stAdcVal;
	stAdcVal = adc_Read(ILLU_SENSOR_CHANNEL);
	s_illuVoltVal = stAdcVal.voltVal;
	DEBUG_ILLU("illu volt val: %f \n",voltVal);
	if(s_currentBrighnessType == INIT)
	{
		illuminance_ChooseInitBrightType();
	}
	else
	{
		illuminace_SwitchBrightType();
	}
	return s_currentBrighnessType;
}

/******************************************************************************/
//COMMON.OPERATION
//    Function Name: illuminance_ReadVoltValue
//
//    Detail: read volt value of illuminance sensor
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: NONE
//
//
//    Using Global Data : (I)
//						  (O)
//
//
//    Note: None
//
/******************************************************************************/
float illuminance_ReadVoltValue(void)
{
	return s_illuVoltVal;
}
