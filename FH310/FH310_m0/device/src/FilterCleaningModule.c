/********************************************************************/
/*                                                                  */
/* File Name     : FilterCleaningModule.c		                    */
/*                                                                  */
/* General       : This file contains the function for init and    	*/
/*				   handle operation  of filer cleaning module       */
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
/*             2016/11/26      Quyen Ngo  	new file       		    */
/*                                                                  */
/*	#0001	   2017/10/28	   Quyen Ngo 			modify			*/
/*		-Change s_nFcmActCnt, s_tFcmWtCnt to global in this file	*/
/*	#0002	   2017/10/28	   Quyen Ngo 			modify			*/
/*		-Reset s_nFcmActCnt, s_tFcmWtCnt when filter stop for avoid	*/
/*		the case that filter stop in middle of Filter cycle			*/
/*	#0003	   2017/10/28	   Quyen Ngo 			modify			*/
/*		-Remove check the s_fPrevErrPinState == false in the condition*/
/*		to check filter in start position(Bug 2000)					*/
/*	#0004	   2017/11/20	   Quyen Ngo 			modify			*/
/*		-Clear 	s_isEnableCheckErr,s_fcmEnaPrevStatus when filter   */
/*		stop														*/
/*	#0005	   2018/01/19	   Quyen Ngo 			modify			*/
/*		-Change filter cycle time from 72h to 6h   					*/
/*																	*/
/********************************************************************/

/* include system files */
/* include user files */
#include "FilterCleaningModule.h"
#include "gpio.h"
#include "timers.h"

/* definitions of structures */
#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

#define N_FCM_ACT				10
#define FCM_WT_TICK				900/DEVICE_TASK_DELAY_TIME
#define FT_ERR_TIMEOUT_TICK		30000/DEVICE_TASK_DELAY_TIME
/** >>#5
Change filter cycle time from 72h to 6h
#5 >>**/
#define T_FCM_CYCLE_IN_SEC      6*3600  //6h
#define CUMULATIVE_MAX_IN_SEC			0xFFFFFFFF
/*global variable*/

static E_FilterSequence s_FilterSequence = FT_STOP;

static bool s_fErrPinState = false;

static bool s_fPrevErrPinState = false;

static bool s_isFilterErr = false;

static bool s_fcmEnaStatus = false;

static uint32_t s_cumulativeHourinSec = 0;

static uint8_t s_nFcmActCnt = 0;

static uint8_t s_tFcmWtCnt = 0;

/* external declaration */

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_Init															*/
/*                                                                                      */
/* Details:  Config and int GPIO for Filter cleaning module								*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FILTER_PORT															*/
/*					FILTER_BIT															*/
/*					LPC_GPIO_PORT														*/
/*					FILTER_ERR_PORT														*/
/*					FILTER_ERR_BIT														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_Init(void)
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, FILTER_PORT, FILTER_BIT);		/* output */
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, FILTER_PORT, FILTER_BIT);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, FILTER_ERR_PORT, FILTER_ERR_BIT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_On															*/
/*                                                                                      */
/* Details:  Turn on Filter ENA signal													*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FILTER_PORT															*/
/*					FILTER_BIT															*/
/*					LPC_GPIO_PORT														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_On(void)
{
	s_fcmEnaStatus = true;
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, FILTER_PORT, FILTER_BIT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_Off															*/
/*                                                                                      */
/* Details:  Turn off Filter ENA signal													*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   		FILTER_PORT															*/
/*					FILTER_BIT															*/
/*					LPC_GPIO_PORT														*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : 																	*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_Off(void)
{
	s_fcmEnaStatus = false;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, FILTER_PORT, FILTER_BIT);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_Start															*/
/*                                                                                      */
/* Details:  start filter module and set s_FilterSequence is start 						*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FilterSequence s_FilterSequence 							*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_Start(void)
{
	DEBUGOUT("FT Start \n");
	s_FilterSequence = FT_START;
	filter_On();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_Stop															*/
/*                                                                                      */
/* Details:  stop filter module and set s_FilterSequence is stop reset err status		*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FilterSequence s_FilterSequence 							*/
/*					   (O) bool s_isFilterErr											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_Stop(void)
{
	s_nFcmActCnt = 0;
	s_tFcmWtCnt = 0;
	s_isFilterErr = false;
	s_FilterSequence = FT_STOP;
	filter_Off();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_Handle														*/
