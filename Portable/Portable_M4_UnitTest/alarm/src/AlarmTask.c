/********************************************************************/
/*                                                                  */
/* File Name    : AlarmTask.c                                    	*/
/*                                                                  */
/* General      : AlarmTask module define a RTOS task for check	    */
/*                and update alarm status					     	*/
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/05     Truong Nguyen  	    			    */
/*                                                                  */
/********************************************************************/

#include <AlarmMgr.h>
#include <TaskCommon.h>
#include "AlarmTask.h"


uint32_t gs_alarmTaskTimer; //timer to counter running time


/****************************************************************************************/
/*                                                                                      */
/* Function name： AlarmTask				                                                */
/*                                                                                      */
/* Details：     Handle all of Alarm task processing           								*/
/* Using Global Data : (I) g_xxxXxxxxxx.aaAaaa.bbbBbbb                              	*/
/*                     (O) gs_alarmTaskTimer                                             */
/*                                                                                      */
/****************************************************************************************/
static void AlarmTask_Loop(void *pvParameters)
{
	gs_alarmTaskTimer = 0;
	alarmMgr_InitAlarm();
#ifdef CHECK_REMAINING_STACK_SIZE
	UBaseType_t uxHighWaterMark;
	uint32_t cnt = 0;
#endif
	while(1)
	{
//		alarmMgr_UpdateAlarmMonitor();
//		alarmMgr_CheckAllAlarm();
//		alarmMgr_UpdateAllAlarm();
		if (gs_alarmTaskTimer <= ALM_TICK_PER_SEC*60*5)//1s*60*5 = 5Mins
			gs_alarmTaskTimer++;

#ifdef CHECK_REMAINING_STACK_SIZE
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		if(cnt>=1000/ALARM_TASK_DELAY_TIME)
		{
			DEBUGOUT("Alarm Task %d\n",uxHighWaterMark);
			cnt = 0;
		}
		else
		{
			cnt++;
		}
#endif
		vTaskDelay(ALARM_TASK_DELAY_TICK);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name： alarmTask_GetAlarmTaskTimerValue				  	                	*/
/*                                                                                      */
/* Details		：  Get time run of Task             										*/
/*                                                                                      */
/****************************************************************************************/
uint32_t alarmTask_GetAlarmTaskTimerValue()
{
	return gs_alarmTaskTimer;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name： alarmTask_CreateAlarmTask				                              	*/
/*                                                                                      */
/* Details		：  Create a RTOS task             										*/
/*                                                                                      */
/****************************************************************************************/
void alarmTask_CreateTask(void)
{
	xTaskCreate(AlarmTask_Loop, "ALARM task", ALARM_TASK_SIZE, (void *)1, TASK_PRIO_ALARM_TASK, ( TaskHandle_t * ) NULL);
}


/* end of files */

