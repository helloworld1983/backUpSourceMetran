/******************************************************************************/
//$COMMON.H$
//   File Name:  BdSetting.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the class declaration for a BD Setting.  Any
//      setting used by Breath Delivery is instantiated from this class.
/******************************************************************************/

#ifndef BDSETTING_H
#define BDSETTING_H
#include <Trigger.h>
#include "BdInterface.h"
#include "CommonConstants.h"
#include "CommonTypes.h"

/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/

// The E_BdSettingId enumeration is a list of BD settings.  These values 
// are used by MgrBdSetting and any other object that wants to access a
// setting.  
typedef enum {

    eMode = (eModeChangeId - eFirstSingleSettingChangeId),
    eTriggerType = (eTriggerTypeChangeId - eFirstSingleSettingChangeId),
    ePressSens = (ePressSensChangeId - eFirstSingleSettingChangeId),
    eFlowSens = (eFlowSensChangeId - eFirstSingleSettingChangeId),
    eBaseFlow = (eBaseFlowChangeId - eFirstSingleSettingChangeId),
    ePctO2 = (ePctO2ChangeId - eFirstSingleSettingChangeId),
    eFlushPctO2 = (eFlushPctO2ChangeID - eFirstSingleSettingChangeId),
    eRr = (eRrChangeId - eFirstSingleSettingChangeId),
    eNIVRr = (eNIVRrChangeId - eFirstSingleSettingChangeId),
    eSupportPress = (eSupportPressChangeId - eFirstSingleSettingChangeId),
    eRiseTime = (eRiseTimeChangeId - eFirstSingleSettingChangeId),
    eExhSens = (eExhSensChangeId - eFirstSingleSettingChangeId),
    eBaselinePress = (eBaselinePressChangeId - eFirstSingleSettingChangeId),
    eInhTime = (eInhTimeChangeId - eFirstSingleSettingChangeId),
    ePcvInhPress = (ePcvInhPressChangeId - eFirstSingleSettingChangeId),

    eAPRVLowInhPress = (eAPRVLowInhPressChangeId - eFirstSingleSettingChangeId),
    eAPRVHighInhPress = (eAPRVHighInhPressChangeId - eFirstSingleSettingChangeId),

    eAPRVTimeHigh = (eAPRVTimeHighChangeId - eFirstSingleSettingChangeId),
    eAPRVTimeLow = (eAPRVTimeLowChangeId - eFirstSingleSettingChangeId),

    eHipLimit = (eHipLimitChangeId - eFirstSingleSettingChangeId),
    eLipLimit = (eLipLimitChangeId - eFirstSingleSettingChangeId),
    eLowBaselineLimit = (eLowBaselineLimitChangeId - eFirstSingleSettingChangeId),

    eHrrLimit = (eHrrLimitChangeId - eFirstSingleSettingChangeId),
    eApneaLimit = (eApneaIntChangeId - eFirstSingleSettingChangeId),

    eExtremeHighPEEP_Autoset = (eExtremeHighPEEPAlarm_AutosetChangedId - eFirstSingleSettingChangeId),
    eExtremeHighInsp_Autoset = (eExtremeHighInspAlarm_AutosetChangedId - eFirstSingleSettingChangeId),

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
    eSITime = (eHFOSITimeChangeID - eFirstSingleSettingChangeId),
    eSIRate = (eHFOSIRateChangeID - eFirstSingleSettingChangeId),
    eAmplitudeControl = (eHFOAmpControlChangeID - eFirstSingleSettingChangeId),
    eAutoSI = (eHFOAutoSIChangeID - eFirstSingleSettingChangeId),

    eTidalVolume = (eTidalVolumeChangeID - eFirstSingleSettingChangeId),
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

// All settings are protected values.  Two versions of the setting are stored;
// they are used to verify that data corruption has not occured.
struct ProtectedValue
{
    LONG value;
    LONG backup;
};

class BdSetting
{
public:
    //Constructor
    BdSetting (E_BdSettingId, Trigger* triggerPtr = NULL,
               E_Bool phaseInImmediately = eFalse);
    BdSetting(E_ConfigIndex nvramIx);

    //Get current value
    LONG GetCurrentValue (void);

    //Get pending value
    LONG GetPendingValue (void);

    //Set pending value
    void SetPendingValue (LONG value);

    //Phase in setting
    virtual void PhaseInSetting (void);

    //Get Bd Setting ID
    E_BdSettingId GetID(void);

protected:
    //Destructor
    inline virtual ~BdSetting(){;}

    //$COMMON.ATTRIBUTE$
    //  Name: CurrentValue
    //  Description:  Value of the setting scaled by SCALE currently
    //      being used by BD
    //  Units: Depends on individual setting
    //  Range: Depends on individual setting
    ProtectedValue CurrentValue;

    //$COMMON.ATTRIBUTE$
    //  Name: PendingValue
    //  Description:  Value of the setting scaled by SCALE that is
    //      waiting to be phased in by BD
    //  Units: Depends on individual setting
    //  Range: Depends on individual setting
    ProtectedValue PendingValue;

private:
    BdSetting (const BdSetting&);

    //$COMMON.ATTRIBUTE$
    //  Name: TriggerPtr
    //  Description:  Pointer to the trigger that is affected by a change
    //      of this setting value
    //  Units: None
    //  Range: n/a
    Trigger* TriggerPtr;

    //$COMMON.ATTRIBUTE$
    //  Name: PhaseInImmediately
    //  Description:  Boolean that indicates if a trigger associated with
    //      this setting should be notified immediately when the setting
    //      value is changed.  This value is only used if TriggerPtr != NULL
    //  Units: None
    //  Range: True, False
    E_Bool PhaseInImmediately;

    //$COMMON.ATTRIBUTE$
    //  Name: Id
    //  Description:  Bd setting ID
    //  Units: None
    //  Range: None
    E_BdSettingId Id;

};

#endif  // BDSETTING_H
