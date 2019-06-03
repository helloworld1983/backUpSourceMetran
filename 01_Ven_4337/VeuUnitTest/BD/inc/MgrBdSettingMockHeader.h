#ifndef INC_MGRBDSETTINGMOCKHEADER_H_
#define INC_MGRBDSETTINGMOCKHEADER_H_

LONG SettingGetCurrentValue(E_BdSettingId);

void MgrBreathDataSetFirstBreath(void);

SHORT mgrBdSettingPtrGetCurrentValue(E_BdSettingId);

void mgrBdSettingPtrProcessNewPhase(void);

void MgrBdSettingPhaseInAllSettings(void);

LONG BDMgrBdSettingGetCurrentValue(E_BdSettingId);

void MgrBreathDataAPRV_PLowPhaseStart(void);

void MgrBdSettingPhaseInNCPAPSettings(void);

void MgrBreathDataProcessNCPAP(void);

void MgrBdSettingPhaseInPIPSetting(void);

void MgrBdSettingPhaseInAllSettings(void);

void MgrBreathDataGetPdBreathDataPtrclearInhPressData(void);

double PtdataGetAverageCompliance12(void);

E_Bool MgrBdSettingGetPressTriggOn(void);

E_Bool MgrBdSettingS_GetInstanceGetPressTriggOn(void);

bool MgrBdSettingGetFlowTriggOn(void);

E_Bool O2SettingAirSourceFault(void);



#endif /* INC_MGRBDSETTINGMOCKHEADER_H_ */
