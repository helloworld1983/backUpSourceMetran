//******************************************************************************
//$COMMON.H$
//    File Name: DInput.h
//    Copyright 1997 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  DiscreteInput - this class represents the discrete input
//          signals that are used in the system.
//
//      Description: The discrete input class is used to represent
//          the various input signals of the Morph ventilator.
//          The DiscreteInput class provides an abstraction for a
//          generic dicrete input signal and serves as a base class
//          for more specific derived classes (Key, DualKey Classes).
//******************************************************************************
#ifndef INC_DISCRETEINPUT_H_
#define INC_DISCRETEINPUT_H_

#include "standard.h"
#include "CommonTypes.h"
#include "BdInterface.h"
#include "stdint.h"

class DiscreteInput
{
    public:
        DiscreteInput( E_DeviceId id,               // device ID
                       uint8_t  uPort, uint8_t uPin, uint32_t uGpioPort, uint8_t uGpioPin, uint16_t uFuction,//port, pin for device
                       E_State    normally = eOff); // state when de-energized.

        // Instanciates and initializes all objects
        void Init(void);

        //set high state
        void SetHigh(void);

        //set low state
        void SetLow(void);

        //read data of pin
        bool ReadGPIO(void);

        // updates state value
        virtual void SetState(E_State desired);

        // returns last obtained state
        E_State GetState(void);

        // reads current state from HW
        E_State GetStateDirect(void);

    private:
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

        //destructor
        virtual ~DiscreteInput(void){};
    protected:

        //$COMMON.ATTRIBUTE$
        //    Name:         Normally
        //    Description: state of input when de-energized
        //    Units: None
        //    Range: eOn, eOff
        E_State     Normally;

        //    Name:        State
        //    Description: represents current value of input signal
        //    Units: eState
        //    Range: eOn, eOff, eUnknown
        E_State     State;

};
#endif /* INC_DISCRETEINPUT_H_ */
