/******************************************************************************/
//$COMMON.H$
//   File Name:  AvMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Apnea Ventilation Mode
//      which is derived from the AlarmBreathingMode base class.
/******************************************************************************/
#ifndef AVMODE_H
#define AVMODE_H

#include "AlarmBreathingMode.h"

class AvMode : public AlarmBreathingMode
{
public:
    //Pointer to the object instantiated from this class
    static AvMode* S_GetInstance (void);

    //Process doing something when another mode enters this mode and exits
    virtual void Enter (E_VentilationMode fromModeId);
    virtual void Exit (void);

    //Constructor
    AvMode (void);

    //Copy constructor
    AvMode (const AvMode&);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process PT Inh Trigger
    virtual void ProcessPtInhTrigger (void);

    //Process Av Bp Time Trigger
    virtual void ProcessAvBpTimeTrigger (void);

    //Process Apneal Auto Reset Trigger
    virtual void ProcessApneaAutoResetTrigger (void);

    //Update Alarm status
    virtual void UpdateAlarmStatus (void);

    //Report mode change
    void ReportModeChangeInducedReset (void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AvMode* S_Instance;
};

#endif // AVMODE_H
