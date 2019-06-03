#ifndef INC_GUIHANDLER_H_
#define INC_GUIHANDLER_H_

/********************************************************************/
/*                                                                  */
/* File Name    :  GuiHandler.h                 		            */
/*                                                                  */
/* General       : Handler timer in gui 			 		        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2008/08/22      Viet Le  			new file       	*/
/* #0001       2017/09/27      Viet Le              modify			*/
/*    Add : #define TIMER_ENABLE_IRDA 17 and 						*/
/*			guihandler_StartTimerEnableIrda() function (bug 1918)	*/
/* #0002       2017/11/10      Linh Nguyen          modify			*/
/*    Add : #define TIMER_UPDATE_RTC 18 	 						*/
/* #0003       2018/02/26      Viet Le              modify			*/
/*	  Add three function : Update animation in operation, Play 		*/
/* childlock in operation, guihandler_PlayChildlockAlarm(bug 2065)	*/
/********************************************************************/

#include "board.h"
#include "GuiTask.h"

#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#define TIMER_UPDATE_ANIMATION_ALARM 		0
#define TIMER_PLAY_CHILD_LOCK 				1
#define TIMER_PLAY_CHILD_LOCK_ALARM 		2
#define TIMER_UPDATE_CUMULATIVE_HOUR 		4
#define TIMER_UPDATE_ANIMATION 				5
#define TIMER_UPDATE_EQUIPMENT_INFOR 		6
#define TIMER_SEND_FR_TO_DEVICE 			7
#define TIMER_PLAY_HIGH_ALARM 				11
#define TIMER_PLAY_MEDIUM_ALARM 			12
#define TIMER_CHECK_RTC 					13
#define TIMER_CHARGING_ANYPAL 				14
#define TIMER_CHECK_DEBUG_LOG				16
#define TIMER_ENABLE_IRDA					17
#define TIMER_UPDATE_RTC					18
#define COUNTER_3S 							6
#define COUNTER_CHILD_LOCK					1
#define TICK_TO_WAIT 						10

void guihandler_CreateTimer(void);

void guihandler_StartTimerAnimationAlarm(void);
void guihandler_StopTimerAnimationAlarm(void);

void guihandler_StartTimerAnimation(void);
void guihandler_StopTimerAnimation(void);

void guihandler_StartTimerPlayChildLock(void);
void guihandler_StopTimerPlayChildLock(void);

void guihandler_StartTimerPlayChildLockAlarm(void);
void guihandler_StopTimerPlayChildLockAlarm(void);

void guihandler_StartTimerUpdateCumulative(void);
void guihandler_StopTimerUpdateCumulative(void);

void guihandler_StartTimerUpdateEquipInfor(void);
void guihandler_StopTimerUpdateEquipInfor(void);

void guihandler_StartTimerSendFRToDevice(void);
void guihandler_StopTimerSendFRToDevice(void);

void guihandler_StartTimerShowHideChildLockAlarm(void);
void guihandler_StopTimerShowHideChildLockAlarm(void);

void guihandler_StartTimerCheckRTC(void);
void guihandler_StopTimerCheckRTC(void);

void guihandler_StartTimerChargingAnypal(void);
void guihandler_StopTimerChargingAnypal(void);

void guihandler_StartTimerCheckDebugLog(void);
void guihandler_StopTimerCheckDebugLog(void);
bool guihandler_GetActiveTimerCheckDebugLog(void);

void guihandler_StartTimerPlayHighAlarm(void);
void guihandler_StopTimerPlayHighAlarm(void);

void guihandler_StartTimerPlayMediumAlarm(void);
void guihandler_StopTimerPlayMediumAlarm(void);

void guihandler_StartTimerEnableIrda(void);

void guihandler_StartRTCUpdateTimer(void);

/** >>#0003
	 Add function update animation in operation
#0003 >>**/
void guihandler_UpdateAnimationOperation(void);

/** >>#0003
	 Add function Play childlock in operation
#0003 >>**/
void guihandler_PlayChildlockOperation(void);

/** >>#0003
	 Add function Play childlock in alarm
#0003 >>**/
void guihandler_PlayChildlockAlarm(void);

#endif /* INC_GUIHANDLER_H_ */
/* end of file */
