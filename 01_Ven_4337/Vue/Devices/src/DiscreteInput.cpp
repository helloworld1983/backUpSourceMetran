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
//        Driveri2317 * driver,
        USHORT chanel,
        E_State normally)
{
    // PRE_CONDITIONS
//    ASSERTION( driver != NULL );
//    ASSERTION((chanel >= 16)&&(chanel <= 32));

    // Set attribute values
    Chanel =  chanel;

    Normally = normally;
    State = eUnknown;

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
    // PRE CONDITIONS
//    ASSERTION( Driver != NULL );
    E_State state;
    SHORT  testLevel = 0;

       // now set the direction and read digital I/O
//    Driver->i2317DirectionSet (Chanel, I2317_IODIR_CHAN_INPUT);
//    testLevel = Driver->i2317InputGet(Chanel);

    // Determine on/off state
    if (testLevel == 0)
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
//    Operation Name: S_Init()
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
void DiscreteInput::S_Init()
{

//    O2PressSwitch  = new DiscreteInput(eO2Pressure, Digitali2317, 16);
//    ASSERTION( O2PressSwitch != NULL );
//
//    AirPressSwitch  = new DiscreteInput(eAirPressure, Digitali2317, 17);
//    ASSERTION( AirPressSwitch != NULL );
//
//    ManualBreathInput  = new DiscreteInput(eManualBreathInput, Digitali2317, 18);
//    ASSERTION( ManualBreathInput != NULL );
//
//    ManualBreathConnection  = new DiscreteInput(eManualBreathConnection, Digitali2317, 19);
//    ASSERTION( ManualBreathConnection != NULL );
//
//    HFODoorSwitch  = new DiscreteInput(eHFODoorSwitch, Digitali2317, 20);
//    ASSERTION( HFODoorSwitch != NULL );
//
//    HFOPistonSwitch  = new DiscreteInput(eHFOPistonSwitch, Digitali2317, 21);
//    ASSERTION( HFODoorSwitch != NULL );
//
//    O2MotorStatus  = new DiscreteInput(eO2MotorStatus, Digitali2317, 25);
//    ASSERTION( O2MotorStatus != NULL );
//
//    AirMotorStatus  = new DiscreteInput(eAirMotorStatus, Digitali2317, 26);
//    ASSERTION( AirMotorStatus != NULL );
//
//    ExhMotorStatus  = new DiscreteInput(eExhMotorStatus, Digitali2317, 27);
//    ASSERTION( ExhMotorStatus != NULL );
//
//    HFOMotorStatus  = new DiscreteInput(eHFOMotorStatus, Digitali2317, 28);
//    ASSERTION( HFOMotorStatus != NULL );
}
