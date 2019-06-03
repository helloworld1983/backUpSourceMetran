//******************************************************************************//
//$COMMON.H$
//    Filename: MgrStemMotor.h
//    Copyright 1996 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  StepMotor
//    This class is the class for the Step Motor Controller Devices
//******************************************************************************//
#include "MgrStemMotor.h"
#include "DebugTrace.h"

MgrStemMotor *MgrStemMotor::S_Instance = NULL;
StepMotor *MgrStemMotor::AirValve = NULL;
StepMotor *MgrStemMotor::O2Valve = NULL;
StepMotor *MgrStemMotor::ExhValve = NULL;
StepMotor *MgrStemMotor::HfoValve = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrStemMotor class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrStemMotor class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrStemMotor* MgrStemMotor::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrStemMotor();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrStemMotor
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
void MgrStemMotor::Init (void)
{
    O2Valve = new StepMotor(0,(CHAR*)"/dev/ttyUSB0",LIFTOFF_OFFSET,2000,100*SCALE, eO2Valve);

    AirValve =  new StepMotor(1,(CHAR*)"/dev/ttyUSB1",LIFTOFF_OFFSET,2000,100*SCALE, eAirValve);

    ExhValve = new StepMotor(2,(CHAR*)"/dev/ttyUSB2",LIFTOFF_OFFSET,MAX_STEP_POSITION,100*SCALE, eExhValve);

    HfoValve = new StepMotor(3,(CHAR*)"/dev/ttyUSB3",LIFTOFF_OFFSET,MAX_STEP_POSITION,100*SCALE, eHfoValve);
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
MgrStemMotor::MgrStemMotor (void)
{
}
