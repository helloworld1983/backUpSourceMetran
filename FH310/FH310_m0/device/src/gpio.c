/********************************************************************/
/*                                                                  */

/* File Name    : Gpio.c		                                    */
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
#include <gpio.h>

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	gpio_Init						             					    */
/*                                                                                      */
/* Details:         Init gpio port									    				*/
/*                                                                                      */
/*                                                                                      */
/* ReturnValue :    void                                     		                    */
/*                                                                                      */
/* Using Global Data : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void gpio_Init(void) {
	Chip_GPIO_Init(LPC_GPIO_PORT);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,UART_SELECT_CBX_PORT, UART_SELECT_CBX_BIT);		/* output */
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,UART_SELECT_CBX_PORT, UART_SELECT_CBX_BIT);
	return;
}

