
#ifndef INC_TASKCOMMON_H_
#define INC_TASKCOMMON_H_
/********************************************************************/
/*                                                                  */
/* File Name    : TaskCommon.h                                	    */
/*                                                                  */
/* General       : This module contain variable and macro define 	*/
/* 				share between freeRTOS task							*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          December 22, 2016  Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#define PORTABLE_SW_VERSION "00-01-007"


#define TASK_PRIO_DEVICE_TASK 	  	(tskIDLE_PRIORITY + 5UL)
#define TASK_PRIO_ALARM_TASK 	  	(tskIDLE_PRIORITY + 4UL)
#define TASK_PRIO_AUDIO_TASK    	(tskIDLE_PRIORITY + 3UL)
#define TASK_PRIO_LOG_TASK        	(tskIDLE_PRIORITY + 2UL)
#define TASK_PRIO_USB_TASK			(tskIDLE_PRIORITY + 1UL)


#define DEVICE_TASK_DELAY_TIME 		5
#define USB_TASK_DELAY_TIME 		20
#define LOG_TASK_DELAY_TIME 		50
#define ALARM_TASK_DELAY_TIME 		50
#define AUDIO_TASK_DELAY_TIME 		50

#define BATTERY_DELAY_TIME 	1000

#define DEVICE_TASK_DELAY_TICK		DEVICE_TASK_DELAY_TIME 	/ portTICK_PERIOD_MS
#define USB_TASK_DELAY_TICK			USB_TASK_DELAY_TIME 	/ portTICK_PERIOD_MS
#define LOG_TASK_DELAY_TICK			LOG_TASK_DELAY_TIME 	/ portTICK_PERIOD_MS
#define ALARM_TASK_DELAY_TICK		ALARM_TASK_DELAY_TIME 	/ portTICK_PERIOD_MS
#define AUDIO_TASK_DELAY_TICK		AUDIO_TASK_DELAY_TIME 	/ portTICK_PERIOD_MS

#define DELAY_TICK_200MS 200 	/ portTICK_PERIOD_MS
#define DELAY_TICK_100MS 101 	/ portTICK_PERIOD_MS
#define DELAY_TICK_500MS 500 	/ portTICK_PERIOD_MS
#define DELAY_TICK_4S 4000 		/ portTICK_PERIOD_MS
#define DELAY_TICK_3S 3000 		/ portTICK_PERIOD_MS
#define DELAY_TICK_2S 2000 		/ portTICK_PERIOD_MS
#define DELAY_TICK_1S 1000		/ portTICK_PERIOD_MS
#define DELAY_TICK_60MS 60 		/ portTICK_PERIOD_MS
#define DELAY_TICK_40MS 40 		/ portTICK_PERIOD_MS
#define DELAY_TICK_30MS 30 		/ portTICK_PERIOD_MS
#define DELAY_TICK_25MS 25 		/ portTICK_PERIOD_MS
#define DELAY_TICK_20MS 20 		/ portTICK_PERIOD_MS
#define DELAY_TICK_10MS 10 		/ portTICK_PERIOD_MS
#define DELAY_TICK_3MS 3 		/ portTICK_PERIOD_MS
#define DELAY_TICK_5MS 5 		/ portTICK_PERIOD_MS
#define DELAY_TICK_1MS 1 		/ portTICK_PERIOD_MS

#define DEVICE_TASK_SIZE	(768)//(256*3)//128*2
#define AUDIO_TASK_SIZE		(256)//(256*2)//128*2-64
#define ALARM_TASK_SIZE 	(512)//(256*2)//128*2-64
#define USB_TASK_SIZE		(768)//(256*3)//128*3
#define LOG_TASK_SIZE		(512)//(256*3)//128*4-64




#define NUMBER_OF_LOG_QUEUE_SEND_FOR_PC_APP 10
#define LOG_QUEUE_RECEIVE_LENGTH 16
#define LOG_QUEUE_LENGTH 32
#define LOG_QUEUE_PRESSURE_LENGTH 2
#define AUDIO_QUEUE_LENGTH 8
#define ALARM_QUEUE_LENGTH 8
#define USB_QUEUE_LENGTH 8
#define BATTERY_QUEUE_LENGTH 8
#define DEVICE_QUEUE_LENGTH 32
//#define CHECK_REMAINING_STACK_SIZE

//#define DEBUG_TASK_EN
#ifdef DEBUG_TASK_EN
#define DEBUG_TASK(...) printf(__VA_ARGS__)
#else
#define DEBUG_TASK(...)
#endif


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C1_Read									      				    */
/*                                                                                      */
/* Details:        Read data from I2C1													*/
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														*/
/*              (O) uint8_t* buf														*/
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                        */
/*                                                                                      */
/****************************************************************************************/
//Status I2C1_Read(uint32_t addr, uint8_t* buf, uint32_t len);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C1_Read									      				     */
/*                                                                                      */
/* Details:        Read data from I2C1													 */
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														 */
/*              (O) uint8_t* buf														 */
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                     */
/*                                                                                      */
/****************************************************************************************/
//Status I2C0_Read(uint32_t addr, uint8_t* buf, uint32_t len);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	I2C0_ReadTimeout							      				    */
/*                                                                                      */
/* Details:        Read data from I2C0 with timeout										*/
/*                                                                                      */
/* Arguments:	(I)	uint32_t addr														*/
/*              (O) uint8_t* buf														*/
/*              (I) uint32_t len                                                        */
/* ReturnValue : Status			                                                        */
/*                                                                                      */
/****************************************************************************************/
//Status I2C0_ReadTimeout(uint32_t addr, uint8_t* buf, uint32_t len);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	WDT_init									      				    */
/*                                                                                      */
/* Details:        Initialize Watch dog timer											*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
//void WDT_init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	WDT_SystemReset								      				    */
/*                                                                                      */
/* Details:        Reset System using Watchdog timer									*/
/*                                                                                      */
/* Arguments:	(I)	None																*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
//void WDT_SystemReset(void);

#endif /* INC_TASKCOMMON_H_ */
