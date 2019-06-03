
#ifndef ALARMTASK_HEADER_H
#define ALARMTASK_HEADER_H

/********************************************************************/
/*                                                                  */
/* File Name    : AlarmTask.h                                    	*/
/*                                                                  */
/* General      : AlarmTask module define a RTOS task for check	    */
/*                and update alarm status					     	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/09     Truong Nguyen(MV)  	 new file       */
/*                                                                  */
/********************************************************************/

/* include system headers */
#include <TaskCommon.h>



/****************************************************************************************/
/*                                                                                      */
/* Function name： alarmTask_CreateAlarmTask				                              	*/
/*                                                                                      */
/* Details		：  Create a RTOS task             										*/
/*                                                                                      */
/****************************************************************************************/
void alarmTask_CreateTask(void);

//Get value of Alarm Task Timer
/****************************************************************************************/
/*                                                                                      */
/* Function name： alarmTask_GetAlarmTaskTimerValue				  	                	*/
/*                                                                                      */
/* Details		：  Get value of Alarm Task Timer (Get time run of Task)             		*/
/*                                                                                      */
/****************************************************************************************/
uint32_t alarmTask_GetAlarmTaskTimerValue();


#endif
/* end of file */

