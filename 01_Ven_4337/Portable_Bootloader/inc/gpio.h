
#ifndef INC_GPIO_H_
#define INC_GPIO_H_

/********************************************************************/
/*                                                                  */
/* File Name    : gpio.h                                    		*/
/*                                                                  */
/* General      : This module contains function to initialize GPIO,	*/
/* 				set default valut to GPIO and check current source	*/
/*              of input voltage				                    */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2017/08/06       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#define POW_ON_PORT_NUM					2
#define POW_ON_BIT_NUM					12
#define USB1_PPWR_PORT_NUM				2
#define USB1_PPWR_BIT_NUM				6
#define BAT_ON_PORT_NUM					1
#define BAT_ON_BIT_NUM					9
#define AC_DETECT_PORT_NUM				1
#define AC_DETECT_BIT_NUM				5
#define CAR_DETECT_PORT_NUM				2
#define CAR_DETECT_BIT_NUM				13
#define BAT_DETECT_PORT_NUM				5
#define BAT_DETECT_BIT_NUM				14
#define VBAT_SEN_PORT_NUM				1
#define VBAT_SEN_BIT_NUM				0
#define FAN_EN_PORT_NUM					2
#define FAN_EN_BIT_NUM					9
#define FAN_12V_PORT_NUM				0
#define FAN_12V_BIT_NUM					4
#define FAN_9V_PORT_NUM					2
#define FAN_9V_BIT_NUM					10
typedef enum
{
	eCarCharge,
	eACCharge,
	eNoCharge
} E_ChargingState;
void gpio_Init(void);
void gpio_Set(void);
E_ChargingState gpio_InputVoltageCheck(void);
#endif /* INC_GPIO_H_ */
