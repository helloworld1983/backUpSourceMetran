/******************************************************************************/
//$COMMON.H$
//   File Name:  PtExhPressTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Exhalation
//      Pressure Trigger, which is derived from the Trigger class.
/******************************************************************************/
#ifndef PTEXHPRESSTRIGGER_H
#define PTEXHPRESSTRIGGER_H

#include <Trigger.h>
const UNSIGNED INIT_PRESS_CRITERIA_PHASE = 200 / MS_PER_TICK; // 200 ms in TICKS
const UNSIGNED MIN_DURATION_PRESS_COND_MET = 100 / MS_PER_TICK; // 100 ms in TICKS

class PtExhPressTrigger : public Trigger
{
public:
    //Constructor
    PtExhPressTrigger (void);

    //check condition and process action
    virtual E_Bool CheckCondition (void);

    //enable trigger
    virtual void EnableTrigger (void);
    virtual void EnableTrigger (UNSIGNED pressure);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

    //Copy constructor
    PtExhPressTrigger (const PtExhPressTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Pressure that causes a non-alarm termination of a
    //      spontaneous or supported inhalation.
    //  Units: cmH2O * SCALE
    //  Range:
    //      During PSV:  Psupport + PEEP + 1.5 cmH2O
    //      During CPAP: CPAP + offset + 1.0 cmH2O
    //      During NIPPV: IPAP + 1.5 cmH2O
    SHORT PressureLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: IsConditionMet
    //  Description:  Boolean that indicates if the trigger criteria were
    //      met the previous time it was checked
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsConditionMet;

    //$COMMON.ATTRIBUTE$
    //  Name: TimeConditionFirstMet
    //  Description:  System Time that the condition was met; used to
    //      insure the criteria are met for a minimum amount of time in
    //      the initial part of inhalation.
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    UNSIGNED TimeConditionFirstMet;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessPtExhTrigger() operation of the active
//      mode.
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
inline void PtExhPressTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to process the patient pressure trigger in the 
    // currently active mode.
    mode->ProcessTriggerID(ePtExhPressTrigger);

}   // end ProcessPtExhTrigger()

#endif // PTEXHPRESSTRIGGER_H
