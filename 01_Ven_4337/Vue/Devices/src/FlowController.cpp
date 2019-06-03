//*****************************************************************************
//$COMMON.CPP$
//    Filename: FlowController.cpp
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the FlowController class
//
//    Interfaces:
//      Air and O2 StepMotor objects
//      Air and O2 FlowSensor objects
//*****************************************************************************
#include "FlowController.h"
#include "DebugTrace.h"

// declare statics
LONG FlowController::AirIndexTable[NUMBER_AIR_GAINS];
LONG FlowController::O2IndexTable[NUMBER_O2_GAINS];
bool FlowController::exhphase = true;
int FlowController::speed = 40;//40;
int FlowController::delay = 0;
static const SHORT SERVO_RATE = 500;
static const UNSIGNED FLOW_SCALER = 1667;
static const SHORT SERVO_SCALE = 8;


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: FlowController()
//
//    Processing: This is the constructor for the FlowController class.
//          This routine calls the Controller() constructor.
//
//    Input Parameters:
//          integral - gain constant
//          proportional - gain constant
//          derivative - gain constant
//          sensor - FlowSensor used to close the loop
//          stepper - StepMotor controlled by the controller.
//    Output Parameters:  None
//    Return Values:      ptr to the FlowController object created.
//    Pre-Conditions: The sensor and step motor objects are assumed
//              to already have been instantiated.
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
FlowController::FlowController(
        E_FlowType type,
        FlowSensor *sensor,
        StepMotor  * stepper)
:    Controller()
{
    PropRefWeight = 0;
    Alpha = 0;
    Beta = 0;
    PrevProportional = 0;
    UpperTarget = 0;

//    LONG i = 0;

    // PRECONDITIONS
    ASSERTION( sensor  != NULL );
    ASSERTION( stepper != NULL );

    // Setup pointers to related devices
    SensorPtr    = sensor;
    StepMotorPtr = stepper;
    SensorType = type;

    OutputValue = 0;   
    firstIntegrator = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Run()
//
//    Processing: This is the main processing loop for the Flow Controllers.
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
void FlowController::Run(void)
{
    // Get current sensor reading

    if (Enabled)
    {
        PreviousMeasured = CurrentMeasured;
        CurrentMeasured  = SensorPtr->GetLastReading();

        // Calculate step position
        Calculate();
        // Command Step Motor
        if(OutputValue < StepMotorPtr->Liftoff)
            OutputValue = StepMotorPtr->Liftoff;
        StepMotorPtr->SetStepPosition(OutputValue);
    }
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Calculate()
//
//    Processing: This routine calculates the desired step motor position
//          for the current execution cycle.  The current sensor reading
//          and desired sensor readings are used to generate a desired step
//          position. 
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:  
//
//******************************************************************************
void FlowController::Calculate(void)
{
    OutputValue = CompStep();    
}
LONG FlowController::CompStep(void)
{
    PreviousMeasured = CurrentMeasured;
    CurrentMeasured  = SensorPtr->GetCompensatedReading();

    LONG error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
    LONG scaleUpInput;

    error = Desired - CurrentMeasured;

    errorGain = error * 100;

    scaleUpInput =  errorGain / SERVO_RATE ;
    scaleUpInput = (LONG) (scaleUpInput << SERVO_SCALE )*3;

    scaleDownIntegrator = (LONG) (firstIntegrator >> SERVO_SCALE);    

    preFilterOutput = scaleDownIntegrator * speed;

    preFilterOutput /= (SHORT)FLOW_SCALER;

    if ( preFilterOutput >= MAX_STEP_POSITION)
    {
        if ( errorGain <= 0 )
        {
            firstIntegrator += scaleUpInput ;
        }
        preFilterOutput = MAX_STEP_POSITION;
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

    return valStep;
}
void FlowController::SetStaticAgr(LONG step)
{
    const SHORT FLOW_SCALE = 1666;
    LONG step_t = step*FLOW_SCALE;

    firstIntegrator = (LONG)((LONG)(step_t/speed)) << SERVO_SCALE;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Reset()
//
//    Processing:
//      This operation is invoked at the start of each inhalation to reset
//      the running sum and the cycle number.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void FlowController::Reset(void)
{
    firstIntegrator = 0;    
}   //  end Reset()



