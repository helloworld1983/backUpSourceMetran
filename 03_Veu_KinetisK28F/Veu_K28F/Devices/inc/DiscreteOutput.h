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
#include "stdint.h"

class DiscreteOutput
{
    public:
        //constructor.
        DiscreteOutput(E_DeviceId id,               // device ID
                       uint8_t  uPort, uint8_t uPin, uint32_t uGpioPort, uint8_t uGpioPin, uint16_t uFuction,//port, pin for device
                       E_State normally = eOff,  // state when de-energized.
                       E_State initialState = eOff);

        // returns current state of signal.
        E_State GetState(void);

        // controls output signal
        virtual void SetState(E_State desired);

        // controls output signal
        virtual void SetStateDirect(E_State desired);

        // applies 5V to output.
        virtual void SetHigh(void);

        // applies 0V to output.
        virtual void SetLow(void);

        // Instanciates all DiscreteOutput objects.
        void Init(void);

        //Set desire state
        void SetDesiredState(E_State desired);

        //get desifre state
        E_State GetDesiredState(void);

        //destructor
        virtual ~DiscreteOutput(void){}

    private:
        //$COMMON.ATTRIBUTE$
        //    Name:        ID
        //    Description: Device id
        //    Units: None
        //    Range: None
        E_DeviceId ID;

        //$COMMON.ATTRIBUTE$
        //    Name:        Port, Pin, GpioPort, GpioPin
        //    Description: Device id
        //    Units: None
        //    Range: None
        uint8_t  Port;
        uint8_t Pin;
        uint32_t GpioPort;
        uint8_t GpioPin;
        uint16_t Function;

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

#endif  // DISCRETE_OUTPUT_H
