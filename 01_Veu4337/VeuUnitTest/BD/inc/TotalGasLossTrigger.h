/******************************************************************************/
//$COMMON.H$
//   File Name:  TotalGasLossTrigger.h
//
//   Class: 
//      This file contains the declaration for the Disconected Alarm Trigger
//      class.The Disconected Alarm Trigger is derived from Trigger class 
//
//   Revision History:
/******************************************************************************/
#ifndef TOTALGASLOSSTRIGGER_H
#define TOTALGASLOSSTRIGGER_H

#include "Trigger.h"

class TotalGasLossTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static TotalGasLossTrigger* S_GetInstance(void);

    //check condition and process action
    virtual E_Bool CheckCondition(void);

    //Process action base on trigger id
    virtual inline void ProcessTrigger(Mode* mode);

    //Constructor
    TotalGasLossTrigger(void);
    //Copy constructor
    TotalGasLossTrigger(const TotalGasLossTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static TotalGasLossTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: gasLossDuration
    //  Description:  Gas loss duration
    //  Units: None
    //  Range: n/a
    USHORT  gasLossDuration;
};      

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessDisconectedAlarmTrigger() operation of 
//      the HFO mode.
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
inline void TotalGasLossTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the Disconected trigger in the current mode.
    mode->ProcessTriggerID(eTotalGasLossTrigger);

}   // end ProcessTrigger()

#endif // TOTALGASLOSSTRIGGER_H
