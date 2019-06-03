/******************************************************************************/
//
//$COMMON.H$
//    File Name: RealTimeClock.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: RealTimeClock
//
//    RealTimeClock allow user init, set and get real time clock
//
/******************************************************************************/

#include "RealTimeClock.h"

#include "DebugTrace.h"
#include "board.h"

#define REAL_TIME_CLOCK_PORT             (0x1)             //PORT to control send data
#define REAL_TIME_CLOCK_PIN              (3)               //PIN to control send data
#define REAL_TIME_CLOCK_GPIO_PORT        (0)               //GPIO PORT to control send data
#define REAL_TIME_CLOCK_GPIO_PIN         (10)              //GPIO PIN to control send data

//extern "C" void RTC_IRQHandler(void)
//{
//    if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
//    {
//        Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
//    }
//
//    if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM))
//    {
//        Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
//    }
//}

RealTimeClock *RealTimeClock::S_Instance = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the RealTimeClock class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to RealTimeClock class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
RealTimeClock* RealTimeClock::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new RealTimeClock();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for Real Time clock
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void RealTimeClock::Init (void)
{
//    Chip_SCU_PinMuxSet(REAL_TIME_CLOCK_PORT, REAL_TIME_CLOCK_PIN, SCU_MODE_FUNC0);
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, REAL_TIME_CLOCK_GPIO_PORT, REAL_TIME_CLOCK_GPIO_PIN);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: Get current time from RTC
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void RealTimeClock::GetCurrentTime (Real_Time_Clock* rtc)
{
    //check pointer rtc is available or not
    ASSERTION(NULL != rtc);

//    RTC_TIME_T rtcTime;
//
//    Chip_RTC_GetFullTime(LPC_RTC, &rtcTime);
//
//    rtc->sec = rtcTime.time[RTC_TIMETYPE_SECOND];
//    rtc->min = rtcTime.time[RTC_TIMETYPE_MINUTE];
//    rtc->hour = rtcTime.time[RTC_TIMETYPE_HOUR];
//    rtc->wday = rtcTime.time[RTC_TIMETYPE_DAYOFWEEK];
//    rtc->mday = rtcTime.time[RTC_TIMETYPE_DAYOFMONTH];
//    rtc->month = rtcTime.time[RTC_TIMETYPE_MONTH];
//    rtc->year = rtcTime.time[RTC_TIMETYPE_YEAR];

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetTime
//
//    Processing: Set time for RTC
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void RealTimeClock::SetTime (const Real_Time_Clock* rtc)
{
    //check pointer rtc is available or not
    ASSERTION(NULL != rtc);

//    RTC_TIME_T rtcTime;
//    rtcTime.time[RTC_TIMETYPE_SECOND]     = rtc->sec;
//    rtcTime.time[RTC_TIMETYPE_MINUTE]     = rtc->min;
//    rtcTime.time[RTC_TIMETYPE_HOUR]       = rtc->hour;
//    rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->wday;
//    rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->mday;
//    rtcTime.time[RTC_TIMETYPE_MONTH]      = rtc->month;
//    rtcTime.time[RTC_TIMETYPE_YEAR]       = rtc->year;
//    Chip_RTC_SetFullTime(LPC_RTC, &rtcTime);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: RealTimeClock
//
//    Processing: Constructor of RealTimeClock class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
RealTimeClock::RealTimeClock (void)
{
}

