/******************************************************************************/
//$COMMON.H$
//   File Name:  BpBackupTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the BpBackupTimeTrigger class
/******************************************************************************/
#ifndef BPBACKUPTIMETRIGGER_H
#define BPBACKUPTIMETRIGGER_H

#include "CyclicTimeTrigger.h"

class BpBackupTimeTrigger : public CyclicTimeTrigger
{
public:
    //constructor
    BpBackupTimeTrigger(E_TriggerId id, CHAR* timerName);

    //enable trigger
    virtual void EnableTrigger(void);
    virtual void EnableTrigger(UNSIGNED time);

    //Re-enable trigger
    virtual void ReEnableTrigger(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //calculates a new breath period based upon the respiratory rate and
    //stores the value in the TimePeriod attribute
    virtual void PhaseInSettingChange(LONG newValue);

    //check previous condition
    virtual E_Bool PreCheckCondition(void);

protected:

private:
    //Copy constructor
    BpBackupTimeTrigger(const BpBackupTimeTrigger&);
    //constructor
    BpBackupTimeTrigger(void);

    void PhaseIn(void);

    //$COMMON.ATTRIBUTE$
    //  Name: IsChangePending
    //  Description:  Indicates if the operator has requested a rate change
    //      that has not yet been phased in.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsChangePending;
};

#endif // BPBACKUPTIMETRIGGER_H
