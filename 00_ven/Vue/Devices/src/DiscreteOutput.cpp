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
#include "stdio.h"
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
//                                Driveri2317 * driver,
                                USHORT chanel,
                                E_State normally,
                                E_State initialState)
{
    // PRE_CONDITIONS
//    ASSERTION( driver != NULL );

    // Assign attribute values
//    Driver = driver;
    Chanel =  chanel;

    State = initialState;
    Normally = normally;
    DesiredState = initialState;

    // Place the output in the Desired intial state
    SetStateDirect(initialState);
    ID =id;
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
//            Driver->SetBit(Chanel);
        }
        else
        {
//            Driver->ClrBit(Chanel);
        }
    }
    else {
        // determine whether to Set or Clear the bit to achieve the "Off" state
        if (Normally == eOff)
        {
//            Driver->ClrBit(Chanel);
        }
        else
        {
//            Driver->SetBit(Chanel);
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
//    Driver->SetBit(Chanel);
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
//    Driver->ClrBit(Chanel);
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

        }
//            Driver->SetBit(Chanel);
        else
        {

        }
//            Driver->ClrBit(Chanel);
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
void DiscreteOutput::S_Init(void)
{
    // Exhalation Pressure Auto-Zero Solenoid
//    ExhPressureZero = new DiscreteOutput(eExhPressZero, Digitali2317, 4);
//    ASSERTION( ExhPressureZero != NULL );
//
//    // Inhalation Pressure Auto-Zero Solenoid
//    InhPressureZero = new DiscreteOutput(eInhPressZero, Digitali2317, 5);
//    ASSERTION( InhPressureZero != NULL );
//
//    SafetyValve = new DiscreteOutput(eSafetyValve, Digitali2317, 6);
//    ASSERTION( SafetyValve != NULL );
//
//    InpedanceLine = new DiscreteOutput(eInpedanceLine, Digitali2317, 7);
//    ASSERTION( InpedanceLine != NULL );
//
//    // Create DiscreteOutput objects used by Morph
//    PurgeFlow = new DiscreteOutput(ePurgeFlow, Digitali2317, 8);
//    ASSERTION( PurgeFlow != NULL );
//
//    OilPump = new DiscreteOutput(eOilPump, Digitali2317, 9,eOff,eOff);
//    ASSERTION( OilPump != NULL );
//
//    RemoteAlarm = new DiscreteOutput(eRemoteAlarm, Digitali2317, 10);
//    ASSERTION( RemoteAlarm != NULL );
//
//    EnableIO = new DiscreteOutput(eIOEnable, Digitali2317, 15);
//    ASSERTION( EnableIO != NULL );
//
//    ProxyFlowResetLine = new DiscreteOutput(eProxyFlowResetLine, Digitali2317, 30, eOff, eOn);
//    ASSERTION( ProxyFlowResetLine != NULL );
}
