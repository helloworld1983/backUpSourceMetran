#ifndef INC_DEVICE_TASK_H_
	#define INC_DEVICE_TASK_H_

/********************************************************************/
/*                                                                  */
/* File Name    : DeviceController.h                                */
/*                                                                  */
/* General       : This module contain function for Create RTOS task
 * to control and monitor device operation							*/
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
/*             Jun 2, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/* #0001	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/

#include <IpcTask.h>
#include "TaskCommon.h"
#include "OperationswHandle.h"
#include "../../ipclib/IpcMsg.h"

/*
 * Create RTOS task
 * Name: "DEVICE task"
 * Cyclic = 50ms
 * Callback function : device_task
 */
void devTask_Create(void);

/*
 * Init device task by register handlsetiing as call-back function for IPC_receive_msg
 * task
 */
void devTask_Init(void);

#endif /* INC_DEVICE_TASK_H_ */
