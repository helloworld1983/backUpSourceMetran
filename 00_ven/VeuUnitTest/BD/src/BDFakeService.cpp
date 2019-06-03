#include "BDFixture.h"

//******************************************************************//
//step motor mock function
void O2ValveSetStepPosition(int i)
{
    BDTestFixture::_StepMotor->O2ValveSetStepPosition(i);
}
void AirValveSetStepPosition(int i)
{
    BDTestFixture::_StepMotor->AirValveSetStepPosition(i);
}
void ExhValveSetStepPosition(int i)
{
    BDTestFixture::_StepMotor->ExhValveSetStepPosition(i);
}
void HfoValveSetStepPosition(int i)
{
    BDTestFixture::_StepMotor->HfoValveSetStepPosition(i);
}
void MgrControllerCMVServoControllerDisable(void)
{
    BDTestFixture::_StepMotor->MgrControllerCMVServoControllerDisable();
}
void MgrControllerCMVServoControllerEnable(void)
{
    BDTestFixture::_StepMotor->MgrControllerCMVServoControllerEnable();
}
void MgrControllerVCVServoControllerDisable(void)
{
    BDTestFixture::_StepMotor->MgrControllerVCVServoControllerDisable();
}
void MgrControllerVCVServoControllerEnable(void)
{
    BDTestFixture::_StepMotor->MgrControllerVCVServoControllerEnable();
}
void O2ValveEnableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->O2ValveEnableMotorErrorCheck();
}
void AirValveEnableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->AirValveEnableMotorErrorCheck();
}
void ExhValveEnableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->ExhValveEnableMotorErrorCheck();
}
void HfoValveEnableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->HfoValveEnableMotorErrorCheck();
}
void O2ValveDisableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->O2ValveDisableMotorErrorCheck();
}
void AirValveDisableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->AirValveDisableMotorErrorCheck();
}
void ExhValveDisableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->ExhValveDisableMotorErrorCheck();
}
void HfoValveDisableMotorErrorCheck(void)
{
    BDTestFixture::_StepMotor->HfoValveDisableMotorErrorCheck();
}
void PurgeFlowSetDesiredState(E_State s)
{
    BDTestFixture::_StepMotor->PurgeFlowSetDesiredState(s);
}
void SafetyValveSetDesiredState(E_State s)
{
    BDTestFixture::_StepMotor->SafetyValveSetDesiredState(s);
}
void PurgeFlowSetStateDirect(E_State s)
{
    BDTestFixture::_StepMotor->PurgeFlowSetStateDirect(s);
}
void OilPumpSetDesiredState(E_State s)
{
    BDTestFixture::_StepMotor->OilPumpSetDesiredState(s);
}
void InpedanceLineSetState(E_State s)
{
    BDTestFixture::_StepMotor->InpedanceLineSetState(s);
}
void InpedanceLineSetDesiredState(E_State s)
{
    BDTestFixture::_StepMotor->InpedanceLineSetDesiredState(s);
}
void SafetyValveSetStateDirect(E_State s)
{
    BDTestFixture::_StepMotor->SafetyValveSetStateDirect(s);
}
void HfoServoDisable(void)
{
    BDTestFixture::_StepMotor->HfoServoDisable();
}
void NCPAPControllerResetIMVStaticVariables(void)
{
    BDTestFixture::_StepMotor->NCPAPControllerResetIMVStaticVariables();
}
void AirFlowControllerEnable(void)
{
    BDTestFixture::_StepMotor->AirFlowControllerEnable();
}
void O2FlowControllerEnable(void)
{
    BDTestFixture::_StepMotor->O2FlowControllerEnable();
}
void NCPAPControllerEnable(void)
{
    BDTestFixture::_StepMotor->NCPAPControllerEnable();
}
void AirFlowControllerDisable(void)
{
    BDTestFixture::_StepMotor->AirFlowControllerDisable();
}
void O2FlowControllerDisable(void)
{
    BDTestFixture::_StepMotor->O2FlowControllerDisable();
}
void ExhControllerDisable(void)
{
    BDTestFixture::_StepMotor->ExhControllerDisable();
}
void NCPAPControllerDisable(void)
{
    BDTestFixture::_StepMotor->NCPAPControllerDisable();
}
void MgrControllerCMVServoControllerResetIMVStaticVariables(void)
{
    BDTestFixture::_StepMotor->MgrControllerCMVServoControllerResetIMVStaticVariables();
}
void MgrControllerVCVServoControllerResetIMVStaticVariables(void)
{
    BDTestFixture::_StepMotor->MgrControllerVCVServoControllerResetIMVStaticVariables();
}
void BDRequestDevice(RequestId Id)
{
    BDTestFixture::_StepMotor->BDRequestDevice(Id);
}
ProxyStatus ProxyFlowSensorgetLastStatus(void)
{
    return BDTestFixture::_StepMotor->ProxyFlowSensorgetLastStatus();
}
void MgrControllerCMVServoControllerSetExhalationTarget(LONG x)
{
    return BDTestFixture::_StepMotor->MgrControllerCMVServoControllerSetExhalationTarget(x);
}
void MgrControllerCMVServoControllerSetInhalationTarget(LONG x)
{
    return BDTestFixture::_StepMotor->MgrControllerCMVServoControllerSetInhalationTarget(x);
}
void MgrControllerVCVServoControllerSetExhalationTarget(LONG x)
{
    return BDTestFixture::_StepMotor->MgrControllerVCVServoControllerSetExhalationTarget(x);
}
void MgrControllerVCVServoControllerSetInhalationTarget(LONG x)
{
    return BDTestFixture::_StepMotor->MgrControllerVCVServoControllerSetInhalationTarget(x);
}
void PressureSensorActivePressureSensorSetPrimary(void)
{
    BDTestFixture::_StepMotor->PressureSensorActivePressureSensorSetPrimary();
}
void NCPAPControllerSetTargetFlow(LONG z)
{
    BDTestFixture::_StepMotor->NCPAPControllerSetTargetFlow(z);
}
void O2FlowControllerReset(void)
{
    BDTestFixture::_StepMotor->O2FlowControllerReset();
}
void AirFlowControllerReset(void)
{
    BDTestFixture::_StepMotor->AirFlowControllerReset();
}
void AirFlowControllerSetDesired(int x)
{
    BDTestFixture::_StepMotor->AirFlowControllerSetDesired(x);
}
void O2FlowControllerSetDesired(int x)
{
    BDTestFixture::_StepMotor->O2FlowControllerSetDesired(x);
}
void MgrControllerCMVServoControllerSetBiasFlow(SHORT x)
{
    BDTestFixture::_StepMotor->MgrControllerCMVServoControllerSetBiasFlow(x);
}
void MgrControllerVCVServoControllerSetBiasFlow(SHORT x)
{
    BDTestFixture::_StepMotor->MgrControllerVCVServoControllerSetBiasFlow(x);
}

