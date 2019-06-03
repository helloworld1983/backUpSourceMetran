#ifndef INC_PHASEMGRMOCKFUNCTION_H_
#define INC_PHASEMGRMOCKFUNCTION_H_

#include "gmock.h"
#include "EPhaseId.h"

class PhaseMgrMockFunction
{
public :
    virtual ~PhaseMgrMockFunction(){};

    MOCK_METHOD1(phaseMgrPtrProcessNewPhase, void(E_PhaseId));

    MOCK_METHOD0(phaseMgrPtrGetCurrentPhaseId, E_PhaseId(void));
    MOCK_METHOD0(BDPhaseMgrGetCurrentPhaseId, E_PhaseId(void));

    MOCK_METHOD1(PhaseMgrProcessNewPhase, void(E_PhaseId));

    MOCK_METHOD1(PhaseMgrProcessNewTriggerType, void(E_TriggerType));

    MOCK_METHOD1(PhaseMgrProcessNewVentType, void(E_VentilationType));

    MOCK_METHOD0(phaseMgrPtrGetCurrentVentType, E_VentilationType(void));

    MOCK_METHOD0(PhaseMgrResetVolumeAdjustmentOfVCVInhPhase, void(void));

    MOCK_METHOD0(NonVentPhaseS_CalcTimeSincePhaseStart, UNSIGNED(void));
    MOCK_METHOD0(InhPhaseS_CalcTimeSincePhaseStart, UNSIGNED(void));

    MOCK_METHOD0(ExhPhaseS_IsInUnrestrictedPhase, E_Bool(void));

    MOCK_METHOD0(APRV_PLowPhaseS_IsInUnrestrictedPhase, E_Bool(void));

    MOCK_METHOD0(APRV_PHighPhaseS_IsInUnrestrictedPhase, E_Bool(void));

    MOCK_METHOD1(APRV_PLowPhaseS_IsInUnrestrictedPhase, E_Bool(UNSIGNED));

    MOCK_METHOD1(APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase, E_Bool(UNSIGNED));
    MOCK_METHOD1(APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase, E_Bool(UNSIGNED));

    MOCK_METHOD0(APRV_SupportedInhPhase_InPLowGetTimeElapsed, UNSIGNED(void));
    MOCK_METHOD0(APRV_SupportedInhPhase_InPHighGetTimeElapsed, UNSIGNED(void));

    MOCK_METHOD1(AutoSIOffTimeTriggerPhaseInSettingChange, void(int));
    MOCK_METHOD1(AutoSIStopTriggerPhaseInSettingChange, void(LONG));

    MOCK_METHOD1(InhPhaseS_IsInUnrestrictedPhase, E_Bool(int));

    MOCK_METHOD0(ExhPhaseS_CalcTimeSincePhaseStart, UNSIGNED(void));

};

#endif /* INC_PHASEMGRMOCKFUNCTION_H_ */
