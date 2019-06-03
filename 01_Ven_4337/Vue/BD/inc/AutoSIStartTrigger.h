/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIStartTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIStartTrigger class
/******************************************************************************/
#ifndef AUTOSISTARTTRIGGER_H
#define AUTOSISTARTTRIGGER_H

#include "TimeTrigger.h"

class AutoSIStartTrigger : public TimeTrigger
{
public:
    //pointer to the object instantiated from this class
    static AutoSIStartTrigger* S_GetInstance(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Re-enable trigger
    virtual void ReEnableTrigger(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //Process phase when setting change
    virtual void PhaseInSettingChange(LONG value);

protected:

private:
    //Constructor
    AutoSIStartTrigger(void);
    //Copy constructor
    AutoSIStartTrigger(const AutoSIStartTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AutoSIStartTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: currentTime
    //  Description:  Current time
    //  Units: None
    //  Range: n/a
    LONG currentTime;

    //$COMMON.ATTRIBUTE$
    //  Name: currentSIRate
    //  Description:  Current SI rate
    //  Units: None
    //  Range: n/a
    LONG currentSIRate;
};

#endif // AUTOSISTARTTRIGGER_H