void MgrControllerVCVServoControllerSetSepecificPhase(void)
{
    BDTestFixture::_StepMotor->MgrControllerVCVServoControllerSetSepecificPhase();
}

UNSIGNED FlowPatternPtrCalcTargetInhTime(LONG x, LONG y)
{
    return BDTestFixture::_StepMotor->FlowPatternPtrCalcTargetInhTime(x, y);
}
void FlowPatternPtrCalcCompensatedPeakFlow(LONG x)
{
    return BDTestFixture::_StepMotor->FlowPatternPtrCalcCompensatedPeakFlow(x);
}
int PressureSensorGetLastReading(void)
{
    return BDTestFixture::_StepMotor->PressureSensorGetLastReading();
}
int HfoValveGetStepPosition(void)
{
    return BDTestFixture::_StepMotor->HfoValveGetStepPosition();
}
int O2ValveGetStepPosition(void)
{
    return BDTestFixture::_StepMotor->O2ValveGetStepPosition();
}
int AirValveGetStepPosition(void)
{
    return BDTestFixture::_StepMotor->AirValveGetStepPosition();
}
int ExhValveGetStepPosition(void)
{
    return BDTestFixture::_StepMotor->ExhValveGetStepPosition();
}

void HfoValveRampToPosition(USHORT x)
{
    BDTestFixture::_StepMotor->HfoValveRampToPosition(x);
}
void O2ValveRampToPosition(USHORT x)
{
    BDTestFixture::_StepMotor->O2ValveRampToPosition(x);
}
void AirValveRampToPosition(USHORT x)
{
    BDTestFixture::_StepMotor->AirValveRampToPosition(x);
}
void ExhValveRampToPosition(USHORT x)
{
    BDTestFixture::_StepMotor->ExhValveRampToPosition(x);
}
void MgrControllerResetIMVStaticVariables(void)
{
    BDTestFixture::_StepMotor->MgrControllerResetIMVStaticVariables();
}
LONG ProxyFlowSensorGetLastReading(void)
{
    return BDTestFixture::_StepMotor->ProxyFlowSensorGetLastReading();
}
//******************************************************************//
//mode mgr mock function
void ModeMgrGoToAlarmMode(E_VentilationMode Mode)
{
    BDTestFixture::_ModeMgr->ModeMgrGoToAlarmMode(Mode);
}

