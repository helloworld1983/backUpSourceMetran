/******************************************************************************/
//$COMMON.H$
//   File Name:  PtInhFlowTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Inhalation
//      Flow Trigger, which is derived from the Trigger base class.
/******************************************************************************/
#ifndef PTINHFLOWTRIGGER_H
#define PTINHFLOWTRIGGER_H

#include <Trigger.h>

class PtInhFlowTrigger : public Trigger
{
public:
    //Constructor
    PtInhFlowTrigger (void);

    //indicates if the trigger fired or not
    virtual E_Bool CheckCondition (void);

    //Check previous condition
    virtual E_Bool PreCheckCondition (void);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Disable trigger
    virtual void DisableTrigger(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

    //updates the FlowLimit and bias flow based upon the new sensitivity
    virtual void PhaseInSettingChange (LONG newValue);

    //Copy constructor
    PtInhFlowTrigger (const PtInhFlowTrigger&);

    //IMV checks condition
    E_Bool IMV_CheckCondition(void);

    //APRV checks condition
    E_Bool APRV_CheckCondition(void);

    //$COMMON.ATTRIBUTE$
    //  Name: InUnrestrictedPhase
    //  Description:  Indicates if the unrestricted phase of exhalation
    //      is active.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool InUnrestrictedPhase;

    //$COMMON.ATTRIBUTE$
    //  Name: FlowLimit
    //  Description:  0 - Flow Sensitivity
    //  Units: ml/sec * SCALE
    //  Range: 0.5 - 20 LPM; 83.33 - 333.33 ml/sec
    LONG FlowLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevState
    //  Description:  Previous state
    //  Units: None
    //  Range: None
    E_State PrevState;

    //$COMMON.ATTRIBUTE$
    //  Name: CurState
    //  Description:  current state
    //  Units: None
    //  Range: None
    E_State CurState;

    //$COMMON.ATTRIBUTE$
    //  Name: timerCnt
    //  Description:  timer counter for APRV, IMV
    //                      check condition
    //  Units: None
    //  Range: None
    unsigned int timerCnt;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessPtInhTrigger() operation of the active
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
inline void PtInhFlowTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to process the patient flow trigger in the 
    // currently active mode.
    mode->ProcessTriggerID(ePtInhFlowTrigger);

}   // end ProcessTrigger()

#endif // PTINHFLOWTRIGGER_H
