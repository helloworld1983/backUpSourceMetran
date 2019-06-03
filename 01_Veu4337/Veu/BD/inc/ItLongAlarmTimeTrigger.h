/******************************************************************************/
//$COMMON.H$
//   File Name:  ItLongAlarmTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for the Inhalation Too Long
//      Alarm Time Trigger.  The ItLongAlarmTimeTrigger is derived from 
//      the TimeTrigger class.
/******************************************************************************/
#ifndef ITLONGALARMTIMETRIGGER_H
#define ITLONGALARMTIMETRIGGER_H

#include "TimeTrigger.h"

class ItLongAlarmTimeTrigger : public TimeTrigger
{
public:
    //Constructor
    ItLongAlarmTimeTrigger (void);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Disable trigger
    virtual void DisableTrigger (void);

    //check condition and process trigger
    virtual E_Bool CheckCondition (void);

    //created to override the PhaseInSettingChange()
    virtual void PhaseInSettingChange (LONG value);

protected:

private:
    //Copy constructor
    ItLongAlarmTimeTrigger (const ItLongAlarmTimeTrigger&);
};

#endif  // ITLONGALARMTIMETRIGGER_H
