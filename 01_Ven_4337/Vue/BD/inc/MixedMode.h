/******************************************************************************/
//$COMMON.H$
//   File Name:  MixedMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This class defines the Mixed Mode of Ventilation.  The terminology
//      Mixed Mode is used to refer to either the SIMV mode in Pressure or
//      Volume Ventilation and the Spontaneous/Timed Mode in Non-Invasive
//      Ventilation.
//
//      The Mixed Mode is a mode that mixes mandatory and spontaneous breathing. 
//      The MixedMode is derived from the base class Mode.
/******************************************************************************/
#ifndef MIXEDMODE_H
#define MIXEDMODE_H

#include "NormBreathingMode.h"

enum E_WindowType
{
    eMandWindow,
    eSpontWindow
};

enum BreathType
{
    eTestBreath,
    eVABreath
};

class MixedMode : public NormBreathingMode
{
    public:
        //Pointer to the object instantiated from this class
        static MixedMode* S_GetInstance (void);

        //Process doing something when another mode enters this mode
        void Enter (E_VentilationMode fromModeId);

        //Return main window or not
        E_WindowType GetWindow(void);

        //$COMMON.ATTRIBUTE$
        //  Name: Window
        //  Description:  Mandatory or spontaneous window.
        //  Units: None
        //  Range: n/a
        E_WindowType Window;

    protected:
        //Constructor for this class
        MixedMode (void);

        //This operation handles the processing of Pt inhalation trigger
        void ProcessPtInhTriggerAC(void);

    private:
        //Copy constructor for class
        MixedMode (const MixedMode&);

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //This operation handles the processing of the Patient Inhalation Trigger for mix mode
        virtual void ProcessPtInhTrigger (void);

        //This operation handles the processing of the Breath Period Time Trigger for mix mode
        virtual void ProcessBpTimeTrigger (void);

        // Request Trigger for the Mixed Mode
        virtual void ProcessManualInhOpReqTrigger (void);

        //handles the processing of the Exhalation Hold Trigger for MIx Mode
        virtual void ProcessExhHoldTrigger (void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static MixedMode* S_Instance;

};

#endif // MIXEDMODE_H
