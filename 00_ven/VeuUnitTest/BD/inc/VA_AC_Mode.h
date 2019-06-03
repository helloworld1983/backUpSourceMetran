/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the VA_AC_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef VA_AC_MODE_H
#define VA_AC_MODE_H

#include "MixedMode.h"
#include "DeviceInterface.h"

class VA_AC_Mode: public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static VA_AC_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode and exits
    void Enter(E_VentilationMode fromModeId);
    void Exit(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Bp Timer Trigger
    virtual void ProcessBpTimeTrigger(void);

    //Process Pt Inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //Process Proxy Disconnect trigger
    virtual void ProcessProxyDisconnectTrigger(void);

    //Process Proxy connect trigger
    virtual void ProcessProxyConnectTrigger(void);

    //Process Apneal Alarm Time Trigger
    virtual void ProcessApneaAlarmTimeTrigger(void);

    //Start Test Breath
    void StartTestBreath(void);

    //$COMMON.ATTRIBUTE$
    //  Name: mTestBreathCount
    //  Description:  Count of test Breath
    //  Units: None
    //  Range: n/a
    static USHORT mTestBreathCount;

    //$COMMON.ATTRIBUTE$
    //  Name: mBreathType
    //  Description:  Breath Type
    //  Units: None
    //  Range: n/a
    static BreathType mBreathType;

    //constructor
    VA_AC_Mode(void);

    //Process Inh Phase
    void ProcessInhPhase(void);

    //Update PIP setting
    void UpdatePIPSetting(void);

    //$COMMON.ATTRIBUTE$
    //  Name: prevProxyStatus
    //  Description:  Status of previous proxy
    //  Units: None
    //  Range: n/a
    ProxyStatus prevProxyStatus;

    //Copy constructor
    VA_AC_Mode(const VA_AC_Mode&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VA_AC_Mode* S_Instance;
};

#endif // VA_AC_MODE_H
