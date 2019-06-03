/********************************************************************/
/*                                                                  */
/* File Name    : USBTask.h                               			*/
/*                                                                  */
/* General      : Initialize USB function and create Queue and		*/
/*  			RTOS Task	     									*/
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/
#ifndef USB_VCOM_INC_USBTASK_H_
#define USB_VCOM_INC_USBTASK_H_


#include <TaskCommon.h>
#include "DualCoreCommon.h"
#include "LogMgr.h"

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_Init                                                        */
/*                                                                                      */
/* Details:        Initiallize USB task by creating USB queue and config USB vcom       */
/*                  function                                                            */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void USBTask_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_CreateTask                                                  */
/*                                                                                      */
/* Details:        Create FREERTOS task name USB task                                   */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void USBTask_CreateTask(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   SendToUSBQueue                                                      */
/*                                                                                      */
/* Details:        Send data to USB Queue                                               */
/*                                                                                      */
/* Arguments:   E_USBQUEUE pUSBQueueData                                                */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToUSBQueue(E_USBQUEUE pUSBQueueData,TickType_t xTicksToWait);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_ReceiveUSBQueue                                             */
/*                                                                                      */
/* Details:        receive data from USB Queue                                          */
/*                                                                                      */
/* Arguments:   E_USBQUEUE *usbQueue                                                    */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_ReceiveUSBQueue(E_USBQUEUE *usbQueue);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   SendToLogQueueForPCApp                                              */
/*                                                                                      */
/* Details:        Send data to Log Queue for PC App                                    */
/*                                                                                      */
/* Arguments:   const POR_LOG_T *plog                                                   */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToLogQueueForPCApp(const POR_LOG_T *plog,TickType_t xTicksToWait);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_ResetLogQueueSendForPCApp                                   */
/*                                                                                      */
/* Details:        Reset log Queue send for PC App                                      */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue :  void                                                                  */
/*                                                                                      */
/****************************************************************************************/
void USBTask_ResetLogQueueSendForPCApp();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_ReceiveLogQueueSendForApp                                   */
/*                                                                                      */
/* Details:        receive data from Log queue                                          */
/*                                                                                      */
/* Arguments:   POR_LOG_T *plog                                                         */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_ReceiveLogQueueSendForApp(POR_LOG_T *plog);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   SendToLogQueueSendPressureForPCApp                                  */
/*                                                                                      */
/* Details:        Send data to Log Queue Pressure for PC App                           */
/*                                                                                      */
/* Arguments:   const POR_LOG_T *plog                                                   */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToLogQueueSendPressureForPCApp(const POR_PRESS_LOG_T *plog,TickType_t xTicksToWait);

bool USBTask_ReceiveLogQueueSendPressureForPCApp(POR_PRESS_LOG_T *pressLog);

#endif /* USB_VCOM_INC_USBTASK_H_ */
