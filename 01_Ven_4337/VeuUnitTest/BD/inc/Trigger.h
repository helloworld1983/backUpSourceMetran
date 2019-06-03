/******************************************************************************/
//$COMMON.H$
//   File Name:  Trigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Base Trigger Class; all Triggers are derived from this class.
/******************************************************************************/
#ifndef TRIGGER_H
#define TRIGGER_H

#include <Mode.h>
#include "CommonConstants.h"
#include "TriggerIds.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class Trigger
{
    public:
        Trigger (E_TriggerId id);
        Trigger(void){};
        virtual void PhaseInSettingChange (LONG newValue);
        virtual E_Bool CheckCondition (void);
        virtual void EnableTrigger (UNSIGNED value);
        virtual void EnableTrigger (void);
        virtual void DisableTrigger (void);
        virtual void ReEnableTrigger (void);
        virtual void UpdateState (E_TriggerId firedTrigger);
        virtual E_Bool PreCheckCondition (void);

        virtual UNSIGNED GetTimePeriod (void);

        virtual inline void ProcessTrigger (Mode* mode);

        inline void SetTriggerDidFire (E_Bool didFire)
        {
            TriggerDidFire = didFire;
        }

        inline E_Bool GetIsEnabled (void)
        {
            return IsEnabled;
        }

        //$COMMON.ATTRIBUTE$
        //    Name: IsEnabled
        //    Description: This attribute indicates whether the trigger is 
        //          enabled.
        //
        //    Units:  none
        //    Range:  True, False
        E_Bool IsEnabled;

        //$COMMON.ATTRIBUTE$
        //    Name: TriggerDidFire
        //    Description: This attribute indicates if the trigger has fired.
        //
        //    Units:  none
        //    Range:  True, False
        E_Bool TriggerDidFire;

        //$COMMON.ATTRIBUTE$
        //    Name: TriggerId
        //    Description: This attribute contains a unique trigger identifier.
        //
        //    Units:  none
        //    Range:  n/a
        E_TriggerId TriggerId;

//        Trigger(void);              // default constructor not used
        Trigger (const Trigger&);    // copy constructor not used

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessBasicTrigger() operation of the active
//      mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
inline void Trigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to process the trigger in the currently 
    // active mode.
    mode->ProcessTriggerID(TriggerId);

}   // end ProcessTrigger()

#endif  // TRIGGER_H
