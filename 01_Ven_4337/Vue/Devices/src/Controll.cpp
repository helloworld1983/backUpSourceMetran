//******************************************************************************
//$COMMON.CPP$
//    Filename: controll.cpp
//    Copyright 1996 InVentived Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the Controller base class
//
//    Interfaces: None
//
//    Restrictions:
//      This class is a base class. Instances of this class are not allowed
//******************************************************************************
#include "Controll.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Controller()
//
//    Processing:  This is the constructor for the Controller base class.
//          This operation invokes the Task() constructor, creates an MsTimer
//          object and then initializes the attributes  
//
//    Input Parameters:  None
//
//    Output Parameters: None
//
//    Return Values:     Ptr to the Controller object created.
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     n/a
//
//    Requirements:
//
//******************************************************************************
Controller::Controller (void)
{
    // Set initial conditions
    ProportionalGain = 0;
    CurrentMeasured = 0;
    PreviousMeasured = 0;
    RunningSum = 0;
    Desired = 0;
    OutputValue = 0;
    Enabled = eFalse;

    CompensatedDerivativeGain = 0;
    CompensatedIntegralGain = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetDesired()
//
//    Processing:   This routine sets the desired rate of the 
//               item (flow or pressure) being controlled.
//
//    Input Parameters:
//          rate: (LONG) desired value of the item being controlled
//
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
void Controller::SetDesired(LONG rate)
{
    // Set the Desired data member to the value requested.
    Desired = rate;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Reset()
//
//    Processing: This operation returns the controller to its
//              initial operating conditions.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
void Controller::Reset(void)
{
    //Set initial conditions
    RunningSum = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Reset()
//
//    Processing: This operation returns desired value
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     Desired - desired value
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
LONG Controller::GetDesired (void)
{
    return Desired;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Enable()
//
//    Processing:   This operation enables the controller object for execution. 
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
void Controller::Enable(void)
{
    // Set enabled flag
    Enabled = eTrue;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Disable()
//
//    Processing: This operation disables the controller object.
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
void Controller::Disable(void)
{
    // Clear enabled flag
    Enabled = eFalse;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: IsEnabled()
//
//    Processing:
//
//    Input Parameters:
//          rate: (int) desired value of the item being controlled
//
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     n/a
//    Requirements:
//
//******************************************************************************
E_Bool Controller::IsEnabled(void)
{
    return(Enabled);
}
