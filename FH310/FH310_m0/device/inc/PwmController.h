#ifndef INC_PWM_CONTROLLER_H_
	#define INC_PWM_CONTROLLER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PwmController.h                                  */
/*                                                                  */
/* General       : This module contain functions for control SCT PWM
 * 					out put chanel with same frequency of FH-310	*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Jul 31, 2016     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/********************************************************************/

#include "board.h"

#define SCT_PWM            LPC_SCT
#define SCT_PWM_FREQ 600
#define SCT_PWM_COOLINGFAN 2 /*P4.4 CTOUT2 (pin 2) matching with output 2 of timer 0*/
#define SCT_PWM_PIN_COOLINGFAN 2 /*CTOUT2*/
#define SCT_PWM_BACKLIGHT 3 /*P4.1 CTOUT1 (pin 1) matching with output 3 of timer 3*/
#define SCT_PWM_PIN_BACKLIGHT 1 /*CTOUT1*/

/*
 * Start PWM output on cooling fan chanel with config duty cycle
 */
void pwm_Start(void);

/*
 * Cooling_fan set duty cycle for PWM
 */
void pwm_Stop(void);

// Init pwm_port
void pwm_Init(void);

// Set duty cycle of cooling fan, if application want to stop cooling fan
// just set dutycycle = 0
// Param: int duty_cycle: Dutcy cycle in %
void coolingFan_Setduty(uint8_t dutyCycle);

// Set duty cycle of back light, if application want to turn off back light
// just set duty cycle = 0
// Param: uint8_t duty_cycle: Dutcy cycle in %
void backlight_Setduty(uint8_t dutyCycle);

#endif /* INC_PWM_CONTROLLER_H_ */
