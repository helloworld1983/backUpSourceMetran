/******************************************************************************/
//$COMMON.H$
//   File Name:  Mode.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the base Mode class.
//      All modes are derived from this base class.
/******************************************************************************/
#ifndef MODE_H
#define MODE_H

#include "CommonTypes.h"
#include "CommonConstants.h"
#include "TriggerIds.h"

class Mode
{
    public:
        //enter from mode E_VentilationMode fromModeId
        virtual void Enter (E_VentilationMode fromModeId) = 0;

        //Exit mode
        virtual void Exit (void) = 0;

        //Update alarm status
        virtual void UpdateAlarmStatus (void) = 0;

        //Process action for each ID trigger
        virtual void ProcessTriggerID(E_TriggerId id);

        //If the inhalation was triggered by the patient, set the return value to TRUE
        static E_Bool S_IsInhPtTriggered (void);

        //set Last trigger's ID
        static void S_SetLastTriggerId (E_TriggerId id);

        //Create safe state
        virtual void CreateSafeState(void);

        //Get manual reset status
        inline E_Bool GetDoesManualReset (void)
        {
            return DoesManualReset;
        }

    protected:
        Mode (E_Bool doesManualReset = eFalse);

        //$COMMON.ATTRIBUTE$
        //  Name: LastTriggerId
        //  Description:  Identifier of the last trigger that fired.
        //  Units: None
        //  Range: n/a
        static E_TriggerId S_LastTriggerId;

    private:
        // The copy and default constructors are not used
        Mode (const Mode&);

        //$COMMON.ATTRIBUTE$
        //  Name: GetDoesManualReset
        //  Description:  
        //      Indicates if the manual reset button causes a transition
        //      out of this mode.
        //  Units: None
        //  Range: n/a
        E_Bool DoesManualReset;
};

#endif // MODE_H
