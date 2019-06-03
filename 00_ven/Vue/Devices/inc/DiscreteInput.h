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

#include "CommonTypes.h"
#include "BdInterface.h"

class DiscreteInput
{
    public:
        DiscreteInput( E_DeviceId id,               // device ID
//                       Driveri2317 *  driver,       // associated I/O driver
                       USHORT     chanel,             // chanel assignment
                       E_State    normally = eOff); // state when de-energized.


        virtual void SetState(E_State desired); // updates state value
        E_State GetState(void);                     // returns last obtained state
        E_State GetStateDirect(void);               // reads current state from HW

        static void S_Init(void);       // Instanciates and initializes all objects

    private:
        virtual ~DiscreteInput(void){};
//        Driveri2317 * Driver;
        USHORT Chanel;

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
