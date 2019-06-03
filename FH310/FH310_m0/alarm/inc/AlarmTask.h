
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
/* Product Name  : FH-310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/09     Linh Nguyen(MV)  	    new file        */
/*                                                                  */
/********************************************************************/

/* include system headers */



/* include user headers */
#include "TaskCommon.h"

/* constants, definitions */

extern uint32_t g_alarmTaskTimer; //timer to counter running time
extern E_ModeSetting g_previousOperationMode;
extern E_ModeSetting g_currentOperationMode;

/* definietion of structures */


/* prototype declarations */

//initialization alarm task
void alarmTask_InitAlarmTask(void);

//Create a RTOS task
void alarmTask_CreateAlarmTask(void);

#endif
/* end of file */

