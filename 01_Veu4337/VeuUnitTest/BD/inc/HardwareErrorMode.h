/******************************************************************************/
//$COMMON.H$
//   File Name:  HardwareErrorMode.h
//
//   Class:
//      This file contains the class declaration for the ET HardwareErrorMode Mode
//      which is derived from the AlarmBreathingMode base class.
//
//   Revision History:
//
/******************************************************************************/
#ifndef HARDWAREERRORMODE_H
#define HARDWAREERRORMODE_H

//#include "AlarmBreathingMode.h"
#include "NonVenMode.h"
//#include "guiinterface.h"

class HardwareErrorMode : public NonVenMode
{
public:
    //Pointer to the object instantiated from this class
    static HardwareErrorMode* S_GetInstance(void);

    //Process doing something when another mode enters this mode and exits
    virtual void Enter(E_VentilationMode fromModeId);
    virtual void Exit(void);

    //Constructor
    HardwareErrorMode(void);

    //Copy constructor
    HardwareErrorMode(const HardwareErrorMode&);

    //Update alarm status
    virtual void UpdateAlarmStatus(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process hardware error auto reset trigger
    virtual void ProcessHardwareErrorAutoresetTrigger(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HardwareErrorMode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: causeOfAlarm
    //  Description: Reason of alarm
    //  Units: None
    //  Range: n/a
    E_GUIEventID causeOfAlarm;

};

#endif // HARDWAREERRORMODE_H
