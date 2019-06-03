/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIStopTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIStopTrigger class
/******************************************************************************/
#ifndef AUTOSISTOPTRIGGER_H
#define AUTOSISTOPTRIGGER_H

#include "TimeTrigger.h"
class AutoSIStopTrigger : public TimeTrigger
{
public:
    //pointer to the object instantiated from this class
    static AutoSIStopTrigger* S_GetInstance(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //process action base on changing phase
    virtual void PhaseInSettingChange(LONG value);

protected:

private:
    //Constructor
    AutoSIStopTrigger(void);

    //Copy constructor
    AutoSIStopTrigger(const AutoSIStopTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AutoSIStopTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: currentTime
    //  Description:  current time
    //  Units: None
    //  Range: n/a
    LONG currentTime;
};

#endif // AUTOSISTOPTRIGGER_H
