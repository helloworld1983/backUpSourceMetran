/******************************************************************************/
//
//$COMMON.H$
//    File Name: WatchdogTimer.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: WatchdogTimer
//
//    WatchdogTimer allow user init, set Watchdog of system
//
/******************************************************************************/
#ifndef INC_WATCHDOGTIMER_H_
#define INC_WATCHDOGTIMER_H_

class WatchDogTimer
{
public:
    // static instance of WatchDogTimer class
    static WatchDogTimer *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Feed watchdog timer
    void FeedWDTimer(void);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of WatchDogTimer class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static WatchDogTimer *S_Instance;

    //WatchDogTimer constructor
    WatchDogTimer(void);
};

#endif /* INC_WATCHDOGTIMER_H_ */
