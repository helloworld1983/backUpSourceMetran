#ifndef SRC_PTDATAMOCKFUNCTION_H_
#define SRC_PTDATAMOCKFUNCTION_H_

#include "gmock.h"
#include "PtData.h"
#include "CommonTypes.h"

class FunctionUsedInPtData
{
public:
    virtual ~FunctionUsedInPtData(void){};

    MOCK_METHOD0(PhaseMgrGetCurrentPhaseId, E_PhaseId(void));
    MOCK_METHOD0(ModeMgrGetCurrentModeId, E_VentilationMode(void));
    MOCK_METHOD0(ModeMgrGetNextModeId, E_VentilationMode(void));
    MOCK_METHOD0(RequestDevice, ProxyStatus(void));
    MOCK_METHOD1(MgrBdSettingGetCurrentValue, LONG(E_BdSettingId));
    MOCK_METHOD3(PdAlarmsCheckHiCondition, void(LONG, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckLowCondition, void(LONG, LONG, E_GUIEventID));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetPressureSum, LONG(void));
    MOCK_METHOD3(PdAlarmsCheckHiBaseLine, void(SHORT, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckLowBaseLine, void(SHORT, LONG, E_GUIEventID));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetCompensatedExhVolume, LONG(void));
    MOCK_METHOD0(MgrBreathDataSetPdDone, void(void));
    MOCK_METHOD0(PdAlarmsResetPEEPAlarmActiveTime, void(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetExhVolume, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetEndExhPress, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetNumExhSamples, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetTimeInExh, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetInhType, E_BreathPhaseIndicator(void));

    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetPeakInhPress, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetEndInhPress, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetNumInhSamples, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetTimeInInh, ULONG(void));

    MOCK_METHOD3(PdAlarmsCheckHiO2, void(LONG, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckLowO2, void(LONG, LONG, E_GUIEventID));

    MOCK_METHOD2(PdAlarmsCheckLowO2, void(E_GUIEventID, E_AlarmStatus));

    MOCK_METHOD1(DeviceInterfaceRequestDevice, ProxyStatus(RequestId Id));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetInhVolume, LONG(void));

    MOCK_METHOD0(MgrBreathDataGetLeakVolTime, double(void));
    MOCK_METHOD0(MgrBreathDataGetInhVolOneBRTime, double(void));
    MOCK_METHOD0(MgrBreathDataGetExhVolOneBRTime, double(void));

    MOCK_METHOD0(MgrBreathDataResetVol, void(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetVte, LONG(void));

    MOCK_METHOD3(PdAlarmsCheckLowPip, void(LONG, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckHiPip, void(LONG, LONG, E_GUIEventID));

    MOCK_METHOD0(MgrBreathDataGetBdBreathDataPtrGetAPRV_PHigh, LONG(void));

    MOCK_METHOD3(PdAlarmsCheckHighPHigh5Time, void(SHORT, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckLowPHigh5Time, void(SHORT, LONG, E_GUIEventID));

    MOCK_METHOD1(AlarmTaskGetIsActive, E_AlarmStatus(E_GUIEventID));
    MOCK_METHOD0(PdAlarmsResetPLowAlarmActiveTime, void(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrGetAPRV_PLow, LONG(void));

    MOCK_METHOD3(PdAlarmsCheckHighPLow5Time, void(SHORT, LONG, E_GUIEventID));
    MOCK_METHOD3(PdAlarmsCheckLowPLow5Time, void(SHORT, LONG, E_GUIEventID));

    MOCK_METHOD0(MgrBreathDataResetAPRVBR, void(void));

    MOCK_METHOD2(TaskSetIsActive, E_ReturnStatus(E_GUIEventID, E_AlarmStatus));

    MOCK_METHOD0(MgrBreathDataGetBdBreathDataPtrGetAPRV_PLow, double(void));

    MOCK_METHOD0(MgrBreathDataGetDeliveredFlow, LONG(void));
    MOCK_METHOD0(MgrBreathDataGetBdBreathDataPtrGetEndExhFlow, LONG(void));

    MOCK_METHOD2(AlarmTaskSetIsActive, void(E_GUIEventID, E_AlarmStatus));

    MOCK_METHOD0(CallThis, void(void));
};

#endif /* SRC_PTDATAMOCKFUNCTION_H_ */
