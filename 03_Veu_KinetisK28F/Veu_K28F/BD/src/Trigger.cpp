/******************************************************************************/
//$COMMON.CPP$
//  File Name:   Trigger.cpp
//  Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//  Purpose: 
//      This file contains the definition of the base Trigger class. 
//      Operations in this class are invoked directly from derived class
//      operations or are invoked when the derived class does not define
//      the virtual operation within its own class.
//
//      Triggers are used to cause Breath Delivery to transition between modes
//      or breath phases.  Each trigger defines a criteria that once met
//      causes a transition.
//
//  Interfaces:
//      Operations in this class only modify attributes within the class.
//
//  Restrictions:
//      None
/******************************************************************************/
#include <Trigger.h>

#include "DebugTrace.h"

/******************************************************************************/
//$COMMON.OPERATION$
//  Operation Name: Trigger()
//
//  Processing: 
//      This operation is the constructor for the base trigger class.  It
//      initializes the data attributes of the class.
//
//  Input Parameters:
//      E_TriggerId id -- trigger ID for the trigger
//
//  Output Parameters:
//      None
//
//  Return Values:
//      None
//
//  Pre-Conditions:
//      None
//
//  Miscellaneous:
//      None
//
//  Requirements:    
//
/******************************************************************************/
Trigger::Trigger (E_TriggerId id) :
        IsEnabled (eFalse), TriggerDidFire (eFalse), TriggerId (id)
{

}   // end Trigger()

/******************************************************************************/
//$COMMON.OPERATION$
//  Operation Name: CheckCondition()
//
//  Processing: 
//      This operation is the base class CheckCondition() method that 
//      simply returns the TriggerDidFire attribute.  Most derived triggers 
//      define their own CheckCondition() method.
//
//  Input Parameters:
//      None
//
//  Output Parameters:
//      None
//
//  Return Values:
//      eTrue if the trigger did fire; eFalse otherwise
//
//  Pre-Conditions:
//      None
//
//  Miscellaneous:
//      None
//
//  Requirements:    
//
/******************************************************************************/
E_Bool Trigger::CheckCondition (void)
{
    return (TriggerDidFire);

}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//  Operation Name: EnableTrigger()
//
//  Processing: 
//      This operation is the base class EnableTrigger() method.  It is invoked 
//      from all derived classes' EnableTrigger() method to perform the common
//      functionality of all triggers.  This method sets the IsEnabled 
//      attribute to eTrue and the TriggerDidFire attribute to eFalse. 
//
//      If any additional functionality common to enabling all triggers is
//      added, it should be added here.
//
//  Input Parameters:
//      None
//
//  Output Parameters:
//      None
//
//  Return Values:
//      None
//
//  Pre-Conditions:
//      None
//
//  Miscellaneous:
//      None
//
//  Requirements:    
//
/******************************************************************************/
void Trigger::EnableTrigger (void)
{

    IsEnabled = eTrue;
    TriggerDidFire = eFalse;

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//  Operation Name: DisableTrigger()
//
//  Processing: 
//      This operation is the base class DisableTrigger() method.  It is invoked 
//      from all derived classes' DisableTrigger() method to perform the common
//      functionality of all triggers.  This method sets the IsEnabled 
//      attribute to eFalse.
//
//      If any additional functionality common to disabling all triggers is
//      added, it should be added here.
//
//  Input Parameters:
//      None
//
//  Output Parameters:
//      None
//
//  Return Values:
//      None
//
//  Pre-Conditions:
//      None
//
//  Miscellaneous:
//      None
//
//  Requirements:    
//
/******************************************************************************/
void Trigger::DisableTrigger (void)
{

    IsEnabled = eFalse;

}   // end DisableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This is an overloaded version of the EnableTrigger() operation.  This
//      operation takes an input value which is used to enable the trigger.
//      This operation simply performs error handling since every derived
//      Trigger class that uses this overloaded operation is expected to
//      define the corresponding method.
//
//    Input Parameters:
//      UNSIGNED - not used 
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void Trigger::EnableTrigger (UNSIGNED)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReEnableTrigger()
//
//    Processing: 
//      This operation simply performs error handling since every derived
//      Trigger class that uses the ReEnable() operation is expected to
//      define the corresponding method.
//
//      This operation was created to allow the TriggerMgr to access it 
//      via a pointer to the base Trigger class.
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
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void Trigger::ReEnableTrigger (void)
{
    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end ReEnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UpdateState()
//
//    Processing: 
//      This operation simply performs error handling since every derived
//      Trigger class that uses the UpdateState() operation is expected to
//      define the corresponding method.
//
//      This operation was created to allow the TriggerMgr to access the 
//      derived class operation via a pointer to the base Trigger class.
//
//    Input Parameters:
//      E_TriggerId - not used 
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void Trigger::UpdateState (E_TriggerId)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end UpdateState()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PreCheckCondition()
//
//    Processing: 
//      This operation simply performs error handling since every derived
//      Trigger class that uses the PreCheckCondition() operation is expected to
//      define the corresponding method.
//
//      This operation was created to allow the TriggerMgr to access the 
//      derived class operation via a pointer to the base Trigger class.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      eFalse
//
//    Pre-Conditions:
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool Trigger::PreCheckCondition (void)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

    return (eFalse);

}   // end PreCheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: PhaseInSettingChange()
//
//    Processing: 
//      This operation simply performs error handling since every derived
//      Trigger class that uses the PhaseInSettingChange() operation is expected 
//      to define the corresponding method.
//
//      This operation was created to allow the TriggerMgr to access the 
//      derived class operation via a pointer to the base Trigger class.
//
//    Input Parameters:
//      SHORT - not used
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void Trigger::PhaseInSettingChange (LONG)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);

}   // end PhaseInSettingChange()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetTimePeriod()
//
//    Processing: 
//      This operation simply performs error handling since every derived
//      Trigger class that uses the GetTimePeriod() operation is expected 
//      to define the corresponding method.  Only TimeTriggers have associated
//      TimePeriods.
//
//      This operation was created to allow the TriggerMgr to access the 
//      derived class operation via a pointer to the base Trigger class.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      0
//
//    Pre-Conditions:
//      This operation should never be called since any derived triggers 
//      that use this method should define their own.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
UNSIGNED Trigger::GetTimePeriod (void)
{

    // Derived classes should create their own operation
    ASSERTION(eFalse);
    return (0);

}   // end GetTimePeriod()
