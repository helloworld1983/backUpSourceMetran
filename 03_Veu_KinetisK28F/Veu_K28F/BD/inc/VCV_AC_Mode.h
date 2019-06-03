/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the VCV_AC_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef VCV_AC_MODE_H
#define VCV_AC_MODE_H

#include "NormBreathingMode.h"
#include "MixedMode.h"

class VCV_AC_Mode: public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static VCV_AC_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);
protected:

private:
    //copy constructor
    VCV_AC_Mode(const VCV_AC_Mode&);

    //constructor
    VCV_AC_Mode(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process Pt inh Trigger
    virtual void ProcessPtInhTrigger(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VCV_AC_Mode* S_Instance;
};

#endif // VCV_AC_MODE_H
