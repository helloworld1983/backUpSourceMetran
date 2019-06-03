/******************************************************************************/
//$COMMON.H$
//   File Name:  ModeChangeTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Mode Change Trigger class is used to trigger a mode change when
//      the operator changes the mode setting.   This class is derived from
//      the base class Trigger.
/******************************************************************************/
#ifndef MODECHANGETRIGGER_H
#define MODECHANGETRIGGER_H

#include <Trigger.h>

class ModeChangeTrigger : public Trigger
{
public:
    //Copy constructor
    ModeChangeTrigger (void);

    //enable trigger
    virtual void EnableTrigger (void);

    //Check condition and process action
    virtual E_Bool CheckCondition (void);

    //called whenever the Mode setting value is changed
    virtual void PhaseInSettingChange (LONG newValue);

    //process action base on trigger ID
    virtual inline void ProcessTrigger (Mode* mode);

protected:

private:
    //$COMMON.ATTRIBUTE$
    //  Name: CurrentMode
    //  Description:  Currently active mode of ventilation.
    //  Units: None
    //  Range: E_VentilationMode
    E_VentilationMode CurrentMode;

    //$COMMON.ATTRIBUTE$
    //  Name: IsArmed
    //  Description:
    //      Indicates if the Alarm Reset Request has been received but
    //      not honored yet.
    //  Units: None
    //  Range: eTrue, eFalse
    E_Bool IsArmed;

    ModeChangeTrigger (const ModeChangeTrigger&);
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessModeChangeTrigger() operation of the 
//      active mode.
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
inline void ModeChangeTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the method to handle the Mode Change in the currently
    // active mode.
    mode->ProcessTriggerID(eModeChangeTrigger);

}   // end ProcessTrigger()

#endif  // MODECHANGETRIGGER_H
