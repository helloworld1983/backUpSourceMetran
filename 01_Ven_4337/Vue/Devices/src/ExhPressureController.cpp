//******************************************************************************//
//$COMMON.CPP$
//   File Name:  EXHCONTR.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the routines for the Exhalation Pressure
//      Controller class.
//
//   Interfaces:
//      Operations in this class interface with the exhalation valve stepper
//      motor to command its position.
//
//******************************************************************************
#define EXHCONTR_CPP
#include "VirtualPressure.h"
#include "PressureSensor.h"
#include "ExhPressureController.h"
#include "DebugTrace.h"

// Constants used for the low order filter
static const LONG EXH_ALPHA = 6;       // 0.04762  ; scaled by 2**7; must add to 1
static const LONG EXH_BETA = 122;      // 0.9524  ; scaled by 2**7; must add to 1

// Exhalation Reference Weight Constants
static const LONG EXH_PROP_REF_WEIGHT = 141;     // 1.1  * 2**7
static const LONG EXH_INTEG_REF_WEIGHT_1 = 136;  // 1.06 * 2**7
static const LONG EXH_INTEG_REF_WEIGHT_2 = 131;  // 1.02 * 2**7

// Constant used for resetting integral sum.
static const USHORT ALMOST_OPEN_POSITION = 1600;  // step position

// Constants to determine when to apply integral and proportional multipliers.
static const LONG LO_ERROR_LIMIT  = (-50);  // 0.5 * scale
static const LONG HI_ERROR_LIMIT1 = (100);  // 1.0 * scale
static const LONG HI_ERROR_LIMIT2 = (200);  // 2.0 * scale

static const SHORT SERVO_RATE = 500;
static const UNSIGNED FLOW_SCALER = 1667;
static const SHORT SERVO_SCALE = 8;

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExhPressureController()
//
//    Processing: This is the constructor for the ExhPressureController class.
//          This routine calls the Controller() constructor.
//
//    Input Parameters:
//          stepper - Step Motor controlled by controller
//
//    Output Parameters:  None
//
//    Return Values:      ptr to instance of controller
//
//    Pre-Conditions: The sensor and step motor objects are assumed
//              to already have been instantiated.
//    Miscellaneous: None
//
//    Requirements:
//
//******************************************************************************
ExhPressureController::ExhPressureController(
    StepMotor  *     stepper) :
    Controller()
{
    // Init ptr to associated stepper motor
    ASSERTION( stepper != NULL );
    StepMotorPtr = stepper;

    // Initialize these attributes.  During normal operation, the Reset()
    // operation is invoked and the values are calculated.  However, during
    // diagnostics, the exhalation controller can also be commanded to control
    // PEEP.  In this scenario, BD settings are not constructed and cannot
    // be used to set values, so they are just set to something known here.
    CurrentMeasured = 0;
    RunningSum = 0;
    firstIntegrator = 0;
    PreFilterIntegrator = 0;
    preFilterPress = 0;
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Reset()
//
//    Processing: The Reset() operation sets the initial value of the
//      accumulated integral error and sets the exhalation valve to
//      almost open.
//      This operation is invoked from the Enable() method each time the
//      controller is enabled.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void ExhPressureController::Reset(void)
{
    firstIntegrator = 0;
    PreFilterIntegrator = 0;
    preFilterPress = 0;
}   //  end Reset()

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: This is the main processing loop for the
//          ExhPressure Controller.
//          The routine obtains the current sensor reading, calculates the step
//          motor position, and commands the step motor.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void ExhPressureController::Run(void)
{
    if(Enabled != eTrue)
        return;
    // Get current sensor reading
    CurrentMeasured = PressureSensor::ActivePressureSensor->GetLastReading();

    // Calculate step position
    Calculate();

    // Command Step Motor
    StepMotorPtr->SetStepPosition(OutputValue);

}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Calculate()
//
//    Processing: This routine calculates the next step position to command the
//      motor in order to achieve a smooth transition to the desired pressure
//      level.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void ExhPressureController::Calculate(void)
{
    LONG error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
    LONG scaleUpInput;

    error = Desired - CurrentMeasured;

    errorGain = error * 100;

    scaleUpInput =  errorGain / SERVO_RATE ;
    scaleUpInput = (LONG) (scaleUpInput << SERVO_SCALE);

    scaleDownIntegrator = (LONG) (firstIntegrator >> SERVO_SCALE);

    preFilterOutput = scaleDownIntegrator * 20;

    preFilterOutput /= (SHORT)FLOW_SCALER;


    if ( preFilterOutput >= 2000)
    {
        if ( errorGain <= 0 )
        {
            firstIntegrator += scaleUpInput ;
        }
        preFilterOutput = 2000;
    }
    else if ( preFilterOutput <= 0 )
    {
        if ( errorGain >= 0 )
        {
            firstIntegrator += scaleUpInput ;
        }
        preFilterOutput = 0;
    }
    else
    {
        firstIntegrator += scaleUpInput ;
    }

    valStep = preFilterOutput;

    OutputValue = valStep;

}   // end Calculate()

//******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PrefilterBlock()
//    Processing:
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void ExhPressureController::PreFilterSetting(void)
{
    LONG  scaleUpInput,error;

    preFilterPress = (LONG)(PreFilterIntegrator >> SERVO_SCALE);  // scale down before feedback.

    error = Desired - preFilterPress;

    scaleUpInput = error << SERVO_SCALE;   // scale up before go into integrator.

    PreFilterIntegrator += scaleUpInput / 10;
}


