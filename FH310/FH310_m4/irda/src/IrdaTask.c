/********************************************************************/
/*                                                                  */
/* File Name    : irda_task.h                                    		*/
/*                                                                  */
/* General       : Check the irda command and send to GUI task        */
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14      Quyen Ngo		  	new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include <irda.h>
#include <IrdaTask.h>
#include <TaskCommon.h>
#include <UsbMgr.h>

#include "DualCoreCommon.h"
#define IRDA_STACK_SIZE	256

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check the irda command from remote control and send to GUI task		    */
/*                                                                                      */
/* Details  -Send Irda command to GUI task via queue					      			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
static void irda_task(void* loop)
{
	while(1)
	{
		if(xSemaphoreTake(g_irdaBinarySem,portMAX_DELAY))
		{
			irda_Handle();
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Create irda task														    */
/*                                                                                      */
/* Details  -Create irda task with														*/
/* 			 task name: IRDA Task
 * 			 task function: irda_task
 * 			 task priority: TASK_PRIO_IRDA_TASK = +2					 				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void irdaTask_Create(void)
{
	/* Start Irda Task */
	xTaskCreate( (TaskFunction_t)irda_task, "IRDA Task",
			IRDA_STACK_SIZE, (void *)1, TASK_PRIO_IRDA_TASK,
			( TaskHandle_t * ) NULL );
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name 	-irda task init															    */
/*                                                                                      */
/* Details  -init the irda task queue									 				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE					 					  							*/
/*																						*/
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
void irdaTask_Init(void)
{
	vSemaphoreCreateBinary(g_irdaBinarySem);
	irda_InitTimer();
	return;
}
