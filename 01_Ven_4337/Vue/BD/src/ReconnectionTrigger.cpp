/******************************************************************************/
//$COMMON.CPP$
//   File Name:  ReconnectionTrigger.cpp
//   Purpose: 
//      This file contains the definition of the Reconected
//      Trigger, which is derived from the base Trigger class.  
//
//   Interfaces:
//      The CheckCondition() operation interfaces with 
//      CheckCondition() also calls SystemClock::Retrieve() to snapshot the
//      current system time.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "ReconnectionTrigger.h"
#include "MgrBreathData.h"
#include "O2Setting.h"

#include "DebugTrace.h"

ReconnectionTrigger* ReconnectionTrigger::S_Instance = NULL;
static const LONG PRESSURE_LIMIT  = 3 * SCALE; // 3 cmH2O

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the ReconnectionTrigger object, it invokes
//      ReconnectionTrigger::S_GetInstance(), which returns a pointer
//      to the ReconnectionTrigger object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      ReconnectionTrigger* - pointer to the instance of this class
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
ReconnectionTrigger* ReconnectionTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ReconnectionTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end ReconnectionTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReconnectionTrigger()
//
//    Processing: 
//      This operation is the constructor for the Patient Exhalation Pressure
//      Trigger.  It invokes the base class Trigger constructor with the 
//      Trigger ID.
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
ReconnectionTrigger::ReconnectionTrigger(void): Trigger(eReconnectionTrigger)
{
  
}   // end ReconnectionTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: EnableTrigger()
//
//    Processing: 
//      This method is invoked when the ET Reconected Trigger is enabled.
//      This operation sets exh valve step closed ,maintains flow and 
//      invokes the base class Trigger::EnableTrigger().
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
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void ReconnectionTrigger::EnableTrigger(void)
{
    // Invoke Base Class Enable Method
    Trigger::EnableTrigger();
}   // end EnableTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD task cycle while the ventilator is
//      in the Disconnection mode. 
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the trigger fired or not
//
//    Pre-Conditions:
//      This operation assumes that the TriggerMgr only invokes it while
//      the trigger is enabled.
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
E_Bool ReconnectionTrigger::CheckCondition(void)
{
	static USHORT filtertimes = 0;

	if(MgrBreathData::S_GetInstance()->GetMonitoredPressure()> PRESSURE_LIMIT)
	{
		filtertimes++;
		if(filtertimes > 250)
		{
			filtertimes = 0;
			TriggerDidFire = eTrue;
		}
	}
	else
	{
		filtertimes = 0;
	}
	
    return(TriggerDidFire);

}   // end CheckCondition()

