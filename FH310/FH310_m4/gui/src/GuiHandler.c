/********************************************************************/
/*                                                                  */
/* File Name    :  GuiHandler.c                 		            */
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
/* #0001       2017/08/25	   Viet Le				modify 			*/
/*             Change AnypalMgr to AnypalDisplayHandler             */
/* #0002       2017/09/05	   Viet Le				modify 			*/
/*             remove #include <assert.h>							*/
/* #0003       2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/* #0004       2017/09/27      Viet Le              modify			*/
/*     Add : TimerHandle_t s_GUITimerEnableIrda,					*/
/*			guihandler_StartTimerEnableIrda() function. 			*/
/*			Enable irda flag in guihandler_TimerCallback (bug 1918) */
/* #0005       2017/10/03      Viet Le              modify			*/
/*	  Call realtimeclock_SetRtcLow(void) in guihandler_TimerCallback*/
/*     set pin RTC to low (bug 1930)								*/
/* #0006       2017/11/10      Linh Nguyen	         modify			*/
/*	  Add and config timer s_RTCUpdateTimer							*/
/* #0007       2017/11/16      Viet Le	             modify			*/
/*	  - Add s_counterChildlock = 0, when play child lock 			*/
/*  s_counterChildlock++. When start timer child lock reset 		*/
/*	s_counterChildlock = 0	(bug 1997)								*/
/* #0008       2017/02/26      Viet Le	             modify			*/
/*	 - Send event enter opration mode, update animation, update		*/
/* calendar and cumulative hour, hide icon charging anypal to  		*/
/*  GUI task. Add three function : Update animation in operation,	*/
/*  Play childlock in operation, Play childlock in alarm(bug 2065) 	*/
/********************************************************************/

#include <stdlib.h>
#include "board.h"
#include "GuiHandler.h"
#include "timers.h"
#include "TaskCommon.h"
#include "queue.h"
#include "setting.h"
#include "AudioMgr.h"
#include "OperationScreen.h"
#include "ModeMgr.h"
#include "EquipmentInforScreen.h"
#include "EquipmentCumulativeHourScreen.h"
#include "AlarmScreenMgr.h"
#include "LowAlarmScreen.h"
#include "StandbyScreen.h"
#include "RealTimeClock.h"
#include "DebugLog.h"
#include "AnyPalDisplayHandler.h"
#include "GuiInterface.h"

