/********************************************************************/
/*                                                                  */
/* File Name    : TextLibrary.c                          			*/
/*                                                                  */
/* General      : StringTable used to keep a list of all the text   */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/


#include "TextLibrary.h"

//String table
const char* g_stringTable[] = {
		"",//eNoStringID,
		"Temperature(Unit):",//eTemperatureUnitDataStringID,
		"Measurement flow rate:",//eMeasurementFRDataStringID,
		"Oxygen concentration:",//eO2PercentDataStringID,
		"Oxygen concentration:",//eO2PercentGraphDataStringID,
		"Temperature(Comp):",//eTemperatureCompOrOutletDataStringID,
		"Product tank pressure:",//ePressure1DataStringID,                                    (Product tank pressure)
		"Cannula pressure:",//ePressure2DataStringID,
		"Illuminance sensor voltage:",//eIlluminanceSensorVoltageStringID
		"Pressure sensor1:",//eCompPositivePressForceStringID,
		"Pressure sensor2:",//eCompNegativePressForceStringID,
		"Ext flow pressure:",//eChanelSwitchingPressStringID,
		"Product tank pressure:",//eProductTankMonitorPSAStringId,
		"Product tank pressure:",//eProductTankMonitorGraphStringId
		"Valve 1 turn:",//eValve1DataStringId,
		"Valve 2 turn:",//eValve2DataStringId,
		"Valve 3 turn:",//eValve3DataStringId,
		"Valve 4 turn:",//eValve4DataStringId,
		"Valve 5 turn:",//eValve5DataStringId
		"Compressor RPM:",//eCompressorRPMDataStringId,
		"Cooling FAN duty:",//eCoolingFANDataStringId,
		"PSA cycle time:",//ePSACycleTimeStringId,
		"PSA parameter:",//ePSAParameterDataStringId,
		"PSA parameter:",//ePSAParameterDataGraphStringId
		"Setting fow rate in patient mode",//eFlowRateSettingStringID,
		"Setting flow rate",//eFlowRatePSAStringID,
		"Setting RPM",//eMotorRPMFunctionTestStringID,
		"PSA pressure",//ePSAPressureStringID,
		"Solenoid 1 on offset time",//eSolenoid1OnOffSetTimeStringID,
		"Solenoid 1 off offset time",//eSolenoid1OffOffSetTimeStringID,
		"Solenoid 2 on offset time",//eSolenoid2OnOffSetTimeStringID,
		"Solenoid 2 off offset time",//eSolenoid2OffOffSetTimeStringID,
		"Solenoid 3 on offset time",//eSolenoid3OnOffSetTimeStringID,
		"Solenoid 3 off offset time",//eSolenoid3OffOffSetTimeStringID,
		"Solenoid 4 on offset time",//eSolenoid4OnOffSetTimeStringID,
		"Solenoid 4 off offset time",//eSolenoid4OffOffSetTimeStringID,
		"Solenoid 5 on offset time A",//eSolenoid5OnOffSetTimeAStringID,
		"Solenoid 5 on offset time B",//eSolenoid5OnOffSetTimeStringID,
		"Solenoid 5 open time A",//eSolenoid5OpenTimeAStringId,
		"Solenoid 5 open time B",//eSolenoid5OpenTimeBStringId,
		"Compressor RPM",//eCompressorRPMPSAStringId,
		"FC output",//eFCOutputPSAStringId,
		"Cooling fan duty",//eCoolingFanDutyPSAStringId,
		"Max PSA time",//eMaxPSATimeStringId,
		"Solenoid open time on End",//eSolenoidOpenTimeOnEndStringId,
		//Setting equipment
		"Guide volume",//eGuidanceVolumeStringId,
		"Alarm volume",//eAlarmVolumeStringId,
		"Buzzer volume",//eBuzzerVolumeStringId,
		"Resting flow rare",//eRestingFRStringId,
		"Exertion flow rare",//eExertionFRStringId,
		"Bedtime flow rare",//eBedtimeFRStringId,
		"LCD brightness",//eLCDBrightnessStringId,
		//End Setting equipment
		"Setting duty",//eCoolingFanFunctiontestStringID,
		"",//"O2 Concentration test",//eO2ConcentrationTestStringID,
		"",//"Flow rate test",//eFlowRateTestStringID,
		"Replaced condition high temp",//eHighTemperatureStringID,
		"Replaced condition low temp",//eLowTemperatureStringID,
		"Replaced condition O2",//eLimitOxygenConcentratorStringID,
		"Valve1",//eTurnValve1StringID,
		"Valve2",//eTurnValve2StringID,
		"Valve3",//eTurnValve3StringID,
		"Valve4",//eTurnValve4StringID,
		"Valve5",//eTurnValve5StringID,
		"Latching valve",//eTurnLatchingValveStringID,
		"Oxygen cylinder installation",//eTurnOxygenCylinderStringID,
		"Motor",//eTurnMotorCoolingFANStringID,
		"Motor",//eTurnMotorFilterCleaningStringID,
		"Motor",//eTurnMotorCompressorStringID,
		"Buzzer",//eTurnBuzzerStringID,
		"Alarm LED(red)",//eTurnAlarmRedLEDStringID,
		"Alarm LED(yellow)",//eTurnAlarmYellowLEDStringID,
		"LED power anomalies",//eTurnLEDPowerAnomaliesStringID,
		"Voice guidance",//eTurnVoiceGuidanceStringID,
		"Latching valve",//eTurnBackupSystemStringID,
		"Oxygen cylinder installation",//eTurnOxygenCylinderValueStringID,
		"Child Lock",//eChildlockStringID,
		"Bluetooth search",//eTurnBluetoothStringID,
		"Bluetooth transfer",//eTurnBluetoothTranferStringID
		"",//"RTC test",//eTurnRTCStringID,
		"",//eOnBtnValve1StringId,
		"",//eOffBtnValve1StringId,
		"",//eOnBtnValve2StringId,
		"",//eOffBtnValve2StringId,
		"",//eOnBtnValve3StringId,
		"",//eOffBtnValve3StringId,
		"",//eOnBtnValve4StringId,
		"",//eOffBtnValv42StringId,
		"",//eOnBtnValve5StringId,
		"",//eOffBtnValve5StringId,
		"",//eOnBtnMotorCoolingStringId,
		"",//eOffBtnMotorCoolingStringId,
		"",//eOnBtnMotorFilterStringId,
		"",//eOffBtnMotorFilterStringId,
		"",//eOnBtnMotorCompressorStringId,
		"",//eOffBtnMotorCompressorStringId,
		"",//eOnBtnBuzzerStringId,
		"",//eOffBtnBuzzerStringId,
		"",//eOnBtnAlarmRedLEDStringId,
		"",//eOffBtnAlarmRedLEDStringId,
		"",//eOnBtnAlarmYellowLEDStringId,
		"",//eOffBtnAlarmYellowLEDStringId,
		"",//eOnBtnPowerLEDStringId,
		"",//eOffBtnPowerLEDStringId,
		"",//eOnBtnVoiceStringId,
		"",//eOffBtnVoiceStringId,
		"",//eOnBtnChildLockStringId,
		"",//eOffBtnChildLockStringId,
		"",//eOnBtnBluetoothStringId,
		"",//eOffBtnBluetoothStringId,
		"",//eOnBtnBluetoothTransferStringId,
		"",//eOffBtnBluetoothTransferStringId,
		"",//eOnBtnRTCStringId,
		"",//eOffBtnRTCStringId,
		"",//eUpBtnCoolingStringId,
		"",//eDownBtnCoolingStringId,
		"",//eUpBtnO2ConcentrationTestStringId,
		"",//eDownBtnO2ConcentrationTestStringId,
		"",//eUpBtnFlowRateTestStringId,
		"",//eDownBtnFlowRateTestStringId,
		"",//eUpBtnCompressorStringId,
		"",//eDownBtnCompressorStringId,
		"",//eUpBtnHighTempStringId,
		"",//eDownBtnHighTempStringId,
		"",//eUpBtnLowTempStringId,
		"",//eDownBtnLowTempStringId,
		"",//eUpBtnO2LimitStringId,
		"",//eDownBtnO2LimitStringId,
		"",//eUpBtnFRPSAStringId,
		"",//eDownBtnFRPSAStringId,
		"",//eUpBtnPSAPressureStringId,
		"",//eDownBtnPSAPressureStringId,
		"",//eUpBtnSolenoid1OnOffSetTimeStringId,
		"",//eDownBtnSolenoid1OnOffSetTimeStringId,
		"",//eUpBtnSolenoid1OffOffSetTimeStringId,
		"",//eDownBtnSolenoid1OffOffSetTimeStringId,
		"",//eUpBtnSolenoid2OnOffSetTimeStringId,
		"",//eDownBtnSolenoid2OnOffSetTimeStringId,
		"",//eUpBtnSolenoid2OffOffSetTimeStringId,
		"",//eDownBtnSolenoid2OffOffSetTimeStringId,
		"",//eUpBtnSolenoid3OnOffSetTimeStringId,
		"",//eDownBtnSolenoid3OnOffSetTimeStringId,
		"",//eUpBtnSolenoid3OffOffSetTimeStringId,
		"",//eDownBtnSolenoid3OffOffSetTimeStringId,
		"",//eUpBtnSolenoid4OnOffSetTimeStringId,
		"",//eDownBtnSolenoid4OnOffSetTimeStringId,
		"",//eUpBtnSolenoid4OffOffSetTimeStringId,
		"",//eDownBtnSolenoid4OffOffSetTimeStringId,
		"",//eUpBtnSolenoid5OnOffSetTimeAStringId,
		"",//eDownBtnSolenoid5OnOffSetTimeAStringId,
		"",//eUpBtnSolenoid5OnOffSetTimeBStringId,
		"",//eDownBtnSolenoid5OnOffSetTimeBStringId,
		"",//eUpBtnSolenoid5OpenTimeAStringId,
		"",//eDownBtnSolenoid5OpenTimeAStringId,
		"",//eUpBtnSolenoid5OpenTimeBStringId,
		"",//eDownBtnSolenoid5OpenTimeBStringId,
		"",//eUpBtnCompressorRPMPSAStringId,
		"",//eDownBtnCompressorRPMPSAStringId,
		"",//eUpBtnFCOutputPSAStringId,
		"",//eDownBtnFCOutputPSAStringId,
		"",//eUpBtnCoolingFanDutyPSAStringId,
		"",//eDownBtnCoolingFanDutyPSAStringId,
		"",//eUpBtnMaxPSATimeStringId,
		"",//eDownBtnMaxPSATimeStringId,
		"",//eUpBtnSolenoidOpenTimeOnEndStringId,
		"",//eDownBtnSolenoidOpenTimeOnEndStringId,
		"",//eUpBtnVoiceGuidanceVolumeStringId,
		"",//eDownBtnVoiceGuidanceVolumeStringId,
		"",//eUpBtnAlarmVolumeStringId,
		"",//eDownBtnAlarmVolumeStringId,
		"",//eUpBtnBuzzerVolumeStringId,
		"",//eDownBtnBuzzerVolumeStringId,
		"",//eUpBtnRestingFRStringId,
		"",//eDownBtnRestingFRStringId,
		"",//eUpBtnExertionFRStringId,
		"",//eDownBtnExertionFRStringId,
		"",//eUpBtnBedtimeFRStringId,
		"",//eDownBtnBedtimeFRStringId,
		"",//eUpBtnLCDBrightnessStringId,
		"",//eDownBtnLCDBrightnessStringId,
		"",//eOpenBtnLatchingStringId,
		"",//eCloseBtnLatchingStringId,
		"",//eExistBtnStringId,
		"",//eNoExistBtnStringId,
		"",//eValueOfSettingDutyStringId,
		"",//eValueOfO2ConcentrationTestStringId,
		"",//eValueOfFlowRateTestStringId
		"",//eValueOfSettingRPMStringId,
		"",//eValueOfValve1StringId,
		"",//eValueOfValve2StringId,
		"",//eValueOfValve3StringId,
		"",//eValueOfValve4StringId,
		"",//eValueOfValve5StringId,
		"",//eValueOfMotorCoolingStringId,
		"",//eValueOfMotorFilterStringId,
		"",//eValueOfMotorCompressorStringId,
		"",//eValueOfBuzzerStringId,
		"",//eValueOfRedLedStringId,
		"",//eValueOfYellowLedStringId,
		"",//eValueOfPowerLedStringId,
		"",//eValueOfVoiceStringId,
		"",//eValueOfLatchingValveStringId,
		"",//eValueOfHighTempStringId,
		"",//eValueOfLowTempStringId,
		"",//eValueOfO2LimitTempStringId,
		"",//eValueOfFRPSAStringId,
		"",//eValueOfPSAPressureStringId,
		"",//eValueOfSolenoid1OnOffSetTimeStringId,
		"",//eValueOfSolenoid1OffOffSetTimeStringId,
		"",//eValueOfSolenoid2OnOffSetTimeStringId,
		"",//eValueOfSolenoid2OffOffSetTimeStringId,
		"",//eValueOfSolenoid3OnOffSetTimeStringId,
		"",//eValueOfSolenoid3OffOffSetTimeStringId,
		"",//eValueOfSolenoid4OnOffSetTimeStringId,
		"",//eValueOfSolenoid4OffOffSetTimeStringId,
		"",//eValueOfSolenoid5OnOffSetTimeAStringId,
		"",//eValueOfSolenoid5OffOffSetTimeStringId,
		"",//eValueOfSolenoid5OpenTimeAStringId,
		"",//eValueOfSolenoid5OpenTimeBStringId,
		"",//eValueOfCompressorRPMPSAStringId,
		"",//eValueOfFCOutputPSAStringId,
		"",//eValueOfCoolingFanDutyPSAStringId,
		"",//eValueOfMaxPSATimeStringId,
		"",//eValueOfSolenoidOpenTimeOnEndStringId,
		"",//eValueOfGuidanceVolumeStringId,
		"",//eValueOfAlarmVolumeStringId,
		"",//eValueOfBuzzerVolumeStringId,
		"",//eValueOfRestingFRStringId,
		"",//eValueOfExertionFRStringId,
		"",//eValueOfBedtimeFRStringId,
		"",//eValueOfLCDBrightnessStringId,
		"",//eValueOfChildLockStringId,
		"",//eValueOfBluetoothStringId,
		"",//eValueOfBluetoothTransferStringId,
		"",//eValueOfRTCStringId,
		"LPM",//eUnitLPMStringId,
		"L/min",//eUnitLPMinStringId
		"V",//eUnitVoltageStringId,
		"sec",//eUnitSecStringId,
		"kPa",//eUnitkPaStringId,
		"%",//eUnitPercentStringId,
		"RPM",//eUnitRPMStringId,
		"℃",//eUnitTemperatureStringId,
		"℃Excess",//eUnitHighTemperatureStringId,
		"℃Insufficient",//eUnitLowTemperatureStringId,
		"%Insufficient",//eUnitLimitO2StringId,
		"",//eNexBtnPageTwoStringId,
		"",//eNexBtnPageThreeStringId,
		"",//ePrevBtnPageThreeStringId,
		"",//ePrevBtnPageFourStringId,
		"",//eBackBtnFunctionTestPageOneStringId,
		"",//eBackBtnFunctionTestPageTwoStringId,
		"",//eBackBtnFunctionTestPageThreeStringId,
		"",//eBackBtnFunctionTestPageFourStringId,
		"",//eHomeBtnFunctionTestPageOneStringId,
		"",//eHomeBtnFunctionTestPageTwoStringId,
		"",//eHomeBtnFunctionTestPageThreeStringId,
		"",//eHomeBtnFunctionTestPageFourStringId,
		"",//ePSAParameterBtnStringId,
		"",//eBackBtnPSAParameterStringId,
		"",//eHomeBtnPSAParameterStringId,
		"",//eHomeBtnServiceModeStringId,
		"",//eHomeBtnMaintenanceModeModeStringId,
		"",//eBackBtnMonitorPSAStringId,
		"",//eHomeBtnMonitorPSAStringId,
		"",//ePatientModeButtonTestStringId,
		"",//eStandbyModeButtonTestStringId,
		"",//eLogDataBtnStringId,
		"",//eZeroBtnStringId,
		"",//eOneBtnStringId,
		"",//eTwoBtnStringId,
		"",//eThreeBtnStringId,
		"",//eFourBtnStringId,
		"",//eFineBtnStringId,
		"",//eSixBtnStringId,
		"",//eSevenBtnStringId,
		"",//eEightBtnStringId,
		"",//eNineBtnStringId,
		"",//eEnterPassBtnStringId,
		"",//eClearPassBtnStringId,
		"",//eClosePopUpPassStringId,
		"",//eSerialNumberStringId,
		"",//eSoftwareVersionStringId,
		"",//eCalendarStringId,
		"",//ePatientCumulativeHourStringId,
		"",//eUintCumulativeHourStringId,
		"",//eBackBtnEquipmentInforStringId,
		"",//eHomeBtnEquipmentInforStringId,
		"",//eNexBtnEquipmentInforStringId,
		"",//eBackBtnEquipmentInforExtStringId,
		"",//eHomeBtnEquipmentInforExtStringId,
		"",//ePrevBtnEquipmentInforExtStringId,
		"",//eEquipmentConnectedNameStringId,
		"",//eEquipmentConnectedCommodityCodeStringId,
		"",//eEquipmentConnectedVersionStringId,
		"",//eEquipmentConnectedIDSerialNumberStringId,
		"",//eEquipmentSettingConnectedNameStringId,
		"",//eEquipmentSettingConnectedCommodityCodeStringId,
		"",//eEquipmentSettingConnectedVersionStringId,
		"",//eEquipmentSettingConnectedIDSerialNumberStringId,
		"",//eBackBtnEquipmentSettingStringId,
		"",//eHomeBtnEquipmentSettingStringId,
		"",//eNexBtnEquipmentSettingStringId,
		"",//eClearBtnPatientsCumulativeHoursStringId,
		"",//eValuePatientCumulativeHourStringId,
		"",//eCreateBtnContactStringId,
		"",//eValueEditContactStringID,
		"",//eEquipmentSettingDefaultBtnStringId,
		"",//eBackBtnEquipmentSettingExtStringId,
		"",//eHomeBtnEquipmentSettingExtStringId,
		"",//ePrevBtnEquipmentSettingExtStringId,
		"",//eDeleteExternalRegisterStringId
		"",//eOkBtnStringId,
		"",//eCancelBtnStringId,
		"",//eTitlePopUpEquipmentSettingStringID,
		"",//eZeroContactBtnStringId,
		"",//eOneContactBtnStringId,
		"",//eTwoContactBtnStringId,
		"",//eThreeContactBtnStringId,
		"",//eFourContactBtnStringId,
		"",//eFineContactBtnStringId,
		"",//eSixContactBtnStringId,
		"",//eSevenContactBtnStringId,
		"",//eEightContactBtnStringId,
		"",//eNineContactBtnStringId,
		"",//eOtherContactBtnStringId,
		"",//eBottomTextAlarmStringId,
		"",//eRoundTextAlarmStringId,
		"",//eContactNumberAlarmStringId,
		"",//eCenterTextContactAlarmStringId,
		"",//eO2CylinderBtnStringId,
		"",//eToptextAlarmStringId,
		"",//eAnypalTextStringId,
		"",//eBackBtnMonitorGraphStringId,
		"",//eHomeBtnMonitorGraphStringId,
		"",//eTitlePSAChangeDialogStringId,
		"",//eDevelopmentSettingBtnStringId
		"",//eMaxIDStringId
};

/****************************************************************************************/
/*                                                                                      */
/* General 	-textLibrary_GetString								 						*/
/*                                                                                      */
/* Details  -Get string by string id													*/
/*			  														                    */
/*                                                                                      */
/* Arguments : (I) E_StringID id :  - string id									 		*/
/*						- range : 0-336													*/
/*						- unit :  NONE													*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : char -	string of id	                                                */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) g_stringTable : string table by id							*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
const char* textLibrary_GetString(E_StringID id)
{
	return g_stringTable[id];
}

/* end of files */
