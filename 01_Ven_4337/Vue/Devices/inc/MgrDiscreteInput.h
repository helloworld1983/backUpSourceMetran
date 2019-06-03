//******************************************************************************
//$COMMON.H$
//    File Name: MgrDiscreteInput.h
//    Copyright 1997 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  DiscreteOutput
//
//      Description: The discrete output class is used to represent
//          the various output signals of the Morph ventilator.
//          The DiscreteInput class provides an abstraction for a
//          generic dicrete output signal and serves as a base class
//          for more specific derived classes (Led Class).
//
//******************************************************************************
#ifndef INC_MGRDISCRETEINPUT_H_
#define INC_MGRDISCRETEINPUT_H_

#include "DiscreteInput.h"

class MgrDiscreteInput
{
public:
    // static instance of MgrDiscreteInput class
    static MgrDiscreteInput *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: AirPressSwitch
    //
    //    Description: Pointer to instance of MgrDiscreteInput air pressure switch
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * AirPressSwitch;

    //$COMMON.ATTRIBUTE$
    //    Name: O2PressSwitch
    //
    //    Description: Pointer to instance of MgrDiscreteInput 02 pressure switch
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * O2PressSwitch;

    //$COMMON.ATTRIBUTE$
    //    Name: HFODoorSwitch
    //
    //    Description: Pointer to instance of MgrDiscreteInput HFO switch
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * HFODoorSwitch;

    //$COMMON.ATTRIBUTE$
    //    Name: HFOPistonSwitch
    //
    //    Description: Pointer to instance of MgrDiscreteInput HFO piston switch
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * HFOPistonSwitch;

    //$COMMON.ATTRIBUTE$
    //    Name: ManualBreathInput
    //
    //    Description: Pointer to instance of MgrDiscreteInput breath input
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * ManualBreathInput;

    //$COMMON.ATTRIBUTE$
    //    Name: ManualBreathConnection
    //
    //    Description: Pointer to instance of MgrDiscreteInput breath connection
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * ManualBreathConnection;

    //$COMMON.ATTRIBUTE$
    //    Name: O2MotorStatus
    //
    //    Description: Pointer to instance of MgrDiscreteInput 02 motor
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * O2MotorStatus;

    //$COMMON.ATTRIBUTE$
    //    Name: AirMotorStatus
    //
    //    Description: Pointer to instance of MgrDiscreteInput air motor
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * AirMotorStatus;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhMotorStatus
    //
    //    Description: Pointer to instance of MgrDiscreteInput exhalation motor
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * ExhMotorStatus;

    //$COMMON.ATTRIBUTE$
    //    Name: HFOMotorStatus
    //
    //    Description: Pointer to instance of MgrDiscreteInput hfo motor
    //    Units: None
    //
    //    Range: N/A
    //
    static DiscreteInput * HFOMotorStatus;


protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of MgrDiscreteInput class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrDiscreteInput *S_Instance;

    //MgrDiscreteInput constructor
    MgrDiscreteInput(void);
};

#endif /* INC_MGRDISCRETEINPUT_H_ */