static TimerHandle_t s_GuiTimerAnimationAlarm;
static TimerHandle_t s_GuiTimerPlayChildLock;
static TimerHandle_t s_GuiTimerPlayChildLockAlarm;
static TimerHandle_t s_GuiTimerUpdateCumulativeHour;
static TimerHandle_t s_GuiTimerAnimation;
static TimerHandle_t s_GUiTimerUpdateEquipInfor;
static TimerHandle_t s_GUITimerSendFRToDevice;
static TimerHandle_t s_GUITimerCheckRTC;
static TimerHandle_t s_GUITimerChargingAnypal;
static TimerHandle_t s_GUITimerCheckDebugLog;
static TimerHandle_t s_GUITimerPlayHighAlarm;
static TimerHandle_t s_GUITimerPlayMediumAlarm;
static TimerHandle_t s_GUITimerEnableIrda;
static TimerHandle_t s_RTCUpdateTimer;
static int s_couter3s = 0;
static int s_counterChildlock = 0;

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_TimerCallback 								       				*/
/*                                                                                      */
/* Details  -Call back function for timer 												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) TimerHandle_t xTimer											 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_couter3s : counter 3 seconds after turn on FH310           */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
static void guihandler_TimerCallback( TimerHandle_t xTimer)
{
	uint32_t timerId = ( uint32_t ) pvTimerGetTimerID( xTimer );
	switch (timerId) {
	case TIMER_UPDATE_ANIMATION_ALARM:
	{
		/** >>#0008
			Remove update directly animation in alarm screen.
			Send event update animation in alarm to GUI task
		#0008 >>**/
		GUI_EVENT_STRUCT event;
		event.id = eUpdateAnimationAlarmEventId;
		guiInterface_SendGuiEvent(event);
		break;
	}
	case TIMER_UPDATE_ANIMATION:
	{
		/** >>#0008
			Remove update directly animation in operation screen.
			Send event update animation in operation screen to GUI task
		#0008 >>**/
		GUI_EVENT_STRUCT event;
		event.id = eUpdateAnimationOperationEventId;
		guiInterface_SendGuiEvent(event);
		break;
	}
	case TIMER_PLAY_CHILD_LOCK:
	{
		/** >>#0008
			Remove check isvisible operation and move code to guihandler_PlayChildlockOperation function.
		#0008 >>**/
		guihandler_PlayChildlockOperation();
		break;
	}
	case TIMER_PLAY_CHILD_LOCK_ALARM:
	{
		/** >>#0008
			Moce code to guihandler_PlayChildlockAlarm function.
		#0008 >>**/
		guihandler_PlayChildlockAlarm();
		break;
	}
	case TIMER_UPDATE_CUMULATIVE_HOUR:
		operationScreen_HandlerCumulativeHour();
		break;
	case TIMER_UPDATE_EQUIPMENT_INFOR:
	{
		/** >>#0008
			Remove update calendar and cumulative hour directly.
			Send event update calendar and cumulative hour to GUI task
		#0008 >>**/
		GUI_EVENT_STRUCT event;
		event.id = eUpdateCalenderCumulativeHoursEventId;
		guiInterface_SendGuiEvent(event);
		break;
	}
	case TIMER_SEND_FR_TO_DEVICE:
	{
		/** >>#0008
			Remove send flow rate to devive directly.
			Send event send flow rate to GUI task
		#0008 >>**/
		GUI_EVENT_STRUCT event;
		event.id = eSendFlowRateToDeviceEventId;
		guiInterface_SendGuiEvent(event);
		break;
	}
	case TIMER_CHECK_RTC:
		realtimeclock_SetRtcLow();
		break;
	case TIMER_CHARGING_ANYPAL:
	{
		/** >>#0008
			Remove hide icon charging directly.
			Send event hide icon charging to GUI task
		#0008 >>**/
		GUI_EVENT_STRUCT event;
		event.id = eHideIconChargingAnypalEventId;
		guiInterface_SendGuiEvent(event);
		break;
	}
	case TIMER_CHECK_DEBUG_LOG:
		debugLog_CheckConditionDebugLog(guiTask_GetRealMonitorData());
		break;
	case TIMER_PLAY_HIGH_ALARM:
		audioMgr_PlayHighAlarm();
		break;
	case TIMER_PLAY_MEDIUM_ALARM:
		audioMgr_PlayMediumAlarm();
		break;
	case TIMER_ENABLE_IRDA:
		guiTask_EnableIrda(true);
		break;
	case TIMER_UPDATE_RTC:
	{
		Real_Time_Clock rtc;
		realtimeclock_gettime(&rtc);
		guiInterface_setCurrentTime(rtc);
		break;
	}
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_CreateTimer 								       				*/
/*                                                                                      */
/* Details  -Create all timer															*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE											 						*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerAnimationAlarm : timer update animation in alarm   */
/*                     (O) s_GuiTimerAnimation : timer update animation in operation	*/
/*                     (O) s_GuiTimerPlayChildLock : timer play child lock in operation	*/
/*                     (O) s_GuiTimerPlayChildLockAlarm : timer play child lock in alarm*/
/*                     (O) s_GuiTimerUpdateCumulativeHour : timer update cumulative hour*/
/*                     (O) s_GUiTimerUpdateEquipInfor : timer update data in equipment	*/
/*                     (O) s_GUITimerSendFRToDevice : timer send flow to device 		*/
/*                     (O) s_GUITimerCheckRTC : timer test RTC					 		*/
/*                     (O) s_GUITimerChargingAnypal : timer charging anypal		 		*/
/*                     (O) s_GUITimerCheckDebugLog : timer check condition debug log	*/
/*                     (O) s_GUITimerPlayHighAlarm : timer play high alarm		 		*/
/*                     (O) s_GUITimerPlayMediumAlarm : timer play medium alarm		 	*/
/*                     (O) s_GUITimerChargingAnypal : timer charging anypal		 		*/
/*                     (O) s_GUITimerChargingAnypal : timer charging anypal		 		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_CreateTimer(void)
{
	s_GuiTimerAnimationAlarm = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Anim alarm",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pdMS_TO_TICKS(500),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) TIMER_UPDATE_ANIMATION_ALARM,
					/* Each timer calls the same callback when
			                      it expires. */
					guihandler_TimerCallback
			);

	s_GuiTimerAnimation = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"Animmation",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(500),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) TIMER_UPDATE_ANIMATION,
					/* Each timer calls the same callback when
				                      it expires. */
					guihandler_TimerCallback
			);

	s_GuiTimerPlayChildLock = xTimerCreate
			( /* Just a text name, not used by the RTOS
				                      kernel. */
					"Child lock",
					/* The timer period in ticks, must be
				                      greater than 0. */
					pdMS_TO_TICKS(100),
					/* The timers will auto-reload themselves
				                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
				                      number of times the timer has expired, which
				                      is initialised to 0. */
					( void * ) TIMER_PLAY_CHILD_LOCK,
					/* Each timer calls the same callback when
				                      it expires. */
					guihandler_TimerCallback
			);

	s_GuiTimerPlayChildLockAlarm = xTimerCreate
			( /* Just a text name, not used by the RTOS
					                      kernel. */
					"Child lock alarm",
					/* The timer period in ticks, must be
					                      greater than 0. */
					pdMS_TO_TICKS(100),
					/* The timers will auto-reload themselves
					                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
					                      number of times the timer has expired, which
					                      is initialised to 0. */
					( void * ) TIMER_PLAY_CHILD_LOCK_ALARM,
					/* Each timer calls the same callback when
					                      it expires. */
					guihandler_TimerCallback
			);

	s_GuiTimerUpdateCumulativeHour = xTimerCreate
			( /* Just a text name, not used by the RTOS
					                      kernel. */
					"Update time",
					/* The timer period in ticks, must be
					                      greater than 0. */
					pdMS_TO_TICKS(1000),
					/* The timers will auto-reload themselves
					                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
					                      number of times the timer has expired, which
					                      is initialised to 0. */
					( void * ) TIMER_UPDATE_CUMULATIVE_HOUR,
					/* Each timer calls the same callback when
					                      it expires. */
					guihandler_TimerCallback
			);
	s_GUiTimerUpdateEquipInfor = xTimerCreate
			( /* Just a text name, not used by the RTOS
						                      kernel. */
					"Update EQ",
					/* The timer period in ticks, must be
						                      greater than 0. */
					pdMS_TO_TICKS(500),
					/* The timers will auto-reload themselves
						                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
						                      number of times the timer has expired, which
						                      is initialised to 0. */
					( void * ) TIMER_UPDATE_EQUIPMENT_INFOR,
					/* Each timer calls the same callback when
						                      it expires. */
					guihandler_TimerCallback
			);
	s_GUITimerSendFRToDevice = xTimerCreate
			( /* Just a text name, not used by the RTOS
							                      kernel. */
					"Send FR",
					/* The timer period in ticks, must be
							                      greater than 0. */
					pdMS_TO_TICKS(1000),
					/* The timers will auto-reload themselves
							                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
							                      number of times the timer has expired, which
							                      is initialised to 0. */
					( void * ) TIMER_SEND_FR_TO_DEVICE,
					/* Each timer calls the same callback when
							                      it expires. */
					guihandler_TimerCallback
			);

	s_GUITimerCheckRTC = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"Check rtc",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(50),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_CHECK_RTC,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);

	s_GUITimerChargingAnypal = xTimerCreate
			( /* Just a text name, not used by the RTOS
													                      kernel. */
					"Charge ATP",
					/* The timer period in ticks, must be
													                      greater than 0. */
					pdMS_TO_TICKS(1000),
					/* The timers will auto-reload themselves
													                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
													                      number of times the timer has expired, which
													                      is initialised to 0. */
					( void * ) TIMER_CHARGING_ANYPAL,
					/* Each timer calls the same callback when
													                      it expires. */
					guihandler_TimerCallback
			);

	s_GUITimerCheckDebugLog = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"Check log",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(1000),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_CHECK_DEBUG_LOG,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);

	s_GUITimerPlayHighAlarm = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"Play hi",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(10),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_PLAY_HIGH_ALARM,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);

	s_GUITimerPlayMediumAlarm = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"Play Med",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(10),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_PLAY_MEDIUM_ALARM,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);


	s_GUITimerEnableIrda = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"EnIrda",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(2500),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_ENABLE_IRDA,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);

	s_RTCUpdateTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
												                      kernel. */
					"RTCUpdate",
					/* The timer period in ticks, must be
												                      greater than 0. */
					pdMS_TO_TICKS(1000),
					/* The timers will auto-reload themselves
												                      when they expire. */
					pdTRUE,
					/* The ID is used to store a count of the
												                      number of times the timer has expired, which
												                      is initialised to 0. */
					( void * ) TIMER_UPDATE_RTC,
					/* Each timer calls the same callback when
												                      it expires. */
					guihandler_TimerCallback
			);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerAnimationAlarm 								        */
