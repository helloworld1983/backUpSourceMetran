/******************************************************************************/
//$COMMON.H$
//   File Name:  TaskCommon.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains no class declaration.  It is a repository for
//      standard basic types and constants used throughout the system.
//
/******************************************************************************/
#ifndef INC_TASKCOMMON_H_
#define INC_TASKCOMMON_H_

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

//define priority for all task
#define TASK_PRIO_BD_TASK           (tskIDLE_PRIORITY + 4UL)
#define TASK_PRIO_ALARM_TASK        (tskIDLE_PRIORITY + 3UL)
#define TASK_PRIO_PD_TASK           (tskIDLE_PRIORITY + 4UL)
#define TASK_PRIO_IPC_TASK          (tskIDLE_PRIORITY + 1UL)
#define TASK_PRIO_CBIT_TASK         (tskIDLE_PRIORITY + 0UL)


//define delay time for all task
#define ALARM_TASK_DELAY_TIME       (20/portTICK_PERIOD_MS)
#define BD_TASK_DELAY_TIME          (20/portTICK_PERIOD_MS)
#define PD_TASK_DELAY_TIME          (0/portTICK_PERIOD_MS)
#define IPC_TASK_DELAY_TIME         (20/portTICK_PERIOD_MS)
#define CBIT_TASK_DELAY_TIME        (20/portTICK_PERIOD_MS)


//define stack size for each task
#define ALARM_TASK_SIZE             (configMINIMAL_STACK_SIZE * 32)
#define BD_TASK_SIZE                (configMINIMAL_STACK_SIZE * 24)
#define PD_TASK_SIZE                (configMINIMAL_STACK_SIZE * 12)
#define IPC_TASK_SIZE               (configMINIMAL_STACK_SIZE * 8)
#define CBIT_TASK_SIZE              (configMINIMAL_STACK_SIZE * 8)

#endif /* INC_TASKCOMMON_H_ */
