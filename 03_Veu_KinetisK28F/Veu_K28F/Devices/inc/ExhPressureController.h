//*****************************************************************************
//$COMMON.H$
//   File Name:  EXHCONTR.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  ExhController
//    This class defines the task which controls the Exhalation Valve.
//
//    Description:
//    Closed loop pressure control is performed.
//    The exhalation pressure sensor readings and desired pressure level are
//    used in calculations to provide desired step positions for the
//    exhalation valve motor controller.
//*****************************************************************************
#ifndef INC_EXHPRESSURECONTROLLER_H_
#define INC_EXHPRESSURECONTROLLER_H_

#include "Controll.h"
#include "Stepmoto.h"

class ExhPressureController : public Controller
{
public:
    // Associated Step Motor
    ExhPressureController(StepMotor * stepper);

    // Sets initial conditions
    void Reset(void);

    // Task Entry Point
    void Run(void);

protected:
private:

    //$COMMON.ATTRIBUTE$
    //    Name:         StepMotorPtr
    //    Description:  Step Motor controlled by this controller.
    //    Units: None
    //    Range: N/A
    StepMotor *StepMotorPtr;   // StepMotor controlled

    LONG firstIntegrator;
    LONG PreFilterIntegrator;
    LONG preFilterPress;

    // Exhalation Control calculation
    void    Calculate(void);
    void    PreFilterSetting(void) ;
};

#endif /* INC_EXHPRESSURECONTROLLER_H_ */
