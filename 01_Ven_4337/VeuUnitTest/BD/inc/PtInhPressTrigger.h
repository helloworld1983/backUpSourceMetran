/******************************************************************************/
//$COMMON.H$
//   File Name:  PtInhPressTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Patient Inhalation
//      Pressure Trigger, which is derived from the Trigger class.
/******************************************************************************/
#ifndef PTINHPRESSTRIGGER_H
#define PTINHPRESSTRIGGER_H

#include <Trigger.h>

class PtInhPressTrigger : public Trigger
{
public:
    //Constructor
    PtInhPressTrigger (void);

    //Check condition and process action
    virtual E_Bool CheckCondition (void);

    //Enable trigger
    virtual void EnableTrigger (void);
    virtual void EnableTrigger (UNSIGNED pressure);

    //updates the PressureLimit as baseline pressure with new value
    virtual void PhaseInSettingChange (LONG newValue);

    //Process action base on trigger id
    virtual inline void ProcessTrigger (Mode* mode);

    //check previous condition and process action
    E_Bool PreCheckCondition (void);

    //Copy constructor
    PtInhPressTrigger (const PtInhPressTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: IsPressureFixed
    //  Description:  Indicates if the PressureLimit is fixed for the
    //      currently active exhalation phase.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsPressureFixed;

    //$COMMON.ATTRIBUTE$
    //  Name: InUnrestrictedPhase
    //  Description:  Indicates if the unrestricted phase of exhalation
    //      is active.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool InUnrestrictedPhase;

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Baseline Pressure - Pressure Sensitivity
    //  Units: cmH2O * SCALE
    //  Range: -20 to 34.9 cmH2O
    SHORT PressureLimit;

    //$COMMON.ATTRIBUTE$
    //  Name: PressureLimit
    //  Description:  Baseline Pressure - Pressure Sensitivity
    //  Units: cmH2O * SCALE
    //  Range: 0.5 to 10.5 cmH2O
    SHORT PressSens;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevFilteredPress
    //  Description:  Filtered pressure for the previous BD cycle;
    //      used to filter the monitor press for the current cycle.
    //  Units: cmH2O * SCALE
    //  Range:  0 - 12000
    LONG PrevFilteredPress;
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
inline void PtInhPressTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to process the patient pressure trigger in the 
    // currently active mode.
    mode->ProcessTriggerID(ePtInhTrigger);

}   // end PtInhPressTrigger()

#endif // PTINHPRESSTRIGGER_H
