/******************************************************************************/
//$COMMON.H$
//   File Name:  NIVBpTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the NIVBpTimeTrigger class
/******************************************************************************/
#ifndef NIVBPTIMETRIGGER_H
#define NIVBPTIMETRIGGER_H

#include "CyclicTimeTrigger.h"

class NIVBpTimeTrigger : public CyclicTimeTrigger
{
public:
    //Constructor
    NIVBpTimeTrigger(E_TriggerId id, CHAR* timerName);

    //Enable trigger
    virtual void EnableTrigger(void);
    virtual void EnableTrigger(UNSIGNED time);

    //Re-Enable trigger
    virtual void ReEnableTrigger(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //invoked whenever the RR is changed
    virtual void PhaseInSettingChange(LONG newValue);

    //check previous condition
    virtual E_Bool PreCheckCondition(void);

protected:


private:
    //Copy Constructor
    NIVBpTimeTrigger(const NIVBpTimeTrigger&);
    //Constructor
    NIVBpTimeTrigger(void);

    //invoked to phase in a new respiratory rate if NIVBpTimeTrigger is enabled
    void PhaseIn(void);

    //$COMMON.ATTRIBUTE$
    //  Name: IsChangePending
    //  Description:  Indicates if the operator has requested a rate change
    //      that has not yet been phased in.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsChangePending;

    //$COMMON.ATTRIBUTE$
    //  Name: breathRateValue
    //  Description:  Breathe rate
    //  Units: None
    //  Range: eTrue, eFalse
    LONG breathRateValue;
};

#endif // NIVBPTIMETRIGGER_H
