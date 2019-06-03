//*****************************************************************************
//$COMMON.CPP$
//    Filename: NCPAPController.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the NCPAPController class
//*****************************************************************************
#ifndef INC_NCPAPCONTROLLER_H_
#define INC_NCPAPCONTROLLER_H_

#include "Standard.h"

class NCPAPController
{

public:
    // static instance of NCPAPController class
    static NCPAPController *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    void Calculate(void);
    void Run(void);
    void SetTargetFlow(LONG value);
    void ResetIMVStaticVariables(void);
    void Disable(void);
    void Enable(void);


protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of NCPAPController class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static NCPAPController *S_Instance;

    //NCPAPController constructor
    NCPAPController(void);

    SHORT ExValve_Filter(SHORT);

    static LONG Int_ExValveFtr;
    LONG TargetFlow;

    //$COMMON.ATTRIBUTE$
    //    Name:        Enabled
    //    Description: Flag denoting whether to perform calculation
    //                 and motor control.
    //    Units: E_Bool
    //    Range: eTrue, eFalse
    E_Bool  Enabled;
};

#endif /* INC_NCPAPCONTROLLER_H_ */
