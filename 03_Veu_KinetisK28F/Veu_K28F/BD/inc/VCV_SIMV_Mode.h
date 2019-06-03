/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the VCV_SIMV_Mode Mode
//      which is derived from the MixedMode base class.
/******************************************************************************/
#ifndef VCV_SIMV_MODE_H
#define VCV_SIMV_MODE_H

#include "MixedMode.h"

class VCV_SIMV_Mode: public MixedMode
{
public:
    //Pointer to the object instantiated from this class
    static VCV_SIMV_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

private:
    //Copy constructor
    VCV_SIMV_Mode(const VCV_SIMV_Mode&);

    //Constructor
    VCV_SIMV_Mode(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static VCV_SIMV_Mode* S_Instance;
};

#endif // VCV_SIMV_MODE_H
