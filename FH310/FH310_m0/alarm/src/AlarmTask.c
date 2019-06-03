/********************************************************************/
/*                                                                  */
/* File Name    : AlarmTask.c                                    	*/
/*                                                                  */
/* General      : AlarmTask module define a RTOS task for check	    */
/*                and update alarm status					     	*/
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/09     Linh Nguyen(MV)  	    new file        */
/* #0002       2017/09/01     Linh Nguyen(MV)       modify			*/
/*		 stop increase time when it is full	         				*/
/********************************************************************/

/* include system files */
#include <AlarmMgr.h>
#include "stdint.h"


/* include user files */
#include "AlarmTask.h"

/* constants, macro definition */
#define ALARM_TASK_DELAY 50 / portTICK_PERIOD_MS
//#define Q_DEBUG
static const uint32_t xTaskDelay = ALARM_TASK_DELAY;
/* definitions of structures */

/*global variable*/

/* external declaration */
uint32_t g_alarmTaskTimer; //timer to counter running time
E_ModeSetting g_previousOperationMode;
E_ModeSetting g_currentOperationMode;


/* Body of functions */

/****************************************************************************************/
/*                                                                                      */
/* Function name： AlarmTask				                                                */
/*                                                                                      */
/* Details：     Handle all of Alarm task processing           								*/
/* Using Global Data : (I) g_previousOperationMode	                   		           	*/
/*                     (I) g_currentOperationMode                                       */
/*                     (O) g_alarmTaskTimer                                             */
/*                                                                                      */
/****************************************************************************************/
static void AlarmTask(void)
{
	g_alarmTaskTimer =0;
	alarmMgr_InitAlarm();
	while(1)
	{
		alarmMgr_UpdateAlarmMonitor();
		bool isOpeationModeChanged = (g_previousOperationMode != g_currentOperationMode);

		//clear all alarm when turn off device
		if (isOpeationModeChanged)
		{
			if(g_currentOperationMode == eStandyMode)
			{
				alarmMgr_ClearAllAlarm();
			}
		}
		alarmMgr_CheckAllAlarm();
		alarmMgr_UpdateAllAlarm();

		// Only check alrarm in Patint Mode and Service mode, don't need to check alarm in another mode
		if (((g_currentOperationMode == ePatientMode) || (g_currentOperationMode == eServiceMode))&&(g_alarmTaskTimer != UINT32_MAX))
		{
			g_alarmTaskTimer++;
		}
		else
		{
			g_alarmTaskTimer =0;
		}
		vTaskDelay(xTaskDelay);
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： alarmTask_CreateAlarmTask				                              	*/
/*                                                                                      */
/* Details		：  Create a RTOS task             										*/
/*                                                                                      */
/****************************************************************************************/
void alarmTask_CreateAlarmTask(void)
{
	//create alarm task with stack size 256, task priority +2
	xTaskCreate((TaskFunction_t)AlarmTask, "ALARM", 256, (void *)1, TASK_PRIO_ALARM_TASK, ( TaskHandle_t * ) NULL);
	return;
}

/* end of files */

