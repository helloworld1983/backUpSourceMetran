#ifndef INC_ALARMINFOR_H_
#define INC_ALARMINFOR_H_

/********************************************************************/
/*                                                                  */
/* File Name    : AlarmInfor.h                            			*/
/*                                                                  */
/* General       : Displaying alarm infor in service and maintenance*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  	        new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "stdbool.h"

#define SIZE_BUFFER_NUMBER_ALARM 			3

//Init alarm infor
void alarmInfor_Init(void);
//Callback function for alarm infor
void alarmInfor_CallbackAlarmInfor(WM_MESSAGE * pMsg);
//Set number for alarm infor
void alarmInfor_SetInforAlarm(int numberAlarm);
//Show number alarm infor
void alarmInfor_ShowAlarmInfor(void);
//Hide number alarm infor
void alarmInfor_HideAlarmInfor(void);

#endif /* INC_ALARMINFOR_H_ */
/* end of file */
