/******************************************************************************/
//$COMMON.CPP$
//   File Name:  SupportedInhPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the definition of the Supported Inhalation Phase 
//      class.  Supported inhalation phases include PSV inhalations and
//      spontaneous inhalations in NIPPV.   
//
//   Interfaces:
//      Operations in this class interface with the TriggerMgr object to
//      enable and disable triggers specific to the supported inhalation 
//      phase.  Operations in this class also invoke base class PressInhPhase
//      operations to perform processing common to all pressure based 
//      inhalations.
//
//      The Start() operations interfaces with MgrBreathData to notify that
//      object of the start of the inhalation phase.
//
//   Restrictions:
//      None
/******************************************************************************/
#include "SupportedInhPhase.h"
#include "MgrBreathData.h"
#include "TriggerMgr.h"
#include "MgrController.h"
#include "MgrBdSetting.h"
#include "ModeMgr.h"
#include "InhVABreathPhase.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SupportedInhPhase()
//
//    Processing: 
//      This operation is the constructor for the SupportedInhPhase class.
//      This operation invokes the constructor of its base class 
//      PressInhPhase.
//
//    Input Parameters:
//      targetPressId - setting ID of the target pressure (support pressure for
//          PSV, IPAP for NIPPV)
//
//      baselinePressId - setting ID of the baseline pressure (PEEP for PSV,
//          dummyBaseline for NIPPV).  In NIPPV, the target pressure (IPAP) is
//          not offset by the baseline pressure (EPAP); therefore a dummy
//          baseline is used.
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
SupportedInhPhase::SupportedInhPhase (E_BdSettingId targetPressId,
                                      E_BdSettingId baselinePressId) :
        PressInhPhase (targetPressId, baselinePressId)
{

}   // end SupportedInhPhase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing: 
//      This operation is invoked at the start of any supported spontaneous
//      inhalation phase.  This operation first calls the PressInhPhase::Start()
//      operation to perform processing common to all pressure based inhalation
//      phases.  This operation then interfaces with the TriggerMgr object
//      to enable the flow, pressure and time triggers that can end a 
//      supported inhalation.
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
void SupportedInhPhase::Start (void)
{
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();// pointer to the TriggerMgr object
    E_BreathPhaseIndicator inhType;  // Spontaneous or Supported
    LONG desiredPress;

    // Invoke base class method to perform processing common to all pressure
    // based inhalation phases.
    PressInhPhase::Start();

    // Enable the Inh Pressure Controller!  Passing target and
    // initial pressure sets up the gain factors.
    LONG PS = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress);

    //for new VA
#ifdef VA_WITH_PS //for new VA
    E_PhaseId id = (PhaseMgr::S_GetInstance())->GetCurrentPhaseId();
    if(id == ePsvInhVABreathPhase){
        PS = InhVABreathPhase::S_GetInstance()->CompPIP()-BaseLine;
    }
    else{
        PS = MgrBdSetting::S_GetInstance()->GetCurrentValue(eSupportPress);
    }
#endif
    //for new VA

    if (PS > 0)
    {
        inhType = eSupported;
        desiredPress = PS + BaseLine;
        triggerMgrPtr->EnableTrigger(ePtExhFlowSuppTrigger);

    }
    else
    {
        inhType = eSpontaneous;
        desiredPress = BaseLine;
        triggerMgrPtr->EnableTrigger(ePtExhFlowSpontTrigger);
    }

    MgrController::CMVServoController->SetInhalationTarget(desiredPress/10,RiseTime);
    MgrController::VCVServoController->SetInhalationTarget(desiredPress/10,RiseTime);
    MgrController::VCVServoController->SetSepecificPhase(ePsvInhPhase);

    E_VentilationMode currentMode = ModeMgr::S_GetInstance()->GetCurrentModeId();
    if(currentMode == eVaMixedMode)
    {
        InhVABreathPhase::S_GetInstance()->SetPrevInhType();
    }

    // Enable the flow, pressure and time triggers than can terminate a
    // supported inhalation.


    // Update patient data
    (MgrBreathData::S_GetInstance())->InhalationStart(inhType);

    TriggerMgr::S_GetInstance()->EnableTrigger(eItLongAlarmTimeTrigger);
}   // end Start()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing: 
//      This operation is invoked at the end of a supported inhalation phase.
//      It first calls the PressInhPhase::End() operation to perform processing 
//      common to all pressure based inhalation phases.  This operation then 
//      interfaces with the TriggerMgr object to disable the flow, pressure 
//      and time triggers that were enabled at the start of the inhalation
//      phase.
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
//
//    Requirements:  
//
/******************************************************************************/
void SupportedInhPhase::End (void)
{
    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    // Get handle to the TriggerMgr object
    triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Disable triggers
    triggerMgrPtr->DisableTrigger(ePtExhFlowSuppTrigger);
    triggerMgrPtr->DisableTrigger(ePtExhFlowSpontTrigger);
    triggerMgrPtr->DisableTrigger(eItLongAlarmTimeTrigger);
    triggerMgrPtr->DisableTrigger(eHipAlarmTrigger);

}   // end End()

