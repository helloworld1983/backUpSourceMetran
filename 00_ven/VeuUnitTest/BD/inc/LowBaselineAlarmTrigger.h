/******************************************************************************/
//$COMMON.H$
//   File Name:  LowBaselineAlarmTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the Low Baseline Alarm Trigger
//      class.  The Low Baseline Pressure Alarm Trigger is derived from the 
//      Trigger class.
/******************************************************************************/
#ifndef LOWBASELINEALARMTRIGGER_H
#define LOWBASELINEALARMTRIGGER_H
#include <Trigger.h>
#include "DebugTrace.h"

class LowBaselineAlarmTrigger : public Trigger
{
public:
    //Process action follow trigger id
    virtual inline void ProcessTrigger (Mode* mode);

    //Constructor
    LowBaselineAlarmTrigger (void);

    //check condition and process action
    virtual E_Bool CheckCondition (void);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Disable trigger
    virtual void DisableTrigger (void);

    LowBaselineAlarmTrigger (const LowBaselineAlarmTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Operator set Low Baseline or Low EPAP setting
    //  Units: cmH2O * SCALE
    //  Range: 0 - 35 cmH2O, scaled
    SHORT PressureLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: IsConditionMet
    //  Description:  Boolean that indicates if the trigger criteria were
    //      met the previous time it was checked
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsConditionMet;

    //$COMMON.ATTRIBUTE$
    //  Name: IsCondMetEveryCycle
    //  Description:  Boolean that indicates if the condition is met every
    //      cycle of exhalation.  This value is used to reset the Low PEEP
    //      or Low EPAP alarm when exhalation time is less than 1 second.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsCondMetEveryCycle;

    //$COMMON.ATTRIBUTE$
    //  Name: TimeConditionFirstMet
    //  Description:  System Time that the condition was met; used to
    //      insure the criteria are met for a minimum amount of time in
    //      the initial part of inhalation.
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    UNSIGNED TimeConditionFirstMet;

    //$COMMON.ATTRIBUTE$
    //  Name: VentType
    //  Description:  Ventilation type currently in effect; used to
    //      determine whether a Low EPAP or Low PEEP alarm is annunciated
    //      or reset.
    //  Units: None
    //  Range: Pressure, Volume, Non-Invasive
    E_VentilationType VentType;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation asserts since it should never be called.
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
inline void LowBaselineAlarmTrigger::ProcessTrigger (Mode* mode)
{
    ASSERTION(eFalse);

}   // end ProcessPtExhTrigger()

#endif // LOWBASELINEALARMTRIGGER_H
