/******************************************************************************/
//$COMMON.H$
//   File Name:  MEMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Start-Up Mode which
//      is derived from the Mode base class.
/******************************************************************************/
#ifndef MEMODE_H
#define MEMODE_H

#include "Mode.h"
#include "OpRequest.h"

class MEMode : public Mode
{
    public:
        //Pointer to the object instantiated from this class
        static MEMode* S_GetInstance(void);

        //Process doing something when another mode enter and exit to MEMode
        virtual void Enter(E_VentilationMode fromModeId);
        virtual void Exit(void);

        //Process action for each ID trigger
        void ProcessTriggerID(E_TriggerId id);

        //Constructor for class
        MEMode(void);

        // The copy constructor is not used.
        MEMode(const MEMode&);

        //This operation simply performs error handling.  It should never be invoke
        virtual void UpdateAlarmStatus(void);

        //This operation simply performs error handling.  It should never be invoke
        void ProcessModeChangeTrigger(void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static MEMode* S_Instance;
};

#endif // STARTUPMODE_H
