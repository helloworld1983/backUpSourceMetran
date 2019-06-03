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
#ifndef INC_REALTIMECLOCK_H_
#define INC_REALTIMECLOCK_H_

typedef struct {
    unsigned short  year;   /* 1..4095 */
    unsigned char   month;  /* 1..12 */
    unsigned char   mday;   /* 1.. 31 */
    unsigned char   wday;   /* 1..7 */
    unsigned char   hour;   /* 0..23 */
    unsigned char   min;    /* 0..59 */
    unsigned char   sec;    /* 0..59 */
} Real_Time_Clock;

class RealTimeClock
{
public:
    // static instance of RealTimeClock class
    static RealTimeClock *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Get current time from RTC
    void GetCurrentTime(Real_Time_Clock *rtc);

    //Set time for RTC
    void SetTime(const Real_Time_Clock* rtc);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of RealTimeClock class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static RealTimeClock *S_Instance;

    //RealTimeClock constructor
    RealTimeClock(void);

};

#endif /* INC_REALTIMECLOCK_H_ */
