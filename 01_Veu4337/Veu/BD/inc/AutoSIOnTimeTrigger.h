/******************************************************************************/
//$COMMON.H$
//   File Name:  AutoSIOnTimeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the AutoSIOnTimeTrigger class
/******************************************************************************/
#ifndef AutoSIOnTimeTrigger_H
#define AutoSIOnTimeTrigger_H

#include "TimeTrigger.h"

class AutoSIOnTimeTrigger : public TimeTrigger
{
public:
    //pointer to the object instantiated from this class
    static AutoSIOnTimeTrigger* S_GetInstance(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Re-enable trigger
    virtual void ReEnableTrigger(void);
    //Enable trigger
    virtual void EnableTrigger(void);

    //invoked when a setting tied to the Time Trigger change
    virtual void PhaseInSettingChange(LONG value);

protected:

private:
    //Constructor
    AutoSIOnTimeTrigger(void);
    //Copy constructor
    AutoSIOnTimeTrigger(const AutoSIOnTimeTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static AutoSIOnTimeTrigger* S_Instance;

    //Update SI timer
    void UpdateSITime();

    //$COMMON.ATTRIBUTE$
    //  Name: currentTime
    //  Description:  Current time value
    //  Units: None
    //  Range: n/a
    LONG currentTime;

    //$COMMON.ATTRIBUTE$
    //  Name: currentSIRate
    //  Description:  Current SI rate
    //  Units: None
    //  Range: n/a
    LONG currentSIRate;

    //$COMMON.ATTRIBUTE$
    //  Name: currentSITimeData
    //  Description:  Current SI time data
    //  Units: None
    //  Range: n/a
    LONG currentSITimeData;

    //$COMMON.ATTRIBUTE$
    //  Name: one_hundred_ms
    //  Description:  100ms
    //  Units: None
    //  Range: n/a
    USHORT one_hundred_ms;
};

#endif // AutoSIOnTimeTrigger_H
