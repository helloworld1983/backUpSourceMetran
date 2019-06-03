/******************************************************************************/
//$COMMON.H$
//   File Name:  CyclicTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: This file contains the class declaration for the CyclicTimeTrigger 
//      class.  The CyclicTimeTrigger class is derived from the TimeTrigger
//      class.
/******************************************************************************/
#ifndef CYCLICTIMETRIGGER_H
#define CYCLICTIMETRIGGER_H

#include "TimeTrigger.h"

class CyclicTimeTrigger : public TimeTrigger
{
public:
    //Constructor
    CyclicTimeTrigger (E_TriggerId id, CHAR* timerName,
                       UNSIGNED timePeriod = TICKS_PER_SEC);

    //Enable trigger
    virtual void EnableTrigger (void);
    virtual void EnableTrigger (UNSIGNED time);

    //returns the value of the TriggerDidFire attribute
    //resets the TriggerDidFire attribute to FALSE
    virtual E_Bool CheckCondition (void);

    //created to override the PhaseInSettingChange()
    virtual void PhaseInSettingChange (LONG value);

protected:
    //Set time period
    virtual void SetTimePeriod (UNSIGNED time);

private:
    //Constructor
    CyclicTimeTrigger (void);

    //Copy constructor
    CyclicTimeTrigger (const CyclicTimeTrigger&);

};

#endif  // TIMETRIGGER_H
