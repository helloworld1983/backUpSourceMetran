
/******************************************************************************/
//
//  File Name		: Led.h
//
//	General       	: XXXX
//
//  Product Name	: Portable
//
//
//  Revision History:
//          Rev:      Date:       		Editor:
//          01        Feb 23, 2017	    Truong Nguyen

//
/******************************************************************************/
#ifndef DEVICE_INC_LED_H_
#define DEVICE_INC_LED_H_

#define SCT_PWM    LPC_SCT

#define HC_4094_DATA_GPIO_PORT	3		// data port
#define HC_4094_DATA_GPIO_PIN	14		// data pin

#define HC_4094_CLK_GPIO_PORT	3		// clock port
#define HC_4094_CLK_GPIO_PIN	15		// clock pin

#define HC_4094_STR_GPIO_PORT	3		// strobe port
#define HC_4094_STR_GPIO_PIN	12		// strobe pin

#define SCT_PWM_PIN_DIMMER	12     // COUT_12 [index 2] Controls LED
#define SCT_PWM_DIMMER		2      // Index of PWM
#define SCT_PWM_RATE   		1000   // PWM frequency 1 KHz


typedef enum{
	eUSB = 0,
	eCHE,
	eA,
	eSPI,
	eOFF,
	eU,
	eUP,
	eUPd,
	eC,
	eCH,
	eOnFlowLed,
	eOffAllLed,
}E_ID_WORD;

typedef enum{
	eLevelBrightnessLow,
	eLevelBrightnessMed,
	eLevelBrightnessHigh
}E_LEVEL_BRIGHTNESS;


/* @brief	Initial  LEDs in panel board
 * @param	Nothing
 * @return	Nothing
 * @note
 */
void led_InitLed();
/****************************************************************/


/* @brief	Set Brightness of LEDs
 * @param	level: there are 3 levels of brightness
 * @return	Nothing
 * @note
 */
void led_SetBrightness(E_LEVEL_BRIGHTNESS level);
/****************************************************************/


/* @brief	Display value Percent on LED 7-Segs on Panel board
 * @param	value:	is the value that display in LED 7segs
 * @return	Nothing
 * @note
 */
void led_DisplayPercentValue(uint8_t value);
/****************************************************************/


/* @brief	Display Word to LED 7-Segs on Panel board
 * @param	word : word that display ErrorCode
 * @return	Nothing
 * @note
 */
void led_DisplayWord(E_ID_WORD word);
/****************************************************************/
/* @brief	Display value on Panel board
 * @return	Nothing
 * @note
 */
void led_DisplayHValue(uint8_t value);
/****************************************************************/
/* @brief	Display value on Panel board
 * @return	Nothing
 * @note
 */
void led_DisplayLValue(uint8_t value);

/****************************************************************/
/* @brief	Display Error Code on Panel board
 * @return	Nothing
 * @note
 */
void led_DisplayErrorCode(uint8_t code);

/****************************************************************/


/* @brief	Turn ON contact LED on Panel board
 * @return	Nothing
 * @note
 */
void led_TurnOnContactLed();

/****************************************************************/


/* @brief	Turn OFF contact LED on Panel board
 * @return	Nothing
 * @note
 */
void led_TurnOffContactLed();

/* @brief	Display Contact alarm
 * @return	Nothing
 * @note
 */
void led_StartDisplayContactAlarm(void);
/* @brief	Stop Display contact alarm
 * @return	Nothing
 * @note
 */
void led_StopDisplayContactAlarm(void);

/* @brief	Return status display or no display ContactAlarm
 * @return	Nothing
 * @note
 */
bool led_getStatusDisplayContactAlarm();
void led_TurnOnInspectLed();
void led_TurnOffInspectLed();
#endif /* DEVICE_INC_LED_H_ */
