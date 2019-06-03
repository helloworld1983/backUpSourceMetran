
/********************************************************************/
/*                                                                  */
/* File Name    : PanelLed.h	                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and	*/
/* 				display LEDs								        */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          Jan	 15, 2017      Truong Nguyen(MV)  		new file    */
/*                                                                  */
/********************************************************************/
#include "board.h"
#ifndef DEVICE_INC_PANEL_LED_H_
#define DEVICE_INC_PANEL_LED_H_

#define SCT_PWM    LPC_SCT

#define HC_4094_DATA_GPIO_PORT	3		// data port
#define HC_4094_DATA_GPIO_PIN	14		// data pin  12

#define HC_4094_CLK_GPIO_PORT	3		// clock port
#define HC_4094_CLK_GPIO_PIN	15		// clock pin  14

#define HC_4094_STR_GPIO_PORT	3		// strobe port
#define HC_4094_STR_GPIO_PIN	12		// strobe pin  15

#define SCT_PWM_PIN_DIMMER	12     // COUT_12 [index 2] Controls LED
#define SCT_PWM_DIMMER		2      // Index of PWM
#define SCT_PWM_RATE   		1000   // PWM frequency 1 KHz



typedef enum{
	D1,		//	O2_3
	D2,		//	O2_2
	D3,		//	O2_3
	D4,		//	O2_0

	D5,		//	BATT2
	D6,		//	BATT1
	D7,		//	BATT0

	D8,		//	AC_IN
	D9,		//	Consv Mode only During BATT operation
	D10,	//	Continuous Mode
	D11,	//	Synchronized Mode
	D12,	//	CALL 	ALARM
	D13,	//	BREATH	ALARM
	D14,	//	CHARGRE ALARM
	D15		//	CHECK 	ALARM
}E_ID_LED;

typedef enum{
	eNoneUI,
	eWithoutChargeLower = 1,
	eWithoutChargeLow,
	eWithoutChargeMed,
	eWithoutChargeHigh,

	eDuringChargeLower,
	eDuringChargeLow,
	eDuringChargeMed,
	eDuringChargeHigh,

	eACDisconnect,
	eACConnect,

	eContinuousModePattern1,
	eContinuousModePattern2,

	eSynchronizedModeClose,
	eSynchronizedModeOpen,

	eACPowerSupply,
	eBatteryPowerSupply,


	eContactErrorNoTrigger,
	eContactErrorTrigger,

	eBreathErrorNoTrigger,
	eBreathErrorTrigger,

	eLowerBatteryNoTrigger,
	eLowerBatteryTrigger,

	eServiceRequireNoTrigger,
	eServiceRequireTrigger,

	eValueZeroOneZero,
	eValueZeroTwoFive,
	eValueZeroFiveZero,
	eValueZeroSevenFive,
	eValueOneZeroZero,
	eValueOneTwoFive,
	eValueOneFiveZero,
	eValueOneSevenFive,
	eValueTwoZeroZero,
	eValueTwoTowFive,
	eValueTwoFiveZero,
	eValueTwoSevenFive,
	eValueThreeZeroZero,

	eOffSevenSeg,
	eDisplayWordSET,
	eDisplayWordE,
	eDisplayWordEn,
	eDisplayWordEnd,
	eDisplayWordUSB,
	eOffAllLeds,
	eOnAllLeds,
	eOffAlarmLeds,

}E_ID_CONDITION;


typedef enum{
	eErrorBreath = 1,
	eErrorAbnormalOxygenConcentration,
	eErrorLowOxygenConcentration,
	eErrorOxygenTankPressure,
	eErrorBatteryTemperature,
	eErrorBatteryCharge,
	eErrorBatteryRemainingCapacityLower,
	eErrorPowerLost,
	eErrorCompressorSupplyPressure,
	eErrorCompressorVacuumPressure,
	eErrorFlowcontrol = 11,
	eErrorSupplyPressureSensor,
	eErrorVacuumPressureSensor,
	eErrorTankPressureSensor,
	eErrorDifferentialPressureSensorCommunication,
	eErrorO2Sensor,
	eErrorFlowControllerCommunicationNoResponse,
	eErrorFlowControllerPressureSensor,
	eErrorFlowControllerFlowSensor,
	eErrorFlowControllerTemperatureSensor,
	eErrorFlowControllerCommunicationCheckSum,
	eErrorFlowControllerCommunication ,//22
	eErrorFlowControllerCommunicationCommand,
	eErrorFlowControllerCommunicationParameter,
	eFlowControllerResponseCheckSumError,//25
	eFlowControllerResponseCodeError,//26
	eFlowControllerResponseDataLenghtError,//27
	eErrorCompressorCommunicationNoResponse,
	eErrorCompressorDCOverVoltage,
	eErrorCompressorDCLowerVoltage,
	eErrorCompressorSoftwareOverCurrent,
	eErrorCompressorHardwareOverCurrent,
	eErrorCompressorBoardTemperature,
	eErrorCompressorMotorTemperature,
	eErrorCompressorRotationSpeed ,
	eErrorCompressorStepOut,
	eErrorCompressorCommunication,
	eErrorCompressorCommunicationCheckSum,
	eErrorCompressorCommunicationCommand,
	eErrorCompressorCommunicationParameter,
	eErrorCompressorResponseCheckSumError,//41
	eErrorCompressorResponseCodeError,//42
	eErrorCompressorResponseDataLenghtError,//43
	eErrorLEDDriverCommunication,
	eErrorAudioCommunication,
	eErrorSpeaker,
	eErrorSPIFlashMemory,
	eErrorBatteryType,	//48
	eErrorTachypnea,

}E_ID_ERRORCODE;


typedef enum{
	eLevelBrightnessLow,
	eLevelBrightnessMed,
	eLevelBrightnessHigh,
}E_LEVEL_BRIGHTNESS;



/* @brief	Initial all LEDs in panel board
 * @param	Nothing
 * @return	Nothing
 * @note
 */
void panelLed_InitLed();
/****************************************************************/


/* @brief	Set Brightness of LEDs
 * @param	level: there are 3 levels of brightness
 * @return	Nothing
 * @note
 */
void panelLed_SetBrightness(E_LEVEL_BRIGHTNESS level);
/****************************************************************************************/

/* @brief	Display Error Code on LED 7-Segs on Panel board
 * @param	errorCode:	is the Error code that display in LED 7segs
 * @return	Nothing
 * @note
 */
void panelLed_DisplayErrorCode(E_ID_ERRORCODE errorCode);
/****************************************************************/

/* @brief	Control all separate LEDs depend on id
 * @param	idCondition	: condition that control all separate LEDs
 * @return	Nothing
 * @note
 */
void panelLed_LedDisplay(E_ID_CONDITION idCondition);

/****************************************************************/

/* @brief	This function for displaying operation time of Portable
 * @param	value	: value display
 * @return	Nothing
 * @note
 */

void panelLed_SevenSegDisplayOperationTimeValue(uint16_t value);

/****************************************************************/

/* @brief	Clear all display for testing Function
 * @param	Nothing
 * @return	Nothing
 * @note
 */
void panelLED_ClearOnAllLED();

#endif /* DEVICE_INC_PANEL_LED_H_ */

