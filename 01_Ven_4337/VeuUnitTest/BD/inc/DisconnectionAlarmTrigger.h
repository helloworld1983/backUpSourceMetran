/******************************************************************************/
//$COMMON.H$
//   File Name:  DisconnectionAlarmTrigger.h
//
//   Class: 
//      This file contains the declaration for the Disconected Alarm Trigger
//      class.The Disconected Alarm Trigger is derived from Trigger class 
//
//   Revision History:
/******************************************************************************/
#ifndef DISCONNECTIONALARMTRIGGER_H
#define DISCONNECTIONALARMTRIGGER_H

#include "Trigger.h"

class DisconnectionAlarmTrigger : public Trigger
{
public:
    static DisconnectionAlarmTrigger* S_GetInstance(void);
    virtual E_Bool CheckCondition(void);
    virtual inline void ProcessTrigger(Mode* mode);
    void PhaseInSettingChange(LONG value);

    DisconnectionAlarmTrigger(void);
    DisconnectionAlarmTrigger(const DisconnectionAlarmTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static DisconnectionAlarmTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: BaseLineSetting
    //  Description:
    //  Units:
    //  Range:
    LONG LowInhPressLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;
};      

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessDisconectedAlarmTrigger() operation of 
//      the HFO mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
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
inline void DisconnectionAlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the Disconected trigger in the current mode.
    mode->ProcessTriggerID(eDisconnectionAlarmTrigger);

}   // end ProcessTrigger()

#endif // DISCONNECTIONALARMTRIGGER_H

