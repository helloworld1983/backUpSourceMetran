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
#include <PanelLed.h>
#include <TaskCommon.h>
#include "board.h"
#include "timers.h"

#ifndef pMS_TO_TICKS
#define pMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#endif


#define LED_250MS_TICKS_TIME	250
#define LED_START_250MS_ID	1

static const uint8_t gs_LEDCode [16] = {0xF7, 0x91, 0xEB, 0xBB, 0x9D, 0xBE, 0xFE, 0x93, 0xFF, 0xBF, 0xDF, 0xEC, 0xE6, 0xE7, 0xEE, 0xCE};
static const uint8_t gs_LEDCode1 [16] = {0x77, 0x11, 0x6B, 0x3B, 0x1D, 0x3E, 0x7E, 0x13, 0x7F, 0x3F, 0x5F, 0x7C, 0x66, 0x79, 0x6E, 0x4E};

static TimerHandle_t gs_Start250MSTimer;
static uint8_t gs_dataLed[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

static bool gs_isContiModePattern1 = false;
static bool gs_isDuringChargeLower = false;
static bool gs_isDuringChargeLow = false;
static bool gs_isDuringChargeMed = false;


static bool gs_isContactAlarm = false;
static bool gs_isBreathAlarm = false;
static bool gs_isBatteryLower = false;

static uint8_t gs_t0tick = 0;
static uint8_t gs_t1tick = 0;
static uint8_t gs_t2tick = 0;
static bool gs_isOnAllLED = false;
static bool gs_isOnChargingLed = false;

static void panelLed_UpdateAnimation();

static void TimerCallback( TimerHandle_t xTimer )
{
	if (xTimerStart(gs_Start250MSTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start LED start 250MS timer \n");
	}
	panelLed_UpdateAnimation();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_StartTimer250MSforAnimation 								*/
/*                                                                                      */
/* Details:         Start Timer 250MS								    				*/
/*                                                                                      */
/* Arguments : (I)  None      															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_StartTimer250MSforAnimation()	// Timer for update Animation LEDs
{
	if (xTimerStart(gs_Start250MSTimer,10) != pdPASS)
	{
		DEBUG_TASK("ERR: Failed to start LED start 250MS timer \n");
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_WriteHC4094 												*/
/*                                                                                      */
/* Details:         Write 1 byte to IC 74HC4094						    				*/
/*                                                                                      */
/* Arguments :  uint8_t data        													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_WriteHC4094(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & (1<<i))
		{
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT,HC_4094_DATA_GPIO_PIN,1);
		}
		else
		{
			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT,HC_4094_DATA_GPIO_PIN,0);
		}

		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 1); // clock ON
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 0);
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_WriteLed	 												*/
/*                                                                                      */
/* Details:         Write 1 array to IC 74HC4094					    				*/
/*                                                                                      */
/* Arguments : (I)  None        														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_WriteLed()
{
	for (uint8_t i = 5; i > 0 ; i--)
	{
		panelLed_WriteHC4094(gs_dataLed[i-1]);
	}
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 1);	//strobe data
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 0);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_CreateTimer50MS 											*/
/*                                                                                      */
/* Details:         Create Timer50MS Animation of LEDs				    				*/
/*                                                                                      */
/* Arguments : None      																*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_CreateTimer250MS()
{
	gs_Start250MSTimer = xTimerCreate
			( /* Just a text name, not used by the RTOS
			                      kernel. */
					"Led Start 250MS timer",
					/* The timer period in ticks, must be
			                      greater than 0. */
					pMS_TO_TICKS(LED_250MS_TICKS_TIME),
					/* The timers will auto-reload themselves
			                      when they expire. */
					pdFALSE,
					/* The ID is used to store a count of the
			                      number of times the timer has expired, which
			                      is initialised to 0. */
					( void * ) LED_START_250MS_ID,
					/* Each timer calls the same callback when
			                      it expires. */
					TimerCallback
			);
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_InitLed	 												*/
/*                                                                                      */
/* Details:      Initial All LEDs on Panel Board					    				*/
/*                                                                                      */
/* Arguments :   None        															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
void panelLed_InitLed()
{

	panelLed_CreateTimer250MS();
	Chip_SCU_PinMuxSet(0x7, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[12]
	Chip_SCU_PinMuxSet(0x7, 5, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC1)); //CTOUT_12 — SCT output 12. Match output 0 of timer 3.
	Chip_SCU_PinMuxSet(0x7, 6, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[14]
	Chip_SCU_PinMuxSet(0x7, 7, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[15]

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_CLK_GPIO_PORT, HC_4094_DATA_GPIO_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT, HC_4094_CLK_GPIO_PIN);

	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 0);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_DATA_GPIO_PORT, HC_4094_DATA_GPIO_PIN, 0);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 0);


	/* Initialize the SCT as PWM and set frequency */
	Chip_SCTPWM_Init(SCT_PWM);
	Chip_SCTPWM_SetRate(SCT_PWM, SCT_PWM_RATE);

	/* Setup Board specific output pin */

	/* Use SCT_OUT12 pin */
	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_DIMMER, SCT_PWM_PIN_DIMMER);


	/* Start with 50% duty cycle */
	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
			Chip_SCTPWM_PercentageToTicks(SCT_PWM, 50));



	for (uint8_t i = 0; i<5 ; i++)
	{
		gs_dataLed[i] = 0;
	}
	panelLed_WriteLed();
	panelLed_StartTimer250MSforAnimation();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_SetBrightness 												*/
