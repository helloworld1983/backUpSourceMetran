/******************************************************************************/
//$COMMON.H$
//   File Name:  BreathingMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the BreathingMode 
//      class.  All modes where breath delivery is active are derived from
//      the BreathingMode class.  The BreathingMode class is derived from
//      the Mode base class.
/******************************************************************************/
#ifndef BREATHINGMODE_H
#define BREATHINGMODE_H

#include <Mode.h>

class BreathingMode : public Mode
{
public:
    //Constructor
    BreathingMode (E_Bool doesManualReset = eFalse);

protected:

private:
    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);
    // These operations are defined in the BreathingMode class because
    // they are common to all breathing modes.
    void ProcessVcvInhTimeTrigger (void);
    void ProcessMandPressInhTimeTrigger (void);
    void ProcessPlateauTimeTrigger (void);
    void ProcessManualInhOpReqTrigger (void);
    void ProcessHipAlarmTrigger (void);
    void ProcessTotalGasLossTrigger (void);
    void ProcessOcclusionAlarmTrigger (void);
    void ProcessDisconnectionAlarmTrigger(void);
    void ProcessHardwareErrorAlarmTrigger(void);


    // The copy constructor is not used
    BreathingMode (const BreathingMode&);
};

#endif // BREATHINGMODE_H
