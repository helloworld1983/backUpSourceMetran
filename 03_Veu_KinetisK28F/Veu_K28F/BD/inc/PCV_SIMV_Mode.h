/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the PCV_SIMV_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef PCV_SIMV_MODE_H
#define PCV_SIMV_MODE_H

#include "MixedMode.h"

class PCV_SIMV_Mode: public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static PCV_SIMV_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

private:
    //Copy Constructor
    PCV_SIMV_Mode(const PCV_SIMV_Mode&);

    //Constructor
    PCV_SIMV_Mode(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Pt Inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //Process Pt Exh Trigger
    virtual void ProcessPtExhTrigger(void);

    //Process Bp Time Tigger
    virtual void ProcessBpTimeTrigger(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PCV_SIMV_Mode* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: IsSynOn
    //  Description:  synchronize or not
    //  Units: None
    //  Range: n/a
    bool IsSynOn;
};

#endif // PCV_SIMV_MODE_H