/*                                                                                      */
/* Details  -Start timer update animationin alarm										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerAnimationAlarm : timer update animation in alarm   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerAnimationAlarm(void)
{
	if (xTimerStart(s_GuiTimerAnimationAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GuiTimerAnimationAlarm \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerAnimationAlarm 								        */
/*                                                                                      */
/* Details  -Stop timer	update animation in alarm										*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerAnimationAlarm : timer update animation in alarm   */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerAnimationAlarm(void)
{
	if (xTimerStop(s_GuiTimerAnimationAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GuiTimerAnimationAlarm \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerAnimation 								    	    */
/*                                                                                      */
/* Details  -Start timer update animation in operation									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerAnimation : timer update animation in operation    */
/*                     (O) s_couter3s : counter 3 seconds after turn on FH310			*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerAnimation(void)
{
	if (xTimerStart(s_GuiTimerAnimation,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GuiTimerAnimation \n");
	}
	s_couter3s = 0;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerAnimation		 								        */
/*                                                                                      */
/* Details  -Stop timer	update animation in operation									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerAnimation : timer update animation in operation    */
/*                     (O) s_couter3s : connter 3 seconds after turn on FH310           */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerAnimation(void)
{
	if (xTimerStop(s_GuiTimerAnimation,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GuiTimerAnimation \n");
	}

	s_couter3s = 0;

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerPlayChildLock 								        */
/*                                                                                      */
/* Details  -Start timer play buzzer child lock in operation							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerPlayChildLock : timer play buzzer in child lock    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerPlayChildLock(void)
{
	s_counterChildlock = 0;
	if (xTimerStart(s_GuiTimerPlayChildLock,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GuiTimerPlayChildLock \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerPlayChildLock 								        	*/
/*                                                                                      */
/* Details  -Stop timer	play buzzer child lock in operation		     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerPlayChildLock : timer play buzzer in childlock     */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerPlayChildLock(void)
{
	if (xTimerStop(s_GuiTimerPlayChildLock,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GuiTimerPlayChildLock \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerPlayChildLockAlarm 								    */
/*                                                                                      */
/* Details  -Start timer play buzzer childlock in alarm									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerPlayChildLockAlarm : timer play buzzer in childlock*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerPlayChildLockAlarm(void)
{
	s_counterChildlock = 0;
	if (xTimerStart(s_GuiTimerPlayChildLockAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GuiTimerPlayChildLockAlarm \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerPlayChildLockAlarm 								    */
/*                                                                                      */
/* Details  -Stop timer	play buzzer childlock in alarm			     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerPlayChildLockAlarm : timer play buzzer in childlock*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerPlayChildLockAlarm(void)
{
	if (xTimerStop(s_GuiTimerPlayChildLockAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GuiTimerPlayChildLockAlarm \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerUpdateCumulative 								        */
/*                                                                                      */
/* Details  -Start timer update cumulative hour				     						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerUpdateCumulativeHour : timer update cumulative hour*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerUpdateCumulative(void)
{
	if (xTimerStart(s_GuiTimerUpdateCumulativeHour,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GuiTimerUpdateCumulativeHour \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerUpdateCumulative 								        */
/*                                                                                      */
/* Details  -Stop timer	update cumulative hour				     						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GuiTimerUpdateCumulativeHour : timer update cumulative hour*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerUpdateCumulative(void)
{
	if( xTimerIsTimerActive( s_GuiTimerUpdateCumulativeHour ) != pdFALSE )
	{
		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eOperationStopLogId);

		// xTimer is active, do something.
		if (xTimerStop(s_GuiTimerUpdateCumulativeHour,TICK_TO_WAIT) != pdPASS) {
			DEBUGOUT("ERR: Failed to stop s_GuiTimerUpdateCumulativeHour \n");
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerUpdateEquipInfor 								        */
/*                                                                                      */
/* Details  -Start timer update data in equipment				     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUiTimerUpdateEquipInfor : timer update data in equipment  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerUpdateEquipInfor(void)
{
	if (xTimerStart(s_GUiTimerUpdateEquipInfor,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUiTimerUpdateEquipInfor \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerUpdateEquipInfor 								        */
/*                                                                                      */
/* Details  -Stop timer	update data in equipment				     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUiTimerUpdateEquipInfor : timer update data in equipment  */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerUpdateEquipInfor(void)
{
	if (xTimerStop(s_GUiTimerUpdateEquipInfor,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUiTimerUpdateEquipInfor \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerSendFRToDevice 								        */
/*                                                                                      */
/* Details  -Start timer send flow rate to device after change flow rate by encoder or 	*/
/*					remote																*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerSendFRToDevice : timer send flow to device         */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerSendFRToDevice(void)
{
	if (xTimerStart(s_GUITimerSendFRToDevice,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerSendFRToDevice \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerSendFRToDevice 								        */
/*                                                                                      */
/* Details  -Stop timer	send flow rate to device after change flow rate by encoder or	*/
/*					remote																*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerSendFRToDevice : timer send flow rate to device    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerSendFRToDevice(void)
{
	if( xTimerIsTimerActive( s_GUITimerSendFRToDevice ) != pdFALSE )
	{
		if(guiTask_GetFlowrate() != guiTask_GetPenddingFlowRate())
		{
			guiTask_SetFowRate(guiTask_GetPenddingFlowRate());
		}

		// xTimer is active, do something.
		if (xTimerStop(s_GUITimerSendFRToDevice,TICK_TO_WAIT) != pdPASS) {
			DEBUGOUT("ERR: Failed to stop s_GUITimerSendFRToDevice \n");
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerCheckRTC 												*/
/*                                                                                      */
/* Details  -Start timer test RTC in development				     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerCheckRTC : timer test RTC                          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerCheckRTC(void)
{
	if (xTimerStart(s_GUITimerCheckRTC,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerCheckRTC \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerCheckRTC 												*/
/*                                                                                      */
/* Details  -Stop timer	test RTC				     									*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerCheckRTC : timer test RTC      				   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerCheckRTC(void)
{
	if (xTimerStop(s_GUITimerCheckRTC,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUITimerCheckRTC \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerChargingAnypal 										*/
/*                                                                                      */
/* Details  -Start timer charging anypal				     							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerChargingAnypal : timer charging anypal     		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerChargingAnypal(void)
{
	if (xTimerStart(s_GUITimerChargingAnypal,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerChargingAnypal \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerChargingAnypal 									    */
/*                                                                                      */
/* Details  -Stop timer	charging anypal				     								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerChargingAnypal : timer charging anypal             */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerChargingAnypal(void)
{
	if (xTimerStop(s_GUITimerChargingAnypal,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUITimerChargingAnypal \n");
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerCheckDebugLog 									    */
/*                                                                                      */
/* Details  -Start timer check condition debug log					     				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerCheckDebugLog : timer check condition debug log    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerCheckDebugLog(void)
{
	if (xTimerStart(s_GUITimerCheckDebugLog,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerCheckDebugLog \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerCheckDebugLog 									    	*/
/*                                                                                      */
/* Details  -Stop timer	check condition debug log				     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerCheckDebugLog : timer check condition debug log    */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerCheckDebugLog(void)
{
	if (xTimerStop(s_GUITimerCheckDebugLog,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUITimerCheckDebugLog \n");
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_GetActiveTimerCheckDebugLog 									*/
/*                                                                                      */
/* Details  -This function assumes xTimer has already been created.				     	*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool : true is active, false is not active 		                    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerCheckDebugLog : timer check debug log              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool guihandler_GetActiveTimerCheckDebugLog(void)
{
	bool result = false;
	if( xTimerIsTimerActive( s_GUITimerCheckDebugLog ) != pdFALSE )
	{
		// xTimer is active, do something.
		result = true;
	}

	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerPlayHighAlarm 										*/
/*                                                                                      */
/* Details  -Start timer play high alarm				     							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerPlayHighAlarm : timer play high alarm              */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerPlayHighAlarm(void)
{
	if (xTimerStart(s_GUITimerPlayHighAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerPlayHighAlarm \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerPlayHighAlarm 											*/
/*                                                                                      */
/* Details  -Stop timer	play high alarm				     								*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerPlayHighAlarm : timer play high alarm    			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerPlayHighAlarm(void)
{
	if (xTimerStop(s_GUITimerPlayHighAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUITimerPlayHighAlarm \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerPlayMediumAlarm 										*/
/*                                                                                      */
/* Details  -Start timer play medium alarm				     							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerPlayMediumAlarm : timer play medium alarm          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerPlayMediumAlarm(void)
{
	if (xTimerStart(s_GUITimerPlayMediumAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerPlayMediumAlarm \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StopTimerPlayMediumAlarm 										*/
/*                                                                                      */
/* Details  -Stop timer	play medium alarm				     							*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerPlayMediumAlarm : timer play medium alarm          */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StopTimerPlayMediumAlarm(void)
{
	if (xTimerStop(s_GUITimerPlayMediumAlarm,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to stop s_GUITimerPlayMediumAlarm \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartTimerEnableIrda 											*/
/*                                                                                      */
/* Details  -Start timer enable Irda						     						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_GUITimerEnableIrda : timer enable Irda		        		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartTimerEnableIrda(void)
{
	if (xTimerStart(s_GUITimerEnableIrda,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_GUITimerEnableIrda \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_StartRTCUpdateTimer 											*/
/*                                                                                      */
/* Details  -Start RTC	Update	Timer						     						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_RTCUpdateTimer								        		*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_StartRTCUpdateTimer(void)
{
	if (xTimerStart(s_RTCUpdateTimer,TICK_TO_WAIT) != pdPASS) {
		DEBUGOUT("ERR: Failed to start s_RTCUpdateTimer \n");
	}
}

/** >>#0008
	 Add function update animation in operation
#0008 >>**/
/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_UpdateAnimationOperation 										*/
/*                                                                                      */
/* Details  -Update animation in operation						     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen							*/
/*                     (O) s_couter3s : connter 3 seconds after turn on FH310			*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_UpdateAnimationOperation(void)
{
	if(WM_IsVisible(g_operationScreen))
	{
		if(modeMgr_GetCurrentMode() == ePatientMode)
		{
			if(standbyScreen_GetStateMachine() == eStartSequence)
			{
				s_couter3s++;
				if(s_couter3s == COUNTER_3S)
				{
					s_couter3s = 0;
					operationScreen_EnterOperationMode();
				}
			}
			else if(standbyScreen_GetStateMachine() == eOperating)
			{
				operationScreen_updateAnimation();
			}
		}
	}

	return;
}

/** >>#0008
	 Add function Play childlock in operation
#0008 >>**/
/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_PlayChildlockOperation 											*/
/*                                                                                      */
/* Details  -Play childlock in operation						     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen							*/
/*                     (O) s_counterChildlock : counter play childlock					*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_PlayChildlockOperation(void)
{
	if(alarmScreenMgr_GetFlagAlarmScreen() == false &&
			modeMgr_GetCurrentMode() == ePatientMode)
	{
		s_counterChildlock++;
		if(s_counterChildlock == COUNTER_CHILD_LOCK)
		{
			audioMgr_Request(eChildLockAudio, eNoAlarmAudio);
		}
		else if(s_counterChildlock > (2*COUNTER_CHILD_LOCK))
		{
			guihandler_StopTimerPlayChildLock();
			s_counterChildlock = 0;
			audioMgr_Request(eChildLockAudio, eNoAlarmAudio);
		}
	}

	return;
}

/** >>#0008
	 Add function Play childlock in alarm
#0008 >>**/
/****************************************************************************************/
/*                                                                                      */
/* General 	-guihandler_PlayChildlockAlarm 												*/
/*                                                                                      */
/* Details  -Play childlock in alarm							     					*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE															 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_operationScreen : operation screen							*/
/*                     (O) s_counterChildlock : counter play childlock					*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void guihandler_PlayChildlockAlarm(void)
{
	if(alarmScreenMgr_GetFlagAlarmScreen() == true)
	{
		s_counterChildlock++;

		if(s_counterChildlock == COUNTER_CHILD_LOCK)
		{
			audioMgr_Request(eChildLockAudio, eNoAlarmAudio);
		}
		else if(s_counterChildlock > (2*COUNTER_CHILD_LOCK))
		{
			guihandler_StopTimerPlayChildLockAlarm();
			s_counterChildlock = 0;
			audioMgr_Request(eChildLockAudio, eNoAlarmAudio);
		}
	}

	return;
}

/* end of files */