void ModeMgrGoToOperatorSetMode(void)
{
    BDTestFixture::_ModeMgr->ModeMgrGoToOperatorSetMode();
}

void modeMgrPtrReturnToLastMode(void)
{
    BDTestFixture::_ModeMgr->modeMgrPtrReturnToLastMode();
}
void modeMgrPtProcessAutoReset(void)
{
    BDTestFixture::_ModeMgr->modeMgrPtProcessAutoReset();
}
void ModeMgrReturnToLastMode(void)
{
    BDTestFixture::_ModeMgr->ModeMgrReturnToLastMode();
}
void modeMgrPtrProcessAutoReset(void)
{
    BDTestFixture::_ModeMgr->modeMgrPtrProcessAutoReset();
}
E_VentilationMode BDModeMgrGetCurrentModeId(void)
{
    return BDTestFixture::_ModeMgr->BDModeMgrGetCurrentModeId();
}
E_Bool ModeS_IsInhPtTriggered(void)
{
    return BDTestFixture::_ModeMgr->ModeS_IsInhPtTriggered();
}
void ModeMgrProcessManualReset(void)
{
    BDTestFixture::_ModeMgr->ModeMgrProcessManualReset();
}
E_WindowType MixedModeGetWindow(void)
{
    return BDTestFixture::_ModeMgr->MixedModeGetWindow();
}
void HighPressModeSetCauseOfAlarm(E_GUIEventID id)
{
    BDTestFixture::_ModeMgr->HighPressModeSetCauseOfAlarm(id);
}
void modeProcessTriggerID(E_TriggerId id)
{
    BDTestFixture::_ModeMgr->modeProcessTriggerID(id);
}

