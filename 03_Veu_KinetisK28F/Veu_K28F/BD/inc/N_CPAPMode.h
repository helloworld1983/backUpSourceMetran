/******************************************************************************/
//$COMMON.H$
//   File Name:  N_CPAPMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the N_CPAPMode which
//      is derived from the NormBreathingMode base class.
/******************************************************************************/
#ifndef N_CPAPMODE_H
#define N_CPAPMODE_H

#include "NormBreathingMode.h"

class N_CPAPMode : public NormBreathingMode
{
public:
    //Pointer to the object instantiated from this class
    static N_CPAPMode* S_GetInstance(void);

    //Process doing something when another mode enters and exits to N_CPAPMode
    virtual void Enter(E_VentilationMode fromModeId);
    virtual void Exit(void);

protected:

private:
    //Constructor for class
    N_CPAPMode(void);

    //Copy constructor for class
    N_CPAPMode(const N_CPAPMode&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static N_CPAPMode* S_Instance;
};

#endif // N_CPAPMODE_H
