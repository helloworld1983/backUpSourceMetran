/*
 * DigitalIO.c
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */
#include <stdbool.h>
#include "fsl_gpio.h"
#include "DigitalIO.h"

#define SOLENOID_1_PORT             GPIO1             //PORT to control send data
#define SOLENOID_1_PIN              (0)               //PIN to control send data

#define SOLENOID_2_PORT             GPIO5            //PORT to control send data
#define SOLENOID_2_PIN              (1)               //PIN to control send data

#define SOLENOID_3_PORT             GPIO4             //PORT to control send data
#define SOLENOID_3_PIN              (9)               //PIN to control send data

#define SOLENOID_4_PORT             GPIO1            //PORT to control send data
#define SOLENOID_4_PIN              (1)               //PIN to control send data

#define SOLENOID_5_PORT            GPIO1            //PORT to control send data
#define SOLENOID_5_PIN              (10)               //PIN to control send data

#define SOLENOID_6_PORT             GPIO1             //PORT to control send data
#define SOLENOID_6_PIN              (0)               //PIN to control send data

/**
 * @brief Array of pin definitions passed to dio_Config() function
 */
typedef struct {
	GPIO_Type* gpioPort;		/* Pin group */
	uint8_t gpioPin;		/* Pin number */
	gpio_pin_config_t stPinconf ;	/* Pin mode and function for SCU */
} DIO_CONF_T;

static DIO_CONF_T s_dioConfArr[] =
{
		{SOLENOID_1_PORT, SOLENOID_1_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_2_PORT, SOLENOID_2_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_3_PORT, SOLENOID_3_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_4_PORT, SOLENOID_4_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_5_PORT, SOLENOID_5_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
		{SOLENOID_6_PORT, SOLENOID_6_PIN, {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode}},
};
void DigitalIO_Init()
{
	for (int i=0; i < (sizeof(s_dioConfArr)/sizeof(DIO_CONF_T)) ; i++)
	{
		GPIO_PinInit(s_dioConfArr[i].gpioPort,s_dioConfArr[i].gpioPin, &(s_dioConfArr[i].stPinconf));
	}
}

void DigitalIO_SetStateSolenoidValve(E_SOLENOIDVALVE valve,E_VALVESTATE state)
{
	switch(valve)
	{
	case eSolenoidValve1:
		if(state == eOnSolenoidValve)
		{
            GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_1_PORT, SOLENOID_1_PIN, 0U);
		}
		break;
	case eSolenoidValve2:
		if(state == eOnSolenoidValve)
		{
			GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_2_PORT, SOLENOID_2_PIN, 0U);
		}
		break;
	case eSolenoidValve3:
		if(state == eOnSolenoidValve)
		{
			GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_3_PORT, SOLENOID_3_PIN, 0U);
		}
		break;
	case eSolenoidValve4:
		if(state == eOnSolenoidValve)
		{
			GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_4_PORT, SOLENOID_4_PIN, 0U);
		}
		break;
	case eSolenoidValve5:
		if(state == eOnSolenoidValve)
		{
			GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_5_PORT, SOLENOID_5_PIN, 0U);
		}
		break;
	case eSolenoidValve6:
		if(state == eOnSolenoidValve)
		{
			GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN, 1U);
		}
		else
		{
			GPIO_PinWrite(SOLENOID_6_PORT, SOLENOID_6_PIN, 0U);
		}
		break;
	default:
		break;
	}
}

