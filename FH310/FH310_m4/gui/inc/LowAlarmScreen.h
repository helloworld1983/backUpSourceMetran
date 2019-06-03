#ifndef INC_LOWALARMSCREEN_H_
#define INC_LOWALARMSCREEN_H_

/********************************************************************/
/*                                                                  */
/* File Name    : LowAlarmScreen.h                                  */
/*                                                                  */
/* General      : Display low alarm screen			                */
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
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "WM.h"
#include "DualCoreCommon.h"

#define RECT_FR_NAME_COLOR_LOW_ALARM 		0xE6D2AA
#define COUNTER_CHANGE_TUBE          		3
#define BACKGROUND_SILENCE_LOW_BTN_COLOR 	0x745D2D
#define TEXT_COLOR_CANNULA				    0xB09865
#define COUNTER_CHILD_LOCK_LOW_ALARM		10

extern WM_HWIN g_lowAlarmScreen;

//Init high alarm screen
void lowAlarmScreen_Init(void);
//Show high alarm screen
void lowAlarmScreen_Show(E_AlarmId alarmId);
//Hide high alarm screen
void lowAlarmScreen_Hide(void);
//Update icon tube when cannula activate
void lowAlarmScreen_UpdateIconTube(void);
//Update animation when cannula activate
void lowAlarmScreen_UpdateCannulaAnimation(void);
//update animation when low alarm activate
void lowAlarmScreen_UpdateLowAnimation(void);
//Show anypal when alarm activate
void lowAlarmScreen_ShowAnypal(void);
//Show alarm anypal when alarm activate
void lowAlarmScreen_ShowAlarmAnypalWhenOtherAlarm(void);
//Show icon charging when alarm activate
void lowAlarmScreen_ShowAnypalCharging(void);
//Send flow rate to device
void lowAlarmScreen_SendFRToDevice(void);
//Change flow rate
void lowAlarmScreen_ChangeFlowRate(uint8_t FRPSAValue);
//Confirm flowrate in childlock mode
void lowAlarmScreen_ConfirmChildLock(uint8_t FRPSAValue);

#endif /* INC_LOWALARMSCREEN_H_ */
/* end of file */
