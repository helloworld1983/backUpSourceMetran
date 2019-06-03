#ifndef INC_PHASEMGRMOCKHEADER_H_
#define INC_PHASEMGRMOCKHEADER_H_

#include "EPhaseId.h"

void phaseMgrPtrProcessNewPhase(E_PhaseId Id);
E_PhaseId phaseMgrPtrGetCurrentPhaseId(void);

E_PhaseId BDPhaseMgrGetCurrentPhaseId(void);

void PhaseMgrProcessNewPhase(E_PhaseId);

void PhaseMgrProcessNewTriggerType(E_TriggerType);

void PhaseMgrProcessNewVentType(E_VentilationType);

E_VentilationType phaseMgrPtrGetCurrentVentType(void);

void PhaseMgrResetVolumeAdjustmentOfVCVInhPhase(void);

UNSIGNED NonVentPhaseS_CalcTimeSincePhaseStart(void);
UNSIGNED InhPhaseS_CalcTimeSincePhaseStart(void);
UNSIGNED ExhPhaseS_CalcTimeSincePhaseStart(void);

E_Bool ExhPhaseS_IsInUnrestrictedPhase(void);

E_Bool APRV_PLowPhaseS_IsInUnrestrictedPhase(void);

E_Bool APRV_PLowPhaseS_IsInUnrestrictedPhase(void);

E_Bool APRV_PHighPhaseS_IsInUnrestrictedPhase(void);

E_Bool APRV_PLowPhaseS_IsInUnrestrictedPhase(UNSIGNED);

UNSIGNED APRV_SupportedInhPhase_InPLowGetTimeElapsed(void);
UNSIGNED APRV_SupportedInhPhase_InPHighGetTimeElapsed(void);

E_Bool APRV_ExhPhase_InPHighS_IsInUnrestrictedPhase(UNSIGNED);
E_Bool APRV_ExhPhase_InPLowS_IsInUnrestrictedPhase(UNSIGNED);

void AutoSIOffTimeTriggerPhaseInSettingChange(int);
void AutoSIStopTriggerPhaseInSettingChange(LONG);

E_Bool InhPhaseS_IsInUnrestrictedPhase(int);

#endif /* INC_PHASEMGRMOCKHEADER_H_ */