//******************************************************************//
//phase mgr mock function
void phaseMgrPtrProcessNewPhase(E_PhaseId Id)
{
    BDTestFixture::_PhaseMgrPtr->phaseMgrPtrProcessNewPhase(Id);
}
void PhaseMgrProcessNewTriggerType(E_TriggerType Id)
{
    BDTestFixture::_PhaseMgrPtr->PhaseMgrProcessNewTriggerType(Id);
}
E_PhaseId phaseMgrPtrGetCurrentPhaseId(void)
{
    return BDTestFixture::_PhaseMgrPtr->phaseMgrPtrGetCurrentPhaseId();
}
E_PhaseId BDPhaseMgrGetCurrentPhaseId(void)
{
    return BDTestFixture::_PhaseMgrPtr->BDPhaseMgrGetCurrentPhaseId();
}
void PhaseMgrProcessNewPhase(E_PhaseId Id)
{
    BDTestFixture::_PhaseMgrPtr->PhaseMgrProcessNewPhase(Id);
}
void PhaseMgrProcessNewVentType(E_VentilationType type)
{
    BDTestFixture::_PhaseMgrPtr->PhaseMgrProcessNewVentType(type);
}
E_VentilationType phaseMgrPtrGetCurrentVentType(void)
{
    return BDTestFixture::_PhaseMgrPtr->phaseMgrPtrGetCurrentVentType();
}
void PhaseMgrResetVolumeAdjustmentOfVCVInhPhase(void)
{
    BDTestFixture::_PhaseMgrPtr->PhaseMgrResetVolumeAdjustmentOfVCVInhPhase();
}
UNSIGNED NonVentPhaseS_CalcTimeSincePhaseStart(void)
{
    return BDTestFixture::_PhaseMgrPtr->NonVentPhaseS_CalcTimeSincePhaseStart();
}
UNSIGNED APRV_SupportedInhPhase_InPHighGetTimeElapsed(void)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_SupportedInhPhase_InPHighGetTimeElapsed();
}
E_Bool ExhPhaseS_IsInUnrestrictedPhase(void)
{
    return BDTestFixture::_PhaseMgrPtr->ExhPhaseS_IsInUnrestrictedPhase();
}
E_Bool APRV_PLowPhaseS_IsInUnrestrictedPhase(void)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_PLowPhaseS_IsInUnrestrictedPhase();
}
E_Bool APRV_PHighPhaseS_IsInUnrestrictedPhase(void)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_PHighPhaseS_IsInUnrestrictedPhase();
}
E_Bool APRV_PLowPhaseS_IsInUnrestrictedPhase(UNSIGNED x)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_PLowPhaseS_IsInUnrestrictedPhase(x);
}
E_Bool APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase(UNSIGNED x)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase(x);
}
E_Bool APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase(UNSIGNED x)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase(x);
}
UNSIGNED APRV_SupportedInhPhase_InPLowGetTimeElapsed(void)
{
    return BDTestFixture::_PhaseMgrPtr->APRV_SupportedInhPhase_InPLowGetTimeElapsed();
}
UNSIGNED InhPhaseS_CalcTimeSincePhaseStart(void)
{
    return BDTestFixture::_PhaseMgrPtr->InhPhaseS_CalcTimeSincePhaseStart();
}
void AutoSIOffTimeTriggerPhaseInSettingChange(int x)
{
    BDTestFixture::_PhaseMgrPtr->AutoSIOffTimeTriggerPhaseInSettingChange(x);
}
void AutoSIStopTriggerPhaseInSettingChange(LONG x)
{
    BDTestFixture::_PhaseMgrPtr->AutoSIStopTriggerPhaseInSettingChange(x);
}
E_Bool InhPhaseS_IsInUnrestrictedPhase(int x)
{
    return BDTestFixture::_PhaseMgrPtr->InhPhaseS_IsInUnrestrictedPhase(x);
}
UNSIGNED ExhPhaseS_CalcTimeSincePhaseStart(void)
{
    return BDTestFixture::_PhaseMgrPtr->ExhPhaseS_CalcTimeSincePhaseStart();
}
//******************************************************************//
//trigger mgr mock function
void triggerMgrPtrEnableTrigger(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->triggerMgrPtrEnableTrigger(Id);
}
void TriggerMgrEnableTrigger(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->TriggerMgrEnableTrigger(Id);
}
void triggerMgrPtrEnableTriggerWith(E_TriggerId Id, int i)
{
    BDTestFixture::_TriggerMgr->triggerMgrPtrEnableTriggerWith(Id, i);
}

