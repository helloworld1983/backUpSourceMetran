/********************************************************************/
/*                                                                  */
/* File Name    : Delay.c                       					*/
/*                                                                  */
/* General       : Support delay procedure in case of OS running 	*/
/*				   or stop running as well							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       2017/09/27      Viet Le				modify			*/
/*       Add #define INSTRUCTION_FRE_KHZ	(204000/4) 				*/
/*			in  delay_HardDelay() function (bug 1917)				*/
/********************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include <Delay.h>

/****************************************************************************************/
/*                                                                                      */
/* General 	-Delay_HardDelay												    		*/
/*                                                                                      */
/* Details  -HardDelay force CPU standing on the HardDelay function.			    	*/
/* 			-HardDelay should only call when RTOS is not stared yet,					*/
/* 					such as while system initialization									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) unsigned long ms	: - miliseconds										*/
/*					-range : 0-4294967296												*/
/*                  -unit : ms                                                          */
/*        		 			                                                            */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void delay_HardDelay(unsigned long ms)
{
#define SYSTEM_FRE_KHZ			(204000)
#define CYCLE_PER_INSTRUCTION	(4)
#define INSTRUCTION_FRE_KHZ		(SYSTEM_FRE_KHZ/CYCLE_PER_INSTRUCTION)
	for(long i = 0; i < ms; i++)
	{
		for(long j = 0; j < INSTRUCTION_FRE_KHZ; j++)
			asm("nop");	//do nothing
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Delay_SoftDelay												    		*/
/*                                                                                      */
/* Details  -soft delay is performed by RTOS,											*/
/*           soft delay just put the calling task to block			    				*/
/*			 it does not hold CPU resource												*/
/*                                                                                      */
/* Arguments : (I) unsigned long ms	: - miliseconds										*/
/*					-range : 0-4294967296												*/
/*                  -unit : ms                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void delay_SoftDelay(unsigned long ms)
{
	vTaskDelay(ms/portTICK_RATE_MS);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Delay_SoftDelay												    		*/
/*                                                                                      */
/* Details  -soft delay is performed by RTOS,											*/
/*           soft delay just put the calling task to block			    				*/
/*			 it does not hold CPU resource												*/
/*                                                                                      */
/* Arguments : (I) unsigned long ms	: - miliseconds										*/
/*					-range : 0-4294967296												*/
/*                  -unit : ms                                                          */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void delay_MS(unsigned long ms)
{
	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)	//scheduler is not started yet
	{
		delay_HardDelay(ms);
	}
	else		//scheduler is already started
	{
		vTaskDelay(ms/portTICK_RATE_MS);
	}

	return;
}

/* end of files */

