#ifndef INC_IRDATASK_H_
#define INC_IRDATASK_H_

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

#include "../../ipclib/IpcMsg.h"
#include "../../ipc/inc/IpcTask.h"

void irdaTask_Create(void);

void irdaTask_Init(void);

void irdaTask_HandleDeviceMsg(IPC_MSG_DATA_t ipcMsgData);

#endif /* INC_IRDATASK_H_ */