void triggerMgrPtrDisableTrigger(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->triggerMgrPtrDisableTrigger(Id);
}
void TriggerMgrDisableTrigger(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->TriggerMgrDisableTrigger(Id);
}

void TriggerMgrReEnableTrigger(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->TriggerMgrReEnableTrigger(Id);
}

void ApneaAutoResetTriggerUpdateState(E_TriggerId Id)
{
    BDTestFixture::_TriggerMgr->ApneaAutoResetTriggerUpdateState(Id);
}

UNSIGNED triggerMgrPtrGetTimePeriod(E_TriggerId Id)
{
    return BDTestFixture::_TriggerMgr->triggerMgrPtrGetTimePeriod(Id);
}
void triggerMgrPtrDisableAllTriggers(void)
{
    BDTestFixture::_TriggerMgr->triggerMgrPtrDisableAllTriggers();
}
void triggerMgrPtrEnableIMVTrigger(void)
{
    BDTestFixture::_TriggerMgr->triggerMgrPtrEnableIMVTrigger();
}
void TriggerMgrEnableExhaleTrigger(void)
{
    BDTestFixture::_TriggerMgr->TriggerMgrEnableExhaleTrigger();
}
void TriggerMgrEnableInhaleTrigger(void)
{
    BDTestFixture::_TriggerMgr->TriggerMgrEnableInhaleTrigger();
}
void ApneaAlarmTimeTriggerEnableTrigger(void)
{
    BDTestFixture::_TriggerMgr->ApneaAlarmTimeTriggerEnableTrigger();
}
void HipAlarmTriggerSetInactiveTimer(int x)
{
    BDTestFixture::_TriggerMgr->HipAlarmTriggerSetInactiveTimer(x);
}
void TriggerMgrDisableAllTriggers(void)
{
    BDTestFixture::_TriggerMgr->TriggerMgrDisableAllTriggers();
}

//******************************************************************//
//alarm mock function
SHORT BDAlarmTaskSetIsActive(E_GUIEventID Id, E_AlarmStatus Status)
{
    return BDTestFixture::_Alarm->BDAlarmTaskSetIsActive(Id, Status);
}

E_AlarmStatus BDAlarmTaskGetIsActive(E_GUIEventID Id)
{
    return BDTestFixture::_Alarm->BDAlarmTaskGetIsActive(Id);
}
E_Bool O2SettingO2SourceFault(void)
{
    return BDTestFixture::_Alarm->O2SettingO2SourceFault();
}
E_Bool alarmResetTriggerPtrGetIsArmed(void)
{
    return BDTestFixture::_Alarm->alarmResetTriggerPtrGetIsArmed();
}
E_AutoResetState apneaArTriggerPtrGetState(void)
{
    return BDTestFixture::_Alarm->apneaArTriggerPtrGetState();
}
E_AlarmLevel BDAlarmTaskGetLevel(E_GUIEventID Id)
{
    return BDTestFixture::_Alarm->BDAlarmTaskGetLevel(Id);
}
void BDAlarmTaskSetResetFlag(E_Bool x)
{
    BDTestFixture::_Alarm->BDAlarmTaskSetResetFlag(x);
}
void AlarmResetOpReqTriggerSetIsIgnoreAlarmReset(E_Bool x)
{
    BDTestFixture::_Alarm->AlarmResetOpReqTriggerSetIsIgnoreAlarmReset(x);
}
void BDAlarmTaskClearAllAlarm(void)
{
    BDTestFixture::_Alarm->BDAlarmTaskClearAllAlarm();
}
void BDAlarmTaskClearHFOAlarm(void)
{
    BDTestFixture::_Alarm->BDAlarmTaskClearHFOAlarm();
}

