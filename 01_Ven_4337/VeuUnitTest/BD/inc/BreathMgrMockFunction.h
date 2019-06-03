#ifndef INC_BREATHMGRMOCKFUNCTION_H_
#define INC_BREATHMGRMOCKFUNCTION_H_

#include "gmock.h"

class MgrBreath
{
public :
    virtual ~MgrBreath(){};

    MOCK_METHOD0(MgrBreathDataClearCompensateBuffer, void(void));

    MOCK_METHOD0(MgrBreathDataInitializeBreathDataPtr, void(void));

    MOCK_METHOD0(InhVABreathPhaseGetPIPTarget, LONG(void));
    MOCK_METHOD0(PtdataS_GetInstanceGetExhLeak, LONG(void));

    MOCK_METHOD0(InhVABreathPhaseClearPIDParameter, void(void));
    MOCK_METHOD0(MgrBreathDataNonVentStart, void(void));

    MOCK_METHOD1(MgrBreathDataAPRV_ExhPhaseStart_InPLow, void(E_BreathPhaseIndicator));

    MOCK_METHOD1(MgrBreathDataAPRV_InhPhaseStart, void(E_BreathPhaseIndicator));

    MOCK_METHOD0(MgrBreathDataExhalationStart, void(void));

    MOCK_METHOD1(MgrBreathDataPlateauStart, void(E_BreathPhaseIndicator));

    MOCK_METHOD1(MgrBreathDataAPRV_PHighPhaseStart, void(E_BreathPhaseIndicator));

    MOCK_METHOD1(MgrBreathDataAPRV_ExhPhaseStart_InPHigh, void(E_BreathPhaseIndicator));

    MOCK_METHOD1(MgrBreathDataInhalationStart, void(E_BreathPhaseIndicator));

    MOCK_METHOD0(MgrBreathDataAPRV_ExhPhaseEnd_InPHigh, void(void));

    MOCK_METHOD0(MgrBreathDataGetTidalVolumeEndInh, LONG(void));
    MOCK_METHOD0(mgrBreathDataPtrGetProxyFlow, LONG(void));

    MOCK_METHOD0(GetBdBreathDataPtrGetInhType, E_BreathPhaseIndicator(void));

    MOCK_METHOD0(InhVABreathPhaseSetPrevInhType, void(void));

    MOCK_METHOD1(InhVABreathPhaseSetPrevPIP, void(LONG));

    MOCK_METHOD0(MgrBreathDataGetMonitoredPressure, SHORT(void));

    MOCK_METHOD0(MgrBreathDataGetBdBreathDataPtrGetTimeInInh, UNSIGNED(void));

    MOCK_METHOD0(MgrBreathDataS_GetInstanceGetMonitoredPressure, long(void));

    MOCK_METHOD0(BDMgrBreathDataGetPdBreathDataPtrGetInhVolume, int(void));

    MOCK_METHOD1(BDCalcElapsedTime, int(UNSIGNED));

    MOCK_METHOD0(mgrBreathDataPtrGetDeliveredFlow, SHORT(void));

    MOCK_METHOD0(mgrBreathDataPtrGetPeakDeliveredFlow, SHORT(void));

    MOCK_METHOD0(PtdataGetExhLeak, LONG(void));

    MOCK_METHOD0(MgrBreathDataGetBdBreathDataPtrGetTimeInExh, UNSIGNED(void));
};




#endif /* INC_BREATHMGRMOCKFUNCTION_H_ */
