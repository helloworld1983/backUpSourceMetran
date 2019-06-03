/******************************************************************************/
//$COMMON.CPP$
//   File Name:  BdSetting.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  
//      This file contains the implementation of the BD Setting class. Each
//      setting is implemented as its own BD Setting object.  Breath Delivery
//      does not maintain every setting in the system; it only keeps settings
//      it needs to know how to deliver breaths in the current ventilation
//      type.  For example, the GUI maintains several RR settings - one for
//      each ventilation type.  The BD only has 1 RR setting, which corresponds
//      to the active ventilation type.
//
//   Interfaces:
//      The SetPendingValue() operation of this class invokes
//      MgrBdSetting::AddSettingToChangeList() to notify the BD Setting Manager
//      that the setting value has changed and needs to be phased in on the
//      next breath phase transition.  SetPendingValue() also interfaces
//      with a trigger object if the TriggerPtr is not NULL. 
//      Trigger::PhaseInSettingChange() is called to notify a trigger of 
//      the new value in cases where a setting change may be phased in sooner
//      than the next breath phase.
//
//   Restrictions:
//      None
/******************************************************************************/
//#include "MgrBdSetting.h"
#include "BdSetting.h"
#include "DebugTrace.h"

// Value used to determine whether or not a setting is corrupted.
static const LONG ALL_FF = ~0;

// constant used to initialize all settings in NVRAM
const LONG UNINITIALIZED_LONG_VALUE = 0x80000001;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BdSetting()
//
//    Processing: 
//      This operation is the constructor for a BD Setting object.  It 
//      initializes the TriggerPtr and PhaseInImmediately attributes to the 
//      passed values and sets the CurrentValue to 0.
//
//    Input Parameters:
//      triggerPtr - pointer to the Trigger object that should be notified
//          when the setting value changes.
//      PhaseInImmediately - boolean that indicates if the Trigger object is 
//          notified immediately or on the breath phase boundary.
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
BdSetting::BdSetting (E_BdSettingId anID, Trigger* triggerPtr, E_Bool phaseInImmediately)
{
    TriggerPtr = triggerPtr;
    PhaseInImmediately = phaseInImmediately;

    // By setting both of these values to 0, the error handler will be invoked
    // if a task tries to access a setting value before that setting has
    // been initialized.
    CurrentValue.value = 0;
    CurrentValue.backup = 0;

    Id = anID;

}   // end BdSetting()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetCurrentValue()
//
//    Processing: 
//      This operation checks the CurrentValue against the backup value.
//      If it is corrupted, error handling is performed.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - the current value 
//
//    Pre-Conditions:
//      The value and backup value must be consistent.
//
//    Miscellaneous:
//      If the value and backup value do not agree, perform error handling.
//
//    Requirements:  
//
/******************************************************************************/
LONG BdSetting::GetCurrentValue (void)
{
    // Make sure the value and backup value are in agreement.
//    ASSERTION((CurrentValue.value ^ CurrentValue.backup) == ALL_FF);

    return (CurrentValue.value);

}   // end GetCurrentValue()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetPendingValue()
//
//    Processing: 
//      This operation checks the PendingValue against the backup value.
//      If it is corrupted, error handling is performed.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG - the current value 
//
//    Pre-Conditions:
//      The value and backup value must be consistent.
//
//    Miscellaneous:
//      If the value and backup value do not agree, perform error handling.
//
//    Requirements:  
//
/******************************************************************************/
LONG BdSetting::GetPendingValue (void)
{
    // Make sure the value and backup value are in agreement.
    ASSERTION((PendingValue.value ^ PendingValue.backup) == ALL_FF);

    return (PendingValue.value);

}   // end GetPendingValue()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSetting()
//
//    Processing: 
//      This operation is invoked by MgrBdSetting::ProcessNewPhase() to phase
//      in a pending setting change.  This operation copies the pending
//      value into the CurrentValue attribute.  If the trigger associated with
//      this setting was not notified of the value change when the change
//      occurred, it is notifed here.
//
//    Input Parameters:
//      None
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
void BdSetting::PhaseInSetting (void)
{
    // Update the Current Value with the Pending Value
    CurrentValue.value = PendingValue.value;
    CurrentValue.backup = PendingValue.backup;

    // If the associated trigger was not notified when the change was made, 
    // notify it now
    if((NULL != TriggerPtr) && (eFalse == PhaseInImmediately))
    {
        // Notify the associated trigger that the value has changed.
        TriggerPtr->PhaseInSettingChange (CurrentValue.value);
    }

}   // end PhaseInSetting()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetPendingValue()
//
//    Processing: 
//      This operations is invoked whenever a settings change request affecting
//      this setting is received.  A setting can be modified via a single 
//      setting change event or it can be modified as part of a Ventilation
//      Type batch settings change.
//
//      This operation updates the PendingValue, invokes MgrBdSetting::
//      AddSettingToChangeList() to add the setting to the list of changed
//      values that must be phased in on the next breath phase transition.
//      If the TriggerPtr is not NULL, this operation invokes the associated
//      trigger operation PhaseInSettingChange() to notify the trigger of 
//      the setting change.
//
//    Input Parameters:
//      value - new setting value
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
void BdSetting::SetPendingValue (LONG value)
{
    // Set the Pending Value to the new value
    PendingValue.value = value;

    // Set the backup pending value to the one's complement of the new value
    PendingValue.backup = ~value;

}   // end SetPendingValue()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: BdSetting()
//
//    Processing:
//      This operation is the constructor for a BD Setting object which is
//      initialized from NVRAM at start-up.
//
//    Input Parameters:
//      nvramIx - NVRAM index
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      NVRAM contains valid values.
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
BdSetting::BdSetting(E_ConfigIndex configIx)
{
    // temporary storage
    LONG value = 0;

    // No associated trigger.
    TriggerPtr = NULL;
    PhaseInImmediately = eFalse;

    // Retrieve value from NVRAM
//    value = systemConfigFile->getValue(configIx);

    // If NVRAM is corrupted, use a default
    if (UNINITIALIZED_LONG_VALUE == value)
    {
        switch (configIx)
        {
            case eCircuitCompliance:
                // Default the value to 0
                value = 0;

                // Send message to GUI so that a dialog is displayed, indicating
                // that a default value is in use.  Assert if not successful.
                break;

            case eCircuitComplianceOnOff:
                value = (LONG) eFalse;

                // Send message to GUI so that a dialog is displayed, indicating
                // that a default value is in use.  Assert if not successful.
                break;

            default:
                ASSERTION(eFalse);
                break;
        }
    }

    // Update the current and pending values with the value.  PendingValue must
    // be updated in case ALL BD settings get phased in due to a change list
    // overflow in MgrBdSetting::ProcessNewPhase().
    CurrentValue.value = value;
    CurrentValue.backup = ~value;
    PendingValue.value = value;
    PendingValue.backup = ~value;

}   // end BdSetting()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetID()
//
//    Processing: Get BdSetting ID
//
//    Input Parameters:
//      NOne
//
//    Output Parameters:
//      None
//
//    Return Values:
//      Id -
//
//    Pre-Conditions:
//      NOne
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
E_BdSettingId BdSetting::GetID (void)
{
    return Id;
}