//******************************************************************//
//Breath Mgr mock function
void MgrBreathDataClearCompensateBuffer(void)
{
    BDTestFixture::_BreathMgr->MgrBreathDataClearCompensateBuffer();
}
void MgrBreathDataInitializeBreathDataPtr(void)
{
    BDTestFixture::_BreathMgr->MgrBreathDataInitializeBreathDataPtr();
}
LONG InhVABreathPhaseGetPIPTarget(void)
{
    return BDTestFixture::_BreathMgr->InhVABreathPhaseGetPIPTarget();
}
void InhVABreathPhaseClearPIDParameter(void)
{
    BDTestFixture::_BreathMgr->InhVABreathPhaseClearPIDParameter();
}
void MgrBreathDataNonVentStart(void)
{
    BDTestFixture::_BreathMgr->MgrBreathDataNonVentStart();
}
void MgrBreathDataAPRV_ExhPhaseStart_InPLow(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataAPRV_ExhPhaseStart_InPLow(x);
}
void MgrBreathDataAPRV_InhPhaseStart(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataAPRV_InhPhaseStart(x);
}
void InhVABreathPhaseSetPrevPIP(LONG x)
{
    BDTestFixture::_BreathMgr->InhVABreathPhaseSetPrevPIP(x);
}

void MgrBreathDataExhalationStart(void)
{
    BDTestFixture::_BreathMgr->MgrBreathDataExhalationStart();
}

void MgrBreathDataPlateauStart(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataPlateauStart(x);
}

void MgrBreathDataInhalationStart(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataInhalationStart(x);
}
void MgrBreathDataAPRV_PHighPhaseStart(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataAPRV_PHighPhaseStart(x);
}
void MgrBreathDataAPRV_ExhPhaseStart_InPHigh(E_BreathPhaseIndicator x)
{
    BDTestFixture::_BreathMgr->MgrBreathDataAPRV_ExhPhaseStart_InPHigh(x);
}

void MgrBreathDataAPRV_ExhPhaseEnd_InPHigh(void)
{
    BDTestFixture::_BreathMgr->MgrBreathDataAPRV_ExhPhaseEnd_InPHigh();
}
void InhVABreathPhaseSetPrevInhType(void)
{
    BDTestFixture::_BreathMgr->InhVABreathPhaseSetPrevInhType();
}

LONG MgrBreathDataGetTidalVolumeEndInh(void)
{
    return BDTestFixture::_BreathMgr->MgrBreathDataGetTidalVolumeEndInh();
}
LONG mgrBreathDataPtrGetProxyFlow(void)
{
    return BDTestFixture::_BreathMgr->mgrBreathDataPtrGetProxyFlow();
}
LONG PtdataS_GetInstanceGetExhLeak(void)
{
    return BDTestFixture::_BreathMgr->PtdataS_GetInstanceGetExhLeak();
}

E_BreathPhaseIndicator GetBdBreathDataPtrGetInhType(void)
{
    return BDTestFixture::_BreathMgr->GetBdBreathDataPtrGetInhType();
}
SHORT MgrBreathDataGetMonitoredPressure(void)
{
    return BDTestFixture::_BreathMgr->MgrBreathDataGetMonitoredPressure();
}

UNSIGNED MgrBreathDataGetBdBreathDataPtrGetTimeInInh(void)
{
    return BDTestFixture::_BreathMgr->MgrBreathDataGetBdBreathDataPtrGetTimeInInh();
}

long MgrBreathDataS_GetInstanceGetMonitoredPressure(void)
{
    return BDTestFixture::_BreathMgr->MgrBreathDataS_GetInstanceGetMonitoredPressure();
}
int BDMgrBreathDataGetPdBreathDataPtrGetInhVolume(void)
{
    return BDTestFixture::_BreathMgr->BDMgrBreathDataGetPdBreathDataPtrGetInhVolume();
}
int BDCalcElapsedTime(UNSIGNED x)
{
    return BDTestFixture::_BreathMgr->BDCalcElapsedTime(x);
}
SHORT mgrBreathDataPtrGetDeliveredFlow(void)
{
    return BDTestFixture::_BreathMgr->mgrBreathDataPtrGetDeliveredFlow();
}
SHORT mgrBreathDataPtrGetPeakDeliveredFlow(void)
{
    return BDTestFixture::_BreathMgr->mgrBreathDataPtrGetPeakDeliveredFlow();
}
LONG PtdataGetExhLeak(void)
{
    return BDTestFixture::_BreathMgr->PtdataGetExhLeak();
}
UNSIGNED MgrBreathDataGetBdBreathDataPtrGetTimeInExh(void)
{
    return BDTestFixture::_BreathMgr->MgrBreathDataGetBdBreathDataPtrGetTimeInExh();
}
//******************************************************************//
//Pd Task mock function
void PdTaskS_SetEventFlag(int i)
{
    BDTestFixture::_PdTask->PdTaskS_SetEventFlag(i);
}

