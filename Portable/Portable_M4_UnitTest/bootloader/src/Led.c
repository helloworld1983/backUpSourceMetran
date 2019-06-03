/******************************************************************************/
//
//  File Name		: Led.c
//
//	General       	: provide function for display when update bootloader
//
//  Product Name	: Portable
//
//
//  Revision History:
//          Rev:      Date:       		Editor:
//          01        Feb 23, 2017	    Truong Nguyen
//
/******************************************************************************/
//#include "board.h"
//#include "Led.h"
//#include "IRQHandle.h"
#include "stdint.h"
//#include "stddef.h"
#include "stdbool.h"
//#include "string.h"


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
}E_ID_WORDD;

typedef enum{
	eLevelBrightnessLoww,
	eLevelBrightnessMedd,
	eLevelBrightnessHighh
}E_LEVEL_BRIGHTNESSS;



static const uint8_t gs_ledCode [16] = {0x77, 0x11, 0x6B, 0x3B, 0x1D, 0x3E, 0x7E, 0x13, 0x7F, 0x3F, 0x5F, 0x7C, 0x66, 0x79, 0x6E, 0x4E};
static uint8_t gs_dataLed[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
static bool gs_isDisplayContactAlarm = false;


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_WriteHC4094 													*/
/*                                                                                      */
/* Details:         Write 1 byte to IC 74HC4094						    				*/
/*                                                                                      */
/* Arguments :  uint8_t data        													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
//static void led_WriteHC4094(uint8_t data)
//{
//	for (uint8_t i = 0; i < 8; i++)
//	{
//		if (data & (1<<i))
//		{
//			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT,HC_4094_DATA_GPIO_PIN,1);
//		}
//		else
//		{
//			Chip_GPIO_WritePortBit(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT,HC_4094_DATA_GPIO_PIN,0);
//		}
//
//		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 1); // clock ON
//		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 0);
//	}
//}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_WriteLed		 												*/
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
//static void led_WriteLed()
//{
//	for (uint8_t i = 5; i > 0 ; i--)
//	{
//		led_WriteHC4094(gs_dataLed[i-1]);
//	}
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 1);	//strobe data
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 0);
//}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_InitLed	 														*/
/*                                                                                      */
/* Details:      Initial LEDs on Panel Board						    				*/
/*                                                                                      */
/* Arguments :   None        															*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
//void led_InitLed()
//{
//	Chip_SCU_PinMuxSet(0x7, 4, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[12]
//	Chip_SCU_PinMuxSet(0x7, 5, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC1)); //CTOUT_12 — SCT output 12. Match output 0 of timer 3.
//	Chip_SCU_PinMuxSet(0x7, 6, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[14]
//	Chip_SCU_PinMuxSet(0x7, 7, (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | SCU_MODE_FUNC0)); //GPIO3[15]
//
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN);
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_CLK_GPIO_PORT, HC_4094_DATA_GPIO_PIN);
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,HC_4094_DATA_GPIO_PORT, HC_4094_CLK_GPIO_PIN);
//
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_CLK_GPIO_PORT, HC_4094_CLK_GPIO_PIN, 0);
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_DATA_GPIO_PORT, HC_4094_DATA_GPIO_PIN, 0);
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 0);
//
//
//	/* Initialize the SCT as PWM and set frequency */
//	Chip_SCTPWM_Init(SCT_PWM);
//	Chip_SCTPWM_SetRate(SCT_PWM, SCT_PWM_RATE);
//
//	/* Setup Board specific output pin */
//
//	/* Use SCT_OUT12 pin */
//	Chip_SCTPWM_SetOutPin(SCT_PWM, SCT_PWM_DIMMER, SCT_PWM_PIN_DIMMER);
//
//
//	/* Start with 50% duty cycle */
//	Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
//			Chip_SCTPWM_PercentageToTicks(SCT_PWM, 50));
//
//	//Chip_SCTPWM_Start(SCT_PWM);
//
//	for (uint8_t i = 0; i<5 ; i++)
//	{
//		gs_dataLed[i] = 0;
//	}
//	led_WriteLed();
//}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_SetBrightness 													*/
/*                                                                                      */
/* Details:         Set Brightness of LEDs							    				*/
/*                                                                                      */
/* Arguments :  E_LEVEL_BRIGHTNESS level        										*/
/*                                                                                      */
/* Using Global Data : (I) gs_dataLed  													*/
/*																						*/
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
int led_SetBrightness(E_LEVEL_BRIGHTNESSS level)
{
	switch (level)
	{
	case eLevelBrightnessLoww:
//		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
//				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 10));
		return eLevelBrightnessLoww;
		break;
	case eLevelBrightnessMedd:
//		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
//				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 50));
		return eLevelBrightnessMedd;
		break;
	case eLevelBrightnessHighh:
//		Chip_SCTPWM_SetDutyCycle(SCT_PWM, SCT_PWM_DIMMER,
//				Chip_SCTPWM_PercentageToTicks(SCT_PWM, 100));
		return eLevelBrightnessHighh;
		break;
	default:
		return eLevelBrightnessLoww;
		break;
	}
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_DisplayPercentValue 											*/
/*                                                                                      */
/* Details:    Display  value on LED 7segs												*/
/*                                                                                      */
/* Arguments :  float value       														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
//void led_DisplayPercentValue(uint8_t value)
//{
//	gs_dataLed[0] = gs_ledCode[value % 10];
//	gs_dataLed[1] = gs_ledCode[(value/10)%10];
//	gs_dataLed[2] = gs_ledCode[value/100];
//	led_WriteLed();
//}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_DisplayWord 													*/
/*                                                                                      */
/* Details:    Display Word on LED 7segs												*/
/*                                                                                      */
/* Arguments :  E_ID_WORD word       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
uint8_t led_DisplayWord(E_ID_WORDD word)
{
//	Chip_SCTPWM_Start(SCT_PWM);
	switch (word)
	{
	case eUSB:
		gs_dataLed[2]= 0x75;//'U'
		gs_dataLed[1]= 0x3E;//'S'
		gs_dataLed[0]= 0X7C;//'B'
		break;

	case eA:
		gs_dataLed[2]= 0x5F;//'A'
		gs_dataLed[1]= 0x00;//''
		gs_dataLed[0]= 0x00;//''
		break;

	case eSPI:
		gs_dataLed[2]= 0x3E;//'S'
		gs_dataLed[1]= 0x4F;//'P'
		gs_dataLed[0]= 0x44;//'I'
		break;

	case eOFF:
		gs_dataLed[4]=	0x00;
		gs_dataLed[3]= 0x00;
		gs_dataLed[2]= 0x00;
		gs_dataLed[1]= 0x00;
		gs_dataLed[0]= 0x00;
		break;

	case eU:
		gs_dataLed[2]= 0x75;//'U'
		gs_dataLed[1]= 0x00;
		gs_dataLed[0]= 0x00;
		break;

	case eUP:
		gs_dataLed[2]= 0x75;//'U'
		gs_dataLed[1]= 0x4F;//'P'
		gs_dataLed[0]= 0x00;
		break;

	case eUPd:
		gs_dataLed[2]= 0x75;//'U'
		gs_dataLed[1]= 0x4F;//'P'
		gs_dataLed[0]= 0x79;//'D'
		break;

	case eCHE:
		gs_dataLed[2]= 0x66;//'C'
		gs_dataLed[1]= 0x5D;//'H'
		gs_dataLed[0]= 0x6E;//'E'
		break;

	case eC:
		gs_dataLed[2]= 0x66;//'C'
		gs_dataLed[1]= 0x00;//''
		gs_dataLed[0]= 0x00;//''
		break;
	case eCH:
		gs_dataLed[2]= 0x66;//'C'
		gs_dataLed[1]= 0x5D;//'H'
		gs_dataLed[0]= 0x00;//''
		break;

	case eOnFlowLed:
		gs_dataLed[4] = 0xF0;
		break;
	case eOffAllLed:
		gs_dataLed[4]= 0x00;
		gs_dataLed[3]= 0x00;
		gs_dataLed[2]= 0x00;
		gs_dataLed[1]= 0x00;
		gs_dataLed[0]= 0x00;
		break;
	default:
		break;
	}
//	led_WriteLed();
	return gs_dataLed[0];
}

void led_DisplayHValue(uint8_t value)
{
	gs_dataLed[0] = gs_ledCode[value%10];
	gs_dataLed[1] = gs_ledCode[(value/10)];
	gs_dataLed[2] = 0x5D;//'H'
//	led_WriteLed();
}

void led_DisplayLValue(uint8_t value)
{
	gs_dataLed[0] = gs_ledCode[value%10];
	gs_dataLed[1] = gs_ledCode[(value/10)];
	gs_dataLed[2] = 0x64;//'L'
//	led_WriteLed();
}
void led_DisplayErrorCode(uint8_t code)
{
//	Chip_SCTPWM_Start(SCT_PWM);
	gs_dataLed[0] = gs_ledCode[code%10];
	gs_dataLed[1] = gs_ledCode[(code/10)];
	gs_dataLed[2] = 0xEE;//'E.'
//	led_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_TurnOnContactLed 												*/
/*                                                                                      */
/* Details:    Turn On Contact Led														*/
/*                                                                                      */
/* Arguments :  				      													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOnContactLed()
{
	gs_dataLed[3] |= 0b10000000;
//	led_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_TurnOffContactLed 												*/
/*                                                                                      */
/* Details:    Turn Off Contact Led														*/
/*                                                                                      */
/* Arguments :  				       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOffContactLed()
{
	gs_dataLed[3] &= 0b01111111;
//	led_WriteLed();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_StartDisplayContactAlarm 										*/
/*                                                                                      */
/* Details:    Display contact Alarm Contact 											*/
/*                                                                                      */
/* Arguments :  				       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/
void led_StartDisplayContactAlarm(void)
{
	gs_isDisplayContactAlarm = true;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_StopDisplayContactAlarm 										*/
/*                                                                                      */
/* Details:    Stop Display Contact Alarm												*/
/*                                                                                      */
/* Arguments :  				       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/


void led_StopDisplayContactAlarm(void)
{
	gs_isDisplayContactAlarm = false;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_getStatusDisplayContactAlarm 									*/
/*                                                                                      */
/* Details:    return status display or no display ContactAlarm							*/
/*                                                                                      */
/* Arguments :  				       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/*                                                                                      */
/****************************************************************************************/
bool led_getStatusDisplayContactAlarm()
{
	return gs_isDisplayContactAlarm;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_TurnOnInspectLed 												*/
/*                                                                                      */
/* Details:    Turn On Inspect Led														*/
/*                                                                                      */
/* Arguments :  				      													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOnInspectLed()
{
	gs_dataLed[3] |= 0b00010000;
//	led_WriteLed();
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_TurnOffInspectLed 												*/
/*                                                                                      */
/* Details:    Turn Off Inspect Led														*/
/*                                                                                      */
/* Arguments :  				       													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) gs_dataLed  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOffInspectLed()
{
	gs_dataLed[3] &= 0b11101111;
//	led_WriteLed();
}
