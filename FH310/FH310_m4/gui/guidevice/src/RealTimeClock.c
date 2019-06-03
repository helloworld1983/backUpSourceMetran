/********************************************************************/
/*                                                                  */
/* File Name    : RealTimeClock.c                                   */
/*                                                                  */
/* General      : RealTimeClock allow user init, set and get        */
/* 				   real time clock					    			*/
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
/* #0001       2017/08/30		Quyen Ngo			modify			*/
/*	 For avoid to access of RTC register <1S						*/
/*   -add realTimeClock_SendRequestSetTime function					*/
/*   -add realTimeClock_ProcessSetTimeRequest						*/
/*	 -add  g_rtcQueue												*/
/*	 -create g_rtcQueue when init RTC								*/
/*	 -add #define RTC_WAIT_TIME_MS		1000						*/
/*   #define RTC_WAI_TIME_TICK_MAX	RTC_WAIT_TIME_MS/GUI_TASK_DELAY_MS*/
/*	 -add 	s_isProcessingRtcSet									*/
/* #0002       2017/09/20		Linh Nguyen			modify			*/
/*   - handle new ID in E_RTC_EVENT_ID                              */
/*	 - modify realTimeClock_SendRequestSetTime function				*/
/* #0003       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/* #0004       201/10/03		Viet Le				modify			*/
/*   -Change these return type to void from int 					*/
/*		(realtimeclock_settime(), realtimeclock_gettime) (bug 1927) */
/* #0005       201/10/03		Viet Le				modify			*/
/*   -Remove NVIC_EnableIRQ((IRQn_Type) RTC_IRQn) 					*/
/*    in realtimeclock_Init()function. 								*/
/*    Call NVIC_EnableIRQ((IRQn_Type) RTC_IRQn) 					*/
/*    in realtimeclock_StartTest() function.						*/
/*    Call NVIC_DisableIRQ((IRQn_Type) RTC_IRQn)                    */
/*    in realtimeclock_StopTest() function (bug 1926)				*/
/* #0006       201/10/03		Viet Le				modify			*/
/*    -Add realtimeclock_SetRtcLow(void) function to low pin rtc.	*/
/*      Call realtimeclock_SetRtcLow(void) 							*/
/*		in guihandler_TimerCallback() (bug 1930)					*/
/********************************************************************/
/* #0007       2017/10/02  		Quyen Ngo			modify			*/
/*	Add void argument for realTimeClock_ProcessSetTimeRequest func	*/
/*	(Bug 1931)														*/
/*  		   2017/10/07      Linh Nguyen(MV)  	    modify	    */
/* #0008 Add realTimeClock_CheckValidDate function  for bug #1938	*/
/* #0009       201/10/18		Viet Le				modify			*/
/*	Remove NVIC_DisableIRQ((IRQn_Type) RTC_IRQn) and 				*/
/*		NVIC_EnableIRQ((IRQn_Type) RTC_IRQn) in 					*/
/*		realtimeclock_settime() function (bug 1985)					*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0010 Change RTC design  (bug #2002)								*/
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0011 Improve RTC design  (bug #2002)							*/
/*  		   2017/11/08      Linh Nguyen(MV)  	    modify	    */
/* #0012 Send cbx update calendar event after 2sec (bug #2002)		*/
/* #0013       201/11/17		Linh Nguyen(MV)			modify		*/
/*	Restart timer update RTC when set time complete (bug 2027)		*/
/* #0014       201/11/22		Linh Nguyen(MV)			modify		*/
/*	Send update event every time when cbx set time (bug 2027)		*/
/********************************************************************/
#include <RealTimeClock.h>
#include "board.h"
#include "TaskCommon.h"
#include "SettingOnOffButton.h"
#include "GuiHandler.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "AnypalInterface.h"

#define RTC_WAIT_TIME_MS		1000
#define RTC_WAI_TIME_TICK_MAX	RTC_WAIT_TIME_MS/GUI_TASK_DELAY_MS
#define SET_TIME_WAIT 2000/GUI_TASK_DELAY_MS

static bool s_startTest = false;
static bool s_pinRtcLow = false;

QueueHandle_t g_rtcQueue;

/****************************************************************************************/
/*                                                                                      */
/* General 	-RTC_IRQHandler			                                            		*/
/*                                                                                      */
/* Details  -RTC interrupt			        											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_startTest :flag start test RTC							    */
/*                     (O) s_pinRtcLow : flag set pin low                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void RTC_IRQHandler(void)
{
	/* Toggle heart beat LED for each second field change interrupt */
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		/* Clear pending interrupt */
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}

	/* Check for alarm match */
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		/* Clear pending interrupt */
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
	}

	if(s_startTest == true)
	{
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, RTC_TEST_PORT, RTC_TEST_BIT);
		s_pinRtcLow = true;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_initGPIO			                                            */
