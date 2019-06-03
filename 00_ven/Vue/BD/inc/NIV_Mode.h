/******************************************************************************/
//$COMMON.H$
//   File Name:  NIV_Mode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the NIV_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef NIV_MODE_H
#define NIV_MODE_H

#include "MixedMode.h"

class NIV_Mode : public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static NIV_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);

    //Process manual Inh Request Trigger
    virtual void ProcessManualInhOpReqTrigger();

private:
    //copy constructor
    NIV_Mode(const NIV_Mode&);

    //Constructor
    NIV_Mode(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static NIV_Mode* S_Instance;
};

#endif // NIV_MODE_H
