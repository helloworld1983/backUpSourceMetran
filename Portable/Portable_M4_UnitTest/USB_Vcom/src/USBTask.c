/********************************************************************/
/*                                                                  */
/* File Name    : USBTask.c                               			*/
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
#include "USBTask.h"
#include "USBControl.h"
#include "UsbVcom.h"
#include "DualCoreCommon.h"

typedef struct
{
    uint8_t mode;
    uint8_t flowRate;
    uint8_t triggerSensitivity;
} SettingValue;

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t logCode;
    uint8_t EventOrAlarm;
    uint8_t alarmType;
    SettingValue oldValue;
    SettingValue newValue;
    uint16_t CRC;
} POR_LOG_T;

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t logCode;
    int8_t PressData[300];
    uint16_t CRC;
} POR_PRESS_LOG_T;

QueueHandle_t logQueueSendForPCApp;
QueueHandle_t logQueueSendPressureForPCApp;
QueueHandle_t USBQueue;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USBTask_Loop							              				*/
/*                                                                                      */
/* Details:        USB task loop function is call function for handle USB communication */
/*                 every 20ms                                                           */
/* Arguments:	void *pvParameters														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void USBTask_Loop(void* pvParameters)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
#ifdef CHECK_REMAINING_STACK_SIZE
	UBaseType_t uxHighWaterMark;
	uint32_t cnt = 0;
#endif
	while(1)
	{
		Chip_WWDT_Feed(LPC_WWDT);
		USB_Handle();
#ifdef CHECK_REMAINING_STACK_SIZE
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		if(cnt>=1000/USB_TASK_DELAY_TIME)
		{
			DEBUGOUT("USB Task %d\n",uxHighWaterMark);
			cnt = 0;
		}
		else
		{
			cnt++;
		}
#endif
		vTaskDelayUntil( &xLastWakeTime, USB_TASK_DELAY_TICK );
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USBTask_Init						              				    */
/*                                                                                      */
/* Details:        Initialize USB task by creating USB queue and config USB vcom 		*/
/*					function															*/
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void USBTask_Init(void)
{
    USBQueue = xQueueCreate(USB_QUEUE_LENGTH, sizeof(E_USBQUEUE));
    logQueueSendForPCApp = xQueueCreate(LOG_QUEUE_LENGTH, sizeof(POR_LOG_T)*NUMBER_OF_LOG_QUEUE_SEND_FOR_PC_APP);
    logQueueSendPressureForPCApp = xQueueCreate(LOG_QUEUE_PRESSURE_LENGTH,sizeof(POR_PRESS_LOG_T));

	USB_init();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	USBTask_CreateTask				              				    	*/
/*                                                                                      */
/* Details:        Create FREERTOS task name USB task									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void USBTask_CreateTask(void)
{
	xTaskCreate(USBTask_Loop,	"USB task",
			USB_TASK_SIZE,(void*)1,TASK_PRIO_USB_TASK,
			( TaskHandle_t *) NULL);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:     SendToUSBQueue                                                      */
/*                                                                                      */
/* Details:        Send data to USB Queue                                             */
/*                                                                                      */
/* Arguments: E_USBQUEUE pUSBQueueData                                            */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*             false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToUSBQueue(E_USBQUEUE pUSBQueueData,TickType_t xTicksToWait)
{
  bool status = false;
  E_USBQUEUE pUSBQueueDataSend = pUSBQueueData;
  //send Data to USB Queue
  if(!xQueueSend(USBQueue, &pUSBQueueDataSend, xTicksToWait))
  {
      //return false if it fails to sent
      status = false;
  }
  else
  {
      //return true if it sent successfully
      status = true;
  }

  return status;
}

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
bool USBTask_ReceiveUSBQueue(E_USBQUEUE *usbQueue)
{
    bool status = false;

    if(xQueueReceive(USBQueue,usbQueue, 0))
    {
        //return true if it receives successfully
        status = true;
    }
    else
    {
        //return false if it fails when receiving
        status = false;
    }

    return status;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:     SendToLogQueueForPCApp                                              */
/*                                                                                      */
/* Details:        Send data to Log Queue for PC App                                  */
/*                                                                                      */
/* Arguments: const POR_LOG_T *plog                                                   */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*             false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToLogQueueForPCApp(const POR_LOG_T *plog,TickType_t xTicksToWait)
{
  bool status = false;
  //send Data to USB Queue
  if(!xQueueSend(logQueueSendForPCApp, plog, xTicksToWait))
  {
      //return false if it fails to sent
      status = false;
  }
  else
  {
      //return true if it sent successfully
      status = true;
  }

  return status;
}

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
void USBTask_ResetLogQueueSendForPCApp()
{
    xQueueReset(logQueueSendForPCApp);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_ReceiveLogQueueSendForApp                                   */
/*                                                                                      */
/* Details:        receive data from Log queue                                          */
/*                                                                                      */
/* Arguments:   POR_LOG_T *plog                                                         */
/*                                                                                      */
/* ReturnValue : true if it receive successfully                                        */
/*               false if it fails to receive                                           */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_ReceiveLogQueueSendForApp(POR_LOG_T *plog)
{
    bool status  = false;
    if(xQueueReceive(logQueueSendForPCApp, plog ,NUMBER_OF_LOG_QUEUE_SEND_FOR_PC_APP))
    {
        //return true if it receives successfully
        status = true;
    }
    else
    {
        //return false if it fails to receive
        status = false;
    }

    return status;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:     SendToLogQueueSendPressureForPCApp                                */
/*                                                                                      */
/* Details:        Send data to Log Queue Pressure for PC App                           */
/*                                                                                      */
/* Arguments: const POR_LOG_T *plog                                                     */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*             false if it fails to sent                                                */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_SendToLogQueueSendPressureForPCApp(const POR_PRESS_LOG_T *plog,TickType_t xTicksToWait)
{
  bool status = false;

  //send Data to USB Queue
  if(!xQueueSend(logQueueSendPressureForPCApp, plog, xTicksToWait))
  {
      //return false if it fails to sent
      status = false;
  }
  else
  {
      //return true if it sent successfully
      status = true;
  }

  return status;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USBTask_ReceiveLogQueueSendPressureForPCApp                         */
/*                                                                                      */
/* Details:        receive data from Log queue                                          */
/*                                                                                      */
/* Arguments:   POR_PRESS_LOG_T *pressLog                                               */
/*                                                                                      */
/* ReturnValue : true if it receive successfully                                        */
/*               false if it fails to receive                                           */
/*                                                                                      */
/****************************************************************************************/
bool USBTask_ReceiveLogQueueSendPressureForPCApp(POR_PRESS_LOG_T *pressLog)
{
    bool status = false;
    if(xQueueReceive(logQueueSendPressureForPCApp,pressLog, 0))
    {
        //return true if it receives successfully
        status = true;
    }
    else
    {
        //return false if it fails to receive
        status = false;
    }
    return status;
}



