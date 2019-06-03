//$COMMON.H$
//    File Name:  StringTable.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class:   This file contains no class definitions.  It is used to
//       keep a list of all of the text strings that are displayed on the
//       display screen. 
/******************************************************************************/
#ifndef STRINGTABLE_H
#define  STRINGTABLE_H

#include "stddef.h"

#ifndef STRING
#include <string>
#include <iostream>
#define STRING std::string
#endif


enum E_Language
{
	eNoLanguage,
    eEnglish,
    eFrench,
    eSpanish,
    eGerman,
    eItalian,
    eRussian,
    eJapanese,
    eChinese,
    eArabic,
    ePortuguese
} ;

enum E_Typeface
{
    eSwiss12Light   , 
    eSwiss15Light   , 
    eSwiss15Bold    ,
    eSwiss21Roman   ,
    eSwiss29Roman   ,
    eSwiss38Bold
};

enum E_StringID 
{
   eNoString,
   // warning messages
   eUpperLimitExceeded,
   eLowerLimitExceeded,
   eUsingDefaults,
   eExpHoldRejectMessage,
   eAltitudeMessage,
   eComplianceMessage,
   
   // constraint violations
   eLowAlarmLimitExceeded,
   eHighAlarmLimitExceeded,
   eBelowEPAP,
   eIPAPExceeded,
   eBelowRiseTime,
   eITimeExceeded,
   eVCVIERatioViolatedApnea,
   eVCVIERatioViolatedFlow,
   eVCVIERatioViolatedVolume, 
   eVCVIERatioViolatedRate,
   eVCVIERatioViolatedPlateau,
   eVCVIERatioViolatedPattern,
   eVCVIERatioViolated,
   ePCVIERatioViolatedApnea,
   ePCVIERatioViolatedITime,
   eNPPVIERatioViolatedApnea,
   eNPPVIERatioViolatedITime,
   eIERatioViolated,
   eVCVMaxITimeViolated,
   eAboveApnea,
   eBelowRespRate,
   
   //confirmation messages
   eSpontaneousModeChange,
   eMixedModeChange,
   eAssistModeChange,
   eSpontModeChange,
   eSpontTimedModeChange,
   eRampChange,
   eSquareChange,
   eVolumeChange,
   ePressureChange,
   eNonInvasiveChange,
   eAdultChange,
   ePediatricChange,

   //
   eUnknownModel,

   //values returned by Model::GetValueString
   eRampPattern,
   eSquarePattern,
   eSinePattern,
   eFlowTriggerText,
   ePressureTriggerText,
   eMixedModeText,
   eSpontaneousModeText,
   eAssistControlText,
   ePatientDataList,
   eAdultPatientTypeText,
   ePediatricPatientTypeText,

   // Setting Widget Labels
   eSWRate,
   eSWTidalVolume,
   eSWPeakFlow,
   eSWPEEP,
   eSWPSV,
   eSWITrigger,
   eSWETrigger,
   eSWO2,
   eSWPlateau,
   eSWRisetime,
   eSWITime,
   eSWHighInspPressure,
   eSWLowInspPressure,
   eSWLowPEEP,
   eSWLowEPAP,
   eSWLowVte,
   eSWLowVteMand,
   eSWLowVteSpont,
   eSWHighRespRate,
   eSWLowMinuteVolume,
   eSWHighMinuteVolume,
   eSWApneaRate,
   eSWApneaInterval,
   eSWPressure,
   eSWIPAP,
   eSWEPAP,

   //Units for Setting Widgets
   eBPM,
   emL,
   eL,
   eLPM,
   ecmH2O,
   ePerCent,
   eSec,
   eFeet,
   eMeters,
   eSTR_kpa,
   eSTR_mb,

    //Strings for UnitsSelector	button labels
	eSTR_Feet,
	eSTR_Meters,
	eSTR_KiloPascals,
	eSTR_Millibars,
	eSTR_cmH2O,

   //Breath indicator labels
   eSpontBI,
   eMandBI,
   eAssistBI,
   ePlateauBI,
   eSupportBI,
   eExhalationBI,
   
   //Ventilation mode labels
   eVCVSIMV,
   eVCVAssist,
   eVCVCPAP,
   ePCVSIMV,
   ePCVAssist,
   ePCVCPAP,
   eNPPVSpontTimed,
   eNPPVAssist,
   eNPPVSpont,

