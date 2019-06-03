/******************************************************************************/
//$COMMON.CPP$
//   File Name:  MandPressInhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the Mandatory Pressure Inhalation Phase class.  This 
//      class is a base class for pressure based mandatory inhalations.  
//      The pressure based mandatory inhalations include the mandatory 
//      breaths when the Vent Type is Pressure (PCV) or NIPPV.
//
//   Interfaces:
//      The Start() and End() operations in this class interface with the
//      TriggerMgr object to enable and disable triggers via the EnableTrigger()
//      operation.  The Start() operation also calls Mode::S_IsInhPtTriggered() 
//      to determine if the inhalation is assisted or mandatory.  Start()
//      also interfaces with MgrBreathData to notify that object of the start
//      of the inhalation phase via a call to InhalationStart().
//
//   Restrictions:
//      None
/******************************************************************************/
#include "MandPressInhPhase.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "TriggerMgr.h"
#include "Mode.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MandPressInhPhase()
//
//    Processing: 
//      This operation is the constructor for the MandPressInhPhase.  It 
//      invokes the PressInhPhase base class constructor.
//
//    Input Parameters:
//      inhPressId -- setting ID of the inhalation pressure; this is
//          the inhalation Pressure in PCV and the IPAP value in NIPPV
//
//      baselinePressId -- setting ID of the baseline pressure; this is the
//          PEEP value for PCV is a dummy 0 value for NIPPV breaths.  
//          Target pressure in NIPPV is not offset by EPAP; target pressure 
//          in PCV is offset by PEEP.
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
//      None
//
/******************************************************************************/
MandPressInhPhase::MandPressInhPhase (E_BdSettingId targetPressId,
                                      E_BdSettingId baselinePressId) :
        PressInhPhase (targetPressId, baselinePressId)
{

}   // end MandPressInhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of each mandatory pressure
//      based inhalation (PCV or mandatory breath in NIPPV).  This operation
//      invokes the base class PressInhPhase::Start() to perform processing
//      common to all pressure based inhalation.  It then enables the 
//      Mandatory Pressure Inhalation Time Trigger.
//
//      This operation calls Mode::S_IsInhPtTriggered() to determine if the
//      inhalation was patient triggered or not.  It then calls 
//      MgrBreathData::InhalationStart() to notify that object of the start
//      of the assisted or mandatory inhalation phase.
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
void MandPressInhPhase::Start (void)
{
    // Invoke base class method to perform processing common to all 
    // pressure based inhalation phases.
    PressInhPhase::Start ();

    // Enable the Inh Pressure Controller!  Target and Initial pressure are used
    // to set up the gain factors.  Mandatory and assisted breaths
    // are treated the same by the controller, so assume it's mandatory.
//    InhController->Enable(TargetPressure, InitialPressure, eMandatory);

    // Get a handle to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance ();

    // Enable the inhalation time trigger
    triggerMgrPtr->EnableTrigger (eMandPressInhTimeTrigger);

    // Enable the Hip2 trigger; Hip1 was enabled in the InhPhase base class.
    triggerMgrPtr->EnableTrigger (eHip2AlarmTrigger);

    // Update patient data;  need to know how the breath was triggered
    // to determine whether or not it is a mandatory or assisted breath
    if(Mode::S_IsInhPtTriggered () == eTrue)
    {
        (MgrBreathData::S_GetInstance ())->InhalationStart (eAssisted);
    }
    else
    {
        (MgrBreathData::S_GetInstance ())->InhalationStart (eMandatory);
    }

}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of a mandatory pressure
//      based inhalation (PCV or mandatory breath in NIPPV).  This operation
//      invokes the base class PressInhPhase::End() to perform processing
//      common to all pressure based inhalations.  It then disables the 
//      Mandatory Pressure Inhalation Time Trigger.
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
void MandPressInhPhase::End (void)
{

    // Invoke base class method to do perfrom processing common to all
    // Pressure Based inhalation phases.
    PressInhPhase::End ();

    // Disable the inhalation time trigger.
    (TriggerMgr::S_GetInstance ())->DisableTrigger (eMandPressInhTimeTrigger);

}   // end End()

