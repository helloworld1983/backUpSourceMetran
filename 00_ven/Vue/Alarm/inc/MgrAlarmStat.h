/******************************************************************************/
//
//$COMMON.H$
//    File Name: MgrAlarmStat.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: AlarmStat
//
//  The MgrAlarmStat class category interfaces with several major class 
//  categories including the BreathManagement class (see Breath Delivery 
//  SRS for more information), the PtData class (see Patient Data SRS),
//  the Continuous Built-in Test (CBIT) class, and the DataIO class to perform
//  low level operations for turning on/off LEDs and alarm horn.  It is the 
//  responsibility of each application to perform the detection and auto-reset 
//  of each of their respective alarms.
/******************************************************************************/
#ifndef  MGRALARMSTAT_H
#define  MGRALARMSTAT_H

#include "AlarmInterface.h"
#include "CommonTypes.h"
#include "AlarmStat.h"
#include "DebugTrace.h"

static const LONG NUMALARMS = (eLastAlarmType - eFirstAlarmType + 1);
static const LONG NUMALARMLEVELS = (eLastAlarmLevel + 1);
static const int NONEWALARMS = -1;

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

    protected:

    private:

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
// class MgrAlarmStat

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      * - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
inline MgrAlarmStat* MgrAlarmStat::S_GetInstance (void)
{
    if(S_Instance == NULL)
    {
        S_Instance = new MgrAlarmStat ();
    }

    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

#endif // MGRALARMSTAT_H

