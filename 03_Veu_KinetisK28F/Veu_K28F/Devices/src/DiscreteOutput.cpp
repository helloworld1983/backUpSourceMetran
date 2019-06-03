//******************************************************************************//
//$COMMON.CPP$
//   File Name:  DiscreteOutput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the methods of the DigitalOutput class.
//
//   Interfaces:
//      Derived Driver classes - interfaces to I/O hardware
//      Location data structure - HW mapping of output signal
//
//   Restrictions: None
//
//******************************************************************************

#include "DiscreteOutput.h"
#include "DebugTrace.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: DiscreteOutput()
//
//    Processing: This constructor creates and initializes 
//          DiscreteOutput objects
//
//    Input Parameters:
//          driver - I/O driver to which signal is connected.
//          port   - port assignment
//          bit    - bit position assignment
//          normally - wiring configuration, state when "de-energized" or "low
//          initialState - initial desired state.
//
//    Output Parameters: None
//
//    Return Values: ptr to created object.
//
//    Pre-Conditions:
//          The Driver objects to which the outputs are assigned must exist
//          prior to the creation of the DiscreteOutput objects.
//
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
DiscreteOutput::DiscreteOutput( E_DeviceId id,
                                uint8_t  uPort, uint8_t uPin, uint32_t uGpioPort, uint8_t uGpioPin, uint16_t uFuction,
                                E_State normally,
                                E_State initialState)
{
    State = initialState;
    Normally = normally;
    DesiredState = initialState;

    // Place the output in the Desired intial state
    SetStateDirect(initialState);
    ID = id;

    Port = uPort;
    Pin = uPin;
    GpioPort = uGpioPort;
    GpioPin = uGpioPin;
    Function = uFuction;
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetState()
//
//    Processing: This routine sets the internal State attribute of the object.
//          It does not actually output to the hardware.
//          It relies on the DataIOTask to periodically drive the outputs
//          to the States specified.
//
//    Input Parameters: desired state
//          This value relates to the intended use of the object regardless
//          of whether that actually means writing a 1 or a 0 to the hardware.
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:
//      The DiscreteOutput objects exists and has a valid Location object.
//      The DataIOTask is assumed to actually drive the hardware
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::SetState(E_State desired)
{

    // Set state attribute depending on desired value and default state.
    if (desired == eOn) {
        // determine whether to Set or Clear the bit to achieve the "On" state
        if (Normally == eOff)
        {
            SetHigh();
        }
        else
        {
            SetLow();
        }
    }
    else {
        // determine whether to Set or Clear the bit to achieve the "Off" state
        if (Normally == eOff)
        {
            SetLow();
        }
        else
        {
            SetHigh();
        }
    }
    // Save the value
    State = desired;
    return;

}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetHigh()
//
//    Processing: Set the output bit to the energized state.
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::SetHigh(void)
{
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin, true);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetLow()
//
//    Processing: Set the output bit to the de-energized state.
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::SetLow(void)
{
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin,false);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetStateDirect()
//
//    Processing: This routine drives the output to the desired state.
//
//    Input Parameters: desired - state to drive the output to.
//          This value relates to the intended use of the object.
//          If desired is eOn the object will be turned "on" regardless
//          of whether that actually means writing a 1 or a 0 to the hardware.
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:
//      The DiscreteOutput objects exists and has a valid Location object.
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::SetStateDirect(E_State desired)
{
    if(State != desired)
    {
        if (desired == eOn)
        {
            SetHigh();
        }
        else
        {
            SetLow();
        }
        State = desired;
        DesiredState = State;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: S_Init()
//
//    Processing: This routine instanciates and initializes all of the
//          DiscreteOutput signals used in the Morph system.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:
//          This routine is specific to the hardware configuration of the 
//          particular project.  Changes in hardware configuration or ports
//          to other projects will require changes to this routine.
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::Init(void)
{
//    Chip_SCU_PinMuxSet(Port, Pin, (Function));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, GpioPort, GpioPin);
//
//    //Set PIN DEFAULT to LOW
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin,false);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetDesiredState (E_State desired)
//
//    Processing: Set desire state
//
//    Input Parameters: desire state
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions: NOne
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void DiscreteOutput::SetDesiredState (E_State desired)
{
    DesiredState = desired;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetDesiredState(void)
//
//    Processing: get desire state
//
//    Input Parameters:  None
//    Output : desire state
//    Return Values:    None
//    Pre-Conditions: NOne
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
E_State DiscreteOutput::GetDesiredState(void)
{
    return DesiredState;
}

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
E_State DiscreteOutput::GetState()
{
    return (State);   // return current State value
}
