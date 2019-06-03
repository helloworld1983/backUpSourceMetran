/******************************************************************************/
//$COMMON.H$
//   File Name:  BpTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: The Breath Period Time Trigger is a time trigger based upon the
//      operator set respiratory rate.  When the Breath Period expires, 
//      a mandatory inhalation is delivered in the mandatory modes.   This
//      trigger is also used in Mixed Mode to insure mandatory breath delivery.
/******************************************************************************/
#ifndef BPTIMETRIGGER_H
#define BPTIMETRIGGER_H

#include "CyclicTimeTrigger.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class BpTimeTrigger : public CyclicTimeTrigger
{
    public:
        BpTimeTrigger (E_TriggerId id, CHAR* timerName);

        virtual void EnableTrigger (void);
        virtual void EnableTrigger (UNSIGNED time);
        virtual void ReEnableTrigger (void);
        virtual E_Bool CheckCondition (void);
        virtual void PhaseInSettingChange (LONG newValue);
        virtual E_Bool PreCheckCondition (void);

    protected:

    private:
        BpTimeTrigger (const BpTimeTrigger&);    // not used
        BpTimeTrigger (void);                    // not used

        void PhaseIn (void);

        //$COMMON.ATTRIBUTE$
        //  Name: IsChangePending
        //  Description:  Indicates if the operator has requested a rate change
        //      that has not yet been phased in.
        //  Units: None
        //  Range: eTrue, eFalse
        E_Bool IsChangePending;
};

#endif  // BPTIMETRIGGER_H
