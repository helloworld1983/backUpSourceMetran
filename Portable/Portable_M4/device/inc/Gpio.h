#ifndef DEVICE_INC_GPIO_H_
#define DEVICE_INC_GPIO_H_

/********************************************************************/
/*                                                                  */
/* File Name    : Gpio.h		                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and  */
/* 					configure all gpio port use in M4 core          */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*          Dec 26, 2016      Thao Ha(MV)  			new file        */
/*                                                                  */
/********************************************************************/

#include "board.h"
#include "Battery.h"

#define VAL1_PORT_NUM 				2
#define VAL1_BIT_NUM 				3
#define VAL2_PORT_NUM 				2
#define VAL2_BIT_NUM 				0
#define VAL3_PORT_NUM 				2
#define VAL3_BIT_NUM 				4
#define FC_RESET_PORT_NUM 				3
#define FC_RESET_BIT_NUM 				0
#define COMP_RESET_PORT_NUM 			1
#define COMP_RESET_BIT_NUM 				1
#define COMP_POW_ON_PORT_NUM			1
#define COMP_POW_ON_BIT_NUM				3
#define FAN_EN_PORT_NUM					2
#define FAN_EN_BIT_NUM					9
#define FAN_12V_PORT_NUM				0
#define FAN_12V_BIT_NUM					4
#define FAN_9V_PORT_NUM					2
#define FAN_9V_BIT_NUM					10
#define BAT_ST1_PORT_NUM				3
#define BAT_ST1_BIT_NUM					7
#define BAT_DETECT_PORT_NUM				5
#define BAT_DETECT_BIT_NUM				14
#define USB_DETECT_5V_PORT_NUM			2
#define USB_DETECT_5V_BIT_NUM			11
#define POW_ON_PORT_NUM					2
#define POW_ON_BIT_NUM					12
#define AC_DETECT_PORT_NUM				1
#define AC_DETECT_BIT_NUM				5
#define CAR_DETECT_PORT_NUM				2
#define CAR_DETECT_BIT_NUM				13
#define BAT_ON_PORT_NUM					1
#define BAT_ON_BIT_NUM					9
#define CHARGE_EN_PORT_NUM				5
#define CHARGE_EN_BIT_NUM				13
#define CHARGE_EN_SUB_PORT_NUM				4
#define CHARGE_EN_SUB_BIT_NUM				11

// Init GPIO port
void gpio_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   gpio_Set                                                            */
/*                                                                                      */
/* Details:       Set default value for gpio port                                       */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/* Local variables : None                                                               */
/*                                                                                      */
/****************************************************************************************/
void gpio_Set(void);



#endif /* DEVICE_INC_GPIO_H_ */
