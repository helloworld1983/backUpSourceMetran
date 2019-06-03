//$COMMON.H$
//    File Name:  ComponentIDs.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class:  This file contains no class definitions.  It merely provides an
//      an enumeration to provide an ID for all of the graphical controls
//      in the Morph application.
//
//
//    Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    10/24/96  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  COMPONENTIDS_H
#define  COMPONENTIDS_H

enum E_ComponentID
{
    eNoComponent,

//Dialogs
    eVolumeSettingsDlg,
    eNPPVSettingsDlg,
    eVolumeApneaSettingsDlg,
    eNPPVApneaSettingsDlg,
    ePressureApneaSettingsDlg,
    ePressureSettingsDlg,
    ePatientDataDlg,
    eVolumeAlarmsSettingsDlg,
    eNPPVAlarmsSettingsDlg,
    ePressureAlarmsSettingsDlg,
    eSleepDlg,
    eValueSettingDialog,
    eITriggerSettingDialog,
    eVolumeMonitorDialog,
    ePressureMonitorDialog,
    eNPPVMonitorDialog,
    eExpHoldDialog,
	eUnitsSettingDialog,
	    
//Diagnostic dialogs
    eSSTDialog,
    eESTDialog,
    eHardwareDialog,
    eSoftwareDialog,
    eUserConfigDialog,
    eErrorCodesDialog,
    eInformationDialog,
    
// common Dialog buttons
    eActivateButton,
    
//ScreensMenuBar buttons
    eScreensMenu,
    ePatientData,
    eSleep,
    eOption,
    eOption1,
    eOption2,
    eOption3,
    eAboutButton,

//SettingsMenuBar buttons
    eSettingsMenu,
    eVolumeSettings,
    eNonInvasiveSettings,
    ePressureSettings,
    eApneaSettings,
    eAlarmsSettings,
    eOptionSettings,

//Diagnostic UpperMenuBar buttons
    eUpperMenuBar,
    eSSTButton,
    eESTButton,
    eHardwareButton,
    eSoftwareButton,
    eUserConfigButton,

//Diagnostic LowerMenuBar buttons
    eLowerMenuBar,
    eErrorCodesButton,
    eInformationButton,

//other Diagnostic buttons
    eStartSSTButton,
    eStartESTButton,
    eInhalationButton,
    eExhalationButton,
    eBlowerButton,
    eFilterButton,
    e24V_Button,
    eSafetyButton,
    eCrossoverButton,
    eApplyNewDateButton,
    eApplyNewTimeButton,
    eComplianceCompButton,
    eNextButton,
    ePrevButton,
    eClearCodesButton,
    e24hrClockButton,

// diagnostic setting widgets
    eAirValveSettingWidget,         
    eOxygenValveSettingWidget,      
    eExhalationValveSettingWidget,  
    eMonitorsSettingWidget,    
    eVoltageWrapSettingWidget,      
    eBlowerSettingWidget,        
    eMonthSettingWidget,        
    eDaySettingWidget,        
    eYearSettingWidget,        
    eHourSettingWidget,        
    eMinuteSettingWidget,        
    eSecondSettingWidget,        
    eAltitudeSettingWidget,        

//ModeSettingWidgets
    eModeSelector,
    eMixedModeButton,
    eSpontaneousModeButton,
    eAssistControlModeButton,

//SettingWidgets
    eSettingWidgetButton,
    eTidalVolumeSettingWidget,
    eO2SettingWidget,
    eRespRateSettingWidget,
    ePeakFlowSettingWidget,
    ePEEPSettingWidget,
    ePSVSettingWidget,
    eITriggerSettingWidget,
    eETriggerSettingWidget,
    ePlateauSettingWidget,
    eRisetimeSettingWidget,
    
    ePressureSettingWidget,
    eITimeSettingWidget,

    eIPAPSettingWidget,
    eEPAPSettingWidget,
    eApneaRateSettingWidget,
    eApneaIntervalSettingWidget,
    eLowEPAPSettingWidget,
    
    eHighInspPressureSettingWidget,
    eLowInspPressureSettingWidget,
    eLowPeepSettingWidget,
    eLowVteMandatorySettingWidget,
    eLowVteSpontaneousSettingWidget,
    eLowVteSettingWidget,
    eHighRespRateSettingWidget,
    eLowMinuteVolumeSettingWidget,
    eHighMinuteVolumeSettingWidget,
                                        
//ValueSettingDialog buttons
    eCancelButton,
    eValueAdjustSpinControl,                //SpinControl for ValueSettingWidget
    eAcceptButton,
    eValueAdjuster,

//additional widgets for ITriggerSettingDialog
    ePressureTriggerAdjuster,
    eFlowTriggerAdjuster,
    ePressureSelectButton,
    eFlowSelectButton,

//SpinControl buttons
    eUpButton,                  //Up button of a SpinControl
    eDownButton,                //Down button of a SpinControl
//Flow Pattern Setting Widget
    eWaveformSelector,
    eSquareButton,
    eRampButton,
    eSineButton,

//PatientSettingWidget
    ePatientTypeSelector,
    eAdultButton,
    ePediatricButton,

//Miscellaneous
    eOKButton,
	eMarkerButton,

//Diagnostic Buttons
    eRetestButton,
    eContinueButton,

//Buttons for UnitsSettingDialog
	eFeetSelectButton,
	eMetersSelectButton,
	ecmH2OSelectButton,
	ekPascalsSelectButton,
	eMillibarsSelectButton,

//Test Buttons
    eButton1,
    eButton2,
    eButton3,
    eButton4,
    eButton5,
    eButton6,
    eButton7,
    eButton8,
    eButton9
};
#endif
