#ifndef INC_ENCODER_H_
	#define INC_ENCODER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : Encoder.h		                                    */
/*                                                                  */
/* General       : This module contain function for init and handle */
/* 				   encoder operation
*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             Sep 29, 2016      Quyen Ngo(MV)  	new file        */
/*                                                                  */
/********************************************************************/
#include "TaskCommon.h"

#ifdef DEBUG
//#define Q_DEBUG
#define DEBUG_ENCODER_EN
#endif

#ifdef DEBUG_ENCODER_EN
#define DEBUG_ENCODER(...) printf(__VA_ARGS__)
#else
#define DEBUG_ENCODER(...)
#endif

#define ENCODER_BASE_VALUE 100 /*Off set for user positive value 100++, 100--*/

#define ENCODER_GPIO_INT_INDEX   4	/* PININT index used for GPIO mapping */
#define ENCODER_IRQ_HANDLER  GPIO4_IRQHandler	/* GPIO interrupt IRQ function name */
#define ENCODER_PININT_NVIC_NAME    PIN_INT4_IRQn	/* GPIO interrupt NVIC interrupt name */

extern SemaphoreHandle_t g_encoderBinarySem;

// encode init port
void encoder_Init(void);

// handle encoder step change
void encoder_Handle(void);
#endif /* INC_ENCODER_H_ */
