/********************************************************************/
/*                                                                  */
/* File Name    : BluetoothTask.c                        			*/
/*                                                                  */
/* General      : Manager of bluetooth task							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/01/8       Linh Nguyen	  	    new file        */
/* #0001       2016/09/05      Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode:								*/ 
/*	- change BTTask_ProcessEvent and BTTask_SendEvent function		*/
/*	- Add BTTask_SendMgs function									*/
/*  - Remove use define                                             */
/* #0002       2016/09/20      Linh Nguyen	  	    modify          */
/*  Fix initialize g_RealDataQueue issue							*/ 
/* #0003       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/* #0004       2018/03/13       Quyen Ngo	  	    modify          */
/*  		Change code to call new function because the			*/
/*			BT reset sequence specification change					*/
/********************************************************************/
#include "Bluetooth.h"
#include "TaskCommon.h"
#include "BluetoothTask.h"
#include "RS232.h"
#include "CompositionMgr.h"
#include "CBX02Handler.h"
#include "DualCoreCommon.h"

#define BT_TASK_SIZE 256*8

static xQueueHandle gs_BTQueue;
xQueueHandle g_RealDataQueue;

/****************************************************************************************/
/*                                                                                      */
/* General 	-BTTask_ProcessEvent                        				    			*/
/*                                                                                      */
/* Details  -Process event data														    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool			                                                    	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static void BTTask_ProcessEvent(void)
{
	BT_MSG_t mgs;

	if(xQueueReceive(gs_BTQueue,&mgs,0) == pdTRUE)
	{
		switch(mgs.id)
		{
		case eUpdateCompositionId:
		{
			composition_Update();
			break;
		}
		case eDeleteCompositionId:
			composition_Delete();
			break;
			/** >>#0004
				Change code to call new function because the
				BT reset sequence specification change
			#0004 >>**/
		case eOnBluetoothId:
			DEBUGOUT("\n eOnBluetoothId");
			bluetooth_ChangeMode(DISCOVER_MODE);
			break;
		case eOffBluetoothId:
			DEBUGOUT("\n eOffBluetoothId");
			bluetooth_ChangeMode(UNDISCOVER_MODE);
			break;
		case eBeginGettingSpO2PulseDataId:
			cbx_SetGettingSpO2Data(true);
			break;
		case eEndGettingSpO2PulseDataId:
			cbx_SetGettingSpO2Data(false);
			break;
		case eOnBluetoothTranferId:
			DEBUGOUT("\n eOnBluetoothTranferId");
			cbx_TurnOnTransferMode(mgs.mgs);
			break;
		case eOffBluetoothTranferId:
			DEBUGOUT("\n eOffBluetoothTranferId");
			cbx_TurnOffTransferMode();
			break;
		default:
			break;
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： BTTask				                                                	*/
/*                                                                                      */
/* Details：     Handle all of Log task processing           								*/
/* Using Global Data : (I) 								                            	*/
/*                     (O)                                             					*/
/* local variables   : (I) 						            			              	*/
/*                                                                                      */
/****************************************************************************************/
static void BTTask(void)
{
	composition_Init();
	while(1)
	{
		BTTask_ProcessEvent();
#ifndef DEBUG
		rs322_ReadFromCBX();
#endif
		bluetooth_HandleResponse();
		vTaskDelay(BT_TASK_DELAY_TICK);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： BTTask_CreateBTTask				                              		*/
/*                                                                                      */
/* Details		：  Create a RTOS task             										*/
/*                                                                                      */
/****************************************************************************************/
void BTTask_CreateBTTask(void)
{
	xTaskCreate((TaskFunction_t)BTTask, "BLE TASK", BT_TASK_SIZE, (void *)1, TASK_PRIO_BT_TASK, ( TaskHandle_t * ) NULL);
	return;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name： BTTask_InitBTTask											    		*/
/*                                                                                      */
/* Details  -init the alarm task queue									 				*/
/*                                                                                      */
/* Using Global Data : (O) alarmQueue							                        */
/*                                                                                      */
/****************************************************************************************/
void BTTask_InitBTTask(void)
{
#ifndef DEBUG
	rs232_Init();
#endif
	bluetooth_Init();
	cbx_CreateTimer();
	gs_BTQueue = xQueueCreate(BT_QUEUE_LENGTH, sizeof(BT_MSG_t));
	g_RealDataQueue = xQueueCreate(BT_QUEUE_LENGTH, sizeof(MONITOR_DATA_t));
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-BTTask_SendEvent														    */
/*                                                                                      */
/* Details  -Send Event to BT task														*/
/*                                                                                      */
/* Arguments : (I) BTEvent event	 						  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool BTTask_SendEvent(BTEventId event)
{
	BT_MSG_t mgs;
	mgs.id = event;
	if(xQueueSend(gs_BTQueue, &mgs,QUEUE_WAITING) == pdFALSE)
	{
		DEBUGOUT("\n Failure send BT event");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-BTTask_SendMgs													    */
/*                                                                                      */
/* Details  -Send mgs to BT task														*/
/*                                                                                      */
/* Arguments : (I) BTEvent event	 						  							*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool BTTask_SendMgs(BT_MSG_t mgs)
{
	if(xQueueSend(gs_BTQueue, &mgs,QUEUE_WAITING) == pdFALSE)
	{
		DEBUGOUT("\n Failure send BT mgs");
		return false;
	}
	return true;
}
