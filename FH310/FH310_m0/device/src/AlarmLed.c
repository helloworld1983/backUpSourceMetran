/********************************************************************/
/*                                                                  */
/* File Name     : AlarmLed.c			                            */
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
/*	1.		   2017/08/25	   Quyen Ngo			Add macoro		*/
/*			               WAIT_TIMER_QUEUE_MAX_TICK for instead of */
/*						   the using direct value					*/
/* #0002	   2017/09/01	   Quyen Ngo			modify			*/
/*		-remove assert	in aled_timer_callback function				*/
/*                                                                  */
/********************************************************************/


#include "gpio.h"
#include <AlarmLed.h>
#include "TaskCommon.h"
#include "timers.h"

#define HIGH_RED_LED_ID 		1
#define MED_YELLOW_LED_ON_ID 	2
#define MED_YELLOW_LED_OFF_ID 	3

#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif

#define HIGH_RED_LED_TIME 		   250
#define MED_YELLOW_ON_TIME 		   250
#define MED_YELLOW_OFF_TIME 	   1000

#define WAIT_TIMER_QUEUE_MAX_TICK  10

static TimerHandle_t s_highRedLedTimer;
static TimerHandle_t s_medYellowOnTimer;
static TimerHandle_t s_medYellowOffTimer;

static bool s_redLedBlinkyState = true;

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_TimerCallback															*/
/*                                                                                      */
/* Details:  Define a callback function that will be used by multiple timer instances	*/
/*                                                                                      */
/* Arguments : (I) TimerHandle_t xTimer					 					  			*/
/*                                                                                      */
/* Macros:	   HIGH_RED_LED_ID															*/
/*			   MED_YELLOW_LED_ON_ID														*/
/*			   MED_YELLOW_LED_OFF_ID													*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I)(O) bool s_redLedBlinkyState									*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
static void aled_TimerCallback( TimerHandle_t xTimer )
{
	uint32_t timerId =  ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	case HIGH_RED_LED_ID:
		if (s_redLedBlinkyState == false) {
			aled_RedOn();
			s_redLedBlinkyState = true;
		} else {
			aled_RedOff();
			s_redLedBlinkyState = false;
		}
		break;
	case MED_YELLOW_LED_ON_ID:
		aled_YellowOff();
		if (xTimerStart(s_medYellowOffTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
			DEBUGOUT("ERR: Failed to start s_medYellowOffTimer \n");
		}
		break;
	case MED_YELLOW_LED_OFF_ID:
		aled_YellowOn();
		if (xTimerStart(s_medYellowOnTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
			DEBUGOUT("ERR: Failed to start s_medYellowOnTimer \n");
		}
		break;
	default:
		DEBUGOUT("Wrong aled timer id \n");
		break;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  aled_Init															*/
/*                                                                                      */
/* Details:  Config GPIO port for alarm led												*/
/*                                                                                      */
/* Arguments : (I) TimerHandle_t xTimer					 					  			*/
/*                                                                                      */
/* Macros:	   LPC_GPIO_PORT															*/
/*			   LED_POWER_ENABLE_PORT_NUM												*/
/*			   LED_POWER_ENABLE_BIT_NUM													*/
/*			   LED_POWER_ENABLE_BIT_NUM													*/
/*			   RED_LED_PORT_NUM															*/
/*			   RED_LED_BIT_NUM															*/
/*			   YELLOW_LED_PORT_NUM														*/
/*			   YELLOW_LED_BIT_NUM														*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data :																	*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_Init(void) {
	// Set pin as GPIO output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,LED_POWER_ENABLE_PORT_NUM, LED_POWER_ENABLE_BIT_NUM);		/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,RED_LED_PORT_NUM, RED_LED_BIT_NUM);							/* output */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,YELLOW_LED_PORT_NUM, YELLOW_LED_BIT_NUM);					/* output */

	aled_RedOff();
	aled_YellowOff();
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:  aled_CreateTimer														*/
/*                                                                                      */
/* Details:  create all timer use for alarm led											*/
/*                                                                                      */
/* Arguments : 											 					  			*/
/*                                                                                      */
/* Macros:	   HIGH_RED_LED_TIME														*/
/*			   HIGH_RED_LED_ID															*/
/*			   MED_YELLOW_ON_TIME   													*/
/*			   MED_YELLOW_LED_ON_ID														*/
/*			   MED_YELLOW_OFF_TIME														*/
/*			   MED_YELLOW_OFF_ID														*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data :	(O)  TimerHandle_t s_highRedLedTimer							*/
/*							 TimerHandle_t s_medYellowOnTimer							*/
/*							 TimerHandle_t s_medYellowOffTer							*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_CreateTimer(void) {
	s_highRedLedTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"hRled tm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(HIGH_RED_LED_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) HIGH_RED_LED_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					aled_TimerCallback
			);

	s_medYellowOnTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"mYledOnTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(MED_YELLOW_ON_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) MED_YELLOW_LED_ON_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					aled_TimerCallback
			);

	s_medYellowOffTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
		                      kernel. */
					"mYledOfTm",
					/* The timer period in ticks, must be
		                      greater than 0. */
					pdMS_TO_TICKS(MED_YELLOW_OFF_TIME),
					/* The timers will auto-reload themselves
		                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
		                      number of times the timer has expired, which
		                      is initialised to 0. */
					( void * ) MED_YELLOW_LED_OFF_ID,
					/* Each timer calls the same callback when
		                      it expires. */
					aled_TimerCallback
			);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_PlayHigh																*/
