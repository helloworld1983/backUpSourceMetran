/******************************************************************************/
//$COMMON.CPP$
//   File Name:  TotalGasLossTrigger.cpp
//   Purpose: 
//      This file contains the definition of the Disconected Alarm Trigger.
//      The Disconected Alarm Trigger is derived from the Trigger.It is used
//		to detect the ET Disconected condition during NormBreathing mode.
//
//   Interfaces:
//      
//   Restrictions:
//      None
//
//   Revision History:
//
//
/******************************************************************************/
#include "TotalGasLossTrigger.h"
#include "O2Setting.h"
#include "PhaseMgr.h"
#include "DebugTrace.h"

TotalGasLossTrigger* TotalGasLossTrigger::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: TotalGasLossTrigger()
//
//    Processing: 
//      This operation is the constructor for the TotalGasLossTrigger.
//      This operation invokes the Trigger base class constructor.
//
//    Input Parameters:
//      triggerId - Trigger ID 
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
TotalGasLossTrigger::TotalGasLossTrigger(void)
    : Trigger(eTotalGasLossTrigger), gasLossDuration(0)  
{
	
}   // end TotalGasLossTrigger()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing: 
//      This operation is invoked every BD Cycle Time while the trigger is 
//      enabled.  If the trigger has not already fired ,check air pressure switch
//		and O2 pressure switch in turn.
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
E_Bool TotalGasLossTrigger::CheckCondition(void)
{	
	if ( (eTrue == O2Setting::S_GetInstance()->O2SourceFault()) &&
		 (eTrue == O2Setting::S_GetInstance()->AirSourceFault()) )
	{
		TriggerDidFire = eTrue;	
	}
	return (TriggerDidFire);
}   // end CheckCondition()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the TotalGasLossTrigger object, it invokes
//      TotalGasLossTrigger::S_GetInstance(), which returns a pointer
//      to the TotalGasLossTrigger object.
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
//      TotalGasLossTrigger* - pointer to the instance of this class
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
TotalGasLossTrigger* TotalGasLossTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new TotalGasLossTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end TotalGasLossTrigger()
