/******************************************************************************/
//$COMMON.H$
//   File Name:  OcclMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Occlusion Mode
//      which is derived from the NonVentMode base class.
/******************************************************************************/
#ifndef OCCLMODE_H
#define OCCLMODE_H

#include <NonVenMode.h>

class OcclMode : public NonVenMode
{
    public:
        //Pointer to the object instantiated from this class
        static OcclMode* S_GetInstance (void);

        //Process something when another mode enters and exits OcclMode
        virtual void Enter (E_VentilationMode fromModeId);
        virtual void Exit (void);

    protected:

    private:
        //Constructor of class
        OcclMode (void);

        //Copy constructor of class
        OcclMode (const OcclMode&);

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //This operation handles the processing of the Occlusion Auto-Reset
        virtual void ProcessOcclAutoResetTrigger (void);

        //updates the occlusion alarm status to indicate that occlusion is still active
        virtual void UpdateAlarmStatus (void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static OcclMode* S_Instance;
};

#endif // OCCLMODE_H
