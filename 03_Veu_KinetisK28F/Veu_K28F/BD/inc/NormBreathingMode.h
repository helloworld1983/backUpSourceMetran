/******************************************************************************/
//$COMMON.H$
//   File Name:  NormBreathingMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the NormBreathingMode 
//      class.  All normal (i.e. non-alarm) modes where breath delivery is 
//      active are derived from the NormBreathingMode class.  The 
//      NormBreathingMode class is derived from the BreathingMode base class.
/******************************************************************************/
#ifndef NORMBREATHINGMODE_H
#define NORMBREATHINGMODE_H

#include "BreathingMode.h"

class NormBreathingMode : public BreathingMode
{
public:

    //Constructor for class
    NormBreathingMode (void);

    //process doing something when it goes out this mode
    virtual void Exit (void);

    //retrieves the current phase from the PhaseMgr object
    void EndSpontInhalation(void);

protected:
    //Update common trigger
    void UpdateCommonTriggers (void);

    //Update alarm status
    virtual void UpdateAlarmStatus (void);

private:
    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);
    // These operations are defined in the NormBreathingMode class because
    // they are common to all normal breathing modes.
    void ProcessApneaAlarmTimeTrigger (void);
    void ProcessModeChangeTrigger (void);
    void ProcessItLongAlarmTimeTrigger (void);
    void ProcessPtExhTrigger (void);
    void ProcessHipAlarmTrigger(void);
    void ProcessExtremePressAlarmTrigger(void);
    void ProcessOcclusionAlarmTrigger(void) ;
    void ProcessAlarmResetTrigger(void);
    void ProcessManualInhOpReqTrigger(void);
    void ProcessProxyDisconnectTrigger(void);
    void ProcessProxyConnectTrigger(void);

    // The copy constructor is not used.
    NormBreathingMode (const NormBreathingMode&);
};

#endif // NORMBREATHINGMODE_H
