/******************************************************************************/
//$COMMON.H$
//   File Name:  SI10sTimerTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the SI10sTimerTrigger class
/******************************************************************************/
#ifndef SI10STIMERTRIGGER_H
#define SI10STIMERTRIGGER_H

#include "TimeTrigger.h"

class SI10sTimerTrigger : public TimeTrigger
{
public:
    //Constructor
    SI10sTimerTrigger(void);

    //enable trigger
    virtual void EnableTrigger(void);

    //Disable trigger
    virtual void DisableTrigger(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //process trigger base on trigger id
    virtual void ProcessTrigger(Mode *mode);

private:
    //Copy Constructor
    SI10sTimerTrigger(const SI10sTimerTrigger&);

    //Upadte si Time
    void UpdateSITime();
    //    PatientDataGUIEvent SIMsg;


    //$COMMON.ATTRIBUTE$
    //  Name: currentSITime
    //  Description:  Current si time
    //  Units: None
    //  Range: n/a
    LONG currentSITime;


    //$COMMON.ATTRIBUTE$
    //  Name: one_hundred_ms
    //  Description:  100ms.
    //  Units: None
    //  Range: n/a
    USHORT one_hundred_ms;
};

#endif // SI10STIMERTRIGGER_H
