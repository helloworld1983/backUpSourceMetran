/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ApneaVAMode Mode
//      which is derived from the VA_AC_Mode base class.
/******************************************************************************/
#ifndef APNEAVAMODE_H
#define APNEAVAMODE_H

#include "VA_AC_Mode.h"

class ApneaVAMode : public VA_AC_Mode
{
public:
    //Pointer to the object instantiated from this class
    static ApneaVAMode* S_GetInstance(void);

    //Process doing something when another mode enters this mode and exits
    void Enter(E_VentilationMode fromModeId);
    void Exit(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Proxy disconnect trigger
    virtual void ProcessProxyDisconnectTrigger(void){}

    //Process Proxy connect trigger
    virtual void ProcessProxyConnectTrigger(void){}

    //Process Pt Inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //Process mandatory press Inh TIme Trigger
    virtual void ProcessMandPressInhTimeTrigger(void);

    //Process alarm Reset trigger
    virtual void ProcessAlarmResetTrigger(void);

    //Process Apneal Alarm Time Trigger
    virtual void ProcessApneaAlarmTimeTrigger(void);

protected:

private:
    //Constructor
    ApneaVAMode(void);

    //Copy constructor
    ApneaVAMode(const ApneaVAMode&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ApneaVAMode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: fromMode
    //  Description:  Change from E_VentilationMode
    //  Units: None
    //  Range: n/a
    E_VentilationMode fromMode;

    //$COMMON.ATTRIBUTE$
    //  Name: IsPtInhTrigger
    //  Description:
    //  Units: None
    //  Range: n/a
    bool IsPtInhTrigger;
};

#endif // APNEAVAMODE_H
