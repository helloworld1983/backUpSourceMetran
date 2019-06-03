/******************************************************************************/
//$COMMON.H$
//   File Name:  ApneaAlarmTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the Apnea Alarm Time Trigger
//      class.  The Apnea Alarm Time Trigger is derived from the TimeTrigger
//      class.  It is used to keep track of the apnea interval timer and 
//      maintain the status of the Apnea alarm.
/******************************************************************************/
#ifndef APNEAALARMTIMETRIGGER_H
#define APNEAALARMTIMETRIGGER_H
#include "TimeTrigger.h"

class ApneaAlarmTimeTrigger : public TimeTrigger
{
public:
    //pointer to the instance of this class
    static ApneaAlarmTimeTrigger* S_GetInstance (void);

    //checks if the TriggerDidFire attribute has been set to TRUE and process action
    virtual E_Bool CheckCondition (void);

    //Reenable trigger
    virtual void ReEnableTrigger (void);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Apnea Interval setting changes
    virtual void PhaseInSettingChange (LONG value);

    //Constructor
    ApneaAlarmTimeTrigger (void);
    //Copy constructor
    ApneaAlarmTimeTrigger (const ApneaAlarmTimeTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ApneaAlarmTimeTrigger* S_Instance;
};

#endif  // APNEAALARMTIMETRIGGER_H
