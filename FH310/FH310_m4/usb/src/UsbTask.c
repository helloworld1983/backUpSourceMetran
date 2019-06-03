/********************************************************************/
/*                                                                  */
/* File Name    : UsbTask.c			               		            */
/*                                                                  */
/* General       : Handle comunication with USB devices on port1	*/
/*				   (include Masstorage and Barcode device)			*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/06      Quyen Ngo		  	new file        */
/* #0001       2017/09/23      Quyen Ngo		  	modify			*/
/*		-remove call to meaningless fucntion						*/
/********************************************************************/

#include "TaskCommon.h"
#include "UsbMgr.h"
#include "UsbTask.h"
#include "HidBarcodeHost.h"
#include "GuiTask.h"
#include "UsbInterface.h"
#include "AnypalHandler.h"
#include "GuiInterface.h"

#define USB_TASK_DELAY 10/portTICK_PERIOD_MS
#define USB_TASK_SIZE 2*1024

QueueHandle_t USBHandleQueue;


/****************************************************************************************/
/*                                                                                      */
/* General 	-usbTask_Run															    */
/*                                                                                      */
/* Details  -Run function of Usb task									      			*/
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
static void usbTask_Run(void* loop)
{
	while(1)
	{
		usbMgr_Handle();
		vTaskDelay(USB_TASK_DELAY);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Create usb task														    */
/*                                                                                      */
/* Details  -Create usb task with														*/
/* 			 task name: USB Task
 * 			 task function: usbTask_Run
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
void usbTask_Create(void)
{
	/* Start Irda Task */
	xTaskCreate( usbTask_Run, "USB Task",
			USB_TASK_SIZE, (void *)1, TASK_PRIO_USB_TASK,
			( TaskHandle_t * ) NULL );
	return;
}



/****************************************************************************************/
/*                                                                                      */
/* Function name 	usbTask_Init													    */
/*                                                                                      */
/* Details  -init the usb task queue									 				*/
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
void usbTask_Init(void)
{
	USBHandleQueue = xQueueCreate(16, sizeof(USB_MSG_STRUCT_t));
	return;
}


