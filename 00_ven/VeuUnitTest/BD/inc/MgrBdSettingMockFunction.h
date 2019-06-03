#ifndef INC_MGRBDSETTINGMOCKFUNCTION_H_
#define INC_MGRBDSETTINGMOCKFUNCTION_H_

#include "gmock.h"

class MgrBdSettingMockFucntion
{
public:
    MOCK_METHOD1(SettingGetCurrentValue, LONG(E_BdSettingId));

    MOCK_METHOD0(MgrBreathDataSetFirstBreath, void(void));

    MOCK_METHOD1(mgrBdSettingPtrGetCurrentValue, SHORT(E_BdSettingId));

    MOCK_METHOD0(mgrBdSettingPtrProcessNewPhase, void(void));
    MOCK_METHOD0(MgrBdSettingPhaseInAllSettings, void(void));

    MOCK_METHOD1(BDMgrBdSettingGetCurrentValue, LONG(E_BdSettingId));
    MOCK_METHOD0(MgrBreathDataAPRV_PLowPhaseStart, void(void));

    MOCK_METHOD0(MgrBdSettingPhaseInNCPAPSettings, void(void));

    MOCK_METHOD0(MgrBreathDataProcessNCPAP, void(void));

    MOCK_METHOD0(MgrBdSettingPhaseInPIPSetting, void(void));
    MOCK_METHOD0(MgrBreathDataGetPdBreathDataPtrclearInhPressData, void(void));

    MOCK_METHOD0(PtdataGetAverageCompliance12, double(void));

    MOCK_METHOD0(MgrBdSettingGetPressTriggOn, E_Bool(void));

    MOCK_METHOD0(MgrBdSettingS_GetInstanceGetPressTriggOn, E_Bool(void));

    MOCK_METHOD0(MgrBdSettingGetFlowTriggOn, bool(void));

    MOCK_METHOD0(O2SettingAirSourceFault, E_Bool(void));
};


#endif /* INC_MGRBDSETTINGMOCKFUNCTION_H_ */