   //Menu bar labels
   eMBVolume,
   eMBNonInvasive,
   eMBPressure,
   eMBApnea,
   eMBSettings,
   eMBAlarms,
   eMBPatientData,
   eMBOptions,
   eMBSleep,

   //Diagnostic Menu bar labels
   eMBSST,
   eMBEST,
   eMBHardwareDiag,
   eMBSoftwareDiag,
   eMBUserConfig,
   eMBErrorCodes,
   eMBInformation,

   // Diagnostic mode message labels
   eWarning,
   eWarningMessage1,
   eWarningMessage2,
   eCompliance,             
   eBlockPatientWyeMsg,
   eUnblockPatientWyeMsg,
   eConnectO2Msg,         
   eDisconnectO2Msg,         
   eO2SensorBadMsg,
   eO2SensorMissingMsg,
   eDisconnectCircuitFromFilterMsg,
   ePatientIsConnectedMsg,
   ePrimaryAudioMsg,
   eBackupAudioMsg,
   eRemoteAlarmMsg,
   ePressManInspKeyMsg,
   ePressAllO2KeyMsg,
   ePressAlarmResetKeyMsg,
   ePressAlarmSilenceKeyMsg,
   ePressScreenLockKeyMsg,
   ePressRotaryKnobAcceptKeyMsg,
   ePressOptionLeftKeyMsg,
   ePressOptionRightKeyMsg,
   ePressExpHoldKeyMsg,
   eTurnRotaryKnobMsg,
   eNormalLEDMsg,
   eAlarmHighLEDMsg,
   eAlarmMedLEDMsg,
   eVentInopLEDMsg,
   eSafetyValveLEDMsg,
   eAlarmSilenceLEDMsg,
   eAllO2LEDMsg,
   eScreenLockLEDMsg,
   eIntBattInUseLEDMsg,
   eIntBattChargingLEDMsg,
   eIntBattLowLEDMsg,
   eSSTPassedMsg,
   eESTPassedMsg,
   eSSTFailedMsg,
   eESTFailedMsg,
   eStepModeMsg,
   eInvalidDateMsg,
   eStartupMsg,

   // Diagnostic mode test results labels
   ePassed,
   eFailed,
   eTesting,
   eCanceling,
   eBlockPatientWyeTest,
   ePatientCircuitTest,
   eOxygenSupplyTest,
   eOxygenDeliveryTest,
   eOxygenSensorTest,
   eAirDeliveryTest,
   eCrossoverCircuitTest,
   eBlowerTest,
   eHeatedFilterTest,
   eSafetyValveTest,
   ePressureReliefValveTest,
   eExhalationValveTest,
   eAudioTest,
   eKeyboardTest,
   eLEDTest,
   eFailureCode,
   eMeasured,
   eTolerance,
   eTestResults,            
   eFailureData,            

   // diagnostic button labels
   eStartSSTLabel,
   eStartESTLabel,
   eAirLabel,
   eOxygenLabel,
   eExhalationLabel,
   eInhalationLabel,
   eMonitorsLabel,
   eVoltageWrapLabel,
   eBlowerLabel,
   eFilterLabel,
   e24V_Label,
   eSafetyLabel,
   eCrossoverLabel,
   eMonthLabel,
   eDayLabel,
   eYearLabel,
   eHourLabel,
   eMinuteLabel,
   eSecondLabel,
   eApplyNewDateLabel,
   eApplyNewTimeLabel,
   eComplianceCompEnbLabel,       
   eNextLabel,
   ePrevLabel,
   eClearCodesLabel,
   eAltitudeLabel,
   e24hrClockLabel,

   // diagnostic sensors labels
   eAirFlowSensorLabel,
   eOxygenFlowSensorLabel,
   eExhalationFlowSensorLabel,
   eAirPositionLabel,
   eOxygenPositionLabel,
   eExhalationPositionLabel,
   eInhalationPressureLabel,
   eExhalationPressureLabel,
   eOxygenSupplyLabel,
   eOxygenSensorLabel,
   eBatteryVoltageLabel,
   eBatteryCurrentLabel,
   eVoltageReferenceLabel,
   eEnclosureTempLabel,
   eInternalOxygenLabel,
   eADC_WrapLabel,

   // diagnostic hardware screen labels
   eEventNumberLabel,
   eEventCodeLabel,
   eDateLabel,
   eTimeLabel,
   eRepeatLabel,
   eCorruptedLabel,
   eEmptyLabel,

