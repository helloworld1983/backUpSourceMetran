/********************************************************************/
/*                                                                  */
/* File Name    : EncoderTask.h                                     */
/*                                                                  */
/* General       :This file contains function for create task and   */
/*                proces encoder interrupt	                        */
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
/*	#0001	   2017/09/28	   Quyen Ngo(MV)		modify			*/
/*		-remove stack high water mark calculation					*/
/* 	#0002	   2017/10/22	   Quyen Ngo(MV)    modify				*/
/*		-Change the wait to handle interrupt not use semaphore		*/
/********************************************************************/

#include "EncoderTask.h"
#include "encoder.h"
#include "EncoderInterface.h"

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoderTask_Run							                 			*/
/*                                                                                      */
/* Details:         run function of Encoder task										*/
/*                                                                                      */
/* Arguments : 							          		                      			*/
/*			  																			*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) 							  									*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
static void encoderTask_Run(void)
{
	while(1)
	{
		encoder_Handle();
		vTaskDelay(ENCODER_TASK_DELAY_TICK);
	}
	return;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoderTaskInit							                 			*/
/*                                                                                      */
/* Details:         Init encoder task													*/
/*                                                                                      */
/* Arguments : 							          		                      			*/
/*			  																			*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) 							  									*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
void encoderTask_Init(void)
{
	encoder_Init();
	return;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	encoderTask_Create						                 			*/
/*                                                                                      */
/* Details:         Create freeRTOS task												*/
/*                                                                                      */
/* Arguments : 							          		                      			*/
/*			  																			*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) 							  									*/
/*                                                                   					*/
/*                                                                                      */
/****************************************************************************************/
void encoderTask_Create(void)
{
	xTaskCreate( (TaskFunction_t)encoderTask_Run, "ENCODER",
			ENCODER_TASK, (void *)1, TASK_PRIO_ENCODER_TASK,
			( TaskHandle_t * ) NULL );
	return;
}

