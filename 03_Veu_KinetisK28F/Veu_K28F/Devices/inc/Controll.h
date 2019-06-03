//*****************************************************************************
//$COMMON.H$
//   File Name:  CONTROLL.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  Controller
//    This class is the base class for the Morph software controllers.
//    The derived classed include FlowController, PressureController,
//    and ExhalationPressureController.
//*****************************************************************************
#ifndef CONTROLL_H
#define CONTROLL_H

#include "CommonTypes.h"

#define MAX_HFO_FREQ    (17)
#define MIN_HFO_FREQ    (5)

enum E_BoostStatus
{
    eBoostInactive,
    eBoostLatched,
    eBoostReset
};

class Controller
{
public:
    //Constructor
    Controller (void);

    // Disables motor control
    virtual void  Disable(void);

    // Returns enabled status
    E_Bool IsEnabled(void);

    // Returns desired value
    LONG GetDesired(void);

    // Sets target value for controller
    virtual void  SetDesired(LONG rate);

    // Sets initial conditions
    virtual void  Reset(void);

    // Enables motor control
    virtual void  Enable(void);

    // Pure Virtual Calculation routine
    virtual void  Calculate(void) = 0;

protected:
    //Destructor
    virtual ~Controller(void){};

    //$COMMON.ATTRIBUTE$
    //    Name:        Desired
    //    Description: objective for controller to attempt to achieve
    //    Units: dependent on derived class
    //    Range: dependent on derived class
    LONG   Desired;

    //$COMMON.ATTRIBUTE$
    //    Name:        CurrentMeasured
    //    Description: current value obtained from sensor
    //    Units: dependent on derived class
    //    Range: dependent on derived class
    LONG   CurrentMeasured;

    //$COMMON.ATTRIBUTE$
    //    Name:        PreviousMeasured
    //    Description: sensor value obtained from previous iteration
    //    Units: dependent on derived class
    //    Range: dependent on derived class
    LONG   PreviousMeasured;

    //$COMMON.ATTRIBUTE$
    //    Name:        OutputValue
    //    Description: Computed Step Position to send to StepMotor object.
    //    Units: dependent on derived class (Step position or flow rate)
    //    Range: 0 - 2000
    LONG   OutputValue;

    //$COMMON.ATTRIBUTE$
    //    Name:        ProportionalGain
    //    Description: Gain Constant for proportional factor.
    //    Units: ????
    //    Range: n/a
    LONG   ProportionalGain;

    //$COMMON.ATTRIBUTE$
    //    Name:        CompensatedDerivativeGain
    //    Description: Gain constant with time factor incorporated.
    //          Used in calculating pseudo derivative factor.
    //    Units: None
    //    Range: N/A
    LONG CompensatedDerivativeGain;

    //$COMMON.ATTRIBUTE$
    //    Name:         CompensatedIntegralGain
    //    Description: Gain constant with time factor incorporated.
    //          Used in calculating integral factor.
    //    Units: None
    //    Range: N/A
    LONG CompensatedIntegralGain;

    //$COMMON.ATTRIBUTE$
    //    Name:        RunningSum
    //    Description: accumulator for integral factor
    //    Units: dependent on derived class
    //    Range: dependent on derived class
    LONG   RunningSum;

    //$COMMON.ATTRIBUTE$
    //    Name:        Enabled
    //    Description: Flag denoting whether to perform calculation
    //                 and motor control.
    //    Units: E_Bool
    //    Range: eTrue, eFalse
    E_Bool  Enabled;
};

#endif   //CONTROLL_H
