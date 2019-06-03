/******************************************************************************/
//$COMMON.H$
//    Filename: MgrController.cpp
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  MgrController
//    This class manage VCVCOntroller and CMVController
/******************************************************************************/

#include "MgrController.h"
#include "DebugTrace.h"

MgrController *MgrController::S_Instance = NULL;
CMVController *MgrController::CMVServoController = NULL;
VCVController *MgrController::VCVServoController = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrController class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrController class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrController* MgrController::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrController();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrController
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
void MgrController::Init (void)
{
    CMVServoController = new CMVController();
    VCVServoController = new VCVController();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrController
//
//    Processing: Constructor for MgrController
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
MgrController::MgrController (void)
{
}
