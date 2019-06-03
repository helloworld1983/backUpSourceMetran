//******************************************************************************
//$COMMON.H$
//    File Name: MgrDiscreteOutput.h
//    Copyright 1997 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  DiscreteOutput
//
//      Description: The discrete output class is used to represent
//          the various output signals of the Morph ventilator.
//          The DiscreteOutput class provides an abstraction for a
//          generic dicrete output signal and serves as a base class
//          for more specific derived classes (Led Class).
//
//******************************************************************************
#ifndef INC_MGRDISCRETEOUTPUT_H_
#define INC_MGRDISCRETEOUTPUT_H_

#include "DiscreteOutput.h"

class MgrDiscreteOutput
{
public:
    // static instance of PressureSensor class
    static MgrDiscreteOutput *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: InpedanceLine
    //
    //    Description: Pointer to instance of Inpedance Line
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * InpedanceLine;

    //$COMMON.ATTRIBUTE$
    //    Name: SafetyValve
    //
    //    Description: Pointer to instance of Safety valve
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * SafetyValve;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhPressureZero
    //
    //    Description: Pointer to instance of exhalation pressure
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * ExhPressureZero;

    //$COMMON.ATTRIBUTE$
    //    Name: InhPressureZero
    //
    //    Description: Pointer to instance of inhalation pressure
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * InhPressureZero;

    //$COMMON.ATTRIBUTE$
    //    Name: PurgeFlow
    //
    //    Description: Pointer to instance of purge flow
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * PurgeFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: OilPump
    //
    //    Description: Pointer to instance of oil pump
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * OilPump;

    //$COMMON.ATTRIBUTE$
    //    Name: Enable24V
    //
    //    Description: Pointer to instance of enable IO
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteOutput * Enable24V;

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of PressureSensor class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrDiscreteOutput *S_Instance;

    //MgrDiscreteOutput constructor
    MgrDiscreteOutput(void);

};

#endif /* INC_MGRDISCRETEOUTPUT_H_ */
