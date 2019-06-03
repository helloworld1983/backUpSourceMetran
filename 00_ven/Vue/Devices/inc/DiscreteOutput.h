//******************************************************************************
//$COMMON.H$
//    File Name: DOutput.h
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
#ifndef    DISCRETE_OUTPUT_H
#define    DISCRETE_OUTPUT_H


#include "standard.h"
#include "CommonTypes.h"
#include "BdInterface.h"

class DiscreteOutput
{

public:
    inline E_State GetState(void);  // returns current state of signal

    DiscreteOutput(E_DeviceId id,               // device ID
//                   Driveri2317 *  driver,       // associated I/O driver
                   USHORT     chanel,             // port assignment
                   E_State    normally = eOff,  // state when de-energized.
                   E_State    initialState = eOff);

    virtual void SetState(E_State desired);       // controls output signal
    virtual void SetStateDirect(E_State desired); // controls output signal
    virtual void SetHigh(void);     // applies 5V to output
    virtual void SetLow(void);      // applies 0V to output
    static void S_Init(void);       // Instanciates all DiscreteOutput objects.
    void SetDesiredState(E_State desired)
    {
        DesiredState = desired;
    }
    E_State GetDesiredState(void){return DesiredState;}
    virtual ~DiscreteOutput(void){}

private:

//    Driveri2317 * Driver;
    USHORT Chanel;
    E_DeviceId ID;

protected:

    //$COMMON.ATTRIBUTE$
    //    Name:        State
    //    Description: Represents commanded state of output signal.
    //          This attribute represents the intended use of the item.
    //          (Ex:Turn the HeatedFilter "On" regardless of
    //          how the hardware is wired)
    //    Units: E_State
    //    Range: eOn, eOff
    E_State     State;

    //$COMMON.ATTRIBUTE$
    //    Name:        DesiredState
    //    Description: Represents commanded state of output signal.
    //          This attribute represents the intended use of the item.
    //          (Ex:Turn the HeatedFilter "On" regardless of
    //          how the hardware is wired)
    //    Units: E_State
    //    Range: eOn, eOff
    E_State     DesiredState;

    //$COMMON.ATTRIBUTE$
    //    Name:        Normally
    //    Description: Represents the wiring configuration of the output.
    //          This determined whether a 1 or 0 is written to the hardware
    //          to achieve a "de-energized" or "low" state.
    //          eOn:  1 is low
    //          eOff: 0 is low (default)
    //          (Ex: a fail-safe relay contact may be wired so that
    //               0 Volts is the "On" position and 24 Volts is "Off".
    //               In this case the Normally attribute would be eOn.
    //    Units: E_State
    //    Range: eOn, eOff, eOpen, eClosed
    E_State     Normally;
};

//
//  Inline Methods
//

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetState()
//
//    Processing: Returns value of State attribute
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:  E_State
//    Pre-Conditions: None
//    Miscellaneous:
//      Does not actually "read" the hardware to get the state. It 
//      assumes that the state is the same as last output.
//    Requirements:
//
//******************************************************************************
inline E_State DiscreteOutput::GetState()
{
    return (State);   // return current State value
}
#endif  // DISCRETE_OUTPUT_H
