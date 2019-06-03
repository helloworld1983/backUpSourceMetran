//******************************************************************************//
//$COMMON.CPP$
//   File Name:  MgrDiscreteOutput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the methods of the DigitalOutput class.
//
//   Interfaces:
//      Derived Driver classes - interfaces to I/O hardware
//      Location data structure - HW mapping of output signal
//
//   Restrictions: None
//
//******************************************************************************

#include "MgrDiscreteOutput.h"
#include "DebugTrace.h"

MgrDiscreteOutput* MgrDiscreteOutput::S_Instance = NULL;
DiscreteOutput* MgrDiscreteOutput::ProxyFlowResetLine = NULL;
DiscreteOutput* MgrDiscreteOutput::InpedanceLine = NULL;
DiscreteOutput* MgrDiscreteOutput::SafetyValve = NULL;
DiscreteOutput* MgrDiscreteOutput::ExhPressureZero = NULL;
DiscreteOutput* MgrDiscreteOutput::InhPressureZero = NULL;
DiscreteOutput* MgrDiscreteOutput::PurgeFlow = NULL;
DiscreteOutput* MgrDiscreteOutput::OilPump = NULL;
DiscreteOutput* MgrDiscreteOutput::EnableIO = NULL;
DiscreteOutput* MgrDiscreteOutput::RemoteAlarm = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrDiscreteOutput class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrDiscreteOutput class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrDiscreteOutput* MgrDiscreteOutput::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrDiscreteOutput();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrDiscreteOutput
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
void MgrDiscreteOutput::Init (void)
{
    // Exhalation Pressure Auto-Zero Solenoid
     ExhPressureZero = new DiscreteOutput(eExhPressZero, 4);
     ASSERTION( ExhPressureZero != NULL );

     // Inhalation Pressure Auto-Zero Solenoid
     InhPressureZero = new DiscreteOutput(eInhPressZero, 5);
     ASSERTION( InhPressureZero != NULL );

     SafetyValve = new DiscreteOutput(eSafetyValve, 6);
     ASSERTION( SafetyValve != NULL );

     InpedanceLine = new DiscreteOutput(eInpedanceLine, 7);
     ASSERTION( InpedanceLine != NULL );

     // Create DiscreteOutput objects used by Morph
     PurgeFlow = new DiscreteOutput(ePurgeFlow, 8);
     ASSERTION( PurgeFlow != NULL );

     OilPump = new DiscreteOutput(eOilPump, 9,eOff,eOff);
     ASSERTION( OilPump != NULL );

     RemoteAlarm = new DiscreteOutput(eRemoteAlarm, 10);
     ASSERTION( RemoteAlarm != NULL );

     EnableIO = new DiscreteOutput(eIOEnable, 15);
     ASSERTION( EnableIO != NULL );

     ProxyFlowResetLine = new DiscreteOutput(eProxyFlowResetLine, 30, eOff, eOn);
     ASSERTION( ProxyFlowResetLine != NULL );
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrDiscreteOutput
//
//    Processing: Constructor of  MgrDiscreteOutput
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
MgrDiscreteOutput::MgrDiscreteOutput (void)
{
}
