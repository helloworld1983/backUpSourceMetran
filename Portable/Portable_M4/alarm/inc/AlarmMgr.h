
#ifndef ALARMMRG_HEADER_H
#define ALARMMRG_HEADER_H

/********************************************************************/
/*                                                                  */
/* File Name    : MrgAlarm.h                  	                    */
/*                                                                  */
/* General       : XXXX 		                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : XXXX                                             */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/02/09     Truong Nguyen(MV)  	   new file     */
/*                                                                  */
/********************************************************************/

#include <DualCoreCommon.h>
#include <TaskCommon.h>

#define ALM_TICK_PER_SEC 1000/ALARM_TASK_DELAY_TIME

#define ERROR_DIFFPRESS_CONST -24.215

/* prototype declarations */

//Initial Alarm
void alarmMgr_InitAlarm(void);

//Update Alarm monitor value for alarm task
void alarmMgr_UpdateAlarmMonitor(void);

//check all alarm trigger condition
void alarmMgr_CheckAllAlarm(void);

//update status for all of alarm
void alarmMgr_UpdateAllAlarm(void);



#endif
/* end of file */

