//*****************************************************************************
//$COMMON.H$
//    Filename: flow_cnt.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  FlowController
//    This class defines the tasks which perform the flow control
//    calculations and sends the desired flow valve step positions
//    the the StepMotor objects for the air and oxygen flow valves.
//*****************************************************************************
#ifndef    FLOW_CNT_H
#define    FLOW_CNT_H

#include "Controll.h"
#include "flowsens.h"
#include "Stepmoto.h"

static const LONG NUMBER_AIR_GAINS = 12;
static const LONG NUMBER_O2_GAINS = 28;

class FlowController : public Controller
{
public:
    //Constructor
    FlowController(
            E_FlowType type,
            FlowSensor * sensor,     // Flow Sensor controller uses
            StepMotor  * stepper);   // Step Motor controlled

    // Sets initial conditions
    void Reset(void);
    // Task Entry Point
    void Run(void);

    void SetStaticAgr(LONG value);
    inline E_FlowType GetSensorType(void){return SensorType;}
    inline LONG GetOutputValue(void){return OutputValue;}

    LONG CompStep(void);

    inline void Enable(void){Enabled = eTrue;}

private:
    static bool exhphase;
    static int speed;
    static int delay;
    // Flow Control Calculation routine
    void Calculate(void);

    //$COMMON.ATTRIBUTE$
    //    Name:        SensorPtr
    //    Description: Flow sensor used to close the flow control loop
    //    Units: None
    //    Range: N/A
    FlowSensor *    SensorPtr;      // Flow Sensor tied to controller

    //$COMMON.ATTRIBUTE$
    //    Name:         StepMotorPtr
    //    Description:  Step Motor controlled by the controller
    //    Units: None
    //    Range: N/A
    StepMotor  *    StepMotorPtr;   // Step Motor tied to controller

    //$COMMON.ATTRIBUTE$
    //    Name:        Type
    //    Description: Denotes whether this flow controller is used for
    //          air or oxygen.
    //    Units: None
    //    Range: N/A
    E_FlowType SensorType;

    //$COMMON.ATTRIBUTE$
    //    Name:        PropRefWeight
    //    Description: Multiplier used to adjust proportional reference value
    //    Units: None
    //    Range: N/A
    LONG PropRefWeight;

    //$COMMON.ATTRIBUTE$
    //  Name: Alpha
    //  Description:  Alpha term used in the low order filter calculation.
    //      This value is scheduled based upon patient type.
    //  Units: none
    //  Range: n/a
    LONG    Alpha;

    //$COMMON.ATTRIBUTE$
    //  Name: Beta
    //  Description:  Beta term used in the low order filter calculation.
    //      This value is scheduled based upon patient type.
    //  Units: none
    //  Range: n/a
    LONG    Beta;

    //$COMMON.ATTRIBUTE$
    //  Name: PrevProportional
    //  Description:  proportional term from previous iteration.
    //      Used for smoothing.
    //  Units: none
    //  Range: n/a
    LONG PrevProportional;

    //$COMMON.ATTRIBUTE$
    //  Name: Upper Target
    //  Description:  Target value for controller to achieve during breath.
    //  Units: cmH20
    //  Range: 0 - 100
    LONG UpperTarget;    

    //$COMMON.ATTRIBUTE$
    //  Name: AirIndexTable
    //  Description: This table is built from the first column
    //      of the gain tables. It is built up during construction.
    //      This array of LONGS  (LONG[]) is the interface needed
    //      to use the common BinarySearch() utility.
    //  Units: LPM
    //  Range: 0-200
    static LONG AirIndexTable[NUMBER_AIR_GAINS];

    //  Name: O2IndexTable
    //  Description: This table is built from the first column
    //      of the gain tables. It is built up during construction.
    //      This array of LONGS  (LONG[]) is the interface needed
    //      to use the common BinarySearch() utility.
    //  Units: LPM
    //  Range: 0-200
    static LONG O2IndexTable[NUMBER_O2_GAINS];

    LONG firstIntegrator;

};
#endif    //FLOW_CNT_H
