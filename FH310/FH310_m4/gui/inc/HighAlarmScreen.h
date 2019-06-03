#ifndef INC_HIGHALARMSCREEN_H_
#define INC_HIGHALARMSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : HighAlarmScreen.h                                 */
/*                                                                  */
/* General      : Display high alarm screen			                */
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       2017/11/17       Viet Le 			modify			*/
/*      - Add define  TIMER_BLINKING 2 seconds(bug 2020)            */
/* #0002       2017/11/17       Viet Le 			modify			*/
/*	- Remove highAlarmScreen_SetExtFlowPath() (Coverity)			*/
/********************************************************************/

#include "WM.h"

#include "DualCoreCommon.h"

#define BACKGROUND_HIGH_ALARM 			0x0947E9
#define BORDER_HIGH_ALARM_COLOR 		0x966B00
#define CENTER_TEXT_COLOR 				0xBE9500
#define SIZE_BUFFER_NUMBER_ERROR		3
#define TIMER_BLINKING	 				2000//2000ms

extern WM_HWIN g_highAlarmScreen;

//Init high alarm screen
void highAlarmScreen_Init(void);
//Show high alarm screen
void highAlarmScreen_Show(E_AlarmId alarmId);
//Hide high alarm screen
void highAlarmScreen_Hide(void);
//Update o2 cylinder
void highAlarmScreen_UpdateO2Cylinder(void);
//Hide button o2 cylinder
void highAlarmScreen_HideO2Cylinder(void);

#endif /* INC_HIGHALARMSCREEN_H_ */
/* end of file */
