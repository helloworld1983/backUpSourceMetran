#ifndef INC_TASKCOMMON_H_
	#define INC_TASKCOMMON_H_
/********************************************************************/
/*                                                                  */
/* File Name    : TaskCommon.h                                	    */
/*                                                                  */
/* General       : This module contain variable and macro define share
 * 					between freeRTOS task							*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m4                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jun 21, 2016     Quyen Ngo(MV)  		new file        */
/* #0001       2016/09/15      Linh Nguyen	  	    modify          */
/*  Change BT task cycle time										*/
/* #0002       2016/09/28      Linh Nguyen	  	    modify          */
/*  Change log queue leng											*/
/* 		       2016/10/19      Linh Nguyen	  	    modify          */
/*  Change log queue leng (bug # 1982)								*/
/********************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "UsbCommonDef.h"

/* higher the number higher the priority */
#define TASK_PRIO_IPC_DISPATCH    			  (tskIDLE_PRIORITY + 5UL)		//50ms
#define TASK_PRIO_GUI_TASK 	  	  			  (tskIDLE_PRIORITY + 1UL)		//20ms
#define TASK_PRIO_IRDA_TASK		              (tskIDLE_PRIORITY + 3UL)		//event driven
#define TASK_PRIO_USB_TASK		              (tskIDLE_PRIORITY + 2UL)		//10ms
#define TASK_PRIO_USB_HOST_TASK	              (tskIDLE_PRIORITY + 0UL)		//0ms
#define TASK_PRIO_FTDI_ANYPAL 	              (tskIDLE_PRIORITY + 3UL)	//10ms

#define TASK_PRIO_LOG_TASK        (tskIDLE_PRIORITY + 4UL)		//50ms

#define TASK_PRIO_BT_TASK        (tskIDLE_PRIORITY + 3UL)		//500ms

#define BT_TASK_DELAY_TIME_MS	50
#define BT_TASK_DELAY_TICK BT_TASK_DELAY_TIME_MS / portTICK_PERIOD_MS

#define IPC_SEND_QUEUE_LENGTH 8
#define IRDA_QUEUE_LENGTH 8
#define LOG_QUEUE_RECEIVE_LENGTH 	256
#define LOG_QUEUE_REQUEST_LENGTH 	16
#define LOG_QUEUE_SEND_LENGTH 		2
#define BARCODE_QUEUE_LENGTH 8
#define BT_QUEUE_LENGTH 8
#define USB_QUEUE_LENGTH 4
#define M4_IPC_QUEUE_LENGTH 8
#define ANYPAL_QUEUE_LENGTH 8
#define BT_QUEUE_LENGTH 8

typedef enum
{
	E_NONE_EVENT_ID,        //Idle
	E_TURN_ON_ID,			// remote turn on
	E_TURN_OFF_ID,		// remote turn off
	E_TURN_UP_ID,		// increase flow rate
	E_TURN_DOWN_ID,	// decrease flow rate
	E_PRESET1_ID,			//preset 1
	E_PRESET2_ID,			//preset 2
	E_PRESET3_ID,		//preset 3
	E_CONFIRM_ID			//confirmed button pressed
} E_IrdaEventId;

typedef enum
{
	E_USB_NONE_EVENT_ID,
	E_USB_ATTACH_ID,
	E_USB_ENUM_COMPLETE_ID,
	E_USB_ENUM_FAIL_ID,
	E_USB_DEATTACH_ID
} E_UsbEventId;

typedef struct {
	uint8_t id; //id of barcode serial packet Fix 0x00
	uint8_t length; // lenght of barcode string
	uint8_t barcodeString[BARCODE_MAXLENGTH]; //barcode string
} BARCODE_SERIAL_PACKET_T;

typedef struct {
	uint16_t majorNum; //major number of version
	uint16_t minorNum; //minor number of version
	uint16_t patchNum; //patch number of version
	uint16_t buildNum; //build number of version
} BT_FW_VERSION_T;

// Global Queue Handle
extern SemaphoreHandle_t g_irdaBinarySem;
extern QueueHandle_t g_m4GuiIpcMsgQueue;

#endif /* INC_TASKCOMMON_H_ */
