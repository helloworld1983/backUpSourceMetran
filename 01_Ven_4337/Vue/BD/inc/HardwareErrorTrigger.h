#ifndef HARDWAREERRORTRIGGER_H
#define HARDWAREERRORTRIGGER_H

#include "Trigger.h"

class HardwareErrorTrigger  : public Trigger
{
public:
    static HardwareErrorTrigger* S_GetInstance(void);
    virtual E_Bool CheckCondition(void);
    virtual inline void ProcessTrigger(Mode* mode);

private:
    HardwareErrorTrigger(void);
    HardwareErrorTrigger(const HardwareErrorTrigger&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HardwareErrorTrigger* S_Instance;

    int two_second;

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
inline void HardwareErrorTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    mode->ProcessTriggerID(eHardwareErrorAlarmTriger);
}   // end ProcessTrigger()



#endif // HARDWAREERRORTRIGGER_H
