/********************************************************************/
/*                                                                  */
/* File Name    : USBControl.h                               		*/
/*                                                                  */
/* General      : Process different request from PC application     */
/*				  Send current system status to PC application		*/
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18,2017     Thao Ha(MV)		  	    new file    */
/*                                                                  */
/********************************************************************/

#ifndef USB_VCOM_INC_USBCONTROL_H_
#define USB_VCOM_INC_USBCONTROL_H_
#define USB_TASK_REQUEST 0
#define USB_TASK_REQUEST_DATA 1
#define USB_TASK_REQUEST_TEST_ALARM_TYPE 1
#define USB_TASK_REQUEST_TEST_ALARM_DATA 2

enum
{
	eMessageIDGetLog,
	eMessageIDGetLogACK,
	eMessageIDSetTriggerSensitivity,
	eMessageIDSetVolume,
	eMessageIDSetOperationTimeFromPreviousMaintenanceContinuousReset,
	eMessageIDSetOperationTimeFromPreviousMaintenanceSynchronizeReset,
	eMessageIDClearHistory,
	eMessageIDClearPressureLog,
	eMessageIDSetDateAndTime,
	eMessageIDGetSystemInformation,
	eMessageIDGetSystemInformationACK,
	eMessageIDGetPressureLog,
	eMessageIDGetPressureLogACK,
	eMessageIDTestAudioVolume,
	eMessageIDSetAllLEDOn,
	eMessageIDSetAllLEDOff,
	eMessageIDSetTotalOperationTimeContinuousReset,
	eMessageIDSetTotalOperationTimeSynchronizeReset,
	eMessageIDSetTriggerSensitivityOffset,
	eMessageIDSetFlowrateOffset,
	eMessageIDSetO2Offset,
	eMessageIDSetDischargeVolumeOffset,
	eMessageIDSetPVSAPress=23,
	eMessageIDSetSol1OnOffsetTime,
	eMessageIDSetSol3OnOffsetTime,
	eMessageIDSetSol2OpenTimeA=28,
	eMessageIDSetSol2OpenTimeB,
	eMessageIDSetSol1DelayOpenTime,
	eMessageIDSetSol3DelayOpenTime,
	eMessageIDSetCoolingFan,
	eMessageIDSetCompressor,
	eMessageIDSetFCOutput,
	eMessageIDSetMaxPSATime,
	eMessageIDSendOperationTimeACK,
	eMessageIDSetTestSynchronizeMode,
	eMessageIDIncreaseFlowrate,
	eMessageIDDecreaseFlowrate,
	eMessageIDChangeMode,
	eMessageIDTestAlarmValue,
	eMessageIDUpgrade,
	eMessageIDSetTriggerSensitivityOffsetFanLow,
	eMessageIDSetTriggerSensitivityOffsetFanMid,
	eMessageIDSetTriggerSensitivityOffsetFanHigh,

//	eMessageIDSetO2AlarmDetectionTime,
	eMessageIDClosePort = 53
};

enum
{
    eProvidePressTestAlarmID,
    eVacuumPressTestAlarmID,
    eTankPressTestAlarmID,
    eBatteryStatusTestAlarmID,
    eBatteryCapacityTestAlarmID,
    eO2ConcentrationTestAlarmID,
    eFlowControllerStatusTestAlarmID,
    eFlowControllerCommunicationTestAlarmID,
    eFlowControllerResponseStatusTestAlarmID,
    eCompressorStatusTestAlarmID,
    eCompressorCommunicationStatusTestAlarmID,
    eCompressorResponseStatusTestAlarmID,
	eDifferentialPressureTestAlarmID,
	eWatchDogTimerTestAlarmID,
	eCompressorBoardTemperatureTestAlarmId,
	eCompressorTemperatureTestAlarmId,
	eSetDefaultParameterTestAlarmId,
	eBatteryChargerErrorTestAlarmId,
    eFlowRateTestAlarmId,
    eEndTestAlarmID
};

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USB_Handle                                                          */
/*                                                                                      */
/* Details      :   This function is called every 30ms to read Vcom buffer to check     */
/*                  whether the request from PC application exists to process, this also*/
/*                  send message to PC application for updating current status on       */
/*                  monitor screen.                                                     */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void USB_Handle(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USB_GetAlarmTest                                                    */
/*                                                                                      */
/* Details      :   Get alarm test                                                      */
/* Arguments:       uint8_t* alarmTestType                                              */
/*                  uint16_t* alarmTestValue                                            */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool USB_GetAlarmTest(uint8_t* alarmTestType,uint16_t* alarmTestValue);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   USB_init                                                            */
/*                                                                                      */
/* Details      :   Init usb vcom                                                       */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void USB_init();

#endif /* USB_VCOM_INC_USBCONTROL_H_ */