/*                                                                                      */
/* Details  -Initialize GPIO		        											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_initGPIO(void)
{
	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, RTC_TEST_PORT, RTC_TEST_BIT);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, RTC_TEST_PORT, RTC_TEST_BIT);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_Init		                                            		*/
/*                                                                                      */
/* Details  -Initialize RTC		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】 Because Chip_RTC_Enable call in bootloader, so does need re-enable on ControlSW	*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_Init(void)
{
	g_rtcQueue = xQueueCreate(RTC_QUEUE_LENGTH, sizeof(RTC_EVENT_STRUCT));
	__disable_irq();
	realtimeclock_initGPIO();

	/* Disable RTC interrupt in NVIC */
	NVIC_DisableIRQ((IRQn_Type) RTC_IRQn);

	/* Set the RTC to generate an interrupt on each second */
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);
	NVIC_SetPriority(RTC_IRQn,RTC_INT_PRIO);

	__enable_irq();

	return;
}


/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_gettime 			                                            */
/*                                                                                      */
/* Details  -Get date time		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock *rtc									 			    */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void						                                          	*/
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_gettime(Real_Time_Clock *rtc)
{
	RTC_TIME_T rtcTime;

	Chip_RTC_GetFullTime(LPC_RTC, &rtcTime);

	rtc->sec = rtcTime.time[RTC_TIMETYPE_SECOND];
	rtc->min = rtcTime.time[RTC_TIMETYPE_MINUTE];
	rtc->hour = rtcTime.time[RTC_TIMETYPE_HOUR];
	rtc->wday = rtcTime.time[RTC_TIMETYPE_DAYOFWEEK];
	rtc->mday = rtcTime.time[RTC_TIMETYPE_DAYOFMONTH];
	rtc->month = rtcTime.time[RTC_TIMETYPE_MONTH];
	rtc->year = rtcTime.time[RTC_TIMETYPE_YEAR];

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_settime			                                            */
/*                                                                                      */
/* Details  -Set date time		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const Real_Time_Clock *rtc									 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void						                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_settime(const Real_Time_Clock *rtc)
{
	RTC_TIME_T rtcTime;

	rtcTime.time[RTC_TIMETYPE_SECOND]     = rtc->sec;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = rtc->min;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = rtc->hour;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->mday;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = rtc->month;
	rtcTime.time[RTC_TIMETYPE_YEAR]       = rtc->year;

	Chip_RTC_SetFullTime(LPC_RTC, &rtcTime);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realTimeClock_CheckValidDate												*/
/*                                                                                      */
/* Details  -Check valid date					            							*/
/* 																						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock date											 		*/
/*																						*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/****************************************************************************************/
static bool realTimeClock_CheckValidDate(Real_Time_Clock date)
{
	if ((date.year > 0) && (date.month > 0 && date.month <= DECEMBER))
	{
		if ((date.month == APRIL) || (date.month == JUNE) || (date.month == SEPTEMBER) || (date.month == NOVEMBER))
		{
			if (date.mday == 0 || date.mday > DAY_30)
				return false;
		}
		else if (date.month == FEBRUARY)
		{
			if (date.year % YEAR_DIV == 0)
			{
				if (date.mday == 0 || date.mday > DAY_29)
					return false;
			}
			else
			{
				if (date.mday == 0 || date.mday > DAY_28)
					return false;
			}
		}
		else
		{
			if (date.mday == 0 || date.mday > DAY_31)
				return false;
		}
	}
	else
	{
		return false;
	}
	if (date.hour > 23 || date.min > 59 || date.sec >59)
	{
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realTimeClock_SendRequestSetTime			                                */
/*                                                                                      */
/* Details  -Push a request set RTC to RTC queue		        						*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const Real_Time_Clock *rtc									 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool: true/false push successful/fail                                  */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) QueueHandle_t g_rtcQueue                      			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool realTimeClock_SendRequestSetTime(const Real_Time_Clock* rtc, E_RTC_EVENT_ID id)
{
	RTC_EVENT_STRUCT event;
	event.id = id;
	event.data = *rtc;
	if(!realTimeClock_CheckValidDate(*rtc))
	{
		DEBUGOUT("\nUn valid date");
		return false;
	}
	if(xQueueSend(g_rtcQueue, &event,0) == pdFALSE)
	{
		DEBUGOUT("\n Failure send");
		return false;
	}
	return true;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realTimeClock_ProcessSetTimeRequest			                            */
/*                                                                                      */
/* Details  -Based on current processing state of RTC, pop reqest and set RTC			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I)																 		*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool: true/false push successful/fail                                  */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void realTimeClock_ProcessSetTimeRequest(void)
{
	static uint16_t s_rtcSetWaitTimeCnt = RTC_WAI_TIME_TICK_MAX;

	bool isEventComing = false;
	static bool s_CBXSetTime = false;
	RTC_EVENT_STRUCT event;
	RTC_EVENT_STRUCT previousEvent;
	previousEvent.id = E_NON_RTC_EVENT_ID;
	static uint16_t s_SetTimeCnt = SET_TIME_WAIT+1;

	if(s_SetTimeCnt == SET_TIME_WAIT)
	{
		if(s_CBXSetTime)
		{
			s_CBXSetTime = false;
			anypalInterface_SendEvent(eCalendarUpdate);
		}
		//restart update RTC timer
		guihandler_StartRTCUpdateTimer();
		Real_Time_Clock rtc;
		realtimeclock_gettime(&rtc);
		guiInterface_setCurrentTime(rtc);
		s_SetTimeCnt++;
	}
	else if (s_SetTimeCnt < SET_TIME_WAIT)
	{
		s_SetTimeCnt++;
	}

	if(s_rtcSetWaitTimeCnt == RTC_WAI_TIME_TICK_MAX)
	{
		while(xQueueReceive(g_rtcQueue,&event,0) == pdTRUE)
		{
			isEventComing = true;
			if(event.id < previousEvent.id)
			{
				event = previousEvent;
			}
			previousEvent = event;
		}
		if(isEventComing)
		{
			if(event.id == E_CBX_REQUEST_SET_RTC_ID)
			{
				s_rtcSetWaitTimeCnt = 0;
				realtimeclock_settime(&event.data);
				s_SetTimeCnt = 0;
				//cbx is setting time
				s_CBXSetTime = true;
			}
			else if(event.id == E_ATP_REQUEST_SET_RTC_ID)
			{
				if(!s_CBXSetTime)
				{
					s_rtcSetWaitTimeCnt = 0;
					realtimeclock_settime(&event.data);
					s_SetTimeCnt = 0;
				}
			}
			else if(event.id == E_USER_REQUEST_SET_RTC_ID)
			{
				s_rtcSetWaitTimeCnt = 0;
				realtimeclock_settime(&event.data);
				s_SetTimeCnt = 0;
			}
		}
	}
	else
	{
		s_rtcSetWaitTimeCnt++;
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_Subtract14Day			                               		*/
/*                                                                                      */
/* Details  -Subtract 14 Day		        											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock* date									 			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_Subtract14Day(Real_Time_Clock* date)
{
	if (date->mday > SUBTRACT_DAY)
	{
		date->mday -= SUBTRACT_DAY;
	}
	else
	{
		if(date->month == JANUARY)
		{
			date->month = DECEMBER;
			date->year--;
		}
		else
		{
			date->month--;
		}
		if ((date->month == JANUARY)||(date->month == MARCH)||(date->month == MAY)
				||(date->month == JULY)||(date->month == AUGUST)||(date->month == OCTOBER)||(date->month == DECEMBER))
		{
			uint8_t tmp = SUBTRACT_DAY - date->mday;
			date->mday = DAY_31 - tmp;
		}
		else if((date->month == APRIL)||(date->month == JUNE)||(date->month == SEPTEMBER)||(date->month == NOVEMBER))
		{
			uint8_t tmp = SUBTRACT_DAY - date->mday;
			date->mday = DAY_30 - tmp;
		}
		else //month =2
		{
			if ((date->year % YEAR_DIV)==0)
			{
				uint8_t tmp = SUBTRACT_DAY - date->mday;
				date->mday = DAY_29 - tmp;
			}
			else
			{
				uint8_t tmp = SUBTRACT_DAY - date->mday;
				date->mday = DAY_28 - tmp;
			}
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_Compare2Date 				                                */
/*                                                                                      */
/* Details  -Compare 2 Date		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock input												*/
/*                 Real_Time_Clock check												*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool realtimeclock_Compare2Date(Real_Time_Clock input, Real_Time_Clock check)
{
	if(input.year != check.year)
	{
		return (input.year > check.year);
	}
	else if(input.month != check.month)
	{
		return (input.month > check.month);
	}
	else if(input.mday != check.mday)
	{
		return (input.mday > check.mday);
	}
	else if(input.hour != check.hour)
	{
		return (input.hour > check.hour);
	}
	else if(input.min != check.min)
	{
		return (input.min > check.min);
	}
	else if(input.sec != check.sec)
	{
		return (input.sec > check.sec);
	}
	else
	{
		return true;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_StartTest 				                                	*/
/*                                                                                      */
/* Details  -Start test RTC		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_startTest : flag start test RTC                            */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_StartTest(void)
{
	s_startTest = true;

	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_StopTest 				                                	*/
/*                                                                                      */
/* Details  -Stop test RTC		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_startTest : flag start test RTC                            */
/*                     (O) s_pinRtcLow : flag set pin low						        */
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_StopTest(void)
{
	s_startTest = false;
	s_pinRtcLow = false;
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, RTC_TEST_PORT, RTC_TEST_BIT);

	NVIC_DisableIRQ((IRQn_Type) RTC_IRQn);
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_GetSatusTest 				                                */
/*                                                                                      */
/* Details  -Get status test	        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_startTest : flag start test RTC					    	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool realtimeclock_GetSatusTest(void)
{
	return s_startTest;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_GetFlagLowPin 				                                */
/*                                                                                      */
/* Details  -Get flag low pin trc 								        				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : bool: : if true is set pin low	, else is false 					    */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_pinRtcLow : flag set pin low							    */
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool realtimeclock_GetFlagLowPin(void)
{
	return s_pinRtcLow;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_SetFlagLowPin 				                                */
/*                                                                                      */
/* Details  -Set flag low pin rtc								        			    */
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool status : - flag set pin low										*/
/*                    - range : 0-1                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_pinRtcLow : flag set pin low                               */
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_SetFlagLowPin(bool status)
{
	s_pinRtcLow = status;
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_GetLastTime 				                                	*/
/*                                                                                      */
/* Details  -Get last time		        												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Real_Time_Clock : last time	                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
Real_Time_Clock realtimeclock_GetLastTime(void)
{
	Real_Time_Clock currentTime;
	guiInterface_RTCGettime(&currentTime);

	return currentTime;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_SetRtcLow 				                                	*/
/*                                                                                      */
/* Details  -Set rtc pin to low		        											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void							                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_SetRtcLow(void)
{
	if(realtimeclock_GetFlagLowPin() == true)
	{
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, RTC_TEST_PORT, RTC_TEST_BIT);
		realtimeclock_SetFlagLowPin(false);
	}

	return;
}

/* end of files */
