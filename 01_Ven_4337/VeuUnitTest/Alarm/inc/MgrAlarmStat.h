#ifndef INC_MGRALARMSTAT_H_
#define INC_MGRALARMSTAT_H_

#include "DebugTrace.h"
#include "AlarmInterface.h"
#include "CommonTypes.h"
#include "stdint.h"
#include "BdInterface.h"

//Define type
typedef void * TimerHandle_t;
typedef uint32_t TickType_t;

typedef long BaseType_t;
#define pdFALSE         ( ( BaseType_t ) 0 )
#define pdTRUE          ( ( BaseType_t ) 1 )

static const LONG NUMALARMS = (eLastAlarmType - eFirstAlarmType + 1);
static const LONG NUMALARMLEVELS = (eLastAlarmLevel + 1);
static const int NONEWALARMS = -1;

class ElevationTimer
{
    public:
        //constructor
        ElevationTimer (UNSIGNED tim);

        // Virtual routine to call upon expiration.
        void ExpirationRoutine (void);

        void Reset(void){};

        void Stop(void){};

        //$COMMON.ATTRIBUTE$
        //    Name: ElevationFlag
        //
        //    Description: flag used to indicate a an alarm is to be elevated
        //                 in priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool ElevationFlag;

    protected:
    private:
};

class Timer
{
public:

    //Constructor for Timer Class, initialize a timer using TIMER's RTOS
    Timer (const char * const pcTimerName, //Just a text name, not used by the RTOS Kernel
           const TickType_t xTimerPeriodInTicks, //The timer period in ticks, must be greater than 0
           const LONG uxAutoReload); // The timers will auto-reload themselves when the expire(pdTRUE)

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
//
//protected:
//
//    //pure virtual function for derived class to do something when Timer expires
//    virtual void ExpirationRoutine (void) = 0;
//
//private:

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

class AlarmControl: public Timer
{
    public:
        // static instance of AlarmControl
        static inline AlarmControl* S_GetInstance (void){};

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
        void SetSilenceButtonStatus(E_Bool val){};

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

class AlarmStat
{

    public:

        // constructor
        AlarmStat (E_GUIEventID id, E_AlarmLevel initialPriority,
                   E_AlarmLevel finalPriority, UNSIGNED tim, UNSIGNED occurent);

        // Set alarm to be active or not active
        E_ReturnStatus SetIsActive (E_AlarmStatus stat);

        // return the alarm status
        E_AlarmStatus GetIsActive (void);

        //return level of alarm
        E_AlarmLevel GetLevel(void);

        //$COMMON.ATTRIBUTE$
        //    Name: Level
        //
        //    Description: Current alarm priority level
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel  Level;

        //$COMMON.ATTRIBUTE$
        //    Name: Level
        //
        //    Description: previous alarm priority level
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel PreLevel;

        //$COMMON.ATTRIBUTE$
        //    Name: Processed
        //
        //    Description: Flag to indicate Alarm's task has processed
        //                 alarm
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool Processed;

        //$COMMON.ATTRIBUTE$
        //    Name: AlarmTimer
        //
        //    Description: Pointer to alarm elevation class
        //
        //    Units: None
        //
        //    Range: N/A
        //
        ElevationTimer * AlarmTimer;

        //$COMMON.ATTRIBUTE$
        //    Name: IsActive
        //
        //    Description: Alarm Status
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmStatus IsActive;

        //$COMMON.ATTRIBUTE$
        //    Name: OldIsActive
        //
        //    Description: Previous Alarm status
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmStatus OldIsActive;

        //$COMMON.ATTRIBUTE$
        //    Name: InitialPriority
        //
        //    Description: inital alarm level priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel InitialPriority;

        //$COMMON.ATTRIBUTE$
        //    Name: FinalPriority
        //
        //    Description: End Alarm level priority
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel FinalPriority;

        //$COMMON.ATTRIBUTE$
        //    Name: Id
        //
        //    Description: Current alarm identifier
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_GUIEventID Id;

    protected:

    private:

};

class MgrAlarmStat
{
    public:
        // static instance of alarm status manager
        static inline MgrAlarmStat* S_GetInstance (void);

        // update all the alarm status
        void StatusUpdate ();

        // if a non silenceable alarm is active returns eTrue else eFalse
        E_Bool NonSilenceableActive (void);

        // sound horns light LEDs
        void Enunciate (int i);

        // process reset request
        void DoReset (void);

        // process silence request
        void DoSilence(void);

        //Clear alarm
        void ClearAllAlarm(void);
        void ClearHFOAlarm(void);
        void ClearIMVAlarm(void);

        //$COMMON.ATTRIBUTE$
        //    Name: Id
        //
        //    Description: array of Alarm global object pointers
        //
        //    Units: None
        //
        //    Range: N/A
        //
        AlarmStat *Alarm[NUMALARMS];

        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: static pointer of alarm status manager
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static MgrAlarmStat* S_Instance;

        //$COMMON.ATTRIBUTE$
        //    Name: currentAlarm
        //
        //    Description: current level of alarm
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_AlarmLevel currentAlarm;

        // constructor
        MgrAlarmStat (void);

        // checks for corrupt alarm status
        E_Bool CorruptStatus (int i);

        // check if alarm needs elevation
        void CheckElevation (int i);

        // check if alarm status has changed
        void CheckStatus (int i);

        // activate the alarm
        void ActivateAlarm (int i);

        // deactivate alarm
        void DeactivateAlarm (int i);

        // raise alarm level
        void ElevateAlarm (int i);

        // indicator used for Enunciation
        UCHAR BuildIndicator ();

        void MakeInactive (int i);

};



#endif /* INC_MGRALARMSTAT_H_ */
