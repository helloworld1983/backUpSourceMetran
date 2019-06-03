#ifndef INC_REALTIMECLOCK_H_
#define INC_REALTIMECLOCK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : RTC.h         		                            */
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
/*             2017/12/05       Thao Ha(MV)	  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include "board.h"

typedef struct {
	unsigned short	year;	/* 1..4095 */
	unsigned char	month;	/* 1..12 */
	unsigned char	mday;	/* 1.. 31 */
	unsigned char	wday;	/* 1..7 */
	unsigned char	hour;	/* 0..23 */
	unsigned char	min;	/* 0..59 */
	unsigned char	sec;	/* 0..59 */
} Real_Time_Clock;

/****************************************************************************************/
/*                                                                                      */
/* General  -Initialize RTC                                                             */
/*                                                                                      */
/* Details  -Initialize RTC                                                             */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void realtimeclock_Init();

/****************************************************************************************/
/*                                                                                      */
/* General  -realtimeclock_gettime                                                      */
/*                                                                                      */
/* Details  -Get current time from RTC                                                  */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) Real_Time_Clock *rtc                                                 */
/*             (O) Real_Time_Clock *rtc                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/****************************************************************************************/
void realtimeclock_gettime (Real_Time_Clock*);			/* Get time */

/****************************************************************************************/
/*                                                                                      */
/* General  -realtimeclock_settime                                                      */
/*                                                                                      */
/* Details  -Set time for RTC                                                           */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) const Real_Time_Clock *rtc                                           */
/*             (O) None                                                                 */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/****************************************************************************************/
void realtimeclock_settime (const Real_Time_Clock*);	/* Set time */


#endif /* INC_REALTIMECLOCK_H_ */
