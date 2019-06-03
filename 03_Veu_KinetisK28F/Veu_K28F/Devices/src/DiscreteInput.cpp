//******************************************************************************//
//$COMMON.CPP$
//   File Name:  DiscreteInput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the methods of the DiscreteInput class.
//
//   Restrictions: None
//
//******************************************************************************
#include "DiscreteInput.h"
#include "DebugTrace.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: DiscreteInput()
//
//    Processing: This constructor creates a DiscreteInput object.
//
//    Input Parameters:
//          id - unique identifier for signal
//          driver - pointer to associated I/O driver
//          port - port assignment
//          bit - bit position assignment
//          normally - default state
//    Output Parameters: None
//    Return Values:    pointer to created object.
//    Pre-Conditions:
//          The related driver must already be instantiated.
//          Port values must be between 0 and 2.
//          Bit values must be between 0 and 7.
//    Miscellaneous:   None
//    Requirements:
//
//******************************************************************************
DiscreteInput::DiscreteInput( E_DeviceId id,
                              uint8_t  uPort, uint8_t uPin, uint32_t uGpioPort, uint8_t uGpioPin, uint16_t uFuction,
                              E_State normally)
{
    Normally = normally;
    State = eUnknown;

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
//    Processing: Sets the state attribute.
//          This method is used by the driver to update the state attribute
//          when the current reading is obtained.
//    Input Parameters: desired
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:
//      Assumes desired is eOn or eOff.
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void DiscreteInput::SetState(E_State desired)
{
    // PRE_CONDITIONS
    //    ASSERTION( (desired  == eOn) || (desired  == eOff) );
    //    DEBUG_ASSERTION( (Normally == eOn) || (Normally == eOff) );

    // Set state attribute depending on desired value and default state.
    if (desired == eOn)
        if (Normally == eOff)
        {
            State = eOn;
        }
        else    // if default state is eOn - turn it eOff when energized
        {
            State = eOff;
        }
    else
        if (Normally == eOff)
        {
            State = eOff;
        }
        else    // if default state is eOn - turn it eOn when de-energized
        {
            State = eOn;
        }

    return;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetHigh()
//
//    Processing: Set hight state
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    current state of input signal.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void DiscreteInput::SetHigh (void)
{
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin, true);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetLow()
//
//    Processing: set low state
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    current state of input signal.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void DiscreteInput::SetLow (void)
{
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin, false);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetState()
//
//    Processing: Get state
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    current state of input signal.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
E_State DiscreteInput::GetState (void)
{
    return (State);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetStateDirect()
//
//    Processing: This method reads the HW to obtain the current state
//          of the input and returns that value
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    current state of input signal.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
E_State DiscreteInput::GetStateDirect()
{
    E_State state;
    SHORT  testLevel = ReadGPIO();

    // Determine on/off state
    if (testLevel == 1)
    {
        state = eOff;
    }
    else
    {
        state = eOn;
    }

    State = state;
    return (State);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Init()
//
//    Processing: This routine instanciates and initializes all
//          of the DiscreteInput objects in the system.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   Driver objects must first be instantiated.
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void DiscreteInput::Init()
{
//    Chip_SCU_PinMuxSet(Port, Pin, (Function));
//    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, GpioPort, GpioPin);

    //Set PIN DEFAULT to LOW
//    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GpioPort, GpioPin,false);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ReadGPIO (void)
//
//    Processing: Check status of pin
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    current state of input signal.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
bool DiscreteInput::ReadGPIO (void)
{
    bool bStatus = false;

//    if(Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,GpioPort, GpioPin))
//    {
//        bStatus = true;
//        State = eOn;
//    }
//    else
//    {
//        bStatus = false;
//        State = eOff;
//    }

    return bStatus;
}
