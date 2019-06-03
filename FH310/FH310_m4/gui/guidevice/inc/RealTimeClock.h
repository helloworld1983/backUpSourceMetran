#ifndef INC_REALTIMECLOCK_H_
#define INC_REALTIMECLOCK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : RealTimeClock.h                                   */
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
/* #0001       201/08/30		Quyen Ngo			modify			*/
/*   -add realTimeClock_SendRequestSetTime function					*/
/*   -add realTimeClock_ProcessSetTimeRequest						*/
/* #0002       201/09/19		Viet Le				modify			*/
/*   -Change unsigned short->uint16_t,unsigned char->uint8_t        */
/*    (Real_Time_Clock struct) (bug 1861)						    */
/* #0003       2017/09/20		Linh Nguyen			modify			*/
/*   - add new ID to E_RTC_EVENT_ID                                 */
/*	 - modify realTimeClock_SendRequestSetTime function				*/
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
/*  		   2017/10/28      Linh Nguyen(MV)  	    modify	    */
/* #0007 Change RTC design  (bug #2002)								*/
/********************************************************************/
#include "board.h"

#define RTC_TEST_PORT      GPIO_PORT(3)
#define RTC_TEST_BIT	   GPIO_PORT(0)
#define RTC_QUEUE_LENGTH		16//<=8 request in 1 second

#define SUBTRACT_DAY  14
#define JANUARY       1
#define FEBRUARY      2
#define MARCH	      3
#define APRIL		  4
#define MAY			  5
#define JUNE		  6
#define JULY 		  7
#define AUGUST	      8
#define SEPTEMBER     9
#define OCTOBER		  10
#define NOVEMBER	  11
#define DECEMBER	  12

#define DAY_31  	  31
#define DAY_30  	  30
#define DAY_29  	  29
#define DAY_28  	  28

#define YEAR_DIV      4

//Struct real time clock
typedef struct {
	uint16_t year;	/* 1..4095 */
	uint8_t	month;	/* 1..12 */
	uint8_t	mday;	/* 1.. 31 */
	uint8_t	wday;	/* 1..7 */
	uint16_t yday;	/* 1..365 */
	uint8_t	hour;	/* 0..23 */
	uint8_t	min;	/* 0..59 */
	uint8_t	sec;	/* 0..59 */
} Real_Time_Clock;

typedef enum{
	E_NON_RTC_EVENT_ID,
	E_ATP_REQUEST_SET_RTC_ID,
	E_CBX_REQUEST_SET_RTC_ID,
	E_USER_REQUEST_SET_RTC_ID
}E_RTC_EVENT_ID;
//Define packet send to GUI
typedef struct{
	E_RTC_EVENT_ID id;//Id packet
	Real_Time_Clock data;//Data send
}RTC_EVENT_STRUCT;

//Init rtc
void realtimeclock_Init(void);
void realtimeclock_gettime(Real_Time_Clock*);			/* Get time */
void realtimeclock_settime(const Real_Time_Clock*);	   /* Set time */
void realtimeclock_Subtract14Day(Real_Time_Clock* date);
bool realtimeclock_Compare2Date(Real_Time_Clock input, Real_Time_Clock check);
void realtimeclock_StartTest(void);
void realtimeclock_StopTest(void);
bool realtimeclock_GetSatusTest(void);
bool realtimeclock_GetFlagLowPin(void);
void realtimeclock_SetFlagLowPin(bool status);
Real_Time_Clock realtimeclock_GetLastTime(void);
void realTimeClock_ProcessSetTimeRequest();
bool realTimeClock_SendRequestSetTime(const Real_Time_Clock* rtc, E_RTC_EVENT_ID id);
void realtimeclock_SetRtcLow(void);

#endif /* INC_REALTIMECLOCK_H_ */
