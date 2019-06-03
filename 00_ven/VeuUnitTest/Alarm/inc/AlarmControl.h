#ifndef INC_ALARMCONTROL_H_
#define INC_ALARMCONTROL_H_

#include "stdint.h"
#include "Standard.h"
#include "BdInterface.h"

//Define type
typedef void * TimerHandle_t;
typedef uint32_t TickType_t;

typedef long BaseType_t;
#define pdFALSE         ( ( BaseType_t ) 0 )
#define pdTRUE          ( ( BaseType_t ) 1 )

class Timer
{
public:

    //Constructor for Timer Class, initialize a timer using TIMER's RTOS
    Timer (const char * const pcTimerName, //Just a text name, not used by the RTOS Kernel
           const TickType_t xTimerPeriodInTicks, //The timer period in ticks, must be greater than 0
           const long uxAutoReload){}; // The timers will auto-reload themselves when the expire(pdTRUE)

    //Destructors for TimerClass, delete timer
    virtual ~Timer (void){};

    //pure virtual function for derived class to do something when Timer expires
    virtual void ExpirationRoutine (void) = 0;

    //$COMMON.ATTRIBUTE$
    //    Name: handle
    //    Description: Timer is used in Timer Class
    //
    //    Units:  none
    //    Range:  n/a
    TimerHandle_t handle;
};

class AlarmControl: public Timer
{
    public:
        // static instance of AlarmControl
        static inline AlarmControl* S_GetInstance (void);

        // Key send with key id
        virtual E_ReturnStatus Send (E_DeviceId id);

        // gets reset flag using a semaphore
        inline E_Bool GetResetFlag (void);

        // gets silence flag using a semaphore
        inline E_Bool GetSilenceFlag (void);

        // sets reset flag using a semaphore
        inline void SetResetFlag (E_Bool val);

        // sets silence flag using a semaphore
        inline void SetSilenceFlag (E_Bool val);

        // turnoff silence duration
        void CancelSilence (void);

        //Return status of silence button
        E_Bool GetSilenceButtonStatus(void);

        //Set silence button status
        void SetSilenceButtonStatus(E_Bool val);

        // static instance of alarm controls
        static AlarmControl* S_Instance;

        // constructor
        AlarmControl (void);

        // Virtual routine to call upon expiration.

        void ExpirationRoutine (void);

        //$COMMON.ATTRIBUTE$
        //    Name: ResetFlag
        //
        //    Description: flag used to indicate a reset event occurred
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool ResetFlag;

        //$COMMON.ATTRIBUTE$
        //    Name: SilenceFlag
        //
        //    Description: flag used to indicate a silence event occurred
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool SilenceFlag;

        //$COMMON.ATTRIBUTE$
        //    Name: SilenceOn
        //
        //    Description: Flag used for silence button activation
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool SilenceOn;
};


#endif /* INC_ALARMCONTROL_H_ */
