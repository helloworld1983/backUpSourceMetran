/******************************************************************************/
//$COMMON.H$
//   File Name:  NoGasMode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the No Gas Mode which 
//      is derived from the NonVentMode base class.
/******************************************************************************/
#ifndef NOGASMODE_H
#define NOGASMODE_H

#include <NonVenMode.h>

// The Blower Spin Down State indicates the status of the blower.  When an 
// air source fault is first detected, this mode is entered to allow the 
// blower to spin down.  Once the blower has stopped, it is never restarted 
// unless there is a system initialization.
enum E_BlowerSpinDownState
{
    eSpinDownNever,         // the blower has not been stopped - an air source 
                            // fault has not been detected
    eSpinDownActive,        // an air source fault was detected and the 
                            // ventilator is in the NoGasMode allowing the       
                            // blower to come to a stop.
    eSpinDownComplete       // Blower has stopped     
};

class NoGasMode : public NonVenMode
{
    public:
        //Set blower spin state
        inline void SetBlowerSpinDownState (E_BlowerSpinDownState state)
        {
            BlowerSpinDownState = state;
        }

        //Pointer to the object instantiated from this class
        static NoGasMode* S_GetInstance (void);

        //Process do something when another mode enters and exits NoGasMode
        virtual void Enter (E_VentilationMode fromModeId);
        virtual void Exit (void);

    protected:

    private:
        //Constructor of class
        NoGasMode (void);

        // The copy constructor is not used.
        NoGasMode (const NoGasMode&);

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //Process when total gas loss
        virtual void ProcessTotalGasLossTrigger (void);

        //Process when reset gas loss
        virtual void ProcessResetGasLossTrigger (void);

        //Process blower spins down
        virtual void ProcessBlowerSpinDownTimeTrigger (void);

        //Update alarm status
        virtual void UpdateAlarmStatus (void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static NoGasMode* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: BlowerSpinDownState
        //  Description: Indicates the state of the blower with regard to 
        //      spin down after detection of an air source fault.
        //  Units: n/a
        //  Range: eSpinDownNever, eSpinDownActive, eSpinDownComplete   
        E_BlowerSpinDownState BlowerSpinDownState;

};

#endif // NOGASMODE_H
