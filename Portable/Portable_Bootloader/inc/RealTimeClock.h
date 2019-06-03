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
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/12/05       Thao Ha		  	    new file        */
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
/* General 	-realtimeclock_gettime	                                            		*/
/*                                                                                      */
/* Details  -Get current time from RTC     												*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/****************************************************************************************/
void realtimeclock_gettime (Real_Time_Clock*);			/* Get time */



#endif /* INC_REALTIMECLOCK_H_ */
