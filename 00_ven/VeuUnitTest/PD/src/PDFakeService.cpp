#include "PDFixture.h"

//Fake funtion used in event group
EventGroupHandle_t xEventGroupCreate( void )
{
    return PDTestFixture::_EventGroupMock->xEventGroupCreate();
}
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )
{
    return PDTestFixture::_EventGroupMock->xEventGroupSetBits(xEventGroup, uxBitsToSet);
}
EventBits_t xEventGroupGetBits( EventGroupHandle_t xEventGroup)
{
    return PDTestFixture::_EventGroupMock->xEventGroupGetBits(xEventGroup);
}
EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )
{
    return PDTestFixture::_EventGroupMock->xEventGroupClearBits(xEventGroup, uxBitsToClear);
}
EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait )
{
    return PDTestFixture::_EventGroupMock->xEventGroupWaitBits(xEventGroup, uxBitsToWaitFor,
                                                               xClearOnExit, xWaitForAllBits,xTicksToWait);
}

//fake function use in PdAlarm
E_AlarmStatus GetIsActive (int AlarmId)
{
    return PDTestFixture::_PdAlarmMock->GetIsActive(AlarmId);
}
E_ReturnStatus SetIsActive (int AlarmId, E_AlarmStatus stat)
{
    return PDTestFixture::_PdAlarmMock->SetIsActive(AlarmId, stat);
}

//Fake function use in PtData
E_PhaseId PhaseMgrGetCurrentPhaseId(void)
{
    return PDTestFixture::_PtDatamMock->PhaseMgrGetCurrentPhaseId();
}
E_VentilationMode ModeMgrGetCurrentModeId(void)
{
    return PDTestFixture::_PtDatamMock->ModeMgrGetCurrentModeId();
}
E_VentilationMode ModeMgrGetNextModeId(void)
{
    return PDTestFixture::_PtDatamMock->ModeMgrGetNextModeId();
}
ProxyStatus RequestDevice(void)
{
    return PDTestFixture::_PtDatamMock->RequestDevice();
}

LONG MgrBdSettingGetCurrentValue(E_BdSettingId Id)
{
    return PDTestFixture::_PtDatamMock->MgrBdSettingGetCurrentValue(Id);
}
void PdAlarmsCheckHiCondition(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHiCondition(x, y, z);
}
void PdAlarmsCheckLowCondition(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowCondition(x, y, z);
}
LONG MgrBreathDataGetPdBreathDataPtrGetPressureSum(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetPressureSum();
}
void PdAlarmsCheckHiBaseLine(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHiBaseLine(x, y, z);
}
void PdAlarmsCheckLowBaseLine(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowBaseLine(x, y, z);
}
LONG MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume();
}
void MgrBreathDataSetPdDone(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataSetPdDone();
}
void PdAlarmsResetPEEPAlarmActiveTime(void)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsResetPEEPAlarmActiveTime();
}
LONG MgrBreathDataGetPdBreathDataPtrGetExhVolume(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetExhVolume();
}
LONG MgrBreathDataGetPdBreathDataPtrGetEndExhPress(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetEndExhPress();
}
LONG MgrBreathDataGetPdBreathDataPtrGetNumExhSamples(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetNumExhSamples();
}
LONG MgrBreathDataGetPdBreathDataPtrGetTimeInExh(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetTimeInExh();
}
E_BreathPhaseIndicator MgrBreathDataGetPdBreathDataPtrGetInhType(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetInhType();
}
LONG MgrBreathDataGetPdBreathDataPtrGetPeakInhPress(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetPeakInhPress();
}
LONG MgrBreathDataGetPdBreathDataPtrGetEndInhPress(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetEndInhPress();
}
LONG MgrBreathDataGetPdBreathDataPtrGetNumInhSamples(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetNumInhSamples();
}
ULONG MgrBreathDataGetPdBreathDataPtrGetTimeInInh(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetTimeInInh();
}
void PdAlarmsCheckHiO2(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHiO2(x, y, z);
}
void PdAlarmsCheckLowO2(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowO2(x, y, z);
}
void PdAlarmsCheckLowO2(E_GUIEventID x, E_AlarmStatus y)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowO2(x, y);
}
ProxyStatus DeviceInterfaceRequestDevice(RequestId id)
{
    return PDTestFixture::_PtDatamMock->DeviceInterfaceRequestDevice(id);
}
LONG MgrBreathDataGetPdBreathDataPtrGetInhVolume(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetInhVolume();
}
double MgrBreathDataGetLeakVolTime(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetLeakVolTime();
}
double MgrBreathDataGetInhVolOneBRTime(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetInhVolOneBRTime();
}
double MgrBreathDataGetExhVolOneBRTime(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetExhVolOneBRTime();
}
void MgrBreathDataResetVol(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataResetVol();
}
LONG MgrBreathDataGetPdBreathDataPtrGetVte(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetVte();
}
void PdAlarmsCheckLowPip(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowPip(x, y, z);
}
void PdAlarmsCheckHiPip(LONG x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHiPip(x, y, z);
}
LONG MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh();
}
void PdAlarmsCheckHighPHigh5Time(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHighPHigh5Time(x, y, z);
}
void PdAlarmsCheckLowPHigh5Time(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowPHigh5Time(x, y, z);
}
E_AlarmStatus AlarmTaskGetIsActive(E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->AlarmTaskGetIsActive(z);
}
void PdAlarmsResetPLowAlarmActiveTime(void)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsResetPLowAlarmActiveTime();
}
LONG MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow();
}
void PdAlarmsCheckHighPLow5Time(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckHighPLow5Time(x, y, z);
}
void PdAlarmsCheckLowPLow5Time(SHORT x, LONG y, E_GUIEventID z)
{
    return PDTestFixture::_PtDatamMock->PdAlarmsCheckLowPLow5Time(x, y, z);
}
void MgrBreathDataResetAPRVBR(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataResetAPRVBR();
}
E_ReturnStatus TaskSetIsActive(E_GUIEventID x, E_AlarmStatus y)
{
    return PDTestFixture::_PtDatamMock->TaskSetIsActive(x, y);
}
double MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow();
}
LONG MgrBreathDataGetDeliveredFlow(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetDeliveredFlow();
}
LONG MgrBreathDataGetBdBreathDataPtrGetEndExhFlow(void)
{
    return PDTestFixture::_PtDatamMock->MgrBreathDataGetBdBreathDataPtrGetEndExhFlow();
}
void AlarmTaskSetIsActive(E_GUIEventID x, E_AlarmStatus y)
{
    return PDTestFixture::_PtDatamMock->AlarmTaskSetIsActive(x, y);
}
void CallThis(void)
{
    return PDTestFixture::_PtDatamMock->CallThis();
}
