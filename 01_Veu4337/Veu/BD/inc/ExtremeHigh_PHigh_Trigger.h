/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeHigh_PHigh_Trigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains the class declaration for the ExtremeHigh_PHigh_Trigger
/******************************************************************************/
#ifndef EXTREMEHIGH_PHIGH_TRIGGER_H
#define EXTREMEHIGH_PHIGH_TRIGGER_H

#include "Trigger.h"

class ExtremeHigh_PHigh_Trigger : public Trigger
{
public:
    //pointer to the instance of this class
    static ExtremeHigh_PHigh_Trigger* S_GetInstance(void);

    //Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Enable trigger
    virtual void EnableTrigger(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);

    //Get high pressure alarm
    inline LONG GetHighPressAlarm(void){return HighPressAlarm;}

    //set inactive timer value
    void SetInactiveTimer(USHORT value);

protected:

    //$COMMON.ATTRIBUTE$
    //    Name: HighPressAlarm
    //    Description:
    //    Units:  LONG
    //    Range:
    LONG HighPressAlarm;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;

private:
    //Constructor
    ExtremeHigh_PHigh_Trigger(void);
    //Copy constructor
    ExtremeHigh_PHigh_Trigger(const ExtremeHigh_PHigh_Trigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static ExtremeHigh_PHigh_Trigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: ExtremeHighPEEP_Offset
    //  Description:  offset of extremely High PEEP
    //  Units: None
    //  Range: n/a
    LONG ExtremeHighPEEP_Offset;

};
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
inline void ExtremeHigh_PHigh_Trigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    //    mode->ProcessExtremePressAlarmTrigger();
    mode->ProcessTriggerID(eExtremeHigh_PHigh_Trigger);
}   // end ProcessTrigger()



#endif // EXTREMEHIGH_PHIGH_TRIGGER_H
