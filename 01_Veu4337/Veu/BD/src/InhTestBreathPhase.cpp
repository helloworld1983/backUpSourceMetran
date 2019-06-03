/******************************************************************************/
//$COMMON.CPP$
//   File Name:  InhTestBreathPhase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: InhTestBreathPhase is the phase at the beginning of VA mode
//            At the beginning of VA mode, The vent starts 3 test breath to compute
//            the averaged Tidal Volume.The Insp Pressure in the test breath:
//                      Target Pressure = 0.7*(PIP setting)
//
//
/******************************************************************************/
#include "InhTestBreathPhase.h"
#include "TriggerMgr.h"
#include "MgrBreathData.h"
#include "MgrBdSetting.h"
#include "MgrController.h"
#include "InhVABreathPhase.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: InhTestBreathPhase()
//
//    Processing:
//
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      LONG PIP
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
InhTestBreathPhase::InhTestBreathPhase
(
    E_BdSettingId targetPressId,
    E_BdSettingId offsetPressId
    )
    : PressInhPhase(targetPressId, offsetPressId)
{

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Start()
//
//    Processing:
//      This operation is invoked at the start of VA Mode
//
//      This operation invokes the base class PressInhPhase::Start() operation to
//      perform processing common to all inhalation phases.  It then calls
//      MgrBdSetting::GetCurrentValue() to retrieve the inhalation, baseline
//      and offset pressures based upon the OffsetPressureId and
//      InhPressId attributes.
//
//      The Target Pressure, Initial Pressure  and Rise Time are determined by
//      this operation based upon operator settings.
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
void InhTestBreathPhase::Start(void)
{
    E_BreathPhaseIndicator breathType;

    breathType = (Mode::S_IsInhPtTriggered() == eTrue) ? eAssisted : eMandatory;

    // Invoke base class method to perform processing common to all
    // pressure based inhalation phases.
    PressInhPhase::Start();

    TargetPressure = (LONG)((double)TargetPressure*0.7);
    LONG lowPIPAlarmSetting = MgrBdSetting::S_GetInstance()->GetCurrentValue(eLipLimit);
    if(TargetPressure < lowPIPAlarmSetting)
        TargetPressure = lowPIPAlarmSetting;

    MgrController::CMVServoController->SetInhalationTarget(TargetPressure/10,RiseTime);

    // Get a handle to the Trigger Manager
    TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();

    // Enable the inhalation time trigger
    triggerMgrPtr->EnableTrigger(eMandPressInhTimeTrigger);
    // Update patient data;
    (MgrBreathData::S_GetInstance())->InhalationStart(breathType);
    InhVABreathPhase::S_GetInstance()->SetPrevPIP(TargetPressure);
    InhVABreathPhase::S_GetInstance()->ClearPIDParameter();

}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: End()
//
//    Processing:
//      This operation is invoked at the end of each pressure based inhalation.
//      This operation disables the Inhalation Pressure Controller.
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
void InhTestBreathPhase::End(void)
{
    TriggerMgr* triggerMgrPtr;  // pointer to the TriggerMgr

    triggerMgrPtr = TriggerMgr::S_GetInstance();

    triggerMgrPtr->DisableTrigger(eMandPressInhTimeTrigger);

    triggerMgrPtr->DisableTrigger(eHip2AlarmTrigger);

    triggerMgrPtr->DisableTrigger(eHipAlarmTrigger);
}

