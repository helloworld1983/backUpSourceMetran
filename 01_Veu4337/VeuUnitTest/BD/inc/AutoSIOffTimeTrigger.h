/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIOffTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIOnTimeTrigger class
/******************************************************************************/
#ifndef AutoSIOffTimeTrigger_H
#define AutoSIOffTimeTrigger_H

#include "TimeTrigger.h"

class AutoSIOffTimeTrigger : public TimeTrigger
{
public:
    //pointer to the object instantiated from this class
    static AutoSIOffTimeTrigger* S_GetInstance(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //process when setting change
    virtual void PhaseInSettingChange(LONG value);

    //Constructor
    AutoSIOffTimeTrigger(void);
    //Copy constructor
    AutoSIOffTimeTrigger(const AutoSIOffTimeTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AutoSIOffTimeTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: currentTime
    //  Description:  Current time.
    //  Units: None
    //  Range: n/a
    LONG currentTime;
};

#endif // AutoSIOffTimeTrigger_H
