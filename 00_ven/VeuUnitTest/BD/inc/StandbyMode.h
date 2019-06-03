/******************************************************************************/
//$COMMON.H$
//   File Name:  StandbyMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Start-Up Mode which
//      is derived from the Mode base class.
/******************************************************************************/
#ifndef STARTUPMODE_H
#define STARTUPMODE_H

#include "Mode.h"
#include "OpRequest.h"

class StandbyMode : public Mode
{
    public:
        //Pointer to the object instantiated from this class
        static StandbyMode* S_GetInstance(void);

        //Process to do when enter StandbyMode from E_VentilationMode fromModeId
        virtual void Enter(E_VentilationMode fromModeId);

        //Process to do when exit StandbyMode
        virtual void Exit(void);

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //Constructor of class
        StandbyMode(void);

        // The copy constructor is not used.
        StandbyMode(const StandbyMode&);

        //This operation simply performs error handling
        //It should never be invoked
        virtual void UpdateAlarmStatus(void);

        //This operation simply performs error handling
        //It should never be invoked
        void ProcessModeChangeTrigger(void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static StandbyMode* S_Instance;
};

#endif // STARTUPMODE_H
