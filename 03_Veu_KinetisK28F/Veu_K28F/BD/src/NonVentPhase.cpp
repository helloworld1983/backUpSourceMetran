/******************************************************************************/
//$COMMON.CPP$
//   File Name:  NonVentPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      NonVentPhase is the breath phase entered when the ventilator goes into
//      a non-ventilation mode.  In this phase, the delivery valves are 
//      closed and the exhalation valve is open.
//
//   Interfaces:
//
//   Restrictions:
//      There is only one instance of this class.
/******************************************************************************/
//#include "nucpp.h"
#include <PhaseMgr.h>
#include "NonVentPhase.h"
#include "Exponential.h"
#include "MgrBdSetting.h"
#include "MgrBreathData.h"
#include "CommonTypes.h"
#include "DebugTrace.h"

UNSIGNED NonVentPhase::S_StartTime = 0;
NonVentPhase* NonVentPhase::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants to
//      access the NonVentPhase object, it calls NonVentPhase::S_GetInstance(),
//      which returns a pointer to the NonVentPhase object.
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
//      NonVentPhase* - pointer to the object instantiated from this class
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
NonVentPhase* NonVentPhase::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new NonVentPhase ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: NonVentPhase()
//
//    Processing: 
//      This operation is the constructor for the NonVentPhase.  It invokes the
//      base class Phase constructor.
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
NonVentPhase::NonVentPhase (void) :
        Phase ()
{

}   // end NonVentPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of every non-ventilation phase.
//
//      The ExhPressController
//      operation SetDesired() is invoked to set the desired pressure and
//      ExhPressController::Enable() is invoked to enable the exhalation
//      pressure controller, which only runs during the exhalation phase.
//
//      The Start() operation then calls SystemClock::Retrieve() to snapshot
//      the system time at the start of exhalation.  It also calls
//      MgrBreathData::ExhalationStart() to update breath data at the start
//      of the exhalation phase.
//
//      Finally, this operation calls TriggerMgr::DisableTrigger() to disable
//      the HIP Alarm triggers and EnableTrigger() to enable the Manual 
//      Inhalation Operatator Request trigger.
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
void NonVentPhase::Start (void)
{
    // Update Patient Data
    (MgrBreathData::S_GetInstance ())->NonVentStart ();

    // Update Patient Data
    (MgrBreathData::S_GetInstance())->NonVentStart();

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of the non-ventilation phase. 
//      It performs no work but is needed since it is defined as pure
//      virtual in the base class Phase.
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
void NonVentPhase::End (void)
{

}   // end End()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked at the end of the non-ventilation phase. 
//      It performs no work but is needed since it is defined as pure
//      virtual in the base class Phase.
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
void NonVentPhase::ProcessBdCycle (void)
{

}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_IsInUnrestrictedPhase()
//
//    Processing: 
//      This operation determines whether the ventilator is in the unrestricted
//      phase of exhalation.  The unrestricted phase of exhalation is defined
//      as the part of the exhalation phase after a minimum exhalation period
//      has elapsed.  The minimum exhalation time is defined by an input
//      argument.
//
//    Input Parameters:
//      minExhTime - minimum exhalation time comparing against
//
//    Output Parameters:
//      None
//
//    Return Values:
//      E_Bool - indicates if the ventilator is at least minExhTime into
//          the exhalation phase
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
E_Bool NonVentPhase::S_IsInUnrestrictedPhase (UNSIGNED minExhTime)
{
    E_Bool rtnValue = eFalse;

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.    
    if(((PhaseMgr::S_GetInstance ())->GetCurrentPhaseId () == eAlarmPhase)
            && (NonVentPhase::S_CalcTimeSincePhaseStart () >= minExhTime))
    {
        rtnValue = eTrue;
    }

    return (rtnValue);

}   // end S_IsInUnrestrictedPhase()

