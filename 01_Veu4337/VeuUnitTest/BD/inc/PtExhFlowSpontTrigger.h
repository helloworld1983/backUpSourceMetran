/******************************************************************************/
//$COMMON.H$
//   File Name:  PtExhFlowSpontTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Exhalation
//      Pressure Trigger, which is derived from the Trigger class.
/******************************************************************************/
#ifndef PTEXHFLOWSPONTTRIGGER_H
#define PTEXHFLOWSPONTTRIGGER_H

#include "Trigger.h"

class PtExhFlowSpontTrigger : public Trigger
{
public:
    //constructor
    PtExhFlowSpontTrigger(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

    PtExhFlowSpontTrigger(const PtExhFlowSpontTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: PrevFilteredDelFlow
    //  Description:  Filtered delivered flow for the previous BD cycle;
    //      used to filter the delivered flow for the current cycle.
    //  Units: TICKS
    //  Range:  0 - 3333.33 ml/sec (200 LPM), scaled
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
inline void PtExhFlowSpontTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to process the patient pressure trigger in the 
    // currently active mode.
    mode->ProcessTriggerID(ePtExhFlowSpontTrigger);

}   // end ProcessPtExhTrigger()

#endif // PTEXHFLOWSPONTTRIGGER_H
