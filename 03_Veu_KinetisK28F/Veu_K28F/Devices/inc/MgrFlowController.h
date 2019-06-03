//*****************************************************************************
//$COMMON.CPP$
//    Filename: MgrFlowController.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the FlowController class
//
//    Interfaces:
//      Air and O2 StepMotor objects
//      Air and O2 FlowSensor objects
//*****************************************************************************
#ifndef INC_MGRFLOWCONTROLLER_H_
#define INC_MGRFLOWCONTROLLER_H_

#include "FlowController.h"

class MgrFlowController
{
public:
    // static instance of PressureSensor class
    static MgrFlowController *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: AirFlowController
    //
    //    Description: Pointer to instance of air flow controller
    //    Units: None
    //
    //    Range: N/A
    //
    static FlowController* AirFlowController;

    //$COMMON.ATTRIBUTE$
    //    Name: O2FlowController
    //
    //    Description: Pointer to instance of air 02 controller
    //    Units: None
    //
    //    Range: N/A
    //
    static FlowController* O2FlowController;


protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of MgrFlowController class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrFlowController *S_Instance;

    //MgrFlowController constructor
    MgrFlowController(void);

};

#endif /* INC_MGRFLOWCONTROLLER_H_ */
