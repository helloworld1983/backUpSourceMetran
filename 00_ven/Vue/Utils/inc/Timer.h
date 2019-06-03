/******************************************************************************/
//$COMMON.H$
//   File Name:  Tasks.h
//
//   Class: This file contains the class declaration for the Timer Class
//                it is based class for all timer class use later
//
//   Warning : When using Debug in call back function of timer.
//             The Stack Size of Task must me considered to make sure all task running
//             or it stucked in one Task(HardFault)
//
/******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

//RTOS includes
#include "FreeRTOS.h"
#include "timers.h"

class Timer
{
public:

    //Constructor for Timer Class, initialize a timer using TIMER's RTOS
    Timer (const char * const pcTimerName, //Just a text name, not used by the RTOS Kernel
           const TickType_t xTimerPeriodInTicks, //The timer period in ticks, must be greater than 0
           const UBaseType_t uxAutoReload); // The timers will auto-reload themselves when the expire(pdTRUE)

    //Destructors for TimerClass, delete timer
    virtual ~Timer (void);

    //Check timer is active or not
    bool IsAcitve (void);

    //Start the timer
    bool Start (TickType_t CmdTimeout = 0);

    //Stop the timer
    bool Stop (TickType_t CmdTimeout = 0);

    //Reset the timer
    bool Reset (TickType_t CmdTimeout = 0);

    //Set new Period for timer
    bool SetPeriod (TickType_t NewPeriod, TickType_t CmdTimeout = 0);

protected:

    //pure virtual function for derived class to do something when Timer expires
    virtual void ExpirationRoutine (void) = 0;

private:

    //$COMMON.ATTRIBUTE$
    //    Name: handle
    //    Description: Timer is used in Timer Class
    //
    //    Units:  none
    //    Range:  n/a
    TimerHandle_t handle;

    //Call back function is call when timer expires
    static void TimerCallbackFunctionAdapter (TimerHandle_t xTimer);
};

#endif /* TIMER_H_ */
