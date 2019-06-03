/******************************************************************************/
//$COMMON.H$
//   File Name:  DisconnectionMode.h
//
//   Class: 
//      This file contains the class declaration for the ET Disconected Mode
//      which is derived from the AlarmBreathingMode base class.
//
//   Revision History:
//
/******************************************************************************/
#ifndef DISCONNECTIONMODE_H
#define DISCONNECTIONMODE_H

#include "AlarmBreathingMode.h"

class DisconnectionMode : public AlarmBreathingMode
{
public:
    //Pointer to the object instantiated from this class
    static DisconnectionMode* S_GetInstance(void);

    //Process doing something when another mode enters this mode and exits
    virtual void Enter(E_VentilationMode fromModeId);
    virtual void Exit(void);

protected:

private:
    //COnstructor
    DisconnectionMode(void);

    //Copy constructor
    DisconnectionMode(const DisconnectionMode&);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process reconnection trigger
    virtual void ProcessReconnectionTrigger(void);

    //Update alarm status
    virtual void UpdateAlarmStatus(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static DisconnectionMode* S_Instance;
};

#endif // DISCONNECTEDMODE_H