/*                                                                                      */
/* Details:  Handle filter module operation and detect error							*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O) E_FilterSequence s_FilterSequence 							*/
/*					   (O) bool s_isFilterErr											*/
/*					   (O) bool s_fErrPinState											*/
/*					   (O) bool s_fPrevErrPinState										*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_Handle(void)
{
	static uint32_t s_firstTFcmCycleTime = 0;
	//check filter is on start position or not
	s_fErrPinState = Chip_GPIO_GetPinState(LPC_GPIO_PORT, FILTER_ERR_PORT, FILTER_ERR_BIT);
	switch (s_FilterSequence) {
	case FT_START:
		if (s_fErrPinState == true)
		{
			DEBUGOUT("FT_START FT_NEW_CYCLE\n");
			s_FilterSequence = FT_NEW_CYCLE;
		}
		break;
	case FT_NEW_CYCLE:
		if ((s_fErrPinState == true) && (s_fPrevErrPinState == false))
		{
			s_nFcmActCnt++;
			DEBUGOUT("s_nFcmActCnt %d \n",s_nFcmActCnt);
			if(s_nFcmActCnt == N_FCM_ACT)
			{
				DEBUGOUT("FT_WAIT_T_FCM_WT \n");
				s_nFcmActCnt = 0;
				s_FilterSequence = FT_WAIT_T_FCM_WT;
			}
		}
		break;
	case FT_WAIT_T_FCM_WT:
		s_tFcmWtCnt++;
		if(s_tFcmWtCnt >= FCM_WT_TICK)
		{
			DEBUGOUT("FT_WAIT_T_FCM_CYCLE \n");
			s_tFcmWtCnt = 0;
			filter_Off();
			s_FilterSequence = FT_WAIT_T_FCM_CYCLE;
			s_firstTFcmCycleTime = s_cumulativeHourinSec;
			DEBUGOUT("Ft 1st time %d \n",s_cumulativeHourinSec);
		}
		break;
	case FT_WAIT_T_FCM_CYCLE:
		//Count 6h and switch to new Cycle
//		DEBUGOUT("Ft hour: %d\n",s_cumulativeHourinSec);
		if((s_cumulativeHourinSec >= s_firstTFcmCycleTime))
		{
			if((s_cumulativeHourinSec - s_firstTFcmCycleTime) == T_FCM_CYCLE_IN_SEC)
			{
				DEBUGOUT("FT_NEW_CYCLE\n");
				s_FilterSequence = FT_NEW_CYCLE;
				filter_On();
			}
		}
		else
		{
			if((s_cumulativeHourinSec - s_firstTFcmCycleTime + CUMULATIVE_MAX_IN_SEC) == T_FCM_CYCLE_IN_SEC)
			{
				DEBUGOUT("FT_NEW_CYCLE\n");
				s_FilterSequence = FT_NEW_CYCLE;
				filter_On();
			}
		}
		break;
	default:
		break;
	}
	filter_DetectErr(s_fPrevErrPinState, s_fErrPinState, s_FilterSequence);
	s_fPrevErrPinState = s_fErrPinState;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_UpdateCumulativeHour											*/
/*                                                                                      */
/* Details:  Handle filter module operation and detect error							*/
/*                                                                                      */
/* Arguments :	(I)	uint32_t sec									 					*/
/*				-Unit: second															*/
/*				-Range: 0~2^32															*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	void												   				*/
/*                                                                                      */
/* Using Global Data : (O)	uint32_t s_cumulativeHourinSec								*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
void filter_UpdateCumulativeHour(uint32_t sec)
{
	s_cumulativeHourinSec = sec;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_DetectErr														*/
/*                                                                                      */
/* Details:  etect err of filter cleaning moduleS										*/
/*                                                                                      */
/* Arguments :	(I)	bool prevErrState								 					*/
/*				(I) bool errState														*/
/*				(I) E_FilterSequence filterSequence										*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	bool true/false: err/no err							   				*/
/*                                                                                      */
/* Using Global Data : 	(O) bool s_isFilterErr											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
bool filter_DetectErr(bool prevErrState, bool errState, E_FilterSequence filterSequence)
{
	static bool s_fcmEnaPrevStatus = false;
	static bool s_isEnableCheckErr = false;
	volatile static uint16_t s_checkErrPinCnt = 0;
	if(filterSequence != FT_STOP)
	{
		if((s_fcmEnaStatus == true) && (s_fcmEnaPrevStatus == false))
		{
			s_checkErrPinCnt = 0;//FCMENA turn on, start count time
			s_isEnableCheckErr = true;
			prevErrState = errState;//Avoid wrong detection when prevErr hold the value before fcmENA is ON
		}
		else if(s_fcmEnaStatus == false)
		{
			s_checkErrPinCnt = 0;
			s_isEnableCheckErr = false;
		}
		s_fcmEnaPrevStatus = s_fcmEnaStatus;
		if(s_isEnableCheckErr == true)
		{
			if(errState != prevErrState)
			{
				s_checkErrPinCnt = 0;
				return s_isFilterErr;
			}
			s_checkErrPinCnt++;
			if(s_checkErrPinCnt >= FT_ERR_TIMEOUT_TICK)
			{
				s_checkErrPinCnt = 0;
				s_isFilterErr = true;
			}
		}
	}
	else
	{
		s_isEnableCheckErr = false;
		s_fcmEnaPrevStatus = false;
	}
	return s_isFilterErr;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_getErrPinState												*/
/*                                                                                      */
/* Details:  Get photo err pin state of filter module									*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	bool true/false: err signal is high or low			 				*/
/*                                                                                      */
/* Using Global Data : 	(I) bool s_fErrPinState											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
bool filter_getErrPinState(void)
{
	return s_fErrPinState;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  filter_getErrStatus													*/
/*                                                                                      */
/* Details:  get err status of filter module											*/
/*                                                                                      */
/* Arguments :														 					*/
/*                                                                                      */
/* Macros:	   																			*/
/*																			 			*/
/* ReturnValue : 	bool true/false: err/ no err						 				*/
/*                                                                                      */
/* Using Global Data : 	(I)bool s_isFilterErr											*/
/*                     								                                 	*/
/*【Note】                                                                            											    	*/
/*                                                                                      */
/****************************************************************************************/
bool filter_getErrStatus(void)
{
	return s_isFilterErr;
}
