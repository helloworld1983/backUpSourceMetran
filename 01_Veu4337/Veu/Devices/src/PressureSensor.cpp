//******************************************************************************
//$COMMON.H$
//   File Name:  PressureSensor.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: PressureSensor
//    This class is the base class for the sensor devices.
//******************************************************************************

#include "PressureSensor.h"
#include "MgrStemMotor.h"
#include "DebugTrace.h"

PressureSensor *PressureSensor::S_Instance = NULL;
Sensor *PressureSensor::ExhPressSensor = NULL;
Sensor *PressureSensor::InhPressSensor = NULL;
VirtualPressureSensor *PressureSensor::ActivePressureSensor = NULL;
ExhPressureController *PressureSensor::ExhController = NULL;

static const UCHAR A2D_CHANNEL_00 = 0;  // Inhalation Pressure Transducer
static const UCHAR A2D_CHANNEL_01 = 1;  // Exhalation Pressure Transducer
static const UCHAR A2D_CHANNEL_02 = 2;  // O2  Flow Sensor - Flow
static const UCHAR A2D_CHANNEL_03 = 3;  // Air Flow Sensor - Flow
static const UCHAR A2D_CHANNEL_05 = 5;  // Air Flow Sensor - Flow

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the PressureSensor class
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
PressureSensor* PressureSensor::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new PressureSensor();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for PressureSensor
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
void PressureSensor::Init (void)
{
    InhPressSensor = new Sensor(eInhPressSensor,
                                A2D_CHANNEL_02);    // A2D channel #

    ASSERTION( NULL != InhPressSensor);

    ExhPressSensor = new Sensor(eExhPressSensor,
                                A2D_CHANNEL_05);

    ASSERTION( NULL != ExhPressSensor);

    ActivePressureSensor = new VirtualPressureSensor( InhPressSensor );

    ASSERTION( NULL != ActivePressureSensor);

    ExhController = new ExhPressureController(MgrStemMotor::ExhValve);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PressureSensor
//
//    Processing: constructor for PressureSensor
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
PressureSensor::PressureSensor (void)
{
}
