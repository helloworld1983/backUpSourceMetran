/******************************************************************************/
//$COMMON.CPP$
//   File Name:  InhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: InhPhase is the base class for all Inhalation Phase classes.
//      Operations in this class perform basic functionality common to all
//      inhalation phases.
//
//   Interfaces:
//      At the start of each inhalation InhPhase::Start() is invoked which
//      interfaces with the TriggerMgr to enable triggers, such as the 
//      Hip1AlarmTrigger, which are enabled for any inhalation.  The Start()
//      operation also interfaces with the MgrBdSetting class to phase in
//      settings at the start of the breath phase.  And, Start() invokes
//      the OS kernel service SystemClock::Retrieve() to snapshot the 
//      inhalation start time.
//
//   Restrictions:
//      This class is an abstract base class and therefore is not instantiated. 
/******************************************************************************/
#include <InhPhase.h>
#include "TriggerMgr.h"
#include "MgrBdSetting.h"
#include "PhaseMgr.h"

// define statics
UNSIGNED InhPhase::S_InhStartTime = 0;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InhPhase()
//
//    Processing: 
//      This operation is the constructor for the InhPhase.  It invokes the
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
InhPhase::InhPhase (void) :
        Phase ()
{

}   // end InhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked from the Start() operation of each of the 
//      specific inhalation phase classes derived from the InhPhase base class.
//      This operation performs processing which is common to all inhalation
//      phases.
//
//      In particular, this operation invokes MgrBdSetting::ProcessNewPhase()
//      to phase in any settings changes that may have been received from
//      the GUI.  It also enables the HIP-1 and occlusion alarm triggers 
//      and re-enables the Apnea Alarm Time Trigger by invoking operations 
//      in the TriggerMgr.
//          
//      Finally, the system clock is snapshot and stored in the S_InhStartTime
//      attribute.
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
void InhPhase::Start (void)
{
    // Initialize a pointer to the TriggerMgr
    TriggerMgr::S_GetInstance()->EnableInhaleTrigger();
//    S_InhStartTime = SystemClock::Retrieve();

}   // end Start()

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
E_Bool InhPhase::S_IsInUnrestrictedPhase(UNSIGNED minInhTime)
{

    E_Bool rtnValue = eFalse;
    E_PhaseId phase = PhaseMgr::S_GetInstance()->GetCurrentPhaseId();

    // If the breath phase is exhalation and the time elapsed since the start
    // of exhalation is greater than or equal to the minimum exhalation time,
    // return TRUE because the ventilator is in the unrestricted phase.
    if (((phase == eMandInhPhase)||(phase == eSpontInhPhase))
            &&(S_CalcTimeSincePhaseStart() >= minInhTime))
    {
        rtnValue = eTrue;
    }

    return(rtnValue);

}   // end S_IsInUnrestrictedPhase()

