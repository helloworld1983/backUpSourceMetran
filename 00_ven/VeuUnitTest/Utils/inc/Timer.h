#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stdint.h"

//Define type
typedef void * TimerHandle_t;
typedef uint32_t TickType_t;

typedef long BaseType_t;
#define pdFALSE         ( ( BaseType_t ) 0 )
#define pdTRUE          ( ( BaseType_t ) 1 )

//define function to mock
BaseType_t xTimerStart(TimerHandle_t handle, TickType_t CmdTimeout);
BaseType_t xTimerStop(TimerHandle_t handle, TickType_t CmdTimeout);
BaseType_t xTimerReset(TimerHandle_t handle, TickType_t CmdTimeout);
BaseType_t xTimerChangePeriod(TimerHandle_t handle, TickType_t NewPeriod, TickType_t CmdTimeout);
BaseType_t xTimerIsTimerActive(TimerHandle_t handle);

//Define function to test
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

class Timer
{
public:
    //Constructor for Timer Class, initialize a timer using TIMER's RTOS
    Timer (const char * const pcTimerName, //Just a text name, not used by the RTOS Kernel
           const TickType_t xTimerPeriodInTicks, //The timer period in ticks, must be greater than 0
           const long uxAutoReload){}; // The timers will auto-reload themselves when the expire(pdTRUE)

    //Destructors for TimerClass, delete timer
    virtual ~Timer (void){};

    //Check timer is active or not
    bool IsAcitve (void){};

    //Start the timer
    bool Start (TickType_t CmdTimeout = 0){};

    //Stop the timer
    bool Stop (TickType_t CmdTimeout = 0){};

    //Reset the timer
    bool Reset (TickType_t CmdTimeout = 0){};

    //Set new Period for timer
    bool SetPeriod (TickType_t NewPeriod, TickType_t CmdTimeout = 0){};


    //pure virtual function for derived class to do something when Timer expires
    virtual void ExpirationRoutine (void){};


    //$COMMON.ATTRIBUTE$
    //    Name: handle
    //    Description: Timer is used in Timer Class
    //
    //    Units:  none
    //    Range:  n/a
    TimerHandle_t handle;

    //Call back function is call when timer expires
//    static void TimerCallbackFunctionAdapter (TimerHandle_t xTimer);
};
#endif /* INC_TIMER_H_ */
