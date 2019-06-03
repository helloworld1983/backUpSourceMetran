/******************************************************************************/
//$COMMON.H$
//   File Name:  SpontMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Spontaneous Mode
//      which is derived from the NormBreathingMode base class.
/******************************************************************************/
#ifndef SPONTMODE_H
#define SPONTMODE_H

#include "NormBreathingMode.h"

class SpontMode : public NormBreathingMode
{
public:
    //Pointer to the object instantiated from this class
    static SpontMode* S_GetInstance (void);

    //Process doing something when another mode enters SpontMode
    void Enter (E_VentilationMode fromModeId);

    //process doing something when exit from this mode
    void Exit(void);

    //Process doing something when mode change
    void ProcessModeChangeTrigger(void);

    virtual void ProcessBpTimeTrigger(void);

    //Processing of the Patient Inhalation Trigger for the Spontaneous Mode
    virtual void ProcessPtInhTrigger (void);

protected:
    //Constructor for class
    SpontMode (void);

private:
    //Copy constructor for class
    SpontMode (const SpontMode&);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //it forces alarms that cannot exist in the Spontaneous mode into auto-reset
    void ReportModeChangeInducedReset (void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static SpontMode* S_Instance;
};

#endif // SPONTMODE_H
