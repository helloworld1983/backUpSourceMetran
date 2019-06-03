/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the PCV_AC_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef PCV_AC_MODE_H
#define PCV_AC_MODE_H

#include "NormBreathingMode.h"
#include "MixedMode.h"

class PCV_AC_Mode : public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static PCV_AC_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

    //Copy constructor
    PCV_AC_Mode(const PCV_AC_Mode&);

    //Constructor
    PCV_AC_Mode(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Pt Inh Trigger
    virtual void ProcessPtInhTrigger(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PCV_AC_Mode* S_Instance;
};

#endif // PCV_AC_MODE_H
