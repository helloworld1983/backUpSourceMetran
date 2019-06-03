#ifndef DEVICE_INC_LOGTASK_H_
#define DEVICE_INC_LOGTASK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : LogTask.h		                                	*/
/*                                                                  */
/* General       : This module contain function for Create RTOS task*/
/* to control Log													*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable	                                        */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18, 2016    Thao Ha(MV)  		new file    	*/
/*                                                                  */
/********************************************************************/
#include "LogMgr.h"

#define NUMBER_OF_LOG 0
#define CMD_BY_LOG_TASK 0
#define RESET_CMD 1234

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logTask_InitLogTask                                                 */
/*                                                                                      */
/* Details:        Init Log task by create logQueueRecive, logQueueSend                 */
/*              logQueueSendForPCApp, logQueueSendPressureForPCApp, logQueueDelete      */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void logTask_InitLogTask();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   logTask_CreateTask                                                  */
/*                                                                                      */
/* Details:        Create FREERTOS task name Log task                                   */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void logTask_CreateTask();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   SendToLogQueueReceive                                               */
/*                                                                                      */
/* Details:        Send data to Log Queue Receive                                       */
/*                                                                                      */
/* Arguments:   E_logQueueType LogQueueType                                             */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool logTask_SendToLogQueueReceive(E_logQueueReceive LogQueueData,TickType_t xTicksToWait);

#endif /* DEVICE_INC_LOGTASK_H_ */