   // diagnostic software screen labels
   eSerialNumberLabel,
   eFlashPartNumberLabel,
   eFlashVersionLabel,
   eOTPPartNumberLabel,
   eOTPVersionLabel,
   eAirStepperVersionLabel,
   eO2StepperVersionLabel,
   eExhStepperVersionLabel,
   eAirFlowSensPartNumLabel,
   eO2FlowSensPartNumLabel,
   eExhFlowSensPartNumLabel,
   eDisplayVersionLabel,
   
   // diagnostic units labels
   eSteps,
   eStepsSml,
   eV,
   eV_Sml,
   emV,
   euV,
   eA,
   eA_Sml,
   eA2DCnts,
   eDACCnts,
   emlcmH2O,
   eDegrees,
   
   // diagnostic test failure labels
   eTheLbl,
   eFailedLbl,
   
   // clock labels
   eAM,
   ePM,
   
   //ModeSettingWidget Labels
   eModeAssist,
   eModeSIMV,
   eModeCPAP,
   eModeSpont,
   eModeSpontTimed,

   eGraph_Pressure,
   eGraph_Volume,

   //Marker Labels
   eGraph_HIP,
   eGraph_PEEP,
   eGraph_VTm,
   eGraph_VTs,

   //patient data list
   eDLIPIP,
   eDLIMAP,
   eDLIVt,
   eDLIRR,
   eDLISpontRR,
   eDLIVE,
   eDLISpontVE,
   eDLIFVt, 
   
   //ValueSettingDialog
   eVSDIncrease,
   eVSDDecrease,

   //ExhHoldDialog
   eEndExhalationPressure,
   eExhalationPausePressure,
   eAutoPEEP,
   eManeuverStatus,
   eExhHoldTitle,
   eExpHoldPending,
   eExpHoldWaiting,
   eExpHoldCompleted,
   eExpHoldOperatorAbort,
   eExpHoldAlarmAbort,
   eExpHoldTimeoutAbort,
   eExpHoldAbort,
   eExpHoldNotStabilized,
   
   //SpecialDataValues
   eBlankedValue,
   
   //ITrigger Selector buttons,
   eITrigPressure,
   eITrigFlow,
   eITrigDialogTitle,
   
   //Ventilation type indicators
   ePressureControl,
   eVolumeControl,
   eNonInvasiveControl,
   eVentViewTitle,

   // Activate button
   eActivate,
   eVentTypeActive,

   // Labels for DataView groups
   eDVPressure,
   eDVVolume,
   eDVRate,
   eDVSettings,
   eDVPatientData,

   // Miscellaneous   buttons
   eCancel,
   eAccept,
   eOK,
   eYes,
   eNo,
   eContinue,
   eRetest,

   eSTR_ModeSettingAbbrevName,
   eSTR_ModeSettingFullName,
   //Alarm messages
   eSTR_Alarm_Occlusion,              
   eSTR_Alarm_GasSuppliesLost,        
   eSTR_Alarm_LowInternalBattery,     
   eSTR_Alarm_CorruptAltitude,
   eSTR_Alarm_CorruptCompliance,
   eSTR_Alarm_UsingDefaultSettings,   
   eSTR_Alarm_PrimaryAlarmFailure,    
   eSTR_Alarm_HighInspPressure,       
   eSTR_Alarm_Apnea,                  
   eSTR_Alarm_LowInspPressure,        
   eSTR_Alarm_AirSourceFault,         
   eSTR_Alarm_O2ValveStuckClosed,     
   eSTR_Alarm_ExhValveStuckOpen,      
   eSTR_Alarm_LowO2Supply,            
   eSTR_Alarm_LowO2,                  
   eSTR_Alarm_LowMinuteVolume,        
   eSTR_Alarm_LowVtMandatory,         
   eSTR_Alarm_HighMinuteVolume,       
   eSTR_Alarm_LowVt,                  
   eSTR_Alarm_LowVtSpontaneous,       
   eSTR_Alarm_ITimeTooLong,           
   eSTR_Alarm_HighRespiratoryRate,    
   eSTR_Alarm_HighO2,                 
   eSTR_Alarm_HighEnclosureTemp,      
   eSTR_Alarm_HighInternalO2,         
   eSTR_Alarm_LowPeep,                
   eSTR_Alarm_LowEpap,
   eSTR_Alarm_BadBatteryCurrentSensor,
   eSTR_Alarm_BadBatteryVoltageSensor,
   eSTR_Alarm_BadInternalTempSensor,
   eSTR_Alarm_BadInternalO2Sensor,
   eSTR_Alarm_BadADCWrapAISensor,
   eSTR_Alarm_LowExternalBattery,     
   eSTR_Alarm_IntBatteryCharging,     
   eSTR_Alarm_InternalBatteryO,       
   eSTR_Alarm_ServiceDue,             
   eSTR_Alarm_NoEventID, 
   
