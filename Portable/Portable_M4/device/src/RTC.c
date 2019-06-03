/********************************************************************/
/*                                                                  */
/* File Name    : RTC.c                			                    */
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
/*             2016/08/14       Thao Ha(MV)	  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include <RTC.h>
#include "board.h"

/****************************************************************************************/
/*                                                                                      */
/* General 	-RTC_IRQHandler			                                            		*/
/*                                                                                      */
/* Details  -This RTC interrupt run every 1s to toggle a pin for checking RTC function  */
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              											*/
/*                                                                                      */
/****************************************************************************************/
void RTC_IRQHandler(void)
{
	static bool state = false;
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
	}
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
	}
	state = !state;
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,0, 10,state);
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-rtc_initGPIO			                                            		*/
/*                                                                                      */
/* Details  -Init GPIO for testing RTC function											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE									 					  			*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              											*/
/*                                                                                      */
/****************************************************************************************/
void rtc_initGPIO()
{
	Chip_SCU_PinMuxSet(0x1, 3, SCU_MODE_FUNC0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 10);
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Initialize RTC			                                            		*/
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
/* local valiables   : NONE			                                                  	*/
/*【Note】                                                                              											*/
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_Init()
{
	__disable_irq();
	rtc_initGPIO();

	/* Disable RTC interrupt in NVIC */
	NVIC_DisableIRQ((IRQn_Type) RTC_IRQn);

	/* Set the RTC to generate an interrupt on each second */
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	/* Enable RTC (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);

	/* Enable RTC interrupt in NVIC */
	NVIC_SetPriority(RTC_IRQn, 3);
	NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);
	__enable_irq();
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
/****************************************************************************************/
/*                                                                                      */
/* General 	-realtimeclock_settime	                                            		*/
/*                                                                                      */
/* Details  -Set time for RTC		     												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) const Real_Time_Clock *rtc			 					  			*/
/*             (O) None																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void realtimeclock_settime (const Real_Time_Clock *rtc)
{
	RTC_TIME_T rtcTime;
	rtcTime.time[RTC_TIMETYPE_SECOND]     = rtc->sec;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = rtc->min;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = rtc->hour;
//	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->wday;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->mday;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = rtc->month;
	rtcTime.time[RTC_TIMETYPE_YEAR]	      = rtc->year;
	Chip_RTC_SetFullTime(LPC_RTC, &rtcTime);
}

