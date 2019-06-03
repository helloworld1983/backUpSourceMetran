/******************************************************************************/
//$COMMON.H$
//   File Name:  PtExhFlowTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Exhalation
//      Flow Trigger, which is derived from the Trigger class.
/******************************************************************************/
#ifndef PTEXHFLOWTRIGGER_H
#define PTEXHFLOWTRIGGER_H
#include <Trigger.h>

const UNSIGNED INIT_FLOW_CRITERIA_PHASE = 200 / MS_PER_TICK;  // 200 ms in TICKS
const UNSIGNED MIN_DURATION_FLOW_COND_MET = 100 / MS_PER_TICK; // 100 ms in TICKS

class PtExhFlowTrigger : public Trigger
{
public:
    //Constructor
    PtExhFlowTrigger (void);

    //Check condition to process action
    virtual E_Bool CheckCondition (void);

    //Enable trigger
    virtual void EnableTrigger (void);
    virtual void EnableTrigger (UNSIGNED sensitivity);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

protected:

private:
    //Copu constructor
    PtExhFlowTrigger (const PtExhFlowTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: ExhSens
    //  Description:  Exhalation sensitivity; percentage of peak flow
    //      that the instantaneous flow must drop to cause the trigger
    //      to fire.
    //  Units: %
    //  Range: 10 - 45%
    LONG ExhSens;

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

    LONG PrevFilteredDelFlow;
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
inline void PtExhFlowTrigger::ProcessTrigger (Mode* mode)
{
//    mode->ProcessTriggerID(ePtExhFlowTrigger);

}   // end PtExhFlowTrigger()

#endif // PTEXHFLOWTRIGGER_H
