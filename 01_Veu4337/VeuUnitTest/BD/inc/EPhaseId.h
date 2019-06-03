#ifndef INC_EPHASEID_H_
#define INC_EPHASEID_H_

#include "BdInterface.h"

enum E_PhaseId
{
    eInitPhase,
    eNonVentPhase, eExhHoldPhase, eAlarmPhase = eNonVentPhase,
    eDiagnosticPhase,
    eSelfTestPhase,
    eMandInhPhase,

    eSpontInhPhase,
    eSpontInhPhase_InAPRV_PLow,
    eSpontInhPhase_InAPRV_PHigh,
    ePlateauPhase,

    eMandInhTestBreathPhase,
    eMandInhVABreathPhase,

#ifdef HFO_SYSTEM
    eHFOPhase,
#endif
    eExhPhase,
    eAPRV_PLow_Phase,
    eAPRVLowPhase,
    eAPRVHighPhase,
    eAPRVExhPhase_InPHigh,
    eAPRVExhPhase_InPLow,

#ifdef  VA_WITH_PS
    eNCPAPPhase,
    ePsvInhVABreathPhase //for new VA
#else
    eNCPAPPhase
#endif
};

typedef enum {
    eVentType = (eVentTypeChangeId - eFirstSingleSettingChangeId),
    eMode = (eModeChangeId - eFirstSingleSettingChangeId),
    eTriggerType = (eTriggerTypeChangeId - eFirstSingleSettingChangeId),
    ePressSens = (ePressSensChangeId - eFirstSingleSettingChangeId),
    eFlowSens = (eFlowSensChangeId - eFirstSingleSettingChangeId),
    eBaseFlow = (eBaseFlowChangeId - eFirstSingleSettingChangeId),
    ePctO2 = (ePctO2ChangeId - eFirstSingleSettingChangeId),
    eApneaInt = (eApneaIntChangeId - eFirstSingleSettingChangeId),
    eFlushPctO2 = (eFlushPctO2ChangeID - eFirstSingleSettingChangeId),
    eRr = (eRrChangeId - eFirstSingleSettingChangeId),
    eNIVRr = (eNIVRrChangeId - eFirstSingleSettingChangeId),
    eAvRr = (eAvRrChangeId - eFirstSingleSettingChangeId),
    eFlowPattern = (eFlowPatternChangeId - eFirstSingleSettingChangeId),
    ePf = (ePfChangeId - eFirstSingleSettingChangeId),
    eTv = (eTvChangeId - eFirstSingleSettingChangeId),

    eSupportPress = (eSupportPressChangeId - eFirstSingleSettingChangeId),
    eRiseTime = (eRiseTimeChangeId - eFirstSingleSettingChangeId),
    eExhSens = (eExhSensChangeId - eFirstSingleSettingChangeId),
    eBaselinePress = (eBaselinePressChangeId - eFirstSingleSettingChangeId),
    eInhTime = (eInhTimeChangeId - eFirstSingleSettingChangeId),
    ePcvInhPress = (ePcvInhPressChangeId - eFirstSingleSettingChangeId),

    eAPRVLowInhPress = (eAPRVLowInhPressChangeId - eFirstSingleSettingChangeId),
    eAPRVHighInhPress = (eAPRVHighInhPressChangeId - eFirstSingleSettingChangeId),
    eNippvIpap = (eNippvIpapChangeId - eFirstSingleSettingChangeId),

    eAPRVTimeHigh = (eAPRVTimeHighChangeId - eFirstSingleSettingChangeId),
    eAPRVTimeLow = (eAPRVTimeLowChangeId - eFirstSingleSettingChangeId),

    eHipLimit = (eHipLimitChangeId - eFirstSingleSettingChangeId),
    eLipLimit = (eLipLimitChangeId - eFirstSingleSettingChangeId),
    eLowBaselineLimit = (eLowBaselineLimitChangeId - eFirstSingleSettingChangeId),

    eHrrLimit = (eHrrLimitChangeId - eFirstSingleSettingChangeId),
    eApneaLimit = (eApneaIntChangeId - eFirstSingleSettingChangeId),

    eExtremeHighPEEP_Autoset = (eExtremeHighPEEPAlarm_AutosetChangedId - eFirstSingleSettingChangeId),
    eExtremeHighInsp_Autoset = (eExtremeHighInspAlarm_AutosetChangedId - eFirstSingleSettingChangeId),
    eTidalVolume = (eTidalVolumeChangeID - eFirstSingleSettingChangeId),

    //#ifdef HFO_SYSTEM
    eHFOFrequency = (eHFOFrequencyChangeId - eFirstSingleSettingChangeId),
    eHFOStrokeVolume = (eHFOStrokeVolumeChangeId - eFirstSingleSettingChangeId),
    eHFOMap          = (eHFOMapChangeId - eFirstSingleSettingChangeId),
    eFreshGas = (eFreshGasChangeId - eFirstSingleSettingChangeId),
    eLowMAPLimit = (eLowMAPLimitChangeId - eFirstSingleSettingChangeId),
    eHiMAP_1Limit = (eHiMAP_1LimitChangeId - eFirstSingleSettingChangeId),
    eHiMAP_2Limit = (eHiMAP_2LimitChangeId - eFirstSingleSettingChangeId),
    eExtremeHighMap_Autoset = (eExtremeHighMapAlarm_AutosetChangedId - eFirstSingleSettingChangeId),

    eLowAmplitudeLimit = (eLowAmplitudeLimitChangeId - eFirstSingleSettingChangeId),
    eHighAmplitudeLimit = (eHighAmplitudeLimitChangeId - eFirstSingleSettingChangeId),
    eLowAmplitude_Autoset = (eLowAmplitude_AutosetChangedID - eFirstSingleSettingChangeId),
    eHighAmplitude_Autoset = (eHighAmplitude_AutosetChangedID - eFirstSingleSettingChangeId),

    eSI = (eSIChangeId - eFirstSingleSettingChangeId),

    eAmplitude = (eHFOAmplitudeChangeID - eFirstSingleSettingChangeId),
    eSIRate = (eHFOSIRateChangeID - eFirstSingleSettingChangeId),
    eAmplitudeControl = (eHFOAmpControlChangeID - eFirstSingleSettingChangeId),

    eInspFlow = (eInspFlowChangeID - eFirstSingleSettingChangeId),
    eWaveForm = (eWaveFormChangeID - eFirstSingleSettingChangeId),
    ePlateauTime = (ePlateauTimeChangeID - eFirstSingleSettingChangeId),
    eBackupBR = (eBackupBRChangeID - eFirstSingleSettingChangeId),
    eSyncPlus = (eSyncPlusChangeID - eFirstSingleSettingChangeId),
    eTubeDiameter = (eTubeDiameterChangeID - eFirstSingleSettingChangeId),
    eTubeLength = (eTubeLengthChangeID - eFirstSingleSettingChangeId),
    eCompRate = (eCompRateChangeID - eFirstSingleSettingChangeId),

    eNasalCPAPFlow = (eNasalCPAPFlowChangeID -eFirstSingleSettingChangeId),
    eTiLimit = (eTiLimitChangeId - eFirstSingleSettingChangeId),
    eHighNCPAPLimit = (eHighNCPAPLimitChangeId - eFirstSingleSettingChangeId),
    eLowNCPAPLimit = (eLowNCPAPLimitChangeId - eFirstSingleSettingChangeId),

    eAPRVHighPHighLimit = (eAPRVHighPHighLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVLowPHighLimit = (eAPRVLowPHighLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVHighPLowLimit = (eAPRVHighPLowLimitChangeId - eFirstSingleSettingChangeId),
    eAPRVLowPLowLimit = (eAPRVLowPLowLimitChangeId - eFirstSingleSettingChangeId),
    eHighTidalVolLimit = (eHighTidalVolLimitChangeId - eFirstSingleSettingChangeId),
    eLowTidalVolLimit = (eLowTidalVolLimitChangeId - eFirstSingleSettingChangeId),

    eProximalLeakLimit = (eProximalLeakLimitChangeId - eFirstSingleSettingChangeId),

    eHighTidalVolAutoSet = (eHighTidalVolAutoSetChangeId - eFirstSingleSettingChangeId),
    eLowTidalVolAutoSet = (eLowTidalVolAutoSetChangeId - eFirstSingleSettingChangeId),

    eAPRVExtremeHighPHighAutoSet = (eAPRVExtremeHighPHighAutoSetChangeId - eFirstSingleSettingChangeId),

    //#endif
    // ADD OPERATOR SETTABLE VALUES HERE
    eHighBaselineLimit = (eHighBaselineLimitChangeId - eFirstSingleSettingChangeId),
    eHpsvLimit = (eHpsvLimitChangeId - eFirstSingleSettingChangeId),
    eLpsvLimit = (eLpsvLimitChangeId - eFirstSingleSettingChangeId),

    eLemtvLimit = (eLemtvLimitChangeId - eFirstSingleSettingChangeId),  //
    eLestvLimit = (eLestvLimitChangeId - eFirstSingleSettingChangeId),  //
    eHemvLimit = (eHemvLimitChangeId - eFirstSingleSettingChangeId),
    eLemvLimit = (eLemvLimitChangeId - eFirstSingleSettingChangeId),
    eLetvLimit = (eLetvLimitChangeId - eFirstSingleSettingChangeId),
    ePatientType = (ePatientTypeChangeId - eFirstSingleSettingChangeId),
    eAutoSI = (eHFOAutoSIChangeID - eFirstSingleSettingChangeId),
    eSITime = (eHFOSITimeChangeID - eFirstSingleSettingChangeId),


    // Values retrieved from NVRAM
    eComplFactor = (eLastSingleSettingChangeId - eFirstSingleSettingChangeId + 1),
    eComplOnOff,

    // This dummy setting is never changed by the operator and is used during
    // NIPPV since NIPPV inhalations are not offset from true baseline, they
    // are offset from 0; the dummy Baseline is always 0.
    eDummyBaselinePress,

    // This value must follow all of the settings
    eNumBdSettings

}E_BdSettingId;

#endif /* INC_EPHASEID_H_ */