/*                                                                                      */
/* Details:         Set Brightness of LEDs							    				*/
/*                                                                                      */
/* Arguments :  E_LEVEL_BRIGHTNESS level        										*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void panelLed_SetBrightness(E_LEVEL_BRIGHTNESS level)
{
	switch (level)
	{
	case eLevelBrightnessLow:
		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 10));
		break;
	case eLevelBrightnessMed:
		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 50));
		break;
	case eLevelBrightnessHigh:
		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 100));
		break;
	default:
		break;
	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_TurnOnLED 													*/
/*                                                                                      */
/* Details:    Turn ON 1 LED									  		  				*/
/*                                                                                      */
/* Arguments :  E_ID_LED idLED        													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_TurnOnLED(E_ID_LED idLED)
{
	switch(idLED)
	{
	case D1:
		gs_dataLed[4] |= 0b00010000;
		break;
	case D2:
		gs_dataLed[4] |= 0b00100000;
		break;
	case D3:
		gs_dataLed[4] |= 0b01000000;
		break;
	case D4:
		gs_dataLed[4] |= 0b10000000;
		break;
	case D5:
		gs_dataLed[4] |= 0b00000010;
		break;
	case D6:
		gs_dataLed[4] |= 0b00000100;
		break;
	case D7:
		gs_dataLed[4] |= 0b00001000;
		break;
	case D8:
		gs_dataLed[4] |= 0b00000001;
		break;
	case D9:
		gs_dataLed[3] |= 0b00001000;
		break;
	case D10:
		gs_dataLed[3] |= 0b00000100;
		break;
	case D11:
		gs_dataLed[3] |= 0b00000010;
		break;
	case D12:
		gs_dataLed[3] |= 0b10000000;
		break;
	case D13:
		gs_dataLed[3] |= 0b01000000;
		break;
	case D14:
		gs_dataLed[3] |= 0b00100000;
		break;
	case D15:
		gs_dataLed[3] |= 0b00010000;
		break;
	default:
		break;
	}
	panelLed_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_TurnOffLED 												*/
/*                                                                                      */
/* Details:    Turn OFF 1 LED									  		  				*/
/*                                                                                      */
/* Arguments :  E_ID_LED idLED        													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_TurnOffLED(E_ID_LED idLED)
{
	switch(idLED)
	{
	case D1:
		gs_dataLed[4] &= 0b11101111;
		break;
	case D2:
		gs_dataLed[4] &= 0b11011111;
		break;
	case D3:
		gs_dataLed[4] &= 0b10111111;
		break;
	case D4:
		gs_dataLed[4] &= 0b01111111;
		break;
	case D5:
		gs_dataLed[4] &= 0b11111101;
		break;
	case D6:
		gs_dataLed[4] &= 0b11111011;
		break;
	case D7:
		gs_dataLed[4] &= 0b11110111;
		break;
	case D8:
		gs_dataLed[4] &= 0b11111110;
		break;
	case D9:
		gs_dataLed[3] &= 0b11110111;
		break;
	case D10:
		gs_dataLed[3] &= 0b11111011;
		break;
	case D11:
		gs_dataLed[3] &= 0b11111101;
		break;
	case D12:
		gs_dataLed[3] &= 0b01111111;
		break;
	case D13:
		gs_dataLed[3] &= 0b10111111;
		break;
	case D14:
		gs_dataLed[3] &= 0b11011111;
		break;
	case D15:
		gs_dataLed[3] &= 0b11101111;
		break;
	default:
		break;
	}
	panelLed_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_SeperateLedDisplay 										*/
