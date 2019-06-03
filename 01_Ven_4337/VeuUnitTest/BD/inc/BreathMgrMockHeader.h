#ifndef INC_BREATHMGRMOCKHEADER_H_
#define INC_BREATHMGRMOCKHEADER_H_

void MgrBreathDataClearCompensateBuffer(void);

void MgrBreathDataInitializeBreathDataPtr(void);

LONG InhVABreathPhaseGetPIPTarget(void);

void InhVABreathPhaseClearPIDParameter(void);

void MgrBreathDataNonVentStart(void);

void MgrBreathDataAPRV_ExhPhaseStart_InPLow(E_BreathPhaseIndicator);

void MgrBreathDataExhalationStart(void);

void MgrBreathDataPlateauStart(E_BreathPhaseIndicator);

void MgrBreathDataAPRV_PHighPhaseStart(E_BreathPhaseIndicator);

void MgrBreathDataAPRV_ExhPhaseStart_InPHigh(E_BreathPhaseIndicator);

void MgrBreathDataAPRV_InhPhaseStart(E_BreathPhaseIndicator);

void MgrBreathDataInhalationStart(E_BreathPhaseIndicator);

void MgrBreathDataAPRV_ExhPhaseEnd_InPHigh(void);

LONG MgrBreathDataGetTidalVolumeEndInh(void);
LONG mgrBreathDataPtrGetProxyFlow(void);
LONG PtdataS_GetInstanceGetExhLeak(void);

E_BreathPhaseIndicator GetBdBreathDataPtrGetInhType(void);

void InhVABreathPhaseSetPrevInhType(void);

void InhVABreathPhaseSetPrevPIP(LONG);

SHORT MgrBreathDataGetMonitoredPressure(void);

UNSIGNED MgrBreathDataGetBdBreathDataPtrGetTimeInInh(void);

long MgrBreathDataS_GetInstanceGetMonitoredPressure(void);

int BDMgrBreathDataGetPdBreathDataPtrGetInhVolume(void);

int BDCalcElapsedTime(UNSIGNED);

SHORT mgrBreathDataPtrGetDeliveredFlow(void);
SHORT mgrBreathDataPtrGetPeakDeliveredFlow(void);

LONG PtdataGetExhLeak(void);

UNSIGNED MgrBreathDataGetBdBreathDataPtrGetTimeInExh(void);

#endif /* INC_BREATHMGRMOCKHEADER_H_ */