   // names for models
   eSTR_MName_ApInt,          
   eSTR_MName_ApneaInterval,  
   eSTR_MName_ApneaRate,      
   eSTR_MName_ApneaRR,        
   eSTR_MName_EndExp,         
   eSTR_MName_EPAP,           
   eSTR_MName_ETrigger,       
   eSTR_MName_FlowTrigger,    
   eSTR_MName_FVt,            
   eSTR_MName_HighPressure,   
   eSTR_MName_HighRate,       
   eSTR_MName_HighVE,         
   eSTR_MName_HIP,            
   eSTR_MName_HRR,            
   eSTR_MName_HVE,            
   eSTR_MName_IERatio,        
   eSTR_MName_IPAP,           
   eSTR_MName_ITime,          
   eSTR_MName_ITrigger,       
   eSTR_MName_LIP,            
   eSTR_MName_LMSTV,          
   eSTR_MName_LMTV,           
   eSTR_MName_LOP,            
   eSTR_MName_LowPEEP,        
   eSTR_MName_LowPressure,    
   eSTR_MName_LowVE,          
   eSTR_MName_LowVtMand,      
   eSTR_MName_LowVtSpont,     
   eSTR_MName_LVE,            
   eSTR_MName_MAP,            
   eSTR_MName_O2,             
   eSTR_MName_PeakFlow,       
   eSTR_MName_PEEP,           
   eSTR_MName_PIP,            
   eSTR_MName_Plateau,        
   eSTR_MName_Pressure,       
   eSTR_MName_PressureTrigger,
   eSTR_MName_PSV,            
   eSTR_MName_PSVRise,        
   eSTR_MName_PSVRisetime,    
   eSTR_MName_Rate,           
   eSTR_MName_RiseTime,       
   eSTR_MName_RR,             
   eSTR_MName_SpontRate,      
   eSTR_MName_SpontRR,        
   eSTR_MName_SpontVE,        
   eSTR_MName_TidalVol,       
   eSTR_MName_TidalVolume,    
   eSTR_MName_TotalVE,        
   eSTR_MName_VE,             
   eSTR_MName_Vol,            
   eSTR_MName_Volume,         
   eSTR_MName_Vt,  
   
   eSTR_ActivationButton_VolumeActive,
   eSTR_ActivationButton_PressureActive,
   eSTR_ActivationButton_NonInvasiveActive,
   eSTR_ActivationButton_VolumeInactive,
   eSTR_ActivationButton_PressureInactive,
   eSTR_ActivationButton_NonInvasiveInactive,
              

   eSTR_VentInopDlg_Title,
   eSTR_VentInopDlg_Error,
   
   eAlarmsListTitle,
   eTestFont,
   eTestFont1,
   eTestFont2,
   eEmptyString,
   eNoError = eNoString
};

struct InitTextDesc 
{
    E_StringID ID;
    E_Typeface TypefaceName;
    STRING String;
};

struct TextDesc
{
    char * TypefaceName;
    STRING String;
};


// Available Typefaces
extern char * HighUrgencyAlarmTypeface;
extern char * LowUrgencyAlarmTypeface;
extern char * BarGraphTypeface ;
extern char * MarkerTypeface;
extern char * DataListUnitsTypeface;
extern char * DataListNameTypeface;
extern char * DataListDataTypeface;
extern char * ValueBoxTypeface;
extern char * ButtonLabelTypeface;
extern char * ValueBarTypeface;
extern char * ValueSettingsTitleTypeface;
extern char * SettingWidgetUnitsTypeface;
extern char * DataViewTitleTypeface;
extern char * TimeStampTypeface;
extern char * LargeDataViewTypeface;

InitTextDesc * GetStringTable(E_Language language = eEnglish);
extern char * TypefaceTable[];
E_Bool StringCompare(STRING a, STRING b, LONG maxElems);
void   StringCopy(STRING from, STRING to, LONG maxElems);
void   StringCopy(char * from, wchar_t * to, LONG maxElems);
#endif

