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

#define AIR_SW_PORT             (0x5)             //PORT to control send data
#define AIR_SW_PIN              (7)               //PIN to control send data
#define AIR_SW_GPIO_PORT        (2)               //GPIO PORT to control send data
#define AIR_SW_GPIO_PIN         (7)              //GPIO PIN to control send data

#define O2_SW_PORT             (0x1)             //PORT to control send data
#define O2_SW_PIN              (16)               //PIN to control send data
#define O2_SW_GPIO_PORT        (0)               //GPIO PORT to control send data
#define O2_SW_GPIO_PIN         (3)              //GPIO PIN to control send data

#define DOOR_SW_PORT             (0x1)             //PORT to control send data
#define DOOR_SW_PIN              (1)               //PIN to control send data
#define DOOR_SW_GPIO_PORT        (0)               //GPIO PORT to control send data
#define DOOR_SW_GPIO_PIN         (8)              //GPIO PIN to control send data

#define HFO_SW_PORT             (0x1)             //PORT to control send data
#define HFO_SW_PIN              (2)               //PIN to control send data
#define HFO_SW_GPIO_PORT        (0)               //GPIO PORT to control send data
#define HFO_SW_GPIO_PIN         (9)              //GPIO PIN to control send data

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
//    O2PressSwitch  = new DiscreteInput(eO2Pressure, AIR_SW_PORT, AIR_SW_PIN, AIR_SW_GPIO_PORT, AIR_SW_GPIO_PIN, SCU_MODE_FUNC0);
//    ASSERTION( O2PressSwitch != NULL );
//
//    AirPressSwitch  = new DiscreteInput(eAirPressure, O2_SW_PORT, O2_SW_PIN, O2_SW_GPIO_PORT, O2_SW_GPIO_PIN, SCU_MODE_FUNC0);
//    ASSERTION( AirPressSwitch != NULL );
//
//    HFODoorSwitch  = new DiscreteInput(eHFODoorSwitch, DOOR_SW_PORT, DOOR_SW_PIN, DOOR_SW_GPIO_PORT, DOOR_SW_GPIO_PIN, SCU_MODE_FUNC0);
//    ASSERTION( HFODoorSwitch != NULL );
//
//    HFOPistonSwitch  = new DiscreteInput(eHFOPistonSwitch, HFO_SW_PORT, HFO_SW_PIN, HFO_SW_GPIO_PORT, HFO_SW_GPIO_PIN, SCU_MODE_FUNC0);
//    ASSERTION( HFODoorSwitch != NULL );
//
//    //initialize IO
//    O2PressSwitch->Init();
//    AirPressSwitch->Init();
//    HFODoorSwitch->Init();
//    HFOPistonSwitch->Init();

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

