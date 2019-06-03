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

#include "MgrFlowController.h"
#include "MgrFlowSens.h"
#include "MgrStemMotor.h"
#include "DebugTrace.h"

MgrFlowController* MgrFlowController::S_Instance = NULL;
FlowController* MgrFlowController::AirFlowController = NULL;
FlowController* MgrFlowController::O2FlowController = NULL;
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrFlowController class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrFlowController class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrFlowController* MgrFlowController::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrFlowController();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrFlowController
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void MgrFlowController::Init (void)
{
    AirFlowController = new FlowController(
                eAir,
                MgrFlowSens::AirFlowSensor,
                MgrStemMotor::AirValve);

    ASSERTION(NULL != AirFlowController);

    O2FlowController  = new FlowController(
                eOxygen,
                MgrFlowSens::O2FlowSensor,
                MgrStemMotor::O2Valve);

    ASSERTION(NULL != O2FlowController);

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrFlowController
//
//    Processing: Constructor of  MgrFlowController
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrFlowController::MgrFlowController (void)
{
}