//******************************************************************//
//Timer mock function
void mStart(void)
{
    BDTestFixture::_Timer->mStart();
}
void mStop(void)
{
    BDTestFixture::_Timer->mStop();
}
void mReset(void)
{
    BDTestFixture::_Timer->mReset();
}

//******************************************************************//
//Mgr BD Setting mock function
void SettingGetCurrentValue(E_BdSettingId Id)
{
    BDTestFixture::_MgrBdSetting->SettingGetCurrentValue(Id);
}
void mgrBdSettingPtrProcessNewPhase(void)
{
    BDTestFixture::_MgrBdSetting->mgrBdSettingPtrProcessNewPhase();
}
void MgrBreathDataSetFirstBreath(void)
{
    BDTestFixture::_MgrBdSetting->MgrBreathDataSetFirstBreath();
}
SHORT mgrBdSettingPtrGetCurrentValue(E_BdSettingId Id)
{
    return BDTestFixture::_MgrBdSetting->mgrBdSettingPtrGetCurrentValue(Id);
}

void MgrBdSettingPhaseInAllSettings(void)
{
    BDTestFixture::_MgrBdSetting->MgrBdSettingPhaseInAllSettings();
}
LONG BDMgrBdSettingGetCurrentValue(E_BdSettingId x)
{
    return BDTestFixture::_MgrBdSetting->BDMgrBdSettingGetCurrentValue(x);
}
void MgrBreathDataAPRV_PLowPhaseStart(void)
{
    BDTestFixture::_MgrBdSetting->MgrBreathDataAPRV_PLowPhaseStart();
}
void MgrBdSettingPhaseInNCPAPSettings(void)
{
    BDTestFixture::_MgrBdSetting->MgrBdSettingPhaseInNCPAPSettings();
}
void MgrBreathDataProcessNCPAP(void)
{
    BDTestFixture::_MgrBdSetting->MgrBreathDataProcessNCPAP();
}
void MgrBdSettingPhaseInPIPSetting(void)
{
    BDTestFixture::_MgrBdSetting->MgrBdSettingPhaseInPIPSetting();
}
void MgrBreathDataGetPdBreathDataPtrclearInhPressData(void)
{
    BDTestFixture::_MgrBdSetting->MgrBreathDataGetPdBreathDataPtrclearInhPressData();
}

double PtdataGetAverageCompliance12(void)
{
    return BDTestFixture::_MgrBdSetting->PtdataGetAverageCompliance12();
}
E_Bool MgrBdSettingGetPressTriggOn(void)
{
    return BDTestFixture::_MgrBdSetting->MgrBdSettingGetPressTriggOn();
}
E_Bool MgrBdSettingS_GetInstanceGetPressTriggOn(void)
{
    return BDTestFixture::_MgrBdSetting->MgrBdSettingS_GetInstanceGetPressTriggOn();
}
E_Bool O2SettingAirSourceFault(void)
{
    return BDTestFixture::_MgrBdSetting->O2SettingAirSourceFault();
}
bool MgrBdSettingGetFlowTriggOn(void)
{
    return BDTestFixture::_MgrBdSetting->MgrBdSettingGetFlowTriggOn();
}