/*                                                                                      */
/* Details:    Display indicators to seperate LEDs 										*/
/*                                                                                      */
/* Arguments :  E_ID_CONDITION idCondition       										*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void panelLed_LedDisplay(E_ID_CONDITION idCondition)
{
	static bool s_IsFirstSequence = true;
	if(s_IsFirstSequence == true)
	{
		Chip_SCTPWM_Start(SCT_PWM);
		s_IsFirstSequence = false;
	}
	switch (idCondition)
	{
		case eBreathErrorNoTrigger:
			gs_isBreathAlarm = false;
			break;
		case eLowerBatteryNoTrigger:
			gs_isBatteryLower = false;
			break;
		default:
			break;
	}
	if(gs_isOnAllLED==true)
	{
		return;
	}
	switch(idCondition)
	{
		case eWithoutChargeLower:
			panelLed_TurnOffLED(D5);
			panelLed_TurnOffLED(D6);
			panelLed_TurnOffLED(D7);
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;

			break;
		case eWithoutChargeLow:
			panelLed_TurnOffLED(D5);
			panelLed_TurnOffLED(D6);
			panelLed_TurnOnLED(D7);
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;

			break;
		case eWithoutChargeMed:
			panelLed_TurnOffLED(D5);
			panelLed_TurnOnLED(D6);
			panelLed_TurnOnLED(D7);
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;

			break;
		case eWithoutChargeHigh:
			panelLed_TurnOnLED(D5);
			panelLed_TurnOnLED(D6);
			panelLed_TurnOnLED(D7);
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;

			break;

		case eDuringChargeLower:
			gs_isDuringChargeLower = true;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;

			break;

		case eDuringChargeLow:
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = true;
			gs_isDuringChargeMed = false;

			break;

		case eDuringChargeMed:
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = true;

			break;

		case eDuringChargeHigh:
			gs_isDuringChargeLower = false;
			gs_isDuringChargeLow = false;
			gs_isDuringChargeMed = false;
			panelLed_TurnOnLED(D5);
			panelLed_TurnOnLED(D6);
			panelLed_TurnOnLED(D7);
			break;

		case eACConnect:
			panelLed_TurnOnLED(D8);
			break;

		case eACDisconnect:
			panelLed_TurnOffLED(D8);
			break;

		case eContinuousModePattern1:
			gs_isContiModePattern1 = true;
			panelLed_TurnOffLED(D9);// new add
			panelLed_TurnOnLED(D11);
			panelLed_TurnOffLED(D10);
			break;
		case eContinuousModePattern2:
			gs_isContiModePattern1 = false;
			panelLed_TurnOffLED(D9);// new add
			panelLed_TurnOnLED(D11);
			panelLed_TurnOffLED(D10);
			break;
		case eSynchronizedModeOpen:
			gs_isContiModePattern1 = false;
			panelLed_TurnOffLED(D9);// new add
			panelLed_TurnOnLED(D10);
			panelLed_TurnOffLED(D11);
			panelLed_TurnOnLED(D1);
			panelLed_TurnOnLED(D2);
			panelLed_TurnOnLED(D3);
			panelLed_TurnOnLED(D4);
			break;

		case eSynchronizedModeClose:
			gs_isContiModePattern1 = false;
			panelLed_TurnOffLED(D9);// new add
			panelLed_TurnOnLED(D10);
			panelLed_TurnOffLED(D11);
			panelLed_TurnOffLED(D1);
			panelLed_TurnOffLED(D2);
			panelLed_TurnOffLED(D3);
			panelLed_TurnOffLED(D4);
			break;

		case eContactErrorTrigger:
			gs_isContactAlarm = true;
			break;

		case eContactErrorNoTrigger:
			gs_isContactAlarm = false;
			panelLed_TurnOffLED(D12);
			break;

		case eBreathErrorTrigger:
			gs_isBreathAlarm = true;
			break;

		case eBreathErrorNoTrigger:
			gs_isBreathAlarm = false;
			panelLed_TurnOffLED(D13);
			break;

		case eLowerBatteryTrigger:
			gs_isBatteryLower = true;
			break;

		case eLowerBatteryNoTrigger:
			gs_isBatteryLower = false;
			panelLed_TurnOffLED(D14);
			break;

		case eServiceRequireTrigger:

			panelLed_TurnOnLED(D15);
			break;

		case eServiceRequireNoTrigger:

			panelLed_TurnOffLED(D15);
			break;

		case eValueZeroOneZero:
			gs_dataLed[2]= gs_LEDCode[0];
			gs_dataLed[1]= gs_LEDCode[1];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueZeroTwoFive:
			gs_dataLed[2]= gs_LEDCode[0];
			gs_dataLed[1]= gs_LEDCode[2];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueZeroFiveZero:
			gs_dataLed[2]= gs_LEDCode[0];
			gs_dataLed[1]= gs_LEDCode[5];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueZeroSevenFive:
			gs_dataLed[2]= gs_LEDCode[0];
			gs_dataLed[1]= gs_LEDCode[7];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueOneZeroZero:
			gs_dataLed[2]= gs_LEDCode[1];
			gs_dataLed[1]= gs_LEDCode[0];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueOneTwoFive:
			gs_dataLed[2]= gs_LEDCode[1];
			gs_dataLed[1]= gs_LEDCode[2];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueOneFiveZero:
			gs_dataLed[2]= gs_LEDCode[1];
			gs_dataLed[1]= gs_LEDCode[5];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueOneSevenFive:
			gs_dataLed[2]= gs_LEDCode[1];
			gs_dataLed[1]= gs_LEDCode[7];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueTwoZeroZero:
			gs_dataLed[2]= gs_LEDCode[2];
			gs_dataLed[1]= gs_LEDCode[0];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueTwoTowFive:
			gs_dataLed[2]= gs_LEDCode[2];
			gs_dataLed[1]= gs_LEDCode[2];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueTwoFiveZero:
			gs_dataLed[2]= gs_LEDCode[2];
			gs_dataLed[1]= gs_LEDCode[5];
			gs_dataLed[0]= gs_LEDCode[0];
			break;

		case eValueTwoSevenFive:
			gs_dataLed[2]= gs_LEDCode[2];
			gs_dataLed[1]= gs_LEDCode[7];
			gs_dataLed[0]= gs_LEDCode[5];
			break;

		case eValueThreeZeroZero:
			gs_dataLed[2]= gs_LEDCode[3];
			gs_dataLed[1]= gs_LEDCode[0];
			gs_dataLed[0]= gs_LEDCode[0];
			break;


		case eOffSevenSeg:
			gs_dataLed[2]= 0;
			gs_dataLed[1]= 0;
			gs_dataLed[0]= 0;
			break;
		case eDisplayWordSET:
			gs_dataLed[2]= 0x3E;//'S'
			gs_dataLed[1]= 0x6E;//'E'
			gs_dataLed[0]= 0x6C;//'T'
			break;
		case eDisplayWordE:
			gs_dataLed[2]= 0x6E;//'E'
			gs_dataLed[1]= 0x00;//''
			gs_dataLed[0]= 0x00;//''
			break;
		case eDisplayWordEn:
			gs_dataLed[2]= 0x6E;//'E'
			gs_dataLed[1]= 0x58;//'n'
			gs_dataLed[0]= 0x00;//''
			break;
		case eDisplayWordEnd:
			gs_dataLed[2]= 0x6E;//'E'
			gs_dataLed[1]= 0x58;//'n'
			gs_dataLed[0]= 0x79;//'d'
			break;
		case eDisplayWordUSB:
			gs_dataLed[2]= 0x75;//'U'
			gs_dataLed[1]= 0x3E;//'S'
			gs_dataLed[0]= 0X7C;//'B'

		case eOffAllLeds:
			gs_isContiModePattern1 = false;
			gs_isOnChargingLed = false;
			gs_dataLed[4]= 0x00;
			gs_dataLed[3]= 0x00;
			gs_dataLed[2]= 0x00;
			gs_dataLed[1]= 0x00;
			gs_dataLed[0]= 0x00;
			break;
		case eOnAllLeds:
			gs_isContiModePattern1 = false;
			gs_dataLed[4]= 0xFF;
			gs_dataLed[3]= 0xFF;
			gs_dataLed[2]= 0xFF;
			gs_dataLed[1]= 0xFF;
			gs_dataLed[0]= 0xFF;
			gs_isOnAllLED=true;
			gs_isOnChargingLed = true;
			break;
		case eOffAlarmLeds:
			panelLed_TurnOffLED(D12);
			panelLed_TurnOffLED(D13);
			panelLed_TurnOffLED(D14);
			panelLed_TurnOffLED(D15);
			gs_isOnAllLED = false;
		default:
			break;
	}
	panelLed_WriteLed();
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_DisplayErrorCode		 									*/
/*                                                                                      */
/* Details:    Display Error Code														*/
/*                                                                                      */
/* Arguments :  E_ID_ERRORCODE errorCode   												*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void panelLed_DisplayErrorCode(E_ID_ERRORCODE errorCode)
{
	switch(errorCode)
	{
	case eErrorBreath:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[1];
		break;
	case eErrorAbnormalOxygenConcentration:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[2];
		break;
	case eErrorLowOxygenConcentration:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[3];
		break;
	case eErrorOxygenTankPressure:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[4];
		break;
	case eErrorBatteryTemperature:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[5];
		break;
	case eErrorBatteryCharge:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[6];
		break;
	case eErrorBatteryRemainingCapacityLower:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[7];
		break;
	case eErrorPowerLost:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[8];
		break;
	case eErrorCompressorSupplyPressure:
		gs_dataLed[1] = gs_LEDCode[0];
		gs_dataLed[0] = gs_LEDCode[9];
		break;
	case eErrorCompressorVacuumPressure:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[0];
		break;
	case eErrorFlowcontrol:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[1];
		break;
	case eErrorSupplyPressureSensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[2];
		break;
	case eErrorVacuumPressureSensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[3];
		break;
	case eErrorTankPressureSensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[4];
		break;
	case eErrorDifferentialPressureSensorCommunication:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[5];
		break;
	case eErrorO2Sensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[6];
		break;
	case eErrorFlowControllerCommunicationNoResponse:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[7];
		break;
	case eErrorFlowControllerPressureSensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[8];
		break;
	case eErrorFlowControllerFlowSensor:
		gs_dataLed[1] = gs_LEDCode[1];
		gs_dataLed[0] = gs_LEDCode[9];
		break;
	case eErrorFlowControllerTemperatureSensor:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[0];
		break;
	case eErrorFlowControllerCommunication:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[1];
		break;
	case eErrorFlowControllerCommunicationCheckSum:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[2];
		break;
	case eErrorFlowControllerCommunicationCommand:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[3];
		break;
	case eErrorFlowControllerCommunicationParameter:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[4];
		break;
	case eFlowControllerResponseCheckSumError:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[5];
		break;
	case eFlowControllerResponseCodeError:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[6];
		break;
	case eFlowControllerResponseDataLenghtError:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[7];
		break;
	case eErrorCompressorCommunicationNoResponse:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[8];
		break;
	case eErrorCompressorDCOverVoltage:
		gs_dataLed[1] = gs_LEDCode[2];
		gs_dataLed[0] = gs_LEDCode[9];
		break;
	case eErrorCompressorDCLowerVoltage:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[0];
		break;
	case eErrorCompressorSoftwareOverCurrent:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[1];
		break;
	case eErrorCompressorHardwareOverCurrent:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[2];
		break;
	case eErrorCompressorBoardTemperature:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[3];
		break;
	case eErrorCompressorMotorTemperature:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[4];
		break;
	case eErrorCompressorRotationSpeed:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[5];
		break;
	case eErrorCompressorStepOut:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[6];
		break;
	case eErrorCompressorCommunication:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[7];
		break;
	case eErrorCompressorCommunicationCheckSum:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[8];
		break;
	case eErrorCompressorCommunicationCommand:
		gs_dataLed[1] = gs_LEDCode[3];
		gs_dataLed[0] = gs_LEDCode[9];
		break;
	case eErrorCompressorCommunicationParameter:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[0];
		break;
	case eErrorCompressorResponseCheckSumError:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[1];
		break;
	case eErrorCompressorResponseCodeError:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[2];
		break;
	case eErrorCompressorResponseDataLenghtError:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[3];
		break;
	case eErrorLEDDriverCommunication:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[4];
		break;
	case eErrorAudioCommunication:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[5];
		break;
	case eErrorSpeaker:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[6];
		break;
	case eErrorSPIFlashMemory:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[7];
		break;
	case eErrorBatteryType:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[8];
		break;

	case eErrorTachypnea:
		gs_dataLed[1] = gs_LEDCode[4];
		gs_dataLed[0] = gs_LEDCode[9];
		break;
	}
	gs_dataLed[2] = gs_LEDCode[14];
	panelLed_WriteLed();
}



/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_UpdateAnimation 											*/
/*                                                                                      */
/* Details:    Update status of Animation seperate LEDs									*/
/*                                                                                      */
/* Arguments :  None      																*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
static void panelLed_UpdateAnimation()
{
	gs_t0tick++;
	gs_t1tick++;
	gs_t2tick++;
	if(gs_t2tick == 4)
	{
		if(gs_isContiModePattern1)
		{
			panelLed_TurnOnLED(D1);
			panelLed_TurnOffLED(D2);
			panelLed_TurnOffLED(D3);
			panelLed_TurnOffLED(D4);
		}
	}
	else if(gs_t2tick == 8)
	{
		if(gs_isContiModePattern1)
		{
			panelLed_TurnOffLED(D1);
			panelLed_TurnOnLED(D2);
			panelLed_TurnOffLED(D3);
			panelLed_TurnOffLED(D4);
		}
	}
	else if(gs_t2tick == 12)
	{
		if(gs_isContiModePattern1)
		{
			panelLed_TurnOffLED(D1);
			panelLed_TurnOffLED(D2);
			panelLed_TurnOnLED(D3);
			panelLed_TurnOffLED(D4);
		}
	}
	else if(gs_t2tick == 16)
	{
		gs_t2tick = 0;
		if(gs_isContiModePattern1)
		{
			panelLed_TurnOffLED(D1);
			panelLed_TurnOffLED(D2);
			panelLed_TurnOffLED(D3);
			panelLed_TurnOnLED(D4);
		}

	}

	if(gs_t1tick == 1)
	{
		if(gs_isContactAlarm)
		{
			panelLed_TurnOnLED(D12);
		}
	}
	else if(gs_t1tick == 2)
	{
		gs_t1tick = 0;
		if(gs_isContactAlarm)
		{
			panelLed_TurnOffLED(D12);
		}
	}

	if(gs_t0tick == 4)	//1000ms
	{
		if (gs_isOnChargingLed == false)
		{
			if(gs_isDuringChargeLower)
			{
				panelLed_TurnOffLED(D5);
				panelLed_TurnOffLED(D6);
				panelLed_TurnOnLED(D7);
			}
			if(gs_isDuringChargeLow)
			{
				panelLed_TurnOffLED(D5);
				panelLed_TurnOnLED(D6);
				panelLed_TurnOnLED(D7);
			}
			if(gs_isDuringChargeMed)
			{
				panelLed_TurnOnLED(D5);
				panelLed_TurnOnLED(D6);
				panelLed_TurnOnLED(D7);
			}
		}
		if(gs_isBreathAlarm)
		{
			panelLed_TurnOnLED(D13);
		}
		if(gs_isBatteryLower)
		{
			panelLed_TurnOnLED(D14);
		}

	}
	else if(gs_t0tick == 8)	//2000ms
	{
		gs_t0tick = 0;

		if (gs_isOnChargingLed == false)
		{
			if(gs_isDuringChargeLower)
			{
				panelLed_TurnOffLED(D5);
				panelLed_TurnOffLED(D6);
				panelLed_TurnOffLED(D7);
			}
			if(gs_isDuringChargeLow)
			{
				panelLed_TurnOffLED(D5);
				panelLed_TurnOffLED(D6);
				panelLed_TurnOnLED(D7);
			}
			if(gs_isDuringChargeMed)
			{
				panelLed_TurnOffLED(D5);
				panelLed_TurnOnLED(D6);
				panelLed_TurnOnLED(D7);
			}
		}
		if(gs_isBreathAlarm)
		{
			panelLed_TurnOffLED(D13);
		}
		if(gs_isBatteryLower)
		{
			panelLed_TurnOffLED(D14);
		}

	}
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLed_SevenSegDisplayTimeValue 									*/
/*                                                                                      */
/* Details:    This function for displaying operation time of Portable					*/
/*                                                                                      */
/* Arguments :  float value       														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void panelLed_SevenSegDisplayOperationTimeValue(uint16_t value)
{
	gs_dataLed[0] = gs_LEDCode1[value % 10];
	gs_dataLed[1] = gs_LEDCode1[(value/10)%10];
	gs_dataLed[2] = gs_LEDCode1[value/100];
	panelLed_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	panelLED_ClearOnAllLED 												*/
/*                                                                                      */
/* Details:    Clear all display for testing Function									*/
/*                                                                                      */
/* Arguments :  float value       														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void panelLED_ClearOnAllLED()
{
	gs_isOnAllLED = false;
	panelLed_LedDisplay(eOffAllLeds);
}




