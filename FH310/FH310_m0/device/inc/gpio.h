#ifndef INC_GPIO_H_
	#define INC_GPIO_H_

/********************************************************************/
/*                                                                  */
/* File Name    : Gpio.h		                                    */
/*                                                                  */
/* General       : This module contain function for Initialize and
 * 					configure all gpio port use in M0 core          */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jun 22, 2016      Quyen Ngo(MV)  	new file        */
/*                                                                  */
/********************************************************************/

#include "board.h"

#define OPERATIONSW_PORT_NUM 			0
#define OPERATIONSW_BIT_NUM 			10
#define VAL1_PORT_NUM 					6
#define VAL1_BIT_NUM 					24
#define VAL2_PORT_NUM 					6
#define VAL2_BIT_NUM 					26
#define VAL3_PORT_NUM 					0
#define VAL3_BIT_NUM 					15
#define VAL4_PORT_NUM 					6
#define VAL4_BIT_NUM 					25
#define VAL5_PORT_NUM 					0
#define VAL5_BIT_NUM 					13
#define ENCODER_PB_PORT_NUM 			4
#define ENCODER_PB_BIT_NUM				9
#define ENCODER_PA_PORT_NUM 			4
#define ENCODER_PA_BIT_NUM				10
#define RED_LED_PORT_NUM      			5
#define RED_LED_BIT_NUM      			12
#define YELLOW_LED_PORT_NUM      		6
#define YELLOW_LED_BIT_NUM      		3
#define LED_POWER_ENABLE_PORT_NUM 		6
#define LED_POWER_ENABLE_BIT_NUM 		8
#define POWER24_DETECT_PORT_NUM			1
#define POWER24_DETECT_BIT_NUM			8
#define NIMH_DETECT_LOW_PORT_NUM 		3
#define NIMH_DETECT_LOW_BIT_NUM			13
#define POWER_BACKUP_EN_PORT_NUM 		0
#define POWER_BACKUP_EN_BIT_NUM 		11
#define FC_RESET_PORT_NUM 				4
#define FC_RESET_BIT_NUM 				14
#define COMP_RESET_PORT_NUM 			7
#define COMP_RESET_BIT_NUM 				23
#define LATCH1_PORT_NUM					6
#define LATCH1_BIT_NUM					28
#define LATCH2_PORT_NUM					6
#define LATCH2_BIT_NUM					27
#define FAN_ERR_PORT_NUM				6
#define FAN_ERR_BIT_NUM					9
#define PSA_SELECT_PORT_NUM				6
#define PSA_SELECT_BIT_NUM				15
#define FILTER_PORT						3
#define FILTER_BIT						12
#define FILTER_ERR_PORT					6
#define FILTER_ERR_BIT					29
#define UART_SELECT_CBX_PORT			7
#define UART_SELECT_CBX_BIT				0
#define BATTERY_CHARGE_PORT				4
#define BATTERY_CHARGE_BIT				2
// Init GPIO port
void gpio_Init(void);

#endif /* INC_GPIO_H_ */
