/******************************************************************************/
//$COMMON.H$
//   File Name:  AcMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the PCV_CPAP_Mode Mode
//      which is derived from the SpontMode base class.
/******************************************************************************/
#ifndef PCV_CPAP_MODE_H
#define PCV_CPAP_MODE_H

#include "SpontMode.h"

class PCV_CPAP_Mode: public SpontMode
{
public:
    //Pointer to the object instantiated from this class
    static PCV_CPAP_Mode* S_GetInstance(void);

    //Process doing something when another mode enters this mode
    void Enter(E_VentilationMode fromModeId);

    //Copy constructor for class
    PCV_CPAP_Mode(const PCV_CPAP_Mode&);

    //Constructor for class
    PCV_CPAP_Mode(void);


    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static PCV_CPAP_Mode* S_Instance;
};

#endif // PCV_CPAP_MODE_H