/*                                                                                      */
/* Details:  Play high alarm led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   HIGH_RED_LED_ID															*/
/*			   MED_YELLOW_LED_ON_ID														*/
/*			   MED_YELLOW_LED_OFF_ID													*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) TimerHandle_t s_highRedLedTimer								*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_PlayHigh(void)
{
	aled_StopMedium();
	if (xTimerStart(s_highRedLedTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_highRedLedTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_StopHigh																*/
/*                                                                                      */
/* Details:  Stop high alarm led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		WAIT_TIMER_QUEUE_MAX_TICK											*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) TimerHandle_t s_highRedLedTimer								*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_StopHigh(void)
{
	aled_RedOff();
	if (xTimerStop(s_highRedLedTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_highRedLedTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_PlayMedium															*/
/*                                                                                      */
/* Details:  stop high alarm and play medium alarm led									*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		WAIT_TIMER_QUEUE_MAX_TICK											*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) TimerHandle_t s_medYellowOnTimer								*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_PlayMedium(void)
{
	aled_StopHigh();
	aled_YellowOn();
	if (xTimerStart(s_medYellowOnTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_medYellowOnTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_StopMedium															*/
/*                                                                                      */
/* Details:  stop medium alarm led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		WAIT_TIMER_QUEUE_MAX_TICK											*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) TimerHandle_t s_medYellowOnTimer								*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_StopMedium(void)
{
	aled_YellowOff();
	if (xTimerStop(s_medYellowOnTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_medYellowOnTimer \n");
	}
	if (xTimerStop(s_medYellowOffTimer,WAIT_TIMER_QUEUE_MAX_TICK) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_medYellowOffTimer \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_RedOn																	*/
/*                                                                                      */
/* Details:  Turn on red alarm led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					RED_LED_PORT_NUM													*/
/*					RED_LED_BIT_NUM														*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_RedOn(void)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,RED_LED_PORT_NUM, RED_LED_BIT_NUM);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_RedOn																	*/
/*                                                                                      */
/* Details:  Turn on red alarm led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					RED_LED_PORT_NUM													*/
/*					RED_LED_BIT_NUM														*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_RedOff(void)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,RED_LED_PORT_NUM, RED_LED_BIT_NUM);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_YellowOn																*/
/*                                                                                      */
/* Details:  Turn on yellow alarm led													*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					YELLOW_LED_PORT_NUM													*/
/*					YELLOW_LED_BIT_NUM													*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_YellowOn(void)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,YELLOW_LED_PORT_NUM, YELLOW_LED_BIT_NUM);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_YellowOff																*/
/*                                                                                      */
/* Details:  Turn off yellow alarm led													*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					YELLOW_LED_PORT_NUM													*/
/*					YELLOW_LED_BIT_NUM													*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_YellowOff(void)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,YELLOW_LED_PORT_NUM, YELLOW_LED_BIT_NUM);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_PowerOn																*/
/*                                                                                      */
/* Details:  Turn on power NG led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					LED_POWER_ENABLE_PORT_NUM											*/
/*					LED_POWER_ENABLE_BIT_NUM											*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_PowerOn(void)
{
	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,LED_POWER_ENABLE_PORT_NUM,LED_POWER_ENABLE_BIT_NUM); // Turn on power led indicator
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General:  aled_PowerOff																*/
/*                                                                                      */
/* Details:  Turn off power NG led														*/
/*                                                                                      */
/* Arguments :											 					  			*/
/*                                                                                      */
/* Macros:	   		LPC_GPIO_PORT														*/
/*					LED_POWER_ENABLE_PORT_NUM											*/
/*					LED_POWER_ENABLE_BIT_NUM											*/
/*																						*/
/* ReturnValue : 	void																*/
/*                                                                                      */
/* Using Global Data : (I) 																*/
/*                    					                                            	*/
/*                     								                                 	*/
/*【Note】                                                                            											    */
/*                                                                                      */
/****************************************************************************************/
void aled_PowerOff(void)
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,LED_POWER_ENABLE_PORT_NUM,LED_POWER_ENABLE_BIT_NUM); // Turn off power led indicator
	return;
}
/* end of file */
