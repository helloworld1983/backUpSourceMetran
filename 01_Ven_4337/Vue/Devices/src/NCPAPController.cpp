//*****************************************************************************
//$COMMON.CPP$
//    Filename: NCPAPController.cpp
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the NCPAPController class
//*****************************************************************************
#include "NCPAPController.h"

#include "O2Setting.h"

#include "MgrStemMotor.h"
#include "MgrFlowController.h"

#include "DebugTrace.h"

NCPAPController * NCPAPController::S_Instance = NULL;
LONG NCPAPController::Int_ExValveFtr = 0;
const USHORT Scale_n        =    8;
const USHORT ValveFtr_Const =    100;
const USHORT iDT            =    500;           //200 for 5ms interval, 500 for 2ms interval

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
NCPAPController* NCPAPController::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new NCPAPController();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for NCPAPController
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
void NCPAPController::Init (void)
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Calculate()
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void NCPAPController::Calculate (void)
{
    LONG desiredAirFlow;
    LONG desiredO2Flow;
    LONG totalFlowSetting = TargetFlow*1000/60;

    O2Setting::S_GetInstance()->CalculateAirO2Flows(totalFlowSetting,&desiredAirFlow,&desiredO2Flow);

    MgrStemMotor::ExhValve->SetStepPosition(ExValve_Filter(MIN_STEP_POSITION));

    MgrFlowController::AirFlowController->SetDesired(desiredAirFlow*60/1000);
    MgrFlowController::O2FlowController->SetDesired(desiredO2Flow*60/1000);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void NCPAPController::Run (void)
{
    if (eTrue == Enabled)
    {
        Calculate();
    }
    else
    {
        return;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetTargetFlow (LONG value)
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void NCPAPController::SetTargetFlow (LONG value)
{
    TargetFlow = value;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ResetIMVStaticVariables (void)
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void NCPAPController::ResetIMVStaticVariables (void)
{
    Int_ExValveFtr = 0;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for NCPAPController
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
NCPAPController::NCPAPController (void)
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for NCPAPController
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
void NCPAPController::Disable (void)
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExValve_Filter()
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
SHORT NCPAPController::ExValve_Filter (SHORT shortInt)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_ExValveFtr>>Scale_n);
    Error = shortInt - Output;
    Temp32 = Error;
    Temp32 = Temp32<<Scale_n;
    Int_ExValveFtr += (LONG)Temp32*ValveFtr_Const/iDT;

    return(Output);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExValve_Filter()
//
//    Processing:
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void NCPAPController::Enable (void)
{
}
