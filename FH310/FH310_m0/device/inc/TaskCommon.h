#ifndef INC_TASK_COMMON_H_
	#define INC_TASK_COMMON_H_
/********************************************************************/
/*                                                                  */
/* File Name    : TaskCommon.h                                	    */
/*                                                                  */
/* General       : This module contain variable and macro define share
 * 					between freeRTOS task							*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jun 21, 2016     Quyen Ngo(MV)  		new file        */
/*#0001		   Aug 25, 2017     Quyen Ngo(MV)       remove macro not*/
/*					use(M0_IPC_QUEUE_LENGTH)						*/
/*#0002		   2017/10/21		Quyen Ngo(MV)		modify			*/
/*		-Add define delay for Encoder task							*/
/********************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define TASK_PRIO_DEVICE_TASK 	  (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_ALARM_TASK 	  (tskIDLE_PRIORITY + 2UL)
#define TASK_PRIO_IPC_DISPATCH    (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_ENCODER_TASK	  (tskIDLE_PRIORITY + 3UL)

#define DEVICE_TASK_STACK			512
#define ENCODER_TASK				256

//task delay time
#define DEVICE_TASK_DELAY_TIME 		20 //20ms
#define DEVICE_TASK_DELAY_TICK 		DEVICE_TASK_DELAY_TIME / portTICK_PERIOD_MS
#define IPC_TASK_DELAY_TIME_MS 		20
#define IPC_TASK_DELAY_TICK 		IPC_TASK_DELAY_TIME_MS / portTICK_PERIOD_MS
#define DEVICE_QUEUE_WAIT_TIME 		20/portTICK_PERIOD_MS			//20ms

#define ENCODER_TASK_DELAY_TIME_MS	50
#define ENCODER_TASK_DELAY_TICK     ENCODER_TASK_DELAY_TIME_MS / portTICK_PERIOD_MS
#define DEVICE_QUEUE_LENGTH	8

#endif /* INC_TASK_COMMON_H_ */
