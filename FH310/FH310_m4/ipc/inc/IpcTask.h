#ifndef _IPC_RECEIVE_MSG_H_
#define _IPC_RECEIVE_MSG_H_

/******************************************************************************/
//
//  File Name		: IpcTask.h
//
//	General       	: This file contains function to create and handle operation of ipc communication
//					  between M4 and M0 core
//
//  Product Name	: FH310_m4
//
//
//  Revision History:
//          Rev:      Date:       Editor:
//          01        Oct 4, 2016     Quyen Ngo(MV)
//          Description:  InitialRevision
//
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "DualCoreCommon.h"

/**
 * @brief	IPC Example initialization
 *
 * This function initializes the IPC Example implementation. It calls
 * IPC_initMsgQueue() to initialize the IPC queue of the calling core,
 * by allocating and passing a queue array. It will be called by the
 * @link EXAMPLE_DUALCORE_CMN_MAIN main()@endlink of both core-M0 and
 * core-M4 code for all the dual core examples.
 *
 * @return	None
 */
extern void ipc_Init(void);

extern void ipc_CreateTasks(void);

/**
 * @brief	IPC register callback function pointer
 *
 * This function registers a callback function pointer to a
 * message \a id, whenever a message with \a id is received the
 * register call-back function will be invoked.
 *
 * @param	id		: ID of message to which the callback \a func be associated
 * @param	func	: pointer to callback function
 * @return	0 on failure [given \a id is greater than 
 * @note	#IPCEX_MAX_IDS], !0 on success
 */
int ipc_RegisterCallback(uint32_t id, void (*func)(IPC_MSG_DATA_t));

/**
 * @brief	Push data on the queue
 * @param	id		: Task ID of the destination task
 * @param	data	: Data containing the message
 * @return	#QUEUE_ERROR or #QUEUE_FULL on error, #QUEUE_INSERT on success
 */
int ipc_PushMsg(uint32_t id, IPC_MSG_DATA_t data);

void ipc_WaitForHandShake();

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _IPC_RECEIVE_MSG_H_ */
