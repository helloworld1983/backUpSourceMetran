/******************************************************************************/
//$COMMON.H$
//   File Name:  NonVentMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Non-Ventilation Mode
//      which is derived from the Mode base class.
/******************************************************************************/
#ifndef NONVENTMODE_H
#define NONVENTMODE_H

#include "Mode.h"

class NonVenMode : public Mode
{
    public:
        //Constructor for the NonVent Mode
        NonVenMode (E_Bool doesManualReset = eFalse);

        //process something when another mode go to NonVenMode
        virtual void Enter (E_VentilationMode fromModeId);

        //process something when another mode get out NonVenMode
        virtual void Exit (void);

    protected:

    private:
        // The copy constructor is not used.
        NonVenMode (const NonVenMode&);
};

#endif // NONVENTMODE_H
