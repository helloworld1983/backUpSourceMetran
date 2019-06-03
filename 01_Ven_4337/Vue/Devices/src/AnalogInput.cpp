//******************************************************************************
//$COMMON.H$
//   File Name:  AnalogInput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  A2DConverter
//
//      This class provides an interface to the Analog-to-Digital hardware.
//******************************************************************************
#include "AnalogInput.h"
#include "DebugTrace.h"

AnalogInput *AnalogInput::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the WatchDogTimer class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to WatchDogTimer class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
AnalogInput* AnalogInput::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new AnalogInput();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetRawCounts()
//
//    Processing: Reads the count value from the A-to-D
//          converter for the channel identified by the
//          input parameter.
//
//    Input Parameters: Address - A2D channel id (0-15)
//    Output Parameters:None
//    Return Values: USHORT
//          Returns A2D counts for the channel read,
//          OR A2D_ERROR_VALUE if an error is encountered
//    Pre-Conditions:
//          Assumes A2D hardware is initialized.
//          Assumes Address is a valid address.
//    Miscellaneous:
//          Hamming Code is ignored.
//
//          This module contains conditional compile directives
//          in order to handle two versions of the hardware.
//          The "old" hardware is the original Metran HW design.
//          The "new" electronics is the improved Dini HW design.
//    Requirements:
//
//******************************************************************************
USHORT AnalogInput::GetRawCounts(UCHAR Chanel)
{
    // 12-bits of return value
    register USHORT A2dCounts = 0;

    return(A2dCounts);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: getCurrentValue (UCHAR address)
//
//    Processing: Get current value
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: CurrentValue - current value
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
float AnalogInput::getCurrentValue (UCHAR address)
{
    USHORT CurrentValue = 0;

    return CurrentValue;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: Initialize for AnalogInput
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
void AnalogInput::Init (void)
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: AnalogInput()
//
//    Processing: This is the private constructor for the A2DConverter class.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Ptr to static A2DConverter object
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
AnalogInput::AnalogInput(int range)
{
    Range = range;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AnalogInput(void)
//
//    Processing: constructor for AnalogInput
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
AnalogInput::AnalogInput (void)
{
}
