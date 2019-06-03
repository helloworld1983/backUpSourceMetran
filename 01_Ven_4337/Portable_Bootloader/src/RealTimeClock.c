
/********************************************************************/
/*                                                                  */
/* File Name    : RealTimeClock.c                                   */
/*                                                                  */
/* General      : RealTimeClock allow user init, set and get        */
/* 				   real time clock					    			*/
/* 								                                    */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
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
#include "board.h"
#include "RealTimeClock.h"

void RTC_IRQHandler(void)
{
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	rtc_initGPIO							              				*/
/*                                                                                      */
/* Details:    Init GPIO for real time clock											*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void rtc_initGPIO()
{
	Chip_SCU_PinMuxSet(0x1, 3, SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 10);
}




/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_gettime	                                            		*/
/*                                                                                      */
/* Details  -Get current time from RTC     												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock *rtc					 					  			*/
/*             (O) Real_Time_Clock *rtc													*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void realtimeclock_gettime (Real_Time_Clock *rtc)
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
}


