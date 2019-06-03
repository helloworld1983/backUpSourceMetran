#ifndef INC_OPERATIONSW_HANDLE_H_
#define INC_OPERATIONSW_HANDLE_H_

/********************************************************************/
/*                                                                  */
/* File Name    : OperationswHandle.h                               */
/*                                                                  */
/* General       : This module contain function for init and handle
 * 				  operation switch									*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jun 2, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/*																	*/
/*	#0001	   2017/09/07      Quyen Ngo(MV)		modify			*/
/*		-change the name of disable_opsw to correct naming rule		*/
/*	#0002	   2017/11/11      Quyen Ngo(MV)		modify			*/
/*		-Change the order of OPRATIONSW_EVENT enum for the default	*/
/*		value of the variable type OPRATIONSW_EVENT is IDLE			*/
/********************************************************************/

#include <Gpio.h>
#include <stdbool.h>
#include "TaskCommon.h"

/*Operatoin sw current state
	current_state = false: push
	current_state = true: release
 */
#define SW_PUSH true
#define SW_UNPUSH false

/* 2S holding sw count
 *  device_task call checking state func every 50ms
 *  50ms*40 = 2S
 */
#define COUNT_2S 2000/DEVICE_TASK_DELAY_TIME

/* 2.5S holding sw count
 *  device_task call checking state func every 50ms
 *  50ms*50 = 2.5S
 */
#define COUNT_2_5S 2500/DEVICE_TASK_DELAY_TIME

typedef enum
{
	IDLE,
	TOGGLE,
	TOGGLESHORT,
	TOGGLE2S
} OPRATIONSW_EVENT;

typedef struct {
	bool status; //Has request or no
	OPRATIONSW_EVENT event; //opsw event
} OPSW_POP_REQUEST_RETURN_T;

// Configure GPIO port for operation switch
void operationsw_config(void);

// Handle operation switch
void operationsw_Handle(void);

// disable operation switch
void operationsw_disable(void);

// Get status
bool operationsw_GetStatus(void);

void operationsw_DisablePush2S(void);


#endif /* INC_OPERATIONSW_HANDLE_H_ */
