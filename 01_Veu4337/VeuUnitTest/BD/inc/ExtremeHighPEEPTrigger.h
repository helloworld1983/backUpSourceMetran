/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHighPEEPTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHighPEEPTrigger
/******************************************************************************/
#ifndef EXTREMEHIGHPEEPTRIGGER_H
#define EXTREMEHIGHPEEPTRIGGER_H
#include "AlarmStat.h"
#include "Trigger.h"
#include "HipAlarmTrigger.h"

class ExtremeHighPEEPTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static ExtremeHighPEEPTrigger* S_GetInstance(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Process action base on trigger id
    virtual inline void ProcessTrigger(Mode* mode);

    //Get high pressure alarm
    LONG GetHighPressAlarm(void);

    //Set new value of inactive timer
    void SetInactiveTimer(USHORT value);

    //$COMMON.ATTRIBUTE$
    //    Name: HighPressAlarm
    //    Description:
    //    Units:  LONG
    //    Range:
    LONG HighPressAlarm;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;

    //Constructor
    ExtremeHighPEEPTrigger(void);

    //Copy constructor
    ExtremeHighPEEPTrigger(const ExtremeHighPEEPTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ExtremeHighPEEPTrigger* S_Instance;

};
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
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
inline void ExtremeHighPEEPTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    mode->ProcessTriggerID(eExtremeHighPEEPTrigger);
}   // end ProcessTrigger()



#endif // EXTREMEHIGHPEEPTRIGGER_H
