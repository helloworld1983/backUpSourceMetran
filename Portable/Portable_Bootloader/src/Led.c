/******************************************************************************/
//
//  File Name		: Led.c
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
#include "board.h"
#include "Led.h"
#include "IRQHandle.h"


//#define RATE_HZ 4  // frequency interrupt of Timer0 is 10Hz

static const uint8_t Led_Code [16] = {0x77, 0x11, 0x6B, 0x3B, 0x1D, 0x3E, 0x7E, 0x13, 0x7F, 0x3F, 0x5F, 0x7C, 0x66, 0x79, 0x6E, 0x4E};
static uint8_t dataled[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
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
static void led_WriteHC4094(uint8_t data)
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
/* Function name: 	led_WriteLed		 												*/
/*                                                                                      */
/* Details:         Write 1 array to IC 74HC4094					    				*/
/*                                                                                      */
/* Arguments : (I)  None        														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
static void led_WriteLed()
{
	for (uint8_t i = 5; i > 0 ; i--)
	{
		led_WriteHC4094(dataled[i-1]);
	}
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 1);	//strobe data
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, HC_4094_STR_GPIO_PORT, HC_4094_STR_GPIO_PIN, 0);
}


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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_InitLed()
{
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

	//Chip_SCTPWM_Start(SCT_PWM);

	for (uint8_t i = 0; i<5 ; i++)
	{
		dataled[i] = 0;
	}
	led_WriteLed();
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_SetBrightness 													*/
/*                                                                                      */
/* Details:         Set Brightness of LEDs							    				*/
/*                                                                                      */
/* Arguments :  E_LEVEL_BRIGHTNESS level        										*/
/*                                                                                      */
/* Using Global Data : (I) dataled  													*/
/*																						*/
/* ReturnValue : None                                                         			*/
/*                                                                                      */
/****************************************************************************************/
void led_SetBrightness(E_LEVEL_BRIGHTNESS level)
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
/* Function name: 	led_DisplayPercentValue 											*/
/*                                                                                      */
/* Details:    Display  value on LED 7segs												*/
/*                                                                                      */
/* Arguments :  float value       														*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_DisplayPercentValue(uint8_t value)
{
	dataled[0] = Led_Code[value % 10];
	dataled[1] = Led_Code[(value/10)%10];
	dataled[2] = Led_Code[value/100];
	led_WriteLed();
}


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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_DisplayWord(E_ID_WORD word)
{
	Chip_SCTPWM_Start(SCT_PWM);
	switch (word)
	{
	case eUSB:
		dataled[2]= 0x75;//'U'
		dataled[1]= 0x3E;//'S'
		dataled[0]= 0X7C;//'B'
		break;

	case eA:
		dataled[2]= 0x5F;//'A'
		dataled[1]= 0x00;//''
		dataled[0]= 0x00;//''
		break;

	case eSPI:
		dataled[2]= 0x3E;//'S'
		dataled[1]= 0x4F;//'P'
		dataled[0]= 0x44;//'I'
		break;

	case eOFF:
		dataled[4]=	0x00;
		dataled[3]= 0x00;
		dataled[2]= 0x00;
		dataled[1]= 0x00;
		dataled[0]= 0x00;
		break;

	case eU:
		dataled[2]= 0x75;//'U'
		dataled[1]= 0x00;
		dataled[0]= 0x00;
		break;

	case eUP:
		dataled[2]= 0x75;//'U'
		dataled[1]= 0x4F;//'P'
		dataled[0]= 0x00;
		break;

	case eUPd:
		dataled[2]= 0x75;//'U'
		dataled[1]= 0x4F;//'P'
		dataled[0]= 0x79;//'D'
		break;

	case eCHE:
		dataled[2]= 0x66;//'C'
		dataled[1]= 0x5D;//'H'
		dataled[0]= 0x6E;//'E'
		break;

	case eC:
		dataled[2]= 0x66;//'C'
		dataled[1]= 0x00;//''
		dataled[0]= 0x00;//''
		break;
	case eCH:
		dataled[2]= 0x66;//'C'
		dataled[1]= 0x5D;//'H'
		dataled[0]= 0x00;//''
		break;

	case eOnFlowLed:
		dataled[4] = 0xF0;
		break;
	case eOffAllLed:
		dataled[4]= 0x00;
		dataled[3]= 0x00;
		dataled[2]= 0x00;
		dataled[1]= 0x00;
		dataled[0]= 0x00;
		break;
	default:
		break;
	}
	led_WriteLed();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_DisplayHValue 													*/
/*                                                                                      */
/* Details:    Display High value of Main SW Version									*/
/*                                                                                      */
/* Arguments :  				      													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_DisplayHValue(uint8_t value)
{
	dataled[0] = Led_Code[value%10];
	dataled[1] = Led_Code[(value/10)];
	dataled[2] = 0x5D;//'H'
	led_WriteLed();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_DisplayLValue 													*/
/*                                                                                      */
/* Details:    Display low value of Main SW Version										*/
/*                                                                                      */
/* Arguments :  				      													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_DisplayLValue(uint8_t value)
{
	dataled[0] = Led_Code[value%10];
	dataled[1] = Led_Code[(value/10)];
	dataled[2] = 0x64;//'L'
	led_WriteLed();
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	led_DisplayErrorCode 												*/
/*                                                                                      */
/* Details:    Display Error Code														*/
/*                                                                                      */
/* Arguments :  				      													*/
/*                                                                                      */
/* ReturnValue : None                                                         			*/
/*																						*/
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_DisplayErrorCode(uint8_t code)
{
	Chip_SCTPWM_Start(SCT_PWM);
	dataled[0] = Led_Code[code%10];
	dataled[1] = Led_Code[(code/10)];
	dataled[2] = 0xEE;//'E.'
	led_WriteLed();
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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOnContactLed()
{
	dataled[3] |= 0b10000000;
	led_WriteLed();
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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOffContactLed()
{
	dataled[3] &= 0b01111111;
	led_WriteLed();
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
	IRQ_timer2Enalble();
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
	IRQ_timer2Disable();
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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOnInspectLed()
{
	dataled[3] |= 0b00010000;
	led_WriteLed();
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
/* Using Global Data : (I) dataled  													*/
/*                                                                                      */
/****************************************************************************************/
void led_TurnOffInspectLed()
{
	dataled[3] &= 0b11101111;
	led_WriteLed();
}
