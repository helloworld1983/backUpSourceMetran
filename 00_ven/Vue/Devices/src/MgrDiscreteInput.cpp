//******************************************************************************
//$COMMON.H$
//    File Name: MgrDiscreteInput.cpp
//    Copyright 1997 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  DiscreteOutput
//
//      Description: The discrete output class is used to represent
//          the various output signals of the Morph ventilator.
//          The DiscreteInput class provides an abstraction for a
//          generic dicrete output signal and serves as a base class
//          for more specific derived classes (Led Class).
//
//******************************************************************************

#include "MgrDiscreteInput.h"
#include "DebugTrace.h"

MgrDiscreteInput *MgrDiscreteInput::S_Instance = NULL;

DiscreteInput *MgrDiscreteInput::AirPressSwitch = NULL;
DiscreteInput *MgrDiscreteInput::O2PressSwitch = NULL;
DiscreteInput *MgrDiscreteInput::HFODoorSwitch = NULL;
DiscreteInput *MgrDiscreteInput::HFOPistonSwitch = NULL;

DiscreteInput *MgrDiscreteInput::ManualBreathInput = NULL;
DiscreteInput *MgrDiscreteInput::ManualBreathConnection = NULL;
DiscreteInput *MgrDiscreteInput::O2MotorStatus = NULL;
DiscreteInput *MgrDiscreteInput::AirMotorStatus = NULL;
DiscreteInput *MgrDiscreteInput::ExhMotorStatus = NULL;
DiscreteInput *MgrDiscreteInput::HFOMotorStatus = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrDiscreteInput class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrDiscreteInput class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrDiscreteInput* MgrDiscreteInput::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrDiscreteInput();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrDiscreteInput
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
void MgrDiscreteInput::Init (void)
{
    O2PressSwitch  = new DiscreteInput(eO2Pressure, 16);
    ASSERTION( O2PressSwitch != NULL );

    AirPressSwitch  = new DiscreteInput(eAirPressure, 17);
    ASSERTION( AirPressSwitch != NULL );

    ManualBreathInput  = new DiscreteInput(eManualBreathInput, 18);
    ASSERTION( ManualBreathInput != NULL );

    ManualBreathConnection  = new DiscreteInput(eManualBreathConnection, 19);
    ASSERTION( ManualBreathConnection != NULL );

    HFODoorSwitch  = new DiscreteInput(eHFODoorSwitch, 20);
    ASSERTION( HFODoorSwitch != NULL );

    HFOPistonSwitch  = new DiscreteInput(eHFOPistonSwitch, 21);
    ASSERTION( HFODoorSwitch != NULL );

    O2MotorStatus  = new DiscreteInput(eO2MotorStatus, 25);
    ASSERTION( O2MotorStatus != NULL );

    AirMotorStatus  = new DiscreteInput(eAirMotorStatus, 26);
    ASSERTION( AirMotorStatus != NULL );

    ExhMotorStatus  = new DiscreteInput(eExhMotorStatus, 27);
    ASSERTION( ExhMotorStatus != NULL );

    HFOMotorStatus  = new DiscreteInput(eHFOMotorStatus, 28);
    ASSERTION( HFOMotorStatus != NULL );
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrDiscreteInput
//
//    Processing: Constructor of  MgrDiscreteInput
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
MgrDiscreteInput::MgrDiscreteInput (void)
{
}

