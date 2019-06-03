/******************************************************************************/
//$COMMON.H$
//   File Name:  AlarmBreathingMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the AlarmBreathingMode 
//      class.  All alarm modes where breath delivery is active (e.g. Apnea)
//      are derived from the AlarmBreathingMode class.  The 
//      AlarmBreathingMode class is derived from the BreathingMode base class.
/******************************************************************************/
#ifndef ALARMBREATHINGMODE_H
#define ALARMBREATHINGMODE_H

#include "BreathingMode.h"

class AlarmBreathingMode : public BreathingMode
{
    public:
        AlarmBreathingMode (E_Bool doesManualReset);
        void SetCauseOfAlarm(E_GUIEventID cause){causeOfAlarm = cause;}

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //Process doing something when another mode enters this mode and exits
        virtual void Enter(E_VentilationMode fromModeId);
        virtual void Exit(void);

        //$COMMON.ATTRIBUTE$
        //  Name: causeOfAlarm
        //  Description: Reason of alarm
        //  Units: None
        //  Range: n/a
        E_GUIEventID causeOfAlarm;

        // These operations are defined in the AlarmBreathingMode class because
        // they are common to all alarm breathing modes.
        virtual void ProcessAlarmResetTrigger (void);
        // The copy constructor is not used
        AlarmBreathingMode (const AlarmBreathingMode&);
        AlarmBreathingMode (void);
};

#endif // ALARMBREATHINGMODE_H
